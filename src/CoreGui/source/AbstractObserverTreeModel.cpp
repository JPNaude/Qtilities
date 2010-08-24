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

#include "AbstractObserverTreeModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <SubjectTypeFilter.h>
#include <QtilitiesCoreConstants.h>
#include <Observer.h>
#include <ActivityPolicyFilter.h>
#include <Logger.h>

#include <QIcon>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

struct Qtilities::CoreGui::AbstractObserverTreeModelData {
    ObserverTreeItem*   rootItem;
    Observer*           selection_parent;
    QModelIndex         selection_index;
};

Qtilities::CoreGui::AbstractObserverTreeModel::AbstractObserverTreeModel(QObject* parent) : QAbstractItemModel(parent), AbstractObserverItemModel()
{
    d = new AbstractObserverTreeModelData;

    // Init root data
    d->rootItem = new ObserverTreeItem();
    d->selection_parent = 0;
}

bool Qtilities::CoreGui::AbstractObserverTreeModel::setObserverContext(Observer* observer) {
    if (!observer)
        return false;

    if (d_observer) {
        d_observer->disconnect(this);
        reset();
    }

    if (!AbstractObserverItemModel::setObserverContext(observer))
        return false;

    rebuildTreeStructure();

    connect(d_observer,SIGNAL(destroyed()),SLOT(handleObserverContextDeleted()));
    connect(d_observer,SIGNAL(layoutChanged()),SLOT(rebuildTreeStructure()));
    connect(d_observer,SIGNAL(dataChanged(Observer*)),SLOT(handleContextDataChanged(Observer*)));

    return true;
}

int Qtilities::CoreGui::AbstractObserverTreeModel::columnPosition(AbstractObserverItemModel::ColumnID column_id) const {
    if (column_id == AbstractObserverItemModel::ColumnName) {
        return 0;
    } else if (column_id == AbstractObserverItemModel::ColumnChildCount) {
        return 1;
    } else if (column_id == AbstractObserverItemModel::ColumnAccess) {
        return 2;
    } else if (column_id == AbstractObserverItemModel::ColumnTypeInfo) {
        return 3;
    } else if (column_id == AbstractObserverItemModel::ColumnCategory) {
        return -1;
    } else if (column_id == AbstractObserverItemModel::ColumnSubjectID) {
        return -1;
    } else if (column_id == AbstractObserverItemModel::ColumnLast) {
        return 3;
    }

    return -1;
}

int Qtilities::CoreGui::AbstractObserverTreeModel::columnVisiblePosition(AbstractObserverItemModel::ColumnID column_id) const {
    int start;
    if (column_id == AbstractObserverItemModel::ColumnName) {
        start = 0;
    } else if (column_id == AbstractObserverItemModel::ColumnChildCount) {
        start = 1;
    } else if (column_id == AbstractObserverItemModel::ColumnAccess) {
        start = 2;
    } else if (column_id == AbstractObserverItemModel::ColumnTypeInfo) {
        start = 3;
    } else if (column_id == AbstractObserverItemModel::ColumnCategory) {
        return -1;
    } else if (column_id == AbstractObserverItemModel::ColumnSubjectID) {
        return -1;
    } else if (column_id == AbstractObserverItemModel::ColumnLast) {
        start = 4;
    }

    // Now we need to subtract hidden columns from current:
    int current = start;
    if (activeHints()) {
        if (start == 1) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnNameHint))
                return -1;
        }
        if (start > 1) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint))
                --current;
        }
        if (start > 2) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint))
                --current;
        }
        if (start > 3) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
                --current;
        }
        if (start == 4) {
            --current;
        }
    }

    return current;
}

