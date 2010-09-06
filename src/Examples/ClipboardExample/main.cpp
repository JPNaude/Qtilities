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

#include "ObserverWidgetConfig.h"
#include "ExtendedObserverTableModel.h"
#include "ExtendedObserverTreeModel.h"

using namespace QtilitiesCore;
using namespace QtilitiesCoreGui;
using namespace Qtilities::Examples::Clipboard;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Clipboard Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());

    QMainWindow* main_window = new QMainWindow();
    QtilitiesApplication::setMainWindow(main_window);
    ConfigurationWidget* config_widget = new ConfigurationWidget();
    QtilitiesApplication::setConfigWidget(config_widget);

    LOG_INITIALIZE();
    Log->setGlobalLogLevel(Logger::Debug);
    Log->setIsQtMessageHandler(true);

    // Create the menu bar and menus in the menu bar:
    bool existed;
    ActionContainer* menu_bar = ACTION_MANAGER->createMenuBar(MENUBAR_STANDARD,existed);
    ActionContainer* edit_menu = ACTION_MANAGER->createMenu(MENU_EDIT,existed);
    menu_bar->addMenu(edit_menu);

    // Get the standard context:
    QList<int> std_context;
    std_context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_STANDARD));

    // Create the configuration widget here and then connect it to the settings action
    Command* command = ACTION_MANAGER->registerActionPlaceHolder(MENU_FILE_SETTINGS,QObject::tr("Settings"),QKeySequence(),std_context);

    QObject::connect(command->action(),SIGNAL(triggered()),config_widget,SLOT(show()));
    edit_menu->addAction(command);
    edit_menu->addSeperator();
    // Register action placeholders for the copy, cut and paste actions:
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_COPY,QObject::tr("Copy"),QKeySequence(QKeySequence::Copy));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_CUT,QObject::tr("Cut"),QKeySequence(QKeySequence::Cut));
    edit_menu->addAction(command);
    command = ACTION_MANAGER->registerActionPlaceHolder(MENU_EDIT_PASTE,QObject::tr("Paste"),QKeySequence(QKeySequence::Paste));
    edit_menu->addAction(command);

    // Important: The Log widget must be created before the above action place holders were added since it registers some actions
    QDockWidget* log_dock_widget = LoggerGui::createLogDockWidget("Clipboard Example Log",true);
    log_dock_widget->show();
    log_dock_widget->resize(1000,250);

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
    observerA->displayHints()->setActivityControlHint(ObserverHints::CheckboxTriggered);
    observerA->displayHints()->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::ActionRemoveAll;
    action_hints |= ObserverHints::ActionDeleteAll;
    action_hints |= ObserverHints::ActionPushDown;
    action_hints |= ObserverHints::ActionPushUp;
    action_hints |= ObserverHints::ActionRefreshView;
    action_hints |= ObserverHints::ActionCopyItem;
    action_hints |= ObserverHints::ActionCutItem;
    action_hints |= ObserverHints::ActionPasteItem;
    action_hints |= ObserverHints::ActionSwitchView;
    action_hints |= ObserverHints::ActionFindItem;
    observerA->displayHints()->setActionHints(action_hints);
    observerA->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    display_flags |= ObserverHints::NavigationBar;
    display_flags |= ObserverHints::ActionToolBar;
    observerA->displayHints()->setDisplayFlagsHint(display_flags);

    Observer* observerB = new Observer("Observer B","Child observer");
    observerB->useDisplayHints();
    // Naming policy filter
    //NamingPolicyFilter* naming_filter2 = new NamingPolicyFilter();
    //naming_filter2->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    //observerB->installSubjectFilter(naming_filter2);
    //observerB->displayHints()->setNamingControlHint(ObserverHints::EditableNames);
    // Activty policy filter
    activity_filter = new ActivityPolicyFilter();
    activity_filter->setActivityPolicy(ActivityPolicyFilter::MultipleActivity);
    activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::AllowNoneActive);
    observerB->installSubjectFilter(activity_filter);
    observerB->displayHints()->setNamingControlHint(ObserverHints::ReadOnlyNames);
    observerB->displayHints()->setActivityControlHint(ObserverHints::FollowSelection);
    observerB->displayHints()->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    observerB->displayHints()->setObserverSelectionContextHint(ObserverHints::SelectionUseSelectedContext);
    observerB->displayHints()->setActionHints(action_hints);
    observerB->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    observerB->displayHints()->setDisplayFlagsHint(display_flags);

    Observer* observerC = new Observer("Observer C","Child observer");
    observerC->useDisplayHints();
    activity_filter = new ActivityPolicyFilter();
    activity_filter->setActivityPolicy(ActivityPolicyFilter::MultipleActivity);
    activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::AllowNoneActive);
    observerC->installSubjectFilter(activity_filter);
    observerC->displayHints()->setActivityControlHint(ObserverHints::FollowSelection);
    observerC->displayHints()->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    // Naming policy filter
    //naming_filter = new NamingPolicyFilter();
    //naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    //observerC->installSubjectFilter(naming_filter);
    //observerC->displayHints()->setNamingControlHint(Observer::EditableNames);
    observerC->displayHints()->setActionHints(action_hints);
    observerC->displayHints()->setItemSelectionControlHint(ObserverHints::SelectableItems);
    observerC->displayHints()->setNamingControlHint(ObserverHints::EditableNames);
    observerC->displayHints()->setItemViewColumnHint(ObserverHints::ColumnNameHint | ObserverHints::ColumnCategoryHint);
    observerC->displayHints()->setDisplayFlagsHint(display_flags);
    observerC->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    QtilitiesCategory category("Category 2");
    observerC->displayHints()->addDisplayedCategory(category);
    //observerC->displayHints()->setCategoryFilterEnabled(true);

    // Create the objects
    QObject* object1 = new QObject();
    object1->setObjectName("Object 1");
    ObserverProperty category_property1(OBJECT_CATEGORY);
    QtilitiesCategory multi_category1;
    multi_category1 << "Category 1";
    category_property1.setValue(qVariantFromValue(multi_category1),observerC->observerID());
    Observer::setObserverProperty(object1,category_property1);

    QObject* object2 = new QObject();
    object2->setObjectName("Object 2");
    ObserverProperty category_property2(OBJECT_CATEGORY);
    QtilitiesCategory multi_category2;
    multi_category2 << "Category 1";
    category_property2.setValue(qVariantFromValue(multi_category2),observerC->observerID());
    Observer::setObserverProperty(object2,category_property2);

    QObject* object3 = new QObject();
    object3->setObjectName("Object 3");
    ObserverProperty category_property3(OBJECT_CATEGORY);
    QtilitiesCategory multi_category3;
    multi_category3 << "Category 1" << "Sub Category 3";
    category_property3.setValue(qVariantFromValue(multi_category3),observerC->observerID());
    Observer::setObserverProperty(object3,category_property3);

    QObject* object4 = new QObject();
    object4->setObjectName("Object 4");
    ObserverProperty category_property4(OBJECT_CATEGORY);
    QtilitiesCategory multi_category4;
    multi_category4 << "Category 1" << "Sub Category 2";
    category_property4.setValue(qVariantFromValue(multi_category4),observerC->observerID());
    Observer::setObserverProperty(object4,category_property4);

    QObject* object5 = new QObject();
    object5->setObjectName("Object 5");
    ObserverProperty category_property5(OBJECT_CATEGORY);
    QtilitiesCategory multi_category5;
    multi_category5 << "Category 1";
    category_property5.setValue(qVariantFromValue(multi_category5),observerC->observerID());
    Observer::setObserverProperty(object5,category_property5);

    QObject* object6 = new QObject();
    object6->setObjectName("Object 6");
    ObserverProperty category_property6(OBJECT_CATEGORY);
    QtilitiesCategory multi_category6;
    multi_category6 << "Category 1" << "Sub Category 1";
    category_property6.setValue(qVariantFromValue(multi_category6),observerC->observerID());
    Observer::setObserverProperty(object6,category_property6);

    QObject* object7 = new QObject();
    object7->setObjectName("Object 7");
    ObserverProperty category_property7(OBJECT_CATEGORY);
    category_property7.setValue(qVariantFromValue(QtilitiesCategory("Category 2")),observerC->observerID());
    Observer::setObserverProperty(object7,category_property7);

    // Create the structure of the tree
    observerA->attachSubject(observerB);
    observerA->attachSubject(observerC);
    observerC->attachSubject(object1);
    observerC->attachSubject(object2);
    observerC->attachSubject(object3);
    observerC->attachSubject(object4);
    observerC->attachSubject(object5);
    observerC->attachSubject(object6);
    observerC->attachSubject(object7);

    // Create the first observer widget:
    ObserverWidget* observer_widgetA = new ObserverWidget(ObserverWidget::TableView);
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
    observer_widgetA->readSettings();
    observer_widgetA->toggleUseGlobalActiveObjects(true);

    // Create the second observer widget:
    ObserverWidget* observer_widgetB = new ObserverWidget(ObserverWidget::TableView);
    observer_widgetB->setAcceptDrops(true);
    observer_widgetB->resize(600,250);
    observer_widgetB->setNavigationStack(nav_stack);
    observer_widgetB->setObserverContext(observerC);
    observer_widgetB->initialize();
    observer_widgetB->readSettings();
    observer_widgetB->toggleUseGlobalActiveObjects(true);

    // Create a container widget for all these widgets:
    QWidget* observer_widget_container = new QWidget();
    if (observer_widget_container->layout())
        delete observer_widget_container->layout();
    QVBoxLayout* observer_widget_container_layout = new QVBoxLayout(observer_widget_container);
    observer_widget_container_layout->addWidget(observer_widgetA);
    observer_widget_container_layout->addWidget(observer_widgetB);
    observer_widget_container_layout->setMargin(0);

    // Set up config widget:
    OBJECT_MANAGER->registerObject(ACTION_MANAGER->commandEditor());
    OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget(false));
    ObserverWidgetConfig observer_config_widget;
    OBJECT_MANAGER->registerObject(&observer_config_widget);
    QList<QObject*> registered_config_pages = OBJECT_MANAGER->registeredInterfaces("IConfigPage");
    config_widget->initialize(registered_config_pages);

    // Create a main widget container:
    main_window->setCentralWidget(observer_widget_container);
    main_window->addDockWidget(Qt::BottomDockWidgetArea,log_dock_widget);
    main_window->resize(1000,800);
    main_window->setMenuBar(menu_bar->menuBar());
    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    main_window->restoreGeometry(settings.value("geometry").toByteArray());
    main_window->restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();
    settings.endGroup();
    main_window->show();
    observer_widget_container->show();
    observer_widgetA->show();
    observer_widgetB->show();

    // Create some widgets connected to the global active objects signal on the object manager:
    QDockWidget* object_scope_dock = new QDockWidget("Object Scope Overview");
    ObjectScopeWidget* scope_widget = new ObjectScopeWidget();
    object_scope_dock->setWidget(scope_widget);
    QObject::connect(OBJECT_MANAGER,SIGNAL(metaTypeActiveObjectsChanged(QList<QObject*>,QString)),scope_widget,SLOT(setObject(QList<QObject*>)));
    QDockWidget* property_browser_dock = new QDockWidget("Object Properties");
    ObjectPropertyBrowser* property_browser = new ObjectPropertyBrowser();
    property_browser_dock->setWidget(property_browser);
    QObject::connect(OBJECT_MANAGER,SIGNAL(metaTypeActiveObjectsChanged(QList<QObject*>,QString)),property_browser,SLOT(setObject(QList<QObject*>)));
    main_window->addDockWidget(Qt::LeftDockWidgetArea,object_scope_dock);
    main_window->addDockWidget(Qt::LeftDockWidgetArea,property_browser_dock);

    // Load the previous session's keyboard mapping file.
    QString shortcut_mapping_file = QString("%1/session/%2").arg(QApplication::applicationDirPath()).arg(FILE_SHORTCUT_MAPPING);
    if (ACTION_MANAGER->importShortcutMapping(shortcut_mapping_file))
        LOG_INFO(QObject::tr("Succesfully loaded shortcut mapping from previous session. Path: ") + shortcut_mapping_file);
    else
        LOG_WARNING(QObject::tr("Failed to load shortcut mapping from previous session. The default mapping scheme will be used. Path: ") + shortcut_mapping_file);

    int result = a.exec();

    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", main_window->saveGeometry());
    settings.setValue("windowState", main_window->saveState());
    settings.endGroup();
    settings.endGroup();

    // Save the current keyboard mapping for the next session.
    if (ACTION_MANAGER->exportShortcutMapping(shortcut_mapping_file))
        LOG_INFO(QObject::tr("Succesfully saved shortcut mapping for next session. Path: ") + shortcut_mapping_file);
    else
        LOG_WARNING(QObject::tr("Failed to save shortcut mapping for next session. Path: ") + shortcut_mapping_file);

    LOG_FINALIZE();
    return result;
}
