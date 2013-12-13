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

#include "QtilitiesProperty.h"
#include "QtilitiesCoreConstants.h"
#include "ObjectManager.h"
#include "Observer.h"

#include <Logger>

#include <QDomDocument>

using namespace Qtilities::Core::Properties;

// ------------------------------------------
// QtilitiesProperty
// ------------------------------------------

Qtilities::Core::QtilitiesProperty::QtilitiesProperty(const QString& property_name) : IObjectBase(), IExportable() {
    if (property_name.isNull())
        name = QString("");
    else
        name = property_name;

    is_reserved = QtilitiesProperty::propertyIsReserved(property_name.toUtf8().data());
    supports_change_notifications = QtilitiesProperty::propertySupportsChangeNotifications(property_name.toUtf8().data());
    is_removable = QtilitiesProperty::propertyIsRemovable(property_name.toUtf8().data());
    read_only = false;
}

Qtilities::Core::QtilitiesProperty::QtilitiesProperty(const char* property_name) : IObjectBase(), IExportable() {
    if (property_name)
        name = QString::fromUtf8(property_name);
    else
        name = QString("");

    is_reserved = QtilitiesProperty::propertyIsReserved(property_name);
    supports_change_notifications = QtilitiesProperty::propertySupportsChangeNotifications(property_name);
    is_removable = QtilitiesProperty::propertyIsRemovable(property_name);
    read_only = false;
}

Qtilities::Core::QtilitiesProperty::QtilitiesProperty(const QtilitiesProperty& property) : IObjectBase(), IExportable(), name(property.propertyNameString()) {
    is_reserved = property.isReserved();
    is_removable = property.isRemovable();
    supports_change_notifications = property.supportsChangeNotifications();
    read_only = property.isReadOnly();

    setIsExportable(property.isExportable());
}

QtilitiesProperty& Qtilities::Core::QtilitiesProperty::operator=(const QtilitiesProperty& property) {
    if (this==&property) return *this;

    name = property.propertyNameString();
    is_reserved = property.isReserved();
    is_removable = property.isRemovable();
    supports_change_notifications = property.supportsChangeNotifications();
    read_only = property.isReadOnly();

    setIsExportable(property.isExportable());
    return *this;
}

Qtilities::Core::QtilitiesProperty::~QtilitiesProperty() {

}

QString Qtilities::Core::QtilitiesProperty::propertyNameString() const {
    return name;
}

void Qtilities::Core::QtilitiesProperty::setPropertyName(const char* new_name) {
    Q_ASSERT(new_name);
    if (new_name)
        name = QString::fromUtf8(new_name);
}

void Qtilities::Core::QtilitiesProperty::setPropertyName(const QString& new_name) {
    name = new_name;
}

bool Qtilities::Core::QtilitiesProperty::isValid() {
    return !name.isEmpty();
}

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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesProperty::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << MARKER_OBSERVER_PROPERTY;
    stream << name;
    stream << is_reserved;
    stream << read_only;
    stream << is_removable;
    stream << supports_change_notifications;
    stream << MARKER_OBSERVER_PROPERTY;
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("QtilitiesProperty binary import failed to detect start marker. Import will fail.");
        return IExportable::Failed;
    }

    stream >> name;
    stream >> is_reserved;
    stream >> read_only;
    stream >> is_removable;
    stream >> supports_change_notifications;

    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("QtilitiesProperty binary import failed to detect end marker. Import will fail.");
        return IExportable::Failed;
    }
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesProperty::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (!object_node)
        return IExportable::Failed;

    if (!name.isEmpty())
        object_node->setAttribute("Name",name);

    if (is_reserved)
        object_node->setAttribute("Reserved","1");
    else
        object_node->setAttribute("Reserved","0");
    if (read_only)
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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesProperty::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    name = object_node->attribute("Name","");

    if (object_node->hasAttribute("Reserved")) {
        if (object_node->attribute("Reserved") == QLatin1String("1"))
            is_reserved = true;
        if (object_node->attribute("Reserved") == QLatin1String("0"))
            is_reserved = false;
    }
    if (object_node->hasAttribute("ReadOnly")) {
        if (object_node->attribute("ReadOnly") == QLatin1String("1"))
            read_only = true;
        if (object_node->attribute("ReadOnly") == QLatin1String("0"))
            read_only = false;
    }
    if (object_node->hasAttribute("Removable")) {
        if (object_node->attribute("Removable") == QLatin1String("1"))
            is_removable = true;
        if (object_node->attribute("Removable") == QLatin1String("0"))
            is_removable = false;
    }
    if (object_node->hasAttribute("Notifications")) {
        if (object_node->attribute("Notifications") == QLatin1String("1"))
            supports_change_notifications = true;
        if (object_node->attribute("Notifications") == QLatin1String("0"))
            supports_change_notifications = false;
    }

    return IExportable::Complete;
}

