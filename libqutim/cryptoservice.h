#ifndef CRYPTOSERVICE_H
#define CRYPTOSERVICE_H

#include "libqutim_global.h"
#include <QPointer>
#include <QVariant>

namespace qutim_sdk_0_3
{
	class ModuleManager;
	class ExtensionInfo;

	class LIBQUTIM_EXPORT CryptoService : public QObject
	{
		Q_OBJECT
	public:
		static QByteArray crypt(const QVariant &value);
		static QVariant decrypt(const QByteArray &value);
	protected:
	public:
		virtual QByteArray cryptImpl(const QByteArray &value) const = 0;
		virtual QByteArray decryptImpl(const QByteArray &value) const = 0;
		virtual void setPassword(const QString &password) = 0;
		CryptoService();
		virtual ~CryptoService();
	private:
	public:
//		static void init(const QList<ExtensionInfo> &exts);
		static QPointer<CryptoService> self;
		friend class ModuleManager;
	};
}

#endif // CRYPTOSERVICE_H
