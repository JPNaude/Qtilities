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

#include "ExampleMode.h"

using namespace QtilitiesExtensionSystem;
using namespace Qtilities::Examples::MainWindow;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Main Window Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());

    // Create a QtilitiesMainWindow to show our different modes.
    QtilitiesMainWindow exampleMainWindow(QtilitiesMainWindow::ModesLeft);
    QtilitiesApplication::setMainWindow(&exampleMainWindow);

    // Initialize the logger.
    LOG_INITIALIZE();
    Log->setIsQtMessageHandler(false);

    AbstractLoggerEngine* engine = Log->newFileEngine("Test File Engine",QApplication::applicationDirPath() + "/test.log");
    if (engine)
        engine->setMessageContexts(Logger::SystemWideMessages | Logger::EngineSpecificMessages);

    // We show a splash screen in this example:
    QPixmap pixmap(QTILITIES_LOGO_BT_300x300);
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    splash->show();
    QObject::connect(EXTENSION_SYSTEM,SIGNAL(newProgressMessage(QString)),splash,SLOT(showMessage(QString)));
    a.processEvents();

    // Create menu related things.
    bool existed;
    ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(MENUBAR_STANDARD,existed);
    exampleMainWindow.setMenuBar(menu_bar->menuBar());
    ActionContainer* file_menu = ACTION_MANAGER->createMenu(MENU_FILE,existed);
    ActionContainer* edit_menu = ACTION_MANAGER->createMenu(MENU_EDIT,existed);
    ActionContainer* view_menu = ACTION_MANAGER->createMenu(MENU_VIEW,existed);
    ActionContainer* about_menu = ACTION_MANAGER->createMenu(MENU_ABOUT,existed);
    menu_bar->addMenu(file_menu);
    menu_bar->addMenu(edit_menu);
    menu_bar->addMenu(view_menu);
    menu_bar->addMenu(about_menu);

    // Get the standard context.
    QList<int> std_context;
    std_context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_STANDARD));

    // Register action place holders for this application. This allows control of your menu structure.
    // File Menu
    std_context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_STANDARD));
    Command* command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_SETTINGS,QObject::tr("Settings"),QKeySequence(),std_context);
    // Create the configuration widget here and then connect it to the above command
    ConfigurationWidget config_widget;
    QObject::connect(command->action(),SIGNAL(triggered()),&config_widget,SLOT(show()));
    file_menu->addAction(command);
    file_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_EXIT,QObject::tr("Exit"),QKeySequence(QKeySequence::Close),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QCoreApplication::instance(),SLOT(quit()));
    file_menu->addAction(command);
    // About Menu
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_ABOUT_QTILITIES,QObject::tr("About Qtilities"),QKeySequence(),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QtilitiesApplication::instance(),SLOT(aboutQtilities()));
    about_menu->addAction(command);

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

    // Load plugins using the extension system:
    Log->toggleQtMsgEngine(true);
    EXTENSION_SYSTEM->enablePluginActivityControl();
    EXTENSION_SYSTEM->addPluginPath("../../plugins/");
    EXTENSION_SYSTEM->initialize();
    Log->toggleQtMsgEngine(false);
    splash->clearMessage();

    // Create the example file system side widget and add it to the global object pool
    QList<int> modes;
    modes << MODE_EXAMPLE_ID;
    ExampleMode* example_mode = new ExampleMode();
    SideViewerWidgetFactory* file_system_side_widget_helper = new SideViewerWidgetFactory(&SideWidgetFileSystem::factory,"File System",modes,modes);
    OBJECT_MANAGER->registerObject(file_system_side_widget_helper,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));
    QObject::connect(file_system_side_widget_helper,SIGNAL(newWidgetCreated(QWidget*)),example_mode,SLOT(handleNewFileSystemWidget(QWidget*)));
    SideViewerWidgetFactory* object_scope_side_widget_helper = new SideViewerWidgetFactory(&ObjectScopeWidget::factory,"Object Scope",modes,modes);
    OBJECT_MANAGER->registerObject(object_scope_side_widget_helper,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));
    #ifndef QTILITIES_NO_PROPERTY_BROWSER
    SideViewerWidgetFactory* property_editor_side_widget_helper = new SideViewerWidgetFactory(&ObjectPropertyBrowser::factory,"Property Browser",modes,modes);
    OBJECT_MANAGER->registerObject(property_editor_side_widget_helper,QtilitiesCategory("GUI::Side Viewer Widgets (ISideViewerWidget)","::"));
    #endif

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

    // Report on the number of side widgets found.
    QList<QObject*> registered_side_widgets = OBJECT_MANAGER->registeredInterfaces("ISideViewerWidget");
    LOG_INFO(QString("%1 side viewer widget(s) found in set of loaded plugins.").arg(registered_side_widgets.count()));

    // Load the previous session's keyboard mapping file.
    QString shortcut_mapping_file = QString("%1/session/%2").arg(QApplication::applicationDirPath()).arg(FILE_SHORTCUT_MAPPING);
    if (ACTION_MANAGER->importShortcutMapping(shortcut_mapping_file))
        LOG_INFO(QObject::tr("Succesfully loaded shortcut mapping from previous session. Path: ") + shortcut_mapping_file);
    else
        LOG_WARNING(QObject::tr("Failed to load shortcut mapping from previous session. The default mapping scheme will be used. Path: ") + shortcut_mapping_file);

    // Show the main window:
    exampleMainWindow.show();
    splash->close();

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
