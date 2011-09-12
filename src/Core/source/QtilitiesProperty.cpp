/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#include "QtilitiesProperty.h"
#include "QtilitiesCoreConstants.h"

#include <Logger>

#include <QDomDocument>

using namespace Qtilities::Core::Properties;

// ------------------------------------------
// QtilitiesProperty
// ------------------------------------------

bool Qtilities::Core::QtilitiesProperty::propertyIsExportable(const char* property_name) {
    if (!qstrcmp(property_name,qti_prop_ALIAS_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_ACCESS_MODE))
        return true;
    if (!qstrcmp(property_name,qti_prop_ACTIVITY_MAP))
        return true;
    if (!qstrcmp(property_name,qti_prop_CATEGORY_MAP))
        return true;
    if (!qstrcmp(property_name,qti_prop_LIMITED_EXPORTS))
        return false;
    if (!qstrcmp(property_name,qti_prop_NAME))
        return false;
    if (!qstrcmp(property_name,qti_prop_NAME_MANAGER_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_OWNERSHIP))
        return false;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_LIMIT))
        return true;
    if (!qstrcmp(property_name,qti_prop_PARENT_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_VISITOR_ID))
        return true;
    if (!qstrcmp(property_name,qti_prop_DISPLAYED_ALIAS_MAP))
        return false;

    return true;
}

bool Qtilities::Core::QtilitiesProperty::propertyIsReserved(const char* property_name) {
    if (!qstrcmp(property_name,qti_prop_ALIAS_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_ACCESS_MODE))
        return false;
    if (!qstrcmp(property_name,qti_prop_ACTIVITY_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_CATEGORY_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_LIMITED_EXPORTS))
        return true;
    if (!qstrcmp(property_name,qti_prop_NAME))
        return false;
    if (!qstrcmp(property_name,qti_prop_NAME_MANAGER_ID))
        return true;
    if (!qstrcmp(property_name,qti_prop_OWNERSHIP))
        return true;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_LIMIT))
        return false;
    if (!qstrcmp(property_name,qti_prop_PARENT_ID))
        return true;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_MAP))
        return true;
    if (!qstrcmp(property_name,qti_prop_VISITOR_ID))
        return true;
    if (!qstrcmp(property_name,qti_prop_DISPLAYED_ALIAS_MAP))
        return false;

    return false;
}

bool Qtilities::Core::QtilitiesProperty::propertyIsRemovable(const char* property_name) {
    if (!qstrcmp(property_name,qti_prop_ALIAS_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_ACCESS_MODE))
        return true;
    if (!qstrcmp(property_name,qti_prop_ACTIVITY_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_CATEGORY_MAP))
        return true;
    if (!qstrcmp(property_name,qti_prop_LIMITED_EXPORTS))
        return false;
    if (!qstrcmp(property_name,qti_prop_NAME))
        return false;
    if (!qstrcmp(property_name,qti_prop_NAME_MANAGER_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_OWNERSHIP))
        return false;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_LIMIT))
        return true;
    if (!qstrcmp(property_name,qti_prop_PARENT_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_VISITOR_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_DISPLAYED_ALIAS_MAP))
        return true;

    return true;
}

bool Qtilities::Core::QtilitiesProperty::propertySupportsChangeNotifications(const char* property_name) {
    if (!qstrcmp(property_name,qti_prop_ALIAS_MAP))
        return true;
    if (!qstrcmp(property_name,qti_prop_ACCESS_MODE))
        return true;
    if (!qstrcmp(property_name,qti_prop_ACTIVITY_MAP))
        return true;
    if (!qstrcmp(property_name,qti_prop_CATEGORY_MAP))
        return true;
    if (!qstrcmp(property_name,qti_prop_LIMITED_EXPORTS))
        return false;
    if (!qstrcmp(property_name,qti_prop_NAME))
        return true;
    if (!qstrcmp(property_name,qti_prop_NAME_MANAGER_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_OWNERSHIP))
        return false;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_LIMIT))
        return false;
    if (!qstrcmp(property_name,qti_prop_PARENT_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_OBSERVER_MAP))
        return false;
    if (!qstrcmp(property_name,qti_prop_VISITOR_ID))
        return false;
    if (!qstrcmp(property_name,qti_prop_DISPLAYED_ALIAS_MAP))
        return false;

    return false;
}

