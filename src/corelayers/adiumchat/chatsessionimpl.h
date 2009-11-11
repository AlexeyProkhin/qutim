/****************************************************************************
 *  chatsessionimpl.h
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

#ifndef CHATSESSIONIMPL_H
#define CHATSESSIONIMPL_H
#include <libqutim/messagesession.h>
#include <QPointer>

class QWebPage;
using namespace qutim_sdk_0_3;
namespace AdiumChat
{
	class ChatStyleOutput;
	class ChatSessionImpl : public ChatSession
	{
		Q_OBJECT
	public:
		ChatSessionImpl ( Account* acc, const QString& id, ChatLayer* chat );
		virtual ~ChatSessionImpl();
		virtual void addContact ( Contact* c );
		virtual void appendMessage ( const Message& message );
		virtual void chatStateChanged ( Contact* c, ChatState state );
		virtual void removeContact ( Contact* c );
		QWebPage *getPage() const;
		Account *getAccount() const;
		QString getId() const;
		ChatUnit *getUnit(bool create = false) const;
	public slots:
		QVariant evaluateJavaScript(const QString &scriptSource);
		virtual void activate(bool active);
		virtual bool isActive();
	private:
		QPointer<QWebPage> m_web_page;
		QScopedPointer<ChatStyleOutput> m_chat_style_output;
		Account *m_account;
		QString m_session_id;
		//additional info and flags
		QString m_previous_sender; //me or nme (not me) //FIXME need refactoring in future
		int m_message_count;
	signals:
		void removed(Account *,const QString &);//may be removed in future
	};
}
#endif // CHATSESSIONIMPL_H
