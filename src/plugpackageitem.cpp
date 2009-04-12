#include "plugpackageitem.h"
#include <QDebug>

plugPackageItem::plugPackageItem(const ItemData& data, const QString& name)
: parent(0)
{
	item_data = data;
	item_name = name;
}

plugPackageItem::plugPackageItem()
: parent(0)
{

}


plugPackageItem::~plugPackageItem() {
	qDeleteAll(children);
}

void plugPackageItem::addChild(plugPackageItem *data, const int& id) {
	data->parent = this;
	children.insert(id,data);
	qDebug () << "add child:" << data->item_name << " id :" << id << "count :" << children.count();
}

void plugPackageItem::removeChild(const int& id) {
	//FIXME так работать не буит ^_^
	children.remove(id);
}
