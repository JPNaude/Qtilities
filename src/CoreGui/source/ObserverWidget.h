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

#ifndef OBSERVERWIDGET_H
#define OBSERVERWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IActionProvider.h"
#include "ObjectPropertyBrowser.h"
#include "ObjectDynamicPropertyBrowser.h"
#include "ObserverTableModel.h"
#include "ObserverTreeModel.h"
#include "SearchBoxWidget.h"

#include <Observer.h>
#include <ObserverHints.h>
#include <IContext.h>
#include <Observer.h>

#include <QMainWindow>
#include <QStack>
#include <QAbstractItemModel>
#include <QModelIndexList>
#include <QTableView>
#include <QTreeView>
#include <QAbstractProxyModel>

namespace Ui {
    class ObserverWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::CoreGui::Constants;
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::Core;

        /*!
        \struct ObserverWidgetData
        \brief A structure used by ObserverWidget classes to store data.
          */
        struct ObserverWidgetData;

        /*!
          \class ObserverWidget
          \brief The ObserverWidget class provides a ready-to-use widget to display information about a specific observer context.

          The ObserverWidget class provides a ready-to-use widget to display information about a specific observer context, reducing the
          workload when you need to display data related to a specific observer context. The goal of the observer widget is to maximize the possible
          number of usage scenarios by being as customizable as possible.

          Below is an example of a basic observer widget in tree mode.

          \image html observer_widget_doc_tree_view_simple.jpg "Observer Widget (Tree View Mode)"

          The \ref page_observer_widgets article provides a comprehensive overview of the different ways that the observer widget can be used.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverWidget : public QMainWindow, public IContext {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)

        public:
            // --------------------------------
            // Core Functions
            // --------------------------------
            //! Default constructor.
            /*!
              \param display_mode The display mode that should be used.
              \param parent The parent widget.
              \param f The Qt::WindowFlags which must be used for the widget.
              */
            ObserverWidget(DisplayMode display_mode = TreeView, QWidget * parent = 0, Qt::WindowFlags f = 0);
            //! A constructor which takes the observer context to be displayed as the first parameter.
            /*!
              This constructor can be used if no functions need to be called on the observer widget object
              before initializing it. The result is less code:

\code
// The long way:
ObserverWidget* categorized_widget = new ObserverWidget();
categorized_widget->setObserverContext(contextPtr);
categorized_widget->initialize();
categorized_widget->show();

// The short way:
ObserverWidget* categorized_widget = new ObserverWidget(contextPtr);
categorized_widget->show();
\endcode

              \param observer_context The observer context to be displayed. If invalid, the constructor will not call setObserverContext() and initialize().
              \param display_mode The display mode that should be used.
              \param parent The parent widget.
              \param f The Qt::WindowFlags which must be used for the widget.
              */
            ObserverWidget(Observer* observer_context, DisplayMode display_mode = TreeView, QWidget * parent = 0, Qt::WindowFlags f = 0);
            //! Default destructor.
            virtual ~ObserverWidget();

        private:
            void constructPrivate(DisplayMode display_mode = TreeView, Observer* observer_context = 0);

        public:
            //! Sets the observer context.
            bool setObserverContext(Observer* observer_ptr);
            //! Gets a pointer to the observer context.
            Observer* observerContext() const;
            //! Provides a pointer to the current selection's parent. If no objects are selected, 0 is returned.
            Observer* selectionParent() const;

            //! The possible refresh modes of an ObserverWidget.
            /*!
             * Controls how ObserverWidget handles changes to the underlying Observer in TreeView mode.
             *
             * The default is RefreshModeShowTree. Before %Qtilities v1.5 the default behaviour was RefreshModeHideTree.
             *
             * \sa setRefreshMode(), setRefreshMode()
             *
             * <i>This enum was added in %Qtilities v1.5.</i>
             */
            enum RefreshMode {
                RefreshModeHideTree,   /*!< Hides the tree view while the internal tree is rebuilt, and a show a progress widget in its place. */
                RefreshModeShowTree    /*!< Shows the tree as normal, and just change the cursor of the tree widget to Qt::WaitCursor. */
            };          
            //! Sets the refresh mode of the item view.
            /*!
             * \param refresh_mode The refresh mode to use.
             *
             * \note You must call this function before initialize() for it to have any effect.
             *
             * \sa refreshMode()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void setRefreshMode(RefreshMode refresh_mode);
            //! Gets the refresh mode of the item view.
            /*!
             * The default is: RefreshModeShowTree
             *
             * \returns The refresh mode of the item view.
             *
             * \sa setRefreshMode()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            RefreshMode refreshMode() const;

            //! Enables/disables lazy initialization.
            /*!
              Lazy initialization means that the internal item view model won't initialize itself during the initialize() call. Its usefull when
              using custom implemented proxy models etc. To initialize the model in lazy initialization mode call refresh().

              Lazy initialization is disabled by default.

              \note You must call this function before initialize() for it to have any effect.
              \note Only usefull when in TreeView mode, in TableView mode this does not have any effect.
              \note After the first initialize() call, lazy initialization is always turned off in order for display mode switching to work properly.

              \sa lazyInitEnabled()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void toggleLazyInit(bool enabled);
            //! Gets if lazy initialization is enabled.
            /*!
              \sa toggleLazyInit()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool lazyInitEnabled() const;

            //! Initializes the observer widget. Make sure to set the item model as well as the flags you would like to use before calling initialize.
            /*!
              \sa toggleLazyInit(), lazyInitEnabled()
              */
            virtual void initialize();
            //! Initializes the observer widget, make an initial selection and expand specified items.
            /*!
              \sa toggleLazyInit(), lazyInitEnabled()

              \param initial_selection The objects which should be selected after the view was initialized.
              \param expanded_items The nodes which should be expanded after the tree was initialized when in tree view mode (thus, only usefull in TreeView mode).

              \note This constructor is only usefull when not doing lazy initialization.

              <i>This function was added in %Qtilities v1.5.</i>
              */
            virtual void initialize(QList<QPointer<QObject> > initial_selection, QList<QPointer<QObject> > expanded_items = QList<QPointer<QObject> >());

