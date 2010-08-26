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

QPointer<Qtilities::CoreGui::ObjectScopeWidget> Qtilities::CoreGui::ObjectScopeWidget::currentWidget;
QPointer<Qtilities::CoreGui::ObjectScopeWidget> Qtilities::CoreGui::ObjectScopeWidget::actionContainerWidget;

struct Qtilities::CoreGui::ObjectScopeWidgetData {
    ObjectScopeWidgetData() : actionAddContext(0),
    actionDetachToSelection(0),
    actionDuplicateInScope(0),
    actionRemoveContext(0),
    obj(0),
    action_provider(0) {}

    QAction* actionAddContext;
    QAction* actionDetachToSelection;
    QAction* actionDuplicateInScope;
    QAction* actionRemoveContext;
    QObject* obj;
    ActionProvider* action_provider;
};

Qtilities::CoreGui::ObjectScopeWidget::ObjectScopeWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ObjectScopeWidget)
{
    m_ui->setupUi(this);
    connect(m_ui->observerTable,SIGNAL(itemClicked(QTableWidgetItem*)),SLOT(handle_currentItemChanged(QTableWidgetItem *)));

    d = new ObjectScopeWidgetData;
    d->obj = 0;

    setAttribute(Qt::WA_DeleteOnClose, true);
    Qt::WindowFlags window_flags = windowFlags();
    window_flags |= Qt::Tool;
    setWindowFlags(window_flags);
    setWindowIcon(QIcon(ICON_OBSERVER));
    m_ui->observerTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_ui->observerTable->setSortingEnabled(true);
    m_ui->observerTable->setToolTip(tr("A list of contexts to which the selected object is attached."));
    constructActions();
    currentWidget = this;
}

Qtilities::CoreGui::ObjectScopeWidget::~ObjectScopeWidget()
{
    delete m_ui;
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

    d->obj = obj;
    d->obj->installEventFilter(this);

    connect(d->obj,SIGNAL(destroyed(QObject*)),SLOT(handleObjectDestroyed()));

    // For this widget we are interested in dynamic observer property changes in ALL the observers
    // which are observing this object
    ObserverProperty observer_map_prop = getObserverProperty(OBSERVER_SUBJECT_IDS);
    for (int i = 0; i < observer_map_prop.observerMap().count(); i++) {
        Observer* observer = OBJECT_MANAGER->observerReference(observer_map_prop.observerMap().keys().at(i));
        if (observer) {
            connect(observer,SIGNAL(destroyed()),SLOT(updateContents()));
        }
    }

    updateContents();
    refreshActions();
}

void Qtilities::CoreGui::ObjectScopeWidget::setObject(QList<QObject*> objects) {
    if (objects.count() == 1)
        setObject(objects.front());
}

void Qtilities::CoreGui::ObjectScopeWidget::handleObjectDestroyed() {
    m_ui->observerTable->clearContents();
    m_ui->txtName->setText(tr("No Scope Information."));
    m_ui->txtObserverLimit->setText(tr("No Scope Information"));
    m_ui->txtOwnership->setText(tr("No Scope Information"));
    if (d->obj)  {
        d->obj->disconnect(this);
        d->obj->removeEventFilter(this);
    }

    d->obj = 0;
    return;
}

void Qtilities::CoreGui::ObjectScopeWidget::setNameVisible(bool visible) {
    m_ui->widgetName->setVisible(visible);
}

