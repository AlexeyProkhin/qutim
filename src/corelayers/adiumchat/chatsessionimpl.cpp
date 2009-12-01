/****************************************************************************
 *  chatsessionimpl.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "chatsessionimpl.h"
#include "chatstyleoutput.h"
#include <QWebPage>
#include <QWebFrame>
#include <libqutim/account.h>
#include <QTextDocument>
#include <QDateTime>
#include <QDebug>
#include "libqutim/history.h"
#include <libqutim/notificationslayer.h>

namespace AdiumChat

{

	ChatSessionImpl::ChatSessionImpl ( ChatUnit* unit, ChatLayer* chat)
	: ChatSession ( chat ),m_chat_style_output(new ChatStyleOutput),m_web_page(new QWebPage)
	{
		m_chat_unit = unit;
		qDebug() << "create session" << unit->title();
		connect(unit,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
		m_store_service_messages = Config("appearance/chat").group("general/history").value<bool>("storeServiceMessages", false);
		Contact *contact = qobject_cast<Contact*>(m_chat_unit);
		if (contact)
			connect(contact,SIGNAL(statusChanged(Status)),SLOT(statusChanged(Status)));
		m_chat_style_output->preparePage(m_web_page,this);
		m_message_count = 0;
		loadHistory();
	}

	void ChatSessionImpl::loadTheme(const QString& path, const QString& variant)
	{
		m_chat_style_output->loadTheme(path,variant);
		m_chat_style_output->preparePage(m_web_page,this);
	}

	void ChatSessionImpl::setVariant(const QString& variant)
	{
		m_chat_style_output->setVariant(variant);
		m_chat_style_output->reloadStyle(m_web_page);
	}

	QString ChatSessionImpl::getVariant() const
	{
		return m_chat_style_output->getVariant();
	}

	void ChatSessionImpl::loadHistory()
	{
		ConfigGroup adium_chat = Config("appearance/chat").group("general/history");
		int max_num = adium_chat.value<int>("maxDisplayMessages",5);
		MessageList messages = History::instance()->read(getUnit(),max_num);
		foreach (Message mess, messages) {
			mess.setProperty("history",true);
			mess.setChatUnit(getUnit());
			appendMessage(mess);
		}
		m_previous_sender = "";
	}

	ChatSessionImpl::~ChatSessionImpl()
	{
		if (m_web_page)
		{
			m_web_page->deleteLater();
		}
		qDebug() << "Session removed:";
	}

	void ChatSessionImpl::addContact ( Contact* c )
	{
		connect(c,SIGNAL(statusChanged(Status)),SLOT(statusChanged(Status)));
	}

	void ChatSessionImpl::appendMessage ( const Message& message )
	{
		Message tmp_message = message;
		if (!tmp_message.chatUnit()) {
			qWarning() << tr("Message %1 must have a ChatUnit").arg(tmp_message.text());
			tmp_message.setChatUnit(getUnit());
		}
		bool same_from = false;
		bool service = tmp_message.property("service").isValid();
		QString item;
		if(tmp_message.text().startsWith("/me ")) {
			tmp_message.setText(tmp_message.text().mid(3));
			tmp_message.setProperty("title",tmp_message.isIncoming() ? tmp_message.chatUnit()->title() : tmp_message.chatUnit()->account()->name());
			item = m_chat_style_output->makeAction(this,tmp_message,true);
			m_previous_sender = "";
		}
		else if (service)
		{
			Notifications::Type service_type = static_cast<Notifications::Type>(tmp_message.property("service").toInt());		;
			item = m_chat_style_output->makeStatus(this,tmp_message);
			m_previous_sender = "";
		}
		else {
			same_from = (m_previous_sender == (tmp_message.isIncoming()?"nme":"me"));
			item = m_chat_style_output->makeMessage(this, tmp_message, true,
															same_from );
			m_previous_sender = (tmp_message.isIncoming()?"nme":"me");
		}

		QString result = m_web_page->mainFrame()->evaluateJavaScript(QString("getEditedHtml(\"%1\", \"%2\");")
																	 .arg(validateCpp(item))
																	 .arg(m_message_count)).toString();
		QString jsTask = QString("append%2Message(\"%1\");").arg(
				result.isEmpty() ? item :
				validateCpp(result.replace("\\","\\\\")), same_from?"Next":"");
		bool isHistory = tmp_message.property("history", false);
		bool silent = tmp_message.property("silent", false);
		if (!isHistory && !silent) {
			Notifications::NotifyOptions opts = service ? Notifications::NotifyOptionUpdate : Notifications::NotifyOptionNormal;
			Notifications::sendNotification(tmp_message,opts);
		}
		if (tmp_message.property("store",true) && (!service || (service && m_store_service_messages)))
			History::instance()->store(message);
		m_web_page->mainFrame()->evaluateJavaScript(jsTask);
		if (result.isEmpty())
			m_message_count++;
	}

	void ChatSessionImpl::chatStateChanged ( Contact* c, ChatState state )
	{
		//TODO
	}

	void ChatSessionImpl::removeContact ( Contact* c )
	{

	}


	QWebPage* ChatSessionImpl::getPage() const
	{
		return m_web_page;
	}

	Account* ChatSessionImpl::getAccount() const
	{
		return m_chat_unit->account();
	}

	QString ChatSessionImpl::getId() const
	{
		return m_chat_unit->id();
	}


	ChatUnit* ChatSessionImpl::getUnit() const
	{
		return m_chat_unit;
	}

	QVariant ChatSessionImpl::evaluateJavaScript(const QString &scriptSource)
	{
		if(m_web_page.isNull())
			return QVariant();
		return m_web_page->mainFrame()->evaluateJavaScript(scriptSource);
	}

	void ChatSessionImpl::setActive(bool active)
	{
		m_active = active;
		emit activated(active);
	}

	bool ChatSessionImpl::isActive()
	{
		return m_active;
	}

	QAbstractItemModel* ChatSessionImpl::getItemsModel() const
	{
		return 0; //TODO
	}
	
	void ChatSessionImpl::statusChanged(Status status)
	{
		Contact *contact = qobject_cast<Contact *>(sender());		
		if (!contact && contact->property("silent").toBool())
			return;
		QString text = contact->property("statusText").toString();
		Message msg;
		msg.setChatUnit(contact);
		msg.setIncoming(true);
		msg.setProperty("service",Notifications::StatusChange);
		msg.setProperty("title",contact->title() + " " + statusToString(status));
		msg.setText(text);
		appendMessage(msg);
	}

}
