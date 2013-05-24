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

#include "ObserverHints.h"

#include <QDomElement>

struct Qtilities::Core::ObserverHintsPrivateData {
    ObserverHintsPrivateData() : observer_selection_context(ObserverHints::SelectionUseParentContext),
        naming_control(ObserverHints::NoNamingControlHint),
        activity_display(ObserverHints::NoActivityDisplayHint),
        activity_control(ObserverHints::NoActivityControlHint),
        item_selection_control(ObserverHints::SelectableItems),
        hierarhical_display(ObserverHints::NoHierarchicalDisplayHint),
        display_flags(ObserverHints::NoDisplayFlagsHint),
        item_view_column_hint(ObserverHints::ColumnNoHints),
        action_hints(ObserverHints::ActionNoHints),
        drag_drop_flags(ObserverHints::NoDragDrop),
        modification_state_display(ObserverHints::NoModificationStateDisplayHint),
        category_editing_flags(ObserverHints::CategoriesReadOnly),
        root_index_display_hint(ObserverHints::RootIndexHide),
        //root_index_display_hint(ObserverHints::RootIndexDisplayDecorated),
        has_inversed_category_display(true),
        category_filter_enabled(false),
        is_modified(false) {}

    ObserverHints::ObserverSelectionContext     observer_selection_context;
    ObserverHints::NamingControl                naming_control;
    ObserverHints::ActivityDisplay              activity_display;
    ObserverHints::ActivityControl              activity_control;
    ObserverHints::ItemSelectionControl         item_selection_control;
    ObserverHints::HierarchicalDisplay          hierarhical_display;
    ObserverHints::DisplayFlags                 display_flags;
    ObserverHints::ItemViewColumnFlags          item_view_column_hint;
    ObserverHints::ActionHints                  action_hints;
    ObserverHints::DragDropFlags                drag_drop_flags;
    ObserverHints::ModificationStateDisplayHint modification_state_display;
    ObserverHints::CategoryEditingFlags         category_editing_flags;
    ObserverHints::RootIndexDisplayHint         root_index_display_hint;
    QList<Qtilities::Core::QtilitiesCategory>   displayed_categories;
    bool                                        has_inversed_category_display;
    bool                                        category_filter_enabled;
    bool                                        is_modified;
};

Qtilities::Core::ObserverHints::ObserverHints(QObject* parent) : QObject(parent), ObserverAwareBase() {
    d = new ObserverHintsPrivateData;

    Observer* obs = qobject_cast<Observer*> (parent);
    if (obs)
        setObserverContext(obs);
}

Qtilities::Core::ObserverHints::~ObserverHints() {
    delete d;
}

Qtilities::Core::ObserverHints::ObserverHints(const ObserverHints& other) : QObject(other.parent()), ObserverAwareBase() {
    d = new ObserverHintsPrivateData;
    d->observer_selection_context = other.observerSelectionContextHint();
    d->naming_control = other.namingControlHint();
    d->activity_display = other.activityDisplayHint();
    d->activity_control = other.activityControlHint();
    d->item_selection_control = other.itemSelectionControlHint();
    d->hierarhical_display = other.hierarchicalDisplayHint();
    d->display_flags = other.displayFlagsHint();
    d->item_view_column_hint = other.itemViewColumnHint();
    d->action_hints = other.actionHints();
    d->displayed_categories = other.displayedCategories();
    d->has_inversed_category_display = other.hasInversedCategoryDisplay();
    d->category_filter_enabled = other.categoryFilterEnabled();
    d->modification_state_display = other.modificationStateDisplayHint();
    d->category_editing_flags = other.categoryEditingFlags();
    d->root_index_display_hint = other.rootIndexDisplayHint();
    d->drag_drop_flags = other.dragDropHint();

    setIsExportable(other.isExportable());
}

