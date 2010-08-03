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

#include "ObserverTableModel.h"
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

Qtilities::CoreGui::ObserverTableModel::ObserverTableModel(const QStringList &headers, QObject* parent) : QAbstractTableModel(parent), AbstractObserverItemModel()
{
    // Headers
    d_headers = headers;
    d_type_grouping_name = QString();

    // Init pointers
    d_naming_filter = 0;
    d_activity_filter = 0;

    d_manual_mode = false;
}

void Qtilities::CoreGui::ObserverTableModel::setObserverContext(Observer* observer)
{
    if (d_observer)
        d_observer->disconnect(this);

    AbstractObserverItemModel::setObserverContext(observer);

    d_naming_filter = 0;
    d_activity_filter = 0;

    if (observer) {
        // Look which known subject filters are installed in this observer
        for (int i = 0; i < observer->subjectFilters().count(); i++) {
            // Check if it is a naming policy subject filter
            NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (observer->subjectFilters().at(i));
            if (naming_filter)
                d_naming_filter = naming_filter;

            // Check if it is an activity policy subject filter
            ActivityPolicyFilter* activity_filter = qobject_cast<ActivityPolicyFilter*> (observer->subjectFilters().at(i));
            if (activity_filter)
                d_activity_filter = activity_filter;
        }

        connect(d_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication)),SIGNAL(layoutChanged()));
        connect(d_observer,SIGNAL(destroyed()),SIGNAL(layoutChanged()));
        connect(d_observer,SIGNAL(propertyBecameDirty(const char*, QObject*)),SLOT(handleDirtyProperty(const char*)));
        handleDataChange();

        if (!d_manual_mode) {
            // Check if this observer provides hints for this model
            if (observer->namingControlHint() != Observer::NoNamingControlHint)
                d_naming_control = observer->namingControlHint();
            if (observer->activityDisplayHint() != Observer::NoActivityDisplayHint)
                d_activity_display = observer->activityDisplayHint();
            if (observer->activityControlHint() != Observer::NoActivityControlHint)
                d_activity_control = observer->activityControlHint();
            if (observer->hierarchicalDisplayHint() != Observer::NoHierarhicalDisplayHint)
                d_hierachical_display_hint = observer->hierarchicalDisplayHint();
            if (observer->itemSelectionControlHint() != Observer::NoItemSelectionControlHint)
                d_item_selection_control = observer->itemSelectionControlHint();
            if (observer->itemViewColumnFlags() != Observer::NoItemViewColumnHint)
                d_item_view_column_flags = observer->itemViewColumnFlags();
        }

        // Check if this observer has a subject type filter installed
        for (int i = 0; i < observer->subjectFilters().count(); i++) {
            SubjectTypeFilter* subject_type_filter = qobject_cast<SubjectTypeFilter*> (observer->subjectFilters().at(i));
            if (subject_type_filter) {
                if (!subject_type_filter->groupName().isEmpty()) {
                    d_type_grouping_name = subject_type_filter->groupName();
                }
                break;
            }
        }

    }
}

