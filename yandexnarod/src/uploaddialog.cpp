/****************************************************************************
 *  uploaddialog.cpp
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <qutim/configbase.h>
#include <qutim/json.h>
#include <qutim/debug.h>
#include <QFileDialog>
#include <QNetworkCookieJar>
#include "uploaddialog.h"

using namespace qutim_sdk_0_3;

YandexNarodBuffer::YandexNarodBuffer(const QString &fileName, const QByteArray &boundary, QObject *parent) :
		QIODevice(parent)
{
	QFileInfo info(fileName);
	QByteArray data;
	data.append("--").append(boundary).append("\r\n");
	data.append("Content-Disposition: form-data; name=\"file\"; filename=\"")
			.append(info.fileName().toUtf8()).append("\"\r\n");
	data.append("Content-Transfer-Encoding: binary\r\n");
	data.append("\r\n");

	QBuffer *buffer = new QBuffer(this);
	buffer->setData(data);
	m_devices.append(buffer);

	m_devices.append(new QFile(fileName, this));

	data.clear();
	data.append("\r\n--").append(boundary).append("--\r\n");

	buffer = new QBuffer(this);
	buffer->setData(data);
	m_devices.append(buffer);
}

YandexNarodBuffer::~YandexNarodBuffer()
{
}

bool YandexNarodBuffer::open(OpenMode mode)
{
	bool result = true;
	foreach (QIODevice *device, m_devices)
		result &= device->open(mode);
	QIODevice::open(mode);
	return result;
}

qint64 YandexNarodBuffer::size() const
{
	quint64 totalSize = 0;
	foreach (QIODevice *device, m_devices)
		totalSize += device->size();
	return totalSize;
}

qint64 YandexNarodBuffer::readData(char *data, qint64 maxlen)
{
	quint64 totalSize = 0;
	foreach (QIODevice *device, m_devices) {
		if (maxlen == 0 || device->atEnd())
			continue;
		quint64 currentSize = device->read(data, maxlen);
		totalSize += currentSize;
		maxlen -= currentSize;
		data += currentSize;
	}
	return totalSize;
}

qint64 YandexNarodBuffer::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data);
	Q_UNUSED(len);
	return -1;
}

YandexNarodUploadDialog::YandexNarodUploadDialog(QNetworkAccessManager *networkManager,
												 YandexNarodAuthorizator *authorizator,
												 qutim_sdk_0_3::Contact *contact) :
m_networkManager(networkManager), m_authorizator(authorizator)
{
	ui.setupUi(this);
	connect(ui.btnUploadCancel, SIGNAL(clicked()), this, SIGNAL(canceled()));
	connect(ui.btnUploadCancel, SIGNAL(clicked()), this, SLOT(close()));
	qutim_sdk_0_3::centerizeWidget(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);

	m_request.setRawHeader("Cache-Control", "no-cache");
	m_request.setRawHeader("Accept", "*/*");
	QByteArray userAgent = "qutIM/";
	userAgent += qutimVersionStr();
	userAgent += " (U; YB/4.2.0; MRA/5.5; en)";
	m_request.setRawHeader("User-Agent", userAgent);

	ConfigGroup group = Config().group("yandexnarod");

	m_filePath = QFileDialog::getOpenFileName(
			this,
			contact ? tr("Choose file for %1").arg(contact->title()) : tr("Choose file for uploading"),
			group.value("lastdir", QString()));

	if (m_filePath.isEmpty()) {
		deleteLater();
	} else {
		m_timer.setInterval(5000);
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(someStrangeSlot()));

		QFileInfo info(m_filePath);
		group.setValue("lastdir", info.dir().path());
		if (!m_authorizator->isAuthorized()) {
			ui.labelStatus->setText(tr("Authorizing..."));
			connect(m_authorizator, SIGNAL(result(YandexNarodAuthorizator::Result,QString)),
					this, SLOT(authorizationResult(YandexNarodAuthorizator::Result,QString)));
			m_authorizator->requestAuthorization();
		} else
			start();

		show();
	}
}

YandexNarodUploadDialog::~YandexNarodUploadDialog()
{
}

bool YandexNarodUploadDialog::processReply(QNetworkReply *reply)
{
	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError)
		return true;

	debug() << reply->request().url() << QString::fromUtf8(reply->readAll());
	ui.labelStatus->setText(reply->errorString());
	return false;
}

void YandexNarodUploadDialog::authorizationResult(YandexNarodAuthorizator::Result result, const QString &error)
{
	if (result == YandexNarodAuthorizator::Success)
		start();
	else
		ui.labelStatus->setText(m_authorizator->resultString(result, error));
}

