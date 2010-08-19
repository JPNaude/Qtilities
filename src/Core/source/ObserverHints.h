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

#ifndef OBSERVERHINTS_H
#define OBSERVERHINTS_H

#include "QtilitiesCore_global.h"
#include "Observer.h"

#include <QObject>

namespace Qtilities {
    namespace Core {
        /*!
        \struct ObserverHintsData
        \brief Structure used by ObserverHints to store private data.
          */
        struct ObserverHintsData;

        /*!
          \class ObserverHints
          \brief The ObserverHints class contains display hints for an Observer class.

          The ObserverHints class contains display hints for an Observer class. When an application uses observer hints,
          all item views displaying observer contexts should respect the hints provided by the ObserverHints object
          associated with an Observer.

          When an ObserverHints object is constructed and it is given an observer context (see ObserverHints()), it will automatically
          send modification state changes to the observer when hints are changed. It is also possible to create an
          ObserverHints class without an observer context in which case no interaction will happen with the observer
          since it is not present.

          When using an observer in a GUI application, you can either set the ObserverHints which
          must be used by your observer using the Qtilities::Core::Observer::setDisplayHints() or you can
          construct a default ObserverHints object using the Qtilities::Core::Observer::useDisplayHints()
          function in which case the default values as shown in ObserverHints().

          \sa Observer, ObserverData
          */
        class QTILIITES_CORE_SHARED_EXPORT ObserverHints : public QObject, public ObserverAwareBase, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)
            Q_ENUMS(DisplayFlag)
            Q_ENUMS(NamingControl)
            Q_ENUMS(ActivityDisplay)
            Q_ENUMS(ActivityControl)
            Q_ENUMS(ItemSelectionControl)
            Q_ENUMS(HierarchicalDisplay)
            Q_ENUMS(ItemViewColumn)