        private:
            //! Initializes the activity for the selection parent when using FollowSelection.
            void initializeFollowSelectionActivityFilter(bool inherit_activity_filter_activity_selection);
            //! Initializes the observer widget. Make sure to set the item model as well as the flags you would like to use before calling initialize.
            /*!
              \sa toggleLazyInit(), lazyInitEnabled()
              */
            virtual void initializePrivate(bool hints_only);

        public:
            //! Gets the current navigation stack of this widget.
            QStack<int> navigationStack() const;
            //! Allows you to set the navigation stack of this widget.
            /*!
              The navigation stack keeps track of the user's navigation history of events related to pushing down into observers and
              pushing up into parents of observers. Using this function you can initialize the widget using a predefined stack. The
              stack stores IDs of observers assigned by the Qtilities::CoreGui::ObjectManager singleton.

              \sa
                - initialize()
              */
            void setNavigationStack(QStack<int> navigation_stack);
            //! Returns the observer ID of the top level observer in the widget. If the top level observer is not defined, -1 is returned.
            int topLevelObserverID();
            //! Event filter which has responsibilities such as drag and drop operations etc.
            bool eventFilter(QObject *object, QEvent *event);

            //! Sets the mouse button to use for drag and drop move operations.
            /*!
              Setting the button to Qt::NoButton disables the move drag/drop functionality.

              \sa dragDropMoveButton()
              */
            void setDragDropMoveButton(Qt::MouseButton mouse_button);
            //! Gets the mouse button to use for drag and drop move operations.
            /*!
              By default Qt::RightButton.

              \sa setDragDropMoveButton()
              */
            Qt::MouseButton dragDropMoveButton() const;
            //! Sets the mouse button to use for drag and drop copy operations.
            /*!
              Setting the button to Qt::NoButton disables the copy drag/drop functionality.

              \sa dragDropCopyButton()
              */
            void setDragDropCopyButton(Qt::MouseButton mouse_button);
            //! Gets the mouse button to use for drag and drop copy operations.
            /*!
              By default Qt::RightButton.

              \sa setDragDropCopyButton()
              */
            Qt::MouseButton dragDropCopyButton() const;

            //! Function which sets a custom table model to be used in this widget when its in TableView mode.
            /*!
              By default the observer widget uses the Qtilities::CoreGui::ObserverTableModel as the model for the table view.
              It is possible to use custom models which allows you to extend the default table model.

              \note This function must be called before initializing the widget for the first time.

              The observer widget will manage the lifetime of the custom model.

              \returns True if the model was successfully set.
              */
            bool setCustomTableModel(ObserverTableModel* table_model);
            //! Function which sets a custom tree model to be used in this widget when its in TreeView mode.
            /*!
              By default the observer widget uses the Qtilities::CoreGui::ObserverTreeModel as the model for the tree view.
              It is possible to use custom models which allows you to extend the default tree model.

              The observer widget will manage the lifetime of the custom model.

              \note This function must be called before initializing the widget for the first time.

              \returns True if the model was successfully set.
              */
            bool setCustomTreeModel(ObserverTreeModel* tree_model);

            //! Function which sets a custom table view to be used in this widget when its in TableView mode.
            /*!
              \note This function must be called before initializing the widget for the first time.

              The observer widget will manage the lifetime of the custom model.

              <i>This function was added in %Qtilities v1.5.</i>
              */
            void setCustomTableView(QTableView* table_view);
            //! Function which sets a custom tree view to be used in this widget when its in TreeView mode.
            /*!
              The observer widget will manage the lifetime of the custom model.

              \note This function must be called before initializing the widget for the first time.

              <i>This function was added in %Qtilities v1.5.</i>
              */
            void setCustomTreeView(QTreeView *tree_view);

            //! Function which sets a custom table proxy model to be used in this widget when its in TableView mode.
            /*!
              By default the observer widget uses the Qtilities::CoreGui::ObserverTableModelProxyFilter as the proxy model for the table view.
              It is possible to use custom proxy models which allows you to extend the default proxy model.

              The observer widget will manage the lifetime of the custom model.

              \note This function must be called before initializing the widget for the first time.

              \returns True if the proxy model was successfully set.
              */
            bool setCustomTableProxyModel(QAbstractProxyModel* proxy_model);
            //! Function which sets a custom tree proxy model to be used in this widget when its in TreeView mode.
            /*!
              By default the observer widget uses the Qtilities::CoreGui::ObserverTreeModelProxyFilter as the proxy model for the tree view. It is possible to use custom proxy models which allows you to extend the default proxy model.

              The observer widget will manage the lifetime of the custom model.

              \note This function must be called before initializing the widget for the first time.

              \returns True if the proxy model was successfully set.
              */
            bool setCustomTreeProxyModel(QAbstractProxyModel* proxy_model);

