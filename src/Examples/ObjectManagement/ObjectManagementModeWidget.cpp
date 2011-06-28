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

#include <QtGui>
#include <QSettings>
#include <QPointer>

#include <QtilitiesProjectManagement>
using namespace QtilitiesProjectManagement;

struct Qtilities::Examples::ObjectManagement::ObjectManagementModeWidgetData {
     ObjectManagementModeWidgetData() : top_level_node(0),
     observer_widget(0),
     scope_widget(0),
     project_item(0) { }

    QPointer<TreeNode> top_level_node;
    ObserverWidget* observer_widget;
    ObjectScopeWidget* scope_widget;

    QAction *actionAddExampleObjects;
    QAction *dot_file_action;
    QAction *exitAct;

    // Example toolbar and actions to control widgets in the tree
    QToolBar* widgets_toolbar;
    QAction* widget_show_action;
    QAction* widget_hide_action;
    QSlider* widget_opacity_slider;

    ObserverProjectItemWrapper* project_item;
};

Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::ObjectManagementModeWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ObjectManagementModeWidget)
{
    ui->setupUi(this);

    d = new ObjectManagementModeWidgetData;
    d->top_level_node= new TreeNode("Root");
    d->top_level_node->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    d->top_level_node->displayHints()->setDisplayFlagsHint(ObserverHints::AllDisplayFlagHint);
    d->top_level_node->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);

    OBJECT_MANAGER->registerObject(d->top_level_node,QtilitiesCategory("Example Objects"));

    // ---------------------------
    // Factory and Project Item Stuff
    // ---------------------------
    d->project_item = new ObserverProjectItemWrapper(d->top_level_node,this);
    OBJECT_MANAGER->registerObject(d->project_item,QtilitiesCategory("Core::Project Items (IProjectItem)","::"));

    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

    // ---------------------------
    // Add Example Objects
    // ---------------------------
    ActionContainer* file_menu = ACTION_MANAGER->menu(qti_action_FILE);
    Q_ASSERT(file_menu);

    d->actionAddExampleObjects = new QAction("Add Example Objects To Selection",this);
    connect(d->actionAddExampleObjects,SIGNAL(triggered()),SLOT(addExampleObjects()));
    Command* command = ACTION_MANAGER->registerAction("Example.PopulateObserver",d->actionAddExampleObjects,context);
    command->setCategory(QtilitiesCategory(QApplication::applicationName()));
    file_menu->addAction(command,qti_action_FILE_EXIT);

    d->dot_file_action = new QAction("Create Dot Graph",this);
    connect(d->dot_file_action,SIGNAL(triggered()),SLOT(createDotFile()));
    command = ACTION_MANAGER->registerAction("Example.CreateDotFile",d->dot_file_action,context);
    command->setCategory(QtilitiesCategory(QApplication::applicationName()));
    file_menu->addAction(command,qti_action_FILE_EXIT);
    file_menu->addSeperator(qti_action_FILE_EXIT);

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
    d->observer_widget = new ObserverWidget(Qtilities::TreeView);
    d->observer_widget->setGlobalMetaType("Example Observer Meta Type");
    d->observer_widget->setAcceptDrops(true);
    d->scope_widget = new ObjectScopeWidget();
    QDockWidget* scope_dock = new QDockWidget("Object Scope Overview");
    scope_dock->setWidget(d->scope_widget);
    d->observer_widget->addDockWidget(Qt::BottomDockWidgetArea,scope_dock);
    connect(d->observer_widget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),d->scope_widget,SLOT(handle_observerWidgetSelectionChange(QList<QObject*>)));
    connect(d->observer_widget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),SLOT(selectionChanged(QList<QObject*>)));
    connect(d->observer_widget,SIGNAL(addActionNewItem_triggered(QObject*)),SLOT(addObject_triggered(QObject*)));
    connect(d->observer_widget,SIGNAL(newObserverWidgetCreated(ObserverWidget*)),SLOT(handle_newObserverWidgetCreated(ObserverWidget*)));
    d->observer_widget->setObserverContext(d->top_level_node);
    d->observer_widget->addToolBar(d->widgets_toolbar);
    d->observer_widget->setAcceptDrops(true);
    d->observer_widget->initialize();
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
        items << tr("New Item") << tr("New Node") << tr("QWidget");

        bool ok;
        QString new_item_selection = QInputDialog::getItem(this, tr("What type of object would you like to add?"),tr("Object Types:"), items, 0, false, &ok);
        if (ok && !new_item_selection.isEmpty()) {
            ok = false;
            if (new_item_selection == "New Item") {
                QString subject_name = QInputDialog::getText(this, tr("Name of object:"), QString("Provide a name for the new object:"), QLineEdit::Normal, "new_object",&ok);
                if (ok && !subject_name.isEmpty()) {
                    TreeItem* new_item = new TreeItem(subject_name);
                    QString subject_category;
                    if (observer->displayHints()) {
                        if (observer->displayHints()->hierarchicalDisplayHint() & ObserverHints::CategorizedHierarchy) {
                            subject_category = QInputDialog::getText(this, tr("Object category:"), QString("Provide a category for the new object, or leave it blank if you want to leave it uncategorized:"), QLineEdit::Normal, "Sample Category",&ok);
                            if (ok)
                                new_item->setCategory(QtilitiesCategory(subject_category),observer->observerID());
                        }
                    }
                    QStringList management_options;
                    management_options << tr("Manual Ownership") << tr("Auto Ownership") << tr("Specific Observer Ownership") << tr("Observer Scope Ownership") << tr("Owned By Subject Ownership");
                    QString item = QInputDialog::getItem(this, tr("How do you want your new object to be managed?"),tr("Ownership Types:"), management_options, 0, false);
                    if (!item.isEmpty()) {
                        if (item == tr("Manual Ownership"))
                            observer->attachSubject(new_item, Observer::ManualOwnership);
                        else if (item == tr("Auto Ownership"))
                            observer->attachSubject(new_item, Observer::AutoOwnership);
                        else if (item == tr("Specific Observer Ownership"))
                            observer->attachSubject(new_item, Observer::SpecificObserverOwnership);
                        else if (item == tr("Observer Scope Ownership"))
                            observer->attachSubject(new_item, Observer::ObserverScopeOwnership);
                        else if (item == tr("Owned By Subject Ownership"))
                            observer->attachSubject(new_item, Observer::OwnedBySubjectOwnership);
                        OBJECT_MANAGER->registerObject(new_item,QtilitiesCategory("Example Objects"));
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
                    OBJECT_MANAGER->registerObject(new_subject,QtilitiesCategory("Example Objects"));
                    new_subject->show();
                }
            } else if (new_item_selection == "New Node")  {
                QString subject_name = QInputDialog::getText(this, tr("Name of Node:"), QString("Provide a name for the new node:"), QLineEdit::Normal, "New Node",&ok);
                if (ok && !subject_name.isEmpty()) {
                    TreeNode* new_node = new TreeNode(subject_name);
                    // Finaly attach the new observer
                    observer->attachSubject(new_node, Observer::ObserverScopeOwnership);
                    OBJECT_MANAGER->registerObject(new_node,QtilitiesCategory("Example Objects"));
                    new_node->displayHints()->setActionHints(ObserverHints::ActionAllHints);
                    new_node->displayHints()->setDisplayFlagsHint(ObserverHints::AllDisplayFlagHint);
                    new_node->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);
                }
            }
        }
    }
}

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::addExampleObjects() {
    // Determine where the new nodes and items must be attached:
    Observer* selected_observer = 0;
    if (d->observer_widget->selectedObjects().count() == 1)
        selected_observer = qobject_cast<Observer*> (d->observer_widget->selectedObjects().front());

    // Create a categorized node:
    TreeNode* nodeA = new TreeNode("Node A");
    nodeA->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames,NamingPolicyFilter::AutoRename);
    nodeA->displayHints()->setItemSelectionControlHint(ObserverHints::SelectableItems);
    nodeA->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    nodeA->displayHints()->setDisplayFlagsHint(ObserverHints::AllDisplayFlagHint);
    nodeA->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    nodeA->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);
    nodeA->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    for (int i = 0; i < 5; i++)
        nodeA->addItem(QString("Item %1").arg(i));
    if (!selected_observer)
        d->top_level_node->attachSubject(nodeA, Observer::ObserverScopeOwnership);
    else
        selected_observer->attachSubject(nodeA, Observer::ObserverScopeOwnership);

    // Create a second node:
    TreeNode* nodeB = new TreeNode("Node B");
    nodeB->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames,NamingPolicyFilter::AutoRename);
    nodeB->displayHints()->setItemSelectionControlHint(ObserverHints::SelectableItems);
    nodeB->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    nodeB->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    nodeB->displayHints()->setDisplayFlagsHint(ObserverHints::AllDisplayFlagHint);
    nodeB->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);
    for (int i = 0; i < 5; i++)
        nodeB->addItem(QString("Item %1").arg(i));
    if (!selected_observer)
        d->top_level_node->attachSubject(nodeB, Observer::ObserverScopeOwnership);
    else
        selected_observer->attachSubject(nodeB, Observer::ObserverScopeOwnership);

    // Create a node with some QWidgets:
    TreeNode* nodeC = new TreeNode("Node C");
    nodeC->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames,NamingPolicyFilter::AutoRename);
    nodeC->displayHints()->setItemSelectionControlHint(ObserverHints::SelectableItems);
    nodeC->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    nodeC->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    nodeC->displayHints()->setDisplayFlagsHint(ObserverHints::AllDisplayFlagHint);
    nodeC->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);
    for (int i = 0; i < 5; i++) {
        QWidget* widget = new QWidget();
        QLabel* label_text = new QLabel(widget);
        label_text->setText(QString(tr("Hello, I'm a widget. I will delete myself when closed.")));
        label_text->adjustSize();
        widget->resize(label_text->width()+10,label_text->height()+10);
        widget->setObjectName(QString("Widget %1").arg(i));
        widget->setWindowTitle(QString("Widget %1").arg(i));
        widget->setAttribute(Qt::WA_DeleteOnClose, true);
        nodeC->attachSubject(widget, Observer::ObserverScopeOwnership);
    }
    if (!selected_observer)
        d->top_level_node->attachSubject(nodeC, Observer::ObserverScopeOwnership);
    else
        selected_observer->attachSubject(nodeC, Observer::ObserverScopeOwnership);

    d->top_level_node->refreshViewsLayout();
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
    connect(new_widget,SIGNAL(addActionNewItem_triggered(QObject*)),SLOT(addObject_triggered(QObject*)));
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

void Qtilities::Examples::ObjectManagement::ObjectManagementModeWidget::createDotFile() {
    ObserverDotWriter dotGraph(d->top_level_node);
    dotGraph.addNodeAttribute(d->top_level_node->treeChildren().front(),"color","red");
    dotGraph.addEdgeAttribute(d->top_level_node,d->top_level_node->treeChildren().front(),"label","\"My label\"");
    dotGraph.addEdgeAttribute(d->top_level_node,d->top_level_node->treeChildren().front(),"style","bold");
    dotGraph.addEdgeAttribute(d->top_level_node,d->top_level_node->treeChildren().at(1),"color","red");
    dotGraph.addGraphAttribute("label","Graph Title");
    dotGraph.generateDotScript();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Dot Input File"),QApplication::applicationDirPath(),tr("Dot Input Files (*.gv)"));
    if (!fileName.isEmpty()) {
        dotGraph.saveToFile(fileName);

        CodeEditorWidget* editor_widget = new CodeEditorWidget;
        editor_widget->setAttribute(Qt::WA_DeleteOnClose,true);
        editor_widget->setAttribute(Qt::WA_QuitOnClose,true);
        editor_widget->loadFile(fileName);
        editor_widget->show();
    }
}