        public:
            // --------------------------------
            // Enumerations
            // --------------------------------
            //! The observer selection context hint provides the ability to specify the context which must be used when selecting an observer in an observer tree hierarchy.
            /*!
              Without this hint, widgets which are viewing a context must make assumptions on what to do when an observer is selected
              in the view. For example, if you select an observer inside a context, should the ActionDeleteAll hint delete all objects
              under the selected observer or in the context where the observer is present (thus the observer's parent
              observer context).

              When using different selection contexts in the same tree you should take care not to create actions which
              are confusing to users, thus the usages of all items in the tree must make sense and be consistant in the
              usage scenario.

              This hint changes the behaviour of the following action handles in the Qtilities::CoreGui::ObserverWidget class:
              - ObserverHints::ActionSwitchView
              - ObserverHints::ActionRemoveAll
              - ObserverHints::ActionDeleteAll
              - ObserverHints::ActionNewItem
              - The double click action handler: Qtilities::CoreGui::ObserverWidget::doubleClickRequest()

              \note This hint must be set on the observer which is selected. Thus if you set it on an observer the hint
              will only be respected for that observer, not any observers underneath it.

              \sa setObserverSelectionContextHint(), observerSelectionContextHint()
              */
            enum ObserverSelectionContext {
                NoObserverSelectionContextHint = 0,     /*!< No observer selection context hint. Uses SelectionUseParentContext by default. */
                SelectionUseParentContext = 1,          /*!< Use the parent observer's context. */
                SelectionUseSelectedContext = 2         /*!< Use the selected observer's context. */
            };
            //! The possible naming control hints of the observer.
            /*!
              \sa setNamingControlHint(), namingControlHint()
              */
            enum NamingControl {
                NoNamingControlHint = 0,    /*!< No naming control hint. Uses ReadOnlyNames by default. */
                ReadOnlyNames = 1,          /*!< Names cannot be edited in item views viewing this observer. */
                EditableNames = 2           /*!< Names are editable in item views viewing this observer. */
            };
            //! The possible activity display hints.
            /*!
              \sa setActivityDisplayHint(), activityDisplayHint()
              */
            enum ActivityDisplay {
                NoActivityDisplayHint = 0,  /*!< No activity display hint. Uses NoActivityDisplay by default. */
                NoActivityDisplay = 1,      /*!< The activity of items are not displayed in item views viewing this observer. */
                CheckboxActivityDisplay = 2 /*!< If the observer has an ActivityPolicyFilter subject filter installed, a check box which shows the activity of subjects are shown in item views viewing this observer. */
            };
            //! The possible activity control hints.
            /*!
              \sa setActivityControlHint(), activityControlHint()
              */
            enum ActivityControl {
                NoActivityControlHint = 0,  /*!< No activity control hint. Uses NoActivityControl by default. */
                NoActivityControl = 1,      /*!< The activity of subjects cannot be changed by the user in item views viewing this observer. */
                FollowSelection = 2,        /*!< The activity of subjects follows the selection of the user in item views viewing this observer. To use this option, ItemSelectionControl must be set to SelectableItems. */
                CheckboxTriggered = 3       /*!< The activity of subjects can be changed by checking or unchecking the checkbox appearing next to subject in item views viewing this observer. To use this option, ActivityDisplay must be set to CheckboxActivityDisplay. */
            };
            //! The possible item selection control hints.
            /*!
              \sa setItemSelectionControlHint(), itemSelectionControlHint()
              */
            enum ItemSelectionControl {
                NoItemSelectionControlHint = 0, /*!< No item selection control hint. Uses NonSelectableItems by default. */
                SelectableItems = 1,            /*!< Items are selectable by the user in item views viewing this observer. */
                NonSelectableItems = 2          /*!< Items are not selectable by the user in item views viewing this observer. */
            };
            //! The possible hierarchical display hints.
            /*!
              \sa setHierarchicalDisplayHint(), hierarchicalDisplayHint()
              */
            enum HierarchicalDisplay {
                NoHierarchicalDisplayHint = 0,   /*!< No hierachical display hint. Uses FlatHierarchy by default. */
                FlatHierarchy = 1,              /*!< The hierarchy of items under an observer is flat. Thus categories are not displayed. */
                CategorizedHierarchy = 2        /*!< Item are grouped by their category. Items which do not have a category associated with them are grouped under an category called QString(OBSERVER_UNCATEGORIZED_CATEGORY). */
            };
            //! The possible item view column hints.
            /*!
              \sa setItemViewColumnHint(), itemViewColumnHint()
              */
            enum ItemViewColumn {
                ColumnNoHints = 1,              /*!< No item view column hint. Only the name column is shown in item views viewing this observer. */
                ColumnNameHint = 1,             /*!< Shows a column with the name of the subject. */
                ColumnIDHint = 2,               /*!< Shows a column with the subject IDs of subjects in item views viewing this observer. */
                ColumnChildCountHint = 4,       /*!< Shows a column with the cumulative count (counts items under each subject recusively) of subjects under each subject in item views viewing this observer. */
                ColumnTypeInfoHint = 8,         /*!< Shows a column with information about the type of subject in item views viewing this observer. */
                ColumnAccessHint = 16,           /*!< Shows a column with information about the access type of the subject in item views viewing this observer. */
                ColumnCategoryHint = 32,        /*!< Shows a column with information about the category of the subject in item views viewing this observer. Only used when CategorizedHierarchy hierachical display hint is used and only affects table models for the observer. */
                ColumnAllHints = ColumnNameHint | ColumnChildCountHint | ColumnTypeInfoHint | ColumnAccessHint | ColumnCategoryHint /*!< All columns, except ColumnIDHint. */
            };
            Q_DECLARE_FLAGS(ItemViewColumnFlags, ItemViewColumn);
            Q_FLAGS(ItemViewColumnFlags);
            //! The possible display flags of the observer.
            /*!
              \sa setDisplayFlagsHint(), displayFlagsHint()
              */
            enum DisplayFlag {
                NoDisplayFlagsHint = 0,     /*!< No display flags hint. Uses ItemView by default. */
                ItemView = 1,               /*!< Display the item view (TreeView, TableView etc.). The item view is always displayed when using the Qtilities::CoreGui::ObserverWidget widget.*/
                NavigationBar = 2,          /*!< Display the navigation bar in TableViews. */
                PropertyBrowser = 4,        /*!< Display the property browser. */
                ActionToolBar = 8,          /*!< Display an action toolbar in the observer widget with all the actions provided through Qtilities::CoreGui::ObserverWidget::actionProvider(). */
                AllDisplayFlagHint = ItemView | NavigationBar | PropertyBrowser | ActionToolBar
            };
            Q_DECLARE_FLAGS(DisplayFlags, DisplayFlag);
            Q_FLAGS(DisplayFlags);
            //! The possible actions which views can perform on an observer. The ObserverWidget class provides the applicable actions through an IActionProvider interface and handles all these actions already.
            /*!
              \sa setActionHints(), actionHints()
              */
            enum ActionItem {
                ActionNoHints = 0,                /*!< No actions are allowed. */
                ActionRemoveItem = 1,             /*!< Allow detachment of subjects from the observer context presented to the user. */
                ActionRemoveAll = 2,              /*!< Allow detachment of all subjects from the observer context presented to the user. \note ObserverWidget only handles this action if a single object is selected. */
                ActionDeleteItem = 4,             /*!< Allow deleting subjects from the observer context presented to the user. */
                ActionDeleteAll = 8,              /*!< Allow deleting of all subjects from the observer context presented to the user. \note ObserverWidget only handles this action if a single object is selected. */
                ActionNewItem = 32,               /*!< Allow new items to be added to the observer context presented to the user. */
                ActionRefreshView = 64,           /*!< Allow refreshing of views. */
                ActionPushUp = 128,               /*!< Allow pushing up in the hierarchy of the displayed observer context in TableViews. */
                ActionPushUpNew = 256,            /*!< Allow pushing up into a new window in the hierarchy of the displayed observer context in TableViews. */
                ActionPushDown = 512,             /*!< Allow pushing down in the hierarchy of the displayed observer context in TableViews. */
                ActionPushDownNew = 1024,         /*!< Allow pushing down into a new window in the hierarchy of the displayed observer context in TableViews. */
                ActionSwitchView = 2048,          /*!< Allow switching between different view modes (TableView and TreeView) for example. When switching from tree to table view mode, the current active context is used for the table view, unless an observer with the ObserverHints::SelectionUseSelectedContext is selected. */
                ActionCopyItem = 4096,            /*!< Allow copy operations which will add details about the selected items in the view to the clipboard using the ObserverMimeData class. */
                ActionCutItem = 8192,             /*!< Allow cut operations similar to the copy operation, the items are just detached from the current context when added to a new context. */
                ActionPasteItem = 16384,          /*!< Allow pasting of ObserverMimeData into the observer context presented to the user. */
                ActionFindItem = 131072,          /*!< Allow finding/searching in the observer context presented to the user. */
                ActionScopeDuplicate = 262144,    /*!< Allow duplication of selected object in the observer context presented to the user. */
                ActionAllHints = ActionRemoveItem | ActionRemoveAll | ActionDeleteItem | ActionDeleteAll | ActionNewItem | ActionRefreshView | ActionPushUp | ActionPushUpNew | ActionPushDown | ActionPushDownNew | ActionSwitchView | ActionCopyItem | ActionCutItem | ActionPasteItem | ActionFindItem | ActionScopeDuplicate /*!< All actions. */
            };
            Q_DECLARE_FLAGS(ActionHints, ActionItem);
            Q_FLAGS(ActionHints);

