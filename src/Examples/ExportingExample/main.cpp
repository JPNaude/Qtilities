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

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include "VersionDetails.h"
using namespace Qtilities::Examples::ExportingExample;

int main(int argc, char *argv[])
{
//    QtilitiesApplication a(argc, argv);
//    QtilitiesApplication::setOrganizationName("YourOrganization");
//    QtilitiesApplication::setOrganizationDomain("YourDomain");
//    QtilitiesApplication::setApplicationName("My Application");
//    QtilitiesApplication::setApplicationVersion("1.0");

//    // Set the application export version:
//    QtilitiesApplication::setApplicationExportVersion(0);

//    // Register our VersionDetails class in the Qtilities factory:
//    FactoryItemID version_info_id("Version Details");
//    OBJECT_MANAGER->registerFactoryInterface(&VersionDetails::factory,version_info_id);

//    // Next create a TreeNode with a couple of our classes attached to it:
//    TreeNode* node = new TreeNode("TestNode");
//    VersionDetails* ver1 = new VersionDetails;
//    ver1->setDescriptionBrief("Version 1 Brief");
//    ver1->setDescriptionDetailed("Version 1 Brief");
//    ver1->setVersionMajor(0);
//    ver1->setVersionMinor(0);
//    VersionDetails* ver2 = new VersionDetails;
//    ver2->setDescriptionBrief("Version 2 Brief");
//    ver2->setDescriptionDetailed("Version 2 Brief");
//    ver2->setVersionMajor(1);
//    ver2->setVersionMinor(2);
//    node->attachSubject(ver1);
//    node->attachSubject(ver2);
//    node->addNode("NewNode");

//    // Next export the node to a file:
//    node->saveToFile("Output_Version_0.xml");
//    node->setApplicationExportVersion(1);
//    node->saveToFile("Output_Version_1.xml");

//    ObserverWidget* view = new ObserverWidget(node);
//    view->show();
//    return a.exec();

    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Simple Example");
    QtilitiesApplication::setApplicationVersion("1.0");

    // Create a main window for our application:
    QMainWindow* main_window = new QMainWindow;
    QtilitiesApplication::setMainWindow(main_window);

    // Create a settings window for our application:
    ConfigurationWidget* config_widget = new ConfigurationWidget;
    QtilitiesApplication::setConfigWidget(config_widget);

    // Initialize the logger:
    LOG_INITIALIZE();

    // Add a menu bar to our main window with a File menu:
    bool existed;
    ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(qti_action_MENUBAR_STANDARD,existed);
    main_window->setMenuBar(menu_bar->menuBar());
    ActionContainer* file_menu = ACTION_MANAGER->createMenu(qti_action_FILE,existed);
    menu_bar->addMenu(file_menu);

    // Our menu items will need to be associated with a context.
    // A good idea is to use the standard context which is always active:
    QList<int> std_context;
    std_context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

    // Create File->Settings and File->Exit menu items:
    Command* command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_FILE_SETTINGS,QObject::tr("Settings"),QKeySequence(),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),config_widget,SLOT(show()));
    file_menu->addAction(command);
    file_menu->addSeperator();
    command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_FILE_EXIT,QObject::tr("Exit"),QKeySequence(QKeySequence::Close),std_context);
    QObject::connect(command->action(),SIGNAL(triggered()),QCoreApplication::instance(),SLOT(quit()));
    file_menu->addAction(command);

    // Lets add a couple of pages to our setting page which handles shortcuts and logging in our application:
    OBJECT_MANAGER->registerObject(ACTION_MANAGER->commandEditor());
    OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget());
    // Initializing the configuration widget will search the global object pool for objects implementing IConfigPage, and automatically add them:
    config_widget->initialize();

    // Now build an example tree which will tell all views to provide some actions for the tree items:
    TreeNode* node = new TreeNode("Root Node");
    node->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    node->displayHints()->setDisplayFlagsHint(ObserverHints::AllDisplayFlagHint);

    TreeNode* nodeA = node->addNode("Node A");
    nodeA->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    nodeA->displayHints()->setDisplayFlagsHint(ObserverHints::AllDisplayFlagHint);
    nodeA->addItem("Item 1");
    nodeA->addItem("Item 2");
    TreeItem* sharedItem = nodeA->addItem("Shared Item");

    TreeNode* nodeB = node->addNode("Node B");
    nodeB->displayHints()->setActionHints(ObserverHints::ActionPushUp | ObserverHints::ActionSwitchView);
    nodeB->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    nodeB->attachSubject(sharedItem);
    nodeB->addItem("Item 3");
    nodeB->addItem("Item 4");
    // Notice we added different display hints on Node B, it will behave different.

    // We show the tree using an ObserverWidget:
    ObserverWidget* tree_widget = new ObserverWidget(node);
    tree_widget->show();

    // Finally, set the ObserverWidget as the main window's central widget and show it:
    main_window->setCentralWidget(tree_widget);
    main_window->show();
    return a.exec();
}
