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

#include "ObjectDynamicPropertyBrowser.h"

#ifdef QTILITIES_PROPERTY_BROWSER
#include <QtilitiesCoreGui>

#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QAction>
#include <QToolBar>

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
    qti_private_MultiContextPropertyData() : observer_id(-1) {}
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
    QMap<QtProperty*, qti_private_MultiContextPropertyData> multi_context_properties;

    QtAbstractPropertyBrowser*              property_browser;
    QtVariantPropertyManager*               property_manager;
    QtVariantPropertyManager*               property_manager_read_only;

    QPointer<QObject>                       obj;
    QPointer<AddDynamicPropertyWizard>      add_property_wizard;

    bool                                    ignore_property_changes_from_object_side;
    bool                                    ignore_property_changes_from_browser_side;

    QToolBar*                               toolbar;
    bool                                    toolbar_visible;
    Qt::ToolBarArea                         preferred_area;
    QAction*                                actionAddProperty;
    QAction*                                actionRemoveProperty;

    bool                                    show_qtilities_properties;
    bool                                    monitor_changes;

    ObjectManager::PropertyTypes            new_property_type;

    bool                                    read_only;
};

Qtilities::CoreGui::ObjectDynamicPropertyBrowser::ObjectDynamicPropertyBrowser(BrowserType browser_type, bool show_toolbar, Qt::ToolBarArea area, QWidget *parent) : QMainWindow(parent)
{
    d = new ObjectDynamicPropertyBrowserPrivateData;
    d->ignore_property_changes_from_object_side = false;
    d->ignore_property_changes_from_browser_side = false;
    d->show_qtilities_properties = false;
    d->monitor_changes = false;
    d->toolbar_visible = show_toolbar;
    d->preferred_area = area;
    d->obj = 0;
    d->actionAddProperty = 0;
    d->actionRemoveProperty = 0;
    d->toolbar = 0;
    d->new_property_type = ObjectManager::NonQtilitiesProperties;
    d->read_only = false;

    if (browser_type == TreeBrowser) {
        QtTreePropertyBrowser* property_browser = new QtTreePropertyBrowser(this);
        property_browser->setRootIsDecorated(false);
        property_browser->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
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
                this, SLOT(propertyChangedFromBrowserSide(QtProperty *, const QVariant &)));

    // Create property manager for read only properties. It does not get a factory.
    d->property_manager_read_only = new QtVariantPropertyManager(this);

    if (d->toolbar_visible)
        toggleToolBar();
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

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::clear() {
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setNewPropertyType(ObjectManager::PropertyTypes new_property_type) {
    if (new_property_type == ObjectManager::NonQtilitiesProperties || new_property_type == ObjectManager::SharedProperties) {
        d->new_property_type = new_property_type;
        if (d->add_property_wizard)
            d->add_property_wizard->setNewPropertyType(new_property_type);
    }
}

ObjectManager::PropertyTypes Qtilities::CoreGui::ObjectDynamicPropertyBrowser::newPropertyType() const {
    return d->new_property_type;
}

void ObjectDynamicPropertyBrowser::toggleToolBar() {
    // Set up the toolbar:
    if (!d->toolbar) {
        d->toolbar = new QToolBar("Dynamic Property Actions");
        addToolBar(d->preferred_area,d->toolbar);
        d->toolbar->setMovable(false);
        d->actionAddProperty = new QAction(QIcon(qti_icon_NEW_16x16),tr("Add Property"),this);
        connect(d->actionAddProperty,SIGNAL(triggered()),SLOT(handleAddProperty()));
        d->toolbar->addAction(d->actionAddProperty);
        d->actionRemoveProperty = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),tr("Remove Selected Property"),this);
        connect(d->actionRemoveProperty,SIGNAL(triggered()),SLOT(handleRemoveProperty()));
        d->toolbar->addAction(d->actionRemoveProperty);
    } else {
        d->toolbar->setVisible(d->toolbar_visible);
        d->toolbar_visible = !d->toolbar_visible;
    }
}

