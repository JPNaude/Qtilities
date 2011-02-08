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

#ifndef QTILITIES_NO_PROPERTY_BROWSER
#include <QtilitiesCoreConstants>
#include <IModificationNotifier>

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>
#include <qtvariantproperty.h>
#include <qtgroupboxpropertybrowser.h>
#include <qttreepropertybrowser.h>
#include <qtpropertybrowser.h>
#include <qtbuttonpropertybrowser.h>

using namespace Qtilities::Core::Properties;

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QWidget, ObjectDynamicPropertyBrowser> ObjectDynamicPropertyBrowser::factory;
    }
}

struct Qtilities::CoreGui::ObjectDynamicPropertyBrowserData {
    QList<QtProperty *>                     top_level_properties;

    QtAbstractPropertyBrowser*              property_browser;
    QtVariantPropertyManager*               property_manager;
    QtVariantPropertyManager*               property_read_only_manager;

    QPointer<QObject>                       obj;
    QStringList                             filter_list;
    bool                                    read_only_properties_disabled;
    bool                                    filter_list_inversed;
};

Qtilities::CoreGui::ObjectDynamicPropertyBrowser::ObjectDynamicPropertyBrowser(BrowserType browser_type, QWidget *parent) : QWidget(parent)
{
    d = new ObjectDynamicPropertyBrowserData;
    d->obj = 0;
    d->filter_list_inversed = false;
    d->read_only_properties_disabled = true;

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

    d->property_browser->layout()->setMargin(0);;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d->property_browser);

    d->property_read_only_manager = new QtVariantPropertyManager(this);
    d->property_manager = new QtVariantPropertyManager(this);
    QtVariantEditorFactory *factory = new QtVariantEditorFactory(this);
    d->property_browser->setFactoryForManager(d->property_manager, factory);

    connect(d->property_manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(handle_property_changed(QtProperty *, const QVariant &)));
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
        if (d->obj) {
            QListIterator<QtProperty *> it(d->top_level_properties);
            while (it.hasNext()) {
                d->property_browser->removeProperty(it.next());
            }
            d->top_level_properties.clear();
            d->obj->disconnect(this);
        }

        inspectClass(d->obj->metaObject());
    }
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QObject *object, bool monitor_changes) {
    if (d->obj == object)
        return;

    if (d->obj) {
        QListIterator<QtProperty *> it(d->top_level_properties);
        while (it.hasNext()) {
            d->property_browser->removeProperty(it.next());
        }
        d->top_level_properties.clear();
        d->obj->disconnect(this);
    }

    d->obj = object;

    if (monitor_changes) {
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
        if (mod_iface)
            connect(mod_iface->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(refresh(bool)));
    }

    if (!d->obj)
        return;

    connect(d->obj,SIGNAL(destroyed()),SLOT(handleObjectDeleted()));
    inspectClass(d->obj->metaObject());
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QPointer<QObject> object, bool monitor_changes) {
    if (d->obj == object)
        return;

    if (d->obj) {
        QListIterator<QtProperty *> it(d->top_level_properties);
        while (it.hasNext()) {
            d->property_browser->removeProperty(it.next());
        }
        d->top_level_properties.clear();
        d->obj->disconnect(this);
    }

    d->obj = object;

    if (monitor_changes) {
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (d->obj);
        if (mod_iface)
            connect(mod_iface->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(refresh(bool)));
    }

    if (!d->obj)
        return;

    connect(d->obj,SIGNAL(destroyed()),SLOT(handleObjectDeleted()));
    inspectClass(d->obj->metaObject());
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QList<QObject*> objects, bool monitor_changes) {
    Q_UNUSED(monitor_changes)
    if (objects.count() == 1)
        setObject(objects.front());
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setObject(QList<QPointer<QObject> > objects, bool monitor_changes) {
    Q_UNUSED(monitor_changes)
    if (objects.count() == 1)
        setObject(objects.front());
}

QObject* Qtilities::CoreGui::ObjectDynamicPropertyBrowser::object() const {
    return d->obj;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setFilterList(QStringList filter_list, bool inversed_list) {
    d->filter_list = filter_list;
    d->filter_list_inversed = inversed_list;

    /*d->map_class_property.clear();
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();

    if (d->obj)
        inspectClass(d->obj->metaObject());
        */
}

QStringList Qtilities::CoreGui::ObjectDynamicPropertyBrowser::filterList() const {
    return d->filter_list;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::clearFilter() {
    d->filter_list_inversed = false;
    d->filter_list.clear();

    // Update
    /*d->map_class_property.clear();
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();

    if (d->obj)
        inspectClass(d->obj->metaObject());
        */
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::setFilterListInversed(bool toggle) {
    bool update = false;
    if (d->filter_list_inversed != toggle)
        update = true;

    d->filter_list_inversed = toggle;

    if (update) {
        /*d->map_class_property.clear();
        QListIterator<QtProperty *> it(d->top_level_properties);
        while (it.hasNext()) {
            d->property_browser->removeProperty(it.next());
        }
        d->top_level_properties.clear();

        if (d->obj)
            inspectClass(d->obj->metaObject());*/
    }
}

bool Qtilities::CoreGui::ObjectDynamicPropertyBrowser::filterListInversed() {
    return d->filter_list_inversed;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::toggleReadOnlyPropertiesDisabled(bool toggle) {
    d->read_only_properties_disabled = toggle;
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handle_property_changed(QtProperty * property, const QVariant & value) {
    if (!d->obj)
        return;

}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::handleObjectDeleted() {
    setObject(0);
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::inspectClass(const QMetaObject *metaObject) {
    if (!metaObject)
        return;

    inspectClass(metaObject->superClass());

    QString className = QLatin1String(metaObject->className());

    // Check className against the filter list
    if (!d->filter_list_inversed) {
        if (d->filter_list.contains(className.split("::").last()))
            return;
    } else {
        if (!d->filter_list.contains(className.split("::").last()))
            return;
    }

    // Handle QObject in a special way.
    /*if (className == "QObject") {
        // If d->obj has a name manager, we don't allow the user to change the object name by not showing QObject's properties
        QVariant name_manager = d->obj->property(OBJECT_NAME_MANAGER_ID);
        if (name_manager.isValid()) {
            // Remove QObject from d->map_class_property
            d->map_class_property.remove(metaObject);
            return;
        }
    }*/



    //d->top_level_properties.append(single_property);
    //d->property_browser->addProperty(single_property);
}

void Qtilities::CoreGui::ObjectDynamicPropertyBrowser::refreshClass(const QMetaObject *metaObject, bool recursive) {
    if (!metaObject)
        return;

    if (recursive)
        refreshClass(metaObject->superClass(), recursive);
}

#endif
