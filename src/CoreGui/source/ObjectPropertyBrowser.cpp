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

#include "ObjectPropertyBrowser.h"

#include <QtilitiesCoreConstants.h>

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

struct Qtilities::CoreGui::ObjectPropertyBrowserData {
    QMap<const QMetaObject *, QtProperty *> map_class_property;
    QMap<QtProperty *, const QMetaObject *> map_property_class;
    QMap<QtProperty *, int>                 map_property_index;
    QMap<const QMetaObject *, QMap<int, QtVariantProperty *> > map_class_index_property;
    QMap<QtProperty *, bool>                map_expandable_properties;
    QList<QtProperty *>                     top_level_properties;

    QtAbstractPropertyBrowser*              property_browser;
    QtVariantPropertyManager*               property_manager;
    QtVariantPropertyManager*               property_read_only_manager;

    QObject* obj;
    QStringList filter_list;
    bool read_only_properties_disabled;
    bool filter_list_inversed;
};

Qtilities::CoreGui::ObjectPropertyBrowser::ObjectPropertyBrowser(BrowserType browser_type, QWidget *parent) : QWidget(parent)
{
    d = new ObjectPropertyBrowserData;
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

Qtilities::CoreGui::ObjectPropertyBrowser::~ObjectPropertyBrowser() {
    delete d;
}

void Qtilities::CoreGui::ObjectPropertyBrowser::setObject(QObject *object) {
    if (d->obj == object)
        return;

    if (d->obj) {
        QListIterator<QtProperty *> it(d->top_level_properties);
        while (it.hasNext()) {
            d->property_browser->removeProperty(it.next());
        }
        d->top_level_properties.clear();
    }

    d->obj = object;

    if (!d->obj)
        return;

    connect(d->obj,SIGNAL(destroyed()),SLOT(handleObjectDeleted()));
    inspectClass(d->obj->metaObject());

    /*if (object && (d->top_level_properties.count() > 0))
        d->property_browser->setEnabled(true);
    else
        d->property_browser->setEnabled(false);
        */
}

QObject* Qtilities::CoreGui::ObjectPropertyBrowser::object() const {
    return d->obj;
}

void Qtilities::CoreGui::ObjectPropertyBrowser::setFilterList(QStringList filter_list, bool inversed_list) {
    d->filter_list = filter_list;
    d->filter_list_inversed = inversed_list;

    d->map_class_property.clear();
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();

    if (d->obj)
        inspectClass(d->obj->metaObject());
}

QStringList Qtilities::CoreGui::ObjectPropertyBrowser::filterList() const {
    return d->filter_list;
}

void Qtilities::CoreGui::ObjectPropertyBrowser::clearFilter() {
    d->filter_list_inversed = false;
    d->filter_list.clear();

    // Update
    d->map_class_property.clear();
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();

    if (d->obj)
        inspectClass(d->obj->metaObject());
}

void Qtilities::CoreGui::ObjectPropertyBrowser::setFilterListInversed(bool toggle) {
    bool update = false;
    if (d->filter_list_inversed != toggle)
        update = true;

    d->filter_list_inversed = toggle;

    if (update) {
        d->map_class_property.clear();
        QListIterator<QtProperty *> it(d->top_level_properties);
        while (it.hasNext()) {
            d->property_browser->removeProperty(it.next());
        }
        d->top_level_properties.clear();

        if (d->obj)
            inspectClass(d->obj->metaObject());
    }
}

bool Qtilities::CoreGui::ObjectPropertyBrowser::filterListInversed() {
    return d->filter_list_inversed;
}

void Qtilities::CoreGui::ObjectPropertyBrowser::toggleReadOnlyPropertiesDisabled(bool toggle) {
    d->read_only_properties_disabled = toggle;
}

void Qtilities::CoreGui::ObjectPropertyBrowser::handle_property_changed(QtProperty * property, const QVariant & value) {
    if (!d->map_property_index.contains(property))
        return;

    int idx = d->map_property_index.value(property);

    const QMetaObject *metaObject = d->obj->metaObject();
    QMetaProperty metaProperty = metaObject->property(idx);
    if (metaProperty.isEnumType()) {
        if (metaProperty.isFlagType())
            metaProperty.write(d->obj, intToFlag(metaProperty.enumerator(), value.toInt()));
        else
            metaProperty.write(d->obj, intToEnum(metaProperty.enumerator(), value.toInt()));
    } else {
        metaProperty.write(d->obj, value);
    }

    refreshClass(metaObject, true);
}

void Qtilities::CoreGui::ObjectPropertyBrowser::handleObjectDeleted() {
    setObject(0);
    clearFilter();
}

void Qtilities::CoreGui::ObjectPropertyBrowser::inspectClass(const QMetaObject *metaObject) {
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
    if (className == "QObject") {
        // If d->obj has a name manager, we don't allow the user to change the object name by not showing QObject's properties
        QVariant name_manager = d->obj->property(OBJECT_NAME_MANAGER_ID);
        if (name_manager.isValid()) {
            // Remove QObject from d->map_class_property
            d->map_class_property.remove(metaObject);
            return;
        }
    }

    QtProperty *single_property = d->map_class_property.value(metaObject);
    if (!single_property) {
        // Remove namespaces from class name
        QStringList class_name_list = className.split("::");
        className = class_name_list.last();

        single_property = d->property_manager->addProperty(QtVariantPropertyManager::groupTypeId(), className);
        d->map_class_property[metaObject] = single_property;
        d->map_property_class[single_property] = metaObject;

        for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
            QMetaProperty metaProperty = metaObject->property(idx);
            int type = metaProperty.userType();
            QtVariantProperty *subProperty = 0;
            if (!metaProperty.isReadable()) {
                subProperty = d->property_read_only_manager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
                subProperty->setValue(QLatin1String("< Non Readable >"));
            } else if (metaProperty.isEnumType()) {
                if (metaProperty.isFlagType()) {
                    subProperty = d->property_manager->addProperty(QtVariantPropertyManager::flagTypeId(), QLatin1String(metaProperty.name()));
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QMap<int, bool> valueMap;
                    QStringList flagNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueMap.contains(value) && isPowerOf2(value)) {
                            valueMap[value] = true;
                            flagNames.append(QLatin1String(metaEnum.key(i)));
                        }
                    subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
                    subProperty->setValue(flagToInt(metaEnum, metaProperty.read(d->obj).toInt()));
                    }
                } else {
                    subProperty = d->property_manager->addProperty(QtVariantPropertyManager::enumTypeId(), QLatin1String(metaProperty.name()));
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
                    QStringList enumNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueMap.contains(value)) {
                            valueMap[value] = true;
                            enumNames.append(QLatin1String(metaEnum.key(i)));
                        }
                    }
                    subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
                    subProperty->setValue(enumToInt(metaEnum, metaProperty.read(d->obj).toInt()));
                }
                if (!metaProperty.isWritable() && d->read_only_properties_disabled)
                    subProperty->setEnabled(false);
            } else if (d->property_manager->isPropertyTypeSupported(type)) {
                if (!metaProperty.isWritable())
                    subProperty = d->property_read_only_manager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                if (!metaProperty.isDesignable())
                    subProperty = d->property_read_only_manager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                else {
                    subProperty = d->property_manager->addProperty(type, QLatin1String(metaProperty.name()));
                    if (!metaProperty.isWritable() && d->read_only_properties_disabled)
                        subProperty->setEnabled(false);
                }
                subProperty->setValue(metaProperty.read(d->obj));
            } else {
                subProperty = d->property_read_only_manager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
                subProperty->setValue(QLatin1String("< Unknown Type >"));
                subProperty->setEnabled(false);
            }
            single_property->addSubProperty(subProperty);
            d->map_property_index[subProperty] = idx;
            d->map_class_index_property[metaObject][idx] = subProperty;
        }
    } else {
        refreshClass(metaObject, false);
    }

    d->top_level_properties.append(single_property);
    d->property_browser->addProperty(single_property);
}

