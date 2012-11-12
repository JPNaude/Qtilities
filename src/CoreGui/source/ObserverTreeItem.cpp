/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
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
    int count = childItemList.count();
    for (int i = count-1; i >= 0; i--) {
        if (childItemList.at(i))
            delete childItemList.at(i);
    }
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
