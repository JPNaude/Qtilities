/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include "ObserverTreeModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <SubjectTypeFilter.h>
#include <QtilitiesCoreConstants.h>
#include <Observer.h>
#include <Logger.h>

#include <QIcon>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;

struct Qtilities::CoreGui::ObserverTreeModelData {
    bool                use_observer_hints;
    ObserverTreeItem*   rootItem;
    Observer*           selection_parent;
};

Qtilities::CoreGui::ObserverTreeModel::ObserverTreeModel(const QStringList &headers, QObject* parent) : QAbstractItemModel(parent), AbstractObserverItemModel()
{
    d = new ObserverTreeModelData;

    // Headers
    d_headers = headers;

    // Init root data
    d->rootItem = new ObserverTreeItem();
    d->selection_parent = 0;
}

void Qtilities::CoreGui::ObserverTreeModel::setObserverContext(Observer* observer)
{
    if (!observer)
        return;

    if (d_observer) {
        d_observer->disconnect(this);
        reset();
    }

    AbstractObserverItemModel::setObserverContext(observer);

    if (d_observer) {
        rebuildTreeStructure();

        if (d->use_observer_hints) {
            // Check if this observer provides hints for this model
            if (observer->namingControlHint() != Observer::NoNamingControlHint)
                d_naming_control = observer->namingControlHint();
            if (observer->activityDisplayHint() != Observer::NoActivityDisplayHint)
                d_activity_display = observer->activityDisplayHint();
            if (observer->activityControlHint() != Observer::NoActivityControlHint)
                d_activity_control = observer->activityControlHint();
            if (observer->itemSelectionControlHint() != Observer::NoItemSelectionControlHint)
                d_item_selection_control = observer->itemSelectionControlHint();
            if (observer->itemViewColumnFlags() != Observer::NoItemViewColumnHint)
                d_item_view_column_flags = observer->itemViewColumnFlags();
        }

        connect(d_observer,SIGNAL(destroyed()),SLOT(handleObserverContextDeleted()));
        connect(d_observer,SIGNAL(partialStateChanged(QString)),SLOT(rebuildTreeStructure(QString)));
    }
}

QStack<int> Qtilities::CoreGui::ObserverTreeModel::getParentHierarchy(const QModelIndex& index) const {
    QStack<int> parent_hierarchy;
    ObserverTreeItem* item = getItem(index);
    if (!item)
        return parent_hierarchy;

    ObserverTreeItem* parent_item = item->parent();
    Observer* parent_observer = qobject_cast<Observer*> (parent_item->getObject());
    // Handle the cases where the parent is a category item
    if (!parent_observer) {
        if (parent_item->itemType() == ObserverTreeItem::CategoryItem) {
            parent_observer = parent_item->containedObserver();
            parent_item = parent_item->parent();
        }
    }

    while (parent_observer) {
        parent_hierarchy.push_front(parent_observer->observerID());
        parent_item = parent_item->parent();
        if (parent_item) {
            parent_observer = qobject_cast<Observer*> (parent_item->getObject());
            if (!parent_observer) {
                if (parent_item->itemType() == ObserverTreeItem::CategoryItem) {
                    parent_observer = parent_item->containedObserver();
                    parent_item = parent_item->parent();
                }
            }
        } else
            parent_observer = 0;
    }

    return parent_hierarchy;
}

void Qtilities::CoreGui::ObserverTreeModel::toggleUseObserverHints(bool toggle) {
    d->use_observer_hints = toggle;
}

