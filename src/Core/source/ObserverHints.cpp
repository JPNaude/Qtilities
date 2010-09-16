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

#include "ObserverHints.h"

#include <QDomElement>

struct Qtilities::Core::ObserverHintsData {
    ObserverHintsData() : observer_selection_context(ObserverHints::SelectionUseParentContext),
        naming_control(ObserverHints::NoNamingControlHint),
        activity_display(ObserverHints::NoActivityDisplayHint),
        activity_control(ObserverHints::NoActivityControlHint),
        item_selection_control(ObserverHints::SelectableItems),
        hierarhical_display(ObserverHints::NoHierarchicalDisplayHint),
        display_flags(ObserverHints::ItemView | ObserverHints::NavigationBar),
        item_view_column_hint(ObserverHints::ColumnNoHints),
        action_hints(ObserverHints::ActionNoHints),
        drag_drop_flags(ObserverHints::NoDragDrop),
        has_inversed_category_display(true),
        category_filter_enabled(false),
        is_modified(false),
        is_exportable(true) {}

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
    QList<QtilitiesCategory>                    displayed_categories;
    bool                                        has_inversed_category_display;
    bool                                        category_filter_enabled;
    bool                                        is_modified;
    bool                                        is_exportable;
};

Qtilities::Core::ObserverHints::ObserverHints(QObject* parent) : QObject(parent), ObserverAwareBase() {
    d = new ObserverHintsData;

    Observer* obs = qobject_cast<Observer*> (parent);
    if (obs)
        setObserverContext(obs);
}

Qtilities::Core::ObserverHints::~ObserverHints() {
    delete d;
}

Qtilities::Core::ObserverHints::ObserverHints(const ObserverHints& other) : QObject(other.parent()), ObserverAwareBase() {
    d = new ObserverHintsData;
    d->observer_selection_context = other.observerSelectionContextHint();
    d->naming_control = other.namingControlHint();
    d->activity_display = other.activityDisplayHint();
    d->activity_control = other.activityControlHint();
    d->item_selection_control = other.itemSelectionControlHint();
    d->hierarhical_display = other.hierarchicalDisplayHint();
    d->display_flags = other.displayFlagsHint();
    d->item_view_column_hint = other.itemViewColumnHint();
    d->action_hints = other.actionHints();
    d->drag_drop_flags = other.dragDropHint();
    d->displayed_categories = other.displayedCategories();
    d->has_inversed_category_display = other.hasInversedCategoryDisplay();
    d->category_filter_enabled = other.categoryFilterEnabled();

    if (observerContext())
        observerContext()->setModificationState(true);
}

void Qtilities::Core::ObserverHints::operator=(const ObserverHints& other) {
    d->observer_selection_context = other.observerSelectionContextHint();
    d->naming_control = other.namingControlHint();
    d->activity_display = other.activityDisplayHint();
    d->activity_control = other.activityControlHint();
    d->item_selection_control = other.itemSelectionControlHint();
    d->hierarhical_display = other.hierarchicalDisplayHint();
    d->display_flags = other.displayFlagsHint();
    d->item_view_column_hint = other.itemViewColumnHint();
    d->action_hints = other.actionHints();
    d->display_flags = other.displayFlagsHint();
    d->displayed_categories = other.displayedCategories();
    d->has_inversed_category_display = other.hasInversedCategoryDisplay();
    d->category_filter_enabled = other.categoryFilterEnabled();

    if (observerContext())
        observerContext()->setModificationState(true);
}

bool Qtilities::Core::ObserverHints::isExportable() const {
    return d->is_exportable;
}

void Qtilities::Core::ObserverHints::setIsExportable(bool is_exportable) {
    d->is_exportable = is_exportable;
}