ObserverHints& Qtilities::Core::ObserverHints::operator=(const ObserverHints& other) {
    if (this==&other) return *this;

    d->observer_selection_context = other.observerSelectionContextHint();
    d->naming_control = other.namingControlHint();
    d->activity_display = other.activityDisplayHint();
    d->activity_control = other.activityControlHint();
    d->item_selection_control = other.itemSelectionControlHint();
    d->hierarhical_display = other.hierarchicalDisplayHint();
    d->display_flags = other.displayFlagsHint();
    d->item_view_column_hint = other.itemViewColumnHint();
    d->action_hints = other.actionHints();
    d->displayed_categories = other.displayedCategories();
    d->has_inversed_category_display = other.hasInversedCategoryDisplay();
    d->category_filter_enabled = other.categoryFilterEnabled();
    d->modification_state_display = other.modificationStateDisplayHint();
    d->category_editing_flags = other.categoryEditingFlags();
    d->root_index_display_hint = other.rootIndexDisplayHint();
    d->drag_drop_flags = other.dragDropHint();

    setIsExportable(other.isExportable());

    if (observerContext())
        observerContext()->setModificationState(true);

    return *this;
}

bool Qtilities::Core::ObserverHints::operator==(const ObserverHints& other) const {
    if (d->observer_selection_context != other.observerSelectionContextHint())
        return false;
    if (d->naming_control != other.namingControlHint())
        return false;
    if (d->activity_display != other.activityDisplayHint())
        return false;
    if (d->activity_control != other.activityControlHint())
        return false;
    if (d->item_selection_control != other.itemSelectionControlHint())
        return false;
    if (d->hierarhical_display != other.hierarchicalDisplayHint())
        return false;
    if (d->display_flags != other.displayFlagsHint())
        return false;
    if (d->item_view_column_hint != other.itemViewColumnHint())
        return false;
    if (d->action_hints != other.actionHints())
        return false;
    if (d->displayed_categories != other.displayedCategories())
        return false;
    if (d->has_inversed_category_display != other.hasInversedCategoryDisplay())
        return false;
    if (d->category_filter_enabled != other.categoryFilterEnabled())
        return false;
    if (d->drag_drop_flags != other.dragDropHint())
        return false;
    if (d->modification_state_display != other.modificationStateDisplayHint())
        return false;
    if (d->category_editing_flags != other.categoryEditingFlags())
        return false;
    if (d->root_index_display_hint != other.rootIndexDisplayHint())
        return false;

    return true;
}

bool Qtilities::Core::ObserverHints::operator!=(const ObserverHints& other) const {
    return !(*this==other);
}

