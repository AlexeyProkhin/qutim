/*
	SqlHistory

	Copyright (c) 2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "sqlhistory.h"
#include <qutim/iconmanagerinterface.h>

SqlHistory::SqlHistory(QObject *parent) :
	QObject(parent)
{
	m_engine = 0;
	m_icon = 0;
}

bool SqlHistory::init(PluginSystemInterface *plugin_system)
{
	PluginInterface::init(plugin_system);
	m_engine = new SqlHistoryNamespace::SqlEngine;
	if(!plugin_system->setLayerInterface(HistoryLayer, m_engine))
	{
		delete m_engine;
		m_engine = 0;
		return false;
	}
	plugin_system->registerEventHandler("Core/ContactList/ItemAdded", this, SLOT(itemAdded(TreeModelItem,QString)));
	return true;
}

void SqlHistory::release()
{
	delete m_engine;
	m_engine = 0;
}

QString SqlHistory::name()
{
	return "SqlHistory";
}

QString SqlHistory::description()
{
	return "SQL History engine";
}

QIcon *SqlHistory::icon()
{
	return m_icon;
}

void SqlHistory::setProfileName(const QString &profile_name)
{
	SystemsCity::instance().setProfileName(profile_name);
	m_icon = new Icon("history");
}

void SqlHistory::itemAdded(const TreeModelItem &item, const QString &)
{
	if(m_engine)
		m_engine->setHistory(item);
}

Q_EXPORT_PLUGIN2(sqlhistory, SqlHistory)