QVariant Qtilities::Core::QtilitiesProperty::constructVariant(const QString& type_string, const QString& value_string) {
    QVariant::Type type = QVariant::nameToType(type_string.toUtf8().constData());
    return QtilitiesProperty::constructVariant(type,value_string);
}

QVariant Qtilities::Core::QtilitiesProperty::constructVariant(QVariant::Type type, const QString& value_string) {
    QVariant variant(type);

    if (type == QVariant::StringList)
        variant.setValue(value_string.split(","));
    else
        variant.setValue(value_string);

    if (!variant.convert(type)) {
        qDebug() << Q_FUNC_INFO << "Failed to convert variant to type" << type;
        Q_ASSERT(1);
    }

    // If this assert happen for you, you can trying to construct a variant type which cannot be constructed from a string.
    // For supported types and more information see the QVariant::toString() documentation.
    // You can also check if your variant will be exportable using the isExportableVariant() function below.
    Q_ASSERT(isExportableVariant(variant));

    return variant;
}

bool Qtilities::Core::QtilitiesProperty::isExportableVariant(QVariant variant) {
    if (variant.type() == QVariant::String ||
            variant.type() == QVariant::Bool ||
            //variant.type() == QVariant::Brush ||
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

Qtilities::Core::MultiContextProperty::MultiContextProperty(const MultiContextProperty& property) : QtilitiesProperty(property){
    context_map = property.contextMap();
    last_change_context = property.lastChangedContext();
}

Qtilities::Core::MultiContextProperty::MultiContextProperty(const QtilitiesProperty& qtilities_property) : QtilitiesProperty(qtilities_property){

}

MultiContextProperty& Qtilities::Core::MultiContextProperty::operator=(const MultiContextProperty& other) {
    if (this==&other) return *this;

    name = other.propertyNameString();
    context_map = other.contextMap();
    last_change_context = other.lastChangedContext();
    is_reserved = other.isReserved();
    is_removable = other.isRemovable();
    read_only = other.isReadOnly();
    supports_change_notifications = other.supportsChangeNotifications();

    setIsExportable(other.isExportable());
    return *this;
}

bool Qtilities::Core::MultiContextProperty::operator==(const MultiContextProperty& other) const {
    if (name != other.propertyNameString())
        return false;
    if (context_map != other.contextMap()) {
//        for (int i = 0; i < context_map.count(); ++i)
//            qDebug() << context_map.values().at(i).toString();
//        for (int i = 0; i < other.contextMap().count(); ++i)
//            qDebug() << other.contextMap().values().at(i).toString();

        return false;
    }
    if (is_reserved != other.isReserved())
        return false;
    if (is_removable != other.isRemovable())
        return false;
    if (read_only != other.isReadOnly())
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

QVariant Qtilities::Core::MultiContextProperty::value(int context_id) const {
    return (context_map.value((quint32) context_id));
}

bool Qtilities::Core::MultiContextProperty::setValue(QVariant new_value, int context_id) {
    if (context_id == -1)
        return false;
    if (!new_value.isValid())
        return false;
    context_map[(quint32) context_id] = new_value;
    last_change_context = context_id;
    return true;
}

QString Qtilities::Core::MultiContextProperty::valueString() const {
    QStringList value_strings;
    for (int i = 0; i < context_map.count(); ++i) {
        if (QtilitiesProperty::isExportableVariant(context_map.values().at(i)))
            value_strings << context_map.values().at(i).toString();
        else
            value_strings << QObject::tr("Non-exportable variant");
    }
    return value_strings.join(",");
}

int Qtilities::Core::MultiContextProperty::lastChangedContext() const {
    return last_change_context;
}

bool Qtilities::Core::MultiContextProperty::hasContext(int context_id) const {
    return context_map.contains((quint32) context_id);
}

void Qtilities::Core::MultiContextProperty::removeContext(int context_id) {
    last_change_context = context_id;
    context_map.remove((quint32) context_id);
}

void Qtilities::Core::MultiContextProperty::addContext(QVariant new_value, int context_id) {
    if (!context_map.contains((quint32) context_id))
        context_map[context_id] = new_value;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::MultiContextProperty::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::MultiContextProperty::exportBinary(QDataStream& stream) const {   
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    IExportable::ExportResultFlags result = QtilitiesProperty::exportBinary(stream);
    if (result == IExportable::Failed)
        return result;

    stream << contextMap();
    stream << MARKER_OBSERVER_PROPERTY;
    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::MultiContextProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    IExportable::ExportResultFlags result = QtilitiesProperty::importBinary(stream,import_list);
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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::MultiContextProperty::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    for (int i = 0; i < context_map.count(); ++i) {
        if (!isExportableVariant(context_map.values().at(i))) {
            LOG_DEBUG("Failed to export MultiContextProperty. It contains a QVariant which cannot be converted to a QString(). Type name: " + QString(context_map.values().at(i).typeName()));
            return IExportable::Incomplete;
        }
    }

    IExportable::ExportResultFlags result = QtilitiesProperty::exportXml(doc,object_node);
    if (result == IExportable::Failed)
        return result;

    object_node->setAttribute("Count",context_map.count());
    for (int i = 0; i < context_map.count(); ++i) {
        QDomElement context_element = doc->createElement("Context_" + QString::number(i));
        object_node->appendChild(context_element);
        context_element.setAttribute("ID",QString::number(context_map.keys().at(i)));
        context_element.setAttribute("Type",context_map.values().at(i).typeName());
        if (context_map.values().at(i).type() == QVariant::StringList)
            context_element.setAttribute("Value",context_map.values().at(i).toStringList().join(","));
        else
            context_element.setAttribute("Value",context_map.values().at(i).toString());
    }

    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::MultiContextProperty::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {   
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    context_map.clear();

    IExportable::ExportResultFlags  result = QtilitiesProperty::importXml(doc,object_node,import_list);
    if (result == IExportable::Failed)
        return result;

    QDomNodeList propNodes = object_node->childNodes();
    for(int i = 0; i < propNodes.count(); ++i) {
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

Qtilities::Core::SharedProperty::SharedProperty(const SharedProperty& shared_property) : QtilitiesProperty(shared_property) {
    property_value = shared_property.value();
}

Qtilities::Core::SharedProperty::SharedProperty(const QtilitiesProperty& qtilities_property) : QtilitiesProperty(qtilities_property) {

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

SharedProperty& Qtilities::Core::SharedProperty::operator=(const SharedProperty& other) {
    if (this==&other) return *this;

    name = other.propertyNameString();
    property_value = other.value();
    is_reserved = other.isReserved();
    is_removable = other.isRemovable();
    read_only = other.isReadOnly();
    supports_change_notifications = other.supportsChangeNotifications();

    setIsExportable(other.isExportable());
    return *this;
}

bool Qtilities::Core::SharedProperty::operator==(const SharedProperty& other) const {
    if (name != other.propertyNameString())
        return false;
    if (property_value != other.value())
        return false;
    if (is_reserved != other.isReserved())
        return false;
    if (read_only != other.isReadOnly())
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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::SharedProperty::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    IExportable::ExportResultFlags result = QtilitiesProperty::exportBinary(stream);
    if (result == IExportable::Failed)
        return result;

    stream << property_value;
    stream << MARKER_OBSERVER_PROPERTY;
    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::SharedProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    IExportable::ExportResultFlags result = QtilitiesProperty::importBinary(stream,import_list);
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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::SharedProperty::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (!isExportableVariant(property_value)) {
        LOG_DEBUG("Failed to export SharedProperty. It contains a QVariant which cannot be converted to a QString(). Type name: " + QString(property_value.typeName()));
        return IExportable::Incomplete;
    }

    IExportable::ExportResultFlags result = QtilitiesProperty::exportXml(doc,object_node);
    if (result == IExportable::Failed)
        return result;

    object_node->setAttribute("Type",property_value.typeName());
    if (property_value.type() == QVariant::StringList)
        object_node->setAttribute("Value",property_value.toStringList().join(","));
    else
        object_node->setAttribute("Value",property_value.toString());

    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::SharedProperty::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {   
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    IExportable::ExportResultFlags result = QtilitiesProperty::importXml(doc,object_node,import_list);
    if (result == IExportable::Failed)
        return result;

    if (object_node->hasAttribute("Type") && object_node->hasAttribute("Value")) {
        QString type_string = object_node->attribute("Type");
        QString value_string = object_node->attribute("Value");
        property_value = constructVariant(type_string,value_string);
    } else
        result = IExportable::Incomplete;

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

// TODO: This does not work yet.. Exactly like Qt docs, don't get it!
QDebug operator<<(QDebug dbg, const Qtilities::Core::SharedProperty &prop) {
    if (Qtilities::Core::QtilitiesProperty::isExportableVariant(prop.value()))
        dbg.nospace() << "(Shared Property: " << prop.propertyNameString() << ", Value: Non exportable variant type)";
    else
        dbg.nospace() << "(Shared Property: " << prop.propertyNameString() << ", Value: " << prop.value().toString() << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Qtilities::Core::MultiContextProperty &prop) {
    dbg.nospace() << "(Multi Context Property: " << prop.propertyNameString() << ")";
    for (int i = 0; i < prop.contextMap().count(); ++i) {
        QString context_name = QString::number(prop.contextMap().keys().at(i));
        Qtilities::Core::Observer* obs = OBJECT_MANAGER->observerReference(prop.contextMap().keys().at(i));
        if (obs)
            context_name = obs->observerName();

        if (Qtilities::Core::QtilitiesProperty::isExportableVariant(prop.contextMap().values().at(i)))
            dbg.nospace() << "(Context: " << context_name << ", Value: Non exportable variant type)";
        else
            dbg.nospace() << "(Context: " << context_name << ", Value: " << prop.contextMap().values().at(i).toString() << ")";
    }

    return dbg.space();
}
