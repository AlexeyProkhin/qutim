/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef APPLICATIONWINDOWPLUGIN_H
#define APPLICATIONWINDOWPLUGIN_H

#include <QDeclarativeComponent>
#include <qutim/chatsession.h>
#include "chatmessagemodel.h"
#include "../../../adiumwebview/lib/webkitmessageviewcontroller.h"

namespace MeegoIntegration
{
class ChatChannelUsersModel;

class ChatController : public WebKitMessageViewController
{
	Q_OBJECT
	Q_PROPERTY(QString fontFamily READ fontFamily NOTIFY fontFamilyChanged)
	Q_PROPERTY(int fontSize READ fontSize NOTIFY fontSizeChanged)
	Q_PROPERTY(QObject* webView READ webView WRITE setWebView NOTIFY webViewChanged)
public:
	ChatController();
	~ChatController();
	
	QString fontFamily() const;
	int fontSize() const;
	QObject *webView() const;
	void setWebView(QObject *webView);
	
public slots:
	void fixFlickable(QObject *object);
	void append(const qutim_sdk_0_3::Message &message);
	void handleElement(const QWebElement &element);
	
protected:
	virtual void appendNick(const QVariant &nick);
	virtual void appendText(const QVariant &text);
	virtual void setDefaultFont(const QString &family, int size);

signals:
	void nickAppended(const QString &nick);
	void textAppended(const QString &text);
	void fontFamilyChanged(const QString &fontFamily);
	void fontSizeChanged(int fontSize);
	void webViewChanged(QObject *webView);
	
private:
	QString m_fontFamily;
	int m_fontSize;
	QObject * m_webView;
};

class ChatChannel : public qutim_sdk_0_3::ChatSession
{
	Q_OBJECT
	Q_PROPERTY(qutim_sdk_0_3::ChatUnit* unit READ unit WRITE setChatUnit NOTIFY unitChanged)
	Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
	Q_PROPERTY(QObject *page READ page WRITE setPage NOTIFY pageChanged)
	Q_PROPERTY(QObject* model READ model CONSTANT)
	Q_PROPERTY(QObject* units READ units CONSTANT)
	Q_PROPERTY(bool supportJavaScript READ supportJavaScript CONSTANT)
public:
	ChatChannel(qutim_sdk_0_3::ChatUnit *unit);
	virtual ~ChatChannel();

	virtual qutim_sdk_0_3::ChatUnit *getUnit() const;
	virtual void setChatUnit(qutim_sdk_0_3::ChatUnit* unit);
	virtual QTextDocument *getInputField();
	virtual void markRead(quint64 id);
	virtual qutim_sdk_0_3::MessageList unread() const;
	int unreadCount() const;
	virtual void addContact(qutim_sdk_0_3::Buddy *c);
	virtual void removeContact(qutim_sdk_0_3::Buddy *c);
	QObject *model() const;
	Q_INVOKABLE qint64 send(const QString &text);
	Q_INVOKABLE void showChat();
	QObject *units() const;
	QObject *page() const;
	void setPage(QObject *page);
	bool supportJavaScript() const;
	
	Q_INVOKABLE QVariant evaluateJavaScript(const QString &script);
	
protected:
	virtual qint64 doAppendMessage(qutim_sdk_0_3::Message &message);
	virtual void doSetActive(bool active);
	
signals:
	void javaScriptRequest(const QString &javaScript, QVariant *variant);
	void messageAppended(const qutim_sdk_0_3::Message &message);
	void unitChanged(qutim_sdk_0_3::ChatUnit *unit);
	void unreadCountChanged(int);
	void pageChanged(QObject *page);
	
private:
	qutim_sdk_0_3::ChatUnit *m_unit;
	qutim_sdk_0_3::MessageList m_unread;
	ChatMessageModel *m_model;
	ChatChannelUsersModel *m_units;
	QObject *m_page;
};
}

#endif // APPLICATIONWINDOWPLUGIN_H

