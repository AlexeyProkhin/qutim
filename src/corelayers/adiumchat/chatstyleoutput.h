/****************************************************************************
 *  chatstyleoutput.h
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

#ifndef CHATSTYLEOUTPUT_H_
#define CHATSTYLEOUTPUT_H_

#include <QObject>
#include "chatstyle.h"
#include "chatsessionimpl.h"
#include <libqutim/contact.h>
class QWebPage;
class QDateTime;

namespace qutim_sdk_0_3
{
	class ChatSession;
}

namespace AdiumChat
{
	class ChatStyleOutput
	{
	public:
		/*
		constructor, _styleName - name of the style to use, it have to be same to directory name
		_variantName - name of the CSS file to use, don't write .css
		*/
		ChatStyleOutput();
		~ChatStyleOutput();

		/*
		passes valriants QHash (list of all variants) from lower class
		*/
		StyleVariants getVariants() const;

		/*
		sets variant to use, it's used for setting
		*/
		void setVariant(const QString &_variantName);
		/*
		creats a html skeleton. Future messages will be added to it
		skeleton consist of styles, header and footer
		it has a mark as well. before this mark new messages should be added
		*/
		QString makeSkeleton(const QString &_chatName, const QString &_ownerName,
							const QString &_partnerName, const QString &_ownerIconPath,
							const QString &_partnerIconPath, const QDateTime &datetime);
		QString makeMessage(const ChatSessionImpl *session, const Message &mes, bool _aligment, bool _sameSender);
		/*
		changes keywords to action atributes in html like "Bob is writing on the desk"
		_name - sender's nickname
		_message - message text
		_direction - "true" if it was send, "false" if it was recived
		_time - time at witch message was send
		_avatarPath - path to avatar, if "" uses style provided picture
		_aligment - "true" if left-to-right, "false" if right-to-left
		_senderID - sender's ID
		_servise - protocol used to send a message
		*/
		QString makeAction(const ChatSessionImpl *session, const Message &mes, const bool& _aligment);

		/*
		It is used for displaying system and user messages like "user gone offline",
		"Marina is now away", "You are being ignored" etc.
		_message - message by it self to be shown
		_time - timestamp
		*/
		QString makeStatus(const QString &_message, const QDateTime &datetime);

		/*
		for degubing purpose, must be deleted before release
		*/
		QString getMainCSS();
		QString getVariantCSS();
		void preparePage(QWebPage *page, Account* acc, const QString& id);
		void reloadStyle(QWebPage* page);
	private:
		/*
		makes html code from plaint text //TODO rewrite on javascript
		*/
		QString findEmail(const QString &_sourceHTML);
		QString findWebAddress(const QString &_sourceHTML);

	private:
		inline void makeTime (QString &input, const QDateTime& datetime,const QString &regexp = "%time\\{([^}]*)\\}%");
		void loadSettings();
		void loadTheme(const QString &name, const QString &variant = QString());
		/*
		style used for output generation
		*/
		ChatStyle m_current_style;
		QScopedPointer<ChatStyleGenerator> m_chat_style_generator;
		/*
		remembers current variant name
		*/
		QString m_current_variant;
		QString m_current_style_path;
		QString m_current_datetime_format;
	};
}

#endif /*CHATSTYLEOUTPUT_H_*/