            // --------------------------------
            // Implementation
            // --------------------------------
            //! Default constructor
            /*!
               \param parent The parent of the observer hint object. If the parent is an observer, setObserverContext() will automatically
                be called using the parent.
              The defaults for all hints are initialized as shown below:
\code
observer_selection_context(ObserverHints::SelectionUseParentContext),
naming_control(ObserverHints::NoNamingControlHint),
activity_display(ObserverHints::NoActivityDisplayHint),
activity_control(ObserverHints::NoActivityControlHint),
item_selection_control(ObserverHints::SelectableItems),
hierarhical_display(ObserverHints::NoHierarchicalDisplayHint),
display_flags(ObserverHints::ItemView | ObserverHints::NavigationBar),
item_view_column_hint(ObserverHints::NoItemViewColumnHint),
action_hints(ObserverHints::None),
category_list(QStringList()),
inverse_categories(true),
category_filter_enabled(false)
\endcode
              */
            ObserverHints(QObject* parent = 0);
            //! Copy constructor.
            ObserverHints(const ObserverHints& other);
            //! Overloaded = operator.
            void operator=(const ObserverHints& other);
            //! Destructor.
            virtual ~ObserverHints();
            //! Exports hints to a QDataStream.
            /*!
              \returns True if succesfull, false otherwise.
              */
            virtual bool exportBinary(QDataStream& stream) const;
            //! Imports hints from a QDataStream.
            /*!
              \returns True if succesfull, false otherwise.
              */
            virtual bool importBinary(QDataStream& stream);

