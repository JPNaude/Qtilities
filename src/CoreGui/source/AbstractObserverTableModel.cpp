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

#include "AbstractObserverTableModel.h"
#include "NamingPolicyFilter.h"
#include "ActivityPolicyFilter.h"
#include "QtilitiesCoreGuiConstants.h"

#include <SubjectTypeFilter.h>
#include <QtilitiesCoreConstants.h>
#include <Observer.h>

#include <QIcon>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

struct Qtilities::CoreGui::AbstractObserverTableModelData {
    AbstractObserverTableModelData() : type_grouping_name(QString()) { }

    QString type_grouping_name;
};

Qtilities::CoreGui::AbstractObserverTableModel::AbstractObserverTableModel(QObject* parent) : QAbstractTableModel(parent), AbstractObserverItemModel()
{
    d = new AbstractObserverTableModelData();
}

bool Qtilities::CoreGui::AbstractObserverTableModel::setObserverContext(Observer* observer) {
    if (d_observer)
        d_observer->disconnect(this);

    if (!AbstractObserverItemModel::setObserverContext(observer))
        return false;

    connect(d_observer,SIGNAL(layoutChanged()),SLOT(handleLayoutChanged()));
    connect(d_observer,SIGNAL(destroyed()),SLOT(handleLayoutChanged()));
    connect(d_observer,SIGNAL(dataChanged()),SLOT(handleDataChanged()));
    reset();

    // Check if this observer has a subject type filter installed
    for (int i = 0; i < observer->subjectFilters().count(); i++) {
        SubjectTypeFilter* subject_type_filter = qobject_cast<SubjectTypeFilter*> (observer->subjectFilters().at(i));
        if (subject_type_filter) {
            if (!subject_type_filter->groupName().isEmpty()) {
                d->type_grouping_name = subject_type_filter->groupName();
            }
            break;
        }
    }

    return true;
}

int Qtilities::CoreGui::AbstractObserverTableModel::columnPosition(AbstractObserverItemModel::ColumnID column_id) const {
    if (column_id == AbstractObserverItemModel::ColumnSubjectID) {
        return 0;
    } else if (column_id == AbstractObserverItemModel::ColumnName) {
        return 1;
    } else if (column_id == AbstractObserverItemModel::ColumnCategory) {
        return 2;
    } else if (column_id == AbstractObserverItemModel::ColumnChildCount) {
        return 3;
    } else if (column_id == AbstractObserverItemModel::ColumnAccess) {
        return 4;
    } else if (column_id == AbstractObserverItemModel::ColumnTypeInfo) {
        return 5;
    } else if (column_id == AbstractObserverItemModel::ColumnLast) {
        return 5;
    }
    return -1;
}

int Qtilities::CoreGui::AbstractObserverTableModel::columnVisiblePosition(AbstractObserverItemModel::ColumnID column_id) const {
    int start;
    if (column_id == AbstractObserverItemModel::ColumnSubjectID) {
        start = 0;
    } else if (column_id == AbstractObserverItemModel::ColumnName) {
        start = 1;
    } else if (column_id == AbstractObserverItemModel::ColumnCategory) {
        start = 2;
    } else if (column_id == AbstractObserverItemModel::ColumnChildCount) {
        start = 3;
    } else if (column_id == AbstractObserverItemModel::ColumnAccess) {
        start = 4;
    } else if (column_id == AbstractObserverItemModel::ColumnTypeInfo) {
        start = 5;
    } else if (column_id == AbstractObserverItemModel::ColumnLast) {
        start = 6;
    }

    // Now we need to subtract hidden columns from current:
    int current = start;
    if (activeHints()) {
        if (start == 0) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnIDHint))
                return -1;
        }
        if (start > 0) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnIDHint))
                --current;
        }
        if (start > 1) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnNameHint))
                --current;
        }
        if (start > 2) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnCategoryHint))
                --current;
        }
        if (start > 3) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint))
                --current;
        }
        if (start > 4) {
            if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
                --current;
        }
        if (start == 6) {
            --current;
        }
    }

    return current;
}

