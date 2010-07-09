/****************************************************************************
 *  proxycontact.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "proxycontact.h"
#include "proxyaccount.h"
#include "clconfplugin.h"

ProxyContact::ProxyContact(Conference *conf) :
	Contact(ClConfPlugin::instance()->account(conf->account())), m_conf(conf)
{
	connect(conf, SIGNAL(destroyed()), SLOT(deleteLater()));
	setMenuOwner(m_conf);
	connect(m_conf, SIGNAL(nameChanged(QString)), SIGNAL(nameChanged(QString)));
	connect(m_conf, SIGNAL(titleChanged(QString)), SIGNAL(titleChanged(QString)));
}

QStringList ProxyContact::tags() const
{
	return QStringList() << QT_TRANSLATE_NOOP("ClConf", "Conferences");
}

void ProxyContact::setTags(const QStringList &tags)
{
	Q_UNUSED(tags);
}

bool ProxyContact::isInList() const
{
	return true;
}

void ProxyContact::setInList(bool inList)
{
	Q_UNUSED(inList);
}

QString ProxyContact::id() const
{
	return m_conf->id();
}

QString ProxyContact::title() const
{
	return m_conf->title();
}

QString ProxyContact::name() const
{
	return m_conf->id();
}

Status ProxyContact::status() const
{
	return Status(Status::Online);
}

bool ProxyContact::sendMessage(const Message &message)
{
	return m_conf->sendMessage(message);
}