            ObserverTableModel* tableModel() const;
            ObserverTreeModel* treeModel() const;
            QAbstractProxyModel* proxyModel() const;

            //! Disables usage of proxy filter models.
            /*!
              This function must be called before initializing the widget for the first time, and it cannot be enabled once it was disabled.

              \note When disabling proxy models, the search box is not available. See toggleSearchBox().
              */
            void disableProxyModels() const;

            //! Sets the display mode of the widget.
            /*!
              \sa displayMode(), toggleDisplayMode()
              */
            void setDisplayMode(DisplayMode display_mode);
            //! Gets the display mode of the widget.
            /*!
              \sa setDisplayMode(), toggleDisplayMode()
              */
            DisplayMode displayMode() const;
            //! Sets if this widget must be read only, thus its actions and property editor will be read only.
            /*!
              \note Make sure you only call this function after the widget has been initialized through initialize().

              \sa readOnly(), readOnlyStateChanged()
              */
            virtual void setReadOnly(bool read_only);
            //! Gets if this widget must be read only, thus its actions and property editor will be read only.
            /*!
              \sa setReadOnly(), readOnlyStateChanged()
              */
            bool readOnly() const;
            //! Finds all expanded item names in the current view.
            /*!
             * It should not be neccesarry to use this function directly, rather use lastExpandedItemsResults() since the internal list of expanded items
             * is automatically updated whenever the expansion state of any item in the view changes.
             *
             * Use this function only if you want to force recalculation of the expanded items.
             *
             * \note If the tree contains duplicate items, this list will also contain duplicate items. If uniqueness is
             * required, use findExpandedObjects() instead.
             *
             * \note Only usefull when displayMode() is Qtilities::TreeView.
             *
             * \sa expandNodes(), lastExpandedItemsResults()
             *
             * <i>This function was added in %Qtilities v1.1.</i>
             */
            QStringList findExpandedItems();
            //! Returns the last set of calculated expanded items from the last findExpandedItems() call without calculating them again.
            /*!
             * When findExpandedItems() is called it stores the results internally. This function accesses those results without
             * recalculating them.
             *
             * \note If the tree contains duplicate items, this list will also contain duplicate items. If uniqueness is
             * required, use findExpandedObjects() instead.
             *
             * \sa findExpandedItems(), expandNodes(), findExpandedObjects(), lastExpandedObjectsResults(), findExpandedCategories(), lastExpandedCategoriesResults()
             *
             * <i>This function was added in %Qtilities v1.1.</i>
             */
            QStringList lastExpandedItemsResults() const;
            //! Finds all expanded objects in the current view.
            /*!
             * It should not be neccesarry to use this function directly, rather use lastExpandedObjectsResults() since the internal list of expanded items
             * is automatically updated whenever the expansion state of any item in the view changes.
             *
             * Use this function only if you want to force recalculation of the expanded items.
             *
             * \note Only usefull when displayMode() is Qtilities::TreeView.
             *
             * \sa expandNodes(), lastExpandedItemsResults(), findExpandedItems(), expandNodes(), findExpandedCategories(), lastExpandedCategoriesResults()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            QList<QPointer<QObject> > findExpandedObjects();
            //! Returns the last set of calculated expanded objects from the last findExpandedObjects() call without calculating them again.
            /*!
             * When findExpandedObjects() is called it stores the results internally. This function accesses those results without
             * recalculating them.
             *
             * \sa findExpandedItems(), expandNodes(), findExpandedItems(), expandNodes(), findExpandedCategories(), lastExpandedCategoriesResults()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            QList<QPointer<QObject> > lastExpandedObjectsResults() const;
            //! Finds all expanded categories in the current view.
            /*!
             * It should not be neccesarry to use this function directly, rather use lastExpandedCategoriesResults() since the internal list of expanded items
             * is automatically updated whenever the expansion state of any item in the view changes.
             *
             * Use this function only if you want to force recalculation of the expanded items.
             *
             * \note Only usefull when displayMode() is Qtilities::TreeView.
             *
             * \sa expandNodes(), lastExpandedItemsResults(), findExpandedItems(), expandNodes(), lastExpandedCategoriesResults()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            QStringList findExpandedCategories();
            //! Returns the last set of calculated expanded categories from the last findExpandedCategories() call without calculating them again.
            /*!
             * When findExpandedCategories() is called it stores the results internally. This function accesses those results without
             * recalculating them.
             *
             * \sa findExpandedItems(), expandNodes(), findExpandedItems(), expandNodes(), findExpandedCategories()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            QStringList lastExpandedCategoriesResults() const;

        private:
            //! Function used internally by findExpandedObjects() and findExpandedItems() to avoid doing duplicate work.
            void updateLastExpandedResults(const QModelIndex& to_add = QModelIndex(), const QModelIndex& to_remove = QModelIndex());

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QWidget, ObserverWidget> factory;           

        private slots:
            void contextDeleted();
            //! The context detach handler check if any observer in the current context's parent hierarchy is deleted. If so, contextDeleted() is called.
            void contextDetachHandler(Observer::SubjectChangeIndication indication, QList<QPointer<QObject> > obj);
            //! Slot which will call the handleSearchStringChanged() slot with an empty QString as parameter.
            void resetProxyModel();
            //! Adapts the size of columns when data changes.
            void adaptColumns(const QModelIndex & topleft, const QModelIndex& bottomRight);
            //! Slot which listens for treeModelBuildAboutToStart() on tree models in order to set the expanded items on them.
            void handleTreeModelBuildAboutToStart();
            //! Slot which listens for the expanded() signal on the tree view if in Qtilities::TreeView mode in order to emit the latest expansion details using expandedNodesChanged().
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void handleExpanded(const QModelIndex & index);
            //! Slot which listens for the collapsed() signal on the tree view if in Qtilities::TreeView mode in order to emit the latest expansion details using expandedNodesChanged().
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void handleCollapsed(const QModelIndex &index);

        public slots:
            //! Resizes columns to their match their contents.
            void resizeColumns();
            //! Expand all nodes for which their display names matches the \p node_names parameters.
            /*!
              If any name in node_names does not exist in the tree it is ignored.

              \param node_names The node names to expand.

              \sa findExpandedItems(), expandObjects(), findExpandedCategories()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void expandNodes(const QStringList& node_names);
            //! Expand all nodes which represents objects in the given list.
            /*!
              If any object in objects does not exist in the tree it is ignored.

              \param objects The objects to expand.

              \sa findExpandedObjects(), findExpandedItems(), findExpandedCategories()

              <i>This function was added in %Qtilities v1.5.</i>
              */
            void expandObjects(const QList<QPointer<QObject> >& objects);
            //! Expand all specified categories.
            /*!
              If any category in the list does not exist in the tree it is ignored.

              \param category_names The categories to expand.

              \sa findExpandedObjects(), findExpandedItems(), findExpandedCategories()

              <i>This function was added in %Qtilities v1.5.</i>
              */
            void expandCategories(const QStringList& category_names);
            //! Expand all nodes specified by the indexes in \p indexes.
            /*!
              If any name in node_names does not exist in the tree it is ignored.

              \param indexes The indexes to expand. When empty, viewExpandAll() will be called.

              \sa findExpandedItems()
              */
            void expandNodes(QModelIndexList indexes);

