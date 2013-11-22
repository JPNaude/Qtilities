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

#include "AbstractObserverItemModel.h"

using namespace Qtilities::Core;

Qtilities::CoreGui::AbstractObserverItemModel::AbstractObserverItemModel() {
    model = new AbstractObserverItemModelData;
    model->hints_default = new ObserverHints;
    model->use_observer_hints = true;
    model->lazy_init = false;
}

Qtilities::CoreGui::AbstractObserverItemModel::~AbstractObserverItemModel() {
    if (model->hints_default)
        delete model->hints_default;
    delete model;
}

void Qtilities::CoreGui::AbstractObserverItemModel::toggleLazyInit(bool enabled) {
    model->lazy_init = enabled;
}

bool Qtilities::CoreGui::AbstractObserverItemModel::lazyInitEnabled() const {
    return model->lazy_init;
}

void Qtilities::CoreGui::AbstractObserverItemModel::toggleUseObserverHints(bool toggle) {
    model->use_observer_hints = toggle;
}

bool Qtilities::CoreGui::AbstractObserverItemModel::usesObserverHints() const {
    return model->use_observer_hints;
}

bool Qtilities::CoreGui::AbstractObserverItemModel::setCustomHints(ObserverHints* custom_hints) {
    if (!custom_hints)
        return false;

    *model->hints_default = *custom_hints;
    return true;
}

Qtilities::Core::ObserverHints* Qtilities::CoreGui::AbstractObserverItemModel::activeHints() const {
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
    for (int i = 0; i < observer->subjectFilters().count(); ++i) {
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

void Qtilities::CoreGui::AbstractObserverItemModel::setRespondToObserverChanges(bool respond_to_observer_changes) {
    //qDebug() << "Changing responding to observer changes in model";
    model->respond_to_observer_changes = respond_to_observer_changes;
}

bool Qtilities::CoreGui::AbstractObserverItemModel::respondToObserverChanges() const {
    return model->respond_to_observer_changes;
}

void Qtilities::CoreGui::AbstractObserverItemModel::setReadOnly(bool read_only) {
    model->read_only = read_only;
}

bool Qtilities::CoreGui::AbstractObserverItemModel::readOnly() const {
    return model->read_only;
}

void Qtilities::CoreGui::AbstractObserverItemModel::setColumnChildCountBaseClass(const QString &base_class_name) {
    model->child_count_base = base_class_name;
}

QString Qtilities::CoreGui::AbstractObserverItemModel::columnChildCountBaseClass() const {
    return model->child_count_base;
}

void Qtilities::CoreGui::AbstractObserverItemModel::setColumnChildCountLimit(int limit) {
    model->child_count_limit = limit;
}

int Qtilities::CoreGui::AbstractObserverItemModel::columnChildCountLimit() const {
    return model->child_count_limit;
}
