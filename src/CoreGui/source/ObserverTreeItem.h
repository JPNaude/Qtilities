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

#ifndef TREEITEMS_H
#define TREEITEMS_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QPointer>

#include <Observer.h>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
          \class Qtilities::CoreGui::ObserverTreeItem
          \brief The ObserverTreeItem class represents a single observer item in a tree view.

          This class should not be confused with TreeItem or TreeFileItem which are actual objects that are used to build tree structures along with TreeNode. The
          ObserverTreeItem class is used internally by ObserverTreeModel to construct a model used to display an Qtilities::Core::Observer tree structure and
          should not be used directly.
          */
        class ObserverTreeItem : public QObject
        {
            Q_OBJECT
            Q_ENUMS(TreeItemType)

        public:
            //! The possible types of items which can be part of the constructed observer tree.
            enum TreeItemType {
                InvalidType         = 0, /*!< An invalid type. */
                TreeItem            = 1, /*!< A tree item. */
                TreeNode            = 2, /*!< A tree node. */
                CategoryItem        = 4, /*!< A category item. */
                AllItemTypes        = TreeItem | TreeNode | CategoryItem
            };
            Q_DECLARE_FLAGS(TreeItemTypeFlags, TreeItemType)
            Q_FLAGS(TreeItemTypeFlags)

            ObserverTreeItem(QObject* obj = 0, ObserverTreeItem *parent = 0, const QVector<QVariant> &data = QVector<QVariant>(), TreeItemType type = InvalidType);
            ObserverTreeItem(const ObserverTreeItem& ref);
            ~ObserverTreeItem();

            ObserverTreeItem *child(int row);
            void appendChild(ObserverTreeItem *child_item);
            //! Checks if a child with the name already exists.
            /*!
              If the child already exists a reference is returned to it. If not 0 is returned.
              */
            ObserverTreeItem* childWithName(const QString& name) const;
            int childCount() const;
            int columnCount() const;
            int row() const;
            ObserverTreeItem *parentItem() const;
            inline QList<QPointer<ObserverTreeItem> > childItemReferences() const { return childItemList; }
            inline void setObject(QObject* object) { obj = object; }
            inline QPointer<QObject> getObject() const { return obj; }
            inline TreeItemType itemType() const { return type; }
            //! Sets the category represented through this item. Only used with CategoryItem types.
            inline void setCategory(const QtilitiesCategory& category) { category_id = category; }
            //! Gets the category represented through this item. Only used with CategoryItem types.
            inline QtilitiesCategory category() const { return category_id; }
            //! Sets a references to an observer in the case where the observer is contained within an interface.
            inline void setContainedObserver(Observer* contained_observer) { contained_observer_ref = contained_observer; }
            //! Gets the contained observer reference. The reference is held by the category item.
            inline Observer* containedObserver() const { return contained_observer_ref; }

        signals:
            void newObjectAdded(QObject* obj, ObserverTreeItem* new_item);

        private:
            QHash<QString,QPointer<ObserverTreeItem> > childItemHash;
            QList<QPointer<ObserverTreeItem> > childItemList;
            QVector<QVariant> itemData;
            QPointer<ObserverTreeItem> parent_item;
            QPointer<QObject> obj;
            TreeItemType type;
            QtilitiesCategory category_id;
            QPointer<Observer> contained_observer_ref;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverTreeItem::TreeItemTypeFlags);
    }
}

#endif // TREEITEMS_H
