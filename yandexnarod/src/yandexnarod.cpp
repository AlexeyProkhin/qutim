/****************************************************************************
 *  yandexnarod.cpp
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

#include "yandexnarod.h"
#include "requestauthdialog.h"
#include "yandexnarodauthorizator.h"
#include <qutim/actiongenerator.h>
#include <qutim/contact.h>
#include <qutim/message.h>
#include <qutim/account.h>
#include <qutim/settingslayer.h>
#include <qutim/contactlist.h>
#include <qutim/configbase.h>
#include <qutim/debug.h>

void YandexNarodPlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Yandex Narod"),
			QT_TRANSLATE_NOOP("Plugin", "Send files via Yandex.Narod filehosting service"),
			PLUGIN_VERSION(0, 2, 0, 0));
}

bool YandexNarodPlugin::load()
{
	MenuController::addAction<Contact>(
			new ActionGenerator(QIcon(),
								QT_TRANSLATE_NOOP("Yandex", "Send file via Yandex.Narod"),
								this, SLOT(onActionClicked())));
//	ActionGenerator *gen = new ActionGenerator(QIcon(),
//											   QT_TRANSLATE_NOOP("Yandex", "Manage Yandex.Narod files"),
//											   this, SLOT(onManageClicked()));
//	ContactList *contactList = ContactList::instance();
//	contactList->metaObject()->invokeMethod(contactList,
//											"addButton", Q_ARG(ActionGenerator*, gen));
	SettingsItem *settings = new GeneralSettingsItem<YandexNarodSettings>(
			Settings::Plugin,
			QIcon(),
			QT_TRANSLATE_NOOP("Yandex", "Yandex Narod"));
	settings->connect(SIGNAL(testclick()), this,  SLOT(on_btnTest_clicked()));
	Settings::registerItem(settings);
	m_netMan = new YandexNarodNetMan(this);
	return true;
}

bool YandexNarodPlugin::unload()
{
	return false;
}

void YandexNarodPlugin::onActionClicked()
{
	QAction *action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	Contact *contact = qobject_cast<Contact *>(action->data().value<MenuController *>());
	Q_ASSERT(contact);

	ConfigGroup group = Config().group("yandexnarod");

	m_uploadWidget = new YandexNarodUploadDialog();
	connect(m_uploadWidget, SIGNAL(canceled()), this, SLOT(removeUploadWidget()));

	m_uploadWidget->show();

	QString filepath = QFileDialog::getOpenFileName(
			m_uploadWidget,
			tr("Choose file for %1").arg(contact->name()),
			group.value("lastdir", QString()));

	if (filepath.length()>0) {
		fi.setFile(filepath);
		group.setValue("lastdir", fi.dir().path());
		group.sync();

		m_netMan = new YandexNarodNetMan(m_uploadWidget);
		m_uploadWidget->setContact(contact);
		connect(m_netMan, SIGNAL(statusText(QString)), m_uploadWidget, SLOT(setStatus(QString)));
		connect(m_netMan, SIGNAL(statusFileName(QString)), m_uploadWidget, SLOT(setFilename(QString)));
		connect(m_netMan, SIGNAL(transferProgress(qint64,qint64)), m_uploadWidget, SLOT(progress(qint64,qint64)));
		connect(m_netMan, SIGNAL(uploadFileURL(QString)), this, SLOT(onFileURL(QString)));
		connect(m_netMan, SIGNAL(uploadFinished()), m_uploadWidget, SLOT(setDone()));
		m_netMan->startUploadFile(filepath);
	}
	else {
		delete m_uploadWidget; m_uploadWidget=0;
	}

	authtest=false;
}

void YandexNarodPlugin::onManageClicked()
{
	if (m_manageDialog.isNull()) {
		m_manageDialog = new YandexNarodManager();
		m_manageDialog->show();
	}
}

void YandexNarodPlugin::on_btnTest_clicked()
{
	YandexNarodSettings *settingsWidget = qobject_cast<YandexNarodSettings *>(sender());
//	testnetman = new YandexNarodNetMan(settingsWidget);
//	connect(testnetman, SIGNAL(statusText(QString)), settingsWidget, SLOT(setStatus(QString)));
//	connect(testnetman, SIGNAL(finished()), this , SLOT(on_TestFinished()));
//	debug() << settingsWidget->getLogin() << settingsWidget->getPasswd();
//	testnetman->startAuthTest(settingsWidget->getLogin(), settingsWidget->getPasswd());
	YandexNarodAuthorizator *auth = new YandexNarodAuthorizator(settingsWidget);
	auth->requestAuthorization(settingsWidget->getLogin(), settingsWidget->getPasswd());
}

void YandexNarodPlugin::on_TestFinished()
{
	delete testnetman;
}

void YandexNarodPlugin::actionStart()
{
}

void YandexNarodPlugin::onFileURL(const QString &url)
{
//	YandexNarodNetMan *netMan = qobject_cast<YandexNarodNetMan *>(sender());
//	Q_ASSERT(netMan);
//
//	if (Contact *contact = netMan->contact()) {
//		QString sendmsg = Config().group("yandexnarod").value("template", QString("File sent: %N (%S bytes)\n%U"));
//		sendmsg.replace("%N", fi.fileName());
//		sendmsg.replace("%U", url);
//		sendmsg.replace("%S", QString::number(fi.size()));
//		m_uploadWidget->setStatus(tr("File sent"));
//		m_uploadWidget->close();
//		Message message;
//		message.setText(sendmsg);
//		contact->account()->getUnitForSession(contact)->sendMessage(message);
//	}
}

QUTIM_EXPORT_PLUGIN(YandexNarodPlugin)
