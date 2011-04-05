/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "packageentry.h"
#include <QStringList>

class PackageEntryData : public QSharedData
{
public:
	PackageEntryData() {}
	
	Attica::Content content;
	QStringList installedFiles;
	QString installedVersion;
};


PackageEntry::PackageEntry() : d(new PackageEntryData)
{
}

PackageEntry::PackageEntry(const PackageEntry &o) : d(o.d)
{
}

PackageEntry &PackageEntry::operator =(const PackageEntry &o)
{
	d = o.d;
	return *this;
}

PackageEntry::~PackageEntry()
{
}

bool PackageEntry::isValid() const
{
	return d->content.isValid();
}

QString PackageEntry::id() const
{
	return d->content.id();
}

Attica::Content PackageEntry::content() const
{
	return d->content;
}
void PackageEntry::setContent(const Attica::Content &content)
{
	d->content = content;
}

QStringList PackageEntry::installedFiles() const
{
	return d->installedFiles;
}

void PackageEntry::setInstalledFiles(const QStringList &files)
{
	d->installedFiles = files;
}

QString PackageEntry::installedVersion() const
{
	return d->installedVersion;
}

void PackageEntry::setInstalledVersion(const QString &version)
{
	d->installedVersion = version;
}
