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

#include "ObserverTreeModel.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ObserverMimeData.h"
#include "QtilitiesApplication.h"
#include "ObserverTreeModelBuilder.h"

#include <SubjectTypeFilter.h>
#include <QtilitiesCoreConstants.h>
#include <Observer.h>
#include <ActivityPolicyFilter.h>
#include <Logger.h>
#include <QtilitiesCategory.h>

#include <QMessageBox>
#include <QIcon>
#include <QDropEvent>
#include <QFileIconProvider>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

struct Qtilities::CoreGui::ObserverTreeModelData  {
    ObserverTreeModelData() : tree_model_up_to_date(true),
        tree_rebuild_queued(false),
        at_least_one_tree_build_completed(false),
        do_auto_select_and_expand(true) {}

    QPointer<ObserverTreeItem>  rootItem;
    QPointer<Observer>          selection_parent;
    QString                     type_grouping_name;
    bool                        read_only;
    QFileIconProvider           icon_provider;

    //! The selected categories.
    QList<QtilitiesCategory>    selected_categories;
    //! Set by setSelectedObjects(), used when layout of the tree changed externally (not in the view)
    QList<QPointer<QObject> >   selected_objects;
    //! Set by recordObserverChange(), used to record requested selected objects emitted using Observer::refreshViewsLayout().
    QList<QPointer<QObject> >   new_selection;
    //! Same as new_selection, but when refreshViewsLayout() is received while the tree model is busy, we store the requested new selection in queued_selection.
    QList<QPointer<QObject> >   queued_selection;

    bool                        tree_model_up_to_date;
    ObserverTreeModelBuilder    tree_builder;
    bool                        tree_rebuild_queued;
    bool                        at_least_one_tree_build_completed;

    //! Stores if select and expand must be done. See enableAutoSelectAndExpand().
    bool                        do_auto_select_and_expand;

    //! Nodes to expand in the tree after a rebuild is done.
    QList<QPointer<QObject> >   expanded_objects;
    QStringList                 expanded_categories;
    //! A map with expanded items which must be replaced after d->expanded_categories was set by the observer widget.
    /*!
        This is needed in order to expand renamed categories.
      */
    QMap<QString,QString>       expanded_categories_replace_map;

    QMutex                      build_mutex;
};

Qtilities::CoreGui::ObserverTreeModel::ObserverTreeModel(QObject* parent) :
    QAbstractItemModel(parent),
    AbstractObserverItemModel()
{
    d = new ObserverTreeModelData;

    // Init root data
    d->rootItem = new ObserverTreeItem();
    d->selection_parent = 0;
    d->type_grouping_name = QString();
    d->read_only = false;

    qRegisterMetaType<Qtilities::CoreGui::ObserverTreeItem>("Qtilities::CoreGui::ObserverTreeItem");
    connect(&d->tree_builder,SIGNAL(buildCompleted(ObserverTreeItem*)),SLOT(receiveBuildObserverTreeItem(ObserverTreeItem*)));
}

Qtilities::CoreGui::ObserverTreeModel::~ObserverTreeModel() {
    if (d->rootItem)
        delete d->rootItem;
    delete d;
}

bool Qtilities::CoreGui::ObserverTreeModel::setObserverContext(Observer* observer) {
    if (d_observer) {
        d_observer->disconnect(this);
        //clearTreeStructure(); // This causes invalid objects showing up, and repainting the treeview.
    }


    if (!observer) {
        clearTreeStructure();
        return false;
    }

    if (!AbstractObserverItemModel::setObserverContext(observer)) {
        clearTreeStructure();
        return false;
    }

    // Check if this observer has a subject type filter installed
    for (int i = 0; i < d_observer->subjectFilters().count(); ++i) {
        SubjectTypeFilter* subject_type_filter = qobject_cast<SubjectTypeFilter*> (d_observer->subjectFilters().at(i));
        if (subject_type_filter) {
            if (!subject_type_filter->groupName().isEmpty()) {
                d->type_grouping_name = subject_type_filter->groupName();
            }
            break;
        }
    }

    // The first time the observer context is set we rebuild the tree:
    if (observer->observerName() != qti_def_GLOBAL_OBJECT_POOL && !lazyInitEnabled())
        recordObserverChange();

    connect(d_observer,SIGNAL(destroyed()),SLOT(handleObserverContextDeleted()));
    connect(d_observer,SIGNAL(layoutChanged(QList<QPointer<QObject> >)),SLOT(recordObserverChange(QList<QPointer<QObject> >)));
    connect(d_observer,SIGNAL(dataChanged(Observer*)),SLOT(handleContextDataChanged(Observer*)));

    // If a selection parent does not exist, we set observer as the selection parent:
    if (!d->selection_parent)
        d->selection_parent = observer;

    return true;
}