void Qtilities::Core::ObserverHints::setObserverSelectionContextHint(ObserverHints::ObserverSelectionContext observer_selection_context) {
    d->observer_selection_context = observer_selection_context;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ObserverSelectionContext Qtilities::Core::ObserverHints::observerSelectionContextHint() const {
    return d->observer_selection_context;
}

void Qtilities::Core::ObserverHints::setNamingControlHint(ObserverHints::NamingControl naming_control) {
    d->naming_control = naming_control;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::NamingControl Qtilities::Core::ObserverHints::namingControlHint() const {
    return d->naming_control;
}

void Qtilities::Core::ObserverHints::setActivityDisplayHint(ObserverHints::ActivityDisplay activity_display) {
    d->activity_display = activity_display;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ActivityDisplay Qtilities::Core::ObserverHints::activityDisplayHint() const {
    return d->activity_display;
}

void Qtilities::Core::ObserverHints::setActivityControlHint(ObserverHints::ActivityControl activity_control) {
    d->activity_control = activity_control;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ActivityControl Qtilities::Core::ObserverHints::activityControlHint() const {
    return d->activity_control;
}

void Qtilities::Core::ObserverHints::setItemSelectionControlHint(ObserverHints::ItemSelectionControl item_selection_control) {
    d->item_selection_control = item_selection_control;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ItemSelectionControl Qtilities::Core::ObserverHints::itemSelectionControlHint() const {
    return d->item_selection_control;
}

void Qtilities::Core::ObserverHints::setHierarchicalDisplayHint(ObserverHints::HierarchicalDisplay hierarhical_display) {
    d->hierarhical_display = hierarhical_display;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::HierarchicalDisplay Qtilities::Core::ObserverHints::hierarchicalDisplayHint() const {
    return d->hierarhical_display;
}

void Qtilities::Core::ObserverHints::setDisplayFlagsHint(ObserverHints::DisplayFlags display_flags) {
    d->display_flags = display_flags;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::DisplayFlags Qtilities::Core::ObserverHints::displayFlagsHint() const {
    return d->display_flags;
}

void Qtilities::Core::ObserverHints::setItemViewColumnHint(ObserverHints::ItemViewColumnFlags item_view_column_hint) {
    d->item_view_column_hint = item_view_column_hint;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::ItemViewColumnFlags Qtilities::Core::ObserverHints::itemViewColumnHint() const {
    return d->item_view_column_hint;
}

void Qtilities::Core::ObserverHints::setActionHints(ObserverHints::ActionHints action_hints) {
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

void Qtilities::Core::ObserverHints::setDisplayedCategories(const QList<QtilitiesCategory>& displayed_categories, bool inversed) {
    d->displayed_categories = displayed_categories;
    d->has_inversed_category_display = inversed;

    // Will update views connected to this signal.
    if (observerContext()) {
        if (!observerContext()->isProcessingCycleActive()) {
            observerContext()->setModificationState(true);
            observerContext()->refreshViewsLayout();
        }
    }
}

void Qtilities::Core::ObserverHints::addDisplayedCategory(const QtilitiesCategory& category) {
    for (int i = 0; i < d->displayed_categories.count(); i++) {
        if (d->displayed_categories.at(i) == category)
            return;
    }

    d->displayed_categories.append(category);
}

void Qtilities::Core::ObserverHints::removeDisplayedCategory(const QtilitiesCategory& category) {
    for (int i = 0; i < d->displayed_categories.count(); i++) {
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

void Qtilities::Core::ObserverHints::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets) {
    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

QString Qtilities::Core::ObserverHints::observerSelectionContextToString(ObserverSelectionContext observer_selection_context) const {
    if (observer_selection_context == NoObserverSelectionContextHint) {
        return "NoObserverSelectionContextHint";
    } else if (observer_selection_context == SelectionUseParentContext) {
        return "SelectionUseParentContext";
    } else if (observer_selection_context == SelectionUseSelectedContext) {
        return "SelectionUseSelectedContext";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ObserverSelectionContext Qtilities::Core::ObserverHints::stringToObserverSelectionContext(const QString& observer_selection_context_string) const {
    if (observer_selection_context_string == "NoObserverSelectionContextHint") {
        return NoObserverSelectionContextHint;
    } else if (observer_selection_context_string == "SelectionUseParentContext") {
        return SelectionUseParentContext;
    } else if (observer_selection_context_string == "SelectionUseSelectedContext") {
        return SelectionUseSelectedContext;
    }

    Q_ASSERT(0);
    return NoObserverSelectionContextHint;
}

QString Qtilities::Core::ObserverHints::namingControlToString(NamingControl naming_control) const {
    if (naming_control == NoNamingControlHint) {
        return "NoNamingControlHint";
    } else if (naming_control == ReadOnlyNames) {
        return "ReadOnlyNames";
    } else if (naming_control == EditableNames) {
        return "EditableNames";
    }

    return QString();
}

Qtilities::Core::ObserverHints::NamingControl Qtilities::Core::ObserverHints::stringToNamingControl(const QString& naming_control_string) const {
    if (naming_control_string == "NoNamingControlHint") {
        return NoNamingControlHint;
    } else if (naming_control_string == "ReadOnlyNames") {
        return ReadOnlyNames;
    } else if (naming_control_string == "EditableNames") {
        return EditableNames;
    }

    Q_ASSERT(0);
    return NoNamingControlHint;
}

QString Qtilities::Core::ObserverHints::activityDisplayToString(ActivityDisplay activity_display) const {
    if (activity_display == NoActivityDisplayHint) {
        return "NoActivityDisplayHint";
    } else if (activity_display == NoActivityDisplay) {
        return "NoActivityDisplay";
    } else if (activity_display == CheckboxActivityDisplay) {
        return "CheckboxActivityDisplay";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ActivityDisplay Qtilities::Core::ObserverHints::stringToActivityDisplay(const QString& activity_display_string) const {
    if (activity_display_string == "NoActivityDisplayHint") {
        return NoActivityDisplayHint;
    } else if (activity_display_string == "NoActivityDisplay") {
        return NoActivityDisplay;
    } else if (activity_display_string == "CheckboxActivityDisplay") {
        return CheckboxActivityDisplay;
    }

    Q_ASSERT(0);
    return NoActivityDisplayHint;
}

QString Qtilities::Core::ObserverHints::activityControlToString(ActivityControl activity_control) const {
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

Qtilities::Core::ObserverHints::ActivityControl Qtilities::Core::ObserverHints::stringToActivityControl(const QString& activity_control_string) const {
    if (activity_control_string == "NoActivityControlHint") {
        return NoActivityControlHint;
    } else if (activity_control_string == "NoActivityControl") {
        return NoActivityControl;
    } else if (activity_control_string == "FollowSelection") {
        return FollowSelection;
    } else if (activity_control_string == "CheckboxTriggered") {
        return CheckboxTriggered;
    }

    Q_ASSERT(0);
    return NoActivityControlHint;
}

QString Qtilities::Core::ObserverHints::itemSelectionControlToString(ItemSelectionControl item_selection_control) const {
    if (item_selection_control == NoItemSelectionControlHint) {
        return "NoItemSelectionControlHint";
    } else if (item_selection_control == SelectableItems) {
        return "SelectableItems";
    } else if (item_selection_control == NonSelectableItems) {
        return "NonSelectableItems";
    }

    return QString();
}

Qtilities::Core::ObserverHints::ItemSelectionControl Qtilities::Core::ObserverHints::stringToItemSelectionControl(const QString& item_selection_control_string) const {
    if (item_selection_control_string == "NoItemSelectionControlHint") {
        return NoItemSelectionControlHint;
    } else if (item_selection_control_string == "SelectableItems") {
        return SelectableItems;
    } else if (item_selection_control_string == "NonSelectableItems") {
        return NonSelectableItems;
    }

    Q_ASSERT(0);
    return NoItemSelectionControlHint;
}

QString Qtilities::Core::ObserverHints::hierarchicalDisplayToString(HierarchicalDisplay hierarchical_display) const {
    if (hierarchical_display == NoHierarchicalDisplayHint) {
        return "NoHierarchicalDisplayHint";
    } else if (hierarchical_display == FlatHierarchy) {
        return "FlatHierarchy";
    } else if (hierarchical_display == CategorizedHierarchy) {
        return "CategorizedHierarchy";
    }

    return QString();
}

Qtilities::Core::ObserverHints::HierarchicalDisplay Qtilities::Core::ObserverHints::stringToHierarchicalDisplay(const QString& hierarchical_display_string) const {
    if (hierarchical_display_string == "NoHierarchicalDisplayHint") {
        return NoHierarchicalDisplayHint;
    } else if (hierarchical_display_string == "FlatHierarchy") {
        return FlatHierarchy;
    } else if (hierarchical_display_string == "CategorizedHierarchy") {
        return CategorizedHierarchy;
    }

    Q_ASSERT(0);
    return NoHierarchicalDisplayHint;
}

QString Qtilities::Core::ObserverHints::itemViewColumnFlagsToString(ItemViewColumnFlags item_view_column_flags) const {
    return QString("%1").arg((int) item_view_column_flags);
}

Qtilities::Core::ObserverHints::ItemViewColumnFlags Qtilities::Core::ObserverHints::stringToItemViewColumnFlags(const QString& item_view_column_flags_string) const {
    return (ItemViewColumnFlags) item_view_column_flags_string.toInt();
}

QString Qtilities::Core::ObserverHints::displayFlagsToString(DisplayFlags display_flags) const {
    return QString("%1").arg((int) display_flags);
}

Qtilities::Core::ObserverHints::DisplayFlags Qtilities::Core::ObserverHints::stringToDisplayFlags(const QString& display_flags_string) const {
    return (DisplayFlags) display_flags_string.toInt();
}

QString Qtilities::Core::ObserverHints::actionHintsToString(ActionHints actions_hints) const {
    return QString("%1").arg((int) actions_hints);
}

Qtilities::Core::ObserverHints::ActionHints Qtilities::Core::ObserverHints::stringToActionHints(const QString& actions_hints_string) const {
    return (ActionHints) actions_hints_string.toInt();
}

QString Qtilities::Core::ObserverHints::dragDropFlagsToString(DragDropFlags drag_drop_flags) const {
    return QString("%1").arg((int) drag_drop_flags);
}

Qtilities::Core::ObserverHints::DragDropFlags Qtilities::Core::ObserverHints::stringToDragDropFlags(const QString& drag_drop_flags_string) const {
    return (DragDropFlags) drag_drop_flags_string.toInt();
}


Qtilities::Core::Interfaces::IFactoryData Qtilities::Core::ObserverHints::factoryData() const {
    return factoryData();
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::ObserverHints::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverHints::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    Q_UNUSED(params)

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

    stream << (quint32) d->displayed_categories.count();
    for (int i = 0; i < d->displayed_categories.count(); i++)
        d->displayed_categories.at(i).exportBinary(stream);

    stream << d->has_inversed_category_display;
    stream << d->category_filter_enabled;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverHints::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(import_list)
    Q_UNUSED(params)

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
    int category_count = qi32;
    for (int i = 0; i < category_count; i++) {
        QtilitiesCategory category(stream);
        d->displayed_categories << category;
    }

    stream >> d->has_inversed_category_display;
    stream >> d->category_filter_enabled;

    if (observerContext())
        observerContext()->setModificationState(true);

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverHints::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    object_node->setAttribute("ActionHints",actionHintsToString(d->action_hints));
    object_node->setAttribute("ActivityControl",activityControlToString(d->activity_control));
    object_node->setAttribute("ActivityDisplay",activityDisplayToString(d->activity_display));
    object_node->setAttribute("DisplayFlags",displayFlagsToString(d->display_flags));
    object_node->setAttribute("DragDropFlags",dragDropFlagsToString(d->drag_drop_flags));
    object_node->setAttribute("HierarchicalDisplay",hierarchicalDisplayToString(d->hierarhical_display));
    object_node->setAttribute("ItemSelectionControl",itemSelectionControlToString(d->item_selection_control));
    object_node->setAttribute("ItemViewColumnFlags",itemViewColumnFlagsToString(d->item_view_column_hint));
    object_node->setAttribute("NamingControl",namingControlToString(d->naming_control));
    object_node->setAttribute("ObserverSelectionContext",observerSelectionContextToString(d->observer_selection_context));

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverHints::importXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(params)

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

    return IExportable::Complete;
}