QVariant Qtilities::CoreGui::AbstractObserverTableModel::dataHelper(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (!d_observer)
        return QVariant();

    // ------------------------------------
    // Handle Subject ID Column
    // ------------------------------------
    if (index.column() == columnPosition(ColumnSubjectID)) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return d_observer->subjectID(index.row());
        }
    // ------------------------------------
    // Handle Name Column: We need to inspect all role properties here:
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnName)) {
        // ------------------------------------
        // Qt::DisplayRole and Qt::EditRole
        // ------------------------------------
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return d_observer->subjectNames().at(index.row());
        // ------------------------------------
        // Qt::CheckStateRole
        // ------------------------------------
        } else if (role == Qt::CheckStateRole) {
            if (model->activity_filter) {
                if (activeHints()->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay || activeHints()->activityControlHint() == ObserverHints::CheckboxTriggered) {
                    QObject* obj = d_observer->subjectReference(getSubjectID(index));
                    QVariant subject_activity = d_observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY);

                    Q_ASSERT(subject_activity.isValid());
                    return subject_activity.toBool();
                }
            }
        // ------------------------------------
        // Qt::DecorationRole
        // ------------------------------------
        } else if (role == Qt::DecorationRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ROLE_DECORATION);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::ForegroundRole
        // ------------------------------------
        } else if (role == Qt::ForegroundRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ROLE_FOREGROUND);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::BackgroundRole
        // ------------------------------------
        } else if (role == Qt::BackgroundRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ROLE_BACKGROUND);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::TextAlignmentRole
        // ------------------------------------
        } else if (role == Qt::TextAlignmentRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ROLE_TEXT_ALIGNMENT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::FontRole
        // ------------------------------------
        } else if (role == Qt::FontRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ROLE_FONT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::SizeHintRole
        // ------------------------------------
        } else if (role == Qt::SizeHintRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ROLE_SIZE_HINT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::WhatsThisRole
        // ------------------------------------
        } else if (role == Qt::WhatsThisRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ROLE_WHATS_THIS);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::ToolTipRole
        // ------------------------------------
        } else if (role == Qt::ToolTipRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty tooltip = d_observer->getSharedProperty(obj,OBJECT_ROLE_TOOLTIP);
            if (tooltip.isValid()) {
                return tooltip.value();
            }
        }
    // ------------------------------------
    // Handle Category Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnCategory) && (activeHints()->hierarchicalDisplayHint() & ObserverHints::CategorizedHierarchy)) {
        if (role == Qt::DisplayRole) {
            // Get the OBJECT_CATEGORY property.
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            QString category = d_observer->getObserverPropertyValue(obj,OBJECT_CATEGORY).toString();
            if (category.isEmpty())
                category = QString(OBSERVER_UNCATEGORIZED_CATEGORY);
            return category;
        }
    // ------------------------------------
    // Handle Child Count Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnChildCount) && (activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint)) {
        if (role == Qt::DisplayRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            Observer* observer = qobject_cast<Observer*> (obj);
            if (observer) {
                return observer->childCount();
            } else {
                // Handle the case where the child is the parent of an observer
                int count = 0;
                foreach (QObject* child, obj->children()) {
                    Observer* child_observer = qobject_cast<Observer*> (child);
                    if (child_observer)
                        count += child_observer->childCount();
                }

                if (count == 0)
                    return QVariant();
                else
                    return count;
            }

        }
    // ------------------------------------
    // Handle Subject Type Info Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnTypeInfo) && (activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint)) {
        if (role == Qt::DisplayRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            if (obj->metaObject()) {
                return QString(QLatin1String(obj->metaObject()->className())).split("::").last();
            }
            return QVariant();
        }
    // ------------------------------------
    // Handle Access Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnAccess) && (activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint)) {
        if (role == Qt::DecorationRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
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
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags Qtilities::CoreGui::AbstractObserverTableModel::flagsHelper(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;

     if (activeHints()->namingControlHint() == ObserverHints::EditableNames)
         item_flags |= Qt::ItemIsEditable;
     else
         item_flags &= ~Qt::ItemIsEditable;

     if (activeHints()->itemSelectionControlHint() == ObserverHints::SelectableItems)
         item_flags |= Qt::ItemIsSelectable;
     else
         item_flags &= ~Qt::ItemIsSelectable;

     if (activeHints()->activityControlHint() == ObserverHints::CheckboxTriggered)
         item_flags |= Qt::ItemIsUserCheckable;
     else
         item_flags &= ~Qt::ItemIsUserCheckable;

     return item_flags;
}

QVariant Qtilities::CoreGui::AbstractObserverTableModel::headerDataHelper(int section, Qt::Orientation orientation, int role) const {
    if ((section == columnPosition(ColumnSubjectID)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnIDHint)
            return tr("ID");
    } else if ((section == columnPosition(ColumnName)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (d->type_grouping_name.isEmpty())
            return tr("Items");
        else
            return d->type_grouping_name;
    } else if ((section == columnPosition(ColumnCategory)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnCategoryHint)
            return tr("Category");
    } else if ((section == columnPosition(ColumnCategory)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (activeHints()->itemViewColumnHint() & ObserverHints::ColumnCategoryHint)
            return tr("Category");
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

bool Qtilities::CoreGui::AbstractObserverTableModel::setDataHelper(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() == columnPosition(ColumnSubjectID)) {
        return false;
    } else if (index.column() == columnPosition(ColumnName)) {
        if (role == Qt::EditRole || role == Qt::DisplayRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            // Check if the object has an OBJECT_NAME property, if not we set the name using setObjectName()
            if (d_observer->getSharedProperty(obj, OBJECT_NAME).isValid()) {
                // Now check if this observer uses an instance name
                ObserverProperty instance_names = d_observer->getObserverProperty(obj,INSTANCE_NAMES);
                if (instance_names.isValid() && instance_names.hasContext(d_observer->observerID()))
                    d_observer->setObserverPropertyValue(obj,INSTANCE_NAMES,value);
                else
                    d_observer->setObserverPropertyValue(obj,OBJECT_NAME,value);
            } else {
                obj->setObjectName(value.toString());
            }
            return true;
        } else if (role == Qt::CheckStateRole) {
            if (model->activity_filter) {
                if (activeHints()->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay && activeHints()->activityControlHint() == ObserverHints::CheckboxTriggered) {
                    QObject* obj = d_observer->subjectReference(getSubjectID(index));
                    // The value coming in here is always Qt::Checked
                    // We get the current check state from the OBJECT_ACTIVITY property and change that:
                    QVariant current_activity = d_observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY);
                    if (current_activity.toBool()) {
                        d_observer->setObserverPropertyValue(obj,OBJECT_ACTIVITY,QVariant(false));
                    } else {
                        d_observer->setObserverPropertyValue(obj,OBJECT_ACTIVITY,QVariant(true));
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

int Qtilities::CoreGui::AbstractObserverTableModel::rowCountHelper(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    if (!d_observer)
        return 0;
    else {
        return d_observer->subjectCount();
    }
}

int Qtilities::CoreGui::AbstractObserverTableModel::columnCountHelper(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    if (!d_observer)
        return 0;
    else {
        return columnPosition(AbstractObserverItemModel::ColumnLast) + 1;
    }
}

void Qtilities::CoreGui::AbstractObserverTableModel::handleDataChanged() {
    emit dataChanged(index(0,0),index(rowCount(),columnCount()));
}

void Qtilities::CoreGui::AbstractObserverTableModel::handleLayoutChanged() {
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

int Qtilities::CoreGui::AbstractObserverTableModel::getSubjectID(const QModelIndex &index) const {
    QModelIndex id_index = createIndex(index.row(),0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok)
        return id;
    else
        return -1;
}

int Qtilities::CoreGui::AbstractObserverTableModel::getSubjectID(int row) const {
    QModelIndex id_index = createIndex(row,0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok)
        return id;
    else
        return -1;
}

QObject* Qtilities::CoreGui::AbstractObserverTableModel::getObject(const QModelIndex &index) const {
    QModelIndex id_index = createIndex(index.row(),0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok && d_observer)
        return d_observer->subjectReference(id);
    else
        return 0;
}

QObject* Qtilities::CoreGui::AbstractObserverTableModel::getObject(int row) const {
    QModelIndex id_index = createIndex(row,0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok && d_observer)
        return d_observer->subjectReference(id);
    else
        return 0;
}

QModelIndex Qtilities::CoreGui::AbstractObserverTableModel::getIndex(QObject* obj) const {
    if (!obj)
        return QModelIndex();

    // Get the subject id of the obj
    int id;
    ObserverProperty subject_id_property = d_observer->getObserverProperty(obj,OBSERVER_SUBJECT_IDS);
    if (subject_id_property.isValid()) {
        id = subject_id_property.value(d_observer->observerID()).toInt();
    }

    // Check if id is valid
    if (id < 0)
        return QModelIndex();

    // Now look for the id in the ID_COLUMN of the table model
    for (int i = 0; i < rowCount(); i++) {
        if (data(index(i,columnPosition(ColumnSubjectID)),Qt::DisplayRole).toInt() == id) {
            return index(i,columnPosition(ColumnSubjectID));
        }
    }

    return QModelIndex();
}