void YandexNarodUploadDialog::start()
{
	ui.labelStatus->setText(tr("Getting storage..."));

	m_request.setUrl(QUrl("http://narod.yandex.ru/disk/getstorage/?type=json"));

	foreach(QNetworkCookie cookie, m_networkManager->cookieJar()->cookiesForUrl(m_request.url())) {
		m_request.setRawHeader("Cookie", cookie.toRawForm(QNetworkCookie::NameAndValueOnly));
		debug() << cookie;
	}

	QNetworkReply *reply = m_networkManager->get(m_request);

	connect(reply, SIGNAL(finished()), this, SLOT(storageReply()));

	ui.progressBar->setValue(0);
}

void YandexNarodUploadDialog::storageReply()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	if (!processReply(reply))
		return;

	ui.labelStatus->setText(tr("Uploading..."));

	QByteArray data = reply->readAll();
	QVariantMap map = m_someData = Json::parse(data).toMap();

	debug() << "storage" << map;

//	отправляем POST: на параметр %url%?tid=%hash%
//	(например, http://07-up1q-narod.yandex.ru/upload/?tid=1263311543ksIa9VwGOTIYGLFGVGXRgPXe)
//	проверяем статус закачки: %purl%?tid=%hash%
//	(например, http://07-up1q-narod.yandex.ru/progress/?tid=1263311543ksIa9VwGOTIYGLFGVGXRgPXe)

	QUrl url(map.value("url").toString());
	url.addQueryItem("tid", map.value("hash").toString());
	m_request.setUrl(url);

	quint32 boundaryTemp[] = { qrand(), qrand() };

	QByteArray boundary = QByteArray(reinterpret_cast<char *>(boundaryTemp),
									 sizeof(boundaryTemp)).toHex();

	QIODevice *device = new YandexNarodBuffer(m_filePath, boundary, this);


	device->open(QIODevice::ReadOnly);

	QNetworkRequest request(m_request);
	request.setRawHeader("Content-Type", "multipart/form-data, boundary=" + boundary);
	request.setRawHeader("Content-Length", QString::number(device->size()).toLatin1());

	QNetworkReply *uploadNetworkReply = m_networkManager->post(request, device);

	connect(uploadNetworkReply, SIGNAL(finished()), this, SLOT(uploadReply()));

	m_timer.start();
}

void YandexNarodUploadDialog::someStrangeSlot()
{
	QUrl url(m_someData.value("purl").toString());
	url.addQueryItem("tid", m_someData.value("hash").toString());
	url.addQueryItem("type", "json");
	m_request.setUrl(url);
	connect(m_networkManager->get(m_request), SIGNAL(finished()),
			this, SLOT(progressReply()));
}

void YandexNarodUploadDialog::uploadReply()
{
	m_timer.stop();
	debug() << "uploadReply";
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	if (!processReply(reply))
		return;


	QByteArray data = reply->readAll();
	QVariantMap map = Json::parse(data).toMap();


	debug() << "upload" << data << map;
	debug() << reply->rawHeaderList();

	someStrangeSlot();
}

void YandexNarodUploadDialog::progressReply()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	if (!processReply(reply))
		return;

	QByteArray data = reply->readAll();
	QVariantMap map = Json::parse(data).toMap();

	debug() << "progress" << reply->request().url() << data << map;
	QVariantMap varMap = map.value("files").toList().value(0).toMap();
//	Q_ASSERT(varMap.isEmpty())
	if (!varMap.isEmpty()) {
		QString url(QLatin1String("http://narod.ru/disk/"));
		url += varMap.value("hash").toString();
		url += QLatin1Char('/');
		url += varMap.value("name").toString();
		url += QLatin1String(".html");
		debug() << url;
	}
}

void YandexNarodUploadDialog::progress(qint64 cBytes, qint64 totalBytes) {
	ui.labelStatus->setText("Uploading...");
	ui.labelProgress->setText("Progress: "+QString::number(cBytes)+" / "+QString::number(totalBytes));
	ui.progressBar->setMaximum(totalBytes);
	ui.progressBar->setValue(cBytes);

	setWindowTitle("[" + ui.progressBar->text() + "] - Uploading...");
	
	QTime etime(0,0,0);
	ui.labelETime->setText("Elapsed time: " + etime.toString("hh:mm:ss") );
	
//	float speed_kbsec = (cBytes / (utime.elapsed()/1000))/1024;
//	if (speed_kbsec>0) ui.labelSpeed->setText("Speed: "+QString::number(speed_kbsec)+" kb/sec");
	
	if (cBytes == totalBytes) ui.labelStatus->setText("Upload complete.");
}
