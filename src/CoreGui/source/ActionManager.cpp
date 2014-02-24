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

#include "ActionManager.h"
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"
#include "CommandEditor.h"
#include "QtilitiesApplication.h"
#include "TreeNode.h"

#include <QtilitiesProperty>
#include <QtilitiesCoreConstants>
#include <QtilitiesLogging>
#include <Qtilities.h>
#include <SubjectIterator>

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
using namespace Qtilities::CoreGui;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Logging::Constants;

bool Qtilities::CoreGui::ActionManager::showed_warning;

struct Qtilities::CoreGui::ActionManagerPrivateData {
    ActionManagerPrivateData() : observer_commands("Registered Commands"),
      observer_action_container("Registered Action Containers") { }

    QPointer<CommandEditor> command_editor;
    TreeNode observer_commands;
    TreeNode observer_action_container;
};

Qtilities::CoreGui::ActionManager::ActionManager(QObject* parent) : IActionManager(parent)
{
    d = new ActionManagerPrivateData;
    setObjectName(tr("Action Manager"));
    showed_warning = false;

    // Set up the observers:
    d->observer_commands.enableCategorizedDisplay();
    d->observer_commands.displayHints()->setActionHints(ObserverHints::ActionFindItem | ObserverHints::ActionRefreshView);
    d->observer_commands.displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);

    connect(CONTEXT_MANAGER,SIGNAL(finishedUnregisterContext(int)),SLOT(unregisterCommandsForContext(int)));
}

