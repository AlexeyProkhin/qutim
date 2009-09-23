#ifndef LIBQUTIM_GLOBAL_H
#define LIBQUTIM_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QObject>

#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
# error Incompatible Qt library, need 4.6.0 at least
#endif

#if defined(LIBQUTIM_LIBRARY)
#  define LIBQUTIM_EXPORT Q_DECL_EXPORT
#else
#  define LIBQUTIM_EXPORT Q_DECL_IMPORT
#endif

#define QUTIM_VERSION_STR "0.2.60.0"
#define QUTIM_VERSION 0x00023c00
#define QUTIM_VERSION_CHECK(major, minor, secminor, patch) ((major<<24)|(minor<<16)|(secminor<<8)|(patch))

template <typename Key, typename T>
class QHash;

namespace qutim_sdk_0_3
{
	enum Status
	{
		Offline = 0,
		Online,
		Away,
		DND,
		NA,
		Occupied,
		FreeChat,
		Evil,       // ?
		Depression, // ?
		Invisible,
		AtHome,     // ?
		AtWork,     // ?
		OutToLunch,
		OnThePhone,
		Connecting  = 100,
		ConnectingStart = Connecting,
		ConnectingStop  = 1000
	};
	class ObjectGenerator;
	class Protocol;
	class ModuleManager;
	typedef QList<const ObjectGenerator *> GeneratorList;
	typedef QHash<QString, Protocol *> ProtocolMap;
	LIBQUTIM_EXPORT const char *qutimVersion();
	LIBQUTIM_EXPORT const char *metaInfo(const QMetaObject *meta, const char *name);
	LIBQUTIM_EXPORT bool isCoreInited();
	LIBQUTIM_EXPORT GeneratorList moduleGenerators(const QMetaObject *module);
	LIBQUTIM_EXPORT ProtocolMap allProtocols();
	LIBQUTIM_EXPORT QString statusToString(Status status, bool translate = true);
}

#endif // LIBQUTIM_GLOBAL_H
