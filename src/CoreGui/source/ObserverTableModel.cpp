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
        read_only(false),
        fetch_count(0) { }

    QString     type_grouping_name;
    bool        read_only;
    int         fetch_count;
    QList<QPointer<QObject> > selected_objects;
};

#define fetch_limit 1000

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

    d->fetch_count = 0;
    connect(d_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication, QList<QPointer<QObject> >)),SLOT(handleLayoutChanged()));
    connect(d_observer,SIGNAL(destroyed()),SLOT(handleLayoutChanged()));
    connect(d_observer,SIGNAL(dataChanged()),SLOT(handleDataChanged()));

    // Check if this observer has a subject type filter installed
    for (int i = 0; i < observer->subjectFilters().count(); ++i) {
        SubjectTypeFilter* subject_type_filter = qobject_cast<SubjectTypeFilter*> (observer->subjectFilters().at(i));
        if (subject_type_filter) {
            if (!subject_type_filter->groupName().isEmpty()) {
                d->type_grouping_name = subject_type_filter->groupName();
            }
            break;
        }
    }

    handleLayoutChanged();
    return true;
}

int Qtilities::CoreGui::ObserverTableModel::columnPosition(AbstractObserverItemModel::ColumnID column_id) const {
    if (column_id == AbstractObserverItemModel::ColumnSubjectID) {
        return 0;
    } else if (column_id == AbstractObserverItemModel::ColumnName) {
        return 1;
    } else if (column_id == AbstractObserverItemModel::ColumnCategory) {
        return 3;
    } else if (column_id == AbstractObserverItemModel::ColumnChildCount) {
        return 2;
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
        start = 3;
    } else if (column_id == AbstractObserverItemModel::ColumnChildCount) {
        start = 2;
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

    if (index.row() >= d_observer->subjectCount() || index.row() < 0)
        return QVariant();

    // ------------------------------------
    // Return nothing for columns which should not be displayed
    // We hide them in Observer Widget, but during initialize
    // the model is built before we get to a place where we can hide the
    // columns. Thus, the info is called on all models at least once
    // before we can hide it. Thus we filter it here:
    // ------------------------------------
    if (index.column() == columnPosition(AbstractObserverItemModel::ColumnName)) {
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnNameHint))
            return QVariant();
    }
    if (index.column() == columnPosition(AbstractObserverItemModel::ColumnChildCount)) {
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint))
            return QVariant();
    }
    if (index.column() == columnPosition(AbstractObserverItemModel::ColumnAccess)) {
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint))
            return QVariant();
    }
    if (index.column() == columnPosition(AbstractObserverItemModel::ColumnTypeInfo)) {
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
            return QVariant();
    }
    if (index.column() == columnPosition(AbstractObserverItemModel::ColumnCategory)) {
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnCategoryHint))
            return QVariant();
    }

    // ------------------------------------
    // Handle Subject ID Column
    // ------------------------------------
    if (index.column() == columnPosition(ColumnSubjectID)) {
        // We need EditRole here, its used in subjectID()
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return d_observer->subjectID(index.row());
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
            QObject* obj = d_observer->subjectAt(index.row());
            if (activeHints()->modificationStateDisplayHint() == ObserverHints::CharacterModificationStateDisplay) {
                IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (obj);
                if (mod_iface)
                    is_modified = mod_iface->isModified();
            }

            QString return_string = d_observer->subjectDisplayedNameInContext(obj);
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
            SharedProperty size_property = ObjectManager::getSharedProperty(obj,qti_prop_SIZE_HINT);
            if (size_property.isValid()) {
                if (size_property.value().toSize().isValid())
                    return size_property.value();
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
                int count = observer->treeCount(columnChildCountBaseClass());
                if ((count > columnChildCountLimit() - 1) && (columnChildCountLimit() != -1))
                    return QString("> %1").arg(columnChildCountLimit() -1);
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
            return observerContext()->observerName();
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
    } else if ((section == columnPosition(ColumnChildCount)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return "";
    } else if ((section == columnPosition(ColumnTypeInfo)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        return QIcon(qti_icon_TYPE_INFO_22x22);
    } else if ((section == columnPosition(ColumnTypeInfo)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        return tr("Type");
    } else if ((section == columnPosition(ColumnTypeInfo)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return "";
    } else if ((section == columnPosition(ColumnAccess)) && (orientation == Qt::Horizontal) && (role == Qt::DecorationRole)) {
        return QIcon(qti_icon_ACCESS_16x16);
    } else if ((section == columnPosition(ColumnAccess)) && (orientation == Qt::Horizontal) && (role == Qt::ToolTipRole)) {
        return tr("Access");
    } else if ((section == columnPosition(ColumnAccess)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return "";
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

                    // We do group activity when the index clicked on is part of the current selection:
                    bool index_part_of_selection = false;
                    for (int p = 0; p < d->selected_objects.count(); p++) {
                        if (d->selected_objects.at(p) == obj) {
                            index_part_of_selection = true;
                            break;
                        }
                    }

                    // Get the ActivityPolicyFilter on the Observer:
                    bool do_group_activity_change = false;
                    if (index_part_of_selection) {
                        foreach (AbstractSubjectFilter* filter, d_observer->subjectFilters()) {
                            ActivityPolicyFilter* activity_filter = qobject_cast<ActivityPolicyFilter*> (filter);
                            if (activity_filter) {
                                // To be able to do a group activity change, we need to check that the following hints are used:
                                // 1) AllowNoneActive must be set
                                // 2) AllowMultipleActivity must be set
                                if (activity_filter->minimumActivityPolicy() == ActivityPolicyFilter::AllowNoneActive &&
                                        activity_filter->activityPolicy() == ActivityPolicyFilter::MultipleActivity)
                                    do_group_activity_change = true;
                            }
                        }
                    }

                    d_observer->endProcessingCycle(false);

                    // Finally, toggle the activity:
                    if (do_group_activity_change && d->selected_objects.count() > 0 && index_part_of_selection) {
                        d_observer->startProcessingCycle();
                        // Loop through all active subjects and set their activity:
                        for (int p = 0; p < d->selected_objects.count(); p++)
                            d_observer->setMultiContextPropertyValue(d->selected_objects.at(p),qti_prop_ACTIVITY_MAP,QVariant(!current_activity.toBool()));
                        d_observer->endProcessingCycle(false);
                        emit selectObjects(d->selected_objects);
                    } else {
                        // Just do single activity:
                        d_observer->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP,QVariant(!current_activity.toBool()));
                    }

                    emit dataChanged(index,index);
                    return true;
                //}
            }
        }
    }
    return false;
}

bool Qtilities::CoreGui::ObserverTableModel::canFetchMore(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    if (!d_observer)
        return false;

    if (d->fetch_count < d_observer->subjectCount())
        return true;
    else
        return false;
}

void Qtilities::CoreGui::ObserverTableModel::fetchMore(const QModelIndex &parent) {
    Q_UNUSED(parent)
    if (!d_observer)
        return;

    int remainder = d_observer->subjectCount() - d->fetch_count;
    //qDebug() << "remainder" << remainder;
    int itemsToFetch = qMin(fetch_limit, remainder);

    beginInsertRows(QModelIndex(), d->fetch_count, d->fetch_count+itemsToFetch-1);
    d->fetch_count += itemsToFetch;
    endInsertRows();

    emit moreDataFetched(itemsToFetch);
}

int Qtilities::CoreGui::ObserverTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    if (!d_observer)
        return 0;
    else {
        if (d->fetch_count == 0) {
            // First time:
            d->fetch_count = qMin(fetch_limit, d_observer->subjectCount());
            return d->fetch_count;
        } else {
            return d->fetch_count;
        }
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
    if (!d_observer)
        return;

    if (!respondToObserverChanges()) {
        #ifdef QTILITIES_BENCHMARKING
        qDebug() << "Ignoring data changes to observer" << d_observer->observerName() << "in table model";
        #endif
        return;
    } else {
        #ifdef QTILITIES_BENCHMARKING
        qDebug() << "Responding to data changes to observer" << d_observer->observerName() << "in table model";
        #endif
    }

    emit dataChanged(createIndex(0,0),createIndex(rowCount()-1,columnCount()-1));
}

void Qtilities::CoreGui::ObserverTableModel::handleLayoutChanged() {
    if (!d_observer)
        return;

    if (!respondToObserverChanges()) {
        #ifdef QTILITIES_BENCHMARKING
        qDebug() << "Ignoring number of subjects changed on observer" << d_observer->observerName() << "in table model";
        #endif
        return;
    } else {
        #ifdef QTILITIES_BENCHMARKING
        qDebug() << "Responding to number of subjects changed on observer" << d_observer->observerName() << "in table model";
        #endif
    }

    d->fetch_count = qMin(fetch_limit, d_observer->subjectCount());
    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit layoutChangeCompleted();
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

void Qtilities::CoreGui::ObserverTableModel::setSelectedObjects(QList<QPointer<QObject> > selected_objects) {
    d->selected_objects = selected_objects;
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

void Qtilities::CoreGui::ObserverTableModel::refresh() {
    handleLayoutChanged();
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

QModelIndex Qtilities::CoreGui::ObserverTableModel::getIndex(QObject* obj, int column) const {
    if (!d_observer)
        return QModelIndex();

    if (!obj)
        return QModelIndex();

    if (column == -1)
        column = columnPosition(ColumnName);

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
    for (int i = 0; i < rowCount(); ++i) {
        if (data(index(i,columnPosition(ColumnSubjectID)),Qt::DisplayRole).toInt() == id) {
            return index(i,column);
        }
    }

    return QModelIndex();
}
