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

    return true;
}

// ------------------------------------------
// MultiContextProperty
// ------------------------------------------

quint32 MARKER_OBSERVER_PROPERTY = 0xBABEFACE;

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
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::MultiContextProperty::exportBinary(QDataStream& stream) const {
    stream << MARKER_OBSERVER_PROPERTY;
    stream << name;
    stream << is_reserved;
    stream << supports_change_notifications;
    stream << contextMap();
    stream << MARKER_OBSERVER_PROPERTY;
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::MultiContextProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("MultiContextProperty binary import failed to detect start marker. Import will fail.");
        return IExportable::Failed;
    }
    char* name_tmp;
    stream >> name_tmp;
    name = name_tmp;
    stream >> is_reserved;
    stream >> supports_change_notifications;
    LOG_TRACE("Streaming observer property: " + QString(name));
    stream >> context_map;
    last_change_context = -1;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("MultiContextProperty binary import failed to detect end marker. Import will fail.");
        return IExportable::Failed;
    }
    return IExportable::Complete;
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
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::SharedProperty::exportBinary(QDataStream& stream) const {
    stream << MARKER_OBSERVER_PROPERTY;
    stream << name;
    stream << is_reserved;
    stream << supports_change_notifications;
    stream << property_value;
    #ifndef QT_NO_DEBUG
        QString property_string = property_value.toString();
        if (!property_string.isEmpty())
            LOG_TRACE("Property Value: " + property_string);
    #endif
    stream << MARKER_OBSERVER_PROPERTY;
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::SharedProperty::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("SharedProperty binary import failed to detect end marker. Import will fail.");
        return IExportable::Failed;
    }

    char* name_tmp;
    stream >> name_tmp;
    name = name_tmp;
    LOG_TRACE("Streaming shared property: " + QString(name));
    stream >> is_reserved;
    stream >> supports_change_notifications;
    stream >> property_value;
    #ifndef QT_NO_DEBUG
        QString property_string = property_value.toString();
        if (!property_string.isEmpty())
            LOG_TRACE("Property Value: " + property_string);
    #endif

    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("SharedProperty binary import failed to detect end marker. Import will fail.");
        return IExportable::Failed;
    }
    return IExportable::Complete;
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
