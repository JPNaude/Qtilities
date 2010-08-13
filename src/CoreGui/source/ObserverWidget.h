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

#ifndef OBSERVERWIDGET_H
#define OBSERVERWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IActionProvider.h"
#include "ObjectPropertyBrowser.h"

#include <Observer.h>
#include <ObserverHints.h>
#include <IContext.h>
#include <Observer.h>

#include <QMainWindow>
#include <QStack>
#include <QAbstractItemModel>
#include <QModelIndexList>

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

          \image html observer_widget_tree.jpg "Observer Widget (Tree View Mode)"
          \image latex observer_widget_tree.eps "Observer Widget (Tree View Mode)" width=3in

          The \ref page_observer_widgets provides a comprehensive overview of the different ways that the observer widget can be used.

          \todo
          - Observer widget does not show activity in tree mode when only one of the lower level observers has an activity policy filter.
          - Document drag-drop, cut-paste
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverWidget : public QMainWindow, public ObserverAwareBase, public IContext {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)
            Q_ENUMS(DisplayMode)

        public:
            //! The possible display modes of an ObserverWidget
            /*!
              \sa ObserverWidget()
              */
            enum DisplayMode {
                TableView,      /*!< Table view mode. */
                TreeView        /*!< Tree view mode. */
            };

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
            //! Default destructor.
            virtual ~ObserverWidget();
            //! Implementation of virtual function ObserverAwareBase::setObserverContext().
            void setObserverContext(Observer* observer);
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
        public slots:
            void contextDeleted();
            //! The context detach handler check if any observer in the current context's parent hierarhcy is deleted. If so, contextDeleted() is called.
            void contextDetachHandler(Observer::SubjectChangeIndication indication, QList<QObject*> obj);
            //! Slot which will call the handleSearchStringChanged() slot with an empty QString as parameter.
            void resetProxyModel();
        signals:
            //! Signal which is emitted when the observer context of this widget changes.
            void observerContextChanged(Observer* new_context);

            // --------------------------------
            // IContext implementation
            // --------------------------------
        public:
            QString contextString() const { return globalMetaType(); }
            QString contextHelpId() const { return QString(); }

            // --------------------------------
            // Functions Related To Display Hints
            // --------------------------------
            //! Function to toggle the visibility of the grid in the item view. Call it after initialization.
            void toggleGrid(bool toggle);
            //! Function to toggle the visibility of the grid in the item view. Call it after initialization.
            void toggleAlternatingRowColors(bool toggle);
            //! Function to toggle usage of hints from the active parent observer. If not default hints will be used.
            void toggleUseObserverHints(bool toggle);
            //! This function will set the hints used for the current selection parent.
            /*!
              \sa toggleUseObserverHints()
              */
            void inheritObserverHints(ObserverHints* display_hints);
        private:
            //! This function will provide the hints which should be used by this widget at any time.
            /*!
              \sa toggleUseObserverHints()
              */
            ObserverHints* activeHints() const;
            ObserverHints* activeHints();

            // --------------------------------
            // Settings, Global Meta Type and Action Provider Functions
            // --------------------------------
        public slots:
            //! Monitors the settings update request signal on the QtilitiesCoreGui instance.
            void handleSettingsUpdateRequest(const QString& request_id);
            //! Slot connected to QCoreApplication::aboutToQuit() signal.
            void writeSettings();
        public:
            //! Saves the current state of the widget.
            void writeSettings(const QString& widget_string);
            //! Restores the widget to a previous state.
            void readSettings(const QString& widget_string);
            //! Sets the id of the IObjectManager setMetaTypeActiveObjects() call used when selectedObjects change in the observer. If QString() is used, the call is not made when the selectedObjects changes.
            void setGlobalMetaType(const QString& meta_type);
            //! Gets the global meta type.
            QString globalMetaType() const;
            //! Returns the action handler interface for this observer widget.
            IActionProvider* actionProvider();
            //! Sets the global object subject type used by this observer widget.
            /*!
              For more information see the \ref meta_type_object_management section of the \ref page_object_management article.
              */
            void setGlobalObjectSubjectType();

            // --------------------------------
            // Functions Related To Selected Objects & Refreshing Of The Views
            // --------------------------------
            //! Provides a list of QObject pointers to all the selected objects.
            QList<QObject*> selectedObjects() const;
            //! Provides a list of QModelIndexes which are currently selected. Use this call instead of the item model selection's selectedIndexes() call since this function will map the indexes from the proxy model's indexes to the real model's indexes.
            QModelIndexList selectedIndexes() const;
        private slots:
            //! Updates the current selection parent context.
            /*!
              This function is only used in TreeView mode.
              */
            void setTreeSelectionParent(Observer* observer);
        public slots:
            //! Selects the specified objects in a table view. If any object is invalid, nothing is selected.
            /*!
              This function only does something when in table viewing mode.
              */
            void selectSubjectsInTable(QList<QObject*> objects);
            //! Slot which handles subject count changes.
            void handleSubjectCountChanged();
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
            // Property Editor Related Functions
            // --------------------------------
        public:
            //! Returns the property editor used inside the observer widget. This can be 0 depending on the display flags used. Always call this function after initialize().
            ObjectPropertyBrowser* propertyBrowser();

        public slots:
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
        private:
            //! Constructs the property browser. If it already exists, this function does nothing.
            void constructPropertyBrowser();
        protected:
            //! Refreshes the property browser, thus hide or show it depending on the active display flags.
            void refreshPropertyBrowser();

            // --------------------------------
            // Action Handlers and Related Functions
            // --------------------------------
        public slots:
            //! Handle the remove item action trigger. Detaches the item from the current observer context.
            void handle_actionRemoveItem_triggered();
            //! Handle the remove all action trigger. Detaches all objects from the current observer context.
            void handle_actionRemoveAll_triggered();
            //! Handle the delete item action trigger.
            void handle_actionDeleteItem_triggered();
            //! Handle the delete all action trigger.
            void handle_actionDeleteAll_triggered();
            //! Handle the new item action trigger.
            void handle_actionNewItem_triggered();
            //! Handle the refresh view action trigger.
            void handle_actionRefreshView_triggered();
            //! Handle the push up (go to parent) action trigger.
            void handle_actionPushUp_triggered();
            //! Handle the push up (go to parent in a new window) action trigger.
            void handle_actionPushUpNew_triggered();
            //! Handle the push down action trigger.
            void handle_actionPushDown_triggered();
            //! Handle the push down in new window action trigger.
            void handle_actionPushDownNew_triggered();
            //! Handle the switch view action trigger.
            void handle_actionSwitchView_triggered();
            //! Handle the copy action.
            void handle_actionCopy_triggered();
            //! Handle the cut action.
            void handle_actionCut_triggered();
            //! Handle the paste action.
            void handle_actionPaste_triggered();
            //! Handle the find item action.
            void handle_actionFindItem_triggered();
            //! Handle the collapse tree view action. Only usefull in TreeView display mode.
            void handle_actionCollapseAll_triggered();
            //! Handle the expand tree view action. Only usefull in TreeView display mode.
            void handle_actionExpandAll_triggered();
            //! Handles search options changes in the SearchBoxWidget if present.
            void handleSearchOptionsChanged();
            //! Handles search string changes in the SearchBoxWidget if present.
            void handleSearchStringChanged(const QString& filter_string);
            //! Refreshes the state of all actions.
            void refreshActions();
        protected:
            //! Constructs actions inside the observer widget.
            void constructActions();

        signals:
            //! Signal which is emitted when the add new item action is triggered.
            void addNewItem_triggered(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when the user double clicks on an item in the observer widget.
            void doubleClickRequest(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when the user pushes up/down in a new observer widget. The new widget is passed as a paramater.
            void newObserverWidgetCreated(ObserverWidget* new_widget);

        private:
            //! Disconnects the clipboard's copy and cut actions from this widget.
            void disconnectClipboard();

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::ObserverWidget *ui;
            ObserverWidgetData* d;
        };
    }
}

#endif // OBSERVERWIDGET_H