QStack<int> Qtilities::CoreGui::AbstractObserverTreeModel::getParentHierarchy(const QModelIndex& index) const {
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

QVariant Qtilities::CoreGui::AbstractObserverTreeModel::dataHelper(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (!d_observer)
        return QVariant();

    // ------------------------------------
    // Handle Name Column
    // ------------------------------------
    if (index.column() == columnPosition(ColumnName)) {
        // ------------------------------------
        // Qt::DisplayRole and Qt::EditRole
        // ------------------------------------
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return "Invalid Item";
            QObject* obj = item->getObject();
            if (obj) {
                // If this is a category item we just use objectName:
                if (item->itemType() == ObserverTreeItem::CategoryItem)
                    return item->objectName();
                else {
                    // Otherwise we must get the name of the object in the parent observer's context:
                    // First get the parent observer:
                    Observer* obs = 0;
                    if (item->parent()) {
                        if (item->parent()->itemType() == ObserverTreeItem::CategoryItem) {
                            obs = item->parent()->containedObserver();
                        } else {
                            obs = qobject_cast<Observer*> (item->parent()->getObject());
                        }
                    }
                    // If observer is valid, we get the name, otherwise we just use object name.
                    if (obs)
                        return obs->subjectNameInContext(obj);
                    else
                        return obj->objectName();
                }
            } else
                return tr("Invalid object");
        // ------------------------------------
        // Qt::CheckStateRole
        // ------------------------------------
        } else if (role == Qt::CheckStateRole) {
            // In here we need to manually get the top level of each index since the active context is
            // not representitive of all indexes we get in here:
            QObject* obj = getObject(index);
            QVariant subject_activity = QVariant();
            Observer* local_selection_parent = parentOfIndex(index);

            // Once we have the local parent, we can check if it must display activity and if so, we return
            // the activity of obj in that context.
            if (local_selection_parent) {
                if (local_selection_parent->displayHints()) {
                    if (local_selection_parent->displayHints()->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay) {
                        // Now we need to check if the observer has an activity policy filter installed
                        ActivityPolicyFilter* activity_filter = 0;
                        for (int i = 0; i < local_selection_parent->subjectFilters().count(); i++) {
                            activity_filter = qobject_cast<ActivityPolicyFilter*> (local_selection_parent->subjectFilters().at(i));
                            if (activity_filter) {
                                subject_activity = local_selection_parent->getObserverPropertyValue(obj,OBJECT_ACTIVITY);
                            }
                        }

                    }
                }
            }

            if (subject_activity.isValid())
                return subject_activity.toBool();
            else
                return QVariant();
        // ------------------------------------
        // Qt::DecorationRole
        // ------------------------------------
        } else if (role == Qt::DecorationRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_DECORATION);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::WhatsThisRole
        // ------------------------------------
        } else if (role == Qt::WhatsThisRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_WHATS_THIS);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::SizeHintRole
        // ------------------------------------
        } else if (role == Qt::SizeHintRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_SIZE_HINT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::StatusTipRole
        // ------------------------------------
        } else if (role == Qt::StatusTipRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_STATUSTIP);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::FontRole
        // ------------------------------------
        } else if (role == Qt::FontRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_FONT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::TextAlignmentRole
        // ------------------------------------
        } else if (role == Qt::TextAlignmentRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_TEXT_ALIGNMENT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::BackgroundRole
        // ------------------------------------
        } else if (role == Qt::BackgroundRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_BACKGROUND);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::ForegroundRole
        // ------------------------------------
        } else if (role == Qt::ForegroundRole) {
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = Observer::getSharedProperty(obj,OBJECT_ROLE_FOREGROUND);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::ToolTipRole
        // ------------------------------------
        } else if (role == Qt::ToolTipRole) {
            // Check if the object has an OBJECT_TOOLTIP shared property to show.
            ObserverTreeItem* item = getItem(index);
            Q_ASSERT(item);
            QObject* obj = item->getObject();
            SharedObserverProperty tooltip = Observer::getSharedProperty(obj,OBJECT_ROLE_TOOLTIP);
            if (tooltip.isValid()) {
                return tooltip.value();
            }
        }
    // ------------------------------------
    // Handle Child Count Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnChildCount)) {
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
    // ------------------------------------
    // Handle Subject Type Info Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnTypeInfo) && (activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint)) {
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
    // ------------------------------------
    // Handle Access Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnAccess) && (activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint)) {
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

