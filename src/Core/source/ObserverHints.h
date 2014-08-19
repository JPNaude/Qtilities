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

#ifndef OBSERVERHINTS_H
#define OBSERVERHINTS_H

#include "QtilitiesCore_global.h"
#include "Observer.h"

#include <QObject>

namespace Qtilities {
    namespace Core {
        /*!
        \struct ObserverHintsPrivateData
        \brief Structure used by ObserverHints to store private data.
          */
        struct ObserverHintsPrivateData;

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
        class QTILIITES_CORE_SHARED_EXPORT ObserverHints : public QObject, public ObserverAwareBase,
                                                           public IModificationNotifier, public IExportable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
            Q_ENUMS(DisplayFlag)
            Q_ENUMS(NamingControl)
            Q_ENUMS(ActivityDisplay)
            Q_ENUMS(ActivityControl)
            Q_ENUMS(ItemSelectionControl)
            Q_ENUMS(HierarchicalDisplay)
            Q_ENUMS(ItemViewColumn)
            Q_ENUMS(DragDropHint)
            Q_ENUMS(ModificationStateDisplayHint)
            Q_ENUMS(CategoryEditingHint)
            Q_ENUMS(RootIndexDisplayHint)

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
                SelectionUseSelectedContext = 2         /*!< Use the selected observer's context. Only enforced when a single selection is present in an ObserverWidget tree. When more items are selected
                                                             SelectionUseParentContext will be used */
            };
            //! Function which returns a string associated with a specific ObserverSelectionContext.
            static QString observerSelectionContextToString(ObserverSelectionContext observer_selection_context);
            //! Function which returns the ObserverSelectionContext associated with a string.
            static ObserverSelectionContext stringToObserverSelectionContext(const QString& observer_selection_context_string);
            //! The possible naming control hints of the observer.
            /*!
              \sa setNamingControlHint(), namingControlHint()
              */
            enum NamingControl {
                NoNamingControlHint = 0,    /*!< No naming control hint. Uses ReadOnlyNames by default. */
                ReadOnlyNames = 1,          /*!< Names cannot be edited in item views viewing this observer. */
                EditableNames = 2           /*!< Names are editable in item views viewing this observer. */
            };
            //! Function which returns a string associated with a specific NamingControl.
            static QString namingControlToString(NamingControl naming_control);
            //! Function which returns the NamingControl associated with a string.
            static NamingControl stringToNamingControl(const QString& naming_control_string);
            //! The possible activity display hints.
            /*!
              \sa setActivityDisplayHint(), activityDisplayHint()
              */
            enum ActivityDisplay {
                NoActivityDisplayHint = 0,  /*!< No activity display hint. Uses NoActivityDisplay by default. */
                NoActivityDisplay = 1,      /*!< The activity of items are not displayed in item views viewing this observer. */
                CheckboxActivityDisplay = 2 /*!< If the observer has an ActivityPolicyFilter subject filter installed, a check box which shows the activity of subjects are shown in item views viewing this observer. */
            };
            //! Function which returns a string associated with a specific ActivityDisplay.
            static QString activityDisplayToString(ActivityDisplay activity_display);
            //! Function which returns the ActivityDisplay associated with a string.
            static ActivityDisplay stringToActivityDisplay(const QString& activity_display_string);
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
            //! Function which returns a string associated with a specific ActivityControl.
            static QString activityControlToString(ActivityControl activity_control);
            //! Function which returns the ActivityControl associated with a string.
            static ActivityControl stringToActivityControl(const QString& activity_control_string);
            //! The possible item selection control hints.
            /*!
              \sa setItemSelectionControlHint(), itemSelectionControlHint()
              */
            enum ItemSelectionControl {
                NoItemSelectionControlHint = 0, /*!< No item selection control hint. Uses NonSelectableItems by default. */
                SelectableItems = 1,            /*!< Items are selectable by the user in item views viewing this observer. */
                NonSelectableItems = 2          /*!< Items are not selectable by the user in item views viewing this observer. */
            };
            //! Function which returns a string associated with a specific ItemSelectionControl.
            static QString itemSelectionControlToString(ItemSelectionControl item_selection_control);
            //! Function which returns the ItemSelectionControl associated with a string.
            static ItemSelectionControl stringToItemSelectionControl(const QString& item_selection_control_string);
            //! The possible hierarchical display hints.
            /*!
              \sa setHierarchicalDisplayHint(), hierarchicalDisplayHint()
              */
            enum HierarchicalDisplay {
                NoHierarchicalDisplayHint = 0,  /*!< No hierachical display hint. Uses FlatHierarchy by default. */
                FlatHierarchy = 1,              /*!< The hierarchy of items under an observer is flat. Thus categories are not displayed. */
                CategorizedHierarchy = 2        /*!< Item are grouped by their category */
            };
            //! Function which returns a string associated with a specific HierarchicalDisplay.
            static QString hierarchicalDisplayToString(HierarchicalDisplay hierarchical_display);
            //! Function which returns the HierarchicalDisplay associated with a string.
            static HierarchicalDisplay stringToHierarchicalDisplay(const QString& hierarchical_display_string);
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
                ColumnAccessHint = 16,          /*!< Shows a column with information about the access type of the subject in item views viewing this observer. */
                ColumnCategoryHint = 32,        /*!< Shows a column with information about the category of the subject in item views viewing this observer. Only used when CategorizedHierarchy hierachical display hint is used and only affects table models for the observer. */
                ColumnAllHints = ColumnNameHint | ColumnChildCountHint | ColumnTypeInfoHint | ColumnAccessHint | ColumnCategoryHint /*!< All columns, except ColumnIDHint. */
            };
            Q_DECLARE_FLAGS(ItemViewColumnFlags, ItemViewColumn)
            Q_FLAGS(ItemViewColumnFlags)
            //! Function which returns a string associated with a specific ItemViewColumnFlags.
            static QString itemViewColumnFlagsToString(ItemViewColumnFlags item_view_column_flags);
            //! Function which returns the ItemViewColumnFlags associated with a string.
            static ItemViewColumnFlags stringToItemViewColumnFlags(const QString& item_view_column_flags_string);
            //! The possible display flags of the observer.
            /*!
              \sa setDisplayFlagsHint(), displayFlagsHint()
              */
            enum DisplayFlag {
                NoDisplayFlagsHint = 3,         /*!< No display flags hint. Uses ItemView | NavigationBar by default. */
                ItemView = 1,                   /*!< Display the item view (TreeView, TableView etc.). The item view is always displayed when using the Qtilities::CoreGui::ObserverWidget widget.*/
                NavigationBar = 2,              /*!< Display the navigation bar in TableViews. */
                PropertyBrowser = 4,            /*!< Display the property browser using Qtilities::CoreGui::ObjectPropertyBrowser. When %Qtilities is build without the QTILITIES_PROPERTY_BROWSER variable defined, this value is meaningless.*/
                ActionToolBar = 8,              /*!< Display an action toolbar in the observer widget with all the actions provided through Qtilities::CoreGui::ObserverWidget::actionProvider(). */
                DynamicPropertyBrowser = 16,    /*!< Display the property browser using Qtilities::CoreGui::ObjectDynamicPropertyBrowser. When %Qtilities is build without the QTILITIES_PROPERTY_BROWSER variable defined, this value is meaningless.*/
                AllDisplayFlagHint = ItemView | NavigationBar | PropertyBrowser | ActionToolBar | DynamicPropertyBrowser
            };
            Q_DECLARE_FLAGS(DisplayFlags, DisplayFlag)
            Q_FLAGS(DisplayFlags)
            //! Function which returns a string associated with a specific DisplayFlags.
            static QString displayFlagsToString(DisplayFlags display_flags);
            //! Function which returns the DisplayFlags associated with a string.
            static DisplayFlags stringToDisplayFlags(const QString& display_flags_string);
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
                ActionFindItem = 32768,           /*!< Allow finding/searching in the observer context presented to the user. */
                ActionAllHints = ActionRemoveItem | ActionRemoveAll | ActionDeleteItem | ActionDeleteAll | ActionNewItem | ActionRefreshView | ActionPushUp | ActionPushUpNew | ActionPushDown | ActionPushDownNew | ActionSwitchView | ActionCopyItem | ActionCutItem | ActionPasteItem | ActionFindItem /*!< All actions. */
            };
            Q_DECLARE_FLAGS(ActionHints, ActionItem)
            Q_FLAGS(ActionHints)
            //! Function which returns a string associated with a specific ActionHints.
            static QString actionHintsToString(ActionHints actions_hints);
            //! Function which returns the ActionHints associated with a string.
            static ActionHints stringToActionHints(const QString& actions_hints_string);
            //! The possible drag drop hints for an observer context.
            /*!
              This hint indicates if an observer context supports drag or/and drop operations of subjects.

              \note Category drops works independent of dragDropHint(). See categoryEditingFlags() for more information about category drops. Note that dragDropHint() must contain AllowDrags in order for category drag/drop operations to work.

              \sa setDragDropHint(), dragDropHint()
              */
            enum DragDropHint {
                NoDragDrop  =    0, /*!< No drag or drops allowed. */
                AcceptDrops =    1, /*!< This context accepts drops.*/
                AllowDrags  =    2, /*!< This context allows drags. */
                AllDragDrop = AcceptDrops | AllowDrags
            };
            Q_DECLARE_FLAGS(DragDropFlags, DragDropHint)
            Q_FLAGS(DragDropFlags)
            //! Function which returns a string associated with a specific DragDropFlags.
            static QString dragDropFlagsToString(DragDropFlags drag_drop_flags);
            //! Function which returns the DragDropFlags associated with a string.
            static DragDropFlags stringToDragDropFlags(const QString& drag_drop_flags_string);
            //! The possible modification state display hints.
            /*!
              \sa setModificationStateDisplayHint(), modificationStateDisplayHint()
              */
            enum ModificationStateDisplayHint {
                NoModificationStateDisplayHint = 0,         /*!< No modification state display hint. The modification state is not displayed in any way. */
                CharacterModificationStateDisplay = 1       /*!< The modification state of items is displayed by appending a specific character "*" to names of modified items. */
            };
            //! Function which returns a string associated with a specific ModificationStateDisplayHint.
            static QString modificationStateDisplayToString(ModificationStateDisplayHint modification_display);
            //! Function which returns the ModificationStateDisplayHint associated with a string.
            static ModificationStateDisplayHint stringToModificationStateDisplay(const QString& modification_display_string);
            //! The possible category editing hints.
            /*!
              \note Only applicable when hierarchicalDisplayToString() is CategorizedHierarchy.
              \note Category drops are enabled through CategoriesAcceptSubjectDrops which works independent of dragDropHint(). See categoryEditingFlags() for more information about category drops. Note that dragDropHint() must contain AllowDrags in order for category drag/drop operations to work.

              Thus, to enable dragging items between categories, the following is needed:

\code
node->displayHints()->setCategoryEditingFlags(ObserverHints::CategoriesEditableAllLevels |
                                            ObserverHints::CategoriesAcceptSubjectDrops);
node->displayHints()->setDragDropHint(ObserverHints::AllowDrags);
\endcode

              \sa setCategoryEditingFlags(), categoryEditingFlags()
              */
            enum CategoryEditingHint {
                CategoriesNoEditingFlags = 0,       /*!< Categories are read only, double clicking on them expands/collapse them. */
                CategoriesReadOnly = 1,             /*!< Categories are read only, double clicking on them expands/collapse them. */
                CategoriesEditableTopLevel = 2,     /*!< Categories are editable, double clicking on them shows the top level category name to the user to edit. */
                CategoriesEditableAllLevels = 4,    /*!< Categories are editable, double clicking on them shows all the category levels to the user to edit. */
                CategoriesAcceptSubjectDrops = 8    /*!< Categories accept subject(s) dropped onto them and assigns the dropped category to the subject(s). */
            };
            Q_DECLARE_FLAGS(CategoryEditingFlags, CategoryEditingHint)
            Q_FLAGS(CategoryEditingFlags)
            //! Function which returns a string associated with a specific CategoryEditingFlags.
            static QString categoryEditingFlagsToString(CategoryEditingFlags category_editing_flags);
            //! Function which returns the CategoryEditingFlags associated with a string.
            static CategoryEditingFlags stringToCategoryEditingFlags(const QString& category_editing_flags);
            //! The root index display hint of the observer.
            /*!
              This hint determines how the root node of the observer is shown in Qtilities::CoreGui::ObserverWidget when in Qtilities::TreeView mode.

              The default is RootIndexHide.

              \sa rootIndexDisplayHintToString(), stringToRootIndexDisplayHint(), setRootIndexDisplayHint(), rootIndexDisplayHint()

              <i>This hint was added in %Qtilities v1.2.</i>
              */
            enum RootIndexDisplayHint {
                RootIndexHide  = 0, /*!< Don't display the root index. An example tree displayed using this hint can be seen below: \image html observer_hints_root_index_hide.jpg "Tree Displayed Using RootIndexHide" */
                RootIndexDisplayDecorated = 1, /*!< Display the root index and decorate it. See QTreeView::rootIsDecorated() for more details on decoration. An example tree displayed using this hint can be seen below: \image html observer_hints_root_index_display_decorated.jpg "Tree Displayed Using RootIndexDisplayDecorated" */
                RootIndexDisplayUndecorated  = 2 /*!< Display the root index but don't decorate it. See QTreeView::rootIsDecorated() for more details on decoration. An example tree displayed using this hint can be seen below: \image html observer_hints_root_index_display_undecorated.jpg "Tree Displayed Using RootIndexDisplayUndecorated" */

            };
            //! Function which returns a string associated with a specific RootIndexDisplayHint.
            /*!
              *<i>This function was added in %Qtilities v1.2.</i>
              */
            static QString rootIndexDisplayHintToString(RootIndexDisplayHint root_index_display_hint);
            //! Function which returns the RootIndexDisplayHint associated with a string.
            /*!
              *<i>This function was added in %Qtilities v1.2.</i>
              */
            static RootIndexDisplayHint stringToRootIndexDisplayHint(const QString& root_index_display_hint_string);

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
display_flags(NoDisplayFlagsHint),
item_view_column_hint(ObserverHints::ColumnNoHints),
drag_drop_flags(ObserverHints::NoDragDrop);
action_hints(ObserverHints::ActionNoHints),
modification_state_display(ObserverHints::NoModificationStateDisplayHint),
category_editing_flags(ObserverHints::CategoriesReadOnly),
category_list(QStringList()),
inverse_categories(true),
category_filter_enabled(false),
\endcode
              */
            ObserverHints(QObject* parent = 0);
            //! Copy constructor.
            ObserverHints(const ObserverHints& other);
            //! Destructor.
            virtual ~ObserverHints();
            ObserverHints& operator=(const ObserverHints& other);
            bool operator==(const ObserverHints& other) const;
            bool operator!=(const ObserverHints& other) const;

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
            /*!
              When in tree mode, Qtilities::CoreGui::ObserverWidget uses the selection control hint of the top level observer (root item)
              and does not inherit the selection control hint of other observer contexts in the tree when they become active
              which will happen when the user selects objects in those contexts.
              */
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
            void setActionHints(ObserverHints::ActionHints action_hints);
            //! Function to get the action hint for this observer's context.
            ObserverHints::ActionHints actionHints() const;
            //! Function to set the drag drop hint for this observer's context.
            void setDragDropHint(ObserverHints::DragDropFlags drag_drop_flags);
            //! Function to get the drag drop hint for this observer's context.
            ObserverHints::DragDropFlags dragDropHint() const;
            //! Function to set the modification state display hint for this observer's context.
            void setModificationStateDisplayHint(ObserverHints::ModificationStateDisplayHint modification_state_display);
            //! Function to get the modification state display hint for this observer's context.
            ObserverHints::ModificationStateDisplayHint modificationStateDisplayHint() const;
            //! Function to set the category editing display hint for this observer's context.
            void setCategoryEditingFlags(ObserverHints::CategoryEditingFlags category_editing_flags);
            //! Function to get the category editing display hint for this observer's context.
            ObserverHints::CategoryEditingFlags categoryEditingFlags() const;
            //! Function to set the root index display hint for this observer's context.
            /*!
              *<i>This function was added in %Qtilities v1.2.</i>
              */
            void setRootIndexDisplayHint(ObserverHints::RootIndexDisplayHint root_index_display_hint);
            //! Function to get the category editing display hint for this observer's context.
            /*!
              *<i>This function was added in %Qtilities v1.2.</i>
              */
            ObserverHints::RootIndexDisplayHint rootIndexDisplayHint() const;

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
            void setDisplayedCategories(const QList<QtilitiesCategory>& displayed_categories, bool inversed = false);
            //! Returns the displayed categories list. Note that this function does not take inversed filtering, or if filtering is enabled into account.
            QList<QtilitiesCategory> displayedCategories() const;
            //! Adds a displayed category to the list of displayed categories.
            /*!
              \sa displayedCategories()
              */
            void addDisplayedCategory(const QtilitiesCategory& category);
            //! Removes a displayed category to the list of displayed categories.
            /*!
              \sa displayedCategories()
              */
            void removeDisplayedCategory(const QtilitiesCategory& category);
            //! Returns true if the displayed categories list is inversed. The default is true.
            void setCategoryFilterEnabled(bool enabled);
            //! Returns true if the category filter is enabled, false otherwise. The default is false.
            bool categoryFilterEnabled() const;
            //! Returns true if the displayed categories list is inversed. The default is true.
            bool hasInversedCategoryDisplay() const;

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            IExportable::ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        private:
            ObserverHintsPrivateData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::ItemViewColumnFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::DisplayFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::ActionHints)
        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::DragDropFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverHints::CategoryEditingFlags)
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::ObserverHints& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::ObserverHints& stream_obj);

#endif // OBSERVERHINTS_H