bool ObjectDynamicPropertyBrowser::isToolBarVisible() const {
    return d->toolbar_visible;
}

QtAbstractPropertyBrowser *ObjectDynamicPropertyBrowser::abstractPropertyBrowser() const {
    return d->property_browser;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::refresh() {
    inspectObject(d->obj);
}

bool Qtilities::CoreGui::ObjectDynamicPropertyBrowser::eventFilter(QObject *object, QEvent *event) {
    if (object == d->obj) {
        if (event->type() == QEvent::DynamicPropertyChange) {
            QDynamicPropertyChangeEvent* property_change_event = static_cast<QDynamicPropertyChangeEvent*> (event);
            if (property_change_event) {
                if (!d->ignore_property_changes_from_object_side)
                    inspectObject(d->obj);
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

void ObjectDynamicPropertyBrowser::setReadOnly(bool read_only) {
    if (d->read_only != read_only) {
        d->read_only = read_only;
        refresh();
        if (d->actionAddProperty)
            d->actionAddProperty->setEnabled(false);
        if (d->actionRemoveProperty)
            d->actionRemoveProperty->setEnabled(false);
    }
}

bool ObjectDynamicPropertyBrowser::readOnly() const {
    return d->read_only;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QObject *object, bool monitor_changes) {
    if (!object) {
        setEnabled(false);
        clear();
        return;
    } else
        setEnabled(true);

    if (d->obj == object)
        return;

    if (d->obj) {
        d->obj->disconnect(this);
        d->obj->removeEventFilter(this);
    }

    d->obj = object;
    d->monitor_changes = monitor_changes;
    if (!d->obj)
        return;

    if (monitor_changes) {
        // Connect to the IModificationNotifier interface if it exists:
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
        if (mod_iface)
            connect(d->obj,SIGNAL(modificationStateChanged(bool)),SLOT(propertyChangedFromObjectSide(bool)));

        // Install an event filter on the object.
        // This will catch property change events as well.
        d->obj->installEventFilter(this);
    }

    connect(d->obj,SIGNAL(destroyed()),SLOT(handleObjectDeleted()));
    inspectObject(d->obj);
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

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::propertyChangedFromBrowserSide(QtProperty * property, const QVariant & value) {
    if (d->ignore_property_changes_from_browser_side)
        return;

    if (!d->obj)
        return;

    QByteArray ba = property->propertyName().toUtf8();
    const char* property_name = ba.data();

    d->ignore_property_changes_from_object_side = true;

    if (d->multi_context_properties.contains(property)) {
        // This is an observer property:
        qti_private_MultiContextPropertyData prop_data = d->multi_context_properties[property];
        if (prop_data.type == qti_private_MultiContextPropertyData::Shared) {
            SharedProperty shared_property(property_name,value);
            if (ObjectManager::setSharedProperty(d->obj,shared_property) && d->monitor_changes) {
                // Connect to the IModificationNotifier interface if it exists:
                IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
                if (mod_iface) {
                    mod_iface->setModificationState(true);
                }
            }
        } else if (prop_data.type == qti_private_MultiContextPropertyData::Mixed) {
            MultiContextProperty multi_context_property = ObjectManager::getMultiContextProperty(d->obj,prop_data.name);
            multi_context_property.setValue(value,prop_data.observer_id);
            if (ObjectManager::setMultiContextProperty(d->obj,multi_context_property) && d->monitor_changes) {
                // Connect to the IModificationNotifier interface if it exists:
                IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
                if (mod_iface) {
                    mod_iface->setModificationState(true);
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
                    mod_iface->setModificationState(true);
                }
            }
        }
    }

    d->ignore_property_changes_from_object_side = true;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::propertyChangedFromObjectSide(bool modified) {
    if (!modified)
        return;

    if (d->ignore_property_changes_from_object_side)
        return;

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

    d->ignore_property_changes_from_browser_side = true;
    clear();

    QList<QByteArray> property_names = obj->dynamicPropertyNames();
    qSort(property_names);
    for (int i = 0; i < property_names.count(); ++i) {
        QString property_name = QString(property_names.at(i).data());
        if (property_name.startsWith("qti.") && !d->show_qtilities_properties)
            continue;

        QVariant property_variant = obj->property(property_names.at(i));
        QVariant property_value = property_variant;

        bool is_enabled = !d->read_only;
        QtProperty *dynamic_property = 0;
        // If it is MultiContextProperty or SharedProperty then we need to handle it:
        if (property_variant.isValid() && property_variant.canConvert<SharedProperty>()) {
            SharedProperty shared_property = (property_variant.value<SharedProperty>());
            if (shared_property.isReserved() || shared_property.isReadOnly() || d->read_only)
                is_enabled = false;
            property_value = shared_property.value();

            // We handle some specific Qtilities properties in a special way:
            if (!strcmp(property_names.at(i).data(),qti_prop_PARENT_ID) ||
                !strcmp(property_names.at(i).data(),qti_prop_NAME_MANAGER_ID)) {
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
            if (is_enabled) {
                if (d->property_manager->isPropertyTypeSupported(property_value.type())) {
                   dynamic_property = d->property_manager->addProperty(property_value.type(), property_name);

                    if (dynamic_property) {
                        d->property_manager->setValue(dynamic_property,property_value);
                        d->top_level_properties.append(dynamic_property);
                        d->property_browser->addProperty(dynamic_property);
                        qti_private_MultiContextPropertyData prop_data;
                        prop_data.name = property_names.at(i).data();
                        prop_data.type = qti_private_MultiContextPropertyData::Shared;
                        d->multi_context_properties[dynamic_property] = prop_data;
                    }
                } else {
                    dynamic_property = d->property_manager_read_only->addProperty(QVariant::String, property_name);
                    d->property_manager_read_only->setValue(dynamic_property,QLatin1String("< Unknown Type >"));
                    dynamic_property->setEnabled(false);
                    d->top_level_properties.append(dynamic_property);
                    d->property_browser->addProperty(dynamic_property);
                }
            } else {
                if (d->property_manager_read_only->isPropertyTypeSupported(property_value.type())) {
                    dynamic_property = d->property_manager_read_only->addProperty(property_value.type(), property_name);

                    if (dynamic_property) {
                        d->property_manager_read_only->setValue(dynamic_property,property_value);
                        d->top_level_properties.append(dynamic_property);
                        d->property_browser->addProperty(dynamic_property);
                        qti_private_MultiContextPropertyData prop_data;
                        prop_data.name = property_names.at(i).data();
                        prop_data.type = qti_private_MultiContextPropertyData::Shared;
                        d->multi_context_properties[dynamic_property] = prop_data;
                    }
                } else {
                    dynamic_property = d->property_manager_read_only->addProperty(QVariant::String, property_name);
                    d->property_manager_read_only->setValue(dynamic_property,QLatin1String("< Unknown Type >"));
                    dynamic_property->setEnabled(false);
                    d->top_level_properties.append(dynamic_property);
                    d->property_browser->addProperty(dynamic_property);
                }
            }
        } else if (property_variant.isValid() && property_variant.canConvert<MultiContextProperty>()) {
            MultiContextProperty multi_context_property = (property_variant.value<MultiContextProperty>());
            if (multi_context_property.isReserved() || multi_context_property.isReadOnly() || d->read_only)
                is_enabled = false;

            // Now make a group property with the values for all the different contexts under it:
            dynamic_property = d->property_manager->addProperty(QtVariantPropertyManager::groupTypeId(), property_name);
            if (dynamic_property) {
                QMap<quint32,QVariant> context_map = multi_context_property.contextMap();
                QList<quint32> keys = context_map.keys();
                int count = keys.count();
                for (int s = 0; s < count; s++) {
                    QVariant sub_property_value = context_map.values().at(s);
                    Observer* obs = OBJECT_MANAGER->observerReference((int) keys.at(s));
                    QString context_name = QString::number(keys.at(s));
                    if (obs)
                        context_name = obs->observerName() + " (" + QString::number(obs->observerID()) + ")";

                    QtProperty* sub_property = 0;
                    if (is_enabled) {
                        if (!d->property_manager->isPropertyTypeSupported(sub_property_value.type())) {
                            sub_property = d->property_manager_read_only->addProperty(QVariant::String, context_name);
                            if (sub_property)
                                d->property_manager_read_only->setValue(sub_property,QLatin1String("< Unknown Type >"));
                        } else {
                            sub_property = d->property_manager->addProperty(sub_property_value.type(), context_name);
                            if (sub_property)
                                d->property_manager->setValue(sub_property,sub_property_value);
                        }
                    } else {
                        if (!d->property_manager_read_only->isPropertyTypeSupported(sub_property_value.type())) {
                            sub_property = d->property_manager_read_only->addProperty(QVariant::String, context_name);
                            d->property_manager_read_only->setValue(sub_property,QLatin1String("< Unknown Type >"));
                            sub_property->setEnabled(false);
                        } else {
                            sub_property = d->property_manager_read_only->addProperty(sub_property_value.type(), context_name);
                            if (sub_property)
                                d->property_manager_read_only->setValue(sub_property,sub_property_value);
                        }

                    }

                    if (sub_property) {
                        dynamic_property->addSubProperty(sub_property);
                        qti_private_MultiContextPropertyData prop_data;
                        prop_data.name = property_names.at(i).data();
                        prop_data.type = qti_private_MultiContextPropertyData::Mixed;
                        prop_data.observer_id = (int) keys.at(s);
                        d->multi_context_properties[sub_property] = prop_data;
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

    d->ignore_property_changes_from_browser_side = false;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handleAddProperty() {
    if (!d->obj)
        return;

    if (!d->add_property_wizard) {
        d->add_property_wizard = new AddDynamicPropertyWizard(AddDynamicPropertyWizard::ConstructAndAdd,this);
        d->add_property_wizard->setNewPropertyType(d->new_property_type);
    } else {
        d->add_property_wizard->restart();
    }

    d->add_property_wizard->setObject(d->obj);
    if (d->add_property_wizard->exec())
        refresh();
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handleRemoveProperty() {
    if (!d->obj)
        return;

    if (!d->property_browser->currentItem())
        return;

    // Get the selected property:
    QtProperty* property = d->property_browser->currentItem()->property();

    if (!property)
        return;

    QByteArray ba = property->propertyName().toUtf8();
    const char* property_name = ba.data();

    if (d->multi_context_properties.contains(property)) {
        // This is an observer property:
        qti_private_MultiContextPropertyData prop_data = d->multi_context_properties[property];
        if (prop_data.type == qti_private_MultiContextPropertyData::Shared) {
            SharedProperty shared_property = ObjectManager::getSharedProperty(d->obj,prop_data.name);
            if (shared_property.isReserved()) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is reserved and cannot be deleted."));
                msgBox.exec();
            } else if (!shared_property.isRemovable()) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is not removable, thus you can't delete it."));
                msgBox.exec();
            } else {
                d->obj->setProperty(prop_data.name,QVariant());
                refresh();
                emit propertyRemoved(property->propertyName());
            }
        } else if (prop_data.type == qti_private_MultiContextPropertyData::Mixed) {
            MultiContextProperty multi_context_property = ObjectManager::getMultiContextProperty(d->obj,prop_data.name);
            if (multi_context_property.isReserved()) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is reserved and cannot be deleted."));
                msgBox.exec();
            } else if (!multi_context_property.isRemovable()) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Dynamic Property Browser"));
                msgBox.setText(tr("The selected property is not removable, thus you can't delete it."));
                msgBox.exec();
            } else {
                d->obj->setProperty(prop_data.name,QVariant());
                refresh();
                emit propertyRemoved(property->propertyName());
            }
        }
    } else {
        // This is a normal property on the object:
        d->obj->setProperty(property_name,QVariant());
        refresh();
        emit propertyRemoved(property->propertyName());
    }
}

#endif