quint32 MARKER_OBSERVER_PROPERTY = 0xBABEFACE;

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::QtilitiesProperty::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesProperty::exportBinary(QDataStream& stream) const {
    stream << MARKER_OBSERVER_PROPERTY;
    stream << name;
    stream << is_reserved;
    stream << is_read_only;
    stream << is_removable;
    stream << supports_change_notifications;
    stream << MARKER_OBSERVER_PROPERTY;
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("QtilitiesProperty binary import failed to detect start marker. Import will fail.");
        return IExportable::Failed;
    }

    char* name_tmp;
    stream >> name_tmp;
    name = name_tmp;
    stream >> is_reserved;
    stream >> is_read_only;
    stream >> is_removable;
    stream >> supports_change_notifications;

    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("SharedProperty binary import failed to detect end marker. Import will fail.");
        return IExportable::Failed;
    }
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesProperty::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    if (!object_node)
        return IExportable::Failed;

    object_node->setAttribute("Name",QString::fromLocal8Bit(name));
    qDebug() << "Export side: " << QString::fromLocal8Bit(name);

    if (is_reserved)
        object_node->setAttribute("Reserved","1");
    else
        object_node->setAttribute("Reserved","0");
    if (is_read_only)
        object_node->setAttribute("ReadOnly","1");
    else
        object_node->setAttribute("ReadOnly","0");
    if (is_removable)
        object_node->setAttribute("Removable","1");
    else
        object_node->setAttribute("Removable","0");
    if (supports_change_notifications)
        object_node->setAttribute("Notifications","1");
    else
        object_node->setAttribute("Notifications","0");

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesProperty::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    if (object_node->hasAttribute("Name")) {
        name = object_node->attribute("Name").toLocal8Bit();
        qDebug() << "Import side: " << name;
    }

    if (object_node->hasAttribute("Reserved")) {
        if (object_node->attribute("Reserved") == "1")
            is_reserved = true;
        if (object_node->attribute("Reserved") == "0")
            is_reserved = false;
    }
    if (object_node->hasAttribute("ReadOnly")) {
        if (object_node->attribute("ReadOnly") == "1")
            is_read_only = true;
        if (object_node->attribute("ReadOnly") == "0")
            is_read_only = false;
    }
    if (object_node->hasAttribute("Removable")) {
        if (object_node->attribute("Removable") == "1")
            is_removable = true;
        if (object_node->attribute("Removable") == "0")
            is_removable = false;
    }
    if (object_node->hasAttribute("Notifications")) {
        if (object_node->attribute("Notifications") == "1")
            supports_change_notifications = true;
        if (object_node->attribute("Notifications") == "0")
            supports_change_notifications = false;
    }

    return IExportable::Complete;
}

QVariant Qtilities::Core::QtilitiesProperty::constructVariant(const QString& type_string, const QString& value_string) const {
    QVariant variant(QVariant::nameToType(type_string.toLocal8Bit().data()));
    variant.setValue(value_string);
    variant.convert(QVariant::nameToType(type_string.toLocal8Bit().data()));

    // If this assert happen for you, you can trying to construct a variant type which cannot be constructed from a string.
    // For supported types and more information see the QVariant::toString() documentation.
    Q_ASSERT(isExportableVariant(variant));

    return variant;
}

bool Qtilities::Core::QtilitiesProperty::isExportableVariant(QVariant variant) const {
    if (variant.type() == QVariant::String ||
            variant.type() == QVariant::Bool ||
            variant.type() == QVariant::ByteArray ||
            variant.type() == QVariant::Char ||
            variant.type() == QVariant::Date ||
            variant.type() == QVariant::DateTime ||
            variant.type() == QVariant::Double ||
            variant.type() == QVariant::Int ||
            variant.type() == QVariant::LongLong ||
            variant.type() == QVariant::StringList ||
            variant.type() == QVariant::Time ||
            variant.type() == QVariant::UInt ||
            variant.type() == QVariant::ULongLong)
        return true;
    else
        return false;
}

// ------------------------------------------
// MultiContextProperty
// ------------------------------------------

Qtilities::Core::MultiContextProperty::MultiContextProperty(const char* property_name) : QtilitiesProperty(property_name) {
    last_change_context = -1;
}

Qtilities::Core::MultiContextProperty::MultiContextProperty(QDataStream &ds, Qtilities::ExportVersion version) : QtilitiesProperty("") {
    setExportVersion(version);
    QList<QPointer<QObject> > import_list;
    importBinary(ds,import_list);
}

Qtilities::Core::MultiContextProperty::MultiContextProperty(const MultiContextProperty& property) : QtilitiesProperty(property.propertyName()){
    context_map = property.contextMap();
    last_change_context = property.lastChangedContext();
}

