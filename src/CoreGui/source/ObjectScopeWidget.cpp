/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#include "ObjectScopeWidget.h"
#include "ui_ObjectScopeWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"
#include "ActionProvider.h"

#include <SubjectTypeFilter.h>
#include <QtilitiesCoreConstants.h>
#include <Observer.h>
#include <ObserverHints.h>
#include <QtilitiesPropertyChangeEvent.h>

#include <QApplication>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QMessageBox>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QWidget, ObjectScopeWidget> ObjectScopeWidget::factory;
    }
}

struct Qtilities::CoreGui::ObjectScopeWidgetPrivateData {
    ObjectScopeWidgetPrivateData() : actionDetachToSelection(0),
        actionRemoveContext(0),
        obj(0),
        action_provider(0) {}

    QAction* actionDetachToSelection;
    QAction* actionRemoveContext;
    QPointer<QObject> obj;
    ActionProvider* action_provider;
    //! The global meta type string used for this widget.
    QString global_meta_type;

};

Qtilities::CoreGui::ObjectScopeWidget::ObjectScopeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectScopeWidget)
{
    ui->setupUi(this);
    connect(ui->observerTable,SIGNAL(itemClicked(QTableWidgetItem*)),SLOT(handle_currentItemChanged(QTableWidgetItem *)));

    d = new ObjectScopeWidgetPrivateData;

    ui->observerTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->observerTable->setSortingEnabled(true);
    ui->observerTable->setToolTip(tr("A list of contexts to which the selected object is attached."));

    // Assign a default meta type for this widget:
    // We construct each action and then register it
    QString context_string = "ObjectScopeWidget";
    int count = 0;
    context_string.append(QString("%1").arg(count));
    while (CONTEXT_MANAGER->hasContext(context_string)) {
        QString count_string = QString("%1").arg(count);
        context_string.chop(count_string.length());
        ++count;
        context_string.append(QString("%1").arg(count));
    }
    CONTEXT_MANAGER->registerContext(context_string);
    d->global_meta_type = context_string;
    setObjectName(context_string);

    // Create actions only after global meta type was set.
    constructActions();
}

Qtilities::CoreGui::ObjectScopeWidget::~ObjectScopeWidget()
{
    delete ui;
    delete d;
}

void Qtilities::CoreGui::ObjectScopeWidget::handle_observerWidgetSelectionChange(QList<QObject*> objects) {
    if (objects.count() == 1) {
        setObject(objects.front());
    }
}

void Qtilities::CoreGui::ObjectScopeWidget::setObject(QObject* obj) {
    if (!obj) {
        handleObjectDestroyed();
        return;
    }

    if (d->obj)
        d->obj->disconnect(this);

    d->obj = obj;
    d->obj->installEventFilter(this);

    connect(d->obj,SIGNAL(destroyed(QObject*)),SLOT(handleObjectDestroyed()));

    // For this widget we are interested in dynamic observer property changes in ALL the observers
    // which are observing this object
    MultiContextProperty context_map_prop = ObjectManager::getMultiContextProperty(d->obj,qti_prop_OBSERVER_MAP);
    for (int i = 0; i < context_map_prop.contextMap().count(); ++i) {
        Observer* observer = OBJECT_MANAGER->observerReference(context_map_prop.contextMap().keys().at(i));
        if (observer) {
            connect(observer,SIGNAL(destroyed()),SLOT(updateContents()),Qt::UniqueConnection);
        }
    }

    updateContents();
    refreshActions();
}

void Qtilities::CoreGui::ObjectScopeWidget::setObject(QPointer<QObject> obj) {
    QObject* object = obj;
    setObject(object);
}

void Qtilities::CoreGui::ObjectScopeWidget::setObject(QList<QObject*> objects) {
    if (objects.count() == 1)
        setObject(objects.front());
}

void Qtilities::CoreGui::ObjectScopeWidget::setObject(QList<QPointer<QObject> > objects) {
    if (objects.count() == 1)
        setObject(objects.front());
}

void Qtilities::CoreGui::ObjectScopeWidget::handleObjectDestroyed() {
    ui->observerTable->clearContents();
    ui->txtName->setText(tr("No Scope Information."));
    ui->txtObserverLimit->setText(tr("No Scope Information"));
    ui->txtOwnership->setText(tr("No Scope Information"));
    if (d->obj)  {
        d->obj->disconnect(this);
        d->obj->removeEventFilter(this);
    }

    d->obj = 0;
    return;
}

void Qtilities::CoreGui::ObjectScopeWidget::setNameVisible(bool visible) {
    ui->widgetName->setVisible(visible);
}