            // --------------------------------
            // Hints Getter and Setter Functions
            // --------------------------------
            //! Sets the observer selection context hint.
            void setObserverSelectionContextHint(ObserverHints::ObserverSelectionContext observer_selection_context);
            //! Gets the observer selection context hint.
            ObserverHints::ObserverSelectionContext observerSelectionContextHint() const;
            //! Sets the naming control hint.
            void setNamingControlHint(ObserverHints::NamingControl naming_control);
            //! Gets the naming control hint.
            ObserverHints::NamingControl namingControlHint() const;
            //! Sets the activity display hint.
            void setActivityDisplayHint(ObserverHints::ActivityDisplay activity_display);
            //! Gets the activity display hint.
            ObserverHints::ActivityDisplay activityDisplayHint() const;
            //! Sets the activity control hint.
            void setActivityControlHint(ObserverHints::ActivityControl activity_control);
            //! Gets the activity control hint.
            ObserverHints::ActivityControl activityControlHint() const;
            //! Sets the selection control hint.
            void setItemSelectionControlHint(ObserverHints::ItemSelectionControl item_selection_control);
            //! Gets the selection control hint.
            ObserverHints::ItemSelectionControl itemSelectionControlHint() const;
            //! Sets the hierarchical display hint for this model.
            void setHierarchicalDisplayHint(ObserverHints::HierarchicalDisplay hierarhical_display);
            //! Gets the hierarchical display hint for this model.
            ObserverHints::HierarchicalDisplay hierarchicalDisplayHint() const;
            //! Function to set display flags hint.
            void setDisplayFlagsHint(ObserverHints::DisplayFlags display_flags);
            //! Function to get current display flags hint.
            ObserverHints::DisplayFlags displayFlagsHint() const;
            //! Function to set item view column display hints.
            void setItemViewColumnHint(ObserverHints::ItemViewColumnFlags item_view_column_hint);
            //! Function to get item view column display hints.
            ObserverHints::ItemViewColumnFlags itemViewColumnHint() const;
            //! Function to set the action hint for this observer's context.
            void setActionHints(ObserverHints::ActionHints popup_menu_items);
            //! Function to get the action hint for this observer's context.
            ObserverHints::ActionHints actionHints() const;

            // --------------------------------
            // Category Display Functionality
            // --------------------------------
            //! Function to set the categories you want to be displayed in item views viewing this observer.
            /*!
              Only applicable when an observer provides a CategorizedHierarchy hierachical display hint.
              Only categories which appears in the category list will be displayed, or passing true as the
              inversed paramater will inverse the list. Thus, all categories will be displayed except the categories
              in the category_list paramater.

              Calling this function will automatically update all views viewing the observer.

              \sa setCategoryFilterEnabled()
              */
            void setDisplayedCategories(const QStringList& displayed_categories, bool inversed = false);
            //! Returns the displayed categories list. Note that this function does not take inversed filtering, or if filtering is enabled into account. The default is QStringList().
            QStringList displayedCategories() const;
            //! Returns true if the displayed categories list is inversed. The default is true.
            void setCategoryFilterEnabled(bool enabled);
            //! Returns true if the category filter is enabled, false otherwise. The default is false.
            bool categoryFilterEnabled() const;
            //! Returns true if the displayed categories list is inversed. The default is true.
            bool hasInversedCategoryDisplay() const;

            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }

            // --------------------------------
            // IModificationNotifier Implemenation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners);
        signals:
            void modificationStateChanged(bool is_modified) const;

        private:
            ObserverHintsData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::ItemViewColumnFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::DisplayFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::ActionHints)
    }
}

#endif // OBSERVERHINTS_H