void Qtilities::Core::MultiContextProperty::operator=(const MultiContextProperty& other) {
    context_map = other.contextMap();
    last_change_context = other.lastChangedContext();
    is_reserved = other.isReserved();
    is_removable = other.isRemovable();
    supports_change_notifications = other.supportsChangeNotifications();

    setIsExportable(other.isExportable());
}

bool Qtilities::Core::MultiContextProperty::operator==(const MultiContextProperty& other) const {
    if (context_map != other.contextMap())
        return false;
    if (is_reserved != other.isReserved())
        return false;
    if (is_removable != other.isRemovable())
        return false;
    if (supports_change_notifications != other.supportsChangeNotifications())
        return false;
    if (isExportable() != other.isExportable())
        return false;

    return true;
}

bool Qtilities::Core::MultiContextProperty::operator!=(const MultiContextProperty& other) const {
    return !(*this==other);
}

Qtilities::Core::MultiContextProperty::~MultiContextProperty() {}

QMap<quint32,QVariant> Qtilities::Core::MultiContextProperty::contextMap() const {
    return context_map;
}

QVariant Qtilities::Core::MultiContextProperty::value(int observer_context) const {
    return (context_map.value((quint32) observer_context));
}

bool Qtilities::Core::MultiContextProperty::setValue(QVariant new_value, int observer_context) {
    if (observer_context == -1)
        return false;
    if (!new_value.isValid())
        return false;
    context_map[(quint32) observer_context] = new_value;
    last_change_context = observer_context;
    return true;
}

int Qtilities::Core::MultiContextProperty::lastChangedContext() const {
    return last_change_context;
}

bool Qtilities::Core::MultiContextProperty::hasContext(int observer_context) const {
    return context_map.contains((quint32) observer_context);
}

void Qtilities::Core::MultiContextProperty::removeContext(int observer_context) {
    context_map.remove((quint32) observer_context);
}

void Qtilities::Core::MultiContextProperty::addContext(QVariant new_value, int observer_context) {
    if (!context_map.keys().contains((quint32) observer_context))
        context_map[observer_context] = new_value;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::MultiContextProperty::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::MultiContextProperty::exportBinary(QDataStream& stream) const {   
    IExportable::Result result = QtilitiesProperty::exportBinary(stream);
    if (result == IExportable::Failed)
        return result;

    stream << contextMap();
    stream << MARKER_OBSERVER_PROPERTY;
    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::MultiContextProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    IExportable::Result result = QtilitiesProperty::importBinary(stream,import_list);
    if (result == IExportable::Failed)
        return result;

    stream >> context_map;
    last_change_context = -1;

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("SharedProperty binary import failed to detect end marker. Import will fail.");
        return IExportable::Failed;
    }

    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::MultiContextProperty::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    for (int i = 0; i < context_map.count(); i++) {
        if (!isExportableVariant(context_map.values().at(i))) {
            LOG_DEBUG("Failed to export MultiContextProperty. It contains a QVariant which cannot be converted to a QString(). Type name: " + QString(context_map.values().at(i).typeName()));
            return IExportable::Incomplete;
        }
    }

    QDomElement property_node = doc->createElement("PropertyMC");
    object_node->appendChild(property_node);

    IExportable::Result result = QtilitiesProperty::exportXml(doc,&property_node);
    if (result == IExportable::Failed)
        return result;

    property_node.setAttribute("Count",context_map.count());
    for (int i = 0; i < context_map.count(); i++) {
        QDomElement context_element = doc->createElement("Context_" + QString::number(i));
        property_node.appendChild(context_element);
        context_element.setAttribute("ID",QString::number(context_map.keys().at(i)));
        context_element.setAttribute("Type",context_map.values().at(i).typeName());
        if (context_map.values().at(i).type() == QVariant::StringList)
            context_element.setAttribute("Value",context_map.values().at(i).toStringList().join(","));
        else
            context_element.setAttribute("Value",context_map.values().at(i).toString());
    }

    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::MultiContextProperty::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    IExportable::Result result = IExportable::Complete;

    context_map.clear();

    QDomNodeList childNodes = object_node->childNodes();
    for (int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "PropertyMC") {
            result = QtilitiesProperty::importXml(doc,&child,import_list);
            if (result == IExportable::Failed)
                return result;

            QDomNodeList propNodes = child.childNodes();
            for(int i = 0; i < propNodes.count(); i++) {
                QDomNode propNode = propNodes.item(i);
                QDomElement prop = propNode.toElement();

                if (prop.isNull())
                    continue;

                if (prop.tagName().startsWith("Context_")) {
                    if (prop.hasAttribute("Type") && prop.hasAttribute("Value")) {
                        int observer_id = prop.attribute("ID").toInt();
                        QString type_string = prop.attribute("Type");
                        QString value_string = prop.attribute("Value");
                        context_map[observer_id] = constructVariant(type_string,value_string);
                    } else
                        result = IExportable::Incomplete;
                    continue;
                }
            }
            continue;
        }
    }

    return result;
}