        signals:
            //! Signal which is emitted when the expanded nodes changes.
            /*!
              This function was added in Qtilities v1.1.
              */
            void expandedNodesChanged(const QStringList& node_names);
            //! Signal which is emitted when the expanded nodes changes.
            /*!
              This function was added in Qtilities v1.5.
              */
            void expandedObjectsChanged(const QList<QPointer<QObject> >& objects);
            //! Signal which is emitted when the observer context of this widget changes.
            void observerContextChanged(Observer* new_context);
            //! Signal which is emitted when this widget's read only state changes.
            /*!
              \sa readOnly(), setReadOnly()
              */
            void readOnlyStateChanged(bool read_only);

            // --------------------------------
            // IContext implementation
            // --------------------------------
        public:
            //! In the case of an ObserverWidget, the contextString() is the same as the globalMetaType().
            QString contextString() const { return globalMetaType(); }
            QString contextHelpId() const { return QString(); }

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // Functions Related To Display Hints
            // --------------------------------
            //! Function to toggle usage of hints from the active parent observer. If not default hints will be used.
            /*!
                \sa activeHints(), setCustomHints()
              */
            void toggleUseObserverHints(bool toggle);
            //! Indicates if this widget uses its own custom hints or that of the active observer.
            bool usesObserverHints() const;
            //! This function allows you to copy the custom hints used by this ObserverWidget from a different ObserverHints instance.
            /*!
              This functions copies the hints provided by custom_hints, thus it does not manage the lifetime of the custom_hints instance passed to it.

              \note These custom hints are only used when usesObserverHints() is false.

              \return True if successfull, false otherwise.

              \sa toggleUseObserverHints(), activeHints()
              */
            bool setCustomHints(ObserverHints* custom_hints);
            //! This function will provide the hints which should be used by this widget at any time.
            /*!
              \sa toggleUseObserverHints(), setCustomHints()
              */
            ObserverHints* activeHints() const;

            //! Sets if the "Expand All"/"Collapse All" actions should be visible when in TreeView mode.
            /*!
              \sa treeExpandCollapseVisible()

              <i>This function was added in %Qtilities v1.2.</i>
              */
            void setTreeExpandCollapseVisible(bool is_visible);
            //! Gets if the "Expand All"/"Collapse All" actions should be visible when in TreeView mode.
            /*!
             *This is true by default.
             *\sa setTreeExpandCollapseVisible()
             *
             *<i>This function was added in %Qtilities v1.2.</i>
              */
            bool treeExpandCollapseVisible() const;

