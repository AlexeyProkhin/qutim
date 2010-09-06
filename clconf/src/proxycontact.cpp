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
#include <qutim/tooltip.h>
#include <qutim/event.h>
#include <QApplication>

ProxyContact::ProxyContact(Conference *conf) :
	Contact(ClConfPlugin::instance()->account(conf->account())), m_conf(conf), m_conn(false)
{
	connect(conf, SIGNAL(destroyed()), SLOT(deleteLater()));
	setMenuOwner(m_conf);
	setMenuFlags(ShowOwnerActions);
	m_conf->installEventFilter(this);
	connect(m_conf, SIGNAL(titleChanged(QString,QString)), SIGNAL(nameChanged(QString,QString)));
	connect(m_conf, SIGNAL(titleChanged(QString,QString)), SIGNAL(titleChanged(QString,QString)));
	connect(m_conf, SIGNAL(joined()), SLOT(onJoined()));
	connect(m_conf, SIGNAL(left()), SLOT(onLeft()));
	connect(m_conf->account(), SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
	updateStatus();
	m_realUnitRequestEvent = Event::registerType("real-chatunit-request");
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
	return m_conf ? m_conf->id() : QString();
}

QString ProxyContact::title() const
{
	return m_conf ? m_conf->title() : QString();
}

QString ProxyContact::name() const
{
	return m_conf ? m_conf->title() : QString();
}

Status ProxyContact::status() const
{
	return m_conf ? m_status : Status();
}

bool ProxyContact::sendMessage(const Message &message)
{
	return m_conf ? m_conf->sendMessage(message) : false;
}

void ProxyContact::onJoined()
{
	if (!m_conn) {
		m_conn = true;
		updateStatus();
	}
}

void ProxyContact::onLeft()
{
	if (m_conn) {
		m_conn = false;
		updateStatus();
	}
}

void ProxyContact::onAccountStatusChanged(const qutim_sdk_0_3::Status &current,
										  const qutim_sdk_0_3::Status &previous)
{
	if (previous.type() != Status::Offline && current.type() == Status::Offline)
		updateStatus();
	else if (previous.type() == Status::Offline && current.type() != Status::Offline)
		updateStatus();
}

void ProxyContact::updateStatus()
{
	Status previous = m_status;
	if (account()->status().type() == Status::Offline)
		m_status.setType(Status::Offline);
	else
		m_status.setType(m_conn ? Status::Online : Status::DND);
	m_status.initIcon(QLatin1String("conference-") + metaInfo(metaObject(), "Protocol"));
	emit statusChanged(m_status, previous);
}

bool ProxyContact::event(QEvent *ev)
{
	if (m_conf && ev->type() == ToolTipEvent::eventType())
		return qApp->sendEvent(m_conf, ev);
	return Contact::event(ev);
}

bool ProxyContact::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == m_conf && ev->type() == Event::eventType()) {
		Event *event = static_cast<Event*>(ev);
		if (event->id == m_realUnitRequestEvent) {
			event->args[0] = qVariantFromValue<Contact*>(this);
			event->accept();
			return true;
		}
	}
	return Contact::eventFilter(obj, ev);
}
