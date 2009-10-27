/****************************************************************************
 *  libqutim_global.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
class QDateTime;

namespace qutim_sdk_0_3
{
	Q_ENUMS(Status)
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
	enum ChatState
	{
		ChatStateActive = 0,    // User is actively participating in the chat session.
		ChatStateInActive,      // User has not been actively participating in the chat session.
		ChatStateGone,          // User has effectively ended their participation in the chat session.
		ChatStateComposing,     // User is composing a message.
		ChateStatePaused        // User had been composing but now has stopped.
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
	LIBQUTIM_EXPORT GeneratorList moduleGenerators(const char *iid);
	template<typename T> inline GeneratorList moduleGenerators_helper(const QObject *)
	{ return moduleGenerators(&T::staticMetaObject); }
	template<typename T> inline GeneratorList moduleGenerators_helper(const void *)
	{ return moduleGenerators(qobject_interface_iid<T *>()); }
	template<typename T> inline GeneratorList moduleGenerators()
	{ return moduleGenerators_helper<T>(reinterpret_cast<T *>(0)); }
	LIBQUTIM_EXPORT ProtocolMap allProtocols();
	LIBQUTIM_EXPORT QString statusToString(Status status, bool translate = true);
	LIBQUTIM_EXPORT QString convertTimeDate(const QString &mac_format, const QDateTime &datetime);
	LIBQUTIM_EXPORT QString &validateCpp(QString &text );
	LIBQUTIM_EXPORT QString getThemePath (const QString& themeName, const QString &category ); //search theme in user and share dir
}

class QDebug;
LIBQUTIM_EXPORT QDebug operator<<(QDebug debug, qutim_sdk_0_3::Status status);

#endif // LIBQUTIM_GLOBAL_H
