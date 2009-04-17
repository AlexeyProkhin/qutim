/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#ifndef PLUGPACKAGEMODEL_H
#define PLUGPACKAGEMODEL_H

#include <QAbstractItemModel>
#include "plugpackage.h"
#include "plugpackageitem.h"

class plugPackageModel : public QAbstractItemModel
{
public:
	plugPackageModel(QObject *parent = 0);
	~plugPackageModel();
// 	void setRootNode (plugPackageItem *plug_package_item);
	QModelIndex index (	int row, int column,
						const QModelIndex &parent) const;
	QModelIndex parent (const QModelIndex &child) const;
	bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	void addItem(const ItemData &item, const QString &name = "");
	void clear();
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section,
								Qt::Orientation orientation,
								int role = Qt::DisplayRole) const;
private:
	plugPackageItem *m_root_node;
	plugPackageItem *nodeFromIndex(const QModelIndex &index) const;
	QHash<QString, plugPackageItem *> m_category_nodes; //! категории пакетов
	QVector<plugPackageItem *> m_packages;
	QList<plugPackageItem *> m_checked_packages;
};

#endif // PLUGPACKAGEMODEL_H