bool Qtilities::CoreGui::ObjectScopeWidget::eventFilter(QObject *object, QEvent *event) {
    if (object == d->obj && event->type() == QEvent::User) {
        QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
        if (qtilities_event) {
            updateContents();
        }
    } else if (object == m_ui->observerTable && event->type() == QEvent::FocusIn) {
        currentWidget = this;
    }
    return false;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::ObjectScopeWidget::actionProvider() {
    if (!actionContainerWidget)
        return 0;

    return actionContainerWidget->d->action_provider;
}

void Qtilities::CoreGui::ObjectScopeWidget::updateContents() {
    if (!d->obj)
        return;

    int observer_count = -1;
    int observer_limit = -2;
    int ownership = -1;
    m_ui->txtName->setText(d->obj->objectName());

    // Observer Count
    ObserverProperty observer_map_prop = getObserverProperty(OBSERVER_SUBJECT_IDS);
    if (observer_map_prop.isValid())
        observer_count = observer_map_prop.observerMap().count();

    // Observer Limit
    SharedObserverProperty shared_property = getSharedProperty(OBSERVER_LIMIT);
    if (shared_property.isValid())
        observer_limit = shared_property.value().toInt();

    if (observer_limit == -1)
        m_ui->txtObserverLimit->setText(tr("Unlimited"));
    else if (observer_limit == -2)
        m_ui->txtObserverLimit->setText(tr("Unlimited"));
    else
        m_ui->txtObserverLimit->setText(QString("%1").arg(observer_limit));

    // Ownership
    shared_property = getSharedProperty(OWNERSHIP);
    if (shared_property.isValid())
        ownership = shared_property.value().toInt();

    if (ownership == -1)
        m_ui->txtOwnership->setText(tr("Manual Ownership"));
    else if (ownership == Observer::ManualOwnership)
        m_ui->txtOwnership->setText(tr("Manual Ownership"));
    else if (ownership == Observer::SpecificObserverOwnership)
        m_ui->txtOwnership->setText(tr("Specific Observer Ownership"));
    else if (ownership == Observer::ObserverScopeOwnership)
        m_ui->txtOwnership->setText(tr("Observer Scope Ownership"));

    m_ui->observerTable->clearContents();
    m_ui->observerTable->setRowCount(observer_count);
    QString tooltip_string;
    bool has_instance_names = false;

    for (int i = 0; i < observer_count; i++) {
        int id = observer_map_prop.observerMap().keys().at(i);
        Observer* observer = OBJECT_MANAGER->observerReference(id);
        Q_ASSERT(observer);

        // Observer Name
        QTableWidgetItem *nameItem = new QTableWidgetItem(observer->observerName(), id);
        m_ui->observerTable->setItem(i, 0, nameItem);
        if (ownership == Observer::SpecificObserverOwnership) {
            QVariant observer_parent = observer->getObserverPropertyValue(d->obj,OBSERVER_PARENT);
            if (observer_parent.isValid() && (observer_parent.toInt() == id)) {
                QTableWidgetItem *ownerItem = new QTableWidgetItem("", id);
                ownerItem->setIcon(QIcon(ICON_SUCCESS));
                ownerItem->setToolTip(tr("This context owns the selected object (Manages its lifetime)."));
                m_ui->observerTable->setItem(i, OwnerColumn, ownerItem);
            }
        }
        tooltip_string.append(QString(tr("<b>Context Name</b>: %1")).arg(observer->observerName()));
        // Context Description
        tooltip_string.append(QString(tr("<br><b>Context Description</b>: %1")).arg(observer->observerDescription()));
        // Context ID
        tooltip_string.append(QString(tr("<br><b>Context ID</b>: %1")).arg(observer->observerID()));
        // Context Category
        ObserverProperty category_prop = getObserverProperty(OBJECT_CATEGORY);
        QString category_string;
        if (category_prop.isValid()) {
            if (category_prop.observerMap().keys().contains(observer->observerID()))
                category_string = category_prop.value(observer->observerID()).toString();
            else
                category_string = QString(OBSERVER_UNCATEGORIZED_CATEGORY);
        } else {
            category_string = QString(OBSERVER_UNCATEGORIZED_CATEGORY);
        }
        tooltip_string.append(QString(tr("<br><b>Context Category</b>: %1")).arg(category_string));

        // Attributes
        tooltip_string.append(tr("<br><br><b>Attributes: </b> "));
        // Attribute: Activity
        QVariant activity = observer->getObserverPropertyValue(d->obj,OBJECT_ACTIVITY);
        if (activity.isValid() && activity.toBool())
            tooltip_string.append(tr("Active, "));

        // Attribute: Object Name Controller
        QVariant name_manager_id = observer->getObserverPropertyValue(d->obj,OBJECT_NAME_MANAGER_ID);
        if (name_manager_id.isValid() && (name_manager_id.toInt() == observer->observerID()))
            tooltip_string.append(tr("Manages Name, "));

        // Attribute: Uses Instance Name
        ObserverProperty instance_names = observer->getObserverProperty(d->obj,INSTANCE_NAMES);
        if (instance_names.isValid() && instance_names.hasContext(observer->observerID())) {
            tooltip_string.append(QString(tr("Uses Instance Name (\"%1\"), ").arg(instance_names.value(observer->observerID()).toString())));
            has_instance_names = true;
            QTableWidgetItem *aliasItem = new QTableWidgetItem("", id);
            aliasItem->setIcon(QIcon(ICON_SUCCESS));
            aliasItem->setToolTip(tr("This context uses an instance name (alias) for the selected object."));
            m_ui->observerTable->setItem(i, UsesInstanceNameColumn, aliasItem);
        }

        // Attribute: Ownership
        int parent_id = this->getSharedProperty(OBSERVER_PARENT).value().toInt();
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

    m_ui->observerTable->resizeRowsToContents();
    m_ui->observerTable->setColumnCount(OwnerColumn+1);
    m_ui->observerTable->showColumn(NameColumn);
    m_ui->observerTable->showColumn(UsesInstanceNameColumn);
    m_ui->observerTable->setColumnWidth(UsesInstanceNameColumn,40);
    m_ui->observerTable->showColumn(OwnerColumn);
    m_ui->observerTable->setColumnWidth(OwnerColumn,40);
    QStringList headers;
    headers << tr("Context List") << tr("Alias") << tr("Owner");
    m_ui->observerTable->setHorizontalHeaderLabels(headers);
    if (ownership == Observer::SpecificObserverOwnership && has_instance_names) {
        m_ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::ResizeToContents);
        //m_ui->observerTable->horizontalHeader()->setResizeMode(OwnerColumn,QHeaderView::Stretch);
    } else if (ownership == Observer::SpecificObserverOwnership && !has_instance_names) {
        m_ui->observerTable->hideColumn(UsesInstanceNameColumn);
        m_ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::ResizeToContents);
        //m_ui->observerTable->horizontalHeader()->setResizeMode(OwnerColumn,QHeaderView::Stretch);
    } else if (ownership != Observer::SpecificObserverOwnership && has_instance_names) {
        m_ui->observerTable->hideColumn(OwnerColumn);
        m_ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::ResizeToContents);
        //m_ui->observerTable->horizontalHeader()->setResizeMode(UsesInstanceNameColumn,QHeaderView::Stretch);
    } else {
        m_ui->observerTable->hideColumn(UsesInstanceNameColumn);
        m_ui->observerTable->hideColumn(OwnerColumn);
        //m_ui->observerTable->horizontalHeader()->setResizeMode(NameColumn,QHeaderView::Stretch);
    }
}

