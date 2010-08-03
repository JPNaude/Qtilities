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

#include <ObserverData.h>

QDataStream &operator<<(QDataStream &ds, Qtilities::Core::ObserverData &s) {
    ds << (qint32) s.subject_limit;
    ds << (qint32) s.subject_id_counter;
    ds << s.observer_description;
    ds << (qint32) s.access_mode;
    ds << (qint32) s.access_mode_scope;
    ds << s.category_access;
    return(ds);
}

QDataStream &operator>>(QDataStream &ds, Qtilities::Core::ObserverData &s) {
    qint32 i32;

    ds >> i32;
    s.subject_limit = i32;
    ds >> i32;
    s.subject_id_counter = i32;
    ds >> s.observer_description;
    ds >> i32;
    s.access_mode = i32;
    ds >> i32;
    s.access_mode_scope = i32;
    ds >> s.category_access;
    return(ds);
}