// ------------------------------------------
// SharedProperty
// ------------------------------------------

Qtilities::Core::SharedProperty::SharedProperty(const char* property_name, QVariant init_value) : QtilitiesProperty(property_name)  {
    property_value = init_value;
}

Qtilities::Core::SharedProperty::SharedProperty(QDataStream &ds, Qtilities::ExportVersion version) : QtilitiesProperty("") {
    setExportVersion(version);
    QList<QPointer<QObject> > import_list;
    importBinary(ds,import_list);
}

Qtilities::Core::SharedProperty::SharedProperty(const SharedProperty& shared_property) : QtilitiesProperty(shared_property.propertyName()) {
    property_value = shared_property.value();
}

QVariant Qtilities::Core::SharedProperty::value() const {
    return property_value;
}

bool Qtilities::Core::SharedProperty::setValue(QVariant new_value) {
    if (!new_value.isValid())
        return false;

    property_value = new_value;
    return true;
}

void Qtilities::Core::SharedProperty::operator=(const SharedProperty& other) {
    property_value = other.value();
    is_reserved = other.isReserved();
    is_removable = other.isRemovable();
    supports_change_notifications = other.supportsChangeNotifications();

    setIsExportable(other.isExportable());
}
bool Qtilities::Core::SharedProperty::operator==(const SharedProperty& other) const {
    if (property_value != other.value())
        return false;
    if (is_reserved != other.isReserved())
        return false;
    if (is_removable != other.isRemovable())
        return false;
    if (supports_change_notifications != other.supportsChangeNotifications())
        return false;
    if (isExportable() != other.isExportable())
        return false;

    return true;
}

bool Qtilities::Core::SharedProperty::operator!=(const SharedProperty& other) const {
    return !(*this==other);
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::SharedProperty::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::SharedProperty::exportBinary(QDataStream& stream) const {
    IExportable::Result result = QtilitiesProperty::exportBinary(stream);
    if (result == IExportable::Failed)
        return result;

    stream << property_value;
    stream << MARKER_OBSERVER_PROPERTY;
    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::SharedProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    IExportable::Result result = QtilitiesProperty::importBinary(stream,import_list);
    if (result == IExportable::Failed)
        return result;

    stream >> property_value;

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("SharedProperty binary import failed to detect end marker. Import will fail.");
        return IExportable::Failed;
    }

    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::SharedProperty::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    if (!isExportableVariant(property_value)) {
        LOG_DEBUG("Failed to export SharedProperty. It contains a QVariant which cannot be converted to a QString(). Type name: " + QString(property_value.typeName()));
        return IExportable::Incomplete;
    }

    QDomElement property_node = doc->createElement("PropertyS");
    object_node->appendChild(property_node);

    IExportable::Result result = QtilitiesProperty::exportXml(doc,&property_node);
    if (result == IExportable::Failed)
        return result;

    property_node.setAttribute("Type",property_value.typeName());
    if (property_value.type() == QVariant::StringList)
        property_node.setAttribute("Value",property_value.toStringList().join(","));
    else
        property_node.setAttribute("Value",property_value.toString());

    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::SharedProperty::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    IExportable::Result result = IExportable::Complete;
    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "PropertyS") {
            IExportable::Result intermediate_result = QtilitiesProperty::importXml(doc,&child,import_list);
            if (intermediate_result == IExportable::Failed)
                return result;

            if (result == IExportable::Complete)
                result = intermediate_result;

            if (child.hasAttribute("Type") && child.hasAttribute("Value")) {
                QString type_string = child.attribute("Type");
                QString value_string = child.attribute("Value");
                property_value = constructVariant(type_string,value_string);
            } else
                result = IExportable::Incomplete;

            continue;
        }
    }

    return result;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::MultiContextProperty& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::MultiContextProperty& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::SharedProperty& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::SharedProperty& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}