Qt::ItemFlags Qtilities::CoreGui::AbstractObserverTreeModel::flagsHelper(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;
     ObserverTreeItem* item = getItem(index);
     if (!item)
         return item_flags;

     // Selectable Items hint always uses the top level observer for good reason. Otherwise
     // we end up in situation where one part of tree can be selected and other part not.
     // When we select a part which is not selectable then, we can't select the part which must
     // be selectable.
     if (model->hints_top_level_observer) {
         if (model->hints_top_level_observer->itemSelectionControlHint() == ObserverHints::SelectableItems)
             item_flags |= Qt::ItemIsSelectable;
         else
             item_flags &= ~Qt::ItemIsSelectable;
     } else {
         if (model->hints_default->itemSelectionControlHint() == ObserverHints::SelectableItems)
             item_flags |= Qt::ItemIsSelectable;
         else
             item_flags &= ~Qt::ItemIsSelectable;
     }

     // Handle category items
     if (item->itemType() == ObserverTreeItem::CategoryItem) {
         item_flags &= ~Qt::ItemIsEditable;
         item_flags &= ~Qt::ItemIsUserCheckable;
     }

     // Handle object items
     if (item->itemType() == ObserverTreeItem::ObjectItem) {
         // The naming control hint we get from the active hints since the user must click
         // in order to edit the name. The click will update activeHints() for us.
         if (activeHints()->namingControlHint() == ObserverHints::EditableNames)
             item_flags |= Qt::ItemIsEditable;
         else
             item_flags &= ~Qt::ItemIsEditable;

         // For the activity display we need to manually get the top level of each index
         // since the active context is not representitive of all indexes we get in here:
         Observer* local_selection_parent = parentOfIndex(index);
         if (local_selection_parent) {
             if (local_selection_parent->displayHints()) {
                 if (local_selection_parent->displayHints()->activityControlHint() == ObserverHints::CheckboxTriggered)
                     item_flags |= Qt::ItemIsUserCheckable;
                 else
                     item_flags &= ~Qt::ItemIsUserCheckable;
             } else {
                 item_flags &= ~Qt::ItemIsUserCheckable;
             }
         } else {
             item_flags &= ~Qt::ItemIsUserCheckable;
         }
     }

     return item_flags;
}

