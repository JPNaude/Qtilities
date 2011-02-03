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

#include "ActionManager.h"
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"
#include "CommandEditor.h"
#include "QtilitiesApplication.h"

#include <ObserverProperty.h>
#include <QtilitiesCoreConstants.h>
#include <Logger.h>
#include <Observer.h>
#include <Qtilities.h>

#include <QMainWindow>
#include <QList>
#include <QHash>
#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <QMessageBox>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;

bool Qtilities::CoreGui::ActionManager::showed_warning;

struct Qtilities::CoreGui::ActionManagerData {
    ActionManagerData() { }

    QPointer<CommandEditor> command_editor;
    QHash<QString, Command*> id_command_map;
    QHash<QString, ActionContainer*> id_container_map;
};

Qtilities::CoreGui::ActionManager::ActionManager(QObject* parent) : IActionManager(parent)
{
    d = new ActionManagerData;
    setObjectName(tr("Action Manager"));

    // Give the manager an icon
    SharedObserverProperty shared_icon_property(QVariant(QIcon(QString(ICON_MANAGER_16x16))),OBJECT_ROLE_DECORATION);
    shared_icon_property.setIsExportable(false);
    Observer::setSharedProperty(this,shared_icon_property);

    showed_warning = false;

    // Make sure App_Path/plugins always exists:
    QDir sessionDir = QDir(QCoreApplication::applicationDirPath() + "/session");
    if (!sessionDir.exists()) {
        sessionDir.cdUp();
        sessionDir.mkdir("/session");
    }
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

    // Check if there is already a front end action for this action id:
    Command* command = 0;
    if (d->id_command_map.keys().contains(id))
        command = d->id_command_map[id];
    else
        command = registerActionPlaceHolder(id,action->text());

    if (command) {
        // Check if we need to assign the text from the original action place holder creation call:
        if (action->text().isEmpty())
            action->setText(command->text());

        MultiContextAction* multi = qobject_cast<MultiContextAction*> (command);
        if (multi) {           
            multi->setObjectName(id);
            multi->addAction(action,context);

            // Set the action's object name equal to the id:
            action->setObjectName(id);

            // Handle the default key sequence
            if (multi->defaultKeySequence().isEmpty())
                multi->setDefaultKeySequence(action->shortcut());

            // Handle the key sequence
            if ((action->shortcut() != multi->keySequence()) && !multi->keySequence().isEmpty() && !action->shortcut().isEmpty())
                LOG_ERROR(QString(tr("Conflicting shortcut found for command %1 when trying to register shortcut %2. Original shortcut of %3 will be used.")).arg(action->text()).arg(action->shortcut().toString()).arg(multi->keySequence().toString()));

            if (multi->keySequence().isEmpty() && !action->shortcut().isEmpty()) {
                #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
                LOG_TRACE(QString(tr("Base action shortcut did not exist previously, now using shortcut (%1) from backend action: %2.")).arg(action->shortcut().toString()).arg(action->text()));
                #endif
                multi->setKeySequence(action->shortcut());
            }

            #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
            LOG_TRACE(QString(tr("Registering new backend action for base action %1 (shortcut %2). New action: %3 (shortcut %4).")).arg(multi->text()).arg(multi->keySequence().toString()).arg(action->text()).arg(action->shortcut().toString()));
            #endif

            // We set the backend action's shortcut to nothing, otherwise we get ambigious action shortcuts.
            action->setShortcut(QKeySequence());

            //emit numberOfCommandsChanged();
            return multi;
        }
    }

    return 0;
}

Qtilities::CoreGui::Command* Qtilities::CoreGui::ActionManager::registerActionPlaceHolder(const QString &id, const QString& user_text, const QKeySequence& key_sequence, const QList<int> &context) {
    // First check if an action with the specified id already exist:
    if (d->id_command_map.keys().contains(id)) {
        LOG_ERROR(tr("Attempting to register action place holder for a command which already exist with ID: ") + id);
        return 0;
    }

    // We create a default action. The user can connect to the trigger produced by Command->action() or add backend actions at a later stage:
    QAction* frontend_action;
    if (user_text.isEmpty())
        frontend_action = new QAction(id.split(".").last(),0);
    else
        frontend_action = new QAction(user_text,0);

    frontend_action->setObjectName(id);
    frontend_action->setShortcutContext(Qt::ApplicationShortcut);

    if (!QtilitiesApplication::mainWindow()) {
        // Show a message box since action manager will always be used in QtGui application.
        if (!showed_warning) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Action Manager");
            msgBox.setText("QtilitiesApplication::mainWindow() is required when registering actions in the action manager.<br><br>Multi context actions will not work as intended.");
            msgBox.exec();
            showed_warning = true;
        }
    } else
        QtilitiesApplication::mainWindow()->addAction(frontend_action);

    MultiContextAction* new_action = new MultiContextAction(frontend_action);
    if (new_action) {
        new_action->setObjectName(id);
        new_action->setDefaultText(id);
        if (!context.isEmpty()) {
            // We create a holder backend action, this action will be internal and the user should user the Command->action() call to use the command.
            QAction* holder_backend_action = new QAction(frontend_action->text(),0);
            new_action->addAction(holder_backend_action,context);
        }
        new_action->setCurrentContext(CONTEXT_MANAGER->activeContexts());
        d->id_command_map[id] = new_action;

        new_action->setKeySequence(key_sequence);
        new_action->setDefaultKeySequence(key_sequence);
        emit numberOfCommandsChanged();
        return new_action;
    } else {
        if (frontend_action)
            delete frontend_action;
        return 0;
    }
}