Qtilities::CoreGui::ActionManager::~ActionManager()
{
    delete d;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::createMenu(const QString &id, bool& existed) {
    if (d->observer_action_container.containsSubjectWithName(id)) {
        existed = true;
        QObject* obj = d->observer_action_container.subjectReference(id);
        return qobject_cast<ActionContainer*> (obj);
    }

    existed = false;
    MenuContainer* new_container = new MenuContainer(id,this);
    if (new_container) {
        new_container->setObjectName(id);
        d->observer_action_container << new_container;
        return new_container;
    } else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::menu(const QString &id) {
    if (d->observer_action_container.containsSubjectWithName(id)) {
        QObject* obj = d->observer_action_container.subjectReference(id);
        return qobject_cast<ActionContainer*> (obj);
    } else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::createMenuBar(const QString &id, bool& existed) {
    if (d->observer_action_container.containsSubjectWithName(id)) {
        existed = true;
        QObject* obj = d->observer_action_container.subjectReference(id);
        return qobject_cast<ActionContainer*> (obj);
    }

    existed = false;

    MenuBarContainer* new_container = new MenuBarContainer(this);

    if (new_container) {
        new_container->setObjectName(id);
        d->observer_action_container << new_container;
        return new_container;
    } else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::menuBar(const QString &id) {
    if (d->observer_action_container.containsSubjectWithName(id)) {
        QObject* obj = d->observer_action_container.subjectReference(id);
        return qobject_cast<ActionContainer*> (obj);
    } else
        return 0;
}

Qtilities::CoreGui::ActionContainer *Qtilities::CoreGui::ActionManager::actionContainer(const QString &id) const {
    QObject* obj = d->observer_action_container.subjectReference(id);
    return qobject_cast<ActionContainer*> (obj);
}

Qtilities::CoreGui::Command *Qtilities::CoreGui::ActionManager::registerAction(const QString &id,
                                                                               QAction *action,
                                                                               const QList<int> &context) {
    if (!action)
        return 0;

    // Check if there is already a front end action for this action id:
    Command* command = 0;
    if (d->observer_commands.containsSubjectWithName(id)) {
        QObject* obj = d->observer_commands.subjectReference(id);
        command = qobject_cast<Command*> (obj);
    } else
        command = registerActionPlaceHolder(id,action->text());

    if (command) {
//        // Check if we need to assign the text from the original action place holder creation call:
//        if (action->text().isEmpty())
//            action->setText(command->text());
        ProxyAction* multi = qobject_cast<ProxyAction*> (command);
        if (multi) {           
            multi->setObjectName(id);

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

            // Add it down here in order to avoid unneccesary updates when changing action in the above code:
            multi->addAction(action,context);

            //emit numberOfCommandsChanged();
            return multi;
        }
    }

    return 0;
}

Qtilities::CoreGui::Command* Qtilities::CoreGui::ActionManager::registerActionPlaceHolder(const QString &id,
                                                                                          const QString& user_text,
                                                                                          const QKeySequence& key_sequence,
                                                                                          const QList<int> &context,
                                                                                          const QIcon &icon) {
    // First check if an action with the specified id already exist:
    if (d->observer_commands.containsSubjectWithName(id)) {
        LOG_ERROR(tr("Attempting to register action place holder for a command which already exist with ID: ") + id);
        return command(id);
    }

    // We create a default action. The user can connect to the trigger produced by Command->action() or add backend actions at a later stage:
    QAction* frontend_action;
    if (user_text.isEmpty())
        frontend_action = new QAction(id.split(".").last(),0);
    else
        frontend_action = new QAction(user_text,0);

    frontend_action->setIcon(icon);
    frontend_action->setObjectName(id);
    frontend_action->setShortcutContext(Qt::ApplicationShortcut);

    if (!QtilitiesApplication::mainWindow()) {
        #ifndef QT_NO_DEBUG
        // Show a message box since action manager will always be used in QtGui application.
        if (!showed_warning) {
            QString warning_msg = QString("QtilitiesApplication::mainWindow() is required when registering actions in the action manager.<br><br>Proxy actions will not work as intended.");
            qWarning() << warning_msg;
            LOG_WARNING(warning_msg);
            showed_warning = true;
        }
        #else
            LOG_DEBUG("QtilitiesApplication::mainWindow() is required when registering actions in the action manager.<br><br>Proxy actions will not work as intended.");
        #endif
    } else
        QtilitiesApplication::mainWindow()->addAction(frontend_action);

    ProxyAction* new_action = new ProxyAction(frontend_action,d->observer_commands.observerID());
    if (new_action) {
        new_action->setObjectName(id);
        new_action->setDefaultText(id);
        if (!context.isEmpty()) {
            // We create a holder backend action, this action will be internal and the user should user the Command->action() call to use the command.
            QAction* holder_backend_action = new QAction(frontend_action->text(),0);
            new_action->addAction(holder_backend_action,context);
        }
        new_action->setCurrentContext(CONTEXT_MANAGER->activeContexts());
        d->observer_commands << new_action;

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

Qtilities::CoreGui::Command *Qtilities::CoreGui::ActionManager::registerShortcut(const QString &id,
                                                                                 const QString& user_text,
                                                                                 QShortcut *shortcut,
                                                                                 const QList<int> &active_contexts) {
    if (!shortcut)
        return 0;

    // First check if a command with the specified id already exist:
    if (d->observer_commands.containsSubjectWithName(id)) {
        LOG_ERROR(tr("Attempting to register a shortcut for a command which already exist with ID: ") + id);
        return command(id);
    }

    QList<int> contexts = active_contexts;
    if (contexts.isEmpty()) {
        // We associate it with the standard context:
        contexts << CONTEXT_MANAGER->contextID(Qtilities::Core::Constants::qti_def_CONTEXT_STANDARD);
    }

    // Create new shortcut:
    ShortcutCommand* new_shortcut = new ShortcutCommand(user_text,shortcut,contexts,d->observer_commands.observerID(),QtilitiesApplication::mainWindow());
    if (new_shortcut) {
        new_shortcut->setObjectName(id);
        new_shortcut->setDefaultText(id);
        new_shortcut->setCurrentContext(CONTEXT_MANAGER->activeContexts());
        d->observer_commands << new_shortcut;

        new_shortcut->setDefaultKeySequence(shortcut->key());
        new_shortcut->setKeySequence(shortcut->key());
        emit numberOfCommandsChanged();
        return new_shortcut;
    }

    return 0;
}

void ActionManager::unregisterCommandsForContext(int context) {
    if (d->observer_commands.subjectCount() == 0)
        return;

    // Loop through all commands and remove this context on them:
    Command* command = qobject_cast<Command*> (d->observer_commands.subjectAt(0));
    SubjectIterator<Qtilities::CoreGui::Command> command_itr(command,&d->observer_commands);

    if (command_itr.current())
        command_itr.current()->unregisterContext(context);

    while (command_itr.hasNext())
        command_itr.next()->unregisterContext(context);
}

Qtilities::CoreGui::Command *Qtilities::CoreGui::ActionManager::command(const QString &id) const {
    if (d->observer_commands.containsSubjectWithName(id)) {
        QObject* obj = d->observer_commands.subjectReference(id);
        return qobject_cast<Command*> (obj);
    } else
        return 0;
}

void Qtilities::CoreGui::ActionManager::handleContextChanged(QList<int> new_contexts) {
    if (d->observer_commands.subjectCount() == 0)
        return;

    Command* command = qobject_cast<Command*> (d->observer_commands.subjectAt(0));
    SubjectIterator<Qtilities::CoreGui::Command> command_itr(command,
                                                             &d->observer_commands);

    if (command_itr.current())
        command_itr.current()->setCurrentContext(new_contexts);

    while (command_itr.hasNext()) {
        command_itr.next()->setCurrentContext(new_contexts);
    }
}

void Qtilities::CoreGui::ActionManager::restoreDefaultShortcuts() {
    if (d->observer_commands.subjectCount() == 0)
        return;

    Command* command = qobject_cast<Command*> (d->observer_commands.subjectAt(0));
    SubjectIterator<Qtilities::CoreGui::Command> command_itr(command,
                                                             &d->observer_commands);

    if (command_itr.current())
        command_itr.current()->setKeySequence(command_itr.current()->defaultKeySequence());

    while (command_itr.hasNext()) {
        Command* cmd = command_itr.next();
        cmd->setKeySequence(cmd->defaultKeySequence());
    }
}

bool Qtilities::CoreGui::ActionManager::saveShortcutMapping(const QString& file_name, Qtilities::ExportVersion version) {
    QDomDocument doc("QtilitiesShortcutsMapping");
    QDomElement root = doc.createElement("QtilitiesShortcutsMapping");
    doc.appendChild(root);

    // Export version:
    root.setAttribute("ExportVersion",QString::number(version));
    root.setAttribute("QtilitiesVersion",CoreGui::QtilitiesApplication::qtilitiesVersionString());

    // All shortcuts:
    QDomElement shortcuts = doc.createElement("Shortcuts");
    root.appendChild(shortcuts);

    Command* command = qobject_cast<Command*> (d->observer_commands.subjectAt(0));
    SubjectIterator<Qtilities::CoreGui::Command> command_itr(command,
                                                             &d->observer_commands);

    if (command_itr.current()) {
        QDomElement tag = doc.createElement("Shortcut_0");
        tag.setAttribute("CommandID",command_itr.current()->defaultText());
        tag.setAttribute("KeySequence",command_itr.current()->keySequence().toString());
        shortcuts.appendChild(tag);
    }

    int count = 1;
    while (command_itr.next()) {
        ++count;
        QDomElement tag = doc.createElement("Shortcut_" + QString::number(count));
        tag.setAttribute("CommandID",command_itr.current()->defaultText());
        tag.setAttribute("KeySequence",command_itr.current()->keySequence().toString());
        shortcuts.appendChild(tag);
    }

    QFile file(file_name);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QString docStr = doc.toString(2);
        docStr.prepend("<!--Created by " + QApplication::applicationName() + " v" + QApplication::applicationVersion() + " on " + QDateTime::currentDateTime().toString() + "-->\n");
        docStr.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        file.write(docStr.toUtf8());
        file.close();
        LOG_INFO("Successfully exported shortcut mapping for this session to: " + file_name);
        return true;
    } else {
        LOG_ERROR("Failed to save shortcut mapping for this session to: " + file_name + ". The file could not be opened in WriteOnly mode.");
        return false;
    }
}

bool Qtilities::CoreGui::ActionManager::loadShortcutMapping(const QString& file_name) {
    QDomDocument doc("QtilitiesShortcutsMapping");
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

    QDomElement root = doc.documentElement();

    // ---------------------------------------------------
    // Inspect file format:
    // ---------------------------------------------------
    bool has_export_version = false;
    Qtilities::ExportVersion read_version;
    if (root.hasAttribute("ExportVersion")) {
        read_version = (Qtilities::ExportVersion) root.attribute("ExportVersion").toInt();
        has_export_version = true;
    }
    if (!has_export_version) {
        LOG_ERROR(QString(tr("The shortcut file is not in a recognizable format. Import will fail.")));
        return false;
    }

    // ---------------------------------------------------
    // Check if input format is supported:
    // ---------------------------------------------------
    bool is_supported_format = false;
    if (!(read_version < Qtilities::Qtilities_1_0 || read_version > Qtilities::Qtilities_Latest))
        is_supported_format = true;

    if (!is_supported_format) {
        LOG_ERROR(QString(tr("Unsupported shortcuts file found with export version: %1. The file will not be parsed.")).arg(read_version));
        return false;
    }

    // ---------------------------------------------------
    // Do the actual import:
    // ---------------------------------------------------
    QDomNodeList childNodes = root.childNodes();
    for(int i = 0; i < childNodes.count(); ++i)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == QLatin1String("Shortcuts")) {
            QDomNodeList shortcutNodes = child.childNodes();
            for(int i = 0; i < shortcutNodes.count(); ++i)
            {
                QDomNode shortcutNode = shortcutNodes.item(i);
                QDomElement shortcut = shortcutNode.toElement();

                if (shortcut.isNull())
                    continue;

                if (shortcut.tagName().startsWith("Shortcut_")) {
                    QString commandID = shortcut.attribute("CommandID");
                    QString key_sequence = shortcut.attribute("KeySequence");
                    if (d->observer_commands.containsSubjectWithName(commandID)) {
                        Command* command = qobject_cast<Command*> (d->observer_commands.subjectReference(commandID));
                        if (command) {
                            command->setKeySequence(QKeySequence(key_sequence));
                            #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
                            LOG_TRACE("Importing shortcut for action: " + commandID + ", Shortcut: " + key_sequence);
                            #endif
                        }

                    } else {
                        LOG_WARNING(QString(tr("Unknown command found in the input shortcut mapping file: %1")).arg(commandID));
                    }
                    continue;
                }
            }
            continue;
        }
    }

    LOG_INFO("Successfully imported shortcut mapping for this session from: " + file_name);
    return true;
}

QWidget* Qtilities::CoreGui::ActionManager::commandEditor() {
    if (!d->command_editor) {
        d->command_editor = new CommandEditor(0);
    }

    return d->command_editor;
}

QList<Command*> Qtilities::CoreGui::ActionManager::commandsWithKeySequence(QKeySequence key_sequence) {
    QList<Command*> commands;

    if (d->observer_commands.subjectCount() == 0)
        return commands;

    Command* command = qobject_cast<Command*> (d->observer_commands.subjectAt(0));
    SubjectIterator<Qtilities::CoreGui::Command> command_itr(command,&d->observer_commands);

    if (command_itr.current()) {
        foreach (const QString& search_string, key_sequence.toString().split(",")) {
            if (command_itr.current()->keySequence().toString().split(",").contains(search_string)) {
                commands << command_itr.current();
                break;
            }
        }
    }

    while (command_itr.next()) {
        foreach (const QString& search_string, key_sequence.toString().split(",")) {
            if (command_itr.current()->keySequence().toString().split(",").contains(search_string)) {
                commands << command_itr.current();
                break;
            }
        }
    }

    return commands;
}

Qtilities::Core::Observer* Qtilities::CoreGui::ActionManager::commandObserver() {
    return &d->observer_commands;
}

Qtilities::Core::Observer* Qtilities::CoreGui::ActionManager::actionContainerObserver() {
    return &d->observer_action_container;
}
