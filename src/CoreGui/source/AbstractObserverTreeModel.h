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

#ifndef ABSTRACTOBSERVERTREEMODEL_H
#define ABSTRACTOBSERVERTREEMODEL_H

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
        \struct AbstractObserverTreeModelData
        \brief Structure used by AbstractObserverTreeModel to store private data.
          */
        struct AbstractObserverTreeModelData;

        /*!
        \class AbstractObserverTreeModel
        \brief The AbstractObserverTreeModel class provides an ready-to-use model that can be used to show the children of an Observer in a QTreeView.

        This model provides helper functions for the abstract virtual functions in QAbstractItemModel. These helper functions
        will handle all these abstract functions for the columns specified by the observer context's display hints. This approach
        allows the extension of the default models.

        The Qtilities::CoreGui::ObserverTreeModel class demonstrates how the helper functions can be used.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT AbstractObserverTreeModel : public QAbstractItemModel, public AbstractObserverItemModel
        {
            Q_OBJECT

        public:
            AbstractObserverTreeModel(QObject *parent = 0);
            virtual ~AbstractObserverTreeModel() {}

            // --------------------------------
            // QAbstractItemModel Implementation
            // --------------------------------
            virtual Qt::ItemFlags flags(const QModelIndex &index) const = 0;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const = 0;
            virtual QVariant data(const QModelIndex &index, int role) const = 0;
            virtual int rowCount(const QModelIndex &parent = QModelIndex()) const = 0;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const = 0;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role) = 0;
            QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
            QModelIndex parent(const QModelIndex &index) const;
            bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
            Qt::DropActions supportedDropActions() const;

            // --------------------------------
            // AbstractObserverItemModel Implementation
            // --------------------------------
            virtual bool setObserverContext(Observer* observer);
            int columnPosition(AbstractObserverItemModel::ColumnID column_id) const;
            int getSubjectID(const QModelIndex &index) const;
            QObject* getObject(const QModelIndex &index) const;

            // --------------------------------
            // AbstractObserverTreeModel Implementation
            // --------------------------------
            //! Flags helper function.
            Qt::ItemFlags flagsHelper(const QModelIndex &index) const;
            //! Header data helper function.
            QVariant headerDataHelper(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            //! Data helper function.
            QVariant dataHelper(const QModelIndex &index, int role) const;
            //! Row count helper function.
            int rowCountHelper(const QModelIndex &parent = QModelIndex()) const;
            //! Column count helper function.
            int columnCountHelper(const QModelIndex &parent = QModelIndex()) const;
            //! Set data helper function.
            bool setDataHelper(const QModelIndex &index, const QVariant &value, int role);
            //! Function which gives the visible column position. Thus it takes into account if columns are hidden.
            int columnVisiblePosition(AbstractObserverItemModel::ColumnID column_id) const;
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
            //! Function to get the model index of an object in the tree. If the object does not exist, QModelIndex() is returned.
            QModelIndex findObject(QObject* obj) const;

        private slots:
            //! Function which will rebuild the complete tree structure under the top level observer.
            /*!
                This slot will automatically be connected to the layoutChanged() signal on the top level observer.
              */
            void rebuildTreeStructure();

        public slots:
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
            //! Function to let the model know which objects are currently selected in the view connected to this model.
            /*!
              This functionality is used when the layout of the tree changed externally (not in the view) and we
              need to rebuild the model. We attempt to select the previously selected objects when the tree
              rebuilding is done.
              */
            void setSelectedObjects(QList<QPointer<QObject> > selected_objects);

        signals:
            //! Signal which is emmited when the current selection parent changed. If the root item is selected, new_observer will be null.
            void selectionParentChanged(Observer* new_observer);
            //! This signal should be used to reselect previously selected objects if they are still present in the tree. The signal is emmited when the tree finished to rebuild itself.
            void selectObjects(QList<QPointer<QObject> > objects) const;

        private:
            //! Recursive function used by findObject() to traverse through the trying to find an object.
            QModelIndex findObject(const QModelIndex& index, QObject* obj) const;
            //! Recursive function to get the ObserverTreeItem associacted with an object.
            ObserverTreeItem* findObject(ObserverTreeItem* item, QObject* obj) const;
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
            AbstractObserverTreeModelData* d;
        };
    }
}

#endif // ABSTRACTOBSERVERTREEMODEL_H
