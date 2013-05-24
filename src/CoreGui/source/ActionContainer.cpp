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

#include "ActionContainer.h"
#include <QPointer>

// --------------------------------
// MenuContainer Implementation
// --------------------------------

struct Qtilities::CoreGui::MenuContainerPrivateData {
    MenuContainerPrivateData() : this_menu(0) { }

   QMenu* this_menu;
   QList<QPointer<MenuContainer> > sub_menus;
   QMap<QString, QAction*> id_action_map;
};

Qtilities::CoreGui::MenuContainer::MenuContainer(const QString& name, QObject* parent) : ActionContainer(parent)
{
    d = new MenuContainerPrivateData;
    d->this_menu = new QMenu(name);
    setObjectName(name);
    d->this_menu->setObjectName(name);
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

    // Find the action with the given default string.
    QList<QString> actions = d->id_action_map.keys();
    int count = actions.count();
    for (int i = 0; i < count; ++i) {
        if (actions.at(i) == before) {
            d->this_menu->insertAction(d->id_action_map[actions.at(i)],command->action());
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
    QList<QString> actions = d->id_action_map.keys();
    int count = actions.count();
    for (int i = 0; i < count; ++i) {
        if (actions.at(i) == before) {
            d->this_menu->insertAction(d->id_action_map[actions.at(i)],sep);
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

    // Find the action with the given string.
    QList<QString> actions = d->id_action_map.keys();
    int count = actions.count();
    for (int i = 0; i < count; ++i) {
        if (actions.at(i) == before) {
            d->this_menu->insertMenu(d->id_action_map[actions.at(i)],menu->menu());
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

// --------------------------------
// MenuBarContainer Implementation
// --------------------------------
struct Qtilities::CoreGui::MenuBarContainerPrivateData {
   MenuBarContainerPrivateData() : this_menu_bar(0) { }

   QMenuBar* this_menu_bar;
   QList<QPointer<ActionContainer> > menus;
};

Qtilities::CoreGui::MenuBarContainer::MenuBarContainer(QObject* parent) : ActionContainer(parent)
{
    d = new MenuBarContainerPrivateData;
    d->this_menu_bar = new QMenuBar(0);
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