            // --------------------------------
            // Global Meta Type and Action Provider Functions
            // --------------------------------
        public:
            //! Function which allows this observer widget to share global object activity with other observer widgets.
            /*!
              This function will allow this observer widget to share global object activity with other observer widgets.
              Because the globalMetaType() for each observer widget must be unique, it is required to use a shared meta type
              for cases where global object activity needs to be shared between multiple observer widgets.

              When using a shared global activity meta type, the normal globalMetaType() will be used for all the normal
              usage scenarios listed in the globalMetaType() documentation, except for the meta type used to identify a set of active
              objects in the object manager.

              \sa sharedGlobalMetaType(), globalMetaType(), setGlobalMetaType(), updateGlobalActiveSubjects()
              */
            void setSharedGlobalMetaType(const QString& shared_meta_type);
            //! Function to get the shared global activity meta type of this observer widget.
            /*!
              \returns The shared global activity meta type. If this feature is not used, QString() will be returned.

              \sa sharedGlobalMetaType(), globalMetaType(), setGlobalMetaType(), updateGlobalActiveSubjects()
              */
            QString sharedGlobalMetaType() const;
            //! Sets the global meta type used for this observer widget.
            /*!
              \returns True if the meta_type string was valid. The validity check is done by checking if that a context with the same name does not yet exist in the context manager.

              \sa globalMetaType(), sharedGlobalMetaType()
              */
            bool setGlobalMetaType(const QString& meta_type);
            //! Gets the global meta type used for this observer widget.
            /*!
              The global meta type is a string which defines this observer widget. The string must be a string which
              can be registered in the context manager. Thus, such a string must not yet exist as a context in the context
              manager.

              The global meta type is used for the following:
              - As the context which is used to register backends for any actions created by this widget.
              - During readSettings() and writeSettings() to uniquely define this widget.
              - As the meta type which is used to identify a set of active objects in the object manager. For more information see Qtilities::Core::Interfaces::IObjectManager::metaTypeActiveObjects().
              - It is recommended to use the global meta type as the request ID when monitoring settings update requests. See handleSettingsUpdateRequest()

              \returns The meta type used for this observer widget.

              \sa setGlobalMetaType(), updateGlobalActiveSubjects(), setSharedGlobalMetaType(), sharedGlobalMetaType()
              */
            QString globalMetaType() const;
            //! Function to toggle if this observer widget updates global active objects under its globalMetaType() meta type.
            /*!
              For more information on global active objects, see the \ref meta_type_object_management section of the \ref page_object_management article.

              \sa useGlobalActiveObjects
              */
            void toggleUseGlobalActiveObjects(bool toggle);
            //! Indicates if this observer widget updates global active objects.
            /*!
              \sa toggleUseGlobalActiveObjects();
              */
            bool useGlobalActiveObjects() const;

            // --------------------------------
            // Functions Related To Selected Objects & Refreshing Of The Views
            // --------------------------------
            //! Function to set the interaction with the user when deleting subjects.
            /*!
              \sa confirmDeletes(), selectionDelete(), selectionDeleteAll()
              */
            void setConfirmDeletes(bool confirm_deletes);
            //! Function to get the interaction with the user when deleting subjects.
            /*!
              The default is true.

              \sa setConfirmDeletes(), selectionDelete(), selectionDeleteAll()
              */
            bool confirmDeletes() const;
            //! Provides a list of QObject pointers to all the selected objects.
            /*!
              \sa selectedObjectsChanged()
              */
            QList<QObject*> selectedObjects() const;
            //! Checks if all current selectedObjects() are in the same context.
            /*!
              \sa selectedObjectsChanged(), selectedObjects()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool selectedObjectsContextMatch() const;
            //! Checks if all current selectedObjects() share the same ObserverHints.
            /*!
              When selectedObjectsContextMatch() is false, this function allows you to check if all selected subjects shares the same hints. This can happen in two scenarios:
              - When usesObserverHints() is false the hints of all subjects will always match.
              - When the selected objects have observer parents with exactly the same hints.

              \sa selectedObjectsChanged(), selectedObjects()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool selectedObjectsHintsMatch() const;
            //! Provides a list of QModelIndexes which are currently selected. Use this call instead of the item model selection's selectedIndexes() call since this function will map the indexes from the proxy model's indexes to the real model's indexes.
            QModelIndexList selectedIndexes() const;
            //! Function to set the default height used for the table view when this widget is used in TableView mode.
            /*!
              The default is 17.

              \sa defaultRowHeight()
              */
            void setDefaultRowHeight(int height);
            //! Function to get the default height used for the table view when this widget is used in TableView mode.
            /*!
              \sa setDefaultRowHeight()
              */
            int defaultRowHeight() const;
            //! Function providing access to the contained table view.
            /*!
              \note This reference is only valid in TableView mode.

              \sa treeView(), view()
              */
            QTableView* tableView();
            //! Function providing access to the contained tree view.
            /*!
              \note This reference is only valid in TreeView mode.

              \sa tableView(), view()
              */
            QTreeView* treeView();
            //! Function providing access to the QAbstractItemView view base class.
            /*!
              This function is usefull when you want to access functionality on the current view which is common
              to both QTreeView and QTableView, without needing to care about the view type.

              \note This reference is only valid in TreeView and TableView modes.

              \sa tableView(), treeView()

              <i>This function was added in %Qtilities v1.2.</i>
              */
            QAbstractItemView* view();
            //! Enables automatic column resizing in the displayed table or tree view, depending on the displayMode().
            /*!
              True by default which will cause the view's columns to be resized to their contents and the name column to be streched. If you
              would like to do this manually, for example if you want to stretch a custom column, you should disable this.

              \sa disableAutoColumnResizing()
              */
            void enableAutoColumnResizing();
            //! Disables automatic column resizing in the displayed table or tree view, depending on the displayMode().
            /*!
              \sa enableAutoColumnResizing()
              */
            void disableAutoColumnResizing();
            //! Enables automatic selection and expansion in TreeView mode to restore the selection and expansion state of the tree after a refresh has occured.
            /*!
              True by default.

              <i>This function was added in %Qtilities v1.5.</i>

              \sa disableAutoSelectAndExpand()
              */
            void enableAutoSelectAndExpand();
            //! Disables automatic selection and expansion in TreeView mode to restore the selection and expansion state of the tree after a refresh has occured.
            /*!
              <i>This function was added in %Qtilities v1.5.</i>

              \sa enableAutoSelectAndExpand()
              */
            void disableAutoSelectAndExpand();

