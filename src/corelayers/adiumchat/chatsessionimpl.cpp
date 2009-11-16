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
		m_chat_style_output->preparePage(m_web_page,this);
		m_message_count = 0;
		loadHistory();
		connect(unit,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
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
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("general/history");
		int max_num = adium_chat.value<int>("maxDisplayMessages",5);
		MessageList messages = History::instance()->read(getUnit(),max_num);
		foreach (Message mess, messages)
		{
			mess.setProperty("history",true);
			appendMessage(mess);
		}
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

	}

	void ChatSessionImpl::appendMessage ( const Message& message )
	{
		//emit activated(true);
		Message tmp_message = message;
		//TODO add normal check if contact is null
		if (!tmp_message.chatUnit())
		{
			//TODO create fake chat unit for unknown messages
			tmp_message.setChatUnit(getUnit());
		}
		bool same_from = false;
		bool isHistory = tmp_message.property("history", false);
		qDebug() << isHistory << tmp_message.property("silent", false);
		if (isHistory)
		{
			m_previous_sender="";
		}
		QString item;
		if(tmp_message.text().startsWith("/me "))
		{
			tmp_message.setText(tmp_message.text().mid(3));
			item = m_chat_style_output->makeAction(this,tmp_message,true);
			m_previous_sender = "";
		}
		else if (tmp_message.property("service").toBool())
		{
			item = m_chat_style_output->makeStatus(message.text(), QDateTime::currentDateTime());
			m_previous_sender = "";
		}
		else
		{
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
		if (!isHistory && !tmp_message.property("silent", false))
		{
//			tmp_message.setChatUnit(m_chat_unit);
			Notifications::sendNotification(tmp_message);
			qDebug() << "message stored";
			History::instance()->store(message);
		}
		m_web_page->mainFrame()->evaluateJavaScript(jsTask);
		if (result.isEmpty())
			m_message_count++;
	}

	void ChatSessionImpl::chatStateChanged ( Contact* c, ChatState state )
	{

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
		emit activated(active);//FIXME
	}

	bool ChatSessionImpl::isActive()
	{
		return m_active;
	}

	QAbstractItemModel* ChatSessionImpl::getItemsModel() const
	{
		return 0; //TODO
	}

}
