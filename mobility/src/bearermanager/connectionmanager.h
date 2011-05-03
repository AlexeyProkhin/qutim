#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <qutim/plugin.h>
#include <qutim/settingslayer.h>
#include <qutim/status.h>
#include <QQueue>

namespace qutim_sdk_0_3
{
	class Account;
}

class QNetworkConfigurationManager;
namespace ConnectionManager
{
using namespace qutim_sdk_0_3;

class ManagerSettings;
class ConnectionManager : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "ConnectionManager")
	Q_CLASSINFO("Service", "Bearer")
	Q_CLASSINFO("Uses", "SettingsLayer")
public:
	virtual ~ConnectionManager();
	virtual void init();
	virtual bool load();
	virtual bool unload();
protected slots:
	void onOnlineStateChanged( bool isOnline );
	void onAccountCreated(qutim_sdk_0_3::Account *account); //autoconnect on startup
	void onStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status);
	void onStatusChangeTimeout();
private:
	QTimer *getTimer(Account *account, bool create = true);
	void removeTimer(QTimer *timer);
	QPointer<QNetworkConfigurationManager> m_network_conf_manager;
	GeneralSettingsItem<ManagerSettings> *m_item;
	QMap<Account*, QTimer*> m_timers;
};

}
#endif // CONNECTIONMANAGER_H