bool Qtilities::CoreGui::ObjectScopeWidget::eventFilter(QObject *object, QEvent *event) {
    if (object == d->obj && event->type() == QEvent::User) {
        QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
        if (qtilities_event) {
            updateContents();
        }
    } else if (object == ui->observerTable && event->type() == QEvent::FocusIn) {
        refreshActions();
        CONTEXT_MANAGER->setNewContext(contextString(),true);
    }
    return false;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::ObjectScopeWidget::actionProvider() {
    return d->action_provider;
}

QString Qtilities::CoreGui::ObjectScopeWidget::contextString() const {
    return d->global_meta_type;
}

QString Qtilities::CoreGui::ObjectScopeWidget::contextHelpId() const {
    return QString();
}

void Qtilities::CoreGui::ObjectScopeWidget::updateContents() {
    if (!d->obj)
        return;

    int observer_count = -1;
    int observer_limit = -2;
    int ownership = -1;
    ui->txtName->setText(d->obj->objectName());

    // Observer Count
    MultiContextProperty context_map_prop = ObjectManager::getMultiContextProperty(d->obj,qti_prop_OBSERVER_MAP);
    if (context_map_prop.isValid())
        observer_count = context_map_prop.contextMap().count();

    // Observer Limit
    SharedProperty shared_property = ObjectManager::getSharedProperty(d->obj,qti_prop_OBSERVER_LIMIT);
    if (shared_property.isValid())
        observer_limit = shared_property.value().toInt();

    if (observer_limit == -1)
        ui->txtObserverLimit->setText(tr("Unlimited"));
    else if (observer_limit == -2)
        ui->txtObserverLimit->setText(tr("Unlimited"));
    else
        ui->txtObserverLimit->setText(QString("%1").arg(observer_limit));

    // Ownership
    shared_property = ObjectManager::getSharedProperty(d->obj,qti_prop_OWNERSHIP);
    if (shared_property.isValid())
        ownership = shared_property.value().toInt();

    if (ownership == -1)
        ui->txtOwnership->setText(tr("Manual Ownership"));
    else if (ownership == Observer::ManualOwnership)
        ui->txtOwnership->setText(tr("Manual Ownership"));
    else if (ownership == Observer::SpecificObserverOwnership)
        ui->txtOwnership->setText(tr("Specific Observer Ownership"));
    else if (ownership == Observer::ObserverScopeOwnership)
        ui->txtOwnership->setText(tr("Observer Scope Ownership"));

    ui->observerTable->clearContents();
    ui->observerTable->setRowCount(observer_count);
    QString tooltip_string;
    bool has_instance_names = false;

    for (int i = 0; i < observer_count; ++i) {
        int id = context_map_prop.contextMap().keys().at(i);
        Observer* observer = OBJECT_MANAGER->observerReference(id);
        if (!observer)  {
            LOG_ERROR("Object scope widget: Found invalid observer ID on object: " + d->obj->objectName());
            continue;
        }

        // Observer Name
        QTableWidgetItem *nameItem = new QTableWidgetItem(observer->observerName(), id);
        ui->observerTable->setItem(i, 0, nameItem);
        if (ownership == Observer::SpecificObserverOwnership) {
            QVariant observer_parent = observer->getMultiContextPropertyValue(d->obj,qti_prop_PARENT_ID);
            if (observer_parent.isValid() && (observer_parent.toInt() == id)) {
                QTableWidgetItem *ownerItem = new QTableWidgetItem("", id);
                ownerItem->setIcon(QIcon(qti_icon_SUCCESS_16x16));
                ownerItem->setToolTip(tr("This context owns the selected object (Manages its lifetime)."));
                ui->observerTable->setItem(i, OwnerColumn, ownerItem);
            }
        }
        tooltip_string.append(QString(tr("<b>Context Name</b>: %1")).arg(observer->observerName()));
        // Context Description
        tooltip_string.append(QString(tr("<br><b>Context Description</b>: %1")).arg(observer->observerDescription()));
        // Context ID
        tooltip_string.append(QString(tr("<br><b>Context ID</b>: %1")).arg(observer->observerID()));
        // Context Category
        MultiContextProperty category_prop = ObjectManager::getMultiContextProperty(d->obj,qti_prop_CATEGORY_MAP);
        QString category_string;
        if (category_prop.isValid()) {
            if (category_prop.contextMap().contains(observer->observerID()))
                category_string = category_prop.value(observer->observerID()).toString();
            else
                category_string = tr("None");
        } else {
            category_string = tr("None");
        }
        tooltip_string.append(QString(tr("<br><b>Context Category</b>: %1")).arg(category_string));

        // Attributes
        tooltip_string.append(tr("<br><br><b>Attributes: </b> "));
        // Attribute: Activity
        QVariant activity = observer->getMultiContextPropertyValue(d->obj,qti_prop_ACTIVITY_MAP);
        if (activity.isValid() && activity.toBool())
            tooltip_string.append(tr("Active, "));

        // Attribute: Object Name Controller
        QVariant name_manager_id = observer->getMultiContextPropertyValue(d->obj,qti_prop_NAME_MANAGER_ID);
        if (name_manager_id.isValid() && (name_manager_id.toInt() == observer->observerID()))
            tooltip_string.append(tr("Manages Name, "));

        // Attribute: Uses Instance Name
        MultiContextProperty instance_names = ObjectManager::getMultiContextProperty(d->obj,qti_prop_ALIAS_MAP);
        if (instance_names.isValid() && instance_names.hasContext(observer->observerID())) {
            tooltip_string.append(QString(tr("Uses Instance Name (\"%1\"), ").arg(instance_names.value(observer->observerID()).toString())));
            has_instance_names = true;
            QTableWidgetItem *aliasItem = new QTableWidgetItem("", id);
            aliasItem->setIcon(QIcon(qti_icon_SUCCESS_16x16));
            aliasItem->setToolTip(tr("This context uses an instance name (alias) for the selected object."));
            ui->observerTable->setItem(i, UsesInstanceNameColumn, aliasItem);
        }

        // Attribute: Ownership
        int parent_id = ObjectManager::getSharedProperty(d->obj,qti_prop_PARENT_ID).value().toInt();
        if (parent_id == observer->observerID())
            tooltip_string.append(tr("Owner"));

        // Subject Filters
        QString filter_name_string;
        for (int r = 0; r < observer->subjectFilters().count(); r++) {
            if (r == 0)
                filter_name_string.append(observer->subjectFilters().at(r)->filterName());
            else
                filter_name_string.append(", " + observer->subjectFilters().at(r)->filterName());
        }
        tooltip_string.append(QString(tr("<br><b>Filters</b>: %1")).arg(filter_name_string));
        nameItem->setToolTip(tooltip_string);
        tooltip_string.clear();   
    }

    ui->observerTable->resizeRowsToContents();
    ui->observerTable->setColumnCount(OwnerColumn+1);
    ui->observerTable->showColumn(NameColumn);
    ui->observerTable->showColumn(UsesInstanceNameColumn);
    ui->observerTable->setColumnWidth(UsesInstanceNameColumn,40);
    ui->observerTable->showColumn(OwnerColumn);
    ui->observerTable->setColumnWidth(OwnerColumn,40);
    QStringList headers;
    headers << tr("Context List") << tr("Alias") << tr("Owner");
    ui->observerTable->setHorizontalHeaderLabels(headers);
    if (ownership == Observer::SpecificObserverOwnership && has_instance_names) {
        #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::ResizeToContents);
        #else
        ui->observerTable->horizontalHeader()->setSectionResizeMode(NameColumn,QHeaderView::ResizeToContents);
        #endif
        //ui->observerTable->horizontalHeader()->setResizeMode(OwnerColumn,QHeaderView::Stretch);
    } else if (ownership == Observer::SpecificObserverOwnership && !has_instance_names) {
        ui->observerTable->hideColumn(UsesInstanceNameColumn);
        #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::ResizeToContents);
        #else
        ui->observerTable->horizontalHeader()->setSectionResizeMode(NameColumn,QHeaderView::ResizeToContents);
        #endif
        //ui->observerTable->horizontalHeader()->setResizeMode(OwnerColumn,QHeaderView::Stretch);
    } else if (ownership != Observer::SpecificObserverOwnership && has_instance_names) {
        ui->observerTable->hideColumn(OwnerColumn);
        #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::ResizeToContents);
        #else
        ui->observerTable->horizontalHeader()->setSectionResizeMode(NameColumn,QHeaderView::ResizeToContents);
        #endif
        //ui->observerTable->horizontalHeader()->setResizeMode(UsesInstanceNameColumn,QHeaderView::Stretch);
    } else {
        ui->observerTable->hideColumn(UsesInstanceNameColumn);
        ui->observerTable->hideColumn(OwnerColumn);
        //ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::Stretch);
    }
}


void Qtilities::CoreGui::ObjectScopeWidget::constructActions() {
    if (d->action_provider)
        return;

    d->action_provider = new ActionProvider(this);

    int context_id = CONTEXT_MANAGER->registerContext(d->global_meta_type);
    QList<int> context;
    context.push_front(context_id);

    // ---------------------------
    // Remove Context
    // ---------------------------
    d->actionRemoveContext = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),"Remove Selected",this);
    d->action_provider->addAction(d->actionRemoveContext);
    d->actionRemoveContext->setEnabled(false);
    connect(d->actionRemoveContext,SIGNAL(triggered()),SLOT(handle_actionRemoveContext_triggered()));
    Command* command = ACTION_MANAGER->registerAction(qti_action_SELECTION_SCOPE_REMOVE_SELECTED,d->actionRemoveContext,context);
    command->setCategory(QtilitiesCategory("Item Scope"));
    // ---------------------------
    // Detach In Selected Context (Creates copy in selected context which is detached from the rest of the contexts)
    // ---------------------------
    d->actionDetachToSelection = new QAction(QIcon(qti_icon_REMOVE_ALL_16x16),"Romove Others",this);
    d->action_provider->addAction(d->actionDetachToSelection);
    d->actionDetachToSelection->setEnabled(false);
    connect(d->actionDetachToSelection,SIGNAL(triggered()),SLOT(handle_actionDetachToSelection_triggered()));
    ACTION_MANAGER->registerAction(qti_action_SELECTION_SCOPE_REMOVE_OTHERS,d->actionDetachToSelection,context);
    command->setCategory(QtilitiesCategory("Item Scope"));

    QAction* sep1 = new QAction(0);
    sep1->setSeparator(true);
    ui->observerTable->addAction(sep1);
    ui->observerTable->addAction(d->actionRemoveContext);
    ui->observerTable->addAction(d->actionDetachToSelection);
    QAction* sep2 = new QAction(0);
    sep2->setSeparator(true);
    ui->observerTable->addAction(sep2);
    ui->observerTable->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void Qtilities::CoreGui::ObjectScopeWidget::refreshActions() {
    if (ui->observerTable->selectedItems().count() > 0) {
        int id = ui->observerTable->currentItem()->type();
        Observer* observer = OBJECT_MANAGER->observerReference(id);
        if (observer) {
            if (observer->displayHints()) {
                if (ui->observerTable->rowCount() > 1)
                    d->actionDetachToSelection->setEnabled(true);
                else
                    d->actionDetachToSelection->setEnabled(false);

                if (observer->displayHints()->actionHints() & ObserverHints::ActionRemoveItem)
                    d->actionRemoveContext->setEnabled(true);
                else
                    d->actionRemoveContext->setEnabled(false);
            }
        } else {
            d->actionDetachToSelection->setEnabled(false);
            d->actionRemoveContext->setEnabled(false);
        }
    } else {
        d->actionDetachToSelection->setEnabled(false);
        d->actionRemoveContext->setEnabled(false);
    }

    // Check if the observer limit is reached, if so disable the add context action.
    if (!d->obj) {
        d->actionDetachToSelection->setEnabled(false);
        d->actionRemoveContext->setEnabled(false);
    }
}

