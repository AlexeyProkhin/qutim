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

#include "plugxmlhandler.h"
#include <QDebug>
#include <QSettings>


plugXMLHandler::packageInfo::packageInfo(QString name, QStringList packageFiles, QString type, QString description, QString author, QString licence) {
	properties["name"] = name;
	files = packageFiles;
	properties["type"] = type;	
	properties["description"] = description;
	properties["author"] = author;	
}

QString plugXMLHandler::packageInfo::toString() {
	return QString();
}


plugXMLHandler::packageInfo::packageInfo() {

}

plugXMLHandler::plugXMLHandler() {
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
    package_db_path = settings.fileName().section("/",0,-2)+"/packages.xml";
	settings.beginGroup("global");
	globalCount = settings.value("count","0").toInt();
	settings.endGroup();
// 	qDebug() << getPackageInfo(1).toString();
}

plugXMLHandler::~plugXMLHandler() {

}

QDomElement plugXMLHandler::createElementFromPackage(packageInfo package_info, int id) {
	QDomDocument doc;
	QDomElement package = doc.createElement("package"); 
	package.setAttribute("id",QString::number(id)); //надеюсь NULL она просто не добавит

	QDomElement package_element;
	QHash<QString,QString>::iterator it = package_info.properties.begin();
	for (it = package_info.properties.begin(); it!=package_info.properties.end();it++) {
 		package_element = doc.createElement(it.key());
  		package_element.appendChild(doc.createTextNode(it.value()));
 		package.appendChild(package_element);		
	}

	package_element = doc.createElement("files");
	package.appendChild(package_element);
	foreach (QString name, package_info.files) {
		QDomElement file_name = doc.createElement("name");
		file_name.appendChild(doc.createTextNode(name));
		package_element.appendChild(file_name);		
	}
	return package;
}

plugXMLHandler::packageInfo plugXMLHandler::createPackageInfoFromDom(const QDomDocument& doc, QString id) {
// 	if (!QString::isEmpty(id))
// 		QDomNode n = doc.elementById();
// 	else
// 		doc.firstChild();
	return packageInfo();
}

plugXMLHandler::packageInfo plugXMLHandler::createPackageInfoFromNode(QDomNode n) {
	
// 	QDomNode n = doc.documentElement().firstChild();
	qDebug () << "start parser";
	packageInfo package_info;
	while (!n.isNull()) {
	    QDomElement e = n.toElement(); // try to convert the node to an element.
	    if (!e.isNull()) {
			if (e.tagName() == "files") {
 				package_info.files = createFilesList(e.firstChild());
			}
			else {
				package_info.properties[e.tagName()] =e.text();
				qDebug() << e.tagName() << " : " << e.text();
			}
	    }
	    n = n.nextSibling();
	}
	return package_info;
}

QStringList plugXMLHandler::createFilesList(QDomNode n) {
	QStringList files;
	while (!n.isNull()) {
		QDomElement e = n.toElement();
 		qDebug() << e.tagName() << " : " << e.text();
		if (!e.isNull())
			files.append(e.text());
		n = n.nextSibling();
	}
	return files;
}

QHash< int, plugXMLHandler::packageInfo > plugXMLHandler::createPackageList(const QDomDocument& root) {

	QDomElement packages = root.documentElement();
	QHash< int, plugXMLHandler::packageInfo > packages_list;
	QDomNode n = packages.firstChild();
	while (!n.isNull()) {
	    QDomElement e = n.toElement(); // try to convert the node to an element.
	    if (!e.isNull()) {
			qDebug() << e.tagName() << " : " << e.attribute("id");
			packages_list[e.attribute("id").toInt()] = createPackageInfoFromNode(n.firstChild());
	    }
	    n = n.nextSibling();
	}
	return packages_list;
}

bool plugXMLHandler::registerPackage(const packageInfo &package_info) {
	QDomDocument doc;
	QFile input(package_db_path);
	if (input.exists()) {
		if (!input.open(QIODevice::ReadOnly)) {
			qDebug() << "Unable to open file";
			return false;
		}
		if (!doc.setContent(&input)) {
			qDebug() << "Unable to open read";
			return false;
		}
		input.close();
	}
 	if (doc.documentElement().isNull()) {
		QDomElement packages = doc.createElement("packages");
		packages.setAttribute("version","0.1pre");
 		doc.appendChild(packages);

	}
	doc.documentElement().appendChild(createElementFromPackage(package_info, globalCount));
	QFile output(QString(package_db_path+".lock"));
	if (!output.open(QIODevice::WriteOnly)) {
		qDebug() << "Unable to write file";
		return false;
	}
	QTextStream out(&output);
	doc.save(out,2,QDomNode::EncodingFromTextStream);
	output.close();
	input.remove();
	output.rename(package_db_path);
	globalCount++;
	updateGlobalCount();	
	qDebug() << doc.toString();
	return true;
}

QStringList plugXMLHandler::removePackage(int package_id) {
	//FIXME возможно стоить выделить в отдельную функцию
	QDomDocument doc_root;
	QFile input(package_db_path);
	QStringList files_list;
	if (!input.open(QIODevice::ReadWrite)) {
		//x3
	}
	if (!doc_root.setContent(&input)) {
		// x3
	}
	QDomElement packages = doc_root.documentElement();
	QDomNode n = packages.firstChild();
	while (!n.isNull()) {
	    QDomElement e = n.toElement(); // try to convert the node to an element.
		qDebug() << e.attribute("id");
		if (e.attribute("id").toInt()==package_id) {
			//FIXME переделать красиво
			qDebug() << "found package by id" << package_id;
			QDomNode p = n.firstChild();
			while (!p.isNull()){
				e = p.toElement();
				if (e.tagName()=="files")
					files_list = createFilesList(e.firstChild());
				p = p.nextSibling();
			}
			doc_root.removeChild(n);
		}
	    n = n.nextSibling();
	}
	qDebug() << doc_root.toString();
	return files_list;
}


plugXMLHandler::packageInfo plugXMLHandler::getPackageInfo(const QString& filename) {
	QDomDocument doc;
	
	QFile input(filename);
	if (!input.open(QIODevice::ReadOnly)) {
		//x3
	}
	if (!doc.setContent(&input)) {
		// x3
	}
	input.close();
	return createPackageInfoFromNode(doc.documentElement().firstChild());
}

QHash< int, plugXMLHandler::packageInfo > plugXMLHandler::getPackageList() {
	QDomDocument doc_root;
	QFile input(package_db_path);
	if (!input.open(QIODevice::ReadOnly)) {
		//x3
	}
	if (!doc_root.setContent(&input)) {
		// x3
	}
	input.close();
	return createPackageList(doc_root);
}

plugXMLHandler::packageInfo plugXMLHandler::getPackageInfo(const QUrl& url) {
	return packageInfo();
}

plugXMLHandler::packageInfo plugXMLHandler::getPackageInfo(const int id) {
	QDomDocument doc;
	
	QFile input(package_db_path);
	if (!input.open(QIODevice::ReadOnly)) {
		//x3
	}
	if (!doc.setContent(&input)) {
		// x3
	}
	input.close();
	
	//return createPackageInfoFromNode(doc.elementById(QString::number(id)));
}


bool plugXMLHandler::isValid(QDomDocument doc) {
	return true;
}

bool plugXMLHandler::updateGlobalCount() {
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman");
	settings.beginGroup("global");
	settings.setValue("count",globalCount);
	settings.endGroup();
}

bool plugXMLHandler::rebuildGlobalCount() {
	return true;
}




