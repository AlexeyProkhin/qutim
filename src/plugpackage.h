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

#ifndef PLUGPACKAGE_H
#define PLUGPACKAGE_H
#include <QString>
#include <QHash>
#include <QStringList>

struct packageInfo {
	packageInfo ();
	packageInfo  (QString name,
				   QStringList packageFiles,
				   QString type = "other",
				   QString description = "Unknown package",
				   QString author = "Unknown author",
				   QString licence = "Unknown licence"
				   //недописанный конструктор
				   );
	//предварительный вариант необходимой инфы о пакете
	QHash<QString,QString> properties;
	QStringList files; //установленные файлы
	quint32 id; //! package id in packages.xml
	QString toString(); //! x3	
};

#endif // PLUGPACKAGE_H
