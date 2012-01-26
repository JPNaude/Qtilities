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

using namespace QtilitiesCore;
using namespace QtilitiesCoreGui;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);

    // Create the observer widget in tree mode
    ObserverWidget* observer_widget = new ObserverWidget();
    // Create the observer widget in table mode
    //ObserverWidget* observer_widget = new ObserverWidget(Qtilities::TableView);
    // Size smallest
    //observer_widget->resize(350,200);
    // Size biggest
    observer_widget->resize(600,250);
    QtilitiesApplication::setMainWindow(observer_widget);

    Log->setLoggerSessionConfigPath(QtilitiesApplication::applicationSessionPath());
    LOG_INITIALIZE();

    // Create a tree using TreeNode, a class inherting Observer:
    TreeNode* rootNode = new TreeNode("Root");
    rootNode->toggleQtilitiesPropertyChangeEvents(true);
    rootNode->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames);
    rootNode->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::CheckboxTriggered,ActivityPolicyFilter::MultipleActivity);
    TreeNode* nodeA = rootNode->addNode("Node A");
    nodeA->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::CheckboxTriggered,ActivityPolicyFilter::MultipleActivity,ActivityPolicyFilter::ParentFollowActivity);
    TreeNode* nodeB = rootNode->addNode("Node B");
    nodeB->enableCategorizedDisplay();
    rootNode->addItem("Child 1");
    rootNode->addItem("Child 2");
    nodeA->addItem("Child 3");
    nodeA->addItem("Child 4");
    nodeB->addItem("Child 5",QtilitiesCategory("Category 1"));
    nodeB->addItem("Child 6",QtilitiesCategory("Category 2"));
    nodeB->addItem("Child 7",QtilitiesCategory("Category 2"));

    // First Set Of Hints:
    observer_widget->toggleUseObserverHints(false);
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::ActionPushDown;
    action_hints |= ObserverHints::ActionPushUp;
    action_hints |= ObserverHints::ActionSwitchView;
    action_hints |= ObserverHints::ActionRefreshView;
    observer_widget->activeHints()->setActionHints(action_hints);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    display_flags |= ObserverHints::NavigationBar;
    display_flags |= ObserverHints::ActionToolBar;
    display_flags |= ObserverHints::PropertyBrowser;
    observer_widget->activeHints()->setDisplayFlagsHint(display_flags);
    observer_widget->activeHints()->setItemViewColumnHint(ObserverHints::ColumnNameHint | ObserverHints::ColumnCategoryHint);
    observer_widget->activeHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    observer_widget->activeHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    QList<QtilitiesCategory> displayed_categories;
    displayed_categories << QtilitiesCategory("Category 1");
    nodeB->displayHints()->setDisplayedCategories(displayed_categories);
    nodeB->displayHints()->setCategoryFilterEnabled(true);
    ObserverHints::ActionHints active_hints = observer_widget->activeHints()->actionHints();
    active_hints |= ObserverHints::ActionFindItem;
    observer_widget->activeHints()->setActionHints(active_hints);

    nodeA->setAccessMode(Observer::ReadOnlyAccess);
    nodeB->setAccessMode(Observer::LockedAccess);

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
    #endif

    // Finally show the widget:
    observer_widget->show();

    return a.exec();
}

//#ifdef QTILITIES_PROPERTY_BROWSER
// Before the observer widget initialization we can specify where the editor should be, and of what type the editor must be:
/*observer_widget->setPreferredPropertyEditorDockArea(Qt::RightDockWidgetArea);
observer_widget->setPreferredPropertyEditorType(ObjectPropertyBrowser::TreeBrowser);
//observer_widget->setPreferredPropertyEditorType(ObjectPropertyBrowser::GroupBoxBrowser);
//observer_widget->setPreferredPropertyEditorType(ObjectPropertyBrowser::ButtonBrowser);
#endif

// We need to tell all three observers that they should provide an action hint for the search action as well.
ObserverHints::ActionHints action_hints = observerA->displayHints()->actionHints();
action_hints |= ObserverHints::ActionFindItem;
nodeA->displayHints()->setActionHints(action_hints);
nodeB->displayHints()->setActionHints(action_hints);*/
//observerC->displayHints()->setActionHints(action_hints);

// After the observer widget was initialized we can access the property editor and call functions on it:
/*#ifdef QTILITIES_PROPERTY_BROWSER
if (observer_widget->propertyBrowser()) {
        QStringList filter_list;
        filter_list << "QObject";
        observer_widget->propertyBrowser()->setFilterList(filter_list);
}
#endif*/
