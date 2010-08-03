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

#include "ObserverProperty.h"

#include <Logger.h>

quint32 MARKER_OBSERVER_PROPERTY = 0xBABEFACE;

bool Qtilities::Core::ObserverProperty::exportObserverPropertyBinary(QDataStream& stream) const {
    stream << MARKER_OBSERVER_PROPERTY;
    stream << name;
    stream << observerMap();
    stream << MARKER_OBSERVER_PROPERTY;
    return true;
}

bool Qtilities::Core::ObserverProperty::importObserverPropertyBinary(QDataStream& stream) {
    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("ObserverProperty binary import failed to detect start marker. Import will fail.");
        return false;
    }
    char* name_tmp;
    stream >> name_tmp;
    name = name_tmp;
    #ifndef QT_NO_DEBUG
        LOG_TRACE("Streaming observer property: " + QString(name));      
    #endif
    stream >> observer_map;
    last_change_context = -1;
    is_exportable = true;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("ObserverProperty binary import failed to detect end marker. Import will fail.");
        return false;
    }
    return true;
}

bool Qtilities::Core::SharedObserverProperty::exportSharedPropertyBinary(QDataStream& stream) const {
    stream << MARKER_OBSERVER_PROPERTY;
    stream << name;
    stream << observerMap();
    stream << property_value;
    #ifndef QT_NO_DEBUG
        QString property_string = property_value.toString();
        if (!property_string.isEmpty())
            LOG_TRACE("Property Value: " + property_string);
    #endif
    stream << MARKER_OBSERVER_PROPERTY;
    return true;
}

bool Qtilities::Core::SharedObserverProperty::importSharedPropertyBinary(QDataStream& stream) {
    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("SharedObserverProperty binary import failed to detect end marker. Import will fail.");
        return false;
    }

    char* name_tmp;
    stream >> name_tmp;
    name = name_tmp;
    #ifndef QT_NO_DEBUG
        LOG_TRACE("Streaming shared property: " + QString(name));
    #endif
    stream >> observer_map;
    stream >> property_value;
    #ifndef QT_NO_DEBUG
        QString property_string = property_value.toString();
        if (!property_string.isEmpty())
            LOG_TRACE("Property Value: " + property_string);
    #endif

    last_change_context = -1;
    is_exportable = true;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_PROPERTY) {
        LOG_ERROR("SharedObserverProperty binary import failed to detect end marker. Import will fail.");
        return false;
    }
    return true;
}