Qtilities::CoreGui::Command *Qtilities::CoreGui::ActionManager::registerShortcut(const QString &id, const QString& user_text, QShortcut *shortcut, const QList<int> &active_contexts) {
    if (!shortcut)
        return 0;

    // First check if a command with the specified id already exist:
    if (d->id_command_map.keys().contains(id)) {
        LOG_ERROR(tr("Attempting to register a shortcut for a command which already exist with ID: ") + id);
        return 0;
    }

    QList<int> contexts = active_contexts;
    if (contexts.isEmpty()) {
        // We associate it with the standard context:
        contexts << CONTEXT_MANAGER->contextID(Qtilities::Core::Constants::CONTEXT_STANDARD);
    }

    // Create new shortcut:
    ShortcutCommand* new_shortcut = new ShortcutCommand(user_text,shortcut,contexts,QtilitiesApplication::mainWindow());
    if (new_shortcut) {
        new_shortcut->setDefaultText(id);
        new_shortcut->setCurrentContext(CONTEXT_MANAGER->activeContexts());
        d->id_command_map[id] = new_shortcut;

        new_shortcut->setKeySequence(shortcut->key());
        new_shortcut->setDefaultKeySequence(shortcut->key());
        emit numberOfCommandsChanged();
        return new_shortcut;
    }

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

    // Export version:
    QDomElement root = doc.createElement("ShortcutStorage");
    doc.appendChild(root);
    QDomElement formatting = doc.createElement("Formatting");
    QDomElement export_version = doc.createElement("Export_Format");
    QDomElement qtilities_version = doc.createElement("Qtilities");
    QDomElement application_version = doc.createElement("Application");
    export_version.setAttribute("version",QString("%1").arg(QTILITIES_SHORTCUT_EXPORT_FORMAT));
    qtilities_version.setAttribute("version",QtilitiesApplication::qtilitiesVersion());
    application_version.setAttribute("version",QtilitiesApplication::applicationVersion());
    formatting.appendChild(qtilities_version);
    formatting.appendChild(application_version);
    formatting.appendChild(export_version);
    root.appendChild(formatting);

    // All shortcuts:
    QDomElement shortcuts = doc.createElement("Shortcuts");
    root.appendChild(shortcuts);
    for (int i = 0; i < d->id_command_map.count(); i++) {
        QDomElement tag = doc.createElement(d->id_command_map.keys().at(i));
        tag.setAttribute("KeySequence",d->id_command_map.values().at(i)->keySequence().toString());
        shortcuts.appendChild(tag);
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
    QDomDocument doc("ShortcutMapping");
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QString error_string;
    int error_line;
    int error_column;
    if (!doc.setContent(&file,&error_string,&error_line,&error_column)) {
        LOG_ERROR(QString(tr("The shortcut file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string));
        file.close();
        return false;
    }
    file.close();

    bool valid_file = false;
    QDomElement docElem = doc.documentElement();
    QDomNode storage_node = docElem.firstChild();
    QDomElement storage_element = storage_node.toElement();
    QDomNode formatting_node = storage_element.firstChild();
    QDomElement formatting_element = formatting_node.toElement();

    QDomNode formatting_version_node = formatting_element.nextSibling();
    while(!formatting_version_node.isNull()) {
        QDomElement formatting_version_element = formatting_version_node.toElement();
        if(!formatting_version_element.isNull()) {
            QString version_string = formatting_version_element.attribute("version");
            if (!version_string.isEmpty() && (formatting_version_element.tagName() == "Export_Format")) {
                bool ok;
                int version = version_string.toInt(&ok);
                if (ok) {
                    if (version == (int) QTILITIES_SHORTCUT_EXPORT_FORMAT)
                        valid_file = true;
                }
            }
        }
        formatting_version_node = formatting_version_node.nextSibling();
    }

    if (!valid_file) {
        LOG_ERROR(QString(tr("The shortcut file is not in a recognizable format. Import will fail.")));
        return false;
    }

    // Get all shortcuts:
    QDomNode shortcut_node = storage_node.nextSibling();
    QDomElement shortcut_element = shortcut_node.toElement();

    QDomNode shortcut_item_node = shortcut_element.firstChild();
    while(!shortcut_item_node.isNull()) {
        QDomElement shortcut_item_element = shortcut_item_node.toElement();
        if(!shortcut_item_element.isNull()) {
            QString attribute = shortcut_item_element.attribute("KeySequence");
            if (d->id_command_map.contains(shortcut_item_element.tagName())) {
                Command* command = d->id_command_map[shortcut_item_element.tagName()];
                if (command) {
                    command->setKeySequence(QKeySequence(attribute));
                    #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
                    LOG_TRACE("Importing shortcut for action: " + shortcut_item_element.tagName() + ", Shortcut: " + attribute);
                    #endif
                }

            } else {
                LOG_WARNING(QString(tr("Unknown command found in the input shortcut mapping file: %1")).arg(shortcut_item_element.tagName()));
            }
        }
        shortcut_item_node = shortcut_item_node.nextSibling();
    }

    return true;
}

QWidget* Qtilities::CoreGui::ActionManager::commandEditor() {
    if (!d->command_editor) {
        d->command_editor = new CommandEditor(0);
    }

    return d->command_editor;
}

void Qtilities::CoreGui::ActionManager::handleCommandDeleted(QObject* obj) {
    Q_UNUSED(obj)
    // We need to remove obj for our lists:

}
