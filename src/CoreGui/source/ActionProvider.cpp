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

    QMap<QAction *, QStringList>        actions;
    QMap<QActionGroup *, QStringList>   action_groups;
};

Qtilities::CoreGui::ActionProvider::ActionProvider(QObject* parent) : QObject(parent) {
    d = new ActionProviderData;
}

Qtilities::CoreGui::ActionProvider::~ActionProvider() {
    delete d;
}

QList<QAction*> Qtilities::CoreGui::ActionProvider::actions(bool only_enabled, const QStringList& category_filter) const {
    if (only_enabled && !category_filter.isEmpty()) {
        QList<QAction*> filtered_list;
        for (int i = 0; i < d->actions.count(); i++) {
            if (d->actions.values().at(i) == category_filter)
                filtered_list.append(d->actions.keys().at(i));
        }
        foreach(QAction* action, filtered_list) {
            if (action->isEnabled())
                filtered_list.removeOne(action);
        }
        return filtered_list;
    } else if (only_enabled && category_filter.isEmpty()) {
        QList<QAction*> filtered_list;
        foreach(QAction* action, d->actions.keys()) {
            if (action->isEnabled())
                filtered_list.append(action);
        }
        return filtered_list;
    } else if (!only_enabled && !category_filter.isEmpty()) {
        QList<QAction*> filtered_list;
        for (int i = 0; i < d->actions.count(); i++) {
            if (d->actions.values().at(i) == category_filter)
                filtered_list.append(d->actions.keys().at(i));
        }
        return filtered_list;
    }
    return d->actions.keys();
}

QMap<QAction*, QStringList> Qtilities::CoreGui::ActionProvider::actionMap(bool only_enabled, const QStringList& category_filter) const {
    if (only_enabled && !category_filter.isEmpty()) {
        QMap<QAction*, QStringList> filtered_map;
        for (int i = 0; i < d->actions.count(); i++) {
            if (d->actions.values().at(i) == category_filter)
                filtered_map[d->actions.keys().at(i)] = d->actions.values().at(i);
        }
        QMap<QAction*, QStringList> filtered_map_2;
        for (int i = 0; i < filtered_map.count(); i++) {
            if (filtered_map.keys().at(i)->isEnabled())
                filtered_map_2[filtered_map.keys().at(i)] = filtered_map.values().at(i);
        }
        return filtered_map_2;
    } else if (only_enabled && category_filter.isEmpty()) {
        QMap<QAction*, QStringList> filtered_map;
        for (int i = 0; i < d->actions.count(); i++) {
            if (d->actions.keys().at(i)->isEnabled())
                filtered_map[d->actions.keys().at(i)] = d->actions.values().at(i);
        }
        return filtered_map;
    } else if (!only_enabled && !category_filter.isEmpty()) {
        QMap<QAction*, QStringList> filtered_map;
        for (int i = 0; i < d->actions.count(); i++) {
            if (d->actions.values().at(i) == category_filter)
                filtered_map[d->actions.keys().at(i)] = d->actions.values().at(i);
        }
        return filtered_map;
    }
    return d->actions;
}

QList<QStringList> Qtilities::CoreGui::ActionProvider::actionCategories() const {
    QList<QStringList> category_list;
    for (int i = 0; i < d->actions.count(); i++) {
        if (!category_list.contains(d->actions.values().at(i)))
            category_list << d->actions.values().at(i);
    }
    return category_list;
}

QList<QActionGroup*> Qtilities::CoreGui::ActionProvider::actionGroups() const {
    return d->action_groups.keys();
}

QMap<QActionGroup*, QStringList> Qtilities::CoreGui::ActionProvider::actionGroupMap(QActionGroup *action_group) const {
    if (action_group) {
        QMap<QActionGroup*, QStringList> filtered_map;
        filtered_map[action_group] = d->action_groups[action_group];
        return filtered_map;
    } else
        return d->action_groups;
}

QList<QStringList> Qtilities::CoreGui::ActionProvider::actionGroupCategories() const {
    QList<QStringList> category_list;
    for (int i = 0; i < d->action_groups.count(); i++) {
        if (!category_list.contains(d->action_groups.values().at(i)))
            category_list << d->action_groups.values().at(i);
    }
    return category_list;
}

QAction* Qtilities::CoreGui::ActionProvider::addAction(QAction* action, const QStringList& category) {
    d->actions[action] = category;
    return action;
}

QActionGroup* Qtilities::CoreGui::ActionProvider::addActionGroup(QActionGroup* action_group, const QStringList& category) {
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
