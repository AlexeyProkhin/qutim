/****************************************************************************
 *  settingswidget.h
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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "libqutim_global.h"
#include <QWidget>
#include <QScopedPointer>

namespace qutim_sdk_0_3
{
	struct SettingsWidgetPrivate;

	class SettingsWidget : public QWidget
	{
		Q_OBJECT
		Q_PROPERTY(bool modified READ isModified NOTIFY modifiedChanged)
	public:
		SettingsWidget();
		virtual ~SettingsWidget();
	public slots:
		void load();
		void save();
	signals:
		void changesStateChanged(bool have_changes);
	protected:
		virtual void loadImpl() = 0;
		virtual void saveImpl() = 0;
		void listenChildrenStates(const QWidgetList &exceptions = QWidgetList());
		bool lookForWidgetState(QWidget *widget, const char *property = 0, const char *signal = 0);
	private slots:
		void onStateChanged(int index);
	private:
		QScopedPointer<SettingsWidgetPrivate> p;
	};
}

#endif // SETTINGSWIDGET_H
