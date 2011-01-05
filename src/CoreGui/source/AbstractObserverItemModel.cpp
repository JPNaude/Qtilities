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

#include "AbstractObserverItemModel.h"

using namespace Qtilities::Core;

Qtilities::CoreGui::AbstractObserverItemModel::AbstractObserverItemModel() {
    model = new AbstractObserverItemModelData;
    model->hints_default = new ObserverHints();
    model->hints_selection_parent = 0;
    model->use_observer_hints = true;
}

Qtilities::CoreGui::AbstractObserverItemModel::~AbstractObserverItemModel() {
    delete model->hints_default;
    delete model;
}

void Qtilities::CoreGui::AbstractObserverItemModel::toggleUsesObserverHints(bool toggle, Observer* observer) {
    if (toggle == model->use_observer_hints)
        return;

    model->use_observer_hints = toggle;
    if (toggle)
        inheritObserverHints(observer);
    else
        inheritObserverHints(d_observer);
}

bool Qtilities::CoreGui::AbstractObserverItemModel::usesObserverHints() const {
    return model->use_observer_hints;
}

bool Qtilities::CoreGui::AbstractObserverItemModel::inheritObserverHints(const Observer* observer) {
    if (!observer) {
        if (model->hints_selection_parent) {
            delete model->hints_selection_parent;
            model->hints_selection_parent = 0;
        }
        return false;
    }

    // Ok now we use the hints from this observer
    model->hints_selection_parent = observer->displayHints();
    return true;
}

Qtilities::Core::ObserverHints* Qtilities::CoreGui::AbstractObserverItemModel::activeHints() const {
    if (model->use_observer_hints && model->hints_selection_parent)
        return model->hints_selection_parent;
    else
        return model->hints_default;
}

Qtilities::Core::ObserverHints* Qtilities::CoreGui::AbstractObserverItemModel::activeHints() {
    if (model->use_observer_hints && model->hints_selection_parent)
        return model->hints_selection_parent;
    else
        return model->hints_default;
}

bool Qtilities::CoreGui::AbstractObserverItemModel::setObserverContext(Observer* observer) {
    if (!ObserverAwareBase::setObserverContext(observer))
        return false;

    model->naming_filter = 0;
    model->activity_filter = 0;

    // Look which known subject filters are installed in this observer
    for (int i = 0; i < observer->subjectFilters().count(); i++) {
        // Check if it is a naming policy subject filter
        NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (observer->subjectFilters().at(i));
        if (naming_filter)
            model->naming_filter = naming_filter;

        // Check if it is an activity policy subject filter
        ActivityPolicyFilter* activity_filter = qobject_cast<ActivityPolicyFilter*> (observer->subjectFilters().at(i));
        if (activity_filter)
            model->activity_filter = activity_filter;
    }

    model->hints_selection_parent = observer->displayHints();
    model->hints_top_level_observer = observer->displayHints();
    return true;
}


