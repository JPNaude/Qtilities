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

#include "ObserverTreeModelBuilder.h"
#include <QtilitiesCoreGui>

#include <stdio.h>
#include <time.h>

using namespace QtilitiesCoreGui;

struct Qtilities::CoreGui::ObserverTreeModelBuilderPrivateData  {
    ObserverTreeModelBuilderPrivateData() : hints(0),
        root_item(0) {}

    QMutex                          build_lock;
    ObserverHints*                  hints;
    bool                            use_hints;
    ObserverTreeItem*               root_item;
};

Qtilities::CoreGui::ObserverTreeModelBuilder::ObserverTreeModelBuilder(ObserverTreeItem* item, bool use_observer_hints, ObserverHints* observer_hints, QObject* parent) : QObject(parent) {
    d = new ObserverTreeModelBuilderPrivateData;

    d->hints = observer_hints;
    d->use_hints = use_observer_hints;
    setRootItem(item);
}

Qtilities::CoreGui::ObserverTreeModelBuilder::~ObserverTreeModelBuilder() {
    delete d;
}

void Qtilities::CoreGui::ObserverTreeModelBuilder::setRootItem(ObserverTreeItem* item) {
    d->root_item = item;
    if (d->root_item) {
        QString context_name = "Root Item";
        Observer* obs = qobject_cast<Observer*> (d->root_item->getObject());
        if (obs)
            context_name = obs->observerName();
    }
}

void Qtilities::CoreGui::ObserverTreeModelBuilder::setUseObserverHints(bool use_observer_hints) {
    d->use_hints = use_observer_hints;
}

void Qtilities::CoreGui::ObserverTreeModelBuilder::setActiveHints(ObserverHints* active_hints) {
    d->hints = active_hints;
}

void Qtilities::CoreGui::ObserverTreeModelBuilder::startBuild() {
    if (!d->root_item) {
        LOG_DEBUG(QString("%1 = no root item specified.").arg(Q_FUNC_INFO));
        emit buildCompleted(d->root_item);
        return;
    }

    d->build_lock.lock();

    //QApplication::processEvents();
    buildRecursive(d->root_item);

    d->build_lock.unlock();

    //printStructure(root_item);
    emit buildCompleted(d->root_item);
}

