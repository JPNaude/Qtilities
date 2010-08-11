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

#include "ObjectManagementModeWidget.h"
#include "ui_ObjectManagementModeWidget.h"
#include "ObserverStringSubject.h"

#include <QtilitiesCoreGuiConstants.h>
#include <QtilitiesCoreGui.h>
#include <ObserverWidget.h>
#include <NamingPolicyFilter.h>
#include <ActivityPolicyFilter.h>
#include <QtilitiesCore.h>
#include <QtilitiesCoreConstants.h>
#include <ObjectScopeWidget.h>
#include <ObserverProjectItemWrapper.h>

#include <QtGui>
#include <QSettings>
#include <QPointer>

using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::ProjectManagement;

struct Qtilities::Examples::ObjectManagement::ObjectManagementModeWidgetData {
     ObjectManagementModeWidgetData() : top_level_observer(0),
     observer_widget(0),
     scope_widget(0),
     project_item(0) { }

    QPointer<Observer> top_level_observer;
    ObserverWidget* observer_widget;
    ObjectScopeWidget* scope_widget;

    QAction *actionAddExampleObjects;
    QAction *exitAct;
    QToolBar* main_toolbar;

    // Example toolbar and actions to control widgets in the tree
    QToolBar* widgets_toolbar;
    QAction* widget_show_action;
    QAction* widget_hide_action;
    QSlider* widget_opacity_slider;

    ObserverProjectItemWrapper* project_item;

