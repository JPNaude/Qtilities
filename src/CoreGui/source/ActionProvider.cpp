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

#include "ActionProvider.h"

struct Qtilities::CoreGui::ActionProviderPrivateData {
    ActionProviderPrivateData() {}

    QMap<QAction *, QtilitiesCategory>        actions;
    QMap<QActionGroup *, QtilitiesCategory>   action_groups;
};

Qtilities::CoreGui::ActionProvider::ActionProvider(QObject* parent) : QObject(parent) {
    d = new ActionProviderPrivateData;
}

Qtilities::CoreGui::ActionProvider::~ActionProvider() {
    delete d;
}

QList<QAction*> Qtilities::CoreGui::ActionProvider::actions(IActionProvider::ActionFilterFlags action_filter, const QtilitiesCategory& category_filter) const {
    // Loop through all actions and inspect them:
    QMap<QString, QAction*> filtered_map;
    QList<QAction*> actions_keys = d->actions.keys();
    QList<QtilitiesCategory> actions_values = d->actions.values();
    for (int i = 0; i < d->actions.count(); ++i) {
        QAction* action = actions_keys.at(i);
        if (!action)
            continue;
        bool add_action = true;

        // Check the category:
        if (category_filter != QtilitiesCategory("Hello World!")) {
            if ((actions_values.at(i) != category_filter))
                add_action = false;
        }

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
    for (int i = 0; i < name_list.count(); ++i)
        final_list << filtered_map[name_list.at(i)];

    return final_list;
}

QMap<QAction*, QtilitiesCategory> Qtilities::CoreGui::ActionProvider::actionMap(IActionProvider::ActionFilterFlags action_filter, const QtilitiesCategory& category_filter) const {
    // Loop through all actions and inspect them:
    QMap<QAction*, QtilitiesCategory> filtered_map;
    QList<QAction*> actions_keys = d->actions.keys();
    QList<QtilitiesCategory> actions_values = d->actions.values();
    for (int i = 0; i < d->actions.count(); ++i) {
        QAction* action = actions_keys.at(i);
        bool add_action = true;

        // Check the category:
        if (actions_values.at(i) != category_filter && !category_filter.isEmpty())
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
            filtered_map[actions_keys.at(i)] = actions_values.at(i);
    }

    return filtered_map;
}

QList<QtilitiesCategory> Qtilities::CoreGui::ActionProvider::actionCategories() const {
    QList<QtilitiesCategory> category_list;
    QList<QtilitiesCategory> actions_values = d->actions.values();
    for (int i = 0; i < actions_values.count(); ++i) {
        if (!category_list.contains(actions_values.at(i)))
            category_list << actions_values.at(i);
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
    for (int i = 0; i < d->action_groups.count(); ++i) {
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
    QList<QAction*> actions_keys = d->actions.keys();
    for (int i = 0; i < d->actions.count(); ++i) {
        actions_keys.at(i)->setEnabled(false);
    }
}

void Qtilities::CoreGui::ActionProvider::enableAllActions() {
    QList<QAction*> actions_keys = d->actions.keys();
    for (int i = 0; i < d->actions.count(); ++i) {
        actions_keys.at(i)->setEnabled(true);
    }
}

QList<QAction*> Qtilities::CoreGui::ActionProvider::findActionsByText(const QString& match_string, Qt::MatchFlags match_flags) {
    QList<QAction*> matches;
    Qt::CaseSensitivity case_sensitivity = Qt::CaseInsensitive;
    if (match_flags & Qt::CaseSensitive)
        case_sensitivity = Qt::CaseSensitive;

    QList<QAction*> actions_keys = d->actions.keys();
    for (int i = 0; i < d->actions.count(); ++i) {
        if (match_flags == Qt::MatchFixedString) {
            if (actions_keys.at(i)->text().compare(match_string,case_sensitivity) == 0)
                matches << actions_keys.at(i);
        } else if (match_flags == Qt::MatchExactly) {
            if (actions_keys.at(i)->text().compare(match_string,case_sensitivity) == 0)
                matches << actions_keys.at(i);
        } else if (match_flags == Qt::MatchStartsWith) {
            if (actions_keys.at(i)->text().startsWith(match_string,case_sensitivity))
                matches << actions_keys.at(i);
        } else if (match_flags == Qt::MatchEndsWith) {
            if (actions_keys.at(i)->text().endsWith(match_string,case_sensitivity))
                matches << actions_keys.at(i);
        } else if (match_flags == Qt::MatchContains) {
            if (actions_keys.at(i)->text().contains(match_string,case_sensitivity))
                matches << actions_keys.at(i);
        } else if (match_flags == Qt::MatchWildcard) {
            QRegExp reg_exp(match_string,case_sensitivity,QRegExp::Wildcard);
            if (reg_exp.exactMatch(actions_keys.at(i)->text()))
                matches << actions_keys.at(i);
        } else if (match_flags == Qt::MatchRegExp) {
            QRegExp reg_exp(match_string,case_sensitivity);
            if (reg_exp.exactMatch(actions_keys.at(i)->text()))
                matches << actions_keys.at(i);
        }
    }

    return matches;
}

QList<QAction*> Qtilities::CoreGui::ActionProvider::findActionsByObjectName(const QString& match_string, Qt::MatchFlags match_flags) {
    QList<QAction*> matches;
    Qt::CaseSensitivity case_sensitivity = Qt::CaseInsensitive;
    if (match_flags & Qt::CaseSensitive)
        case_sensitivity = Qt::CaseSensitive;

    for (int i = 0; i < d->actions.count(); ++i) {
        if (match_flags == Qt::MatchFixedString) {
            if (d->actions.keys().at(i)->objectName().compare(match_string,case_sensitivity) == 0)
                matches << d->actions.keys().at(i);
        } else if (match_flags == Qt::MatchExactly) {
            if (d->actions.keys().at(i)->objectName().compare(match_string,case_sensitivity) == 0)
                matches << d->actions.keys().at(i);
        } else if (match_flags == Qt::MatchStartsWith) {
            if (d->actions.keys().at(i)->objectName().startsWith(match_string,case_sensitivity))
                matches << d->actions.keys().at(i);
        } else if (match_flags == Qt::MatchEndsWith) {
            if (d->actions.keys().at(i)->objectName().endsWith(match_string,case_sensitivity))
                matches << d->actions.keys().at(i);
        } else if (match_flags == Qt::MatchContains) {
            if (d->actions.keys().at(i)->objectName().contains(match_string,case_sensitivity))
                matches << d->actions.keys().at(i);
        } else if (match_flags == Qt::MatchWildcard) {
            QRegExp reg_exp(match_string,case_sensitivity,QRegExp::Wildcard);
            if (reg_exp.exactMatch(d->actions.keys().at(i)->objectName()))
                matches << d->actions.keys().at(i);
        } else if (match_flags == Qt::MatchRegExp) {
            QRegExp reg_exp(match_string,case_sensitivity);
            if (reg_exp.exactMatch(d->actions.keys().at(i)->objectName()))
                matches << d->actions.keys().at(i);
        }
    }

    return matches;
}

void Qtilities::CoreGui::ActionProvider::removeAction(QAction* action) {
    d->actions.remove(action);
}

void Qtilities::CoreGui::ActionProvider::removeActionCategory(const QtilitiesCategory& category) {
    QList<QAction*> actions_to_remove = d->actions.keys(category);
    foreach (QAction* action, actions_to_remove)
        d->actions.remove(action);
}
