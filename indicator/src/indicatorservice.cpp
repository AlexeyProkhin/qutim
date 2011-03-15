/****************************************************************************
 * indicatorservice.cpp
 *  Copyright © 2010-2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv3 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include <qindicateindicator.h>
#include "indicatorservice.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QDateTime>
#include <QApplication>

IndicatorService::IndicatorService() :
	desktopName( QUTIM_DESKTOP_FILE ),
	indicateServer(QIndicate::Server::defaultInstance()),
	quitButton(new QIndicate::Indicator)
{
	indicateServer->setType("message.im");
	indicateServer->setDesktopFile(desktopName);
	indicateServer->show();
	connect(indicateServer, SIGNAL(serverDisplay()), SLOT(showMainWindow()));
	connect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));

	//QImage icon = qutim_sdk_0_3::Icon("qutim").pixmap(64).toImage();
	
	qApp->setQuitOnLastWindowClosed(false);

	/* Quit Button */
	quitButton->setNameProperty(QT_TRANSLATE_NOOP("Plugin", "Close qutIM"));
	QImage icon = qutim_sdk_0_3::Icon("application-exit").pixmap(64).toImage();
	connect(quitButton, SIGNAL(display(QIndicate::Indicator*)), qApp, SLOT(quit()));
	quitButton->setIconProperty(icon);
	quitButton->show();
}

IndicatorService::~IndicatorService()
{
	quitButton->hide();
	delete quitButton;
	disconnect(indicateServer, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	disconnect(qutim_sdk_0_3::ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	qDeleteAll(sessionIndicators);
	indicateServer->hide();
	delete indicateServer;
}

void IndicatorService::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	qDebug() << "[Indicator] onSessionCreated";
	if (sessionIndicators.contains(session))
		return;
	qDebug() << "[Indicator] List doesn't contain session. Adding indicator";
	QIndicate::Indicator *indicator = new QIndicate::Indicator;
	sessionIndicators.insert(session, indicator);
	
	connect(session, SIGNAL(destroyed()), SLOT(onSessionDestroyed()));
	connect(session, SIGNAL(unreadChanged(const qutim_sdk_0_3::MessageList&)), SLOT(onUnreadChanged(const qutim_sdk_0_3::MessageList&)));
	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
	connect(indicator, SIGNAL(display(QIndicate::Indicator*)), SLOT(onIndicatorDisplay(QIndicate::Indicator*)), Qt::QueuedConnection);

	QString name = session->getUnit()->title();
	qDebug() << "Setting indicator name: " << name;
	indicator->setNameProperty(name);
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(false);
	indicator->setCountProperty(session->unread().count());
	indicator->setIconProperty(QImage());
	indicator->setIndicatorProperty("subtype", "im");
	indicator->setIndicatorProperty("sender", name);
	indicator->show();
}

void IndicatorService::onSessionDestroyed()
{
	qDebug() << "[Indicator] onSessionDestroyed";
	qutim_sdk_0_3::ChatSession *session = static_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session)
		return;
	delete sessionIndicators.take(session);
}

void IndicatorService::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
{
	qDebug() << "[Indicator] onUnreadChanged";
	if (messages.isEmpty())
		return;
	qDebug() << "[Indicator] Message list isn't empty. Looking for session.";
	qutim_sdk_0_3::ChatSession* session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session || session->isActive())
		return;
	qDebug() << "[Indicator] session exists and not active.";

	QIndicate::Indicator *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	qDebug() << "[Indicator] Indicator has been found. Displaying.";
	QDateTime time = messages.last().time();
	qDebug() << "Setting indicator time: " << time;
	indicator->setTimeProperty(time);
	indicator->setDrawAttentionProperty(true);
	QImage icon = qutim_sdk_0_3::Icon("mail-unread-new").pixmap(64).toImage();
	indicator->setIconProperty(icon);
	indicator->setCountProperty(session->unread().count());
	// TODO: ->setIconProperty(QImage)
	indicator->show();
}

void IndicatorService::onAccountCreated(qutim_sdk_0_3::Account *)
{
}

void IndicatorService::onAccountDestroyed(QObject *)
{
}

void IndicatorService::loadSettings()
{
}

void IndicatorService::onSessionActivated(bool active)
{
	qDebug() << "[Indicator] onSessionActivated";
	if (!active)
		return;
	qutim_sdk_0_3::ChatSession *session = qobject_cast<qutim_sdk_0_3::ChatSession*>(sender());
	if (!session)
		return;
	QIndicate::Indicator *indicator = sessionIndicators.value(session);
	if (!indicator)
		return;
	indicator->setTimeProperty(QDateTime::currentDateTime());
	indicator->setDrawAttentionProperty(false);
	indicator->setCountProperty(session->unread().count());
	indicator->setIconProperty(QImage());
	//indicator->hide();
}

void IndicatorService::onIndicatorDisplay(QIndicate::Indicator* indicator)
{
	qDebug() << "[Indicator] onIndicatorDisplay";
	qutim_sdk_0_3::ChatSession* session = sessionIndicators.key(indicator);
	if (!sessionIndicators.contains(session))
		return;
	session->activate();
}

void IndicatorService::showMainWindow()
{
	qDebug() << "[Indicator] showMainWindow";
	if (QObject *obj = qutim_sdk_0_3::ServiceManager::getByName("ContactList"))
	{
		obj->metaObject()->invokeMethod(obj, "show");
		QWidget *objWidget = qobject_cast<QWidget*>(obj);
		if (objWidget)
			objWidget->raise();
	}
}
