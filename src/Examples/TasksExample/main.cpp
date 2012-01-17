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

#include <QApplication>
#include <QtGui>

#include <QtilitiesExtensionSystem>
#include <QtilitiesProjectManagement>

#include "ExampleMode.h"

using namespace QtilitiesExtensionSystem;
using namespace QtilitiesProjectManagement;
using namespace Qtilities::Examples::TasksExample;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Tasks Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

    // Create a QtilitiesMainWindow to show our different modes:
    QtilitiesMainWindow exampleMainWindow(QtilitiesMainWindow::ModesTop);
    exampleMainWindow.showTaskSummaryWidget();
    QtilitiesApplication::setMainWindow(&exampleMainWindow);

    // Create the configuration widget:
    ConfigurationWidget config_widget;
    QtilitiesApplication::setConfigWidget(&config_widget);

    // Initialize the logger:
    QtilitiesApplication::applicationSessionPath();
    LOG_INITIALIZE();
    Log->setIsQtMessageHandler(false);

    // Speed up application launching a bit...
    ACTION_MANAGER->commandObserver()->startProcessingCycle();
    ACTION_MANAGER->actionContainerObserver()->startProcessingCycle();
    OBJECT_MANAGER->objectPool()->startProcessingCycle();

    // Initialize the clipboard manager:
    CLIPBOARD_MANAGER->initialize();

    // Create menu related things.
    bool existed;
    ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(qti_action_MENUBAR_STANDARD,existed);
    exampleMainWindow.setMenuBar(menu_bar->menuBar());
    ActionContainer* file_menu = ACTION_MANAGER->createMenu(qti_action_FILE,existed);
    ActionContainer* about_menu = ACTION_MANAGER->createMenu(qti_action_ABOUT,existed);
    menu_bar->addMenu(file_menu);
    menu_bar->addMenu(about_menu);

    // Get the standard context.
    QList<int> std_context;
    std_context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

    // Register action place holders for this application. This allows control of your menu structure.
    // File Menu
    std_context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));
    Command* command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_FILE_SETTINGS,QObject::tr("Settings"),QKeySequence(),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),&config_widget,SLOT(show()));
    file_menu->addAction(command);
    file_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_FILE_EXIT,QObject::tr("Exit"),QKeySequence(QKeySequence::Close),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QCoreApplication::instance(),SLOT(quit()));
    file_menu->addAction(command);
    // About Menu
    command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_ABOUT_QTILITIES,QObject::tr("About Qtilities"),QKeySequence(),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QtilitiesApplication::instance(),SLOT(aboutQtilities()));
    about_menu->addAction(command);

    // Create the Example before plugin loading since it registers a project items:
    ExampleMode* example_mode = new ExampleMode;

    // Load plugins using the extension system:
    Log->toggleQtMsgEngine(true);
    EXTENSION_SYSTEM->enablePluginActivityControl();
    EXTENSION_SYSTEM->setFilteredPlugins(QStringList("*Project*"));
    EXTENSION_SYSTEM->addPluginPath("../../plugins/");
    EXTENSION_SYSTEM->initialize();
    Log->toggleQtMsgEngine(false);

    // Now that all the modes have been loaded from the plugins, add them to the main window:
    QList<QObject*> registered_modes = OBJECT_MANAGER->registeredInterfaces("IMode");
    registered_modes << example_mode;
    LOG_INFO(QString("%1 application mode(s) found in set of loaded plugins.").arg(registered_modes.count()));
    exampleMainWindow.modeManager()->addModes(registered_modes);

    // Register command editor config page.
    OBJECT_MANAGER->registerObject(ACTION_MANAGER->commandEditor(),QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));
    // Register extension system config page.
    OBJECT_MANAGER->registerObject(EXTENSION_SYSTEM->configWidget(),QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));

    // Report on the number of config pages found.
    QList<QObject*> registered_config_pages = OBJECT_MANAGER->registeredInterfaces("IConfigPage");
    LOG_INFO(QString("%1 configuration page(s) found in set of loaded plugins.").arg(registered_config_pages.count()));
    config_widget.initialize(registered_config_pages);

    // Load the previous session's keyboard mapping file.
    QString shortcut_mapping_file = QString("%1/%2").arg(QtilitiesApplication::applicationSessionPath()).arg(qti_def_PATH_SHORTCUTS_FILE);
    ACTION_MANAGER->loadShortcutMapping(shortcut_mapping_file);

    // Show the main window:
    exampleMainWindow.readSettings();
    exampleMainWindow.show();

    ACTION_MANAGER->commandObserver()->endProcessingCycle(false);
    ACTION_MANAGER->actionContainerObserver()->endProcessingCycle(false);
    OBJECT_MANAGER->objectPool()->endProcessingCycle(false);

    int result = a.exec();
    exampleMainWindow.writeSettings();

    // Save the current keyboard mapping for the next session.
    ACTION_MANAGER->saveShortcutMapping(shortcut_mapping_file);

    LOG_FINALIZE();
    return result;
}
