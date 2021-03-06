/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2003 Justin Karneges <justin@affinix.com>
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

#include "idle.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#ifndef HAVE_XSS

namespace Psi
{
IdlePlatform::IdlePlatform() {}
IdlePlatform::~IdlePlatform() {}
bool IdlePlatform::init() { return false; }
int IdlePlatform::secondsIdle() { return 0; }
}
#else

#include <qapplication.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>

static XErrorHandler old_handler = 0;
extern "C" int xerrhandler(Display* dpy, XErrorEvent* err)
{
	if(err->error_code == BadDrawable)
		return 0;

	return (*old_handler)(dpy, err);
}

namespace Psi
{
class IdlePlatform::Private
{
public:
	Private() {}

	XScreenSaverInfo *ss_info;
};

IdlePlatform::IdlePlatform()
{
	d = new Private;
	d->ss_info = 0;
}

IdlePlatform::~IdlePlatform()
{
	if(d->ss_info)
		XFree(d->ss_info);
	if(old_handler) {
		XSetErrorHandler(old_handler);
		old_handler = 0;
	}
	delete d;
}

bool IdlePlatform::init()
{
	if(d->ss_info)
		return true;

	old_handler = XSetErrorHandler(xerrhandler);

	int event_base, error_base;
	if(XScreenSaverQueryExtension(QX11Info::display(), &event_base, &error_base)) {
		d->ss_info = XScreenSaverAllocInfo();
		return true;
	}
	return false;
}

int IdlePlatform::secondsIdle()
{
	if(!d->ss_info)
		return 0;
	if(!XScreenSaverQueryInfo(QX11Info::display(), QX11Info::appRootWindow(), d->ss_info))
		return 0;
	return d->ss_info->idle / 1000;
}
}

#endif

#endif