        private slots:
            //! Updates the current selection parent context.
            /*!
              \note This function is only used in TreeView mode.
              */
            void setTreeSelectionParent(Observer* observer);
            //! Handles notification from the tree model that a new tree rebuilding operation has started.
            /*!
              \note This function is only used in TreeView mode.
              */
            void handleTreeRebuildStarted();
            //! Handles notification from the tree model that the tree has been rebuilt.
            /*!
              \note This function is only used in TreeView mode.
              */
            void handleTreeRebuildCompleted(bool emit_tree_build_completed = true);

            //! Updates the view's current selection from an activity change in the observer's activity filter when using FollowSelection.
            void updateSelectionFromActivityFilter(QList<QObject*> objects);

        public slots:
            //! Sets the global object subject type used by this observer widget.
            /*!
              If objects are selected, they are set as the active objects. If no objects are selected, the observer context
              is set as the active object.

              For more information see the \ref meta_type_object_management section of the \ref page_object_management article.
              */
            void updateGlobalActiveSubjects();
            //! Selects the specified objects in the active item view.
            /*!
              \param objects The objects that must be selected. If any objects in the list are not present in the view, they will be ignored. If the list is empty, nothing will happen.

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.

              \sa selectedObjectsChanged(), clearSelection(), selectObject(), selectCategories(), selectCategory()
              */
            void selectObjects(QList<QObject*> objects);
            //! Selects the specified object in the active item view.
            /*!
              \param object The object that must be selected. If the object is not present in the view, it will be ignored.

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.

              \sa selectedObjectsChanged(), clearSelection(), selectObjects(), selectCategories(), selectCategory()
              */
            void selectObject(QObject* object);
            //! Selects the specified categories in the active item view.
            /*!
              \param categories The categories that must be selected. If any categories in the list are not present in the view, they will be ignored. If the list is empty, nothing will happen.

              \sa selectedObjectsChanged(), clearSelection(), selectObject(), selectObjects(), selectCategory()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void selectCategories(QList<QtilitiesCategory> categories);
            //! Selects the specified object in the active item view.
            /*!
              \param category The category that must be selected. If the category is not present in the view, it will be ignored.

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.
              \note This function only does something when displayMode() is Qtilities::TreeView.

              \sa selectedObjectsChanged(), clearSelection(), selectObject(), selectObjects(), selectCategories()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void selectCategory(QtilitiesCategory category);
            //! Clears the current selection in the view.
            /*!
              \sa selectedObjectsChanged(), selectObjects(), selectObject()
              */
            void clearSelection();
            //! Selects the specified objects in a smart pointer list in the active item view.
            /*!
              \param objects The objects that must be selected. If any objects in the list are not present in the view, they will be ignored. If the list is empty, the current selection will be cleared (or you could use clearSelection()).

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.

              \sa selectedObjectsChanged(), clearSelection(), selectObject()
              */
            void selectObjects(QList<QPointer<QObject> > objects);
            //! Slot which resizes the rows in table view mode.
            /*!
              Slot which resizes the rows in table view mode.

              \param height The height which must be used. By default the default row heigth of 17 pixels is used.

              \sa defaultRowHeight(), setDefaultRowHeight()
              */
            void resizeTableViewRows(int height = -1);
            //! Handles the selection model change.
            /*!
              This function is called whenever the selection in the item view changes. The function
              will handle the selection change and then emit selectedObjectChanged().

              In TreeView mode, the function will call the Qtilities::CoreGui::ObserverTreeModel::calculateSelectionParent()
              function to update the selection parent in the tree. Once the selection parent is updated, the setTreeSelectionParent()
              function will be called which will call initialize(true) on this widget in order to initialize the
              widget for the new selection parent.
              */
            void handleSelectionModelChange();
        signals:
            //! Signal which is emitted when the display mode of this widget is toggled.
            void displayModeChanged(Qtilities::DisplayMode display_mode);
            //! Signal which is emitted when object selection changes.
            void selectedObjectsChanged(QList<QObject*> selected_objects, Observer* selection_parent = 0);

            // --------------------------------
            // Item Widgets Related Functions
            // --------------------------------
        public:
            #ifdef QTILITIES_PROPERTY_BROWSER
            //! Returns the property editor used inside the observer widget. This can be 0 depending on the display flags used. Always call this function after initialize().
            /*!
              \sa propertyBrowserDock(), dynamicPropertyBrowserDock(), dynamicPropertyBrowser()
              */
            ObjectPropertyBrowser* propertyBrowser();
            //! Returns the dock widget containing the property browser.
            /*!
              \sa propertyBrowser(), dynamicPropertyBrowserDock(), dynamicPropertyBrowser()
              */
            QDockWidget* propertyBrowserDock();
            //! Returns the dynamic property editor used inside the observer widget. This can be 0 depending on the display flags used. Always call this function after initialize().
            /*!
              \sa dynamicPropertyBrowserDock(), propertyBrowserDock(), propertyBrowser()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            ObjectDynamicPropertyBrowser* dynamicPropertyBrowser();
            //! Returns the dock widget containing the dynamic property browser.
            /*!
              \sa dynamicPropertyBrowser(), propertyBrowserDock(), propertyBrowser()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            QDockWidget* dynamicPropertyBrowserDock();
            #endif
            //! Returns the SearchBoxWidget contained inside the observer widget.
            /*!
              \returns The search box widget instance, if it does not exist null is returned.
              */
            SearchBoxWidget* searchBoxWidget();

