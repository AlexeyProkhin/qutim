#include "plugpackagemodel.h"

// #define Q_VALID_TYPE(item) (item.m_item_type <= 2)
// #define Q_CHECK_TYPE(item) \
// 	if( !Q_VALID_TYPE(item) ) \
// 		return
#define Q_INDEX(item) createIndex(item->childNumber(), 0, item)

PlugPackageModel::PlugPackageModel(QObject* parent) : QAbstractItemModel(parent), m_root_item(new plugPackageItem)
{

}

QModelIndex PlugPackageModel::index(int row, int column, const QModelIndex& parent) const {
	Q_UNUSED(column);
	plugPackageItem *item = getItem(parent);
	if(!item)
		return QModelIndex();
	item = item->getChild(row);
	return item ? Q_INDEX(item) : QModelIndex();
}

QModelIndex PlugPackageModel::parent(const QModelIndex& child) const {
	plugPackageItem *item = getItem(child);
	if(!item)
		return QModelIndex();
	item = item->parent();
	if(!item || !item->getData())
		return QModelIndex();
	return Q_INDEX(item);
}

int PlugPackageModel::rowCount(const QModelIndex& parent) const {
	plugPackageItem *item = getItem(parent);
	return item ? item->childCount() : 0;	
}

bool PlugPackageModel::hasChildren(const QModelIndex& parent) const {
return QAbstractItemModel::hasChildren(parent);
}

QVariant PlugPackageModel::data(const QModelIndex& index, int role) const {
	plugPackageItem *item = getItem(index);
	ItemData *data = item->getData();
	if(!item)
		return QVariant();
	if(!data)
		return QVariant();
// 	switch(role)
// 	{
// 		case Qt::DisplayRole:
// 			return data->name.isEmpty() ? data->item.m_item_type == 1 ? "Not in list" : data->item.m_item_name : data->name;
// 		case Qt::DecorationRole:
// 			return data->icons.count()==0 ? QVariant() : data->icons[0];
// 		case Qt::UserRole:
// 			return reinterpret_cast<qptrdiff>(data);
// 		case Qt::ToolTipRole:
// 			return m_plugin_system->getItemToolTip(data->item);
// 		default:
// 			return QVariant();
// 	}
	return QVariant();
}

void PlugPackageModel::addItem(const packageInfo& item,const quint32 &id) {
 // 	Q_CHECK_TYPE(item);
	if(getItem(item)) return;
	plugPackageItem *data_item = new plugPackageItem(item, id);
	addItem(item, data_item);
}

void PlugPackageModel::addItem(const packageInfo& item, plugPackageItem* data_item) {
 	QHash<QString, plugPackageItem *> &category = m_items[item.properties["type"]];
 	if (category.isEmpty()) {
		category.insert(item.properties["type"],data_item);
		beginInsertRows(QModelIndex(), m_root_item->childCount(),m_root_item->childCount());
		m_root_item->addChild(item.properties["type"],data_item);
		endInsertRows();
		return;
 	}
	plugPackageItem *package = category.value(item.id, 0); //блин надо у элессара уточнить как оно работает
	if (!package)	{
		delete data_item;
		return;
		}
	beginInsertRows(Q_INDEX(package),package->childCount(),package.childCount());
	package->addChild(item.id, data_item);
	endInsertRows();
// 	return;
// 	category.insert()
// 	if(item.m_item_type == 2)
// 	{
// 		protocol.insert(item.m_account_name, data_item);
// 		beginInsertRows(QModelIndex(), m_root_item->childCount(), m_root_item->childCount());
// 		m_root_item->addChild(item.m_protocol_name+"."+item.m_account_name, data_item);
// 		endInsertRows();
// 		return;
// 	}
// 	DataItem *account = protocol.value(item.m_account_name, 0);
// 	if(!account)
// 	{
// 		delete data_item;
// 		return;
// 	}
// 	if(item.m_item_type == 1)
// 	{
// 		beginInsertRows(Q_INDEX(account), account->childCount(), account->childCount());
// 		account->addChild(item.m_item_name, data_item);
// 		endInsertRows();
// 		return;
// 	}
// 	DataItem *group = account->getChild(item.m_parent_name);
// 	if(!group)
// 	{
// 		delete data_item;
// 		return;
// 	}
// 	beginInsertRows(Q_INDEX(group), group->childCount(), group->childCount());
// 	group->addChild(item.m_item_name, data_item);
// 	endInsertRows();
}

plugPackageItem* PlugPackageModel::getItem(const QModelIndex& index) const {

}

plugPackageItem* PlugPackageModel::getItem(const packageInfo& item) const {

}

QStringList PlugPackageModel::getItemChildren(const packageInfo& item) {

}

const plugPackageItem* PlugPackageModel::getItemData(const packageInfo& item) {

}


void PlugPackageModel::removeItem(const packageInfo& item) {

}

void PlugPackageModel::setItemAttribute(const packageInfo& item, packageAttribute type, bool on) {

}

void PlugPackageModel::setItemDescription(const packageInfo& item, const QVector< QVariant >& text) {

}

void PlugPackageModel::setItemIcon(const packageInfo& item, const QIcon& icon, int position) {

}

void PlugPackageModel::setItemName(const packageInfo& item, const QString& name) {

}

void PlugPackageModel::setItemShortDesc(const packageInfo& item, const QString& shortdesc) {

}

void PlugPackageModel::setItemVisibility(const packageInfo& item, int flags) {

}













