/****************************************************************************
 *  snachandler.h
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

#ifndef SNACHANDLER_H
#define SNACHANDLER_H

#include <QObject>
#include <QPair>
#include "snac.h"
#include "util.h"
#include "oscarconnection.h"

typedef QPair<quint16, quint16> SNACInfo;

class SNACHandler : public QObject
{
	Q_OBJECT
public:
    SNACHandler();
	const QList<SNACInfo> &infos() { return m_infos; }
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac) = 0;
protected:
	QList<SNACInfo> m_infos;
};

#endif // SNACHANDLER_H
