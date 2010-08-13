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

struct Qtilities::Core::ObserverHintsData {
    ObserverHintsData() : observer_selection_context(ObserverHints::SelectionUseParentContext),
        naming_control(ObserverHints::NoNamingControlHint),
        activity_display(ObserverHints::NoActivityDisplayHint),
        activity_control(ObserverHints::NoActivityControlHint),
        item_selection_control(ObserverHints::SelectableItems),
        hierarhical_display(ObserverHints::NoHierarhicalDisplayHint),
        display_flags(ObserverHints::ItemView | ObserverHints::NavigationBar),
        item_view_column_hint(ObserverHints::ColumnNoHint),
        action_hints(ObserverHints::None),
        displayed_categories(QStringList()),
        has_inversed_category_display(true),
        category_filter_enabled(false) {}

    ObserverHints::ObserverSelectionContext     observer_selection_context;
    ObserverHints::NamingControl                naming_control;
    ObserverHints::ActivityDisplay              activity_display;
    ObserverHints::ActivityControl              activity_control;
    ObserverHints::ItemSelectionControl         item_selection_control;
    ObserverHints::HierarhicalDisplay           hierarhical_display;
    ObserverHints::DisplayFlags                 display_flags;
    ObserverHints::ItemViewColumnFlags          item_view_column_hint;
    ObserverHints::ActionHints                  action_hints;
    QStringList                                 displayed_categories;
    bool                                        has_inversed_category_display;
    bool                                        category_filter_enabled;
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
    d->observer_selection_context = other.observerSelectionContext();
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

    if (observerContext())
        observerContext()->setModificationState(true);
}

void Qtilities::Core::ObserverHints::operator=(const ObserverHints& other) {
    d->observer_selection_context = other.observerSelectionContext();
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

    if (observerContext())
        observerContext()->setModificationState(true);
}

bool Qtilities::Core::ObserverHints::exportBinary(QDataStream& stream) const {
    stream << (quint32) d->observer_selection_context;
    stream << (quint32) d->naming_control;
    stream << (quint32) d->activity_display;
    stream << (quint32) d->activity_control;
    stream << (quint32) d->item_selection_control;
    stream << (quint32) d->hierarhical_display;
    stream << (quint32) d->display_flags;
    stream << (quint32) d->item_view_column_hint;
    stream << (quint32) d->action_hints;
    stream << d->displayed_categories;
    stream << d->has_inversed_category_display;
    stream << d->category_filter_enabled;
    return true;
}

bool Qtilities::Core::ObserverHints::importBinary(QDataStream& stream) {
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
    d->hierarhical_display = ObserverHints::HierarhicalDisplay (qi32);
    stream >> qi32;
    d->display_flags = ObserverHints::DisplayFlags (qi32);
    stream >> qi32;
    d->item_view_column_hint = ObserverHints::ItemViewColumnFlags (qi32);
    stream >> qi32;
    d->action_hints = ObserverHints::ActionHints (qi32);
    stream >> d->displayed_categories;
    stream >> d->has_inversed_category_display;
    stream >> d->category_filter_enabled;

    if (observerContext())
        observerContext()->setModificationState(true);

    return true;
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

void Qtilities::Core::ObserverHints::setHierarchicalDisplayHint(ObserverHints::HierarhicalDisplay hierarhical_display) {
    d->hierarhical_display = hierarhical_display;

    if (observerContext())
        observerContext()->setModificationState(true);
}

Qtilities::Core::ObserverHints::HierarhicalDisplay Qtilities::Core::ObserverHints::hierarchicalDisplayHint() const {
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

void Qtilities::Core::ObserverHints::setDisplayedCategories(const QStringList& displayed_categories, bool inversed) {
    d->displayed_categories = displayed_categories;
    d->has_inversed_category_display = inversed;

    // Will update views connected to this signal.
    if (observerContext()) {
        if (!observerContext()->isProcessingCycleActive()) {
            observerContext()->setModificationState(true);
        }
    }
}

QStringList Qtilities::Core::ObserverHints::displayedCategories() const {
    return d->displayed_categories;
}

void Qtilities::Core::ObserverHints::setCategoryFilterEnabled(bool enabled) {
    if (enabled != d->category_filter_enabled) {
        d->category_filter_enabled = enabled;

        if (observerContext())
            observerContext()->setModificationState(true);
    }
}

bool Qtilities::Core::ObserverHints::categoryFilterEnabled() const {
    return d->category_filter_enabled;
}

bool Qtilities::Core::ObserverHints::hasInversedCategoryDisplay() const {
    return d->has_inversed_category_display;
}