void Qtilities::CoreGui::ObserverTreeModelBuilder::buildRecursive(ObserverTreeItem* item, QList<QPointer<QObject> > category_objects) {
     // In here we build the complete structure of all the children below item.
    Observer* observer = qobject_cast<Observer*> (item->getObject());
    ObserverTreeItem* new_item;

    if (!observer && item->getObject()) {
        // Handle cases where the item is a category item
        if (item->itemType() == ObserverTreeItem::CategoryItem) {
            // Get the observer from the parent of item
            if (item->parentItem()) {
                Observer* parent_observer = item->containedObserver();
                if (parent_observer) {
                    // Now add all items belonging to this category
                    int cat_count = category_objects.count();
                    for (int i = 0; i < cat_count; ++i) {
                        //QApplication::processEvents();
                        // Storing all information in the data vector here can improve performance
                        QPointer<QObject> object = category_objects.at(i);
                        if (object) {
                            QPointer<Observer> obs = qobject_cast<Observer*> (object);
                            QVector<QVariant> column_data;
                            column_data << QVariant(parent_observer->subjectNameInContext(object));
                            if (obs) {
                                new_item = new ObserverTreeItem(object,item,column_data,ObserverTreeItem::TreeNode);
                            } else {
                                new_item = new ObserverTreeItem(object,item,column_data,ObserverTreeItem::TreeItem);
                            }
                            item->appendChild(new_item);

                            if (obs)
                                buildRecursive(new_item);
                        }
                    }
                }
            }
        }
    }

    if (observer) {
        // If this observer is locked we don't show its children:
        if (observer->accessMode() != Observer::LockedAccess) {
            // Check the HierarchicalDisplay hint of the observer:
            // Remember this is an recursive function, we can't use hints directly since thats linked to the selection parent.
            bool use_categorized;
            ObserverHints* hints_to_use = 0;
            if (d->use_hints) {
                if (observer->displayHints()) {
                    use_categorized = (observer->displayHints()->hierarchicalDisplayHint() == ObserverHints::CategorizedHierarchy);
                    hints_to_use = observer->displayHints();
                } else
                    use_categorized = false;
            } else {
                use_categorized = (d->hints->hierarchicalDisplayHint() == ObserverHints::CategorizedHierarchy);
                hints_to_use = d->hints;
            }

            if (use_categorized) {
                // Get the object / category hash:
                QMap<QPointer<QObject>, QString> category_map = observer->subjectReferenceCategoryMap();
                QSet<QString> categories = category_map.values().toSet();

                foreach (const QString& category_string, categories) {
                    //QApplication::processEvents();
                    QtilitiesCategory category = QtilitiesCategory(category_string,"::");
                    // Check the category against the displayed category list:
                    bool valid_category = true;
                    if (hints_to_use) {
                        QList<QtilitiesCategory> displayed_categories = hints_to_use->displayedCategories();
                        if (hints_to_use->categoryFilterEnabled()) {
                            if (hints_to_use->hasInversedCategoryDisplay()) {
                                if (!displayed_categories.contains(category))
                                    valid_category = true;
                                else
                                    valid_category = false;
                            } else {
                                if (displayed_categories.contains(category))
                                    valid_category = true;
                                else
                                    valid_category = false;
                            }
                        }
                    }

                    // Only add valid categories:
                    if (valid_category) {
                        // Ok here we need to create items for each category level and add the items underneath it.
                        int level_counter = 0;
                        QList<ObserverTreeItem*> tree_item_list;
                        while (level_counter < category.categoryDepth()) {
                            QStringList category_levels = category.toStringList(level_counter+1);

                            // Get the correct parent:
                            ObserverTreeItem* correct_parent;
                            if (tree_item_list.count() == 0)
                                correct_parent = item;
                            else
                                correct_parent = tree_item_list.last();

                            // Check if the parent item already has a category for this level:
                            ObserverTreeItem* existing_item = correct_parent->childWithName(category_levels.last());
                            if (!existing_item) {
                                // Create a category for the first level and add all items under this category to the tree:
                                QVector<QVariant> category_columns;
                                category_columns << category_levels.last();
                                QObject* category_item = new QObject();
                                // Check the access mode of this category and add it to the category object:
                                QtilitiesCategory shortened_category(category_levels);
                                Observer::AccessMode category_access_mode = observer->accessMode(shortened_category);
                                if (category_access_mode != Observer::InvalidAccess) {
                                    SharedProperty access_mode_property(qti_prop_ACCESS_MODE,(int) observer->accessMode(shortened_category));
                                    ObjectManager::setSharedProperty(category_item,access_mode_property);
                                }
                                category_item->setObjectName(category_levels.last());

                                // Create new item:
                                new_item = new ObserverTreeItem(category_item,correct_parent,category_columns,ObserverTreeItem::CategoryItem);
                                new_item->setContainedObserver(observer);
                                new_item->setCategory(category_levels);

                                // Append new item to correct parent item:
                                if (tree_item_list.count() == 0)
                                    item->appendChild(new_item);
                                else
                                    tree_item_list.last()->appendChild(new_item);
                                tree_item_list.push_back(new_item);

                                // If this item has locked access, we don't dig into any items underneath it:
                                if (observer->accessMode(shortened_category) != Observer::LockedAccess) {
                                    QList<QPointer<QObject> > safe_list = category_map.keys(category_levels.join("::"));
                                    buildRecursive(new_item,safe_list);
                                } else
                                    break;
                            } else
                                tree_item_list.push_back(existing_item);

                            // Increment the level counter:
                            ++level_counter;
                        }
                    }
                }

                // Here we need to add all items which do not belong to a specific category:
                // Get the list of uncategorized items from the observer
                QList<QObject*> uncat_list = observer->subjectReferencesByCategory(QtilitiesCategory());
                QStringList uncat_names = observer->subjectNamesByCategory(QtilitiesCategory());
                int uncat_list_count = uncat_list.count();
                for (int i = 0; i < uncat_list_count; ++i) {
                    //QApplication::processEvents();
                    QObject* obj_at = uncat_list.at(i);
                    Observer* obs = qobject_cast<Observer*> (obj_at);
                    QVector<QVariant> column_data;
                    column_data << QVariant(uncat_names.at(i));
                    if (obs) {
                        new_item = new ObserverTreeItem(obj_at,item,column_data,ObserverTreeItem::TreeNode);
                        item->appendChild(new_item);
                        // If this item has locked access, we don't dig into any items underneath it:
                        if (obs->accessMode(QtilitiesCategory()) != Observer::LockedAccess && obs)
                            buildRecursive(new_item);
                    } else {
                        new_item = new ObserverTreeItem(obj_at,item,column_data,ObserverTreeItem::TreeItem);
                        item->appendChild(new_item);
                    }
                }
            } else {
                int count = observer->subjectCount();
                for (int i = 0; i < count; ++i) {
                    //QApplication::processEvents();
                    QObject* obj_at = observer->subjectAt(i);
                    Observer* obs = qobject_cast<Observer*> (obj_at);
                    QVector<QVariant> column_data;
                    column_data << QVariant(observer->subjectNameInContext(obj_at));
                    if (obs)
                        new_item = new ObserverTreeItem(obj_at,item,column_data,ObserverTreeItem::TreeNode);
                    else
                        new_item = new ObserverTreeItem(obj_at,item,column_data,ObserverTreeItem::TreeItem);
                    item->appendChild(new_item);

                    if (obs)
                        buildRecursive(new_item);
                }
            }
        }
    }
}

void Qtilities::CoreGui::ObserverTreeModelBuilder::printStructure(ObserverTreeItem* item, int level) {
    if (level == 0) {
        item = d->root_item;
        qDebug() << "Tree Debug (" << level << "): Object = " << item->objectName() << ", Parent = None, Child Count = " << item->childCount();
    } else
        qDebug() << "Tree Debug (" << level << "): Object = " << item->objectName() << ", Parent = " << item->parentItem()->objectName() << ", Child Count = " << item->childCount();

    for (int i = 0; i < item->childCount(); ++i)
        printStructure(item->child(i),level+1);
}
