/****************************************************************************
 *  jmucmanager.cpp
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <QClipboard>
#include <QMessageBox>
#include <QDebug>
#include "../jaccount.h"
#include "jmucmanager.h"
#include "jbookmarkmanager.h"
#include "jinvitemanager.h"
#include "jmucsession.h"
#include <qutim/messagesession.h>
#include <qutim/icon.h>
#include <qutim/status.h>
#include "../roster/jsoftwaredetection.h"
//Jreen
#include <jreen/client.h>
#include <jreen/privacymanager.h>

namespace Jabber
{

using namespace qutim_sdk_0_3;
using namespace Jreen;

class JMUCManagerPrivate
{
	Q_DECLARE_PUBLIC(JMUCManager)
public:
	JMUCManagerPrivate(JMUCManager *q) : q_ptr(q) {}
	JMUCManager *q_ptr;
	JAccount *account;
	JBookmarkManager *bookmarkManager;
	//JInviteManager *inviteManager;
	QHash<QString, JMUCSession *> rooms;
	QList<JMUCSession *> roomsToConnect;
	void connectAll()
	{
		foreach (JMUCSession *session, rooms) {
			Jreen::MUCRoom *room = session->room();
			if (!room->isJoined() && room->presence() != Presence::Unavailable
					&& !roomsToConnect.contains(session)) {
				session->join();
			}
		}
		for (int i = 0; i < roomsToConnect.size(); i++)
			roomsToConnect.at(i)->join();
		roomsToConnect.clear();
	}
	void leaveAll()
	{
		foreach (JMUCSession *room, rooms) {
			room->leave();
		}
	}
	void _q_status_changed(qutim_sdk_0_3::Status status)
	{		
		if (status == Status::Offline)
			leaveAll();
		else if (status != Status::Connecting)
			connectAll();
	}
};

JMUCManager::JMUCManager(JAccount *account, QObject *parent) :
	QObject(parent),
	d_ptr(new JMUCManagerPrivate(this))
{
	Q_D(JMUCManager);
	d->account = account;
	d->bookmarkManager = new JBookmarkManager(d->account);
	//d->inviteManager = new JInviteManager(d->account);
	connect(d->bookmarkManager, SIGNAL(bookmarksChanged()), SLOT(bookmarksChanged()));
	connect(d->account->privacyManager(), SIGNAL(listReceived(QString,QList<Jreen::PrivacyItem>)),
			this, SLOT(onListReceived(QString,QList<Jreen::PrivacyItem>)));
	connect(d->account->privacyManager(), SIGNAL(activeListChanged(QString)),
			this, SLOT(onActiveListChanged(QString)));
	connect(d->account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(_q_status_changed(qutim_sdk_0_3::Status)));
}

JMUCManager::~JMUCManager()
{
}

void JMUCManager::onListReceived(const QString &name, const QList<Jreen::PrivacyItem> &items)
{
	Q_D(JMUCManager);
	Jreen::PrivacyManager *manager = d->account->privacyManager();
	qDebug() << Q_FUNC_INFO << name << manager->activeList();
	if (name == manager->activeList()) {
		QSet<QString> badList;
		QSet<Jreen::JID> conferences;
		foreach (JMUCSession *muc, d->rooms)
			conferences << muc->room()->id();
		QMutableSetIterator<Jreen::JID> it(conferences);
		for (int i = 0; i < items.size() && !conferences.isEmpty(); i++) {
			it.toFront();
			while (it.hasNext()) {
				const JID &jid = it.next();
				const PrivacyItem &item = items.at(i);
				qDebug() << jid << item.type() << item.jid() << item.check(jid) << item.action() << item.stanzaTypes();
				if ((item.stanzaTypes() & PrivacyItem::PresenceOut) && item.check(jid)) {
					if (item.action() == PrivacyItem::Deny)
						badList << jid.domain();
					conferences.remove(jid);
				}
			}
		}
		if (badList.isEmpty()) {
			d->connectAll();
			return;
		}
		QSetIterator<QString> it2(badList);
		QList<Jreen::PrivacyItem> newList = items;
		while (it2.hasNext()) {
			QString domain = it2.next();
			Jreen::JID jid;
			jid.setDomain(domain);
			Jreen::PrivacyItem item;
			item.setAction(PrivacyItem::Allow);
			item.setOrder(0);
			item.setJID(jid);
			item.setStanzaTypes(PrivacyItem::PresenceOut);
			newList.prepend(item);
		}
		manager->setList(name, newList);
		d->connectAll();
	}
}

void JMUCManager::onActiveListChanged(const QString &name)
{
	d_func()->account->privacyManager()->requestList(name);
}

void JMUCManager::bookmarksChanged()
{
	Q_D(JMUCManager);
	foreach (const QString &conference, d->rooms.keys()) {
		JMUCSession *muc = d->rooms.value(conference);
		Bookmark::Conference room = d->bookmarkManager->find(conference);
		muc->setBookmark(room);
		if (!room.isValid()) {
			if (!ChatLayer::instance()->getSession(muc, false))
				closeMUCSession(muc);
		}
	}
}

void JMUCManager::join(const QString &conference, const QString &nick, const QString &password)
{
	Q_D(JMUCManager);
	JMUCSession *room = d->rooms.value(conference, 0);
	if (room && room->isError()) {
		room->setBookmark(Bookmark::Conference());
		closeMUCSession(room);
		room = 0;
		if (nick.isEmpty())
			return;
	}
	if (room && room->me() && !nick.isEmpty() && room->me()->name() != nick) {
		if (room->isJoined()) {
			QMessageBox::warning(0, tr("Join groupchat on")+" "+room->id(),
								 tr("You already in conference with another nick"));
		} else {
			room->setBookmark(Bookmark::Conference());
			closeMUCSession(room);
			room = 0;
		}
	}
	if (!room) {
		Q_ASSERT(!nick.isEmpty()); // Room doesn't exist. Nickname is required.
		Jreen::JID jid = conference;
		jid.setResource(nick);
		room = new JMUCSession(jid, password, d->account);
		const QList<Bookmark::Conference> bookmarks = d->bookmarkManager->bookmarksList();
		for (int i = 0; i < bookmarks.count(); i++) {
			if (bookmarks[i].jid() == conference && bookmarks[i].nick() == nick) {
				room->setBookmark(bookmarks[i]);
				break;
			}
		}
		d->rooms.insert(conference, room);
		Jreen::PrivacyManager *manager = d->account->privacyManager();
		emit conferenceCreated(room);
		d->roomsToConnect << room;
		manager->requestList(manager->activeList());
	} else {
		room = d->rooms.value(conference);
		room->join();
	}
	//		p->account->client()->registerPresenceHandler(JID(conference.toStdString()),
	//													  p->account->connection()->softwareDetection());
	ChatSession *session = ChatLayer::get(room, true);
	connect(session, SIGNAL(destroyed()), room, SIGNAL(initClose()));
	connect(room, SIGNAL(initClose()), SLOT(closeMUCSession()));
	//	I think that it should be called by plugins, but not by protocol itself,
	//	because it's rather slow method due to a lot of gui initialization
	//	session->activate();

	bookmarkManager()->saveRecent(conference,nick,password);
}

void JMUCManager::closeMUCSession()
{
	JMUCSession *room = qobject_cast<JMUCSession *>(sender());
	closeMUCSession(room);
}

void JMUCManager::closeMUCSession(JMUCSession *room)
{

}

void JMUCManager::appendMUCSession(JMUCSession *room)
{
	Q_ASSERT(room);
	d_func()->rooms.insert(room->id(), room);
}

void JMUCManager::setPresenceToRooms(Jreen::Presence::Type presence)
{
	Q_D(JMUCManager);
	if (presence == Jreen::Presence::Unavailable) {
		foreach (JMUCSession *room, d->rooms) {
			if(room->isJoined()) {
				room->setAutoJoin(true);
				room->leave();
			}
		}
	} else {
		foreach (JMUCSession *room, d->rooms)
			if(room->isJoined() || room->isAutoJoin())
				room->join();
	}
}

ChatUnit *JMUCManager::muc(const Jreen::JID &jid)
{
	if (JMUCSession *muc = d_func()->rooms.value(jid.bare())) {
		if (jid.isBare())
			return muc;
		else
			return muc->participant(jid.resource());
	}
	return 0;
}

JBookmarkManager *JMUCManager::bookmarkManager()
{
	return d_func()->bookmarkManager;
}

void JMUCManager::syncBookmarks()
{
	d_func()->bookmarkManager->sync();
}

bool JMUCManager::event(QEvent *event)
{
	return QObject::event(event);
}

void JMUCManager::leave(const QString &room)
{
	JMUCSession *muc = d_func()->rooms.value(room);
	muc->leave();
}

}

#include "jmucmanager.moc"