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

#ifndef OBSERVERDATA_H
#define OBSERVERDATA_H

#include "QtilitiesCore_global.h"
#include "AbstractSubjectFilter.h"
#include "PointerList.h"
#include "IFactory.h"

#include <QSharedData>
#include <QObject.h>
#include <QMutex.h>
#include <QHash>

namespace Qtilities {
    namespace Core {
        class ObserverHints;
        using namespace Qtilities::Core::Interfaces;

        /*!
          \class ObserverData
          \brief The ObserverData class contains data which is shared by different references of the same observer.

          Each Observer holds an explicitly shared data pointer to an ObserverData object.

          \sa Observer
          */

        class QTILIITES_CORE_SHARED_EXPORT ObserverData : public QSharedData
        {
        public:
            ObserverData() : subject_limit(-1), subject_id_counter(0),
            ignore_dynamic_property_changes(false), deliver_qtilties_property_changed_events(false),
            access_mode(0), display_hints(0), factory_data(IFactoryData()), process_cycle_active(false), is_modified(false) {}
            ObserverData(const ObserverData &other) : QSharedData(other), subject_list(other.subject_list) ,
            subject_filters(other.subject_filters), subject_limit(other.subject_limit), subject_id_counter(0),
            ignore_dynamic_property_changes(other.ignore_dynamic_property_changes),
            deliver_qtilties_property_changed_events(other.deliver_qtilties_property_changed_events),
            access_mode(other.access_mode), access_mode_scope(other.access_mode_scope),
            category_access(other.category_access), display_hints(other.display_hints),
            factory_data(other.factory_data), process_cycle_active(other.process_cycle_active),
            is_modified(other.is_modified) {}

            //! Exports observer data to a QDataStream.
            /*!
              \returns True if successful, false otherwise.
              */
            bool exportBinary(QDataStream& stream) const;
            //! Imports observer data from a QDataStream.
            /*!
              \returns True if succesfull, false otherwise.
              */
            bool importBinary(QDataStream& stream);

            PointerList<QObject> subject_list;
            PointerList<AbstractSubjectFilter> subject_filters;
            int subject_limit;
            int subject_id_counter;
            int observer_id;
            QString observer_description;
            QMutex observer_mutex;
            bool ignore_dynamic_property_changes;
            bool deliver_qtilties_property_changed_events;
            int access_mode;
            int access_mode_scope;
            QHash<QString, int> category_access;
            ObserverHints* display_hints;
            IFactoryData factory_data;
            bool process_cycle_active;
            bool is_modified;
        };
    }
}

#endif // OBSERVERDATA_H
