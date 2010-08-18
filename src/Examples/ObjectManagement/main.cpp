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

#include <QtGui>

#include <QtilitiesExtensionSystem>

#include "ObjectManagementMode.h"

using namespace QtilitiesCore;
using namespace QtilitiesCoreGui;
using namespace QtilitiesExtensionSystem;
using namespace Qtilities::Examples::ObjectManagement;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Object Management Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());

    // Initialize the logger.
    LOG_INITIALIZE(false);
    Log->setIsQtMessageHandler(false);

    // Create the menu bar and menus in the menu bar:
    bool existed;
    ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(MENUBAR_STANDARD,existed);
    ActionContainer* file_menu = ACTION_MANAGER->createMenu(MENU_FILE,existed);
    ActionContainer* edit_menu = ACTION_MANAGER->createMenu(MENU_EDIT,existed);
    ActionContainer* about_menu = ACTION_MANAGER->createMenu(MENU_ABOUT,existed);
    menu_bar->addMenu(file_menu);
    menu_bar->addMenu(edit_menu);
    menu_bar->addMenu(about_menu);

    // Get the standard context:
    QList<int> std_context;
    std_context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_STANDARD));

    // Register action place holders for this application. This allows control of your menu structure:
    // File Menu
    Command* command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_SAVE,QObject::tr("Save"),QKeySequence(QKeySequence::Save));
    file_menu->addAction(command);
    file_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_PRINT,QObject::tr("Print"),QKeySequence(QKeySequence::Print));
    file_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_PRINT_PREVIEW,QObject::tr("Print Preview"),QKeySequence());
    file_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_PRINT_PDF,QObject::tr("Print PDF"),QKeySequence());
    file_menu->addAction(command);
    file_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_SETTINGS,QObject::tr("Settings"),QKeySequence(),std_context);
    // Create the configuration widget here and then connect it to the above command
    ConfigurationWidget config_widget;
    QObject::connect(command->action(),SIGNAL(triggered()),&config_widget,SLOT(show()));
    file_menu->addAction(command);
    file_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_EXIT,QObject::tr("Exit"),QKeySequence(QKeySequence::Close),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QCoreApplication::instance(),SLOT(quit()));
    file_menu->addAction(command);

    // Edit Menu
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_UNDO,QObject::tr("Undo"),QKeySequence(QKeySequence::Undo));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_REDO,QObject::tr("Redo"),QKeySequence(QKeySequence::Redo));
    edit_menu->addAction(command);
    edit_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_COPY,QObject::tr("Copy"),QKeySequence(QKeySequence::Copy));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_CUT,QObject::tr("Cut"),QKeySequence(QKeySequence::Cut));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_PASTE,QObject::tr("Paste"),QKeySequence(QKeySequence::Paste));
    edit_menu->addAction(command);
    edit_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_SELECT_ALL,QObject::tr("Select All"),QKeySequence(QKeySequence::SelectAll));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_CLEAR,QObject::tr("Clear"));
    edit_menu->addAction(command);
    edit_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_FIND,QObject::tr("Find"),QKeySequence(QKeySequence::Find));
    edit_menu->addAction(command);

    // About Menu
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_ABOUT_QTILITIES,QObject::tr("About Qtilities"),QKeySequence(),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QtilitiesApplication::instance(),SLOT(aboutQtilities()));
    about_menu->addAction(command);

    // We want to use paste operations in this application, thus initialize the clipboard.
    // It is important to do this after registering the copy, cut and paste action holders above.
    // The initialization will register backends for these actions.
    CLIPBOARD_MANAGER->initialize();

    // Create a QtilitiesMainWindow to show our different modes.
    QtilitiesMainWindow exampleMainWindow(0);
    QtilitiesApplication::instance()->setMainWindow(&exampleMainWindow);
    exampleMainWindow.setMenuBar(menu_bar->menuBar());

    // Create an instance of the example object management mode:
    ObjectManagementMode* object_management_mode = new ObjectManagementMode();
    if (exampleMainWindow.addMode(object_management_mode))
       CONTEXT_MANAGER->registerContext(object_management_mode->contextString());

    // Load plugins using the extension system:
    Log->toggleQtMsgEngine(true);
    ExtensionSystemCore::instance()->loadPlugins();
    Log->toggleQtMsgEngine(false);

    // Now that all the modes have been loaded from the plugins, add them to the main window:
    QList<QObject*> registered_modes = OBJECT_MANAGER->registeredInterfaces("IMode");
    LOG_INFO(QString("%1 application mode(s) found in set of loaded plugins.").arg(registered_modes.count()));
    exampleMainWindow.addModes(registered_modes);

    // Register command editor config page.
    OBJECT_MANAGER->registerObject(ACTION_MANAGER->commandEditor());
    // Register extension system config page.
    OBJECT_MANAGER->registerObject(ExtensionSystemCore::instance()->configWidget());

    // Report on the number of config pages found.
    QList<QObject*> registered_config_pages = OBJECT_MANAGER->registeredInterfaces("IConfigPage");
    LOG_INFO(QString("%1 configuration page(s) found in set of loaded plugins.").arg(registered_config_pages.count()));
    config_widget.initialize(registered_config_pages);

    // Load the previous session's keyboard mapping file.
    QString shortcut_mapping_file = QString("%1/session/%2").arg(QApplication::applicationDirPath()).arg(FILE_SHORTCUT_MAPPING);
    if (ACTION_MANAGER->importShortcutMapping(shortcut_mapping_file))
        LOG_INFO(QObject::tr("Succesfully loaded shortcut mapping from previous session. Path: ") + shortcut_mapping_file);
    else
        LOG_WARNING(QObject::tr("Failed to loaded shortcut mapping from previous session. The default mapping scheme will be used. Path: ") + shortcut_mapping_file);

    // Show the main window:
    exampleMainWindow.show();

    int result = a.exec();
    exampleMainWindow.writeSettings();

    // Save the current keyboard mapping for the next session.
    if (ACTION_MANAGER->exportShortcutMapping(shortcut_mapping_file))
        LOG_INFO(QObject::tr("Succesfully saved shortcut mapping for next session. Path: ") + shortcut_mapping_file);
    else
        LOG_WARNING(QObject::tr("Failed to save shortcut mapping for next session. Path: ") + shortcut_mapping_file);

    LOG_FINALIZE();
    return result;
}
