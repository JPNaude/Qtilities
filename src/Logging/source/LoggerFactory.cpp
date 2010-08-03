/****************************************************************************
**
** Copyright 2009, Jaco Naude
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library;  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "LoggerFactory.h"

#include <QDataStream>

quint32 MARKER_FACTORY_TAG = 0xFACFACFA;

QDataStream &operator<<(QDataStream &ds, Qtilities::Logging::LoggerFactoryData &s) {
    ds << MARKER_FACTORY_TAG;
    ds << s.d_factory_tag;
    ds << MARKER_FACTORY_TAG;
    return(ds);
}

QDataStream &operator>>(QDataStream &ds, Qtilities::Logging::LoggerFactoryData &s) {
    quint32 ui32;
    ds >> ui32;
    Q_ASSERT(ui32 == MARKER_FACTORY_TAG);
    ds >> s.d_factory_tag;
    Q_ASSERT(ui32 == MARKER_FACTORY_TAG);
    return(ds);
}