Qtilities::CoreGui::ObserverProperty Qtilities::CoreGui::ObjectScopeWidget::getObserverProperty(const char* property_name) const {
    if (!d->obj)
        return ObserverProperty();

    QVariant prop;
    prop = d->obj->property(property_name);

    if (prop.isValid() && prop.canConvert<ObserverProperty>()) {
        // This is a normal observer property (not shared)
        return prop.value<ObserverProperty>();
    } else if (!prop.isValid()) {
        return ObserverProperty();
    } else {
        return ObserverProperty();
    }
}

Qtilities::CoreGui::SharedObserverProperty Qtilities::CoreGui::ObjectScopeWidget::getSharedProperty(const char* property_name) const {
    if (!d->obj)
        return SharedObserverProperty();

    QVariant prop;
    prop = d->obj->property(property_name);

    if (prop.isValid() && prop.canConvert<SharedObserverProperty>()) {
        // This is a normal observer property (not shared)
        return prop.value<SharedObserverProperty>();
    } else if (!prop.isValid()) {
        return SharedObserverProperty();
    } else {
        return SharedObserverProperty();
    }
}

void Qtilities::CoreGui::ObjectScopeWidget::constructActions() {
    if (actionContainerWidget)
        return;
    actionContainerWidget = this;
    d->action_provider = new ActionProvider(this);

    // ---------------------------
    // Add Context
    // ---------------------------
    d->actionAddContext = new QAction(QIcon(ICON_NEW),"Add",this);
    //d->action_provider->addAction(d->actionAddContext);
    d->actionAddContext->setEnabled(false);
    connect(d->actionAddContext,SIGNAL(triggered()),SLOT(handle_actionAddContext_triggered()));
    ACTION_MANAGER->registerAction(SELECTION_SCOPE_ADD,d->actionAddContext);
    // ---------------------------
    // Remove Context
    // ---------------------------
    d->actionRemoveContext = new QAction(QIcon(ICON_REMOVE_ONE),"Remove Selected",this);
    d->action_provider->addAction(d->actionRemoveContext);
    d->actionRemoveContext->setEnabled(false);
    connect(d->actionRemoveContext,SIGNAL(triggered()),SLOT(handle_actionRemoveContext_triggered()));
    ACTION_MANAGER->registerAction(SELECTION_SCOPE_REMOVE_SELECTED,d->actionRemoveContext);
    // ---------------------------
    // Detach In Selected Context (Creates copy in selected context which is detached from the rest of the contexts)
    // ---------------------------
    d->actionDetachToSelection = new QAction(QIcon(ICON_REMOVE_ALL),"Romove Others",this);
    d->action_provider->addAction(d->actionDetachToSelection);
    d->actionDetachToSelection->setEnabled(false);
    connect(d->actionDetachToSelection,SIGNAL(triggered()),SLOT(handle_actionDetachToSelection_triggered()));
    ACTION_MANAGER->registerAction(SELECTION_SCOPE_REMOVE_OTHERS,d->actionDetachToSelection);
    // ---------------------------
    // Duplicate In Selected Context
    // ---------------------------
    d->actionDuplicateInScope = new QAction(QIcon(ICON_SPLIT),"Duplicate",this);
    //d->action_provider->addAction(d->actionDuplicateInScope);
    d->actionDuplicateInScope->setEnabled(false);
    connect(d->actionDuplicateInScope,SIGNAL(triggered()),SLOT(handle_actionDuplicateInScope_triggered()));
    ACTION_MANAGER->registerAction(SELECTION_SCOPE_DUPLICATE,d->actionDuplicateInScope);

    //m_ui->observerTable->addAction(d->actionAddContext);
    QAction* sep1 = new QAction(0);
    sep1->setSeparator(true);
    m_ui->observerTable->addAction(sep1);
    m_ui->observerTable->addAction(d->actionRemoveContext);
    m_ui->observerTable->addAction(d->actionDetachToSelection);
    QAction* sep2 = new QAction(0);
    sep2->setSeparator(true);
    m_ui->observerTable->addAction(sep2);
    //m_ui->observerTable->addAction(d->actionDuplicateInScope);
    m_ui->observerTable->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void Qtilities::CoreGui::ObjectScopeWidget::refreshActions() {
    if (!actionContainerWidget)
        constructActions();

    if (!currentWidget)
        return;

    if (m_ui->observerTable->selectedItems().count() > 0) {
        int id = currentWidget->m_ui->observerTable->currentItem()->type();
        Observer* observer = OBJECT_MANAGER->observerReference(id);
        if (observer) {
            if (observer->displayHints()) {
                if (m_ui->observerTable->rowCount() > 1)
                    actionContainerWidget->d->actionDetachToSelection->setEnabled(true);
                else
                    actionContainerWidget->d->actionDetachToSelection->setEnabled(false);

                if (observer->displayHints()->actionHints() & ObserverHints::ActionScopeDuplicate)
                    actionContainerWidget->d->actionDuplicateInScope->setEnabled(true);
                else
                    actionContainerWidget->d->actionDuplicateInScope->setEnabled(false);

                if (observer->displayHints()->actionHints() & ObserverHints::ActionRemoveItem)
                    actionContainerWidget->d->actionRemoveContext->setEnabled(true);
                else
                    actionContainerWidget->d->actionRemoveContext->setEnabled(false);
            }
        } else {
            actionContainerWidget->d->actionDetachToSelection->setEnabled(false);
            actionContainerWidget->d->actionDuplicateInScope->setEnabled(false);
            actionContainerWidget->d->actionRemoveContext->setEnabled(false);
        }
    } else {
        actionContainerWidget->d->actionDetachToSelection->setEnabled(false);
        actionContainerWidget->d->actionDuplicateInScope->setEnabled(false);
        actionContainerWidget->d->actionRemoveContext->setEnabled(false);
    }

    // Check if the observer limit is reached, if so disable the add context action.
    if (d->obj) {
        QVariant prop;
        prop = d->obj->property(OBSERVER_LIMIT);

        if (prop.isValid() && prop.canConvert<SharedObserverProperty>()) {
            // This is a shared property
            int limit = (prop.value<SharedObserverProperty>()).value().toInt();
            if (m_ui->observerTable->rowCount() >= limit) {
                actionContainerWidget->d->actionAddContext->setEnabled(false);
                return;
            }
        }

        actionContainerWidget->d->actionAddContext->setEnabled(true);
    } else {
        actionContainerWidget->d->actionDetachToSelection->setEnabled(false);
        actionContainerWidget->d->actionDuplicateInScope->setEnabled(false);
        actionContainerWidget->d->actionRemoveContext->setEnabled(false);
        actionContainerWidget->d->actionAddContext->setEnabled(false);
    }
}

void Qtilities::CoreGui::ObjectScopeWidget::handle_actionDuplicateInScope_triggered() {
    if (!currentWidget)
        return;

    QMessageBox msgBox;
    msgBox.setText(tr("This feature is not currenlty implemented and will become available in a future release."));
    msgBox.exec();
}

void Qtilities::CoreGui::ObjectScopeWidget::handle_actionAddContext_triggered() {
    if (!currentWidget)
        return;

    QMessageBox msgBox;
    msgBox.setText(tr("This feature is not currenlty implemented and will become available in a future release."));
    msgBox.exec();
}

void Qtilities::CoreGui::ObjectScopeWidget::handle_actionRemoveContext_triggered() {
    if (!currentWidget)
        return;

    // Get the current selected observer
    int id = currentWidget->m_ui->observerTable->currentItem()->type();
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
    if (!currentWidget)
        return;

    // Get the ids of all unselected items
    QList<int> other_ids;
    int selected_id = currentWidget->m_ui->observerTable->currentItem()->type();
    for (int i = 0; i < m_ui->observerTable->rowCount(); i++) {
        if (selected_id != m_ui->observerTable->item(i,0)->type())
            other_ids << m_ui->observerTable->item(i,0)->type();
    }

    QStringList unsupported_items;
    for (int i = 0; i < other_ids.count(); i++) {
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
        msgBox.setInformativeText("Do you want to continue and remove the contexts which does support this?");
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

    for (int i = 0; i < other_ids.count(); i++) {
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
    refreshActions();
}

void Qtilities::CoreGui::ObjectScopeWidget::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
