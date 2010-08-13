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

#include <QtilitiesCoreGuiModule>

using namespace QtilitiesCoreModule;
using namespace QtilitiesCoreGuiModule;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Jaco Naude");
    QCoreApplication::setOrganizationDomain("Qtilities");
    QCoreApplication::setApplicationName("Observer Widget Example");
    QCoreApplication::setApplicationVersion(QtilitiesCore::instance()->version());

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
    // Set observer hints
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::PushDown;
    action_hints |= ObserverHints::PushUp;
    action_hints |= ObserverHints::SwitchView;
    action_hints |= ObserverHints::RefreshView;
    observerA->displayHints()->setActionHints(action_hints);
    observerA->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHint);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    display_flags |= ObserverHints::NavigationBar;
    //display_flags |= ObserverHints::PropertyBrowser;
    observerA->displayHints()->setDisplayFlagsHint(display_flags);

    Observer* observerB = new Observer("Observer B","Child observer");
    observerB->useDisplayHints();
    // Activty policy filter
    activity_filter = new ActivityPolicyFilter();
    activity_filter->setActivityPolicy(ActivityPolicyFilter::MultipleActivity);
    activity_filter->setMinimumActivityPolicy(ActivityPolicyFilter::AllowNoneActive);
    observerB->installSubjectFilter(activity_filter);
    observerB->displayHints()->setNamingControlHint(ObserverHints::ReadOnlyNames);
    observerB->displayHints()->setActivityControlHint(ObserverHints::FollowSelection);
    observerB->displayHints()->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    // Set observer hints
    observerB->displayHints()->setActionHints(action_hints);
    observerB->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHint);
    //observerB->displayHints()->setDisplayFlagsHint(display_flags);

    Observer* observerC = new Observer("Observer C","Child observer");
    observerC->useDisplayHints();
    // Naming policy filter
    /*naming_filter = new NamingPolicyFilter();
    naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    observerC->installSubjectFilter(naming_filter);
    observerC->displayHints()->setNamingControlHint(Observer::EditableNames);*/
    observerC->displayHints()->setActionHints(action_hints);
    //observerC->displayHints()->setItemViewColumnFlags(Observer::AllColumnsHint);
    observerC->displayHints()->setItemSelectionControlHint(ObserverHints::NonSelectableItems);
    observerC->displayHints()->setNamingControlHint(ObserverHints::EditableNames);
    observerC->displayHints()->setItemViewColumnHint(ObserverHints::ColumnCategoryHint);
    observerC->displayHints()->setDisplayFlagsHint(display_flags);
    observerC->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    QStringList displayed_categories;
    displayed_categories << "Category 2";
    observerC->displayHints()->setDisplayedCategories(displayed_categories);
    //observerC->displayHints()->setCategoryFilterEnabled(true);

    // Create the objects
    QObject* object1 = new QObject();
    object1->setObjectName("Object 1");
    QObject* object2 = new QObject();
    object2->setObjectName("Object 2");
    QObject* object3 = new QObject();
    ObserverProperty category_property3(OBJECT_CATEGORY);
    category_property3.setValue("Category 1",observerC->observerID());
    Observer::setObserverProperty(object3,category_property3);
    object3->setObjectName("Object 3");
    QObject* object4 = new QObject();
    object4->setObjectName("Object 4");
    QObject* object5 = new QObject();
    ObserverProperty category_property5(OBJECT_CATEGORY);
    category_property5.setValue("Category 2",observerC->observerID());
    Observer::setObserverProperty(object5,category_property5);
    object5->setObjectName("Object 5");

    // Create the structure of the tree
    observerA->attachSubject(observerB);
    observerA->attachSubject(observerC);
    observerA->attachSubject(object1);
    observerA->attachSubject(object4);

    observerB->attachSubject(object2);
    observerB->attachSubject(object3);
    //observerB->attachSubject(object3,Observer::SpecificObserverOwnership);

    observerC->attachSubject(object5);
    observerC->attachSubject(object3);

    // Set access modes only now after all subjects were attached
    //observerB->setAccessMode(Observer::ReadOnlyAccess);
    //observerC->setAccessMode(Observer::LockedAccess);

    // Create the observer widget in tree mode
    ObserverWidget* observer_widget = new ObserverWidget();
    // Create the observer widget in table mode
    //ObserverWidget* observer_widget = new ObserverWidget(ObserverWidget::TableView);
    // Size smallest
    //observer_widget->resize(350,200);
    // Size biggest
    observer_widget->resize(600,250);
    observer_widget->setObserverContext(observerA);

    // Before the observer widget initialization we can specify where the editor should be, and of what type the editor must be:
    observer_widget->setPreferredPropertyEditorDockArea(Qt::RightDockWidgetArea);
    observer_widget->setPreferredPropertyEditorType(ObjectPropertyBrowser::TreeBrowser);
    //observer_widget->setPreferredPropertyEditorType(ObjectPropertyBrowser::GroupBoxBrowser);
    //observer_widget->setPreferredPropertyEditorType(ObjectPropertyBrowser::ButtonBrowser);

    // We need to tell all three observers that they should provide an action hint for the search action as well.
    action_hints = observerA->displayHints()->actionHints();
    action_hints |= ObserverHints::FindItem;
    observerA->displayHints()->setActionHints(action_hints);
    observerB->displayHints()->setActionHints(action_hints);
    //observerC->displayHints()->setActionHints(action_hints);

    observer_widget->initialize();

    // We add all actions provided by the observer widget to a toolbar at the top of the observer widget for this example.
    QList<QAction*> actions = observer_widget->actionProvider()->actions();
    QToolBar* toolbar = observer_widget->addToolBar("Context Toolbar");
    observer_widget->addToolBar(toolbar);
    toolbar->addActions(actions);

    // After the observer widget was initialized we can access the property editor and call functions on it:
    if (observer_widget->propertyBrowser()) {
            QStringList filter_list;
            filter_list << "QObject";
            observer_widget->propertyBrowser()->setFilterList(filter_list);
    }

    observer_widget->show();

    // Create an example search box widget
    /*SearchBoxWidget* search_box_widget = new SearchBoxWidget(SearchBoxWidget::AllSearchOptions,SearchBoxWidget::SearchAndReplace);
    search_box_widget->show();*/

    // Create the object scope widget and set it's object to object3:
    ObjectScopeWidget* scope_widget = new ObjectScopeWidget();
    scope_widget->resize(300,300);
    scope_widget->setObject(object3);
    scope_widget->show();

    return a.exec();
}
