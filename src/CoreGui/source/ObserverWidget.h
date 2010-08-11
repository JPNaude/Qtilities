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
            enum DisplayMode { TableView, TreeView };

            ObserverWidget(DisplayMode display_mode = TreeView, QWidget * parent = 0, Qt::WindowFlags f = 0);
            virtual ~ObserverWidget();
            bool eventFilter(QObject *object, QEvent *event);

            // --------------------------------
            // IContext implementation
            // --------------------------------
            QString contextString() const { return globalMetaType(); }
            QString contextHelpId() const { return QString(); }

            //! Implementation of virtual function ObserverAwareBase::setObserverContext().
            void setObserverContext(Observer* observer);

            //! Returns the observer ID of the top level observer in the widget
            int topLevelObserverID();

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

            //! Initializes the observer widget. Make sure to set the item model as well as the flags you would like to use before calling initialize.
            virtual void initialize(bool hints_only = false);

            //! Function to set display flags.
            void setDisplayFlags(Observer::DisplayFlags display_flags);
            //! Function to get current display flags.
            Observer::DisplayFlags displayFlags() const;
            //! Function to get current display mode.
            ObserverWidget::DisplayMode displayMode() const;
            //! Function to set the visible actions items.
            void setActionHints(Observer::ActionHints popup_menu_items);
            //! Function to get the visible actions items.
            Observer::ActionHints actionHints() const;
            //! Function to toggle the visibility of the grid in the item view. Call it after initialization.
            void toggleGrid(bool toggle);
            //! Function to toggle the visibility of the grid in the item view. Call it after initialization.
            void toggleAlternatingRowColors(bool toggle);
            //! Function to toggle usage of hints from the active parent observer. If not default hints will be used.
            void toggleUseObserverHints(bool toggle);

            //! Returns the property editor used inside the observer widget. This can be 0 depending on the display flags used. Always call this function after initialize().
            ObjectPropertyBrowser* propertyBrowser();
            //! Provides a list of QObject pointers to all the selected objects.
            QList<QObject*> selectedObjects() const;
            //! Provides a list of QModelIndexes which are currently selected. Use this call instead of the item model selection's selectedIndexes() call since this function will map the indexes from the proxy model's indexes to the real model's indexes.
            QModelIndexList selectedIndexes() const;
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

        public slots:
            //! Monitors the settings update request signal on the QtilitiesCoreGui instance.
            void handleSettingsUpdateRequest(const QString& request_id);
            //! Slot which handles subject count changes.
            void handleSubjectCountChanged();

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

            // Here we add slots for all the possible standard popup menu items and toolbar items
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

            //! Handles the selection model change. This basically enables/disables actions in the table view.
            void handleSelectionModelChange();
            //! Refreshes the state of all actions.
            void refreshActions();
            //! Updates the current selection parent context
            void setSelectionParent(Observer* observer);
            //! Slot connected to QCoreApplication::aboutToQuit() signal.
            void writeSettings();

        signals:
            //! Signal which is emitted when the add new item action is triggered.
            void addNewItem_triggered(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when the user double clicks on an item in the observer widget.
            void doubleClickRequest(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when object selection changes.
            void selectedObjectsChanged(QList<QObject*> selected_objects, Observer* selection_parent = 0);
            //! Signal which is emitted when the user pushes up/down in a new observer widget. The new widget is passed as a paramater.
            void newObserverWidgetCreated(ObserverWidget* new_widget);
            //! Signal which is emitted when the observer context of this widget changes.
            void observerContextChanged(Observer* new_context);

        public slots:
            void contextDeleted();
            //! The context detach handler check if any observer in the current context's parent hierarhcy is deleted. If so, contextDeleted() is called.
            void contextDetachHandler(Observer::SubjectChangeIndication indication, QList<QObject*> obj);
            //! Selects the specified objects in the item view. If any object is invalid, nothing is selected.
            /*!
              This function only does something when in table viewing mode.
              */
            void selectSubjectsByRef(QList<QObject*> objects);
            void handleSearchOptionsChanged();
            void handleSearchStringChanged(const QString& filter_string);
            void resetProxyModel();

        private:
            void constructObjectManagementToolBar();
            void constructPropertyBrowser();
            void disconnectClipboard();
            void setGlobalObjectSubjectType();

        protected:
            void refreshPropertyBrowser();           
            void constructActions();
            void changeEvent(QEvent *e);

        private:
            Ui::ObserverWidget *ui;
            ObserverWidgetData* d;
        };
    }
}

#endif // OBSERVERWIDGET_H