void Qtilities::CoreGui::ObjectScopeWidget::handle_actionRemoveContext_triggered() {
    // Get the current selected observer
    int id = ui->observerTable->currentItem()->type();
    Observer* observer = OBJECT_MANAGER->observerReference(id);
    if (!observer)
        return;

    if (observer->displayHints()) {
        // Check if a detach operation is supported by this observer.
        if (observer->displayHints()->actionHints() & ObserverHints::ActionRemoveItem)
            observer->detachSubject(d->obj);
        else {
            QMessageBox msgBox;
            msgBox.setText(tr("The selected context does not support removing of child items. The operation cannot continue."));
            msgBox.exec();
        }
    }
}

void Qtilities::CoreGui::ObjectScopeWidget::handle_actionDetachToSelection_triggered() {
    // Get the ids of all unselected items
    QList<int> other_ids;
    int selected_id = ui->observerTable->currentItem()->type();
    for (int i = 0; i < ui->observerTable->rowCount(); ++i) {
        if (selected_id != ui->observerTable->item(i,0)->type())
            other_ids << ui->observerTable->item(i,0)->type();
    }

    QStringList unsupported_items;
    for (int i = 0; i < other_ids.count(); ++i) {
        Observer* observer = OBJECT_MANAGER->observerReference(other_ids.at(i));
        if (!observer)
            break;

        if (observer->displayHints()) {
            if (!(observer->displayHints()->actionHints() & ObserverHints::ActionRemoveItem)) {
                unsupported_items << observer->observerName();
            }
        } else {
            unsupported_items << observer->observerName();
        }
    }

    if (unsupported_items.count() > 0)  {
        QMessageBox msgBox;
        msgBox.setText(tr("All other contexts does not support removing of child items."));
        msgBox.setInformativeText(tr("Do you want to continue and remove the contexts which does support this?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        switch (ret) {
           case QMessageBox::No:
               return;
           default:
               break;
        }
    }

    for (int i = 0; i < other_ids.count(); ++i) {
        Observer* observer = OBJECT_MANAGER->observerReference(other_ids.at(i));
        if (!observer)
            break;

        if (observer->displayHints()) {
            if (observer->displayHints()->actionHints() & ObserverHints::ActionRemoveItem) {
                observer->detachSubject(d->obj);
            }
        }
    }
}

void Qtilities::CoreGui::ObjectScopeWidget::handle_currentItemChanged(QTableWidgetItem * current) {
    Q_UNUSED(current)

    refreshActions();
}

void Qtilities::CoreGui::ObjectScopeWidget::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
