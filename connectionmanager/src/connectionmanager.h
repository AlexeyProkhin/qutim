#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <qutim/plugin.h>
#include <qutim/settingslayer.h>
#include <qutim/status.h>

#if (QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
#	define HAVE_NETWORK_MANAGER
#endif

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
#ifdef HAVE_NETWORK_MANAGER
		QPointer<QNetworkConfigurationManager> m_network_conf_manager;
#endif
		GeneralSettingsItem<ManagerSettings> *m_item;
	};

}
#endif // CONNECTIONMANAGER_H
