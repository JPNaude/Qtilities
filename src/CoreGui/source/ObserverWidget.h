/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#ifndef OBSERVERWIDGET_H
#define OBSERVERWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IActionProvider.h"
#include "ObjectPropertyBrowser.h"
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
        class ObserverWidgetData;

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
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverWidget : public QMainWindow, public ObserverAwareBase, public IContext {
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
            //! Implementation of virtual function ObserverAwareBase::setObserverContext().
            bool setObserverContext(Observer* observer);
            //! Initializes the observer widget. Make sure to set the item model as well as the flags you would like to use before calling initialize.
            virtual void initialize(bool hints_only = false);
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
            //! Function which sets a custom table proxy model to be used in this widget when its in TableView mode.
            /*!
              By default the observer widget uses the Qtilities::CoreGui::ObserverTableModelCategoryFilter as the proxy model for the table view.
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
        public slots:
            void contextDeleted();
            //! The context detach handler check if any observer in the current context's parent hierarchy is deleted. If so, contextDeleted() is called.
            void contextDetachHandler(Observer::SubjectChangeIndication indication, QList<QPointer<QObject> > obj);
            //! Slot which will call the handleSearchStringChanged() slot with an empty QString as parameter.
            void resetProxyModel();
        signals:
            //! Signal which is emitted when the observer context of this widget changes.
            void observerContextChanged(Observer* new_context);

            // --------------------------------
            // IContext implementation
            // --------------------------------
        public:
            //! In the case of an ObserverWidget, the contextString() is the same as the globalMetaType().
            QString contextString() const { return globalMetaType(); }
            QString contextHelpId() const { return QString(); }

            //! Indicates if this observer widget appends contexts of selected objects implementing IContext.
            /*!
              False by default.

              \sa setAppendSelectedContexts()
              */
            bool appendSelectedContexts() const;
            //! Enables/disables appending of selected contexts when selected objects change.
            void setAppendSelectedContexts(bool enable);

            // --------------------------------
            // IObjectBase Implemenation
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

              \note These hints are only used when usesObserverHints() is false.

              \return True if successfull, false otherwise.

              \sa toggleUseObserverHints(), activeHints()
              */
            bool setCustomHints(ObserverHints* custom_hints);
            //! This function will provide the hints which should be used by this widget at any time.
            /*!
              \sa toggleUseObserverHints(), setCustomHints()
              */
            ObserverHints* activeHints() const;

            // --------------------------------
            // Settings, Global Meta Type and Action Provider Functions
            // --------------------------------
        public slots:
            //! Monitors the settings update request signal on the QtilitiesCoreGui instance.
            void handleSettingsUpdateRequest(const QString& request_id);
            //! Slot connected to QCoreApplication::aboutToQuit() signal.
            /*!
              Saves settings about this ObserverWidget instance using QSettings. The following parameters are saved:
              - The main window state of the observer widget (ObserverWidget inherits QMainWindow).
              - The grid style.
              - The default table view row size (Only in TableView mode). \sa defaultRowHeight()
              - The display mode. \sa DisplayMode
              - If delete operations must be confirmed. \sa confirmDeletes()
              - If automatic column resizing is enabled. \sa enableAutoColumnResizing()

              \note This connection is made in the readSettings() functions. Thus if you don't want to store settings
              for an ObserverWidget, don't read it when the widget is created.

              For more information see \ref configuration_widget_storage_layout.

              \sa readSettings(), globalMetaType()
              */
            virtual void writeSettings();
        public:
            //! Restores the widget to a previous state.
            /*!
              \note This function must be called only after initialize() was called.

              For more information see \ref configuration_widget_storage_layout.

              \sa writeSettings(), globalMetaType()
              */
            virtual void readSettings();
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
            //! Sets the global object subject type used by this observer widget.
            /*!
              If objects are selected, they are set as the active objects. If no objects are selected, the observer context
              is set as the active object.

              For more information see the \ref meta_type_object_management section of the \ref page_object_management article.
              */
            void updateGlobalActiveSubjects();
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
            //! Provides a pointer to the current selection's parent. If no objects are selected, 0 is returned.
            Observer* selectionParent() const;
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
              */
            QTableView* tableView();
            //! Function providing access to the contained tree view.
            /*!
              \note This reference is only valid in TreeView mode.
              */
            QTreeView* treeView();
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
        private slots:
            //! Updates the current selection parent context.
            /*!
              This function is only used in TreeView mode.
              */
            void setTreeSelectionParent(Observer* observer);
        public slots:
            //! Selects the specified objects in the active item view.
            /*!
              \param objects The objects that must be selected. If any objects in the list are not present in the view, they will be ignored. If the list is empty, the current selection will be cleared.

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.

              \sa selectedObjectsChanged()
              */
            void selectObjects(QList<QObject*> objects);
            //! Selects the specified object in the active item view.
            /*!
              \param object The object that must be selected. If the objects is not present in the view, it will be ignored.

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.

              \sa selectedObjectsChanged(), clearSelection(), selectObjects()
              */
            void selectObject(QObject* object);
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
            //! Signal which is emitted when object selection changes.
            void selectedObjectsChanged(QList<QObject*> selected_objects, Observer* selection_parent = 0);

            // --------------------------------
            // Item Widgets Related Functions
            // --------------------------------
        public:
            #ifdef QTILITIES_PROPERTY_BROWSER
            //! Returns the property editor used inside the observer widget. This can be 0 depending on the display flags used. Always call this function after initialize().
            /*!
              \sa propertyBrowserDock()
              */
            ObjectPropertyBrowser* propertyBrowser();
            //! Returns the dock widget containing the property browser.
            /*!
              \sa propertyBrowser();
              */
            QDockWidget* propertyBrowserDock();
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
        protected:
            //! Constructs the property browser and show it. If it already exists, this function does nothing.
            void constructPropertyBrowser();
            //! Refreshes the property browser, thus hide or show it depending on the active display flags.
            void refreshPropertyBrowser();
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
            //! This function is triggered by the Qtilities::Core::ObserverHints::ActionNewItem action.
            virtual void handle_actionNewItem_triggered();
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
            void toggleSearchBox();
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
            #ifndef QT_NO_DEBUG
            void selectionDebug() const;
            #endif

        protected:
            //! Constructs actions inside the observer widget.
            void constructActions();

        signals:
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
            //! Disconnects the clipboard's copy and cut actions from this widget.
            void disconnectClipboard();

        protected:
            void changeEvent(QEvent *e);

            Ui::ObserverWidget *ui;
            ObserverWidgetData* d;
        };
    }
}

#endif // OBSERVERWIDGET_H

