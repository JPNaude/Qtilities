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
          */
        class ObserverTreeItem : public QObject
        {
            Q_OBJECT
            Q_ENUMS(TreeItemType)

        public:
            //! The possible types of items which can be part of the constructed observer tree.
            enum TreeItemType {
                ObjectItem,         /*!< An object item. */
                CategoryItem        /*!< A category item. */
            };

            ObserverTreeItem(QObject* obj = 0, ObserverTreeItem *parent = 0, const QVector<QVariant> &data = QVector<QVariant>(), TreeItemType type = ObjectItem);
            ~ObserverTreeItem();

            ObserverTreeItem *child(int row);
            void appendChild(ObserverTreeItem *child_item);
            int childCount() const;
            int columnCount() const;
            QVariant data(int column) const;
            int row() const;
            ObserverTreeItem *parent();
            inline void setObject(QObject* object) { obj = object; }
            inline QObject* getObject() const { return obj; }
            inline TreeItemType itemType() const { return type; }
            //! Sets the category represented through this item. Only used with CategoryItem types.
            inline void setCategory(const QString& category) { category_id = category; }
            //! Gets the category represented through this item. Only used with CategoryItem types.
            inline QString category() const { return category_id; }
            //! Sets a references to an observer in the case where the observer is contained within an interface.
            /*!
            This is used in cases where a non-observer based child is the parent of an observer.
            An example of this where the requirement is an QObject based interface, thus
            the object can't inherit from Observer directly. It can but it defeats the purpose of an
            abstract interface which needs to hide the actual implementation. In such cases use the
            observer class through containment and make the interface implementation it's parent.
              */
            inline void setContainedObserver(Observer* contained_observer) { contained_observer_ref = contained_observer; }
            //! Gets the contained observer reference. The reference is held by the category item.
            inline Observer* containedObserver() const { return contained_observer_ref; }

        signals:
            void newObjectAdded(QObject* obj, ObserverTreeItem* new_item);

        private:
            QList<QPointer<ObserverTreeItem> > childItems;
            QVector<QVariant> itemData;
            QPointer<ObserverTreeItem> parentItem;
            QPointer<QObject> obj;
            TreeItemType type;
            QString category_id;
            QPointer<Observer> contained_observer_ref;
        };
    }
}

#endif // TREEITEMS_H
