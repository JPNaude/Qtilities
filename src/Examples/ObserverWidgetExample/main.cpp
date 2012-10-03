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

#include "AvailablePropertyProvider.h"

using namespace QtilitiesCore;
using namespace QtilitiesCoreGui;
using namespace Qtilities::Examples::ObserverWidgetExample;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);

    // Create the observer widget in tree mode:
    // (Note we can also use TreeWidget which is the same as ObserverWidget)
    ObserverWidget* observer_widget = new ObserverWidget;
    // Create the observer widget in table mode
    //ObserverWidget* observer_widget = new ObserverWidget(Qtilities::TableView);
    // Size smallest
    //observer_widget->resize(350,200);
    // Size biggest
    observer_widget->resize(600,250);
    QtilitiesApplication::setMainWindow(observer_widget);

    Log->setLoggerSessionConfigPath(QtilitiesApplication::applicationSessionPath());
    LOG_INITIALIZE();
    Log->setIsQtMessageHandler(false);

    // -----------------------------------
    // Construct some hints:
    // -----------------------------------
    ObserverHints* hints = new ObserverHints;
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::ActionPushDown;
    action_hints |= ObserverHints::ActionPushUp;
    action_hints |= ObserverHints::ActionSwitchView;
    action_hints |= ObserverHints::ActionRefreshView;
    hints->setActionHints(action_hints);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    display_flags |= ObserverHints::NavigationBar;
    display_flags |= ObserverHints::ActionToolBar;
    display_flags |= ObserverHints::PropertyBrowser;
    hints->setDisplayFlagsHint(display_flags);
    hints->setItemViewColumnHint(ObserverHints::ColumnNameHint | ObserverHints::ColumnCategoryHint);
    hints->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    hints->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    ObserverHints::ActionHints active_hints = hints->actionHints();
    active_hints |= ObserverHints::ActionFindItem;
    hints->setActionHints(active_hints);

    #ifdef QTILITIES_PROPERTY_BROWSER
    hints->setDisplayFlagsHint(ObserverHints::ItemView |
                               ObserverHints::ActionToolBar |
//                             ObserverHints::PropertyBrowser |
                               ObserverHints::DynamicPropertyBrowser);
    #endif

    // -----------------------------------
    // Create a tree using TreeNode, a class inherting Observer:
    // -----------------------------------
    TreeNode* rootNode = new TreeNode("Root");
    rootNode->copyHints(hints);
    rootNode->enableNamingControl(ObserverHints::EditableNames,
                                  NamingPolicyFilter::ProhibitDuplicateNames);
    rootNode->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    rootNode->enableActivityControl(ObserverHints::CheckboxActivityDisplay,
                                    ObserverHints::CheckboxTriggered,
                                    ActivityPolicyFilter::MultipleActivity);

    TreeNode* nodeA = rootNode->addNode("Node A");
    nodeA->enableActivityControl(ObserverHints::CheckboxActivityDisplay,
                                 ObserverHints::CheckboxTriggered,
                                 ActivityPolicyFilter::MultipleActivity,
                                 ActivityPolicyFilter::ParentFollowActivity);

    TreeNode* nodeB = rootNode->addNode("Node B");
    nodeB->copyHints(rootNode->displayHints());
    nodeB->enableCategorizedDisplay();

    // Add some tree items:
    rootNode->addItem("Child 1");
    rootNode->addItem("Child 2");
    nodeA->addItem("Child 3");
    nodeA->addItem("Child 4");
    nodeB->addItem("Child 5",QtilitiesCategory("Category 1"));
    nodeB->addItem("Child 6",QtilitiesCategory("Category 2"));
    nodeB->addItem("Child 7",QtilitiesCategory("Category 2"));

    // Only change access modes after our items were added to nodes:
    nodeA->setAccessMode(Observer::ReadOnlyAccess);
    nodeB->setAccessMode(Observer::LockedAccess);

    // If we want to, we can filter categories on nodes. However, to see the effect,
    // we need to change nodeB's access to not be LockedAccess as it was set above.
//    QList<QtilitiesCategory> displayed_categories;
//    displayed_categories << QtilitiesCategory("Category 1");
//    nodeB->displayHints()->setDisplayedCategories(displayed_categories);
//    nodeB->displayHints()->setCategoryFilterEnabled(true);

    // Add some example dynamic properties on items to show in dynamic property editor:
    ObjectManager::setSharedProperty(rootNode,"Test Property",false);

    // The observer widget will use the hints of the selection's parent observer context.
    // However we can also tell it to use its own hints by uncommenting the line below:
    //observer_widget->toggleUseObserverHints(false);

    // -----------------------------------
    // Demonstrate Use Of Property Browsers
    // -----------------------------------
    #ifdef QTILITIES_PROPERTY_BROWSER
    hints->setDisplayFlagsHint(ObserverHints::ItemView |
//                             ObserverHints::PropertyBrowser |
                               ObserverHints::DynamicPropertyBrowser);

    // Before the observer widget initialization we can specify where the editor should be, and of what type the editor must be:
    observer_widget->setPreferredDynamicPropertyEditorDockArea(Qt::RightDockWidgetArea);
    observer_widget->setPreferredDynamicPropertyEditorType(ObjectDynamicPropertyBrowser::TreeBrowser);

    // Before the observer widget initialization we can specify where the editor should be, and of what type the editor must be:
    observer_widget->setPreferredPropertyEditorDockArea(Qt::RightDockWidgetArea);
    observer_widget->setPreferredPropertyEditorType(ObjectPropertyBrowser::ButtonBrowser);
    #endif

    // Init and show the observer widget:
    observer_widget->setObserverContext(rootNode);
    observer_widget->initialize();

    #ifdef QTILITIES_PROPERTY_BROWSER
    // Filter QObject in the property browser:
    if (observer_widget->propertyBrowser()) {
      QStringList filter_list;
      filter_list << "QObject";
      observer_widget->propertyBrowser()->setFilterList(filter_list);
    }
    // We can also show Qtilities properties on the dynamic property browser:
    if (observer_widget->dynamicPropertyBrowser()) {
        observer_widget->dynamicPropertyBrowser()->toggleQtilitiesProperties(true);
        observer_widget->dynamicPropertyBrowser()->toggleToolBar();
        observer_widget->dynamicPropertyBrowser()->setNewPropertyType(ObjectManager::SharedProperties);
    }
    #endif

    // -----------------------------------
    // Finally show the widget:
    // -----------------------------------
    observer_widget->show();

    // -----------------------------------
    // Create and register an example implementation of IAvailablePropertyProvider to
    // demonstrate how it works.
    // -----------------------------------
    AvailablePropertyProvider* property_provider = new AvailablePropertyProvider;
    OBJECT_MANAGER->registerObject(property_provider);

    return a.exec();
}
