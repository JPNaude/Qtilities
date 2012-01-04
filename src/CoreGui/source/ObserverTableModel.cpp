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

#include "ObserverTableModel.h"
#include "NamingPolicyFilter.h"
#include "ActivityPolicyFilter.h"
#include "QtilitiesCoreGuiConstants.h"

#include <SubjectTypeFilter>
#include <QtilitiesCoreConstants>
#include <Observer>
#include <QtilitiesCategory>

#include <QIcon>
#include <QMessageBox>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

struct Qtilities::CoreGui::ObserverTableModelData {
    ObserverTableModelData() : type_grouping_name(QString()),
        read_only(false) { }

    QString type_grouping_name;
    bool read_only;
};

Qtilities::CoreGui::ObserverTableModel::ObserverTableModel(QObject* parent) : QAbstractTableModel(parent), AbstractObserverItemModel()
{
    d = new ObserverTableModelData();
}

bool Qtilities::CoreGui::ObserverTableModel::setObserverContext(Observer* observer) {
    if (d_observer)
        d_observer->disconnect(this);

    if (!observer)
        return false;

    // We cannot view the global object pool as a table because it is not part of the qti_prop_OBSERVER_MAP property.
    // Therefore we tell this to the user.
    if (observer->objectName() == QString(qti_def_GLOBAL_OBJECT_POOL)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Qtilities ObserverTableModel Error");
        msgBox.setText("It is not possible to view the Global Object Pool using ObserverTableModel. Use ObserverTreeModel instead.");
        msgBox.exec();
        return false;
    }

    if (!AbstractObserverItemModel::setObserverContext(observer))
        return false;

    connect(d_observer,SIGNAL(layoutChanged(QList<QPointer<QObject> >)),SLOT(handleLayoutChanged()));
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

int Qtilities::CoreGui::ObserverTableModel::columnPosition(AbstractObserverItemModel::ColumnID column_id) const {
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

int Qtilities::CoreGui::ObserverTableModel::columnVisiblePosition(AbstractObserverItemModel::ColumnID column_id) const {
    int start = 0;
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

QVariant Qtilities::CoreGui::ObserverTableModel::data(const QModelIndex &index, int role) const {
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
            // Check the modification state of the object if it implements IModificationNotifier:
            bool is_modified = false;
            if (activeHints()->modificationStateDisplayHint() == ObserverHints::CharacterModificationStateDisplay) {
                IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d_observer->subjectAt(index.row()));
                if (mod_iface)
                    is_modified = mod_iface->isModified();
            }

            QString return_string = d_observer->subjectDisplayedNames().at(index.row());
            if (is_modified)
                return return_string + "*";
            else
                return return_string;

        // ------------------------------------
        // Qt::CheckStateRole
        // ------------------------------------
        } else if (role == Qt::CheckStateRole) {
            if (model->activity_filter) {
                if (activeHints()->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay || activeHints()->activityControlHint() == ObserverHints::CheckboxTriggered) {
                    QObject* obj = d_observer->subjectReference(getSubjectID(index));
                    QVariant subject_activity = d_observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP);

                    if (subject_activity.isValid()) {
                        if (subject_activity.toBool())
                            return Qt::Checked;
                        else
                            return Qt::Unchecked;
                    } else
                        return QVariant();
                }
            }
        // ------------------------------------
        // Qt::DecorationRole
        // ------------------------------------
        } else if (role == Qt::DecorationRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_DECORATION);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::ForegroundRole
        // ------------------------------------
        } else if (role == Qt::ForegroundRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_FOREGROUND);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::BackgroundRole
        // ------------------------------------
        } else if (role == Qt::BackgroundRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_BACKGROUND);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::TextAlignmentRole
        // ------------------------------------
        } else if (role == Qt::TextAlignmentRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_TEXT_ALIGNMENT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::FontRole
        // ------------------------------------
        } else if (role == Qt::FontRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_FONT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::SizeHintRole
        // ------------------------------------
        } else if (role == Qt::SizeHintRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_SIZE_HINT);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::WhatsThisRole
        // ------------------------------------
        } else if (role == Qt::WhatsThisRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_WHATS_THIS);
            if (icon_property.isValid()) {
                return icon_property.value();
            }
        // ------------------------------------
        // Qt::ToolTipRole
        // ------------------------------------
        } else if (role == Qt::ToolTipRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            SharedProperty tooltip = ObjectManager::getSharedProperty(obj,qti_prop_TOOLTIP);
            if (tooltip.isValid()) {
                return tooltip.value();
            }
        }
    // ------------------------------------
    // Handle Category Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnCategory)) {
        if (role == Qt::DisplayRole) {
            // Get the qti_prop_CATEGORY_MAP property.
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            QVariant category_variant = d_observer->getMultiContextPropertyValue(obj,qti_prop_CATEGORY_MAP);
            if (category_variant.isValid()) {
                QtilitiesCategory category = category_variant.value<QtilitiesCategory>();
                if (!category.isEmpty()) {
                    return category.toString();
                }
            }
            return QString();
        }
    // ------------------------------------
    // Handle Child Count Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnChildCount)) {
        if (role == Qt::DisplayRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            if (!obj)
                return QVariant();

            Observer* observer = qobject_cast<Observer*> (obj);
            if (observer) {
                return observer->treeCount();
            } else {
                // Handle the case where the child is the parent of an observer
                int count = 0;
                foreach (QObject* child, obj->children()) {
                    Observer* child_observer = qobject_cast<Observer*> (child);
                    if (child_observer)
                        count += child_observer->treeCount();
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
    } else if (index.column() == columnPosition(ColumnTypeInfo)) {
        if (role == Qt::DisplayRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            if (!obj)
                return QVariant();

            if (obj->metaObject()) {
                return QString(QLatin1String(obj->metaObject()->className())).split("::").last();
            }
            return QVariant();
        }
    // ------------------------------------
    // Handle Access Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnAccess)) {
        if (role == Qt::DecorationRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            if (!obj)
                return QVariant();

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
                    return QIcon(qti_icon_READ_ONLY_16x16);
                if (observer->accessMode() == Observer::LockedAccess)
                    return QIcon(qti_icon_LOCKED_16x16);
            } else {
                // Inspect the object to see if it has the qti_prop_ACCESS_MODE observer property.
                QVariant mode = d_observer->getMultiContextPropertyValue(obj,qti_prop_ACCESS_MODE);
                if (mode.toInt() == (int) Observer::ReadOnlyAccess)
                    return QIcon(qti_icon_READ_ONLY_16x16);
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags Qtilities::CoreGui::ObserverTableModel::flags(const QModelIndex &index) const {
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;

     if (activeHints()->namingControlHint() == ObserverHints::EditableNames && !d->read_only)
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

QVariant Qtilities::CoreGui::ObserverTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((section == columnPosition(ColumnSubjectID)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return tr("ID");
    } else if ((section == columnPosition(ColumnName)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (d->type_grouping_name.isEmpty())
            return tr("Items");
        else
            return d->type_grouping_name;
    } else if ((section == columnPosition(ColumnCategory)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return tr("Category");
    } else if ((section == columnPosition(ColumnCategory)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        return tr("Category");
    } else if ((section == columnPosition(ColumnChildCount)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        return QIcon(qti_icon_CHILD_COUNT_22x22);
    } else if ((section == columnPosition(ColumnChildCount)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        return tr("Child Count");
    } else if ((section == columnPosition(ColumnTypeInfo)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        return QIcon(qti_icon_TYPE_INFO_22x22);
    } else if ((section == columnPosition(ColumnTypeInfo)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        return tr("Type");
    } else if ((section == columnPosition(ColumnAccess)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        return QIcon(qti_icon_ACCESS_16x16);
    } else if ((section == columnPosition(ColumnAccess)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        return tr("Access");
    }

    return QVariant();
}

bool Qtilities::CoreGui::ObserverTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (d->read_only)
        return false;

    if (index.column() == columnPosition(ColumnSubjectID)) {
        return false;
    } else if (index.column() == columnPosition(ColumnName)) {
        if (role == Qt::EditRole || role == Qt::DisplayRole) {
            QObject* obj = d_observer->subjectReference(getSubjectID(index));
            // Check if the object has an qti_prop_NAME property, if not we set the name using setObjectName()
            if (ObjectManager::getSharedProperty(obj, qti_prop_NAME).isValid()) {
                // Now check if this observer uses an instance name
                MultiContextProperty instance_names = ObjectManager::getMultiContextProperty(obj,qti_prop_ALIAS_MAP);
                if (instance_names.isValid() && instance_names.hasContext(d_observer->observerID()))
                    d_observer->setMultiContextPropertyValue(obj,qti_prop_ALIAS_MAP,value);
                else
                    d_observer->setMultiContextPropertyValue(obj,qti_prop_NAME,value);
            } else {
                obj->setObjectName(value.toString());
            }
            return true;
        } else if (role == Qt::CheckStateRole) {
            if (model->activity_filter) {
                //if (activeHints()->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay && activeHints()->activityControlHint() == ObserverHints::CheckboxTriggered) {
                    QObject* obj = d_observer->subjectReference(getSubjectID(index));
                    // The value coming in here is always Qt::Checked
                    // We get the current check state from the qti_prop_ACTIVITY_MAP property and change that:
                    QVariant current_activity = d_observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP);
                    d_observer->startProcessingCycle();
                    if (current_activity.toBool()) {
                        d_observer->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP,QVariant(false));
                    } else {
                        d_observer->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP,QVariant(true));
                    }
                    d_observer->endProcessingCycle();
                    emit dataChanged(index,index);
                    return true;
                //}
            }
        }
    }
    return false;
}

int Qtilities::CoreGui::ObserverTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    if (!d_observer)
        return 0;
    else {
        return d_observer->subjectCount();
    }
}

int Qtilities::CoreGui::ObserverTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    if (!d_observer)
        return 0;
    else {
        return columnPosition(AbstractObserverItemModel::ColumnLast) + 1;
    }
}

void Qtilities::CoreGui::ObserverTableModel::handleDataChanged() {
    emit dataChanged(index(0,0),index(rowCount(),columnCount()));
}

void Qtilities::CoreGui::ObserverTableModel::handleLayoutChanged() {
    emit layoutAboutToBeChanged();
    emit layoutChanged();
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

void Qtilities::CoreGui::ObserverTableModel::setReadOnly(bool read_only) {
    if (d->read_only == read_only)
        return;

    d->read_only = read_only;
}

bool Qtilities::CoreGui::ObserverTableModel::readOnly() const {
    return d->read_only;
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
    int id = -1;
    MultiContextProperty subject_id_property = ObjectManager::getMultiContextProperty(obj,qti_prop_OBSERVER_MAP);
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
