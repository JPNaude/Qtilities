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

#ifndef OBSERVERMIMEDATA_H
#define OBSERVERMIMEDATA_H

#include "QtilitiesCore_global.h"

#include <QMimeData>
#include <QList>
#include <QPointer>

namespace Qtilities {
    namespace Core {
        /*!
          \class ObserverMimeData
          \brief The ObserverMimeData stores information about subjects. Example usage scenarios are during drag-drop and clipboard operations.
         */
        class QTILIITES_CORE_SHARED_EXPORT ObserverMimeData : public QMimeData {
            Q_OBJECT

        public:
            ObserverMimeData(QList<QPointer<QObject> > subject_list, int source_id, Qt::DropAction drop_action) : QMimeData() {
                d_source_id = source_id;
                d_subject_list = subject_list;
                d_drop_action = drop_action;
            }
            ObserverMimeData(const ObserverMimeData& other) : QMimeData(),
                d_source_id(other.sourceID()),
                d_subject_list(other.subjectList()),
                d_drop_action(other.dropAction()) {}

            //! Gets the ID of the observer which populated the mime data object.
            int sourceID() const { return d_source_id; }
            //! Gets the list of subjects to which the mime data object applies.
            QList<QPointer<QObject> > subjectList() const { return d_subject_list; }
            //! Gets the drop action assocaited with this mime data object when used during a drop action.
            Qt::DropAction dropAction() const { return d_drop_action; }

        private:
            int                         d_source_id;
            QList<QPointer<QObject> >   d_subject_list;
            Qt::DropAction              d_drop_action;
        };
    }
}

#endif // OBSERVERMIMEDATA_H