        public slots:
            #ifdef QTILITIES_PROPERTY_BROWSER
            //! Sets the desired area of the property editor (if it is used by the observer context).
            /*!
              This area will be used to position the property editor dock widget when the widget is first shown during a session. Afterwards the widget will remember where the dock widget is.
              */
            void setPreferredPropertyEditorDockArea(Qt::DockWidgetArea property_editor_dock_area);
            //! Sets the desired type of the property editor (if it is used by the observer context).
            /*!
              The property editor type must be set before calling initialize().
              */
            void setPreferredPropertyEditorType(ObjectPropertyBrowser::BrowserType property_editor_type);
            //! Sets the desired property browser filter list to be used when constructing a property browser in this widget.
            void setPreferredPropertyFilter(QStringList filter_list, bool inversed_filter = false);

            //! Sets the desired area of the dynamic property editor (if it is used by the observer context).
            /*!
              This area will be used to position the dynamic property editor dock widget when the widget is first shown during a session. Afterwards the widget will remember where the dock widget is.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setPreferredDynamicPropertyEditorDockArea(Qt::DockWidgetArea property_editor_dock_area);
            //! Sets the desired type of the dynamic property editor (if it is used by the observer context).
            /*!
              The dynamic property editor type must be set before calling initialize().

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setPreferredDynamicPropertyEditorType(ObjectDynamicPropertyBrowser::BrowserType property_editor_type);
//            //! Sets the desired dynamic property browser filter list to be used when constructing a property browser in this widget.
//            /*!
//              <i>This function was added in %Qtilities v1.1.</i>
//              */
//            void setPreferredDynamicPropertyFilter(QStringList filter_list, bool inversed_filter = false);
        protected:
            //! Constructs the property browser and show it. If it already exists, this function does nothing.
            void constructPropertyBrowser();
            //! Refreshes the property browser, thus hide or show it depending on the active display flags.
            void refreshPropertyBrowser();
            //! Constructs the dynamic property browser and show it. If it already exists, this function does nothing.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void constructDynamicPropertyBrowser();
            //! Refreshes the dynamic property browser, thus hide or show it depending on the active display flags.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void refreshDynamicPropertyBrowser();
            #endif
        public:
            //! Refreshes the action toolbar with the current state of the actions.
            void refreshActionToolBar(bool force_full_refresh = false);
        protected:
            //! Deletes all the current action toolbars.
            void deleteActionToolBars();

            // --------------------------------
            // Action Handlers and Related Functions
            // --------------------------------
        public:
            //! Returns the action handler interface for this observer widget.
            /*!
              All constructed actions for the current observer context will be available through this function.

              Actions are grouped into the following categories:
              - Items : Actions related to actions that can be performed on selected items.
              - View : Actions related to actions that can be performed on the item view (such as switching it etc.)
              - Hierarchy : Actions related observer tree hierarhcies.

              It is possible to add actions to these categories.
              */
            IActionProvider* actionProvider();
        public slots:
            //! Detaches the current selected objects in the item view from the current selection parent.
            /*!
              \sa selectionParent(), selectedObjects(), selectedObjectsChanged()
              */
            virtual void selectionDetach();
            //! Detaches all subjects from the current selection parent.
            /*!
              \sa selectionParent(), selectedObjects(), selectedObjectsChanged()
              */
            virtual void selectionDetachAll();
            //! Deletes the current selected items.
            /*!

              \sa selectedObjects(), selectedObjectsChanged(), setConfirmDeletes(), confirmDeletes()
              */
            virtual void selectionDelete();
            //! Deletes all subjects under the current selection parent or in the selected context.
            /*!
              \sa selectionParent(), selectedObjects(), selectedObjectsChanged(), setConfirmDeletes(), confirmDeletes()
              */
            virtual void selectionDeleteAll();

        private:
            void selectionRemoveItems(bool delete_items);
            void selectionRemoveAll(bool delete_all);

        public slots:
            //! Refreshes the current item view.
            /*!
              This function will emit the refreshViewsData() signal on the top level observer context.
              The refresh operation will be followed by calling selectObjects() with the same objects
              which were selected before calling this function was called. The function will then call
              refreshAction() before it exists.

              \sa topLevelObserverID(), addActionNewItem_triggered(), refreshActions();
              */
            virtual void refresh();
            //! In TableView mode this function can be used to push up to (set the observer context of the widget) to the current selection parent.
            /*!
              \sa selectionParent(), selectionPushUpNew()
              */
            virtual void selectionPushUp();
            //! In TableView mode this function can be used to push up to (set the observer context of the widget) to the current selection parent in a new ObserverWidget.
            /*!
              \sa selectionParent(), selectionPushUp()
              */
            virtual void selectionPushUpNew();
            //! In TableView mode this function can be used to push down into (set the observer context of the widget) to the current selected observer.
            /*!
              This function only does something if an observer or an object which contains an observer is selected.

              \sa selectionParent(), selectionPushDownNew()
              */
            virtual void selectionPushDown();
            //! In TableView mode this function can be used to push down into (set the observer context of the widget) to the current selected observer in a new ObserverWidget.
            /*!
              This function only does something if an observer or an object which contains an observer is selected.

              \sa selectionParent(), selectionPushDown()
              */
            virtual void selectionPushDownNew();
            //! Toggles the display mode of the ObserverWidget.
            /*!
              \sa setDisplayMode(), displayMode()
              */
            virtual void toggleDisplayMode();
            //! Function to copy the current selection to the application clipboard.
            virtual void selectionCopy();
            //! Function to cut the current selection to the application clipboard.
            virtual void selectionCut();
            //! Function which is connected to the Qtilities::CoreGui::Actions::qti_action_EDIT_PASTE action if it exists.
            /*!
              When initializing the %Qtilities clipboard manager the paste action will be created automatically.
              */
            virtual void handle_actionPaste_triggered();