QVariant Qtilities::CoreGui::ObserverTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (!d_observer)
        return QVariant();

    if (index.column() == IdColumn) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return d_observer->subjectID(index.row());
        }
    } else if (index.column() == NameColumn) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return d_observer->subjectNames().at(index.row());
        } else if (role == Qt::CheckStateRole) {
            if (d_activity_filter) {
                if (d_activity_display == Observer::CheckboxActivityDisplay || d_activity_control == Observer::CheckboxTriggered) {
                    QObject* obj = d_observer->subjectReference(getSubjectID(index));
                    QVariant subject_activity = d_observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY);

                    Q_ASSERT(subject_activity.isValid());
                    return subject_activity.toBool();
                }
            }
        } else if (role == Qt::DecorationRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));

            // Check if it has the OBJECT_ICON shared property set.
            SharedObserverProperty icon_property = d_observer->getSharedProperty(obj,OBJECT_ICON);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        } else if (role == Qt::ToolTipRole) {
            // Check if the object has an OBJECT_TOOLTIP shared property to show.
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedObserverProperty tooltip = d_observer->getSharedProperty(obj,OBJECT_TOOLTIP);
            if (tooltip.isValid()) {
                return tooltip.value();
            }
        }
    } else if (index.column() == CategoryColumn && (d_hierachical_display_hint && Observer::CategorizedHierarchy)) {
        if (role == Qt::DisplayRole) {
            // Get the OBJECT_CATEGORY property.
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            QString category = d_observer->getObserverPropertyValue(obj,OBJECT_CATEGORY).toString();
            if (category.isEmpty())
                category = tr("Uncategorized");
            return category;
        }
    } else if (index.column() == ChildCountColumn && (d_item_view_column_flags && Observer::ChildCountColumn)) {
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
    } else if (index.column() == TypeInfoColumn && (d_item_view_column_flags && Observer::TypeInfoColumn)) {
        if (role == Qt::DisplayRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            if (obj->metaObject()) {
                return QString(QLatin1String(obj->metaObject()->className())).split("::").last();
            }
            return QVariant();
        }
    } else if (index.column() == AccessColumn && (d_item_view_column_flags && Observer::AccessColumn)) {
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

Qt::ItemFlags Qtilities::CoreGui::ObserverTableModel::flags(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;

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

     return item_flags;
}


QVariant Qtilities::CoreGui::ObserverTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((section == NameColumn) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (d_type_grouping_name.isEmpty())
            return tr("Items");
        else
            return d_type_grouping_name;
    } else if ((section == CategoryColumn) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (d_item_view_column_flags & Observer::CategoryColumn)
            return tr("Category");
    } else if ((section == CategoryColumn) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        if (d_item_view_column_flags & Observer::CategoryColumn)
            return tr("Category");
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

bool Qtilities::CoreGui::ObserverTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() == IdColumn) {
        return false;
    } else if (index.column() == NameColumn) {
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
            if (d_activity_filter) {
                if (d_activity_display == Observer::CheckboxActivityDisplay && d_activity_control == Observer::CheckboxTriggered) {
                    QObject* obj = d_observer->subjectReference(getSubjectID(index));
                    // The value comming in here is always Qt::Checked
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

int Qtilities::CoreGui::ObserverTableModel::rowCount(const QModelIndex &parent) const {
    if (!d_observer)
        return 0;
    else {
        return d_observer->subjectCount();
    }
}

int Qtilities::CoreGui::ObserverTableModel::columnCount(const QModelIndex &parent) const {
    if (!d_observer)
        return 0;
    else
        return TypeInfoColumn+1;
}

void Qtilities::CoreGui::ObserverTableModel::handleDataChange() {
    emit dataChanged(index(0,0),index(rowCount(),columnCount()));
    emit layoutChanged();
}

void Qtilities::CoreGui::ObserverTableModel::handleDirtyProperty(const char* property_name) {
    if (!strcmp(property_name,OBJECT_NAME)) {
        handleDataChange();
    } else if (!strcmp(property_name,OBJECT_ACTIVITY)) {
        handleDataChange();
    }
}

int Qtilities::CoreGui::ObserverTableModel::getSubjectID(const QModelIndex &index) const {
    QModelIndex id_index = createIndex(index.row(),0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok)
        return id;
    else
        return -1;
}

int Qtilities::CoreGui::ObserverTableModel::getSubjectID(int row) const {
    QModelIndex id_index = createIndex(row,0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok)
        return id;
    else
        return -1;
}

QObject* Qtilities::CoreGui::ObserverTableModel::getObject(const QModelIndex &index) const {
    QModelIndex id_index = createIndex(index.row(),0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok && d_observer)
        return d_observer->subjectReference(id);
    else
        return 0;
}

QObject* Qtilities::CoreGui::ObserverTableModel::getObject(int row) const {
    QModelIndex id_index = createIndex(row,0);
    bool ok;
    int id = data(id_index, Qt::EditRole).toInt(&ok);
    if (ok && d_observer)
        return d_observer->subjectReference(id);
    else
        return 0;
}

QModelIndex Qtilities::CoreGui::ObserverTableModel::getIndex(QObject* obj) const {
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
        if (data(index(i,IdColumn),Qt::DisplayRole).toInt() == id) {
            return index(i,IdColumn);
        }
    }

    return QModelIndex();
}