    Factory<QObject> string_subject_factory;
};

Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::ObjectManagementModeWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ObjectManagementModeWidget)
{
    ui->setupUi(this);

    d = new ObjectManagementModeWidgetData;
    d->top_level_observer= new Observer("Example Top Level","");
    d->top_level_observer->setActionHints(Observer::AllActionsHint);

    // ---------------------------
    // Setup the top level observer
    // ---------------------------
    NamingPolicyFilter* naming_filter = new NamingPolicyFilter();
    naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    d->top_level_observer->installSubjectFilter(naming_filter);
    d->top_level_observer->setNamingControlHint(Observer::EditableNames);

    // ---------------------------
    // Factory and Project Item Stuff
    // ---------------------------
    QtilitiesCore::instance()->objectManager()->registerIFactory(this);
    FactoryInterfaceData observer_string_subject_data("Observer String Subject",QStringList(),QStringList());
    d->string_subject_factory.registerFactoryInterface(&ObserverStringSubject::factory,observer_string_subject_data);

    d->project_item = new ObserverProjectItemWrapper(this);
    d->project_item->setObserverContext(d->top_level_observer);
    QtilitiesCore::instance()->objectManager()->registerObject(d->project_item);

    QList<int> context;
    context.push_front(QtilitiesCore::instance()->contextManager()->contextID(CONTEXT_STANDARD));

    // ---------------------------
    // Add Example Objects
    // ---------------------------
    d->actionAddExampleObjects = new QAction("Populate With Example Objects",this);
    connect(d->actionAddExampleObjects,SIGNAL(triggered()),SLOT(addExampleObjects()));
    Command* command = QtilitiesCoreGui::instance()->actionManager()->registerAction("Example.PopulateObserver",d->actionAddExampleObjects,context);

    ActionContainer* file_menu = QtilitiesCoreGui::instance()->actionManager()->menu(MENU_FILE);
    Q_ASSERT(file_menu);

    file_menu->addAction(command,MENU_FILE_EXIT);
    file_menu->addSeperator(MENU_FILE_EXIT);

    // ---------------------------
    // Initialize widget control toolbar and actions
    // ---------------------------
    d->widgets_toolbar = new QToolBar("Widget Toolbar",0);
    d->widgets_toolbar->hide();
    d->widgets_toolbar->setEnabled(false);
    d->widget_show_action = new QAction("Show Widget",0);
    d->widget_show_action->setToolTip("Show Selected Widget");
    d->widget_hide_action = new QAction("Hide Widget",0);
    d->widget_hide_action->setToolTip("Hide Selected Widget");
    d->widget_opacity_slider = new QSlider(Qt::Horizontal,0);
    d->widget_opacity_slider->setToolTip("Change Selected Widget's Opacity");
    d->widget_opacity_slider->setValue(100);
    d->widgets_toolbar->addAction(d->widget_show_action);
    d->widgets_toolbar->addAction(d->widget_hide_action);
    d->widgets_toolbar->addWidget(d->widget_opacity_slider);
    connect(d->widget_show_action,SIGNAL(triggered()),SLOT(handle_actionShowWidget()));
    connect(d->widget_hide_action,SIGNAL(triggered()),SLOT(handle_actionHideWidget()));
    QAbstractSlider* abstract_slider = qobject_cast<QAbstractSlider*> (d->widget_opacity_slider);
    connect(abstract_slider,SIGNAL(valueChanged(int)),SLOT(handle_sliderWidgetOpacity(int)));

    // ---------------------------
    // Create ObserverWidget
    // ---------------------------
    d->observer_widget = new ObserverWidget(ObserverWidget::TreeView);
    d->observer_widget->setGlobalMetaType("Example Observer Meta Type");
    d->scope_widget = new ObjectScopeWidget();
    QDockWidget* scope_dock = new QDockWidget("Object Scope Overview");
    scope_dock->setWidget(d->scope_widget);
    d->observer_widget->addDockWidget(Qt::BottomDockWidgetArea,scope_dock);
    connect(d->observer_widget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),d->scope_widget,SLOT(handle_observerWidgetSelectionChange(QList<QObject*>)));
    connect(d->observer_widget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),SLOT(selectionChanged(QList<QObject*>)));
    connect(d->observer_widget,SIGNAL(addNewItem_triggered(QObject*)),SLOT(addObject_triggered(QObject*)));
    connect(d->observer_widget,SIGNAL(newObserverWidgetCreated(ObserverWidget*)),SLOT(handle_newObserverWidgetCreated(ObserverWidget*)));
    d->observer_widget->setObserverContext(d->top_level_observer);
    d->observer_widget->setAcceptDrops(true);
    d->observer_widget->initialize();
    QList<QAction*> actions = d->observer_widget->actionProvider()->actions();
    d->main_toolbar = d->observer_widget->addToolBar("Context Toolbar");
    d->observer_widget->addToolBar(d->widgets_toolbar);
    d->main_toolbar->addActions(actions);
    d->observer_widget->toggleGrid(false);
    d->observer_widget->layout()->setMargin(0);

    // Create new layout with new widget
    if (layout())
        delete layout();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    layout->addWidget(d->observer_widget);
    layout->setMargin(0);
    d->observer_widget->show();
}

Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::~ObjectManagementModeWidget()
{
    delete ui;
    delete d;
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::addObject_triggered(QObject* object) {
    Observer* observer = qobject_cast<Observer*> (object);
    if (observer) {
        QStringList items;
        items << tr("Observer Subject String") << tr("Simple QWidget") << tr("Child Observer");

        bool ok;
        QString new_item_selection = QInputDialog::getItem(this, tr("What type of object would you like to add?"),tr("Object Types:"), items, 0, false, &ok);
        if (ok && !new_item_selection.isEmpty()) {
            ok = false;
            if (new_item_selection == "Observer Subject String") {
                QString subject_name = QInputDialog::getText(this, tr("Name of object:"), QString("Provide a name for the new object:"), QLineEdit::Normal, "new_object",&ok);
                if (ok && !subject_name.isEmpty()) {
                    ObserverStringSubject* new_subject = new ObserverStringSubject(subject_name);
                    QString subject_category;
                    if (observer->hierarchicalDisplayHint() & Observer::CategorizedHierarchy) {
                        subject_category = QInputDialog::getText(this, tr("Object category:"), QString("Provide a category for the new object, or leave it blank if you want to leave it uncategorized:"), QLineEdit::Normal, "Sample Category",&ok);
                        ObserverProperty object_category(OBJECT_CATEGORY);
                        object_category.setValue(QVariant(subject_category),observer->observerID());
                        QVariant object_category_variant = qVariantFromValue(object_category);
                        new_subject->setProperty(object_category.propertyName(),object_category_variant);
                    }
                    QStringList management_options;
                    management_options << tr("Manual Ownership") << tr("Auto Ownership") << tr("Specific Observer Ownership") << tr("Observer Scope Ownership") << tr("Owned By Subject Ownership");
                    QString item = QInputDialog::getItem(this, tr("How do you want your new object to be managed?"),tr("Ownership Types:"), management_options, 0, false);
                    if (!item.isEmpty()) {
                        if (item == tr("Manual Ownership"))
                            observer->attachSubject(new_subject, Observer::ManualOwnership);
                        else if (item == tr("Auto Ownership"))
                            observer->attachSubject(new_subject, Observer::AutoOwnership);
                        else if (item == tr("Specific Observer Ownership"))
                            observer->attachSubject(new_subject, Observer::SpecificObserverOwnership);
                        else if (item == tr("Observer Scope Ownership"))
                            observer->attachSubject(new_subject, Observer::ObserverScopeOwnership);
                        else if (item == tr("Owned By Subject Ownership"))
                            observer->attachSubject(new_subject, Observer::OwnedBySubjectOwnership);
                    }
                }
            } else if (new_item_selection == "QWidget")  {
                QString subject_name = QInputDialog::getText(this, tr("Name of widget:"), QString("Provide a name for the new widget:"), QLineEdit::Normal, "new_widget",&ok);
                if (ok && !subject_name.isEmpty()) {
                    QWidget* new_subject = new QWidget();
                    QLabel* label_text = new QLabel(new_subject);
                    label_text->setText(QString(tr("Hello, I'm a widget observed by %1. I will delete myself when closed.")).arg(observer->observerName()));
                    label_text->adjustSize();
                    new_subject->resize(label_text->width()+10,label_text->height()+10);
                    new_subject->setWindowTitle(subject_name);
                    new_subject->setObjectName(subject_name);
                    new_subject->setAttribute(Qt::WA_DeleteOnClose, true);
                    observer->attachSubject(new_subject, Observer::SpecificObserverOwnership);
                    new_subject->show();
                }
            } else if (new_item_selection == "Child Observer")  {
                QString subject_name = QInputDialog::getText(this, tr("Name of observer:"), QString("Provide a name for the new observer:"), QLineEdit::Normal, "new_observer",&ok);
                if (ok && !subject_name.isEmpty()) {
                    Observer* new_observer = new Observer(subject_name,"Example Object Manager");
                    // Finaly attach the new observer
                    observer->attachSubject(new_observer, Observer::ObserverScopeOwnership);
                }
            }
        }
    }
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::addExampleObjects() {
    // Add a QObject observer
    Observer* new_observer_1 = new Observer("QObject Manager 1","Example observer which manages QObject instances in a categorized manner.");
    NamingPolicyFilter* naming_filter = new NamingPolicyFilter();
    naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    naming_filter->setUniquenessResolutionPolicy(NamingPolicyFilter::PromptUser);
    naming_filter->setValidityResolutionPolicy(NamingPolicyFilter::PromptUser);
    new_observer_1->installSubjectFilter(naming_filter);
    ActivityPolicyFilter* activity_filter = new ActivityPolicyFilter();
    activity_filter->setActivityPolicy(ActivityPolicyFilter::UniqueActivity);
    new_observer_1->installSubjectFilter(activity_filter);
    new_observer_1->setActivityControlHint(Observer::CheckboxTriggered);
    new_observer_1->setActivityDisplayHint(Observer::CheckboxActivityDisplay);
    new_observer_1->setItemSelectionControlHint(Observer::SelectableItems);
    new_observer_1->setNamingControlHint(Observer::EditableNames);
    new_observer_1->setHierarchicalDisplayHint(Observer::CategorizedHierarchy);

    Observer* selected_observer = 0;
    if (d->observer_widget->selectedObjects().count() == 1)
        selected_observer = qobject_cast<Observer*> (d->observer_widget->selectedObjects().front());

    if (!selected_observer)
        d->top_level_observer->attachSubject(new_observer_1, Observer::ObserverScopeOwnership);
    else
        selected_observer->attachSubject(new_observer_1, Observer::ObserverScopeOwnership);

    // Add example QObjects to "QObject Manager 1"
    ObserverStringSubject* new_subject_1 = new ObserverStringSubject("ObserverStringSubject 1");
    new_observer_1->attachSubject(new_subject_1,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_2 = new ObserverStringSubject("ObserverStringSubject 2");
    new_observer_1->attachSubject(new_subject_2,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_3 = new ObserverStringSubject("ObserverStringSubject 3");
    new_observer_1->attachSubject(new_subject_3,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_4 = new ObserverStringSubject("ObserverStringSubject 4");
    new_observer_1->attachSubject(new_subject_4,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_5 = new ObserverStringSubject("ObserverStringSubject 5");
    new_observer_1->attachSubject(new_subject_5,Observer::SpecificObserverOwnership);

    // Add a QObject observer
    Observer* new_observer_2 = new Observer("QObject Manager 2","Example observer which manages QObject instances");
    naming_filter = new NamingPolicyFilter();
    naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    naming_filter->setUniquenessResolutionPolicy(NamingPolicyFilter::PromptUser);
    naming_filter->setValidityResolutionPolicy(NamingPolicyFilter::PromptUser);
    new_observer_2->installSubjectFilter(naming_filter);
    activity_filter = new ActivityPolicyFilter();
    activity_filter->setActivityPolicy(ActivityPolicyFilter::MultipleActivity);
    new_observer_2->installSubjectFilter(activity_filter);
    new_observer_2->setActivityControlHint(Observer::CheckboxTriggered);
    new_observer_2->setActivityDisplayHint(Observer::CheckboxActivityDisplay);
    new_observer_2->setItemSelectionControlHint(Observer::SelectableItems);
    new_observer_2->setNamingControlHint(Observer::EditableNames);

    if (!selected_observer)
        d->top_level_observer->attachSubject(new_observer_2, Observer::ObserverScopeOwnership);
    else
        selected_observer->attachSubject(new_observer_2, Observer::ObserverScopeOwnership);

    // Add example QObjects to "QObject Manager 2"
    ObserverStringSubject* new_subject_6 = new ObserverStringSubject("ObserverStringSubject 6");
    new_observer_2->attachSubject(new_subject_6,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_7 = new ObserverStringSubject("ObserverStringSubject 7");
    new_observer_2->attachSubject(new_subject_7,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_8 = new ObserverStringSubject("ObserverStringSubject 8");
    new_observer_2->attachSubject(new_subject_8,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_9 = new ObserverStringSubject("ObserverStringSubject 9");
    new_observer_2->attachSubject(new_subject_9,Observer::SpecificObserverOwnership);
    ObserverStringSubject* new_subject_10 = new ObserverStringSubject("ObserverStringSubject 10");
    new_observer_2->attachSubject(new_subject_10,Observer::SpecificObserverOwnership);

    // Add a QWidget observer
    Observer* new_observer_3 = new Observer("QWidget Manager","Example observer which manages QWidget instances");
    naming_filter = new NamingPolicyFilter();
    naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    naming_filter->setUniquenessResolutionPolicy(NamingPolicyFilter::AutoRename);
    naming_filter->setValidityResolutionPolicy(NamingPolicyFilter::AutoRename);
    new_observer_3->installSubjectFilter(naming_filter);
    activity_filter = new ActivityPolicyFilter();
    activity_filter->setActivityPolicy(ActivityPolicyFilter::UniqueActivity);
    new_observer_3->installSubjectFilter(activity_filter);
    new_observer_3->setActivityControlHint(Observer::CheckboxTriggered);
    new_observer_3->setActivityDisplayHint(Observer::CheckboxActivityDisplay);
    new_observer_3->setItemSelectionControlHint(Observer::SelectableItems);
    new_observer_3->setNamingControlHint(Observer::EditableNames);

    if (!selected_observer)
        d->top_level_observer->attachSubject(new_observer_3, Observer::ObserverScopeOwnership);
    else
        selected_observer->attachSubject(new_observer_3, Observer::ObserverScopeOwnership);

    // Add example QWidgets to "QWidget Manager 2"
    QWidget* new_subject_11 = new QWidget();
    new_subject_11->setObjectName("Example QWidget 9");
    new_subject_11->setWindowTitle("Example QWidget 9");
    new_subject_11->setAttribute(Qt::WA_DeleteOnClose, true);
    new_observer_3->attachSubject(new_subject_11, Observer::ObserverScopeOwnership);
    QWidget* new_subject_12 = new QWidget();
    new_subject_12->setObjectName("Example QWidget 8");
    new_subject_12->setWindowTitle("Example QWidget 8");
    new_subject_12->setAttribute(Qt::WA_DeleteOnClose, true);
    new_observer_3->attachSubject(new_subject_12, Observer::ObserverScopeOwnership);
    QWidget* new_subject_13 = new QWidget();
    new_subject_13->setObjectName("Example QWidget 7");
    new_subject_13->setWindowTitle("Example QWidget 7");
    new_subject_13->setAttribute(Qt::WA_DeleteOnClose, true);
    new_observer_3->attachSubject(new_subject_13, Observer::ObserverScopeOwnership);
    QWidget* new_subject_14 = new QWidget();
    new_subject_14->setObjectName("Example QWidget 6");
    new_subject_14->setWindowTitle("Example QWidget 6");
    new_subject_14->setAttribute(Qt::WA_DeleteOnClose, true);
    new_observer_3->attachSubject(new_subject_14, Observer::ObserverScopeOwnership);
    QWidget* new_subject_15 = new QWidget();
    new_subject_15->setObjectName("Example QWidget 5");
    new_subject_15->setWindowTitle("Example QWidget 5");
    new_subject_15->setAttribute(Qt::WA_DeleteOnClose, true);
    new_observer_3->attachSubject(new_subject_15, Observer::ObserverScopeOwnership);

    // Show the observer widgets for all of these
    /*ObserverWidget* new_child_widget = new ObserverWidget(ObserverWidget::TableView);
    new_child_widget->setObserverContext(new_observer_1);
    new_child_widget->initialize();
    new_child_widget->show();

    new_child_widget = new ObserverWidget(ObserverWidget::TableView);
    new_child_widget->setObserverContext(new_observer_2);
    new_child_widget->initialize();
    new_child_widget->show();

    new_child_widget = new ObserverWidget(ObserverWidget::TableView);
    new_child_widget->setObserverContext(new_observer_3);
    new_child_widget->initialize();
    new_child_widget->show();*/
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::selectionChanged(QList<QObject*> new_selection) {
    // Enable / disable widget related toolbar
    if (new_selection.count() == 1) {
        QWidget* widget = qobject_cast<QWidget*> (d->observer_widget->selectedObjects().front());
        if (widget) {
            d->widgets_toolbar->show();
            d->widgets_toolbar->setEnabled(true);
            d->widget_opacity_slider->setValue(widget->windowOpacity()*100);
            if (widget->isVisible()) {
                d->widget_show_action->setEnabled(false);
                d->widget_hide_action->setEnabled(true);
                d->widget_opacity_slider->setEnabled(true);
            } else {
                d->widget_show_action->setEnabled(true);
                d->widget_hide_action->setEnabled(false);
                d->widget_opacity_slider->setEnabled(false);
            }
        } else {
            d->widgets_toolbar->hide();
            d->widgets_toolbar->setEnabled(false);
        }
    }
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::handle_newObserverWidgetCreated(ObserverWidget* new_widget) {
    connect(new_widget,SIGNAL(addNewItem_triggered(QObject*)),SLOT(addObject_triggered(QObject*)));
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::handle_actionShowWidget() {
    if (d->observer_widget->selectedObjects().count() == 1) {
        QWidget* widget = qobject_cast<QWidget*> (d->observer_widget->selectedObjects().front());
        if (widget) {
            widget->show();
            d->widget_show_action->setEnabled(false);
            d->widget_hide_action->setEnabled(true);
            d->widget_opacity_slider->setEnabled(true);
        }
    }
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::handle_actionHideWidget() {
    if (d->observer_widget->selectedObjects().count() == 1) {
        QWidget* widget = qobject_cast<QWidget*> (d->observer_widget->selectedObjects().front());
        if (widget) {
            widget->hide();
            d->widget_show_action->setEnabled(true);
            d->widget_hide_action->setEnabled(false);
            d->widget_opacity_slider->setEnabled(false);
        }
    }
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::handle_sliderWidgetOpacity(int value) {
    if (d->observer_widget->selectedObjects().count() == 1) {
        QWidget* widget = qobject_cast<QWidget*> (d->observer_widget->selectedObjects().front());
        if (widget) {
            double opacity = ((double) value)/((double) 100);
            widget->setWindowOpacity(opacity);
        }
    }
}


QStringList Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::factoryTags() const {
    QStringList tags;
    tags << "Example IFactory";
    return tags;
}

QObject* Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::createInstance(const IFactoryData& ifactory_data) {
    if (ifactory_data.d_factory_tag == QString("Example IFactory")) {
        QObject* obj = d->string_subject_factory.createInstance(ifactory_data.d_instance_tag);
        if (obj) {
            return obj;
        }
    }
    return 0;
}

