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

#include "ActionManager.h"
#include "QtilitiesCoreGui.h"
#include "QtilitiesCoreGuiConstants.h"
#include "CommandEditor.h"

#include <ObserverProperty.h>
#include <QtilitiesCoreConstants.h>
#include <QtilitiesCore.h>
#include <Logger.h>
#include <Observer.h>

#include <QMainWindow>
#include <QList>
#include <QHash>
#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QtDebug>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;

struct Qtilities::CoreGui::ActionManagerData {
    ActionManagerData() { }

    QPointer<CommandEditor> command_editor;
    QHash<QString, Command* > id_command_map;
    QHash<QString, ActionContainer*> id_container_map;
};

Qtilities::CoreGui::ActionManager::ActionManager(QObject* parent) : IActionManager(parent)
{
    d = new ActionManagerData;
    setObjectName(tr("Action Manager"));

    // Give the manager an icon
    SharedObserverProperty shared_icon_property(QVariant(QIcon(QString(ICON_MANAGER_16x16))),OBJECT_ICON);
    shared_icon_property.setIsExportable(false);
    Observer::setSharedProperty(this,shared_icon_property);
}

Qtilities::CoreGui::ActionManager::~ActionManager()
{
    delete d;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::createMenu(const QString &id, bool& existed) {
    if (d->id_container_map.keys().contains(id)) {
        existed = true;
        return d->id_container_map[id];
    }

    existed = false;

    MenuContainer* new_container = new MenuContainer(id,this);

    if (new_container) {
        d->id_container_map[id] = new_container;
        return new_container;
    } else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::menu(const QString &id) {
    if (d->id_container_map.keys().contains(id))
        return d->id_container_map[id];
    else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::createMenuBar(const QString &id, bool& existed) {
    if (d->id_container_map.keys().contains(id)) {
        existed = true;
        return d->id_container_map[id];
    }

    existed = false;

    MenuBarContainer* new_container = new MenuBarContainer(this);

    if (new_container) {
        d->id_container_map[id] = new_container;
        return new_container;
    } else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::menuBar(const QString &id) {
    if (d->id_container_map.keys().contains(id))
        return d->id_container_map[id];
    else
        return 0;
}

Qtilities::CoreGui::Command *Qtilities::CoreGui::ActionManager::registerAction(const QString &id, QAction *action, const QList<int> &context) {
    if (!action)
        return 0;

    // Check if this action is intended to become part of a multi context action or not.
    if (d->id_command_map.keys().contains(id)) {        
        Command* cmd = d->id_command_map[id];
        // Check if we need to assign the text from the original action place holder creation call
        if (action->text().isEmpty())
            action->setText(cmd->text());

        MultiContextAction* multi = qobject_cast<MultiContextAction*> (cmd);
        if (multi) {           
            multi->addAction(action,context);
            // Handle the default key sequence
            if (multi->defaultKeySequence().isEmpty())
                multi->setDefaultKeySequence(action->shortcut());

            // Handle the key sequence
            if ((action->shortcut() != multi->keySequence()) && !multi->keySequence().isEmpty() && !action->shortcut().isEmpty()) {
                LOG_ERROR(QString(tr("Conflicting shortcut found for command %1 when trying to register shortcut %2. Original shortcut of %3 will be used.")).arg(action->text()).arg(action->shortcut().toString()).arg(multi->keySequence().toString()));
            }

            if (multi->keySequence().isEmpty() && !action->shortcut().isEmpty()) {
                //LOG_TRACE(QString(tr("Base action shortcut did not exist previously, now using shortcut (%1) from backend action: %2.")).arg(action->shortcut().toString()).arg(action->text()));
                multi->setKeySequence(action->shortcut());
            }

            action->setShortcut(QKeySequence());

            //LOG_TRACE(QString(tr("Registering new backend action for base action %1 (shortcut %2). New action: %3 (shortcut %4).")).arg(multi->text()).arg(multi->keySequence().toString()).arg(action->text()).arg(action->shortcut().toString()));

            return multi;
        } else
            return 0;
    } else {
        QAction* frontend_action = new QAction(action->text(),0);
        MultiContextAction* new_action = new MultiContextAction(frontend_action);
        if (new_action) {
            new_action->setDefaultText(id);
            new_action->addAction(action,context);
            new_action->setCurrentContext(QtilitiesCore::instance()->contextManager()->currentContexts());
            d->id_command_map[id] = new_action;

            new_action->setKeySequence(action->shortcut());
            action->setShortcut(QKeySequence());
            new_action->setDefaultKeySequence(action->shortcut());
            return new_action;
        } else
            return 0;
    }
}

Qtilities::CoreGui::Command* Qtilities::CoreGui::ActionManager::registerActionPlaceHolder(const QString &id, const QString& user_text, const QKeySequence& key_sequence, const QList<int> &context) {
    // First check if an action with the specified id already exist:
    if (commandMap().keys().contains(id)) {
        LOG_ERROR(tr("Attempting to register action place holder for an action which already exist with ID: ") + user_text);
        return 0;
    }

    // We create a default action. The user can connect to the trigger produced by Command->action() or add backend actions at a later stage.
    QAction* frontend_action;
    if (user_text.isEmpty())
        frontend_action = new QAction(id.split(".").last(),0);
    else
        frontend_action = new QAction(user_text,0);

    MultiContextAction* new_action = new MultiContextAction(frontend_action);
    if (new_action) {
        new_action->setDefaultText(id);
        if (!context.isEmpty()) {
            // We create a holder backend action, this action will be internal and the user should user the Command->action() call to use the command.
            QAction* holder_backend_action = new QAction(frontend_action->text(),0);
            new_action->addAction(holder_backend_action,context);
        }
        new_action->setCurrentContext(QtilitiesCore::instance()->contextManager()->currentContexts());
        d->id_command_map[id] = new_action;

        new_action->setKeySequence(key_sequence);
        new_action->setDefaultKeySequence(key_sequence);
        return new_action;
    } else
        return 0;

}

Qtilities::CoreGui::Command *Qtilities::CoreGui::ActionManager::registerShortcut(QShortcut *shortcut, const QString &default_text, const QList<int> &context) {
    return 0;
}

Qtilities::CoreGui::Command *Qtilities::CoreGui::ActionManager::command(const QString &id) const {
    if (d->id_command_map.keys().contains(id))
        return d->id_command_map.value(id);
    else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::actionContainer(const QString &id) const {
    return d->id_container_map.value(id);
}

void Qtilities::CoreGui::ActionManager::handleContextChanged(QList<int> new_contexts) {
    for (int i = 0; i < d->id_command_map.count(); i++) {
        if (d->id_command_map.values().at(i))
            d->id_command_map.values().at(i)->setCurrentContext(new_contexts);
    }
}

QHash<QString, Qtilities::CoreGui::Command* > Qtilities::CoreGui::ActionManager::commandMap() {
    return d->id_command_map;
}

void Qtilities::CoreGui::ActionManager::restoreDefaultShortcuts() {
    for (int i = 0; i < d->id_command_map.count(); i++) {
        Command* command = d->id_command_map.values().at(i);
        if (command) {
            command->setKeySequence(command->defaultKeySequence());
        }
    }
}

bool Qtilities::CoreGui::ActionManager::exportShortcutMapping(const QString& file_name) {
    QDomDocument doc("KeyboardMapping");
    QDomElement root = doc.createElement("Commands");
    doc.appendChild(root);

    for (int i = 0; i < d->id_command_map.count(); i++) {
        QDomElement tag = doc.createElement(d->id_command_map.keys().at(i));
        tag.setAttribute("KeySequence",d->id_command_map.values().at(i)->keySequence().toString());
        root.appendChild(tag);
    }

    QFile data(file_name);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        doc.save(out,4);
        return true;
    } else {
        return false;
    }
}

bool Qtilities::CoreGui::ActionManager::importShortcutMapping(const QString& file_name) {
    QDomDocument doc("KeyboardMapping");
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            QString attribute = e.attribute("KeySequence");
            if (!attribute.isEmpty()) {
                Command* command = d->id_command_map[e.tagName()];
                if (command)
                    command->setKeySequence(QKeySequence(attribute));
                else
                    LOG_WARNING(QString(tr("Unknown command found in the input shortcut mapping file: %1")).arg(e.tagName()));
            }

        }
        n = n.nextSibling();
    }

    return true;
}

QWidget* Qtilities::CoreGui::ActionManager::commandEditor() {
    if (!d->command_editor) {
        d->command_editor = new CommandEditor(0);
    }

    return d->command_editor;
}
