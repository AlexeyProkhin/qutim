#include "mdock.h"
#include <qutim/icon.h>
#include <qutim/extensioninfo.h>
#include <qutim/debug.h>
#include <QApplication>
#include <QLabel>
#import <AppKit/NSDockTile.h>

extern void qt_mac_set_dock_menu(QMenu *);

namespace MacIntegration
{
	// TODO add dock behaviour
	struct MDockPrivate
	{ 
		QMenu *dockMenu;
		QActionGroup *statusGroup;
		QMenu *chatMenu;
		QHash<ChatSession *, int> unreadSessions;
		QHash<ChatSession *, QAction *> aliveSessions;
		QIcon standartIcon;
	};

	void MDock::setBadgeLabel(const QString &message)
	{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSString* mac_message = [[NSString alloc] initWithUTF8String: message.toUtf8().constData()];
		NSDockTile *dockTile = [NSApp dockTile];
		[dockTile setBadgeLabel: mac_message];
		[dockTile display];
		[pool release];
	}

	MDock::MDock() : d_ptr(new MDockPrivate())
	{
		Q_D(MDock);
		d->standartIcon = Icon("qutim");
		d->dockMenu = new QMenu;
		d->statusGroup = new QActionGroup(this);
		createStatusAction(Status::Online);
		createStatusAction(Status::FreeChat);
		createStatusAction(Status::Away);
		createStatusAction(Status::DND);
		createStatusAction(Status::NA);
		createStatusAction(Status::Invisible);
		createStatusAction(Status::Offline);
		d->dockMenu->addSeparator();
		d->chatMenu = new QMenu(tr("Opened chats"), d->dockMenu);
		d->dockMenu->addMenu(d->chatMenu);
		qt_mac_set_dock_menu(d->dockMenu);
		connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
				this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
		qApp->setQuitOnLastWindowClosed(false);
	}  

	MDock::~MDock()
	{
	}

	void MDock::createStatusAction(Status::Type type)
	{
		Q_D(MDock);
		Status s = Status(type);
		QAction *act = new QAction(s.icon(), s.name(), d->dockMenu);
		act->setCheckable(true);
		connect(act, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
		act->setParent(d->dockMenu);
		act->setData(type);
		d->dockMenu->addAction(act);
		d->statusGroup->addAction(act);
	}

	void MDock::onStatusChanged()
	{
		if (QAction *a = qobject_cast<QAction *>(sender())) {
			Status::Type type = static_cast<Status::Type>(a->data().value<int>());
			foreach(qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
				foreach(Account *account, proto->accounts()) {
					Status status = account->status();
					status.setType(type);
					status.setSubtype(0);
					account->setStatus(status);
				}
			}
		}
	} 

	void MDock::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
	{
		Q_D(MDock);
		QAction *action = new QAction(session->getUnit()->title(), d->chatMenu);
		action->setCheckable(true);
		connect(action, SIGNAL(triggered()), session, SLOT(activate()));
		d->chatMenu->addAction(action);
		d->aliveSessions.insert(session, action);
		connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
		connect(session, SIGNAL(activated(bool)), this, SLOT(onActivatedSession(bool)));
		connect(session, SIGNAL(destroyed()), SLOT(onSessionDestroyed()));
	} 

	void MDock::onSessionDestroyed()
	{ 
		ChatSession *session = static_cast<ChatSession*>(sender());
		delete d_func()->aliveSessions.take(session);
		onUnreadChanged(MessageList());
	}
	
	void MDock::onActivatedSession(bool state)
	{ 
		ChatSession *session = static_cast<ChatSession*>(sender());
		d_func()->aliveSessions.value(session)->setChecked(state);
	}

	void MDock::onUnreadChanged(const qutim_sdk_0_3::MessageList &unread)
 	{  
		ChatSession *session = static_cast<ChatSession*>(sender());
		Q_ASSERT(session != NULL);
		Q_D(MDock);
		if (unread.isEmpty())
			d->unreadSessions.remove(session);
		else
			d->unreadSessions.insert(session, unread.count());
		int uCount = 0;
		foreach (ChatSession *s, d->aliveSessions.keys())
			if (d->unreadSessions.keys().contains(s)) {
				d->aliveSessions.value(s)->setText("✉ " + d->aliveSessions.value(s)->text().remove("✉ "));
				uCount += d->unreadSessions.value(s, 0);
			} else {
				d->aliveSessions.value(s)->setText(d->aliveSessions.value(s)->text().remove("✉ "));
			}
		setBadgeLabel(!uCount ? QString() : QString::number(uCount));
	}

	void MDock::onStatusChanged(const qutim_sdk_0_3::Status &status)
	{
		/*Account *account = qobject_cast<Account*>(sender());
		  if (account == m_activeAccount || !m_activeAccount) {
		  m_activeAccount = account;
		  if (account->status().type() == Status::Offline) {
		  m_activeAccount = 0;
		  }
		  d->standartIcon = status.icon();
		  }
		  if (!m_activeAccount) {
		  foreach (Account *acc, m_accounts) {
		  if (acc->status().type() != Status::Offline) {
		  m_activeAccount = acc;
		  d->standartIcon = acc->status().icon();
		  break;
		  }
		  }
		  }
		  if (!m_isMail)
		  m_icon->setIcon(d->standartIcon);*/
	} 
}