QVariant Qtilities::CoreGui::AbstractObserverTreeModel::headerDataHelper(int section, Qt::Orientation orientation, int role) const
{
    if ((section == columnPosition(ColumnName)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return tr("Contents Tree");
    } else if ((section == columnPosition(ColumnChildCount)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint)
            return QIcon(ICON_CHILD_COUNT);
    } else if ((section == columnPosition(ColumnChildCount)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint)
            return tr("Child Count");
    } else if ((section == columnPosition(ColumnTypeInfo)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint)
            return QIcon(ICON_TYPE_INFO);
    } else if ((section == columnPosition(ColumnTypeInfo)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint)
            return tr("Type");
    } else if ((section == columnPosition(ColumnAccess)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint)
            return QIcon(ICON_ACCESS);
    } else if ((section == columnPosition(ColumnAccess)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint)
            return tr("Access");
    }

     return QVariant();
}

QModelIndex Qtilities::CoreGui::AbstractObserverTreeModel::index(int row, int column, const QModelIndex &parent) const {
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

QModelIndex Qtilities::CoreGui::AbstractObserverTreeModel::parent(const QModelIndex &index) const {
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

bool Qtilities::CoreGui::AbstractObserverTreeModel::setDataHelper(const QModelIndex &set_data_index, const QVariant &value, int role) {
    if (set_data_index.column() == columnPosition(AbstractObserverItemModel::ColumnName)) {
        if (role == Qt::EditRole || role == Qt::DisplayRole) {
            ObserverTreeItem* item = getItem(set_data_index);
            QObject* obj = item->getObject();
            Observer* local_selection_parent = d->selection_parent;

            if (!local_selection_parent)
                return false;

            // Check if the object has an OBJECT_NAME property, if not we set the name using setObjectName()
            if (obj) {
                if (local_selection_parent->getSharedProperty(obj, OBJECT_NAME).isValid()) {
                    // Now check if this observer uses an instance name
                    ObserverProperty instance_names = Observer::getObserverProperty(obj,INSTANCE_NAMES);
                    if (instance_names.isValid() && instance_names.hasContext(local_selection_parent->observerID()))
                        local_selection_parent->setObserverPropertyValue(obj,INSTANCE_NAMES,value);
                    else
                        local_selection_parent->setObserverPropertyValue(obj,OBJECT_NAME,value);
                } else {
                    obj->setObjectName(value.toString());
                }
            }

            // We cannot emit dataChanged(index,index) here since changing the name might do a replace
            // and then the persistant indexes are out of date.
            return true;
        } else if (role == Qt::CheckStateRole) {
            ObserverTreeItem* item = getItem(set_data_index);
            QObject* obj = item->getObject();
            Observer* local_selection_parent = parentOfIndex(set_data_index);

            if (local_selection_parent) {
                if (local_selection_parent->displayHints()) {
                    if (local_selection_parent->displayHints()->activityControlHint() == ObserverHints::CheckboxTriggered && local_selection_parent->displayHints()->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay) {
                        // Now we need to check if the observer has an activity policy filter installed
                        ActivityPolicyFilter* activity_filter = 0;
                        for (int i = 0; i < local_selection_parent->subjectFilters().count(); i++) {
                            activity_filter = qobject_cast<ActivityPolicyFilter*> (local_selection_parent->subjectFilters().at(i));
                            if (activity_filter) {
                                // The value comming in here is always Qt::Checked.
                                // We get the current check state from the OBJECT_ACTIVITY property and change that.
                                QVariant current_activity = local_selection_parent->getObserverPropertyValue(obj,OBJECT_ACTIVITY);
                                if (current_activity.toBool()) {
                                    local_selection_parent->setObserverPropertyValue(obj,OBJECT_ACTIVITY,QVariant(false));
                                } else {
                                    local_selection_parent->setObserverPropertyValue(obj,OBJECT_ACTIVITY,QVariant(true));
                                }

                                handleContextDataChanged(local_selection_parent);
                            }
                        }
                    }
                }
            }
            return true;
        }
    }
    return false;
}

int Qtilities::CoreGui::AbstractObserverTreeModel::rowCountHelper(const QModelIndex &parent) const {
     ObserverTreeItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = d->rootItem;
     else
         parentItem = static_cast<ObserverTreeItem*>(parent.internalPointer());

     return parentItem->childCount();
}

int Qtilities::CoreGui::AbstractObserverTreeModel::columnCountHelper(const QModelIndex &parent) const {
    if (parent.isValid()) {
         return columnPosition(AbstractObserverItemModel::ColumnLast) + 1;
     } else
         return d->rootItem->columnCount();
}

void Qtilities::CoreGui::AbstractObserverTreeModel::rebuildTreeStructure() {
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
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

Qtilities::Core::Observer* Qtilities::CoreGui::AbstractObserverTreeModel::calculateSelectionParent(QModelIndexList index_list) {
    if (index_list.count() == 1) {
        d->selection_parent = parentOfIndex(index_list.front());
        d->selection_index = index_list.front();

        // Get the hints from the observer:
        if (d->selection_parent)
            inheritObserverHints(d->selection_parent);

        emit selectionParentChanged(d->selection_parent);
        return d->selection_parent;
    } else {
        return 0;
    }
}

int Qtilities::CoreGui::AbstractObserverTreeModel::getSubjectID(const QModelIndex &index) const {
    QObject* obj = getItem(index)->getObject();
    if (d_observer)
        return d_observer->getObserverPropertyValue(obj,OBSERVER_SUBJECT_IDS).toInt();
    else
        return -1;
}

QObject* Qtilities::CoreGui::AbstractObserverTreeModel::getObject(const QModelIndex &index) const {
    ObserverTreeItem* item = getItem(index);
    if (item)
        return item->getObject();
    else
        return 0;
}

void Qtilities::CoreGui::AbstractObserverTreeModel::setupChildData(ObserverTreeItem* item) {
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
            bool flat_structure = true;
            // Check the HierarchicalDisplay hint of the observer
            if (observer->displayHints()) {
                if (observer->displayHints()->hierarchicalDisplayHint() == ObserverHints::CategorizedHierarchy) {
                    // Create items for each each category
                    foreach (QString category, observer->subjectCategories()) {
                        // Check the category against the displayed category list
                        bool valid_category = true;
                        if (observer->displayHints()->categoryFilterEnabled()) {
                            if (observer->displayHints()->hasInversedCategoryDisplay()) {
                                if (!observer->displayHints()->displayedCategories().contains(category))
                                    valid_category = true;
                                else
                                    valid_category = false;
                            } else {
                                if (observer->displayHints()->displayedCategories().contains(category))
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
                            SharedObserverProperty icon_property(QIcon(ICON_FOLDER_16X16),OBJECT_ROLE_DECORATION);
                            observer->setSharedProperty(category_item,icon_property);
                            SharedObserverProperty access_mode_property((int) observer->accessMode(category),OBJECT_ACCESS_MODE);
                            observer->setSharedProperty(category_item,access_mode_property);
                            if (category.isEmpty())
                                category == QString(OBSERVER_UNCATEGORIZED_CATEGORY);
                            category_item->setObjectName(category);
                            new_item = new ObserverTreeItem(category_item,item,category_columns,ObserverTreeItem::CategoryItem);
                            new_item->setContainedObserver(observer);
                            new_item->setCategory(category);
                            item->appendChild(new_item);
                            if (observer->accessMode(category) != Observer::LockedAccess)
                                setupChildData(new_item);
                            flat_structure = false;
                        }
                    }
                }
            }

            if (flat_structure) {
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

Qtilities::Core::Observer* Qtilities::CoreGui::AbstractObserverTreeModel::selectionParent() const {
    return d->selection_parent;
}

Qtilities::Core::Observer* Qtilities::CoreGui::AbstractObserverTreeModel::parentOfIndex(const QModelIndex &index) const {
    Observer* local_selection_parent = 0;
    ObserverTreeItem* item = getItem(index);
    if (!item)
        return 0;

    if (item->itemType() != ObserverTreeItem::CategoryItem) {
        ObserverTreeItem* item_parent = item->parent();
        if (!item_parent)
            return 0;
        local_selection_parent = qobject_cast<Observer*> (item_parent->getObject());

        if (!local_selection_parent) {
            // Handle the cases where the parent is a category item
            if (item_parent->itemType() == ObserverTreeItem::CategoryItem) {
                local_selection_parent = item_parent->containedObserver();
            }

            // Handle the cases where the parent is an object which has this object's parent observer as its child.
            if (!local_selection_parent) {
                if (item_parent->getObject()) {
                    for (int i = 0; i < item_parent->getObject()->children().count(); i++) {
                        local_selection_parent = qobject_cast<Observer*> (item_parent->getObject()->children().at(i));
                        if (local_selection_parent)
                            break;
                    }
                }
            }
        }
    } else {
        local_selection_parent = item->containedObserver();
    }
    return local_selection_parent;  
}

void Qtilities::CoreGui::AbstractObserverTreeModel::handleContextDataChanged(Observer* observer) {
    if (!observer)
        return;

    // If the observer is the current selection parent, we call handleContextDataChanged(d->selection_index)
    // since the selection will be in this parent context and that function will refresh the complete context.
    if (observer == d->selection_parent) {
        handleContextDataChanged(d->selection_index);
        // Not sure why this refreshes the complete tree at present.
        // It is probably because observers context to each other's dataChanged() signals.
    }
}

void Qtilities::CoreGui::AbstractObserverTreeModel::handleContextDataChanged(const QModelIndex &set_data_index) {
    // We get the indexes for the complete context since activity of many objects might change:
    // Warning: This is not going to work for categorized hierarchy observers.
    QModelIndex parent_index = parent(set_data_index);
    int column_count = columnVisiblePosition(ColumnLast);
    int row_count = rowCount(parent_index) - 1;
    QModelIndex top_left = index(0,0,parent_index);
    QModelIndex bottom_right = index(row_count,column_count,parent_index);
    if (hasIndex(row_count,column_count,parent_index))
        emit dataChanged(top_left,bottom_right);
}

void Qtilities::CoreGui::AbstractObserverTreeModel::deleteRootItem() {
    if (!d->rootItem)
        return;

    delete d->rootItem;
    d->rootItem = 0;
}

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::AbstractObserverTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        ObserverTreeItem *item = static_cast<ObserverTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }

    return 0;
}

void Qtilities::CoreGui::AbstractObserverTreeModel::handleObserverContextDeleted() {
    reset();
    rebuildTreeStructure();
    emit layoutChanged();
}

void Qtilities::CoreGui::AbstractObserverTreeModel::printStructure(ObserverTreeItem* item, int level) {
    if (level == 0) {
        item = d->rootItem;
        LOG_TRACE(QString("Tree Debug (%1): Object = %2, Parent = None, Child Count = %3").arg(level).arg(item->objectName()).arg(item->childCount()));
    } else
        LOG_TRACE(QString("Tree Debug (%1): Object = %2, Parent = %3, Child Count = %4").arg(level).arg(item->objectName()).arg(item->parent()->objectName()).arg(item->childCount()));

    for (int i = 0; i < item->childCount(); i++) {
        printStructure(item->child(i),level+1);
    }
}
