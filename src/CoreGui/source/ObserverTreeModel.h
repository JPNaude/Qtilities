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

#ifndef OBSERVERTREEMODEL_H
#define OBSERVERTREEMODEL_H

#include "AbstractObserverItemModel.h"
#include "QtilitiesCoreGui_global.h"
#include "ObserverTreeItem.h"

#include <Observer.h>

#include <QMutex>
#include <QAbstractItemModel>
#include <QStack>
#include <QItemSelection>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct ObserverTreeModelData
        \brief Structure used by ObserverTreeModel to store private data.
          */
        struct ObserverTreeModelData;

        /*!
        \class ObserverTreeModel
        \brief The ObserverTreeModel class provides an ready-to-use model that can be used to show the children of an Observer in a QTreeView.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTreeModel : public QAbstractItemModel, public AbstractObserverItemModel
        {
            Q_OBJECT
            Q_ENUMS(ColumnIDs)

        public:
            ObserverTreeModel(const QStringList &headers = QStringList(), QObject *parent = 0);
            virtual ~ObserverTreeModel() {}

            // --------------------------------
            // ObserverTableModel Implementation
            // --------------------------------
            virtual Qt::ItemFlags flags(const QModelIndex &index) const;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            virtual QVariant data(const QModelIndex &index, int role) const;
            int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
            QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
            QModelIndex parent(const QModelIndex &index) const;

            // --------------------------------
            // AbstractObserverItemModel Implementation
            // --------------------------------
            bool setObserverContext(Observer* observer);
            virtual int columnPosition(AbstractObserverItemModel::ColumnID column_id) const;
            int getSubjectID(const QModelIndex &index) const;
            QObject* getObject(const QModelIndex &index) const;

            // --------------------------------
            // ObserverTreeModel Implementation
            // --------------------------------
            //! Returns a QStack with the parent hierarchy (in terms of observer IDs) for the object at the given index.
            QStack<int> getParentHierarchy(const QModelIndex& index) const;
            //! Returns the parent observer of the current selection, it only works if a single item is selected, otherwise returns 0.
            /*!
              The selection parent is calculated using the calculateSelectionParent() function. This function return
              the parent identified the last time that function was called.
              */
            Observer* selectionParent() const;
            //! Returns the parent observer of the item at the specified index.
            /*!
              \returns The parent observer of the current index, or 0 if the index is invalid.
              \note For the root item the parent will also be 0.
              */
            Observer* parentOfIndex(const QModelIndex& index) const;
            //! Returns the ObserverTreeItem at a the specified index.
            /*!
              \returns The ObserverTreeItem at the current index, or 0 if the index is invalid.
              */
            ObserverTreeItem* getItem(const QModelIndex &index) const;

        public slots:
            //! Function which will rebuild the complete tree structure under the top level observer.
            /*!
                This slot will automatically be connected to the layoutChanged() signal on the top level observer.
              */
            void rebuildTreeStructure();
            //! Function which will calculate the selection parent of a selected object.
            /*!
              \param index_list The list of indexes currently selected in the tree view. This function will only calculate the selection parent if the list contain only one item.
              */
            Observer* calculateSelectionParent(QModelIndexList index_list);
            //! This slot will clean up the tree and refresh the view when the top level observer is deleted.
            void handleObserverContextDeleted();
            //! Handle context data changes where the context is defined by an Observer reference.
            /*!
              This function will emit the dataChanged() signal with the indexes of all items underneath this observer.
              */
            void handleContextDataChanged(Observer* observer);
            //! Handle context data changes where the context is defined by a child QModelIndex.
            /*!
              This function will emit the dataChanged() signal with the indexes of all items in the same context as the item defined by \p index.
              */
            void handleContextDataChanged(const QModelIndex &set_data_index);

        signals:
            //! Signal which is emmited when the current selection parent changed. If the root item is selected, new_observer will be null.
            void selectionParentChanged(Observer* new_observer);

        private:
            //! Deletes all tree items, starting with the root item.
            void deleteRootItem();
            //! Function called by rebuildTreeStructure during recursive building of the tree.
            void setupChildData(ObserverTreeItem* item);
            //! Prints the structure of the tree as trace messages.
            /*!
              \sa LOG_TRACE
              */
            void printStructure(ObserverTreeItem* item = 0, int level = 0);

        protected:
            ObserverTreeModelData* d;
        };
    }
}

#endif // OBSERVERTREEMODEL_H
