/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
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
****************************************************************************/

#include "ObjectDynamicPropertyBrowser.h"

#ifdef QTILITIES_PROPERTY_BROWSER
#include <QtilitiesCoreGui>

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QAction>
#include <QtGui/QToolBar>

#include <qtvariantproperty.h>
#include <qtgroupboxpropertybrowser.h>
#include <qttreepropertybrowser.h>
#include <qtpropertybrowser.h>
#include <qtbuttonpropertybrowser.h>

using namespace QtilitiesCoreGui;

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QWidget, ObjectDynamicPropertyBrowser> ObjectDynamicPropertyBrowser::factory;
    }
}

struct Qtilities::CoreGui::qti_private_MultiContextPropertyData {
    qti_private_MultiContextPropertyData() {}
    qti_private_MultiContextPropertyData(const qti_private_MultiContextPropertyData& other) {
        type = other.type;
        name = other.name;
        observer_id = other.observer_id;
    }

    enum SubPropertyType {
        Mixed,
        Shared
    };

    //! The type of this sub property.
    SubPropertyType             type;
    //! The name of the property on the active object.
    const char*                 name;
    //! The observer ID for which the value changed in the case of MultiContextProperty properties.
    int                         observer_id;
};

struct Qtilities::CoreGui::ObjectDynamicPropertyBrowserPrivateData {
    QList<QtProperty*>                      top_level_properties;
    QMap<QtProperty*, qti_private_MultiContextPropertyData> observer_properties;

    QtAbstractPropertyBrowser*              property_browser;
    QtVariantPropertyManager*               property_manager;
    QtVariantPropertyManager*               property_manager_read_only;

    QPointer<QObject>                       obj;

    bool                                    ignore_property_changes;

    QToolBar*                               toolbar;
    QAction*                                actionAddProperty;
    QAction*                                actionRemoveProperty;

    bool                                    show_qtilities_properties;
    bool                                    monitor_changes;
};

Qtilities::CoreGui::ObjectDynamicPropertyBrowser::ObjectDynamicPropertyBrowser(BrowserType browser_type, bool show_toolbar, QWidget *parent) : QMainWindow(parent)
{
    d = new ObjectDynamicPropertyBrowserPrivateData;
    d->ignore_property_changes = false;
    d->show_qtilities_properties = false;
    d->monitor_changes = false;
    d->obj = 0;
    if (browser_type == TreeBrowser) {
        QtTreePropertyBrowser* property_browser = new QtTreePropertyBrowser(this);
        property_browser->setRootIsDecorated(false);
        d->property_browser = property_browser;
    } else if (browser_type == GroupBoxBrowser) {
        QtGroupBoxPropertyBrowser* property_browser = new QtGroupBoxPropertyBrowser(this);
        d->property_browser = property_browser;
    } else if (browser_type == ButtonBrowser) {
        QtButtonPropertyBrowser* property_browser = new QtButtonPropertyBrowser(this);
        d->property_browser = property_browser;
    }

    d->property_browser->layout()->setMargin(0);
    setCentralWidget(d->property_browser);

    // Create property manager for editable:
    d->property_manager = new QtVariantPropertyManager(this);
    QtVariantEditorFactory *factory = new QtVariantEditorFactory(this);
    d->property_browser->setFactoryForManager(d->property_manager, factory);
    connect(d->property_manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(handle_property_changed(QtProperty *, const QVariant &)));

    // Create property manager for read only properties. It does not get a factory.
    d->property_manager_read_only = new QtVariantPropertyManager(this);

    // Set up the toolbar:
    if (show_toolbar) {
        d->toolbar = addToolBar("Dynamic Property Actions");
        d->actionAddProperty = new QAction(QIcon(qti_icon_NEW_16x16),"Add Property",this);
        connect(d->actionAddProperty,SIGNAL(triggered()),SLOT(handleAddProperty()));
        d->toolbar->addAction(d->actionAddProperty);
        d->actionRemoveProperty = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),"Remove Selected Property",this);
        connect(d->actionRemoveProperty,SIGNAL(triggered()),SLOT(handleRemoveProperty()));
        d->toolbar->addAction(d->actionRemoveProperty);
    }
}

Qtilities::CoreGui::ObjectDynamicPropertyBrowser::~ObjectDynamicPropertyBrowser() {
    delete d;
}

