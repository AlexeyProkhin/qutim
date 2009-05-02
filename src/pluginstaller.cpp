/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#include "pluginstaller.h"
#include <QDebug>
#include <QSettings>
#include <osdabzip/unzip.h>
#include "plugdownloader.h"
#include "collisionprotect.h"
#include <QDir>
#include <QFileDialog>

plugInstaller::plugInstaller()
: package_info()
{
	
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "plugman");
	settings.beginGroup("features");
	collision_protect = settings.value("collisionprotect",true).toBool();
	settings.endGroup();
    outPath = settings.fileName().section("/",0,-2) + "/";
// 	connect (this,SIGNAL(finished()),this,SLOT(deleteLater())); // в случае завершения установки обьект может быть удалён
	connect (this,SIGNAL(error(QString)),this,SLOT(errorHandler(QString))); //в случае ошибки вызывается этот класс
}

plugInstaller::~plugInstaller() {
	
}

QStringList plugInstaller::unpackArch(const QString& inPath) {
//
	//FIXME need SANDBOX!
    UnZip uz;
    UnZip::ErrorCode ec = uz.openArchive(inPath);
    if (ec != UnZip::Ok) {
        lastError = tr("Unable to open archive");
        return QStringList();
    }
    QStringList packFiles = uz.fileList();
	if (collision_protect) {
		CollisionProtect protect(outPath);
		if (!protect.checkPackageFiles(packFiles))
			return QStringList();
		}
	
    uz.extractAll(outPath);
    if (ec != UnZip::Ok) {
        lastError = tr ("Unable to extract archive");
        return QStringList();
    }
    uz.closeArchive(); // Close the zip file and free used resources
//     qDebug() << "Unpack archive:" << outPath;
	//FIXME Костыль
	//TODO дописать в osdabzip возможность удалять файлы прямо из архива.
	packFiles.removeOne("Pinfo.xml");
	QFile::remove(outPath+"/Pinfo.xml");
	m_progressBar->setValue(75);
    return packFiles;
}

void plugInstaller::installPackage() {
	QString path = QFileDialog::getOpenFileName(0,tr("Install package from file"),".",
												tr("Archives (*.zip);;XML files (*.xml)"));
	if (path.isEmpty())
		return;
	m_progressBar->setVisible(true);
	m_progressBar->setValue(0);
	if ((path.section(".",-1))=="zip") {
	    this->installFromFile(path);
	}
	else if ((path.section(".",-1))=="xml")
	{
	    this->installFromXML(path);
	}
}


void plugInstaller::installFromFile(const QString& inPath) {
	QString name = inPath.section("/",-1).section(".",0,-2);
	if (!hasPackageInfo(inPath)) {
		return;
	}
	if (collision_protect) {
		CollisionProtect protect(outPath);
		if (!protect.checkPackageName(package_info.properties["name"])) {
			emit error("Exist name");
			return;
		}
	}	
	install(inPath);
}

bool plugInstaller::hasPackageInfo(const QString& archPath) {
	UnZip uz;
	UnZip::ErrorCode ec = uz.openArchive(archPath);
	if (ec != UnZip::Ok) {
		lastError = tr("Unable to open archive");
		return false;
	}
	QStringList packFiles = uz.fileList();
	if (!packFiles.contains("Pinfo.xml"))
		return false;
	QString tmp_path = outPath + "/plugman/cache/"; //FIXME need SANDBOX!
	uz.extractFile("Pinfo.xml",tmp_path);
	if (ec != UnZip::Ok) {
		lastError = tr ("Unable to extract archive");
		return false;
	}
	plugXMLHandler handler;
	tmp_path.append("Pinfo.xml");
	package_info = handler.getPackageInfo(tmp_path);
 	QFile::remove(tmp_path);
	uz.closeArchive();
	return true;
}


void plugInstaller::installFromXML(const QString& inPath) {
	plugXMLHandler plug_handler;
    plugDownloader *plug_loader = new plugDownloader ();
	plug_loader->setProgressbar(m_progressBar);
	package_info = plug_handler.getPackageInfo(inPath);
	if (!package_info.isValid()) {
		emit error("Invalid package");
		return;
	}
	if (collision_protect) {
		CollisionProtect protect(outPath);
		if (!protect.checkPackageName(package_info.properties["name"])) {
			emit error("Exist name");
			return;
		}
	if (!protect.checkPackageFiles(package_info.files))
			return;
	}	
    connect(plug_loader,SIGNAL(downloadFinished(QStringList)),this,SLOT(install(QStringList)));
    plug_loader->addItem(downloaderItem(package_info.properties["url"],
															  package_info.properties["name"]
															  +"-"+package_info.properties["version"]
															  +".zip")
															 ); //FIXME
	plug_loader->startDownload();
}

void plugInstaller::install(QString inPath) {
	//FIXME
	m_progressBar->setValue(50);
	package_info.files = unpackArch(inPath);
	if (package_info.files.isEmpty()) {
		emit error(tr("Unable to install package"));
		return;
	}
	plugXMLHandler plug_handler;
	m_progressBar->setValue(100);
	plug_handler.registerPackage(package_info);
	m_progressBar->setVisible(false);
	deleteLater();
}

void plugInstaller::install(QStringList fileList)
{
	foreach (QString filePath,fileList) {
		install(filePath);
	}
}


void plugInstaller::removePackage(const int& id) {
	plugXMLHandler plug_handler;
	QStringList fileList = plug_handler.removePackage(id);
	ushort i = 0;
	foreach (QString filePath,fileList) {
		QFile output (filePath);
		output.remove();
		m_progressBar->setValue(qRound(i/fileList.count()*100));
	}
}

void plugInstaller::errorHandler(const QString& error) {
	qDebug() << error;
	deleteLater();
}