void Qtilities::CoreGui::ObjectPropertyBrowser::refreshClass(const QMetaObject *metaObject, bool recursive) {
    if (!metaObject)
        return;

    if (recursive)
        refreshClass(metaObject->superClass(), recursive);

    QtProperty *classProperty = d->map_class_property.value(metaObject);
    if (!classProperty)
        return;

    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isReadable()) {
            if (d->map_class_index_property.contains(metaObject) && d->map_class_index_property[metaObject].contains(idx)) {
                QtVariantProperty *subProperty = d->map_class_index_property[metaObject][idx];
                if (metaProperty.isEnumType()) {
                    if (metaProperty.isFlagType())
                        subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(d->obj).toInt()));
                    else
                        subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(d->obj).toInt()));
                } else {
                    subProperty->setValue(metaProperty.read(d->obj));
                }
            }
        }
    }
}

int Qtilities::CoreGui::ObjectPropertyBrowser::enumToInt(const QMetaEnum &metaEnum, int enumValue) const {
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            if (value == enumValue)
                return pos;
            valueMap[value] = pos++;
        }
    }
    return -1;
}

int Qtilities::CoreGui::ObjectPropertyBrowser::intToEnum(const QMetaEnum &metaEnum, int intValue) const {
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    if (intValue >= values.count())
        return -1;
    return values.at(intValue);
}

bool Qtilities::CoreGui::ObjectPropertyBrowser::isSubValue(int value, int subValue) const {
    if (value == subValue)
        return true;
    int i = 0;
    while (subValue) {
        if (!(value & (1 << i))) {
            if (subValue & 1)
                return false;
        }
        i++;
        subValue = subValue >> 1;
    }
    return true;
}

bool Qtilities::CoreGui::ObjectPropertyBrowser::isPowerOf2(int value) const {
    while (value) {
        if (value & 1) {
            return value == 1;
        }
        value = value >> 1;
    }
    return false;
}

int Qtilities::CoreGui::ObjectPropertyBrowser::flagToInt(const QMetaEnum &metaEnum, int flagValue) const {
    if (!flagValue)
        return 0;
    int intValue = 0;
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            if (isSubValue(flagValue, value))
                intValue |= (1 << pos);
            valueMap[value] = pos++;
        }
    }
    return intValue;
}

int Qtilities::CoreGui::ObjectPropertyBrowser::intToFlag(const QMetaEnum &metaEnum, int intValue) const {
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    int flagValue = 0;
    int temp = intValue;
    int i = 0;
    while (temp) {
        if (i >= values.count())
            return -1;
        if (temp & 1)
            flagValue |= values.at(i);
        i++;
        temp = temp >> 1;
    }
    return flagValue;
}

