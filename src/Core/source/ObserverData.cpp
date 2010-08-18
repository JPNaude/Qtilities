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

#include "ObserverData.h"
#include "ObserverHints.h"

bool Qtilities::Core::ObserverData::exportBinary(QDataStream& stream) const {
    stream << (qint32) subject_limit;
    stream << (qint32) subject_id_counter;
    stream << observer_description;
    stream << (qint32) access_mode;
    stream << (qint32) access_mode_scope;
    stream << category_access;
    stream << deliver_qtilties_property_changed_events;
    if (display_hints) {
        stream << (bool) true;
        return display_hints->exportBinary(stream);
    } else {
        return true;
    }
}

bool Qtilities::Core::ObserverData::importBinary(QDataStream& stream) {
    quint32 ui32;
    stream >> ui32;
    subject_limit = ui32;
    stream >> ui32;
    subject_id_counter = ui32;
    stream >> observer_description;
    stream >> ui32;
    access_mode = ui32;
    stream >> ui32;
    access_mode_scope = ui32;
    stream >> category_access;
    stream >> deliver_qtilties_property_changed_events;
    bool has_hints;
    stream >> has_hints;
    if (has_hints) {
        if (!display_hints)
            display_hints = new ObserverHints();
        return display_hints->importBinary(stream);
    } else
        return true;
}