QSize Qtilities::CoreGui::ObjectDynamicPropertyBrowser::sizeHint() const {
    if (d->property_browser) {
        if (d->property_browser->sizeHint().isValid() && d->property_browser->sizeHint().width() != 0
            && d->property_browser->sizeHint().height() != 0)
            return d->property_browser->sizeHint();
    }

    return this->size();
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::refresh(bool has_changes) {
    if (d->obj && has_changes) {
        d->ignore_property_changes = true;
        inspectObject(d->obj);
        d->ignore_property_changes = false;
    }
}

bool Qtilities::CoreGui::ObjectDynamicPropertyBrowser::eventFilter(QObject *object, QEvent *event) {
    if (object == d->obj) {
        if (event->type() == QEvent::DynamicPropertyChange) {
            QDynamicPropertyChangeEvent* property_change_event = static_cast<QDynamicPropertyChangeEvent*> (event);
            if (property_change_event) {
                //qDebug() << "Dynamic change event update in ObjectDynamicPropertyBrowser: " << property_change_event->propertyName();
                d->ignore_property_changes = true;
                //inspectObject(d->obj);
                d->ignore_property_changes = false;
            }
        }
    }

    return false;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::toggleQtilitiesProperties(bool show_qtilities_properties) {
    if (d->show_qtilities_properties != show_qtilities_properties) {
        d->show_qtilities_properties = show_qtilities_properties;
        refresh();
    }
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QObject *object, bool monitor_changes) {
    if (d->obj == object)
        return;

    if (d->obj) {
        d->obj->disconnect(this);
        d->obj->removeEventFilter(this);
    }

    d->obj = object;
    d->monitor_changes = monitor_changes;

    if (monitor_changes) {
        // Connect to the IModificationNotifier interface if it exists:
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
        if (mod_iface)
            connect(d->obj,SIGNAL(modificationStateChanged(bool)),SLOT(refresh(bool)));

        // Install an event filter on the object.
        // This will catch property change events as well.
        d->obj->installEventFilter(this);
    }

    if (!d->obj)
        return;

    connect(d->obj,SIGNAL(destroyed()),SLOT(handleObjectDeleted()));
    d->ignore_property_changes = true;
    inspectObject(d->obj);
    d->ignore_property_changes = false;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QPointer<QObject> object, bool monitor_changes) {
    QObject* obj = object;
    setObject(obj,monitor_changes);
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QList<QObject*> objects, bool monitor_changes) {
    if (objects.count() == 1)
        setObject(objects.front(),monitor_changes);
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QList<QPointer<QObject> > objects, bool monitor_changes) {
    if (objects.count() == 1) {
        QObject* obj = objects.front();
        setObject(obj,monitor_changes);
    }
}

QObject* Qtilities::CoreGui::ObjectDynamicPropertyBrowser::object() const {
    return d->obj;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handle_property_changed(QtProperty * property, const QVariant & value) {
    if (d->ignore_property_changes)
        return;

    if (!d->obj)
        return;

    QByteArray ba = property->propertyName().toAscii();
    const char* property_name = ba.data();

    if (d->observer_properties.contains(property)) {
        // This is an observer property:
        qti_private_MultiContextPropertyData prop_data = d->observer_properties[property];
        if (prop_data.type == qti_private_MultiContextPropertyData::Shared) {
            SharedProperty shared_property(property_name,value);
            if (Observer::setSharedProperty(d->obj,shared_property) && d->monitor_changes) {
                // Connect to the IModificationNotifier interface if it exists:
                IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
                if (mod_iface) {
                    d->ignore_property_changes = true;
                    mod_iface->setModificationState(true);
                    d->ignore_property_changes = false;
                }
            }
        } else if (prop_data.type == qti_private_MultiContextPropertyData::Mixed) {
            MultiContextProperty observer_property = Observer::getMultiContextProperty(d->obj,prop_data.name);
            observer_property.setValue(value,prop_data.observer_id);
            if (Observer::setMultiContextProperty(d->obj,observer_property) && d->monitor_changes) {
                // Connect to the IModificationNotifier interface if it exists:
                IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
                if (mod_iface) {
                    d->ignore_property_changes = true;
                    mod_iface->setModificationState(true);
                    d->ignore_property_changes = false;
                }
            }
        }
    } else {
        // Check if this is a normal property on the object:
        if (d->top_level_properties.contains(property)) {
            d->obj->setProperty(property_name,value);

            if (d->monitor_changes) {
                // Connect to the IModificationNotifier interface if it exists:
                IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
                if (mod_iface) {
                    d->ignore_property_changes = true;
                    mod_iface->setModificationState(true);
                    d->ignore_property_changes = false;
                }
            }
        }
    }

    refresh();
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handleObjectDeleted() {
    setObject(0);
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::inspectObject(const QObject* obj) {
    if (!obj)
        return;

    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();

    for (int i = 0; i < obj->dynamicPropertyNames().count(); i++) {
        QString property_name = QString(obj->dynamicPropertyNames().at(i).data());
        if (property_name.startsWith("qti.") && !d->show_qtilities_properties)
            continue;

        QVariant property_variant = obj->property(obj->dynamicPropertyNames().at(i));
        QVariant property_value = property_variant;

        bool is_enabled = true;
        QtProperty *dynamic_property = 0;
        // If it is MultiContextProperty or SharedProperty then we need to handle it:
        if (property_variant.isValid() && property_variant.canConvert<SharedProperty>()) {
            SharedProperty shared_property = (property_variant.value<SharedProperty>());
            if (shared_property.isReserved())
                is_enabled = false;
            property_value = shared_property.value();

            // We handle some specific Qtilities properties in a special way:
            if (!strcmp(obj->dynamicPropertyNames().at(i).data(),qti_prop_PARENT_ID) ||
                !strcmp(obj->dynamicPropertyNames().at(i).data(),qti_prop_NAME_MANAGER_ID)) {
                    int observer_id = property_value.toInt();
                    if (observer_id == -1) {
                        property_value = QLatin1String("None");
                    } else {
                        Observer* obs = OBJECT_MANAGER->observerReference(observer_id);
                        if (obs)
                            property_value = obs->observerName();
                        else
                            property_value = QLatin1String("< Unregistered Observer >");
                    }
            }

            // Now add the property:
            if (d->property_manager->isPropertyTypeSupported(property_value.type())) {
                if (is_enabled)
                    dynamic_property = d->property_manager->addProperty(property_value.type(), property_name);
                else
                    dynamic_property = d->property_manager_read_only->addProperty(property_value.type(), property_name);
                if (dynamic_property) {
                    d->property_manager->setValue(dynamic_property,property_value);
                    d->top_level_properties.append(dynamic_property);
                    d->property_browser->addProperty(dynamic_property);
                    qti_private_MultiContextPropertyData prop_data;
                    prop_data.name = obj->dynamicPropertyNames().at(i).data();
                    prop_data.type = qti_private_MultiContextPropertyData::Shared;
                    d->observer_properties[dynamic_property] = prop_data;
                }
            } else {
                dynamic_property = d->property_manager_read_only->addProperty(QVariant::String, property_name);
                d->property_manager_read_only->setValue(dynamic_property,QLatin1String("< Unknown Type >"));
                dynamic_property->setEnabled(false);
                d->top_level_properties.append(dynamic_property);
                d->property_browser->addProperty(dynamic_property);
            }
        } else if (property_variant.isValid() && property_variant.canConvert<MultiContextProperty>()) {
            MultiContextProperty observer_property = (property_variant.value<MultiContextProperty>());
            if (observer_property.isReserved())
                is_enabled = false;

            // Now make a group property with the values for all the different contexts under it:
            dynamic_property = d->property_manager->addProperty(QtVariantPropertyManager::groupTypeId(), property_name);
            if (dynamic_property) {
                QMap<quint32,QVariant> context_map = observer_property.contextMap();
                for (int s = 0; s < context_map.count(); s++) {
                    QVariant sub_property_value = context_map.values().at(s);
                    Observer* obs = OBJECT_MANAGER->observerReference((int) context_map.keys().at(s));
                    QString context_name = QString::number(context_map.keys().at(s));
                    if (obs)
                        context_name = obs->observerName() + " (" + QString::number(obs->observerID()) + ")";

                    QtProperty* sub_property = 0;
                    if (!d->property_manager->isPropertyTypeSupported(sub_property_value.type())) {
                        sub_property = d->property_manager_read_only->addProperty(QVariant::String, context_name);
                        d->property_manager_read_only->setValue(sub_property,QLatin1String("< Unknown Type >"));
                        sub_property->setEnabled(false);
                    } else {
                        if (is_enabled)
                            sub_property = d->property_manager->addProperty(sub_property_value.type(), context_name);
                        else
                            sub_property = d->property_manager_read_only->addProperty(sub_property_value.type(), context_name);

                        if (sub_property) {
                            d->property_manager->setValue(sub_property,sub_property_value);
                        }
                    }

                    if (sub_property) {
                        dynamic_property->addSubProperty(sub_property);
                        qti_private_MultiContextPropertyData prop_data;
                        prop_data.name = obj->dynamicPropertyNames().at(i).data();
                        prop_data.type = qti_private_MultiContextPropertyData::Mixed;
                        prop_data.observer_id = (int) context_map.keys().at(s);
                        d->observer_properties[sub_property] = prop_data;
                    }
                }

                d->top_level_properties.append(dynamic_property);
                d->property_browser->addProperty(dynamic_property);
            }
        } else {
            // Now add the property:
            if (d->property_manager->isPropertyTypeSupported(property_value.type())) {
                if (is_enabled)
                    dynamic_property = d->property_manager->addProperty(property_value.type(), property_name);
                else
                    dynamic_property = d->property_manager_read_only->addProperty(property_value.type(), property_name);
                if (dynamic_property) {
                    d->property_manager->setValue(dynamic_property,property_value);
                    d->top_level_properties.append(dynamic_property);
                    d->property_browser->addProperty(dynamic_property);
                }
            } else {
                dynamic_property = d->property_manager_read_only->addProperty(QVariant::String, property_name);
                d->property_manager_read_only->setValue(dynamic_property,QLatin1String("< Unknown Type >"));
                dynamic_property->setEnabled(false);
                d->top_level_properties.append(dynamic_property);
                d->property_browser->addProperty(dynamic_property);
            }
        }
    }
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handleAddProperty() {
    if (!d->obj)
        return;

    QVariant::Type selected_type;
    bool ok;
    QString property_name = QInputDialog::getText(this, tr("Name your property"),tr("Property name:"), QLineEdit::Normal,"New Property", &ok);
    if (!ok && property_name.isEmpty())
        return;

    QStringList type_names;
    type_names << QVariant::typeToName(QVariant::Bool);
    type_names << QVariant::typeToName(QVariant::Brush);
    type_names << QVariant::typeToName(QVariant::Color);
    type_names << QVariant::typeToName(QVariant::Date);
    type_names << QVariant::typeToName(QVariant::Font);
    type_names << QVariant::typeToName(QVariant::Point);
    type_names << QVariant::typeToName(QVariant::Size);
    type_names << QVariant::typeToName(QVariant::String);
    type_names << QVariant::typeToName(QVariant::UInt);

    QInputDialog dialog;
    dialog.setComboBoxItems(type_names);
    dialog.setComboBoxEditable(false);
    dialog.setWindowTitle(tr("Choose a property type"));
    dialog.setLabelText(tr("Available property types:"));
    dialog.setOption(QInputDialog::UseListViewForComboBoxItems,true);
    if (dialog.exec()) {
        QString item = dialog.textValue().trimmed();
        QByteArray type_name_ba = item.toAscii();
        const char* type_name = type_name_ba.data();
        QByteArray property_name_ba = property_name.toAscii();
        const char* char_property_name = property_name_ba.data();
        selected_type = QVariant::nameToType(type_name);
        if (!d->obj->setProperty(char_property_name,QVariant(selected_type)))
            refresh();
        else {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Dynamic Property Browser"));
            msgBox.setText(tr("Failed to add property to object in QObject::setProperty()."));
            msgBox.exec();
        }
    }
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handleRemoveProperty() {
    if (!d->obj)
        return;

    // Get the selected property:
    QtProperty* property = d->property_browser->currentItem()->property();

    if (!property)
        return;

    QByteArray ba = property->propertyName().toAscii();
    const char* property_name = ba.data();

    if (d->observer_properties.contains(property)) {
        // This is an observer property:
        qti_private_MultiContextPropertyData prop_data = d->observer_properties[property];
        if (prop_data.type == qti_private_MultiContextPropertyData::Shared) {
            SharedProperty shared_property = Observer::getSharedProperty(d->obj,prop_data.name);
            if (shared_property.isReserved()) {
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is reserved and cannot be deleted."));
                msgBox.exec();
            } else if (!shared_property.isRemovable()) {
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is not removable and cannot be deleted."));
                msgBox.exec();
            } else {
                d->obj->setProperty(prop_data.name,QVariant());
            }
        } else if (prop_data.type == qti_private_MultiContextPropertyData::Mixed) {
            MultiContextProperty observer_property = Observer::getMultiContextProperty(d->obj,prop_data.name);
            if (observer_property.isReserved()) {
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is reserved and cannot be deleted."));
                msgBox.exec();
            } else if (!observer_property.isRemovable()) {
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is not removable and cannot be deleted."));
                msgBox.exec();
            } else {
                d->obj->setProperty(prop_data.name,QVariant());
            }
        }
    } else {
        // This is a normal property on the object:
        d->obj->setProperty(property_name,QVariant());
    }
}

#endif
