/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#include "ObserverTreeItem.h"
#include "Observer.h"

#include <QStringList>
#include <QtDebug>

Qtilities::CoreGui::ObserverTreeItem::ObserverTreeItem(QObject* object,
                                                       ObserverTreeItem *parent,
                                                       const QVector<QVariant> &data,
                                                       TreeItemType item_type) : QObject(0) {
    parent_item = parent;
    itemData = data;
    obj = object;
    type = item_type;
    contained_observer_ref = 0;
    //qDebug() << type;

    if (obj) {
        setObjectName(obj->objectName());
    } else {
        setObjectName("Root item prior to tree construction");
    }
}

Qtilities::CoreGui::ObserverTreeItem::ObserverTreeItem(const ObserverTreeItem& ref) : QObject(0) {
    parent_item = ref.parentItem();
    itemData = ref.itemData;
    obj = ref.obj;
    type = ref.type;
    contained_observer_ref = 0;

    if (ref.obj) {
        setObjectName(ref.obj->objectName());
    } else {
        setObjectName("Root item prior to tree construction");
    }
}

Qtilities::CoreGui::ObserverTreeItem::~ObserverTreeItem() {
    QListIterator<QPointer<ObserverTreeItem> > itr(childItemList);
    while (itr.hasNext()) {
        QPointer<ObserverTreeItem> item = itr.next();
        if (item)
            delete item;
    }
//    int count = childItemList.count();
//    for (int i = count-1; i >= 0; i--) {
//        if (childItemList.at(i))
//            delete childItemList.at(i);
//    }
}

void Qtilities::CoreGui::ObserverTreeItem::appendChild(ObserverTreeItem *child_item) {
    childItemHash[child_item->getObject()->objectName()] = child_item;
    childItemList << child_item;
    child_item->setParent(this);
}

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::ObserverTreeItem::childWithName(const QString& name) const {
    if (childItemHash.contains(name))
        return childItemHash[name];

    return 0;
}

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::ObserverTreeItem::child(int row) {
    return childItemList.at(row);
}

int Qtilities::CoreGui::ObserverTreeItem::childCount() const {
    return childItemList.count();
}

int Qtilities::CoreGui::ObserverTreeItem::columnCount() const {
    return itemData.count();
}

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::ObserverTreeItem::parentItem() const {
    return parent_item;
}

int Qtilities::CoreGui::ObserverTreeItem::row() const {
    if (parent_item)
        return parent_item->childItemList.indexOf(const_cast<ObserverTreeItem*>(this));

    return 0;
}
