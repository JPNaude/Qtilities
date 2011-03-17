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

#ifndef IACTIONMANAGER_H
#define IACTIONMANAGER_H

#include <ObserverData>

#include "QtilitiesCoreGui_global.h"
#include "ActionContainer.h"
#include "Command.h"

#include <QList>
#include <QMainWindow>

class QString;
class QAction;
class QShortcut;

namespace Qtilities {
    namespace Core {
        class Observer;
    }
    namespace CoreGui {
        //! Namespace containing available interfaces which forms part of the CoreGui Module.
        namespace Interfaces {
            /*!
            \class IActionManager
            \brief Interface used to communicate with the action manager.

            For more information see the \ref page_action_management article.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IActionManager : public QObject
            {
                Q_OBJECT

            public:
                IActionManager(QObject* parent = 0) : QObject(parent) {}
                virtual ~IActionManager() {}

                //! Creates a menu with the given string ID.
                /*!
                  \param id The string id which should be used to identify the menu.
                  \param existed If a menu with the same name already existed this parameter will be set to true, false otherwise.
                  \returns The menu (newly created or old) which is represented by the string id.
                  */
                virtual ActionContainer *createMenu(const QString &id, bool& existed) = 0;
                //! Function to access a menu.
                /*!
                  \param id The string id of the menu requested.
                  \returns The menu which is represented by the string id. Null if no menu with the given id exists.
                  */
                virtual ActionContainer *menu(const QString &id) = 0;
                //! Creates a menu bar with the given string ID.
                /*!
                  \param id The string id which should be used to identify the menu bar.
                  \param existed If a menu bar with the same name already existed this parameter will be set to true, false otherwise.
                  \returns The menu bar (newly created or old) which is represented by the string id.
                  */
                virtual ActionContainer *createMenuBar(const QString &id, bool& existed) = 0;
                //! Function to access a menu bar.
                /*!
                  \param id The string id of the menu bar requested.
                  \returns The menu bar which is represented by the string id. Null if no menu bar with the given id exists.
                  */
                virtual ActionContainer *menuBar(const QString &id) = 0;
                //! Registers an action with the action manager.
                /*!
                  This function can be used to register an action with the action manager. If an action placeholder already exists with the given id, this action will be added as a backend for the current proxy action under the given contexts.

                  \param id The internal id used to represent the action. This is the name used to display the action in the action manager.
                  \param action The backend action associated with the id & context pairing.
                  \param context The context for which this action should be registered. The action manager is linked with the context manager and when the context changes, all the actions will be updated for the new context. When an action does not have a backend action specified for a given context, it will be disabled. Leaving the context the default QList<int>() will register it in the standard application context (always active).
                  \return The command created for the given id.

                  \note This function will set the shortcut of the backend action to QKeySequence() to avoid ambigious action triggers when the frontend action and the backend action are both active. If the frontend action does not have an shortcut associated with it yet, it will inherit the shortcut of \p action. If the frontend already contains an action which is different than the shortcut provided by \p action, an error message will be printed and the original shortcut will be used.

                  \note The user_text of the command will be taken from the action's text() function.
                  */
                virtual Command *registerAction(const QString &id, QAction *action, const QList<int> &context = QList<int>()) = 0;
                //! Registers an action placeholder with the action manager.
                /*!
                  This function is used to create a placeholder for future action types in the system. This is usefull in cases where the actual backend action is not registered at application startup. An example of this would be actions contained in an object produced by a factory. For these scenarios you must register an place holder for the action at startup (to allow the action to be visible in the command editor, and to allow the action to be set according to a previous shortcut mapping scheme at application startup. If you want to use the registerActionPlaceHolder function in this way, leave the context as default. This will create the action place holder and you can register backend actions at a later time using the registerAction() call.

                  You can also use this function if you want to create a command which is not a multi-backed a command. In that case you connect directly to the command->action() triggered() signal to access the action. If you want to use the registerActionPlaceHolder function in this way you must provide an context for the action.

                  \param id The internal id used to represent the action. This is the name used to display the action in the command editor.
                  \param user_text The user visible text that will be used for this command.
                  \param shortcut A shortcut to be associated with the command.
                  \param context Pass an empty QList<int>() if you do want to use the action as an mutli backed action, pass the needed context otherwise.
                  \return The command created for the given id.
                  */
                virtual Command *registerActionPlaceHolder(const QString &id, const QString& user_text, const QKeySequence& key_sequence = QKeySequence(), const QList<int> &context = QList<int>()) = 0;
                //! Function to register a shortcut in the action manager.
                /*!
                  \param id The internal id used to represent the shortcut. This is the name used to display the shortcut in the command editor.
                  \param user_text The user visible text that will be used for this command.
                  \param shortcut A reference to the shortcut to be registered. If this reference is invalid, this function does nothing.
                  \param active_contexts Pass an empty QList<int>() if you do want to use the shortcut to be always active.
                  \return The command created for the given id.
                  */
                virtual Command *registerShortcut(const QString &id, const QString& user_text, QShortcut *shortcut, const QList<int> &active_contexts = QList<int>()) = 0;
                //! Access function for actions and shortcuts.
                virtual Command *command(const QString &id) const = 0;
                //! Access function for menus and menu bars.
                virtual ActionContainer *actionContainer(const QString &id) const = 0;

                //! Restores the default shortcut configuration.
                virtual void restoreDefaultShortcuts() = 0;
                //! Exports the current shortcut configuraiton.
                virtual bool saveShortcutMapping(const QString& file_name, Qtilities::ExportVersion version = Qtilities::Qtilities_Latest) = 0;
                //! Imports a previously exported shortcut configuration.
                virtual bool loadShortcutMapping(const QString& file_name) = 0;
                //! Creates the command editor/shortcut configuration page.
                /*!
                    The command editor widget is returned by the function call.

                    \sa Qtilities::CoreGui::CommandEditor
                    */
                virtual QWidget* commandEditor() = 0;
                //! Returns commands that uses a specific shortcut mapping.
                virtual QList<Command*> commandsWithKeySequence(QKeySequence key_sequence) = 0;
                //! Returns the observer managing commands.
                virtual Core::Observer* commandObserver() = 0;
                //! Returns the observer managing action containers.
                virtual Core::Observer* actionContainerObserver() = 0;

            public slots:
                virtual void handleContextChanged(QList<int> new_contexts) = 0;

            signals:
                //! Signal which is emitted when the number of actions in the action manager changes.
                void numberOfCommandsChanged();
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IActionManager,"com.Qtilities.ObjManagement.IActionManager/1.0")

#endif // IACTIONMANAGER_H