int Qtilities::CoreGui::ObserverTreeModel::columnPosition(AbstractObserverItemModel::ColumnID column_id) const {
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

int Qtilities::CoreGui::ObserverTreeModel::columnVisiblePosition(AbstractObserverItemModel::ColumnID column_id) const {
    int start = 0;
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

QStack<int> Qtilities::CoreGui::ObserverTreeModel::getParentHierarchy(const QModelIndex& index) const {
    QStack<int> parent_hierarchy;
    ObserverTreeItem* item = getItem(index);
    if (!item)
        return parent_hierarchy;

    ObserverTreeItem* parent_item = item->parentItem();
    Observer* parent_observer = qobject_cast<Observer*> (parent_item->getObject());
    // Handle the cases where the parent is a category item:
    if (!parent_observer) {
        while (parent_item->itemType() == ObserverTreeItem::CategoryItem)
            parent_item = parent_item->parentItem();
        parent_observer = qobject_cast<Observer*> (parent_item->getObject());
    }

    while (parent_observer) {
        parent_hierarchy.push_front(parent_observer->observerID());
        parent_item = parent_item->parentItem();
        if (parent_item) {
            parent_observer = qobject_cast<Observer*> (parent_item->getObject());
            // Handle the cases where the parent is a category item:
            if (!parent_observer) {
                while (parent_item->itemType() == ObserverTreeItem::CategoryItem)
                    parent_item = parent_item->parentItem();
                parent_observer = qobject_cast<Observer*> (parent_item->getObject());
            }
        } else
            parent_observer = 0;
    }

    return parent_hierarchy;
}

QVariant Qtilities::CoreGui::ObserverTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (!d_observer)
        return QVariant();

    if (!d->tree_model_up_to_date)
        return QVariant();

    // ------------------------------------
    // Return nothing for columns which should not be displayed
    // We hide them in Observer Widget, but during initialize
    // the model is built before we get to a place where we can hide the
    // columns. Thus, the info is called on all models at least once
    // before we can hide it. Thus we filter it here:
    // ------------------------------------
    if (index.column() == columnPosition(AbstractObserverItemModel::ColumnSubjectID)) {
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnIDHint))
            return QVariant();
    }
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
    // Handle Name Column
    // ------------------------------------
    if (index.column() == columnPosition(ColumnName)) {
        // ------------------------------------
        // Qt::DisplayRole and Qt::EditRole
        // ------------------------------------
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return tr("Invalid Item");
            QObject* obj = item->getObject();
            if (obj) {
                // If this is a category item we just use objectName:
                if (item->itemType() == ObserverTreeItem::CategoryItem) {
                    if (role == Qt::DisplayRole)
                        return item->objectName();
                    else if (role == Qt::EditRole) {
                        if (activeHints()->categoryEditingFlags() & ObserverHints::CategoriesEditableTopLevel)
                            return item->objectName();
                        else if (activeHints()->categoryEditingFlags() & ObserverHints::CategoriesEditableAllLevels)
                            return item->category().toString();
                        else
                            return "Not supposed to be edited.";
                    }
                } else {
                    // Otherwise we must get the name of the object in the parent observer's context:
                    // First get the parent observer:
                    Observer* obs = 0;
                    if (item->parentItem()) {
                        if (item->parentItem()->itemType() == ObserverTreeItem::CategoryItem) {
                            obs = item->parentItem()->containedObserver();
                        } else {
                            obs = qobject_cast<Observer*> (item->parentItem()->getObject());
                        }
                    }

                    // Check the modification state of the object if it implements IModificationNotifier:
                    bool is_modified = false;
                    if (activeHints()->modificationStateDisplayHint() == ObserverHints::CharacterModificationStateDisplay && role == Qt::DisplayRole) {
                        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (obj);
                        if (mod_iface)
                            is_modified = mod_iface->isModified();
                    }

                    QString return_string;
                    // If observer is valid, we get the name, otherwise we just use object name.
                    if (obs) {
                        if (role == Qt::DisplayRole)
                            return_string = obs->subjectDisplayedNameInContext(obj);
                        else if (role == Qt::EditRole)
                            return_string = obs->subjectDisplayedNameInContext(obj,false);
                    } else
                        return_string = obj->objectName();

                    if (is_modified)
                        return return_string + "*";
                    else
                        return return_string;
                }
            } else {
                // We might get in here when a view tries to repaint itself (for example when a message box dissapears above it) before
                // the tree has been rebuilt properly.
                return "Refreshing...";
            }
        // ------------------------------------
        // Qt::CheckStateRole
        // ------------------------------------
        } else if (role == Qt::CheckStateRole) {
            // In here we need to manually get the top level of each index since the active context is
            // not representitive of all indexes we get in here:
            QObject* obj = getObject(index);
            if (!obj)
                return QVariant();

            QVariant subject_activity = QVariant();
            Observer* local_selection_parent = parentOfIndex(index);
            if (local_selection_parent) {
                // Once we have the local parent, we can check if it must display activity and if so, we return
                // the activity of obj in that context.

                // We need to check a few things things:
                // 1. Do we use the observer hints?
                // 2. If not, does the selection have hints?
                ObserverHints* hints_to_use_for_selection = 0;
                if (model->use_observer_hints && local_selection_parent->displayHints())
                    hints_to_use_for_selection = local_selection_parent->displayHints();
                else if (!model->use_observer_hints)
                    hints_to_use_for_selection = activeHints();

                if (hints_to_use_for_selection) {
                    if (hints_to_use_for_selection->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay) {
                        ActivityPolicyFilter* activity_filter = 0;
                        for (int i = 0; i < local_selection_parent->subjectFilters().count(); ++i) {
                            activity_filter = qobject_cast<ActivityPolicyFilter*> (local_selection_parent->subjectFilters().at(i));
                            if (activity_filter) {
                                subject_activity = local_selection_parent->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP);
                            }
                        }
                    }
                }
            }

            if (subject_activity.isValid()) {
                if (subject_activity.toBool())
                    return Qt::Checked;
                else
                    return Qt::Unchecked;
            } else
                return QVariant();
        // ------------------------------------
        // Qt::DecorationRole
        // ------------------------------------
        } else if (role == Qt::DecorationRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                // If this is a category item we just use objectName:
                if (item->itemType() == ObserverTreeItem::CategoryItem)
                    return d->icon_provider.icon(QFileIconProvider::Folder);
                else {
                    SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_DECORATION);
                    if (icon_property.isValid())
                        return icon_property.value();
                }
            }
        // ------------------------------------
        // Qt::WhatsThisRole
        // ------------------------------------
        } else if (role == Qt::WhatsThisRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_WHATS_THIS);
                if (icon_property.isValid())
                    return icon_property.value();                        
            }
        // ------------------------------------
        // Qt::SizeHintRole
        // ------------------------------------
        } else if (role == Qt::SizeHintRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                SharedProperty size_property = ObjectManager::getSharedProperty(obj,qti_prop_SIZE_HINT);
                if (size_property.isValid()) {
                    if (size_property.value().toSize().isValid())
                        return size_property.value();
                }
            }
        // ------------------------------------
        // Qt::StatusTipRole
        // ------------------------------------
        } else if (role == Qt::StatusTipRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_STATUSTIP);
                if (icon_property.isValid())
                    return icon_property.value();
            }
        // ------------------------------------
        // Qt::FontRole
        // ------------------------------------
        } else if (role == Qt::FontRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_FONT);
                if (icon_property.isValid())
                    return icon_property.value();
            }
        // ------------------------------------
        // Qt::TextAlignmentRole
        // ------------------------------------
        } else if (role == Qt::TextAlignmentRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_TEXT_ALIGNMENT);
                if (icon_property.isValid())
                    return icon_property.value();
            }
        // ------------------------------------
        // Qt::BackgroundRole
        // ------------------------------------
        } else if (role == Qt::BackgroundRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_BACKGROUND);
                if (icon_property.isValid())
                    return icon_property.value();
            }
        // ------------------------------------
        // Qt::ForegroundRole
        // ------------------------------------
        } else if (role == Qt::ForegroundRole) {
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            // Check if it has the role shared property set.
            if (obj) {
                SharedProperty icon_property = ObjectManager::getSharedProperty(obj,qti_prop_FOREGROUND);
                if (icon_property.isValid())
                    return icon_property.value();
            }
        // ------------------------------------
        // Qt::ToolTipRole
        // ------------------------------------
        } else if (role == Qt::ToolTipRole) {
            // Check if the object has an OBJECT_TOOLTIP shared property to show.
            ObserverTreeItem* item = getItem(index);
            if (!item)
                return QVariant();
            QObject* obj = item->getObject();

            if (obj) {
                SharedProperty tooltip = ObjectManager::getSharedProperty(obj,qti_prop_TOOLTIP);
                if (tooltip.isValid())
                    return tooltip.value();
            }
        }
    // ------------------------------------
    // Handle Child Count Column
    // ------------------------------------
    } else if (index.column() == columnPosition(ColumnChildCount)) {
        if (role == Qt::DisplayRole) {
            // Check if it is an observer, in that case we return treeCount() on the observer
            Observer* observer = qobject_cast<Observer*> (getItem(index)->getObject());
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
    } else if (index.column() == columnPosition(ColumnAccess)) {
        if (role == Qt::DecorationRole) {
            // First handle categories:
            ObserverTreeItem* item = getItem(index);
            if (item) {
                // If this is a category item we check if it has access & scope defined.
                if (item->itemType() == ObserverTreeItem::CategoryItem) {
                    // Get the parent observer:
                    Observer* obs = 0;
                    if (item->parentItem()) {
                        if (item->parentItem()->itemType() == ObserverTreeItem::CategoryItem) {
                            obs = item->parentItem()->containedObserver();
                        } else {
                            obs = qobject_cast<Observer*> (item->parentItem()->getObject());
                        }
                    }
                    // If observer is valid, we get the name, otherwise we just use object name.
                    if (obs) {
                        if (obs->accessModeScope() == Observer::CategorizedScope) {
                            if (obs->accessMode(item->category()) == Observer::ReadOnlyAccess)
                                return QIcon(qti_icon_READ_ONLY_16x16);
                            else if (obs->accessMode(item->category()) == Observer::LockedAccess)
                                return QIcon(qti_icon_LOCKED_16x16);
                            else
                                return QVariant();
                        }
                    }
                } else {
                    QObject* obj = getObject(index);
                    if (!obj)
                        return QVariant();
                    Observer* observer = qobject_cast<Observer*> (obj);
                    if (observer) {
                        if (observer->accessModeScope() == Observer::GlobalScope) {
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
                            if (mode.toInt() == Observer::LockedAccess)
                                return QIcon(qti_icon_LOCKED_16x16);
                        }
                    } else {
                        // Inspect the object to see if it has the qti_prop_ACCESS_MODE observer property.
                        QVariant mode = d_observer->getMultiContextPropertyValue(obj,qti_prop_ACCESS_MODE);
                        if (mode.toInt() == (int) Observer::ReadOnlyAccess)
                            return QIcon(qti_icon_READ_ONLY_16x16);
                        if (mode.toInt() == Observer::LockedAccess)
                            return QIcon(qti_icon_LOCKED_16x16);
                    }
                }
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags Qtilities::CoreGui::ObserverTreeModel::flags(const QModelIndex &index) const {
    if (!d->tree_model_up_to_date)
        return Qt::NoItemFlags;

     if (!index.isValid())
         return Qt::NoItemFlags;

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
         if (activeHints()->categoryEditingFlags() & ObserverHints::CategoriesEditableTopLevel)
             item_flags |= Qt::ItemIsEditable;
         else if (activeHints()->categoryEditingFlags() & ObserverHints::CategoriesEditableAllLevels)
             item_flags |= Qt::ItemIsEditable;
         else
             item_flags &= ~Qt::ItemIsEditable;

         item_flags &= ~Qt::ItemIsUserCheckable;
     } else {
         // The naming control hint we get from the active hints since the user must click
         // in order to edit the name. The click will update activeHints() for us.
         if (activeHints()->namingControlHint() == ObserverHints::EditableNames && !d->read_only)
             item_flags |= Qt::ItemIsEditable;
         else
             item_flags &= ~Qt::ItemIsEditable;

         // For the activity display we need to manually get the top level of each index
         // since the active context is not representitive of all indexes we get in here:
         Observer* local_selection_parent = parentOfIndex(index);
         if (local_selection_parent) {
             if (local_selection_parent->displayHints()) {
                 if ((local_selection_parent->displayHints()->activityControlHint() == ObserverHints::CheckboxTriggered))
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

     // Handle drag & drop hints:
     if (index.column() != columnPosition(ColumnName)) {
         item_flags &= ~Qt::ItemIsDragEnabled;
         if (!d->read_only)
            item_flags &= ~Qt::ItemIsDropEnabled;
     } else {
         if (item->itemType() == ObserverTreeItem::CategoryItem) {
             if (activeHints()->categoryEditingFlags() & ObserverHints::CategoriesAcceptSubjectDrops) {
                 item_flags |= Qt::ItemIsDropEnabled;
             } else {
                 item_flags &= ~Qt::ItemIsDropEnabled;
             }

            item_flags &= ~Qt::ItemIsDragEnabled;
         } else if (item->itemType() == ObserverTreeItem::TreeItem) {
             // For items we need to check the drag drop hint of the parent of the index:
             Observer* local_selection_parent = parentOfIndex(index);
             if (local_selection_parent) {
                 // We need to check a few things things:
                 // 1. Do we the observer hints?
                 // 2. If so, does the observer have hints?
                 ObserverHints* hints_to_use = 0;
                 if (!model->use_observer_hints)
                     hints_to_use = activeHints();
                 else {
                     if (local_selection_parent->displayHints())
                        hints_to_use = local_selection_parent->displayHints();
                 }

                 if (hints_to_use) {
                     // Check if drags are allowed:
                     if (hints_to_use->dragDropHint() & ObserverHints::AllowDrags)
                         item_flags |= Qt::ItemIsDragEnabled;
                     else
                         item_flags &= ~Qt::ItemIsDragEnabled;

                 } else {
                     item_flags &= ~Qt::ItemIsDragEnabled;
                 }
             } else {
                 item_flags &= ~Qt::ItemIsDragEnabled;
             }
             item_flags &= ~Qt::ItemIsDropEnabled;
         } else if (item->itemType() == ObserverTreeItem::TreeNode) {
             // Check if the node (observer) accepts drops or allows drags:
             Observer* obs = qobject_cast<Observer*> (item->getObject());
             if (obs) {
                 // We need to check a few things:
                 // 1. Do we the observer hints?
                 // 2. If so, does the observer have hints?
                 ObserverHints* hints_to_use = 0;
                 if (!model->use_observer_hints)
                     hints_to_use = activeHints();
                 else {
                     if (obs->displayHints())
                        hints_to_use = obs->displayHints();
                 }

                 if (hints_to_use) {
                     // Check if drags are allowed:
                     if (hints_to_use->dragDropHint() & ObserverHints::AllowDrags)
                         item_flags |= Qt::ItemIsDragEnabled;
                     else
                         item_flags &= ~Qt::ItemIsDragEnabled;
                     // Check if drops are accepted:
                     if ((hints_to_use->dragDropHint() & ObserverHints::AcceptDrops) && !d->read_only)
                         item_flags |= Qt::ItemIsDropEnabled;
                     else
                         item_flags &= ~Qt::ItemIsDropEnabled;
                 } else {
                     item_flags &= ~Qt::ItemIsDragEnabled;
                     item_flags &= ~Qt::ItemIsDropEnabled;
                 }
             } else {
                 item_flags &= ~Qt::ItemIsDragEnabled;
                 item_flags &= ~Qt::ItemIsDropEnabled;
             }
         }
     }

     return item_flags;
}

QVariant Qtilities::CoreGui::ObserverTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((section == columnPosition(ColumnName)) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (d->type_grouping_name.isEmpty() && observerContext())
            return observerContext()->observerName();
        else
            return d->type_grouping_name;
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

     return QAbstractItemModel::headerData(section,orientation,role);
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!d->tree_model_up_to_date)
        return QModelIndex();

    if (!d->rootItem)
        return QModelIndex();

    if (!hasIndex(row, column, parent))
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
    if (!d->tree_model_up_to_date)
        return QModelIndex();

    if (!index.isValid())
        return QModelIndex();

    ObserverTreeItem *childItem = getItem(index);
    if (!childItem)
        return QModelIndex();
    ObserverTreeItem *parentItem = childItem->parentItem();
    if (!parentItem)
        return QModelIndex();
    if (!parentItem->getObject())
        return QModelIndex();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

bool Qtilities::CoreGui::ObserverTreeModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) {
    Q_UNUSED(data)
    Q_UNUSED(action)

    if (!d->tree_model_up_to_date)
        return false;

    if (!(activeHints()->dragDropHint() & ObserverHints::AcceptDrops || activeHints()->categoryEditingFlags() & ObserverHints::CategoriesAcceptSubjectDrops))
        return false;

    Observer* obs = 0;
    ObserverTreeItem::TreeItemType target_item_type = ObserverTreeItem::InvalidType;
    ObserverTreeItem* target_item = 0;
    QtilitiesCategory* target_category = 0;
    if (row == -1 && column == -1) {
        // In this case we need to check if we dropped on an observer, in that case
        // we don't get the parent, we use parent.
        target_item = getItem(parent);
        if (target_item) {
            target_item_type = target_item->itemType();
            if (target_item_type == ObserverTreeItem::CategoryItem) {
                target_category = new QtilitiesCategory(target_item->category());
                if (!(activeHints()->categoryEditingFlags() & ObserverHints::CategoriesAcceptSubjectDrops))
                    return false;
            }

            if (target_item->getObject())
                obs = qobject_cast<Observer*> (target_item->getObject());
        }

        if (!obs)
            obs = parentOfIndex(parent);
    } else
        obs = parentOfIndex(index(row,column,parent));

    if (obs) {
        const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (CLIPBOARD_MANAGER->mimeData());
        if (observer_mime_data) {
            // Handle cases where we dropped subjects on a category:
            if (target_item_type == ObserverTreeItem::CategoryItem) {
                if (target_category) {
                    if (observer_mime_data->sourceID() != obs->observerID()) {
                        LOG_ERROR_P(QString(tr("The drop operation could not be completed. You can only assign categories using drag/drop operations within the same context.")));
                        return false;
                    }

                    obs->startProcessingCycle();
                    bool do_refresh = false;

                    // Get all subjects in mime data and change their categories:
                    QList<QPointer<QObject> > subjects = observer_mime_data->subjectList();
                    for (int i = 0; i < subjects.count(); ++i) {
                        if (subjects.at(i)) {
                            QVariant category_variant = obs->getMultiContextPropertyValue(subjects.at(i),qti_prop_CATEGORY_MAP);
                            // Check if a category property exists:
                            if (category_variant.isValid()) {
                                QtilitiesCategory current_category = category_variant.value<QtilitiesCategory>();
                                // Skip if current category is same as new category:
                                if (current_category == *target_category && !target_category->isEmpty())
                                    continue;

                                obs->setMultiContextPropertyValue(subjects.at(i),qti_prop_CATEGORY_MAP,qVariantFromValue(*target_category));
                                IModificationNotifier* mod_notifier = qobject_cast<IModificationNotifier*> (subjects.at(i));
                                if (mod_notifier)
                                    mod_notifier->setModificationState(true);
                                do_refresh= true;
                            } else {
                                // If the category does not exist, we create and add it:
                                MultiContextProperty new_category_prop(qti_prop_CATEGORY_MAP);
                                new_category_prop.setValue(qVariantFromValue(*target_category),obs->observerID());

                                ObjectManager::setMultiContextProperty(subjects.at(i),new_category_prop);
                                IModificationNotifier* mod_notifier = qobject_cast<IModificationNotifier*> (subjects.at(i));
                                if (mod_notifier)
                                    mod_notifier->setModificationState(true);
                                do_refresh= true;
                            }
                        }
                    }

                    obs->endProcessingCycle(false);
                    if (do_refresh)
                        obs->refreshViewsLayout(observer_mime_data->subjectList());
                    delete target_category;
                    return true;
                }
            } else {
                QString error_msg;
                // Now check the proposed action of the event.
                if (observer_mime_data->dropAction() == Qt::MoveAction) {
                    // Attempt to move the dragged objects:
                    if (!OBJECT_MANAGER->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),d_observer->observerID(),&error_msg))
                        LOG_ERROR_P(error_msg);
                } else if (observer_mime_data->dropAction() == Qt::CopyAction) {
                    // Attempt to copy the dragged objects:
                    // Either do all or nothing:
                    if (obs->canAttach(const_cast<ObserverMimeData*> (observer_mime_data),&error_msg,true) != Observer::Rejected) {
                        QList<QPointer<QObject> > dropped_list = obs->attachSubjects(const_cast<ObserverMimeData*> (observer_mime_data));
                        if (dropped_list.count() != observer_mime_data->subjectList().count()) {
                            LOG_WARNING_P(QString(tr("The drop operation completed partially. %1/%2 objects were drop successfully.").arg(dropped_list.count()).arg(observer_mime_data->subjectList().count())));
                        } else {
                            LOG_INFO_P(QString(tr("The drop operation completed successfully on %1 objects.").arg(dropped_list.count())));
                        }
                    } else
                        LOG_ERROR_P(QString(tr("The drop operation could not be completed. All objects could not be accepted by the destination context. Error message: ") + error_msg));
                }
            }
        } else
            LOG_ERROR_P(QString(tr("The drop operation could not be completed. The clipboard manager does not contain a valid mime data object.")));
    } else
        LOG_ERROR_P(QString(tr("The drop operation could not be completed. A suitable context to place your dropped data in could not be found.")));

    CLIPBOARD_MANAGER->clearMimeData();
    return true;
}

Qt::DropActions Qtilities::CoreGui::ObserverTreeModel::supportedDropActions() const {
    if (!d->tree_model_up_to_date)
        return Qt::IgnoreAction;

    Qt::DropActions drop_actions = 0;
    if (!d->read_only)
        drop_actions |= Qt::CopyAction;
    //drop_actions |= Qt::MoveAction;
    return drop_actions;
}

bool Qtilities::CoreGui::ObserverTreeModel::setData(const QModelIndex &set_data_index, const QVariant &value, int role) {
    if (!d->tree_model_up_to_date)
        return false;

    if (d->read_only)
        return false;

    if (set_data_index.column() == columnPosition(AbstractObserverItemModel::ColumnName)) {
        if (role == Qt::EditRole || role == Qt::DisplayRole) {
            ObserverTreeItem* item = getItem(set_data_index);
            QObject* obj = item->getObject();

            QPointer<Observer> local_selection_parent = d->selection_parent;

            if (!local_selection_parent)
                return false;

            // Check if the object has a qti_prop_NAME property, if not we set the name using setObjectName()
            if (obj) {
                if (item->itemType() == ObserverTreeItem::CategoryItem) {
                    if (activeHints()->categoryEditingFlags() & ObserverHints::CategoriesEditableTopLevel) {
                        QStringList list = item->category().toStringList();
                        QString bottom_name_old = item->category().categoryBottom();
                        list.pop_back();
                        QtilitiesCategory new_category(list);
                        new_category << value.toString();

                        // Replace category in expanded category list if it was in there:
                        // Expanded items only contains the top node names, thus
                        // a simple replace works for CategoriesEditableTopLevel.
                        d->expanded_categories_replace_map.clear();
                        d->expanded_categories_replace_map[bottom_name_old] = new_category.categoryBottom();

                        // Replace category in d->selected_categories if it was in there:
                        if (d->selected_categories.contains(item->category())) {
                            d->selected_categories.removeOne(item->category());
                            d->selected_categories << new_category;
                        }

                        local_selection_parent->renameCategory(item->category(),new_category,false);
                        return true;
                    } else if (activeHints()->categoryEditingFlags() & ObserverHints::CategoriesEditableAllLevels) {
                        QString bottom_name_old = item->category().categoryBottom();
                        QtilitiesCategory new_category(value.toString(),QString("::"));

                        // Replace category in expanded category list if it was in there:
                        // Expanded items only contains the top node names, thus
                        // a simple replace DOES NOT neccesserily work for CategoriesEditableAllLevels.
                        // However since we don't know what the user wants to do, we still add it
                        // for cases where only the top level category is changed.
                        d->expanded_categories_replace_map.clear();
                        d->expanded_categories_replace_map[bottom_name_old] = new_category.categoryBottom();

                        // Replace category in d->selected_categories if it was in there:
                        if (d->selected_categories.contains(item->category())) {
                            d->selected_categories.removeOne(item->category());
                            d->selected_categories << new_category;
                        }

                        local_selection_parent->renameCategory(item->category(),new_category,false);                       
                        return true;
                    } else
                        return false;
                } else {
                    if (ObjectManager::getSharedProperty(obj, qti_prop_NAME).isValid()) {
                        // Now check if this observer uses an instance name
                        MultiContextProperty instance_names = ObjectManager::getMultiContextProperty(obj,qti_prop_ALIAS_MAP);
                        if (instance_names.isValid() && instance_names.hasContext(local_selection_parent->observerID()))
                            local_selection_parent->setMultiContextPropertyValue(obj,qti_prop_ALIAS_MAP,value);
                        else
                            local_selection_parent->setMultiContextPropertyValue(obj,qti_prop_NAME,value);
                    } else {
                        obj->setObjectName(value.toString());
                    }
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
                        for (int i = 0; i < local_selection_parent->subjectFilters().count(); ++i) {
                            activity_filter = qobject_cast<ActivityPolicyFilter*> (local_selection_parent->subjectFilters().at(i));
                            if (activity_filter) {
                                // The value comming in here is always Qt::Checked.
                                // We get the current check state from the qti_prop_ACTIVITY_MAP property and change that.
                                QVariant current_activity = local_selection_parent->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP);
                                local_selection_parent->startProcessingCycle();
                                if (current_activity.toBool()) {
                                    local_selection_parent->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP,QVariant(false));
                                } else {
                                    local_selection_parent->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP,QVariant(true));
                                }
                                local_selection_parent->endProcessingCycle(false);
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

int Qtilities::CoreGui::ObserverTreeModel::rowCount(const QModelIndex &parent) const {
    if (!d->tree_model_up_to_date)
        return 0;

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
    if (!d->tree_model_up_to_date)
        return 0;

    if (parent.isValid()) {
         return columnPosition(AbstractObserverItemModel::ColumnLast) + 1;
     } else
         return d->rootItem->columnCount();
}

void Qtilities::CoreGui::ObserverTreeModel::recordObserverChange(QList<QPointer<QObject> > new_selection) {
    if (!respondToObserverChanges()) {
        #ifdef QTILITIES_BENCHMARKING
        qDebug() << "Ignoring layout changes to observer" << d_observer->observerName() << "in tree model.";
        #endif
        return;
    }

    if (d->tree_model_up_to_date) {
        if (d->build_mutex.tryLock()) {
            d->new_selection = new_selection;
            #ifdef QTILITIES_BENCHMARKING
            qDebug() << "Recording observer change on tree model: " << objectName() << ". The tree is not being built at the moment. Initiating a build request.";
            #endif
            rebuildTreeStructure();
            d->build_mutex.unlock();
        } else {
            d->tree_rebuild_queued = true;
            d->queued_selection = new_selection;
        }
    } else {
        d->tree_rebuild_queued = true;
        d->queued_selection = new_selection;
        #ifdef QTILITIES_BENCHMARKING
        qDebug() << QString("Received tree rebuild request in " + d_observer->observerName() + "'s view. The current tree is being rebuilt, thus queueing this change.");
        #endif
    }
}

void Qtilities::CoreGui::ObserverTreeModel::clearTreeStructure() {
    #ifdef QTILITIES_BENCHMARKING
    qDebug() << "Clearing tree structure on view: " << objectName();
    #endif

    emit treeModelBuildStarted();
    beginResetModel();
    emit layoutAboutToBeChanged();
    d->tree_model_up_to_date = false;
    deleteRootItem();
    QVector<QVariant> columns;
    columns.push_back("Child Count");
    columns.push_back("Access");
    columns.push_back("Type Info");
    columns.push_back("Object Tree");
    d->rootItem = new ObserverTreeItem(0,0,columns);
    d->rootItem->setObjectName("Root Item");

    d->tree_model_up_to_date = true;
    endResetModel();
    emit layoutChanged();
    emit treeModelBuildEnded();
}

void Qtilities::CoreGui::ObserverTreeModel::rebuildTreeStructure() {
    #ifdef QTILITIES_BENCHMARKING
    qDebug() << "Rebuilding tree structure on view: " << objectName();
    #endif

    // The view will call setExpandedItems() in its slot.
    // Note that the first time we show a context we don't emit the
    // signal below. This will send an empty list of expanded items
    // to the view which will cause it to expand all items.
    if (d->at_least_one_tree_build_completed)
        emit treeModelBuildAboutToStart();

    // d->expanded_categories would have been set in the above code.
    // Now we do the needed replacements:
    QList<QString> keys = d->expanded_categories_replace_map.keys();
    int count = keys.count();
    for (int i = 0; i < count; ++i) {
        if (d->expanded_categories.contains(keys.at(i))) {
            d->expanded_categories.removeOne(keys.at(i));
            d->expanded_categories << d->expanded_categories_replace_map.values().at(i);
            //qDebug() << "Doing expanded items replace:" << d->expanded_categories_replace_map.keys().at(i) << "with" << d->expanded_categories_replace_map.values().at(i);
        }
    }
    emit treeModelBuildStarted();

    // Rebuild the tree structure:
    beginResetModel();
    d->tree_model_up_to_date = false;
    deleteRootItem();

    // The root index display hint determines how we create the root node:
    QVector<QVariant> columns;
    columns.push_back("Child Count");
    columns.push_back("Access");
    columns.push_back("Type Info");
    columns.push_back("Object Tree");
    ObserverTreeItem* item_to_send_to_builder = 0;
    if (model->hints_top_level_observer) {
        if (model->hints_top_level_observer->rootIndexDisplayHint() == ObserverHints::RootIndexHide) {
            d->rootItem = new ObserverTreeItem(d_observer,0,columns,ObserverTreeItem::TreeNode);
            d->rootItem->setObjectName("Root Item");
            item_to_send_to_builder = d->rootItem;
        } else if (model->hints_top_level_observer->rootIndexDisplayHint() == ObserverHints::RootIndexDisplayDecorated || model->hints_top_level_observer->rootIndexDisplayHint() == ObserverHints::RootIndexDisplayUndecorated) {
            d->rootItem = new ObserverTreeItem(0,0,columns,ObserverTreeItem::TreeNode);
            d->rootItem->setObjectName("Root Item");
            ObserverTreeItem* top_level_observer_item = new ObserverTreeItem(d_observer,d->rootItem,QVector<QVariant>(),ObserverTreeItem::TreeNode);
            d->rootItem->appendChild(top_level_observer_item);
            item_to_send_to_builder = top_level_observer_item;
        }
    } else {
        d->rootItem = new ObserverTreeItem(d_observer,0,columns,ObserverTreeItem::TreeNode);
        d->rootItem->setObjectName("Root Item");
        item_to_send_to_builder = d->rootItem;
    }

    d->tree_rebuild_queued = false;
    d->tree_builder.setRootItem(item_to_send_to_builder);
    d->tree_builder.setUseObserverHints(model->use_observer_hints);
    d->tree_builder.setActiveHints(activeHints());
    //QApplication::processEvents();
    d->tree_builder.startBuild();
    //QApplication::processEvents();
}

void Qtilities::CoreGui::ObserverTreeModel::receiveBuildObserverTreeItem(ObserverTreeItem* item) {
    Q_UNUSED(item)

    d->tree_model_up_to_date = true;

    endResetModel();

    if (d->tree_rebuild_queued) {
        // Process events here before doing the next rebuild:
        QApplication::processEvents();
        emit selectObjects(d->queued_selection);
        rebuildTreeStructure();
    } else {
        // From my understanding not needed because we do a proper reset sequence.
        // They cause a repaint which makes the rebuilt operation flicker.
        //emit layoutAboutToBeChanged();
        //emit layoutChanged();
        if (d->do_auto_select_and_expand) {
            // Restore expanded categories:
            QModelIndexList expanded_indexes = findExpandedNodeIndexes(d->expanded_categories);
            expanded_indexes << findExpandedNodeIndexes(d->expanded_objects);
            emit expandItemsRequest(expanded_indexes);

            // Handle item selection after tree has been rebuilt:
            if (d->new_selection.count() > 0)
                emit selectObjects(d->new_selection);
            else if (d->selected_objects.count() > 0)
                emit selectObjects(d->selected_objects);
            else if (d->selected_categories.count() > 0)
                emit selectCategories(d->selected_categories);
        }

        d->at_least_one_tree_build_completed = true;
    }

    emit treeModelBuildEnded();
}

void Qtilities::CoreGui::ObserverTreeModel::setExpandedItems(const QList<QPointer<QObject> >& expanded_objects, const QStringList& expanded_categories) {
    //qDebug() << "setExpandedItems" << expanded_categories;
    d->expanded_categories = expanded_categories;
    d->expanded_objects = expanded_objects;
}

void Qtilities::CoreGui::ObserverTreeModel::enableAutoSelectAndExpand() {
    d->do_auto_select_and_expand = true;
}

void Qtilities::CoreGui::ObserverTreeModel::disableAutoSelectAndExpand() {
    d->do_auto_select_and_expand = false;
}

Qtilities::Core::Observer* Qtilities::CoreGui::ObserverTreeModel::calculateSelectionParent(QModelIndexList index_list) {
    if (index_list.count() == 1) {
        d->selection_parent = parentOfIndex(index_list.front());

        // Do some hints debugging:
        /*if (d->selection_parent) {
            qDebug() << "New selection parent: " << d->selection_parent->objectName() << " with display hints: " << d->selection_parent->displayHints()->displayFlagsHint();
        }*/

        // Get the hints from the observer:
        if (d->selection_parent) {
            model->hints_selection_parent = d->selection_parent->displayHints();
        }

        emit selectionParentChanged(d->selection_parent);
        return d->selection_parent;
    } else if (index_list.count() > 1) {
        Observer* parent = 0;
        bool match = true;

        foreach (QModelIndex index, index_list) {
            Observer* obs = parentOfIndex(index);
            if (parent == 0)
                parent = obs;
            else if (obs != parent) {
                parent = obs;
                match = false;
                break;
            }
        }

        //qDebug() << "Multiple selection, determining parent. Is the same:" << match;

        // Only pass on the selection parent if the selected objects are all in the same context:
        if (match) {
            d->selection_parent = parent;

            // Get the hints from the observer:
            if (d->selection_parent) {
                model->hints_selection_parent = d->selection_parent->displayHints();
            }

            emit selectionParentChanged(d->selection_parent);
            return d->selection_parent;
        } else {
            d->selection_parent = 0;
            model->hints_selection_parent = 0;
            emit selectionParentChanged(0);
            return 0;
        }
    } else {
        d->selection_parent = d_observer;
        // Get the hints from the observer:
        if (d->selection_parent) {
            model->hints_selection_parent = d->selection_parent->displayHints();
        }

        emit selectionParentChanged(d->selection_parent);
        return d->selection_parent;
    }
}

int Qtilities::CoreGui::ObserverTreeModel::getSubjectID(const QModelIndex &index) const {
    QObject* obj = getItem(index)->getObject();
    if (d_observer)
        return d_observer->getMultiContextPropertyValue(obj,qti_prop_OBSERVER_MAP).toInt();
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

void Qtilities::CoreGui::ObserverTreeModel::refresh() {
    recordObserverChange();
}

Qtilities::Core::Observer* Qtilities::CoreGui::ObserverTreeModel::selectionParent() const {
    return d->selection_parent;
}

Qtilities::Core::Observer* Qtilities::CoreGui::ObserverTreeModel::parentOfIndex(const QModelIndex &index) const {
    Observer* local_selection_parent = 0;
    ObserverTreeItem* item = getItem(index);
    if (!item)
        return 0;

    ObserverTreeItem* item_parent = item->parentItem();
    if (!item_parent)
        return 0;

    if (activeHints()->rootIndexDisplayHint() == ObserverHints::RootIndexHide) {
        if (item_parent == d->rootItem)
            return observerContext();
    }

    if (item_parent->getObject())
        local_selection_parent = qobject_cast<Observer*> (item_parent->getObject());
    else
        return 0;

    if (!local_selection_parent) {
        // Handle the cases where the parent is a category item:
        if (item_parent->itemType() == ObserverTreeItem::CategoryItem) {
            local_selection_parent = item_parent->containedObserver();
        }
    }

    return local_selection_parent;  
}

void Qtilities::CoreGui::ObserverTreeModel::handleContextDataChanged(Observer* observer) {
    if (!observer)
        return;

    if (!respondToObserverChanges()) {
        qDebug() << "Ignoring data changes to observer" << observer->observerName() << "in tree model.";
        return;
    }

    QModelIndex parent_index = findObject(observer);
    bool parent_index_valid = parent_index.isValid();
    if (!parent_index_valid)
        return;
    else {
        // Refresh the parent as well since the code below will only refresh the children of the parent.
        emit dataChanged(parent_index,parent_index);
    }

    QModelIndex top_left = index(0,0,parent_index);
    bool top_left_valid = top_left.isValid();
    if (!top_left_valid)
        return;

    int column_count = columnCount()-1;
    int row_count = rowCount(parent_index) - 1;
    QModelIndex bottom_right = index(row_count-1,column_count-1,parent_index);
    bool bottom_right_valid = bottom_right.isValid();
    if (!bottom_right_valid)
        return;

    if (hasIndex(row_count,column_count,parent_index))
        emit dataChanged(top_left,bottom_right);
}

void Qtilities::CoreGui::ObserverTreeModel::handleContextDataChanged(const QModelIndex &set_data_index) {
    // We get the indexes for the complete context since activity of many objects might change:
    // Warning: This is not going to work for categorized hierarchy observers.
    QModelIndex parent_index = parent(set_data_index);
    bool parent_index_valid = parent_index.isValid();
    if (!parent_index_valid)
        return;

    QModelIndex top_left = index(0,0,parent_index);
    bool top_left_valid = top_left.isValid();
    if (!top_left_valid)
        return;

    int column_count = columnCount()-1;
    int row_count = rowCount(parent_index) - 1;
    QModelIndex bottom_right = index(row_count-1,column_count-1,parent_index);
    bool bottom_right_valid = bottom_right.isValid();
    if (!bottom_right_valid)
        return;

    if (hasIndex(row_count,column_count,parent_index))
        emit dataChanged(top_left,bottom_right);
}

void Qtilities::CoreGui::ObserverTreeModel::setSelectedObjects(QList<QPointer<QObject> > selected_objects) {
    d->selected_objects = selected_objects;
}

void Qtilities::CoreGui::ObserverTreeModel::setSelectedCategories(QList<QtilitiesCategory> selected_categories) {
    d->selected_categories = selected_categories;
}

void Qtilities::CoreGui::ObserverTreeModel::setReadOnly(bool read_only) {
    if (d->read_only == read_only)
        return;

    d->read_only = read_only;
}

bool Qtilities::CoreGui::ObserverTreeModel::readOnly() const {
    return d->read_only;
}

QModelIndexList Qtilities::CoreGui::ObserverTreeModel::getPersistentIndexList() const {
    return persistentIndexList();
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::findObject(QObject* obj, int column) const {
    QList<QModelIndex> indices = rootIndices();
    for (int i = 0; i < indices.count(); i++) {
        QModelIndex index = findObject(indices.at(i),obj,column);
        if (index.isValid())
            return index;
    }

    return QModelIndex();
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::getIndex(QObject *obj, int column) const {
    return findObject(obj,column);
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::findCategory(QtilitiesCategory category) const {
    QList<QModelIndex> indices = rootIndices();
    for (int i = 0; i < indices.count(); i++) {
        QModelIndex index = findCategory(indices.at(i),category);
        if (index.isValid())
            return index;
    }

    return QModelIndex();
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::findObject(const QModelIndex& current_index, QObject* obj, int column) const {
    QModelIndex correct_index;
    if (current_index.isValid()) {
        // Check this index:
        ObserverTreeItem* item = getItem(current_index);
        if (item) {
            if (item->getObject() == obj)
                return current_index;

            if (column == -1)
                column = columnPosition(ObserverTreeModel::ColumnName);

            //qDebug() << "findObject(): " << item->getObject()->objectName();

            // Ok it was not this index, loop through all children under this index:
            for (int i = 0; i < item->childCount(); ++i) {
                QModelIndex child_index = index(i,column,current_index);
                correct_index = findObject(child_index,obj);
                if (correct_index.isValid()) {
                    return correct_index;
                }
            }
        }
    }
    return QModelIndex();
}

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::ObserverTreeModel::findObject(ObserverTreeItem* item, QObject* obj) const {
    // Check item:
    if (item->getObject() == obj)
        return item;

    // Check all the children of item and traverse into them.
    for (int i = 0; i < item->childCount(); ++i) {
        ObserverTreeItem* tree_item = findObject(item->childItemReferences().at(i),obj);
        if (tree_item)
            return tree_item;
    }

    return 0;
}

QModelIndex Qtilities::CoreGui::ObserverTreeModel::findCategory(const QModelIndex& current_index, QtilitiesCategory category) const {
    QModelIndex correct_index;
    if (current_index.isValid()) {
        // Check this index:
        ObserverTreeItem* item = getItem(current_index);
        if (item) {
            if ((item->category() == category) && (item->itemType() == ObserverTreeItem::CategoryItem)) {
                return current_index;
            }

            // Ok it was not this index, loop through all children under this index:
            for (int i = 0; i < item->childCount(); ++i) {
                QModelIndex child_index = index(i,0,current_index);
                correct_index = findCategory(child_index,category);
                if (correct_index.isValid()) {
                    return correct_index;
                }
            }
        }
    }
    return QModelIndex();
}

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::ObserverTreeModel::findCategory(ObserverTreeItem* item, QtilitiesCategory category) const {
    // Check item:
    if ((item->category() == category) && (item->itemType() == ObserverTreeItem::CategoryItem))
        return item;

    // Check all the children of item and traverse into them.
    for (int i = 0; i < item->childCount(); ++i) {
        ObserverTreeItem* tree_item = findCategory(item->childItemReferences().at(i),category);
        if (tree_item)
            return tree_item;
    }

    return 0;
}

void Qtilities::CoreGui::ObserverTreeModel::deleteRootItem() {
    if (!d->rootItem)
        return;

    delete d->rootItem;
    d->rootItem = 0;
}

QList<QModelIndex> Qtilities::CoreGui::ObserverTreeModel::rootIndices() const {
    QList<QModelIndex> indices;
    if (!model->hints_top_level_observer)
        return indices;

    if (model->hints_top_level_observer->rootIndexDisplayHint() == ObserverHints::RootIndexHide) {
        // There are rowCount() root indexes.
        for (int i = 0; i < rowCount(); i++) {
            QModelIndex current_index = index(i,0);
            if (current_index.isValid())
                indices << current_index;
        }
    } else {
        indices << index(0,0);
    }
    return indices;
}

QModelIndexList Qtilities::CoreGui::ObserverTreeModel::findExpandedNodeIndexes(const QStringList& node_names) const {
    QModelIndexList complete_match_list;
    foreach (const QString& item, node_names) {
        complete_match_list.append(match(index(0,columnPosition(AbstractObserverItemModel::ColumnName)),Qt::DisplayRole,QVariant::fromValue(item),1,Qt::MatchRecursive));
        complete_match_list.append(match(index(0,columnPosition(AbstractObserverItemModel::ColumnName)),Qt::DisplayRole,QVariant::fromValue(item + "*"),1,Qt::MatchRecursive));
    }
    return complete_match_list;
}

QModelIndexList Qtilities::CoreGui::ObserverTreeModel::findExpandedNodeIndexes(const QList<QPointer<QObject> > &objects) const {
    QModelIndexList complete_match_list;
    for (int i = 0; i < objects.count(); i++) {
        QModelIndex index = findObject(objects.at(i));
        if (index.isValid())
            complete_match_list << index;
    }
    return complete_match_list;
}

QModelIndexList Qtilities::CoreGui::ObserverTreeModel::getAllIndexes(ObserverTreeItem* item) const {
    static QModelIndexList indexes;
    if (!item) {
        indexes.clear();

        QList<QModelIndex> incides = rootIndices();
        for (int i = 0; i < incides.count(); i++) {
            item = getItem(incides.at(i));
            if (!item)
                return indexes;

            // Add this root and call getAllIndexes on all its children.
            QModelIndex index = findObject(item->getObject());
            indexes << index;
            for (int r = 0; r < item->childCount(); ++r)
                getAllIndexes(item->child(r));
        }
    } else {
        // Add this root and call getAllIndexes on all its children.
        QModelIndex index = findObject(item->getObject());
        indexes << index;
        for (int r = 0; r < item->childCount(); ++r)
            getAllIndexes(item->child(r));
    }

    return indexes;
}

Qtilities::CoreGui::ObserverTreeItem* Qtilities::CoreGui::ObserverTreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()) {
        ObserverTreeItem *item = static_cast<ObserverTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }

    return 0;
}

void Qtilities::CoreGui::ObserverTreeModel::handleObserverContextDeleted() {
    if (d_observer)
        d_observer->disconnect(this);
    clearTreeStructure();
    d->selection_parent = 0;
}
