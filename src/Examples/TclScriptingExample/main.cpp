/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
#include <QMessageBox>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include "TclScriptingMode.h"
using namespace Qtilities::Examples::TclScripting;

#include "qtclconsole.h"
#include "tclnotify.h"
#include "commandsManager.h"
#include "commands.h"

extern void Qtk_InitNotifier( QApplication * );

int main(int argc, char ** argv) {
    //init tcl
    Tcl_FindExecutable(argv[0]);
    Tcl_Interp * interp = Tcl_CreateInterp();
    Tcl_Init( interp );

    QApplication a( argc, argv );

    Tcl_SetServiceMode (TCL_SERVICE_ALL);
    Qtk_InitNotifier( &a );

    // Now do normal app stuff:
    QtilitiesApplication::initialize();
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Tcl Scripting Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

    // Create a QtilitiesMainWindow to show our different modes:
    QtilitiesMainWindow* exampleMainWindow = new QtilitiesMainWindow(QtilitiesMainWindow::ModesTop);
    QtilitiesApplication::setMainWindow(exampleMainWindow);

    // Create the configuration widget:
    ConfigurationWidget config_widget;
    QtilitiesApplication::setConfigWidget(&config_widget);

    // Initialize the logger:
    Log->setLoggerSessionConfigPath(QtilitiesApplication::applicationSessionPath());
    Log->setLoggerSettingsEnabled(false);
    LOG_INITIALIZE();

    commandsManager::getInstance(interp)->registerFunction("msgbox" , (commandsManager::commandType) CallQMessageBox, "Shows the Qt message box");

    // Make the TCL scripting mode:
    TclScriptingMode* tcl_scripting_mode = new TclScriptingMode;
    //QMainWindow* tcl_scripting_mode = new QMainWindow;

    //Instantiate and set the focus to the QtclConsole
    QtclConsole *console = QtclConsole::getInstance((QMainWindow*)tcl_scripting_mode,"Tcl Console");
    tcl_scripting_mode->setFocusProxy((QWidget*)console);
    tcl_scripting_mode->dock()->setFocusProxy((QWidget*)console);
    tcl_scripting_mode->setCentralWidget((QWidget*)console);
    tcl_scripting_mode->show();

    //OBJECT_MANAGER->registerObject(tcl_scripting_mode);
    OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget(false));

    // Create menu related things.
    bool existed;
    ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(qti_action_MENUBAR_STANDARD,existed);
    exampleMainWindow->setMenuBar(menu_bar->menuBar());
    ActionContainer* file_menu = ACTION_MANAGER->createMenu(qti_action_FILE,existed);
    menu_bar->addMenu(file_menu);

    // Get the standard context.
    QList<int> std_context;
    std_context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

    // Register action place holders for this application. This allows control of your menu structure.
    // File Menu
    Command* command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_FILE_SETTINGS,QObject::tr("Settings"),QKeySequence(),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),&config_widget,SLOT(show()));
    file_menu->addAction(command);
    file_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_FILE_EXIT,QObject::tr("Exit"),QKeySequence(QKeySequence::Close),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QCoreApplication::instance(),SLOT(quit()));
    file_menu->addAction(command);

    config_widget.initialize();
//    exampleMainWindow->modeManager()->initialize();
//    exampleMainWindow->readSettings();
//    exampleMainWindow->show();

    int result = a.exec();
    exampleMainWindow->writeSettings();

    LOG_FINALIZE();
    return result;
}
