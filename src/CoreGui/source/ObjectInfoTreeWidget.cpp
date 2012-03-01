/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#include "ObjectInfoTreeWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"

#include <QtilitiesCoreConstants.h>
#include <QtilitiesProperty.h>
#include <Observer.h>

#include <QMetaObject>
#include <QMetaMethod>
#include <QEvent>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;

QPointer<Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget> Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::currentWidget;
QPointer<Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget> Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::actionContainerWidget;

struct Qtilities::CoreGui::qti_private_ObjectInfoTreeWidgetPrivateData {
    qti_private_ObjectInfoTreeWidgetPrivateData() : paste_enabled(false) {}

    bool paste_enabled;
};

Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::qti_private_ObjectInfoTreeWidget(QWidget *parent) : QTreeWidget(parent) {
    d = new qti_private_ObjectInfoTreeWidgetPrivateData;

    setColumnCount(1);
    setSortingEnabled(true);
    headerItem()->setText(0,tr("Registered Objects"));
    currentWidget = 0;
}

Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::~qti_private_ObjectInfoTreeWidget() {
    delete d;
}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::mousePressEvent(QMouseEvent* event) {
    if (!event)
        return;

    if (event->type() == QEvent::MouseButtonPress) {
        if (currentWidget != this) {
            // Disconnect the paste action from the previous observer.
            Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
            if (command->action())
                command->action()->disconnect(currentWidget);

            // Connect to the paste action
            connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionPaste_triggered()));
        }

        currentWidget = this;
    }

    QTreeWidget::mousePressEvent(event);
}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::setObjectMap(QMap<QPointer<QObject>, QString> object_map) {
   clear();

    // Build up a tree using the object map
    // Check all the categories by looking at the qti_prop_CATEGORY_MAP observer property on the objects.
    // Objects without a category property will not be shown
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < object_map.count(); i++) {
        if (!object_map.keys().at(i))
            break;

        QTreeWidgetItem* item = 0;
        QString category_string = tr("More...");
        QVariant prop = object_map.keys().at(i)->property(qti_prop_CATEGORY_MAP);
        if (prop.isValid() && prop.canConvert<SharedProperty>()) {
            SharedProperty observer_property =  prop.value<SharedProperty>();
            if (observer_property.isValid()) {
                // Check if the top level category already exists
                category_string = observer_property.value().toString();
            }
        }

        for (int t = 0; t < topLevelItemCount(); t++) {
            if (topLevelItem(t)->text(0) == category_string)
                item = topLevelItem(t);
        }
        if (!item) {
            item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(category_string));
            // If we use this widget in the future: TODO: rather use QFileIconProvider::Folder;
            item->setData(0,Qt::DecorationRole,QIcon(QString(qti_icon_FOLDER_16X16)));
            items.append(item);
        }

        addTopLevelItem(item);

        // Add this object to the category item
        QTreeWidgetItem* child = new QTreeWidgetItem((QTreeWidget*)0, QStringList(object_map.values().at(i)));
        item->addChild(child);

        // Populate the child item
        populateItem(child,object_map.keys().at(i));

        // Check if it has the OBJECT_ICON shared property set.
        prop = object_map.keys().at(i)->property(qti_prop_DECORATION);
        if (prop.isValid() && prop.canConvert<SharedProperty>())
            child->setIcon(0,(prop.value<SharedProperty>().value().value<QIcon>()));
    }
    expandToDepth(0);
    sortItems(0,Qt::AscendingOrder);
}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::setHierarchyDepth(int depth) {
    Q_UNUSED(depth)

}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::setPasteEnabled(bool enable_paste) {
    d->paste_enabled = enable_paste;
}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::constructActions() {
    if (actionContainerWidget)
        return;
    actionContainerWidget = this;
}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::refreshActions() {
    if (!actionContainerWidget)
        constructActions();

    if (!currentWidget)
        return;
}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::handle_actionPaste_triggered() {
    if (!currentWidget)
        return;

    if (currentWidget->d->paste_enabled){
        // Check if the subjects being dropped are of the same type as the destination observer.
        // If this is not the case, we do not allow the drop.
        const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (QApplication::clipboard()->mimeData());
        if (observer_mime_data) {
            emit currentWidget->pasteActionOccured(observer_mime_data);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Paste Operation Failed.");
            msgBox.setInformativeText("The paste operation could not be completed. Unsupported data type for this context.\n\nDo you want to keep the data in the clipboard for later usage?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();

            switch (ret) {
              case QMessageBox::No:
                  CLIPBOARD_MANAGER->acceptMimeData();
                  break;
              case QMessageBox::Yes:
                  break;
              default:
                  break;
            }
        }
    }
}

void Qtilities::CoreGui::qti_private_ObjectInfoTreeWidget::populateItem(QTreeWidgetItem* item, QObject* obj) {
    // Add the following categories:
    // 1. Methods (Signals and Slots)
    // 2. Properties
    // 3. Events
    // Further categories can be added by subclassing this class.
    if (item) {
        // Methods:
        QTreeWidgetItem* methods = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Methods")));
        methods->setIcon(0, QIcon(qti_icon_METHOD_16x16));
        item->addChild(methods);
        QTreeWidgetItem* events = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Events")));
        events->setIcon(0, QIcon(qti_icon_EVENT_16x16));
        item->addChild(events);
        const QMetaObject* mo = obj->metaObject();
        for(int j=QObject::staticMetaObject.methodCount(); j<mo->methodCount(); j++)
        {
            QMetaMethod m = mo->method(j);
            QTreeWidgetItem* item =  0;

            switch(m.methodType())
            {
            case QMetaMethod::Signal: {
                item = new QTreeWidgetItem(events, Event);
                QString event = m.signature();
                item->setText(0, event);
                item->setData(0, Qt::ForegroundRole, Qt::blue);
                } break;
            case QMetaMethod::Method:
            case QMetaMethod::Slot:
            default: {
                if(m.access() != QMetaMethod::Public)
                    break;
                item = new QTreeWidgetItem(methods, Method);
                QString methodName = QString(m.signature()).section('(', 0, 0);
                item->setText(0, methodName);
                item->setData(0, Qt::ForegroundRole, Qt::darkGreen);
                } break;
            }
        }

        // Properties:
        QTreeWidgetItem* properties = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Properties")));
        properties->setIcon(0, QIcon(qti_icon_PROPERTY_16x16));
        item->addChild(properties);
        for(int j=QObject::staticMetaObject.propertyCount(); j<mo->propertyCount(); j++)
        {
            QMetaProperty prop = mo->property(j);
            QTreeWidgetItem* item = new QTreeWidgetItem(properties, Property);
            item->setData(0, Qt::ForegroundRole, Qt::red);
            item->setText(0, prop.name());
        }

        // Allow other classes to add categories to the item tree item
        emit populateTreeItem(obj, item);
    }
}
