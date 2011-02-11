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

#include "ActionContainer.h"
#include <QPointer>

// --------------------------------
// MenuContainer Implemenation
// --------------------------------

struct Qtilities::CoreGui::MenuContainerPrivateData {
    MenuContainerPrivateData() : this_menu(0) { }

   QMenu* this_menu;
   QList<QPointer<MenuContainer> > sub_menus;
   QMap<QString, QAction*> id_action_map;
};

Qtilities::CoreGui::MenuContainer::MenuContainer(const QString& name, QObject* parent) : ActionContainer(name, parent)
{
    d = new MenuContainerPrivateData;
    d->this_menu = new QMenu(container_name);
    setObjectName(name);
    d->this_menu->setObjectName(name);

    setEmptyPolicy(ActionContainer::Disable);
}

Qtilities::CoreGui::MenuContainer::~MenuContainer() {
    delete d->this_menu;
    delete d;
}

QMenu* Qtilities::CoreGui::MenuContainer::menu() const {
    return d->this_menu;
}

void Qtilities::CoreGui::MenuContainer::addAction(Command *command, const QString &before) {
    if (!command)
        return;

    d->id_action_map[command->defaultText()] = command->action();
    connect(command->action(),SIGNAL(changed()),SLOT(evaluateMenuActions()));

    // Find the action with the given default string.
    for (int i = 0; i < d->id_action_map.count(); i++) {
        if (d->id_action_map.keys().at(i) == before) {
            d->this_menu->insertAction(d->id_action_map[d->id_action_map.keys().at(i)],command->action());
            return;
        }
    }

   /* if (d->id_action_map.count() > 0)
        d->this_menu->insertAction(d->id_action_map[d->id_action_map.keys().at(0)],command->action());
    else
        d->this_menu->addAction(command->action());*/
    d->this_menu->addAction(command->action());
}

void Qtilities::CoreGui::MenuContainer::addSeperator(const QString &before) {
    QAction* sep = new QAction(d->this_menu);
    sep->setSeparator(true);

    // Find the action with the given string.
    for (int i = 0; i < d->id_action_map.count(); i++) {
        if (d->id_action_map.keys().at(i) == before) {
            d->this_menu->insertAction(d->id_action_map[d->id_action_map.keys().at(i)],sep);
            return;
        }
    }

    d->this_menu->addSeparator();
}

QMenuBar* Qtilities::CoreGui::MenuContainer::menuBar() const {
    return 0;
}

void Qtilities::CoreGui::MenuContainer::addMenu(ActionContainer *menu, const QString &before) {
    if (!menu)
        return;

    connect(menu->menu(),SIGNAL(aboutToHide()),SLOT(evaluateMenuActions()));

    // Find the action with the given string.
    for (int i = 0; i < d->id_action_map.count(); i++) {
        if (d->id_action_map.keys().at(i) == before) {
            d->this_menu->insertMenu(d->id_action_map[d->id_action_map.keys().at(i)],menu->menu());
            QPointer<MenuContainer> sub_menu_ptr;
            sub_menu_ptr = qobject_cast<MenuContainer*> (menu);
            d->sub_menus.push_front(sub_menu_ptr);
            menu->setParent(this);
            if (menu->menu())
                menu->setObjectName(menu->menu()->title());
            return;
        }
    }

    if (d->id_action_map.count() > 0)
        d->this_menu->insertMenu(d->id_action_map[d->id_action_map.keys().at(0)],menu->menu());
    else
        d->this_menu->addMenu(menu->menu());
}

void Qtilities::CoreGui::MenuContainer::evaluateMenuActions() {
    // We evaulate all sub menu and child action's. If they are all hidden, we hide this menu.
    /*bool visible_item = false;
    foreach (QAction* action, d->id_action_map.values()) {
        if (action->isVisible()) {
            visible_item = true;
            break;
        }
    }

    if (!visible_item) {
        QMenu* sender_menu = qobject_cast<QMenu*> (sender());
        for (int i = 0; i < d->sub_menus.count(); i++) {
            if (d->sub_menus.at(i)->menu()->isVisible()) {
                // Check that the visible menu is not the menu which emitted the aboutToHide() signal.
                if (sender_menu == d->sub_menus.at(i)->menu())
                    break;

                visible_item = true;
                break;
            }
        }
    }

    d->this_menu->setVisible(visible_item);*/
}

// --------------------------------
// MenuBarContainer Implemenation
// --------------------------------
struct Qtilities::CoreGui::MenuBarContainerPrivateData {
   MenuBarContainerPrivateData() : this_menu_bar(0) { }

   QMenuBar* this_menu_bar;
   QList<QPointer<ActionContainer> > menus;
};

Qtilities::CoreGui::MenuBarContainer::MenuBarContainer(QObject* parent) : ActionContainer(QString(), parent)
{
    d = new MenuBarContainerPrivateData;
    d->this_menu_bar = new QMenuBar(0);

    setEmptyPolicy(ActionContainer::None);
}

Qtilities::CoreGui::MenuBarContainer::~MenuBarContainer() {
    delete d;
}

QMenu* Qtilities::CoreGui::MenuBarContainer::menu() const {
    return 0;
}

void Qtilities::CoreGui::MenuBarContainer::addAction(Command *command, const QString &before) {
    Q_UNUSED(command)
    Q_UNUSED(before)
    return;
}

void Qtilities::CoreGui::MenuBarContainer::addSeperator(const QString &before) {
    Q_UNUSED(before)
    return;
}

QMenuBar* Qtilities::CoreGui::MenuBarContainer::menuBar() const {
    return d->this_menu_bar;
}

void Qtilities::CoreGui::MenuBarContainer::addMenu(ActionContainer *menu, const QString &before) {
    if (!menu)
        return;

    // Find the action with the given string.
    foreach (QAction* action, d->this_menu_bar->actions()) {
        if (action->text() == before) {
            d->this_menu_bar->insertMenu(action,menu->menu());
            menu->setParent(this);
            return;
        }
    }

    /*if (d->this_menu_bar->actions().count() > 0)
        d->this_menu_bar->insertMenu(d->this_menu_bar->actions().at(0),menu->menu());
    else
        d->this_menu_bar->addMenu(menu->menu()); */
    d->this_menu_bar->addMenu(menu->menu());
}

