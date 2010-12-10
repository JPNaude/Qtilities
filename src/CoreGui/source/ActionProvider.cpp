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

#include "ActionProvider.h"

struct Qtilities::CoreGui::ActionProviderData {
    ActionProviderData() {}

    QMap<QAction *, QtilitiesCategory>        actions;
    QMap<QActionGroup *, QtilitiesCategory>   action_groups;
};

Qtilities::CoreGui::ActionProvider::ActionProvider(QObject* parent) : QObject(parent) {
    d = new ActionProviderData;
}

Qtilities::CoreGui::ActionProvider::~ActionProvider() {
    delete d;
}

QList<QAction*> Qtilities::CoreGui::ActionProvider::actions(IActionProvider::ActionFilterFlags action_filter, const QtilitiesCategory& category_filter) const {
    // Loop through all actions and inspect them:
    QMap<QString, QAction*> filtered_map;
    for (int i = 0; i < d->actions.count(); i++) {
        QAction* action = d->actions.keys().at(i);
        bool add_action = true;

        // Check the category:
        if (d->actions.values().at(i) != category_filter && !category_filter.isEmpty())
            add_action = false;

        // Check for IActionProvider::FilterDisabled:
        if (action_filter & IActionProvider::FilterDisabled && add_action) {
            if (!action->isEnabled())
                add_action = false;
        }

        // Check for IActionProvider::FilterHidden:
        if (action_filter & IActionProvider::FilterHidden && add_action) {
            if (!action->isVisible())
                add_action = false;
        }

        if (add_action) {
            filtered_map[action->text()] = action;
        }
    }

    // We need to sort the list here by the action names:
    QStringList name_list = filtered_map.keys();
    qSort(name_list.begin(), name_list.end());

    // Build final list in sorted order and return it:
    QList<QAction*> final_list;
    for (int i = 0; i < name_list.count(); i++)
        final_list << filtered_map[name_list.at(i)];

    return final_list;
}

QMap<QAction*, QtilitiesCategory> Qtilities::CoreGui::ActionProvider::actionMap(IActionProvider::ActionFilterFlags action_filter, const QtilitiesCategory& category_filter) const {
    // Loop through all actions and inspect them:
    QMap<QAction*, QtilitiesCategory> filtered_map;
    for (int i = 0; i < d->actions.count(); i++) {
        QAction* action = d->actions.keys().at(i);
        bool add_action = true;

        // Check the category:
        if (d->actions.values().at(i) != category_filter && !category_filter.isEmpty())
            add_action = false;

        // Check for IActionProvider::FilterDisabled
        if (action_filter & IActionProvider::FilterDisabled && add_action) {
            if (!action->isEnabled())
                add_action = false;
        }

        // Check for IActionProvider::FilterHidden
        if (action_filter & IActionProvider::FilterHidden && add_action) {
            if (!action->isVisible())
                add_action = false;
        }

        if (add_action)
            filtered_map[d->actions.keys().at(i)] = d->actions.values().at(i);
    }

    return filtered_map;
}

QList<QtilitiesCategory> Qtilities::CoreGui::ActionProvider::actionCategories() const {
    QList<QtilitiesCategory> category_list;
    for (int i = 0; i < d->actions.count(); i++) {
        if (!category_list.contains(d->actions.values().at(i)))
            category_list << d->actions.values().at(i);
    }
    qSort(category_list);
    return category_list;
}

QList<QActionGroup*> Qtilities::CoreGui::ActionProvider::actionGroups() const {
    return d->action_groups.keys();
}

QMap<QActionGroup*, QtilitiesCategory> Qtilities::CoreGui::ActionProvider::actionGroupMap(QActionGroup *action_group) const {
    if (action_group) {
        QMap<QActionGroup*, QtilitiesCategory> filtered_map;
        filtered_map[action_group] = d->action_groups[action_group];
        return filtered_map;
    } else
        return d->action_groups;
}

QList<QtilitiesCategory> Qtilities::CoreGui::ActionProvider::actionGroupCategories() const {
    QList<QtilitiesCategory> category_list;
    for (int i = 0; i < d->action_groups.count(); i++) {
        if (!category_list.contains(d->action_groups.values().at(i)))
            category_list << d->action_groups.values().at(i);
    }
    return category_list;
}

QAction* Qtilities::CoreGui::ActionProvider::addAction(QAction* action, const QtilitiesCategory& category) {
    d->actions[action] = category;
    return action;
}

void Qtilities::CoreGui::ActionProvider::addActions(QList<QAction*> actions, const QtilitiesCategory& category) {
    foreach (QAction* action, actions) {
        addAction(action, category);
    }
}

QActionGroup* Qtilities::CoreGui::ActionProvider::addActionGroup(QActionGroup* action_group, const QtilitiesCategory& category) {
    d->action_groups[action_group] = category;
    return action_group;
}

void Qtilities::CoreGui::ActionProvider::disableAllActions() {
    for (int i = 0; i < d->actions.count(); i++) {
        d->actions.keys().at(i)->setEnabled(false);
    }
}

void Qtilities::CoreGui::ActionProvider::enableAllActions() {
    for (int i = 0; i < d->actions.count(); i++) {
        d->actions.keys().at(i)->setEnabled(true);
    }
}

QAction* Qtilities::CoreGui::ActionProvider::findActionByText(const QString& action_text) {
    for (int i = 0; i < d->actions.count(); i++) {
        if (d->actions.keys().at(i)->text() == action_text) {
            return d->actions.keys().at(i);
        }
    }

    return 0;
}

QAction* Qtilities::CoreGui::ActionProvider::findActionByObjectName(const QString& object_name) {
    for (int i = 0; i < d->actions.count(); i++) {
        if (d->actions.keys().at(i)->objectName() == object_name) {
            return d->actions.keys().at(i);
        }
    }

    return 0;
}