            //! Toggles the visibility of the SearchBoxWidget at the bottom of the ObserverWidget.
            /*!
              \note When proxy models are disabled, the search box is not available and this function does nothing. See disableProxyModels().

              \sa setCheckedSearchBoxItemFilters()
              */
            void toggleSearchBox();
            //! Sets the active item type filters in the menu on the search box widget used in this ObserverWidget.
            /*!
             * \sa searchBoxCheckedItemFilters(), toggleSearchBox()
             *
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void setSearchBoxCheckedItemFilters(ObserverTreeItem::TreeItemTypeFlags item_filter_flags);
            //! Gets the active item type filters in the menu on the search box widget used in this ObserverWidget.
            /*!
             * By default, ObserverTreeItem::TreeItem
             *
             * \sa setSearchBoxCheckedItemFilters(), toggleSearchBox()
             *
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            ObserverTreeItem::TreeItemTypeFlags searchBoxCheckedItemFilters() const;
            //! Collapse all items in the tree view to a depth of 1 in TreeView mode.
            virtual void viewCollapseAll();
            //! Expand all items in the tree view in TreeView mode.
            virtual void viewExpandAll();
            //! Handles search options changes in the SearchBoxWidget if present.
            void handleSearchOptionsChanged();
            //! Handles search string changes in the SearchBoxWidget if present.
            void handleSearchStringChanged(const QString& filter_string);
            //! Handle changes to the type of items which must be filtered.
            void handleSearchItemTypesChanged();
            //! Refreshes the state of all actions.
            void refreshActions();

        private slots:
            //! Handle post layout changed actions in table view mode.
            void handleLayoutChangeCompleted();
            //! This function is triggered by the Qtilities::Core::ObserverHints::ActionNewItem action.
            virtual void handle_actionNewItem_triggered();
            #ifndef QT_NO_DEBUG
            void selectionDebug() const;
            #endif

        protected:
            //! Constructs actions inside the observer widget.
            void constructActions();

        signals:
            //! Signal which is emitted when a new tree building cycle starts (Only when in TreeView mode).
            void treeModelBuildStarted() const;
            //! Signal which is emitted when a new tree building cycle ends (Only when in TreeView mode).
            void treeModelBuildEnded() const;
            //! Signal which is emitted when the add new item action is triggered.
            /*!
              The parameters used during this signal emission is defferent depending on the display mode and the
              selected objects. The following are the possible scenarios:

              <b>1) Table View Mode: no object selected:</b>

              \param object The context displayed in the table.
              \param parent_observer The last context in the navigation stack, if no such context exists 0 is returned.

              <b>2) Table View Mode: 1 object selected:</b>

              If an observer is selected with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is selected with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The context currently displayed in the table view.

              <b>3) Table View Mode: multiple objects selected:</b>

              \param object Null (0).
              \param parent_observer The context currently displayed in the table view.

              <b>4) Tree View Mode: no objects selected:</b>

              \param object Null (0).
              \param parent_observer The last context in the navigation stack, if no such context exists 0 is returned.

              <b>5) Tree View Mode: 1 object selected:</b>

              If an observer is selected with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is selected with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The parent context of the selected object. If it does not have a parent, 0.

              <b>6) Tree View Mode: multiple objects selected:</b>

              \param object Null (0).
              \param parent_observer The parent context of the selected object. If it does not have a parent, 0.

              \note In Tree View Mode, categories are handled as normal QObjects with the category name accessible through the objectName() function.
              */
            void addActionNewItem_triggered(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when the user double clicks on an item in the observer widget.
            /*!
              The parameters used during this signal emission is defferent depending on the display mode and the
              selected objects. The following are the possible scenarios:

              <b>1) Table View Mode: 1 object selected:</b>

              If an observer is double clicked with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is double clicked with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The context currently displayed in the table view.

              <b>2) Tree View Mode: 1 object selected:</b>

              If an observer is double clicked with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is double clicked with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The parent context of the selected object. If it does not have a parent, 0.

              \note In Tree View Mode, categories are handled as normal QObjects with the category name accessible through the objectName() function.
              */
            void doubleClickRequest(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when the user pushes up/down in a new observer widget. The new widget is passed as a paramater.
            void newObserverWidgetCreated(ObserverWidget* new_widget);

        private:
            //! Refreshes the visible columns.
            void refreshColumnVisibility();
            //! Disconnects the clipboard's copy and cut actions from this widget.
            void disconnectClipboard();
            void changeEvent(QEvent *e);

            Ui::ObserverWidget *ui;
            ObserverWidgetData* d;
        };

        //! Makes ObserverWidget available under the name TreeWidget.
        typedef ObserverWidget TreeWidget;
    }
}

#endif // OBSERVERWIDGET_H