void Qtilities::Core::ObserverHints::setObserverSelectionContextHint(ObserverHints::ObserverSelectionContext observer_selection_context) {
    if (d->observer_selection_context == observer_selection_context)
        return;

    d->observer_selection_context = observer_selection_context;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ObserverSelectionContext Qtilities::Core::ObserverHints::observerSelectionContextHint() const {
    return d->observer_selection_context;
}

void Qtilities::Core::ObserverHints::setNamingControlHint(ObserverHints::NamingControl naming_control) {
    if (d->naming_control == naming_control)
        return;

    d->naming_control = naming_control;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::NamingControl Qtilities::Core::ObserverHints::namingControlHint() const {
    return d->naming_control;
}

void Qtilities::Core::ObserverHints::setActivityDisplayHint(ObserverHints::ActivityDisplay activity_display) {
    if (d->activity_display == activity_display)
        return;

    d->activity_display = activity_display;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ActivityDisplay Qtilities::Core::ObserverHints::activityDisplayHint() const {
    return d->activity_display;
}

void Qtilities::Core::ObserverHints::setActivityControlHint(ObserverHints::ActivityControl activity_control) {
    if (d->activity_control == activity_control)
        return;

    d->activity_control = activity_control;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ActivityControl Qtilities::Core::ObserverHints::activityControlHint() const {
    return d->activity_control;
}

void Qtilities::Core::ObserverHints::setItemSelectionControlHint(ObserverHints::ItemSelectionControl item_selection_control) {
    if (d->item_selection_control == item_selection_control)
        return;

    d->item_selection_control = item_selection_control;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ItemSelectionControl Qtilities::Core::ObserverHints::itemSelectionControlHint() const {
    return d->item_selection_control;
}

void Qtilities::Core::ObserverHints::setHierarchicalDisplayHint(ObserverHints::HierarchicalDisplay hierarhical_display) {
    if (d->hierarhical_display == hierarhical_display)
        return;

    d->hierarhical_display = hierarhical_display;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::HierarchicalDisplay Qtilities::Core::ObserverHints::hierarchicalDisplayHint() const {
    return d->hierarhical_display;
}

void Qtilities::Core::ObserverHints::setDisplayFlagsHint(ObserverHints::DisplayFlags display_flags) {
    if (d->display_flags == display_flags)
        return;

    d->display_flags = display_flags;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::DisplayFlags Qtilities::Core::ObserverHints::displayFlagsHint() const {
    return d->display_flags;
}

void Qtilities::Core::ObserverHints::setItemViewColumnHint(ObserverHints::ItemViewColumnFlags item_view_column_hint) {
    if (d->item_view_column_hint == item_view_column_hint)
        return;

    d->item_view_column_hint = item_view_column_hint;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ItemViewColumnFlags Qtilities::Core::ObserverHints::itemViewColumnHint() const {
    return d->item_view_column_hint;
}

void Qtilities::Core::ObserverHints::setActionHints(ObserverHints::ActionHints action_hints) {
    if (d->action_hints == action_hints)
        return;

    d->action_hints = action_hints;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ActionHints Qtilities::Core::ObserverHints::actionHints() const {
    return d->action_hints;
}

void Qtilities::Core::ObserverHints::setDragDropHint(ObserverHints::DragDropFlags drag_drop_flags) {
    d->drag_drop_flags = drag_drop_flags;
}

Qtilities::Core::ObserverHints::DragDropFlags Qtilities::Core::ObserverHints::dragDropHint() const {
    return d->drag_drop_flags;
}

void Qtilities::Core::ObserverHints::setModificationStateDisplayHint(ObserverHints::ModificationStateDisplayHint modification_state_display) {
    if (d->modification_state_display == modification_state_display)
        return;

    d->modification_state_display = modification_state_display;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ModificationStateDisplayHint Qtilities::Core::ObserverHints::modificationStateDisplayHint() const {
    return d->modification_state_display;
}

void Qtilities::Core::ObserverHints::setCategoryEditingFlags(ObserverHints::CategoryEditingFlags category_editing_flags) {
    if (d->category_editing_flags == category_editing_flags)
        return;

    d->category_editing_flags = category_editing_flags;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::CategoryEditingFlags Qtilities::Core::ObserverHints::categoryEditingFlags() const {
    return d->category_editing_flags;
}

void ObserverHints::setRootIndexDisplayHint(ObserverHints::RootIndexDisplayHint root_index_display_hint) {
    if (d->root_index_display_hint == root_index_display_hint)
        return;

    d->root_index_display_hint = root_index_display_hint;

    if (observerContext())
        observerContext()->setModificationState(true);
}

ObserverHints::RootIndexDisplayHint ObserverHints::rootIndexDisplayHint() const {
    return d->root_index_display_hint;
}

void Qtilities::Core::ObserverHints::setDisplayedCategories(const QList<QtilitiesCategory>& displayed_categories, bool inversed) {
    if (d->displayed_categories == displayed_categories && d->has_inversed_category_display == inversed)
        return;

    d->displayed_categories = displayed_categories;
    d->has_inversed_category_display = inversed;

    // Will update views connected to this signal.
    if (observerContext()) {
        observerContext()->setModificationState(true);
        // Processing cycle check is done in refreshViewsLayout():
        observerContext()->refreshViewsLayout();
    }
}

void Qtilities::Core::ObserverHints::addDisplayedCategory(const QtilitiesCategory& category) {
    for (int i = 0; i < d->displayed_categories.count(); ++i) {
        if (d->displayed_categories.at(i) == category)
            return;
    }

    d->displayed_categories.append(category);
}

void Qtilities::Core::ObserverHints::removeDisplayedCategory(const QtilitiesCategory& category) {
    for (int i = 0; i < d->displayed_categories.count(); ++i) {
        if (d->displayed_categories.at(i) == category) {
            d->displayed_categories.removeAt(i);
            return;
        }
    }
}

QList<Qtilities::Core::QtilitiesCategory> Qtilities::Core::ObserverHints::displayedCategories() const {
    return d->displayed_categories;
}

void Qtilities::Core::ObserverHints::setCategoryFilterEnabled(bool enabled) {
    if (enabled != d->category_filter_enabled) {
        d->category_filter_enabled = enabled;

        if (observerContext()) {
            observerContext()->setModificationState(true);
            observerContext()->refreshViewsLayout();
        }
    }
}

bool Qtilities::Core::ObserverHints::categoryFilterEnabled() const {
    return d->category_filter_enabled;
}

bool Qtilities::Core::ObserverHints::hasInversedCategoryDisplay() const {
    return d->has_inversed_category_display;
}

bool Qtilities::Core::ObserverHints::isModified() const {
    return d->is_modified;
}

void Qtilities::Core::ObserverHints::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

QString Qtilities::Core::ObserverHints::observerSelectionContextToString(ObserverSelectionContext observer_selection_context) {
    if (observer_selection_context == NoObserverSelectionContextHint) {
        return "NoObserverSelectionContextHint";
    } else if (observer_selection_context == SelectionUseParentContext) {
        return "SelectionUseParentContext";
    } else if (observer_selection_context == SelectionUseSelectedContext) {
        return "SelectionUseSelectedContext";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ObserverSelectionContext Qtilities::Core::ObserverHints::stringToObserverSelectionContext(const QString& observer_selection_context_string) {
    if (observer_selection_context_string == QLatin1String("NoObserverSelectionContextHint")) {
        return NoObserverSelectionContextHint;
    } else if (observer_selection_context_string == QLatin1String("SelectionUseParentContext")) {
        return SelectionUseParentContext;
    } else if (observer_selection_context_string == QLatin1String("SelectionUseSelectedContext")) {
        return SelectionUseSelectedContext;
    }

    Q_ASSERT(0);
    return NoObserverSelectionContextHint;
}

QString Qtilities::Core::ObserverHints::namingControlToString(NamingControl naming_control) {
    if (naming_control == NoNamingControlHint) {
        return "NoNamingControlHint";
    } else if (naming_control == ReadOnlyNames) {
        return "ReadOnlyNames";
    } else if (naming_control == EditableNames) {
        return "EditableNames";
    }

    return QString();
}

Qtilities::Core::ObserverHints::NamingControl Qtilities::Core::ObserverHints::stringToNamingControl(const QString& naming_control_string) {
    if (naming_control_string == QLatin1String("NoNamingControlHint")) {
        return NoNamingControlHint;
    } else if (naming_control_string == QLatin1String("ReadOnlyNames")) {
        return ReadOnlyNames;
    } else if (naming_control_string == QLatin1String("EditableNames")) {
        return EditableNames;
    }

    Q_ASSERT(0);
    return NoNamingControlHint;
}

QString Qtilities::Core::ObserverHints::activityDisplayToString(ActivityDisplay activity_display) {
    if (activity_display == NoActivityDisplayHint) {
        return "NoActivityDisplayHint";
    } else if (activity_display == NoActivityDisplay) {
        return "NoActivityDisplay";
    } else if (activity_display == CheckboxActivityDisplay) {
        return "CheckboxActivityDisplay";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ActivityDisplay Qtilities::Core::ObserverHints::stringToActivityDisplay(const QString& activity_display_string) {
    if (activity_display_string == QLatin1String("NoActivityDisplayHint")) {
        return NoActivityDisplayHint;
    } else if (activity_display_string == QLatin1String("NoActivityDisplay")) {
        return NoActivityDisplay;
    } else if (activity_display_string == QLatin1String("CheckboxActivityDisplay")) {
        return CheckboxActivityDisplay;
    }

    Q_ASSERT(0);
    return NoActivityDisplayHint;
}

QString Qtilities::Core::ObserverHints::activityControlToString(ActivityControl activity_control) {
    if (activity_control == NoActivityControlHint) {
        return "NoActivityControlHint";
    } else if (activity_control == NoActivityControl) {
        return "NoActivityControl";
    } else if (activity_control == FollowSelection) {
        return "FollowSelection";
    } else if (activity_control == CheckboxTriggered) {
        return "CheckboxTriggered";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ActivityControl Qtilities::Core::ObserverHints::stringToActivityControl(const QString& activity_control_string) {
    if (activity_control_string == QLatin1String("NoActivityControlHint")) {
        return NoActivityControlHint;
    } else if (activity_control_string == QLatin1String("NoActivityControl")) {
        return NoActivityControl;
    } else if (activity_control_string == QLatin1String("FollowSelection")) {
        return FollowSelection;
    } else if (activity_control_string == QLatin1String("CheckboxTriggered")) {
        return CheckboxTriggered;
    }

    Q_ASSERT(0);
    return NoActivityControlHint;
}

QString Qtilities::Core::ObserverHints::itemSelectionControlToString(ItemSelectionControl item_selection_control) {
    if (item_selection_control == NoItemSelectionControlHint) {
        return "NoItemSelectionControlHint";
    } else if (item_selection_control == SelectableItems) {
        return "SelectableItems";
    } else if (item_selection_control == NonSelectableItems) {
        return "NonSelectableItems";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ItemSelectionControl Qtilities::Core::ObserverHints::stringToItemSelectionControl(const QString& item_selection_control_string) {
    if (item_selection_control_string == QLatin1String("NoItemSelectionControlHint")) {
        return NoItemSelectionControlHint;
    } else if (item_selection_control_string == QLatin1String("SelectableItems")) {
        return SelectableItems;
    } else if (item_selection_control_string == QLatin1String("NonSelectableItems")) {
        return NonSelectableItems;
    }

    Q_ASSERT(0);
    return NoItemSelectionControlHint;
}

QString Qtilities::Core::ObserverHints::hierarchicalDisplayToString(HierarchicalDisplay hierarchical_display) {
    if (hierarchical_display == NoHierarchicalDisplayHint) {
        return "NoHierarchicalDisplayHint";
    } else if (hierarchical_display == FlatHierarchy) {
        return "FlatHierarchy";
    } else if (hierarchical_display == CategorizedHierarchy) {
        return "CategorizedHierarchy";
    }

    return QString();
}

Qtilities::Core::ObserverHints::HierarchicalDisplay Qtilities::Core::ObserverHints::stringToHierarchicalDisplay(const QString& hierarchical_display_string) {
    if (hierarchical_display_string == QLatin1String("NoHierarchicalDisplayHint")) {
        return NoHierarchicalDisplayHint;
    } else if (hierarchical_display_string == QLatin1String("FlatHierarchy")) {
        return FlatHierarchy;
    } else if (hierarchical_display_string == QLatin1String("CategorizedHierarchy")) {
        return CategorizedHierarchy;
    }

    Q_ASSERT(0);
    return NoHierarchicalDisplayHint;
}

QString Qtilities::Core::ObserverHints::itemViewColumnFlagsToString(ItemViewColumnFlags item_view_column_flags) {
    return QString("%1").arg((int) item_view_column_flags);
}

Qtilities::Core::ObserverHints::ItemViewColumnFlags Qtilities::Core::ObserverHints::stringToItemViewColumnFlags(const QString& item_view_column_flags_string) {
    return (ItemViewColumnFlags) item_view_column_flags_string.toInt();
}

QString Qtilities::Core::ObserverHints::displayFlagsToString(DisplayFlags display_flags) {
    return QString("%1").arg((int) display_flags);
}

Qtilities::Core::ObserverHints::DisplayFlags Qtilities::Core::ObserverHints::stringToDisplayFlags(const QString& display_flags_string) {
    return (DisplayFlags) display_flags_string.toInt();
}

QString Qtilities::Core::ObserverHints::actionHintsToString(ActionHints actions_hints) {
    return QString("%1").arg((int) actions_hints);
}

Qtilities::Core::ObserverHints::ActionHints Qtilities::Core::ObserverHints::stringToActionHints(const QString& actions_hints_string) {
    return (ActionHints) actions_hints_string.toInt();
}

QString Qtilities::Core::ObserverHints::dragDropFlagsToString(DragDropFlags drag_drop_flags) {
    return QString("%1").arg((int) drag_drop_flags);
}

Qtilities::Core::ObserverHints::DragDropFlags Qtilities::Core::ObserverHints::stringToDragDropFlags(const QString& drag_drop_flags_string) {
    return (DragDropFlags) drag_drop_flags_string.toInt();
}

QString Qtilities::Core::ObserverHints::categoryEditingFlagsToString(CategoryEditingFlags category_editing_flags) {
    return QString("%1").arg((int) category_editing_flags);
}

Qtilities::Core::ObserverHints::CategoryEditingFlags Qtilities::Core::ObserverHints::stringToCategoryEditingFlags(const QString& category_editing_flags) {
    return (CategoryEditingFlags) category_editing_flags.toInt();
}

QString ObserverHints::rootIndexDisplayHintToString(RootIndexDisplayHint root_index_display_hint) {
    if (root_index_display_hint == RootIndexHide) {
        return "RootIndexHide";
    } else if (root_index_display_hint == RootIndexDisplayDecorated) {
        return "RootIndexDisplayDecorated";
    } else if (root_index_display_hint == RootIndexDisplayUndecorated) {
        return "RootIndexDisplayUndecorated";
    }

    return QString();
}

ObserverHints::RootIndexDisplayHint ObserverHints::stringToRootIndexDisplayHint(const QString &root_index_display_hint_string) {
    if (root_index_display_hint_string == QLatin1String("RootIndexHide")) {
        return RootIndexHide;
    } else if (root_index_display_hint_string == QLatin1String("RootIndexDisplayDecorated")) {
        return RootIndexDisplayDecorated;
    } else if (root_index_display_hint_string == QLatin1String("RootIndexDisplayUndecorated")) {
        return RootIndexDisplayUndecorated;
    }

    Q_ASSERT(0);
    return RootIndexHide;
}

QString Qtilities::Core::ObserverHints::modificationStateDisplayToString(ModificationStateDisplayHint modification_display) {
    if (modification_display == NoModificationStateDisplayHint) {
        return "NoModificationStateDisplayHint";
    } else if (modification_display == CharacterModificationStateDisplay) {
        return "CharacterModificationStateDisplay";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ModificationStateDisplayHint Qtilities::Core::ObserverHints::stringToModificationStateDisplay(const QString& modification_display_string) {
    if (modification_display_string == QLatin1String("NoModificationStateDisplayHint")) {
        return NoModificationStateDisplayHint;
    } else if (modification_display_string == QLatin1String("CharacterModificationStateDisplay")) {
        return CharacterModificationStateDisplay;
    }

    Q_ASSERT(0);
    return NoModificationStateDisplayHint;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::ObserverHints::instanceFactoryInfo() const {
    return instanceFactoryInfo();
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::ObserverHints::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverHints::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << (quint32) d->observer_selection_context;
    stream << (quint32) d->naming_control;
    stream << (quint32) d->activity_display;
    stream << (quint32) d->activity_control;
    stream << (quint32) d->item_selection_control;
    stream << (quint32) d->hierarhical_display;
    stream << (quint32) d->display_flags;
    stream << (quint32) d->item_view_column_hint;
    stream << (quint32) d->action_hints;
    stream << (quint32) d->drag_drop_flags;
    stream << (quint32) d->modification_state_display;

    // -----------------------------------
    // Start of specific to Qtilities::Qtilities_1_1:
    // -----------------------------------
    if (exportVersion() == Qtilities::Qtilities_1_1) {
        stream << (quint32) d->category_editing_flags;
    }
    // -----------------------------------
    // End of specific to Qtilities::Qtilities_1_1:
    // -----------------------------------
    // Start of specific to Qtilities::Qtilities_1_2:
    // -----------------------------------
    if (exportVersion() == Qtilities::Qtilities_1_2) {
        stream << (quint32) d->root_index_display_hint;
    }
    // -----------------------------------
    // End of specific to Qtilities::Qtilities_1_2:
    // -----------------------------------

    stream << (quint32) d->displayed_categories.count();
    for (int i = 0; i < d->displayed_categories.count(); ++i)
        d->displayed_categories.at(i).exportBinary(stream);

    stream << d->has_inversed_category_display;
    stream << d->category_filter_enabled;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverHints::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;
     
    quint32 qi32;
    stream >> qi32;
    d->observer_selection_context = ObserverHints::ObserverSelectionContext (qi32);
    stream >> qi32;
    d->naming_control = ObserverHints::NamingControl (qi32);
    stream >> qi32;
    d->activity_display = ObserverHints::ActivityDisplay (qi32);
    stream >> qi32;
    d->activity_control = ObserverHints::ActivityControl (qi32);
    stream >> qi32;
    d->item_selection_control = ObserverHints::ItemSelectionControl (qi32);
    stream >> qi32;
    d->hierarhical_display = ObserverHints::HierarchicalDisplay (qi32);
    stream >> qi32;
    d->display_flags = ObserverHints::DisplayFlags (qi32);
    stream >> qi32;
    d->item_view_column_hint = ObserverHints::ItemViewColumnFlags (qi32);
    stream >> qi32;
    d->action_hints = ObserverHints::ActionHints (qi32);
    stream >> qi32;
    d->drag_drop_flags = ObserverHints::DragDropFlags (qi32);
    stream >> qi32;
    d->modification_state_display = ObserverHints::ModificationStateDisplayHint (qi32);

    // -----------------------------------
    // Start of specific to Qtilities::Qtilities_1_1:
    // -----------------------------------
    if (exportVersion() == Qtilities::Qtilities_1_1) {
        stream >> qi32;
        d->category_editing_flags = ObserverHints::CategoryEditingFlags (qi32);
    }
    // -----------------------------------
    // End of specific to Qtilities::Qtilities_1_1:
    // -----------------------------------
    // Start of specific to Qtilities::Qtilities_1_2:
    // -----------------------------------
    if (exportVersion() == Qtilities::Qtilities_1_2) {
        stream >> qi32;
        d->root_index_display_hint = ObserverHints::RootIndexDisplayHint (qi32);
    }
    // -----------------------------------
    // End of specific to Qtilities::Qtilities_1_2:
    // -----------------------------------

    stream >> qi32;
    int category_count = qi32;
    for (int i = 0; i < category_count; ++i) {
        QtilitiesCategory category(stream,exportVersion());
        d->displayed_categories << category;
    }

    stream >> d->has_inversed_category_display;
    stream >> d->category_filter_enabled;

    if (observerContext())
        observerContext()->setModificationState(true);

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverHints::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    // Export hints:
    if (d->action_hints != ActionNoHints)
        object_node->setAttribute("ActionHints",actionHintsToString(d->action_hints));
    if (d->activity_control != NoActivityControlHint)
        object_node->setAttribute("ActivityControl",activityControlToString(d->activity_control));
    if (d->activity_display != NoActivityDisplayHint)
        object_node->setAttribute("ActivityDisplay",activityDisplayToString(d->activity_display));
    if (d->display_flags != NoDisplayFlagsHint)
        object_node->setAttribute("DisplayFlags",displayFlagsToString(d->display_flags));
    if (d->drag_drop_flags != NoDragDrop)
        object_node->setAttribute("DragDropFlags",dragDropFlagsToString(d->drag_drop_flags));
    if (d->hierarhical_display != NoHierarchicalDisplayHint)
        object_node->setAttribute("HierarchicalDisplay",hierarchicalDisplayToString(d->hierarhical_display));
    if (d->item_view_column_hint != SelectableItems)
        object_node->setAttribute("ItemSelectionControl",itemSelectionControlToString(d->item_selection_control));
    if (d->item_view_column_hint != ColumnNoHints)
        object_node->setAttribute("ItemViewColumnFlags",itemViewColumnFlagsToString(d->item_view_column_hint));
    if (d->naming_control != NoNamingControlHint)
        object_node->setAttribute("NamingControl",namingControlToString(d->naming_control));
    if (d->observer_selection_context != SelectionUseParentContext)
        object_node->setAttribute("ObserverSelectionContext",observerSelectionContextToString(d->observer_selection_context));
    if (d->modification_state_display != NoModificationStateDisplayHint)
        object_node->setAttribute("ModificationStateDisplay",modificationStateDisplayToString(d->modification_state_display));

    // -----------------------------------
    // Start of specific to Qtilities::Qtilities_1_1:
    // -----------------------------------
    if (exportVersion() == Qtilities::Qtilities_1_1) {
        if (d->category_editing_flags != CategoriesReadOnly)
            object_node->setAttribute("CategoryEditingFlags",categoryEditingFlagsToString(d->category_editing_flags));
    }
    // -----------------------------------
    // End of specific to Qtilities::Qtilities_1_1:
    // -----------------------------------
    // Start of specific to Qtilities::Qtilities_1_2:
    // -----------------------------------
    if (exportVersion() == Qtilities::Qtilities_1_2) {
        if (d->root_index_display_hint != RootIndexHide)
            object_node->setAttribute("RootIndexDisplayHint",rootIndexDisplayHintToString(d->root_index_display_hint));
    }
    // -----------------------------------
    // End of specific to Qtilities::Qtilities_1_2:
    // -----------------------------------

    // Export category related stuff only if it is neccesarry:
    if (d->displayed_categories.count() > 0) {
        QDomElement category_data = doc->createElement("CategoryFilter");
        object_node->appendChild(category_data);

        if (d->category_filter_enabled)
            category_data.setAttribute("FilterEnabled","True");
        else
            category_data.setAttribute("FilterEnabled","False");
        if (d->has_inversed_category_display)
            category_data.setAttribute("FilterInversed","True");
        else
            category_data.setAttribute("FilterInversed","False");
        category_data.setAttribute("CategoryCount",d->displayed_categories.count());

        for (int i = 0; i < d->displayed_categories.count(); ++i) {
            QDomElement category_item = doc->createElement("Category_" + QString::number(i));
            d->displayed_categories.at(i).exportXml(doc,&category_item);
            category_data.appendChild(category_item);
        }
    }
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ObserverHints::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;
     
    // Hints:
    if (object_node->hasAttribute("ActionHints"))
        d->action_hints = stringToActionHints(object_node->attribute("ActionHints"));
    if (object_node->hasAttribute("ActivityControl"))
        d->activity_control = stringToActivityControl(object_node->attribute("ActivityControl"));
    if (object_node->hasAttribute("ActivityDisplay"))
        d->activity_display = stringToActivityDisplay(object_node->attribute("ActivityDisplay"));
    if (object_node->hasAttribute("DisplayFlags"))
        d->display_flags = stringToDisplayFlags(object_node->attribute("DisplayFlags"));
    if (object_node->hasAttribute("DragDropFlags"))
        d->drag_drop_flags = stringToDragDropFlags(object_node->attribute("DragDropFlags"));
    if (object_node->hasAttribute("HierarchicalDisplay"))
        d->hierarhical_display = stringToHierarchicalDisplay(object_node->attribute("HierarchicalDisplay"));
    if (object_node->hasAttribute("ItemSelectionControl"))
        d->item_selection_control = stringToItemSelectionControl(object_node->attribute("ItemSelectionControl"));
    if (object_node->hasAttribute("ItemViewColumnFlags"))
        d->item_view_column_hint = stringToItemViewColumnFlags(object_node->attribute("ItemViewColumnFlags"));
    if (object_node->hasAttribute("NamingControl"))
        d->naming_control = stringToNamingControl(object_node->attribute("NamingControl"));
    if (object_node->hasAttribute("ObserverSelectionContext"))
        d->observer_selection_context = stringToObserverSelectionContext(object_node->attribute("ObserverSelectionContext"));
    if (object_node->hasAttribute("ModificationStateDisplay"))
        d->modification_state_display = stringToModificationStateDisplay(object_node->attribute("ModificationStateDisplay"));

    // -----------------------------------
    // Start of specific to Qtilities v1.1:
    // -----------------------------------
    if (object_node->hasAttribute("CategoryEditingFlags"))
        d->category_editing_flags = stringToCategoryEditingFlags(object_node->attribute("CategoryEditingFlags"));
    // -----------------------------------
    // End of specific to Qtilities v1.1:
    // -----------------------------------
    // Start of specific to Qtilities v1.2:
    // -----------------------------------
    if (object_node->hasAttribute("RootIndexDisplayHint"))
        d->root_index_display_hint = stringToRootIndexDisplayHint(object_node->attribute("RootIndexDisplayHint"));
    // -----------------------------------
    // End of specific to Qtilities v1.2:
    // -----------------------------------

    // Category stuff:
    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); ++i)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == QLatin1String("CategoryFilter")) {
            if (child.attribute("FilterEnabled") == QLatin1String("True"))
                d->category_filter_enabled = true;
            else
                d->category_filter_enabled = false;
            if (child.attribute("FilterInversed") == QLatin1String("True"))
                d->has_inversed_category_display = true;
            else
                d->has_inversed_category_display = false;
            QDomNodeList categoryNodes = child.childNodes();
            for(int i = 0; i < categoryNodes.count(); ++i)
            {
                QDomNode categoryNode = categoryNodes.item(i);
                QDomElement category = categoryNode.toElement();

                if (category.isNull())
                    continue;

                if (category.tagName().startsWith("Category_")) {
                    QtilitiesCategory new_category;
                    new_category.importXml(doc,&category,import_list);
                    if (new_category.isValid())
                        d->displayed_categories << new_category;
                    continue;
                }
            }
            continue;
        }
    }

    return IExportable::Complete;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::ObserverHints& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::ObserverHints& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}