QVariant Qtilities::CoreGui::ObserverTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (!d_observer)
        return QVariant();

    if (index.column() == NameColumn) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return getItem(index)->data(index.column());
        } else if (role == Qt::CheckStateRole) {
            if (d_activity_display == Observer::CheckboxActivityDisplay || d_activity_control == Observer::CheckboxTriggered) {
                ObserverTreeItem* item = getItem(index);
                Q_ASSERT(item);
                ObserverTreeItem* parent_item = item->parent();
                QObject* obj = item->getObject();
                Observer* parent_observer = 0;

                if (parent_item)
                    parent_observer = qobject_cast<Observer*> (parent_item->getObject());
                QVariant subject_activity;

                if (parent_observer)
                    subject_activity = parent_observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY);
                else
                    subject_activity = d_observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY);

                if (subject_activity.isValid())
                    return subject_activity.toBool();
                else
                    return QVariant();
            }
        } else if (role == Qt::DecorationRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ICON);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        } else if (role == Qt::ToolTipRole) {
            // Check if the object has an OBJECT_TOOLTIP shared property to show.
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();
            SharedObserverProperty tooltip = d_observer->getSharedProperty(obj,OBJECT_TOOLTIP);
            if (tooltip.isValid()) {
                return tooltip.value();
            }
        }
    } else if (index.column() == ChildCountColumn) {
        if (role == Qt::DisplayRole) {
            // Check if it is an observer, in that case we return childCount() on the observer
            Observer* observer = qobject_cast<Observer*> (getItem(index)->getObject());
            if (observer) {
                return observer->childCount();
            } else {
                QObject* obj = getItem(index)->getObject();
                // Handle the case where the child is the parent of an observer
                int count = 0;
                foreach (QObject* child, obj->children()) {
                    Observer* child_observer = qobject_cast<Observer*> (child);
                    if (child_observer)
                        count += child_observer->childCount();
                }

                if (count == 0) {
                    if (getItem(index)->itemType() == ObserverTreeItem::CategoryItem) {
                        Observer* parent_observer = qobject_cast<Observer*> (getItem(index)->containedObserver());
                        if (parent_observer)
                            return parent_observer->subjectReferencesByCategory(getItem(index)->category()).count();
                        else
                            return QVariant();
                    } else
                        return QVariant();
                } else
                    return count;
            }
        }
    } else if (index.column() == TypeInfoColumn && (d_item_view_column_flags && Observer::TypeInfoColumn)) {
        if (role == Qt::DisplayRole) {
            QObject* obj = getObject(index);
            if (obj) {
                if (obj->metaObject()) {
                    if (QString(QLatin1String(obj->metaObject()->className())).split("::").last() != "QObject")
                        return QString(QLatin1String(obj->metaObject()->className())).split("::").last();
                }
            }
            return QVariant();
        }
    } else if (index.column() == AccessColumn && (d_item_view_column_flags && Observer::AccessColumn)) {
        if (role == Qt::DecorationRole) {
            QObject* obj = getObject(index);
            Observer* observer = qobject_cast<Observer*> (obj);
            if (!observer) {
                // Handle the case where the child is the parent of an observer
                foreach (QObject* child, obj->children()) {
                    observer = qobject_cast<Observer*> (child);
                    if (observer)
                        break;
                }
            }

            if (observer) {
                if (observer->accessModeScope() == Observer::GlobalScope) {
                    if (observer->accessMode() == Observer::FullAccess)
                        return QVariant();
                    if (observer->accessMode() == Observer::ReadOnlyAccess)
                        return QIcon(ICON_READ_ONLY);
                    if (observer->accessMode() == Observer::LockedAccess)
                        return QIcon(ICON_LOCKED);
                } else {
                    // Inspect the object to see if it has the OBJECT_ACCESS_MODE observer property.
                    QVariant mode = d_observer->getObserverPropertyValue(obj,OBJECT_ACCESS_MODE);
                    if (mode.toInt() == (int) Observer::ReadOnlyAccess)
                        return QIcon(ICON_READ_ONLY);
                    if (mode.toInt() == Observer::LockedAccess)
                        return QIcon(ICON_LOCKED);
                }
            } else {
                // Inspect the object to see if it has the OBJECT_ACCESS_MODE observer property.
                QVariant mode = d_observer->getObserverPropertyValue(obj,OBJECT_ACCESS_MODE);
                if (mode.toInt() == (int) Observer::ReadOnlyAccess)
                    return QIcon(ICON_READ_ONLY);
                if (mode.toInt() == Observer::LockedAccess)
                    return QIcon(ICON_LOCKED);
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags Qtilities::CoreGui::ObserverTreeModel::flags(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;
     ObserverTreeItem* item = getItem(index);

     if (item->itemType() == ObserverTreeItem::CategoryItem) {
         item_flags &= ~Qt::ItemIsEditable;
         item_flags &= ~Qt::ItemIsUserCheckable;

         if (d_item_selection_control == Observer::SelectableItems)
             item_flags |= Qt::ItemIsSelectable;
         else
             item_flags &= ~Qt::ItemIsSelectable;
     } else {
         if (d_naming_control == Observer::EditableNames)
             item_flags |= Qt::ItemIsEditable;
         else
             item_flags &= ~Qt::ItemIsEditable;

         if (d_item_selection_control == Observer::SelectableItems)
             item_flags |= Qt::ItemIsSelectable;
         else
             item_flags &= ~Qt::ItemIsSelectable;

         if (d_activity_control == Observer::CheckboxTriggered)
             item_flags |= Qt::ItemIsUserCheckable;
         else
             item_flags &= ~Qt::ItemIsUserCheckable;
     }

     return item_flags;
}

QVariant Qtilities::CoreGui::ObserverTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if ((section == NameColumn) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return d->rootItem->data(section);
    } else if ((section == ChildCountColumn) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        if (d_item_view_column_flags & Observer::ChildCountColumn)
            return QIcon(ICON_CHILD_COUNT);
    } else if ((section == ChildCountColumn) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (d_item_view_column_flags & Observer::ChildCountColumn)
            return tr("Child Count");
    } else if ((section == TypeInfoColumn) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        if (d_item_view_column_flags & Observer::TypeInfoColumn)
            return QIcon(ICON_TYPE_INFO);
    } else if ((section == TypeInfoColumn) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (d_item_view_column_flags & Observer::TypeInfoColumn)
            return tr("Type");
    } else if ((section == AccessColumn) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        if (d_item_view_column_flags & Observer::AccessColumn)
            return QIcon(ICON_ACCESS);
    } else if ((section == AccessColumn) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (d_item_view_column_flags & Observer::AccessColumn)
            return tr("Access");
    }

     return QVariant();
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::index(int row, int column, const QModelIndex &parent) const {
     if (!hasIndex(row, column, parent) || !d->rootItem)
         return QModelIndex();

     ObserverTreeItem *parentItem;

     if (!parent.isValid())
         parentItem = d->rootItem;
     else
         parentItem = static_cast<ObserverTreeItem*>(parent.internalPointer());

     ObserverTreeItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::parent(const QModelIndex &index) const {
     if (!index.isValid())
         return QModelIndex();

     ObserverTreeItem *childItem = getItem(index);
     if (!childItem)
         return QModelIndex();
     ObserverTreeItem *parentItem = childItem->parent();
     if (!parentItem)
         return QModelIndex();
     if (!parentItem->getObject())
         return QModelIndex();

     if (parentItem == d->rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
}

bool Qtilities::CoreGui::ObserverTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.column() == 0) {
        if (role == Qt::EditRole || role == Qt::DisplayRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            ObserverTreeItem* parent_item = item->parent();
            QObject* obj = item->getObject();
            Observer* parent_observer = 0;

            if (parent_item)
                parent_observer = qobject_cast<Observer*> (parent_item->getObject());

            if (!parent_observer)
                parent_observer = d_observer;

            // Check if the object has an OBJECT_NAME property, if not we set the name using setObjectName()
            if (parent_observer->getSharedProperty(obj, OBJECT_NAME).isValid()) {
                // Now check if this observer uses an instance name
                ObserverProperty instance_names = parent_observer->getObserverProperty(obj,INSTANCE_NAMES);
                if (instance_names.isValid() && instance_names.hasContext(d_observer->observerID()))
                    parent_observer->setObserverPropertyValue(obj,INSTANCE_NAMES,value);
                else
                    parent_observer->setObserverPropertyValue(obj,OBJECT_NAME,value);
            } else {
                obj->setObjectName(value.toString());
            }

            return true;
        } else if (role == Qt::CheckStateRole) {         
            if (d_activity_display == Observer::CheckboxActivityDisplay && d_activity_control == Observer::CheckboxTriggered) {
                ObserverTreeItem* item = getItem(index);
                Q_ASSERT(item);
                ObserverTreeItem* parent_item = item->parent();
                QObject* obj = item->getObject();
                Observer* parent_observer = 0;

                if (parent_item)
                    parent_observer = qobject_cast<Observer*> (parent_item->getObject());

                if (!parent_observer)
                    parent_observer = d_observer;

                // The value comming in here is always Qt::Checked
                // We get the current check state from the OBJECT_ACTIVITY property and change that
                QVariant current_activity = parent_observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY);
                if (current_activity.toBool()) {
                    parent_observer->setObserverPropertyValue(obj,OBJECT_ACTIVITY,QVariant(false));
                } else {
                    parent_observer->setObserverPropertyValue(obj,OBJECT_ACTIVITY,QVariant(true));
                }
                return true;
            }
        }
    }
    return false;
}

int Qtilities::CoreGui::ObserverTreeModel::rowCount(const QModelIndex &parent) const {
     ObserverTreeItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = d->rootItem;
     else
         parentItem = static_cast<ObserverTreeItem*>(parent.internalPointer());

     return parentItem->childCount();
}

int Qtilities::CoreGui::ObserverTreeModel::columnCount(const QModelIndex &parent) const {
     if (parent.isValid())
         return TypeInfoColumn+1;
         //return static_cast<ObserverTreeItem*>(parent.internalPointer())->columnCount();
     else
         return d->rootItem->columnCount();
}

void Qtilities::CoreGui::ObserverTreeModel::rebuildTreeStructure(const QString& partial_modification_notifier) {
    QString sender_name = "No Sender";
    if (sender())
        sender_name = sender()->objectName();

    if (!(partial_modification_notifier.isEmpty() || partial_modification_notifier == "Hierarchy"))
        return;

    reset();
    deleteRootItem();
    QVector<QVariant> columns;
    columns.push_back(QString("Child Count"));
    columns.push_back(QString("Access"));
    columns.push_back(QString("Type Info"));
    columns.push_back(QString("Object Tree"));
    d->rootItem = new ObserverTreeItem(0,0,columns);
    d->rootItem->setObjectName("Root Item");
    ObserverTreeItem* top_level_observer_item = new ObserverTreeItem(d_observer,d->rootItem);
    d->rootItem->appendChild(top_level_observer_item);
    setupChildData(top_level_observer_item);
    //printStructure();
    emit layoutChanged();
}

Qtilities::Core::Observer* Qtilities::CoreGui::ObserverTreeModel::calculateSelectionParent(QModelIndexList index_list) {
    if (index_list.count() == 1) {
        d->selection_parent = 0;
        ObserverTreeItem* item = getItem(index_list.front());
        if (!item)
            return 0;

        if (item->itemType() != ObserverTreeItem::CategoryItem) {
            ObserverTreeItem* item_parent = item->parent();
            Q_ASSERT(item_parent);
            d->selection_parent = qobject_cast<Observer*> (item_parent->getObject());

            // Handle the cases where the parent is a category item
            if (!d->selection_parent) {
                if (item_parent->itemType() == ObserverTreeItem::CategoryItem) {
                    d->selection_parent = item_parent->containedObserver();
                }
            }
        } else {
            d->selection_parent = item->containedObserver();
        }

        emit selectionParentChanged(d->selection_parent);
        return d->selection_parent;
    } else
        return 0;
}

int Qtilities::CoreGui::ObserverTreeModel::getSubjectID(const QModelIndex &index) const {
    QObject* obj = getItem(index)->getObject();
    if (d_observer)
        return d_observer->getObserverPropertyValue(obj,OBSERVER_SUBJECT_IDS).toInt();
    else
        return -1;
}

QObject* Qtilities::CoreGui::ObserverTreeModel::getObject(const QModelIndex &index) const {
    ObserverTreeItem* item = getItem(index);
    if (item)
        return item->getObject();
    else
        return 0;
}

void Qtilities::CoreGui::ObserverTreeModel::setupChildData(ObserverTreeItem* item) {
    // In here we build the complete structure of all the children below item.
    Observer* observer = qobject_cast<Observer*> (item->getObject());
    ObserverTreeItem* new_item;

    if (!observer && item->getObject()) {
        // Handle cases where a non-observer based child is the parent of an observer.
        // An example of this is a variable in QtSF, where the requiement is an QObject based interface, thus
        // the object can't inherit from Observer directly. It can but it defeats the purpose of an
        // abstract interface which needs to hide the actual implementation. In such cases use the
        // observer class through containment and make the interface implementation it's parent.
        foreach (QObject* child, item->getObject()->children()) {
            Observer* child_observer = qobject_cast<Observer*> (child);
            if (child_observer)
                observer = child_observer;
        }
    }

    if (!observer && item->getObject()) {
        // Handle cases where the item is a category item
        if (item->itemType() == ObserverTreeItem::CategoryItem) {
            // Get the observer from the parent of item
            if (item->parent()) {
                Observer* parent_observer = item->containedObserver();
                if (parent_observer) {
                    // Now add all items belonging to this category
                    for (int i = 0; i < parent_observer->subjectReferencesByCategory(item->category()).count(); i++) {
                        // Storing all information in the data vector here can improve performance
                        QVector<QVariant> column_data;
                        column_data << QVariant(parent_observer->subjectNamesByCategory(item->category()).at(i));
                        new_item = new ObserverTreeItem(parent_observer->subjectReferencesByCategory(item->category()).at(i),item,column_data);
                        item->appendChild(new_item);
                        setupChildData(new_item);
                    }
                }
            }
        }
    }

    if (observer) {
        // If this observer is locked we don't show its children
        if (observer->accessMode() != Observer::LockedAccess) {
            // Check the HierarhicalDisplay hint of the observer
            if (observer->hierarchicalDisplayHint() == Observer::CategorizedHierarchy) {
                // Create items for each each category
                foreach (QString category, observer->subjectCategories()) {
                    // Check the category against the displayed category list
                    bool valid_category = true;
                    if (observer->categoryFilterEnabled()) {
                        if (observer->hasInversedCategoryDisplay()) {
                            if (!observer->displayedCategories().contains(category))
                                valid_category = true;
                            else
                                valid_category = false;
                        } else {
                            if (observer->displayedCategories().contains(category))
                                valid_category = true;
                            else
                                valid_category = false;
                        }
                    }

                    // Only add valid categories
                    if (valid_category) {
                        QVector<QVariant> category_columns;
                        category_columns << category;
                        QObject* category_item = new QObject();
                        SharedObserverProperty icon_property(QIcon(ICON_FOLDER_16X16),OBJECT_ICON);
                        observer->setSharedProperty(category_item,icon_property);
                        SharedObserverProperty access_mode_property((int) observer->accessMode(category),OBJECT_ACCESS_MODE);
                        observer->setSharedProperty(category_item,access_mode_property);
                        if (category.isEmpty())
                            category == tr("Uncategorized");
                        category_item->setObjectName(category);
                        new_item = new ObserverTreeItem(category_item,item,category_columns,ObserverTreeItem::CategoryItem);
                        new_item->setContainedObserver(observer);
                        new_item->setCategory(category);
                        item->appendChild(new_item);
                        if (observer->accessMode(category) != Observer::LockedAccess)
                            setupChildData(new_item);
                    }
                }
            } else {
                for (int i = 0; i < observer->subjectCount(); i++) {
                    // Storing all information in the data vector here can improve performance
                    QVector<QVariant> column_data;
                    column_data << QVariant(observer->subjectNames().at(i));
                    new_item = new ObserverTreeItem(observer->subjectAt(i),item,column_data);
                    item->appendChild(new_item);
                    setupChildData(new_item);
                }
            }
        }
    }
}

Qtilities::Core::Observer* Qtilities::CoreGui::ObserverTreeModel::selectionParent() const {
    return d->selection_parent;
}

Qtilities::Core::Observer* Qtilities::CoreGui::ObserverTreeModel::parentOfIndex(const QModelIndex &index) const {
    ObserverTreeItem* item = getItem(index);
    if (!item)
        return 0;

    ObserverTreeItem* parent_item = item->parent();
    Observer* parent_observer = 0;

    if (parent_item)
        parent_observer = qobject_cast<Observer*> (parent_item->getObject());

    return parent_observer;
}

void Qtilities::CoreGui::ObserverTreeModel::useObserverHints(const Observer* observer) {
    if (!d->use_observer_hints)
        return;

    if (observer) {
        // Check if the specified observer provides any hints
        if (observer->namingControlHint() != Observer::NoNamingControlHint)
            d_naming_control = observer->namingControlHint();
        if (observer->activityDisplayHint() != Observer::NoActivityDisplayHint)
            d_activity_display = observer->activityDisplayHint();
        if (observer->activityControlHint() != Observer::NoActivityControlHint)
            d_activity_control = observer->activityControlHint();
        if (observer->itemSelectionControlHint() != Observer::NoItemSelectionControlHint)
            d_item_selection_control = observer->itemSelectionControlHint();
        if (observer->itemViewColumnFlags() != Observer::NoItemViewColumnHint)
            d_item_view_column_flags = observer->itemViewColumnFlags();
    } else if (d_observer) {
        // Check if the specified observer provides any hints
        if (d_observer->namingControlHint() != Observer::NoNamingControlHint)
            d_naming_control = d_observer->namingControlHint();
        if (d_observer->activityDisplayHint() != Observer::NoActivityDisplayHint)
            d_activity_display = d_observer->activityDisplayHint();
        if (d_observer->activityControlHint() != Observer::NoActivityControlHint)
            d_activity_control = d_observer->activityControlHint();
        if (d_observer->itemSelectionControlHint() != Observer::NoItemSelectionControlHint)
            d_item_selection_control = d_observer->itemSelectionControlHint();
        if (d_observer->itemViewColumnFlags() != Observer::NoItemViewColumnHint)
            d_item_view_column_flags = d_observer->itemViewColumnFlags();
    }
}

void Qtilities::CoreGui::ObserverTreeModel::deleteRootItem() {
    if (!d->rootItem)
        return;

    delete d->rootItem;
    d->rootItem = 0;
}

/*!void Qtilities::CoreGui::ObserverTreeModel::disconnectObserver(Observer* observer) {
    if (!observer)
        return;

    Observer* child_observer;
    observer->disconnect(this);
    for (int i = 0; i < observer->subjectCount(); i++) {
        child_observer = qobject_cast<Observer*> (observer->subjectAt(i));
        if (child_observer)
            disconnectObserver(child_observer);
        else {
            // Handle the case where a child is the parent of an observer
            foreach (QObject* child, observer->subjectAt(i)->children()) {
                child_observer = qobject_cast<Observer*> (child);
                if (child_observer)
                    disconnectObserver(child_observer);
            }
        }
    }
}*/

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::ObserverTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        ObserverTreeItem *item = static_cast<ObserverTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }

    return 0;
}

void Qtilities::CoreGui::ObserverTreeModel::resetModel() {
    reset();
    rebuildTreeStructure();
    emit layoutChanged();
}

void Qtilities::CoreGui::ObserverTreeModel::handleObserverContextDeleted() {
   d_observer = 0;
   resetModel();
}

void Qtilities::CoreGui::ObserverTreeModel::printStructure(ObserverTreeItem* item, int level) {
    if (level == 0) {
        item = d->rootItem;
        LOG_TRACE(QString("Tree Debug (%1): Object = %2, Parent = None, Child Count = %3").arg(level).arg(item->objectName()).arg(item->childCount()));
    } else
        LOG_TRACE(QString("Tree Debug (%1): Object = %2, Parent = %3, Child Count = %4").arg(level).arg(item->objectName()).arg(item->parent()->objectName()).arg(item->childCount()));

    for (int i = 0; i < item->childCount(); i++) {
        printStructure(item->child(i),level+1);
    }
}
