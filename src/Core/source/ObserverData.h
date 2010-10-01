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
#include "QtilitiesCoreConstants.h"
#include "PointerList.h"
#include "IFactoryProvider.h"
#include "QtilitiesCategory.h"
#include "AbstractSubjectFilter.h"
#include "IExportable.h"

#include <QSharedData>
#include <QObject>
#include <QMutex>
#include <QHash>

namespace Qtilities {
    namespace Core {
        class ObserverHints;
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::Core::Constants;

        /*!
          \class ObserverData
          \brief The ObserverData class contains data which is shared by different references of the same observer.

          Each Observer holds an explicitly shared data pointer to an ObserverData object.

          \sa Observer
          */

        class QTILIITES_CORE_SHARED_EXPORT ObserverData : public QSharedData, public IExportable
        {
        public:
            ObserverData() : subject_limit(-1), subject_id_counter(0),
            filter_subject_events_enabled(false), deliver_qtilities_property_changed_events(false),
            access_mode(0), display_hints(0),
            factory_data(InstanceFactoryInfo(FACTORY_QTILITIES,FACTORY_TAG_OBSERVER,QString())),
            process_cycle_active(false), is_modified(false) {}

            ObserverData(const ObserverData &other) : QSharedData(other), subject_list(other.subject_list) ,
            subject_filters(other.subject_filters), subject_limit(other.subject_limit), subject_id_counter(0),
            filter_subject_events_enabled(other.filter_subject_events_enabled),
            deliver_qtilities_property_changed_events(other.deliver_qtilities_property_changed_events),
            access_mode(other.access_mode), access_mode_scope(other.access_mode_scope),
            categories(other.categories), display_hints(other.display_hints),
            factory_data(other.factory_data), process_cycle_active(other.process_cycle_active),
            is_modified(other.is_modified) {}

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            /*!
              \note QtilitiesCategory is not a QObject, thus it returns 0.
              */
            QObject* objectBase() { return 0; }
            /*!
              \note QtilitiesCategory is not a QObject, thus it returns 0.
              */
            const QObject* objectBase() const { return 0; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            /*!
              \note ObserverData returns an invalid InstanceFactoryInfo object since it is not registered in any factory.
              */
            InstanceFactoryInfo factoryData() const;
            virtual IExportable::Result exportBinary(QDataStream& stream, QList<QVariant> params = QList<QVariant>()) const;
            virtual IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());
            virtual IExportable::Result exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            virtual IExportable::Result importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());

            PointerList subject_list;
            QList<AbstractSubjectFilter*> subject_filters;
            int subject_limit;
            int subject_id_counter;
            int observer_id;
            QString observer_description;
            QMutex observer_mutex;
            bool filter_subject_events_enabled;
            bool deliver_qtilities_property_changed_events;
            int access_mode;
            int access_mode_scope;
            // This list does NOT store all categories in the observer context,
            // only categories for which access modes were defined.
            QList<QtilitiesCategory> categories;
            ObserverHints* display_hints;
            InstanceFactoryInfo factory_data;
            bool process_cycle_active;
            bool is_modified;
        };
    }
}

#endif // OBSERVERDATA_H
