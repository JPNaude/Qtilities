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

#include <Qtilities>

#include "ObserverWidgetConfig.h"
#include "ExtendedObserverTableModel.h"
#include "ExtendedObserverTreeModel.h"

using namespace Qtilities;
using namespace Qtilities::Examples::Clipboard;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Clipboard Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

    QMainWindow* main_window = new QMainWindow;
    QtilitiesApplication::setMainWindow(main_window);
    ConfigurationWidget* config_widget = new ConfigurationWidget(Qtilities::TreeView);
    QtilitiesApplication::setConfigWidget(config_widget);

    Log->setLoggerSessionConfigPath(QtilitiesApplication::applicationSessionPath());
    LOG_INITIALIZE();
    Log->setGlobalLogLevel(Logger::Debug);

    // Create the menu bar and menus in the menu bar:
    bool existed;
    ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(qti_action_MENUBAR_STANDARD,existed);
    ActionContainer* edit_menu = ACTION_MANAGER->createMenu(qti_action_EDIT,existed);
    menu_bar->addMenu(edit_menu);

    // Get the standard context:
    QList<int> std_context;
    std_context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

    // Create the configuration widget here and then connect it to the settings action
    Command* command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_FILE_SETTINGS,QObject::tr("Settings"),QKeySequence(),std_context);

    QObject::connect(command->action(),SIGNAL(triggered()),config_widget,SLOT(show()));
    edit_menu->addAction(command);
    edit_menu->addSeperator();
    // Register action placeholders for the copy, cut and paste actions:
    command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_EDIT_COPY,QObject::tr("Copy"),QKeySequence(QKeySequence::Copy));
    command->setCategory(QtilitiesCategory("Editing"));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_EDIT_CUT,QObject::tr("Cut"),QKeySequence(QKeySequence::Cut));
    command->setCategory(QtilitiesCategory("Editing"));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(qti_action_EDIT_PASTE,QObject::tr("Paste"),QKeySequence(QKeySequence::Paste));
    command->setCategory(QtilitiesCategory("Editing"));
    edit_menu->addAction(command);

    // Important: The Log widget must be created before the above action place holders were added since it registers some actions
    QString engine_name = "Clipboard Example Log";
    QDockWidget* log_dock_widget = LoggerGui::createLogDockWidget(&engine_name);
    log_dock_widget->setObjectName("Session Log Dock Widget");
    log_dock_widget->show();

    // We want to use paste operations in this application, thus initialize the clipboard.
    // It is important to do this after registering the copy, cut and paste action holders above.
    // The initialization will register backends for these actions.
    CLIPBOARD_MANAGER->initialize();

    Observer* observerA = new Observer("Observer A","Top level observer");
    observerA->useDisplayHints();
    // Naming policy filter
    NamingPolicyFilter* naming_filter = new NamingPolicyFilter();
    naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    observerA->installSubjectFilter(naming_filter);
    observerA->displayHints()->setNamingControlHint(ObserverHints::EditableNames);
    // Activty policy filter
    ActivityPolicyFilter* activity_filter = new ActivityPolicyFilter();
    activity_filter->setActivityPolicy(ActivityPolicyFilter::MultipleActivity);
    activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::AllowNoneActive);
    observerA->installSubjectFilter(activity_filter);
    observerA->displayHints()->setActivityControlHint(ObserverHints::FollowSelection);
    observerA->displayHints()->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::ActionRemoveItem;
    action_hints |= ObserverHints::ActionDeleteItem;
    action_hints |= ObserverHints::ActionPushDown;
    action_hints |= ObserverHints::ActionPushUp;
    action_hints |= ObserverHints::ActionRefreshView;
    action_hints |= ObserverHints::ActionCopyItem;
    action_hints |= ObserverHints::ActionCutItem;
    action_hints |= ObserverHints::ActionPasteItem;
    action_hints |= ObserverHints::ActionSwitchView;
    action_hints |= ObserverHints::ActionFindItem;
    observerA->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    observerA->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    display_flags |= ObserverHints::NavigationBar;
    display_flags |= ObserverHints::ActionToolBar;
    observerA->displayHints()->setDisplayFlagsHint(display_flags);
    observerA->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);

    // Now use TreeNode for nodes B and C to show that is makes working with Observers much easier:
    TreeNode* observerB = new TreeNode("TreeNode B");
    observerB->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames);
    activity_filter = observerB->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::FollowSelection,ActivityPolicyFilter::MultipleActivity);
    activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::AllowNoneActive);
    observerB->displayHints()->setObserverSelectionContextHint(ObserverHints::SelectionUseSelectedContext);
    observerB->displayHints()->setActionHints(action_hints);
    observerB->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    observerB->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    observerB->displayHints()->setDisplayFlagsHint(display_flags);
    observerB->displayHints()->setDragDropHint(ObserverHints::AcceptDrops);

    TreeNode* observerC = new TreeNode("TreeNode C");
    activity_filter = observerC->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::FollowSelection,ActivityPolicyFilter::MultipleActivity);
    activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::AllowNoneActive);
    observerC->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames);
    observerC->displayHints()->setActionHints(action_hints);
    observerC->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    observerC->displayHints()->setItemSelectionControlHint(ObserverHints::SelectableItems);
    observerC->displayHints()->setNamingControlHint(ObserverHints::EditableNames);
    observerC->displayHints()->setItemViewColumnHint(ObserverHints::ColumnNameHint | ObserverHints::ColumnCategoryHint);
    observerC->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    observerC->displayHints()->setDisplayFlagsHint(display_flags);
    observerC->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    QtilitiesCategory category("Category 2");
    observerC->displayHints()->addDisplayedCategory(category);
    observerC->displayHints()->setCategoryFilterEnabled(false);
    observerC->setAccessModeScope(Observer::CategorizedScope);
    observerC->displayHints()->setDragDropHint(ObserverHints::AllowDrags);

    // Add some items to the tree:
    observerB->addItem("Item 1");
    observerC->addItem("Item 2",QtilitiesCategory("Category 1"));
    observerC->addItem("Item 3",QtilitiesCategory("Category 1"));
    observerC->addItem("Item 4",QtilitiesCategory("Category 1"));

    QtilitiesCategory multi_category;
    multi_category << "Category 1" << "Sub Category 1";
    observerC->addItem("Item 5",multi_category);
    observerC->addItem("Item 6",QtilitiesCategory("Category 1::Sub Category 2","::"));
    observerC->addItem("Item 7",QtilitiesCategory("Category 1::Sub Category 3","::"));

    // Create the structure of the tree
    observerA->attachSubject(observerB);
    observerA->attachSubject(observerC);

    // Only set the access mode after the object with the category exists in the context.
    observerC->setAccessMode(Observer::ReadOnlyAccess,category);

    // Create the first observer widget:
    ObserverWidget* observer_widgetA = new ObserverWidget(Qtilities::TableView);
    observer_widgetA->setAcceptDrops(true);
    observer_widgetA->resize(600,250);
    QStack<int> nav_stack;
    nav_stack << observerA->observerID();
    observer_widgetA->setNavigationStack(nav_stack);
    observer_widgetA->setObserverContext(observerB);
    ExtendedObserverTableModel* extended_table_model = new ExtendedObserverTableModel();
    observer_widgetA->setCustomTableModel(extended_table_model);
    ExtendedObserverTreeModel* extended_tree_model = new ExtendedObserverTreeModel();
    observer_widgetA->setCustomTreeModel(extended_tree_model);
    observer_widgetA->initialize();
    observer_widgetA->toggleUseGlobalActiveObjects(true);

    // Create the second observer widget:
    ObserverWidget* observer_widgetB = new ObserverWidget(Qtilities::TableView);
    observer_widgetB->setAcceptDrops(true);
    observer_widgetB->resize(600,250);
    observer_widgetB->setNavigationStack(nav_stack);
    observer_widgetB->setObserverContext(observerC);
    observer_widgetB->initialize();
    observer_widgetB->toggleUseGlobalActiveObjects(true);

    // Note, since v1.5, this example does not restore the ObserverWidget setting when the application is closed and reopened.
    // This change was introduced because of the removal of writeSettings() and readSettings() on ObserverWidget. Implementing it
    // here instead of in ObserverWidget itself is the right way to do this.

    // Create a container widget for all these widgets:
    QWidget* observer_widget_container = new QWidget();
    if (observer_widget_container->layout())
        delete observer_widget_container->layout();
    QVBoxLayout* observer_widget_container_layout = new QVBoxLayout(observer_widget_container);
    observer_widget_container_layout->addWidget(observer_widgetA);
    observer_widget_container_layout->addWidget(observer_widgetB);
    observer_widget_container_layout->setMargin(0);

    // Set up config widget:
    OBJECT_MANAGER->registerObject(ACTION_MANAGER->commandEditor(),QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));
    OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget(false),QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));
    ObserverWidgetConfig observer_config_widget;
    OBJECT_MANAGER->registerObject(&observer_config_widget,QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));
    QList<QObject*> registered_config_pages = OBJECT_MANAGER->registeredInterfaces("IConfigPage");
    config_widget->initialize(registered_config_pages);

    // Create a main widget container:
    main_window->setCentralWidget(observer_widget_container);
    main_window->addDockWidget(Qt::BottomDockWidgetArea,log_dock_widget);
    main_window->resize(1000,800);
    main_window->setMenuBar(menu_bar->menuBar());
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    main_window->restoreGeometry(settings.value("geometry").toByteArray());
    main_window->restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
    main_window->show();
    observer_widget_container->show();
    observer_widgetA->show();
    observer_widgetB->show();

    // Create some widgets connected to the global active objects signal on the object manager:
    QDockWidget* object_scope_dock = new QDockWidget("Object Scope Overview");
    object_scope_dock->setObjectName("Object Scope Overview Dock Widget");
    ObjectScopeWidget* scope_widget = new ObjectScopeWidget();
    object_scope_dock->setWidget(scope_widget);
    QObject::connect(OBJECT_MANAGER,SIGNAL(metaTypeActiveObjectsChanged(QList<QPointer<QObject> >,QString)),scope_widget,SLOT(setObject(QList<QPointer<QObject> >)));
    main_window->addDockWidget(Qt::LeftDockWidgetArea,object_scope_dock);

    #ifdef QTILITIES_PROPERTY_BROWSER
    QDockWidget* property_browser_dock = new QDockWidget("Object Properties");
    property_browser_dock->setObjectName("Object Properties Dock Widget");
    ObjectPropertyBrowser* property_browser = new ObjectPropertyBrowser;
    property_browser_dock->setWidget(property_browser);
    QObject::connect(OBJECT_MANAGER,SIGNAL(metaTypeActiveObjectsChanged(QList<QPointer<QObject> >,QString)),property_browser,SLOT(setObject(QList<QPointer<QObject> >)));
    main_window->addDockWidget(Qt::LeftDockWidgetArea,property_browser_dock);
    #endif

    // Load the previous session's keyboard mapping file.
    QString shortcut_mapping_file = QString("%1/%2").arg(QtilitiesApplication::applicationSessionPath()).arg(qti_def_PATH_SHORTCUTS_FILE);
    ACTION_MANAGER->loadShortcutMapping(shortcut_mapping_file);

    // Create a debug widget
    //DebugWidget* debug_widget = new DebugWidget;
    //debug_widget->modeWidget()->show();

    int result = a.exec();

    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", main_window->saveGeometry());
    settings.setValue("windowState", main_window->saveState());
    settings.endGroup();
    settings.endGroup();

    // Save the current keyboard mapping for the next session.
    ACTION_MANAGER->saveShortcutMapping(shortcut_mapping_file);

    LOG_FINALIZE();
    return result;
}
