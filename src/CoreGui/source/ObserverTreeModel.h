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
        \struct ObserverTreeModelData
        \brief Structure used by ObserverTreeModel to store private data.
          */
        struct ObserverTreeModelData;

        /*!
        \class ObserverTreeModel
        \brief The ObserverTreeModel class provides an ready-to-use model that can be used to show the children of an Qtilities::Core::Observer in a QTreeView.

        To view the contents of an Qtilities::Core::Observer, use the setObserverContext function. This function will initialize the model and show the names
        of subjects attached to a specific observer in a QTreeView. ObserverTreeModel has knowledge of the standard subject filters which
        are included in the %Qtilities libraries. Thus it will for example automatically show check boxes etc. if the observer context has
        an Qtilities::Core::ActivityPolicyFilter installed.

        To customize this model you can simply subclass it and reimplement the virtual functions of QAbstractItemModel. This allows you to
        add columns etc. to your view. The <a class="el" href="namespace_qtilities_1_1_examples_1_1_clipboard.html">Clipboard Example</a> shows how
        to do this.

        \sa ObserverTableModel
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTreeModel : public QAbstractItemModel, public AbstractObserverItemModel
        {
            Q_OBJECT

        public:
            ObserverTreeModel(QObject *parent = 0);
            virtual ~ObserverTreeModel();

            // --------------------------------
            // QAbstractItemModel Implementation
            // --------------------------------
            virtual Qt::ItemFlags flags(const QModelIndex &index) const;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;
            virtual QVariant data(const QModelIndex &index, int role) const;
            virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
            virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
            virtual QModelIndex parent(const QModelIndex &index) const;
            virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
            virtual Qt::DropActions supportedDropActions() const;

            // --------------------------------
            // AbstractObserverItemModel Implementation
            // --------------------------------
            virtual bool setObserverContext(Observer* observer);
            int columnPosition(AbstractObserverItemModel::ColumnID column_id) const;
            int getSubjectID(const QModelIndex &index) const;
            QObject* getObject(const QModelIndex &index) const;
            void setShowModificationState(bool is_enabled);
            void refresh();

            // --------------------------------
            // ObserverTreeModel Implementation
            // --------------------------------
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
            /*!
              \param obj The object to find.
              \param column The column requested.
              \returns The QModelIndex of the specified object. If the object was not found QModelIndex() is returned.

              \note This function does exactly the same as getIndex()
             */
            QModelIndex findObject(QObject* obj, int column = -1) const;
            //! Function to get the model index of an object in the tree. If the object does not exist, QModelIndex() is returned.
            /*!
              \param obj The object to find.
              \param column The colum requested.
              \returns The QModelIndex of the specified object. If the object was not found QModelIndex() is returned.

              \note This function does exactly the same as findObject()
             */
            QModelIndex getIndex(QObject* obj, int column = -1) const;
            //! Function to get the model index of a category. If the category does not exist, QModelIndex() is returned.
            QModelIndex findCategory(QtilitiesCategory category) const;
            //! Finds the matching QModelIndex indexes for all nodes with display names specified by \p node_names.
            /*!
              \sa Qtilities::CoreGui::ObserverWidget::findExpandedItems()
              */
            QModelIndexList findExpandedNodeIndexes(const QStringList& node_names) const;
            //! Finds the matching QModelIndex indexes for all objects specified by \p objects.
            /*!
              \sa Qtilities::CoreGui::ObserverWidget::findExpandedObjects()

              <i>This function was added in %Qtilities v1.5.</i>
              */
            QModelIndexList findExpandedNodeIndexes(const QList<QPointer<QObject> >& objects) const;

        public slots:
            //! When the observer context changes, this function will take note of the change and when needed, the model will rebuild the internal tree structure using rebuildTreeStructure();
            /*!
              \param new_selection The selection of object that must be selected when the internal tree structure is rebuilt.
              */
            void recordObserverChange(QList<QPointer<QObject> > new_selection = QList<QPointer<QObject> >());
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

              \sa setSelectedCategories()
              */
            void setSelectedObjects(QList<QPointer<QObject> > selected_objects);
            //! Function to let the model know which categories is currently selected in the view connected to the model.
            /*!
              Allows selection of categories after tree rebuilds.

              \sa setSelectedObjects()
              */
            void setSelectedCategories(QList<QtilitiesCategory> selected_categories);
            //! Sets if this model must be read only, thus its actions and property editor will be read only.
            /*!
              \sa readOnly()
              */
            virtual void setReadOnly(bool read_only);

        public:
            //! Gets if this model must be read only, thus its actions and property editor will be read only.
            /*!
              \sa setReadOnly()
              */
            bool readOnly() const;
            //! Returns the persistent index list of the model.
            QModelIndexList getPersistentIndexList() const;
            //! Gets all indexes under a specified model index.
            /*!
              Just call getAllIndexes() when you want to use this function. It will automatically start with the root node.
              */
            QModelIndexList getAllIndexes(ObserverTreeItem* item = 0) const;
            //! Sets all expanded nodes and categories.
            void setExpandedItems(const QList<QPointer<QObject> > &expanded_objects, const QStringList &expanded_categories);

            //! Enables automatic selection and expansion to restore the selection and expansion state of the tree after a refresh has occured.
            /*!
              True by default.

              <i>This function was added in %Qtilities v1.5.</i>

              \sa disableAutoSelectAndExpand()
              */
            void enableAutoSelectAndExpand();
            //! Disables automatic selection and expansion to restore the selection and expansion state of the tree after a refresh has occured.
            /*!
              <i>This function was added in %Qtilities v1.5.</i>

              \sa enableAutoSelectAndExpand()
              */
            void disableAutoSelectAndExpand();

        signals:
            //! Signal which is emmited when the current selection parent changed. If the root item is selected, new_observer will be null.
            void selectionParentChanged(Observer* new_observer);
            //! This signal will be handled by a slot in the ObserverWidget parent of this model and the objects will be selected. The signal is emitted when the tree finished to rebuild itself.
            void selectObjects(QList<QPointer<QObject> > objects) const;
            //! This signal will be handled by a slot in the ObserverWidget parent of this model and the objects will be selected. The signal is emitted when the tree finished to rebuild itself.
            void selectObjects(QList<QObject*> objects) const;
            //! This signal will be handled by a slot in the ObserverWidget parent of this model and the objects will be selected. The signal is emitted when the tree finished to rebuild itself.
            void selectCategories(QList<QtilitiesCategory> categories) const;
            //! Signal which is emitted just before a tree model rebuild will start.
            void treeModelBuildAboutToStart() const;
            //! Signal which is emitted when a new tree building cycle starts.
            void treeModelBuildStarted() const;
            //! Signal which is emitted when a new tree building cycle ends.
            void treeModelBuildEnded() const;
            //! Signal which requests for a set of items to be expanded in the view.
            void expandItemsRequest(QModelIndexList indexes);

        private slots:
            //! Clears the tree structure without rebuilding it again from its observer context.
            void clearTreeStructure();
            //! Function which will rebuild the complete tree structure under the top level observer.
            void rebuildTreeStructure();
            //! Slot which receives ready-built ObserverTreeItem from ObserverTreeModelBuilder.
            void receiveBuildObserverTreeItem(ObserverTreeItem* item);

        protected:
            //! Recursive function used by findObject() to traverse through the tree trying to find an object.
            QModelIndex findObject(const QModelIndex& index, QObject* obj, int column = -1) const;
            //! Recursive function to get the ObserverTreeItem associacted with an object.
            ObserverTreeItem* findObject(ObserverTreeItem* item, QObject* obj) const;

            //! Recursive function used by findCategory() to traverse through the tree trying to find a category.
            QModelIndex findCategory(const QModelIndex& index, QtilitiesCategory category) const;
            //! Recursive function to get the ObserverTreeItem associacted with a category.
            ObserverTreeItem* findCategory(ObserverTreeItem* item, QtilitiesCategory category) const;
            //! Deletes all tree items, starting with the root item.
            void deleteRootItem();
            //! Finds the root indices.
            /*!
             * Depending on the root index display hint, there can be one or more root indices.
             */
            QList<QModelIndex> rootIndices() const;

            ObserverTreeModelData* d;
        };
    }
}

#endif // ABSTRACTOBSERVERTREEMODEL_H
