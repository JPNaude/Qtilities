/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
        class ObserverRelationalTable;
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
            //! The possible export flags used during extended observer exports.
            /*!
              \sa exportBinaryExt(), exportXMLExt()
              */
            enum ExportItem {
                ExportData                  = 1, /*!< Exports all observer data, subjects and their children. */
                ExportVisitorIDs            = 2, /*!< XML Only: Indicates that VisitorIDs must be added to subject nodes. This is needed when ExportRelationalData is used, and therefore it is automatically enabled in that case.  */
                ExportRelationalData        = 4, /*!< Indicates that an ObserverRelationalTable must be constructed for the observer and it must be exported with the observer data. During extended imports the relational structure of the tree under your observer will be reconstructed. */
                ExportAllItems             = ExportData | ExportVisitorIDs | ExportRelationalData
            };
            Q_DECLARE_FLAGS(ExportItemFlags, ExportItem);
            Q_FLAGS(ExportItemFlags);

            ObserverData(Observer* obs, const QString& observer_name) : subject_limit(-1),
                subject_id_counter(0),
                filter_subject_events_enabled(false),
                deliver_qtilities_property_changed_events(false),
                access_mode(0),
                display_hints(0),
                factory_data(InstanceFactoryInfo(qti_def_FACTORY_QTILITIES,qti_def_FACTORY_TAG_OBSERVER,QString())),
                start_processing_cycle_count(0),
                process_cycle_active(false),
                is_modified(false),
                observer(obs),
                object_deletion_policy(0),
                number_of_subjects_start_of_proc_cycle(0),
                broadcast_modification_state_changes(true)
            {
                subject_list.setObjectName(observer_name);
            }

            ObserverData(const ObserverData &other) : QSharedData(other),
                subject_list(other.subject_list),
                subject_filters(other.subject_filters),
                subject_limit(other.subject_limit),
                subject_id_counter(0),
                filter_subject_events_enabled(other.filter_subject_events_enabled),
                deliver_qtilities_property_changed_events(other.deliver_qtilities_property_changed_events),
                access_mode(other.access_mode),
                access_mode_scope(other.access_mode_scope),
                categories(other.categories),
                display_hints(other.display_hints),
                factory_data(other.factory_data),
                start_processing_cycle_count(other.start_processing_cycle_count),
                process_cycle_active(other.process_cycle_active),
                is_modified(other.is_modified),
                observer(other.observer),
                object_deletion_policy(other.object_deletion_policy),
                number_of_subjects_start_of_proc_cycle(0),
                broadcast_modification_state_changes(true) {}

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
            void setExportVersion(Qtilities::ExportVersion version);
            void setExportTask(ITask* task);
            void clearExportTask();
            IExportable::ExportResultFlags exportBinary(QDataStream& stream) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // Extended Access Call Functions From Observer
            // --------------------------------
            //! Extended binary export function.
            IExportable::ExportResultFlags exportBinaryExt(QDataStream& stream, ExportItemFlags export_flags) const;
            //! Extended XML export function.
            IExportable::ExportResultFlags exportXmlExt(QDomDocument* doc, QDomElement* object_node, ExportItemFlags export_flags) const;

            // --------------------------------
            // Export Implementations For Different Qtilities Versions
            // --------------------------------
        private:
            IExportable::ExportResultFlags exportBinaryExt_1_0(QDataStream& stream, ExportItemFlags export_flags) const;
            IExportable::ExportResultFlags importBinaryExt_1_0(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXmlExt_1_0(QDomDocument* doc, QDomElement* object_node, ExportItemFlags export_flags) const;
            IExportable::ExportResultFlags importXmlExt_1_0(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            //! Construct relationships between a list of objects with the relational data being passed to the function as a RelationalObserverTable.
            bool constructRelationships(QList<QPointer<QObject> >& objects, ObserverRelationalTable* table) const;
            //! Creates a list of exportable subjects for cases where ExportRelationalData is enabled.
            /*!
              In this case, we need to make sure objects appearing multiple times in the tree is not exported more than once. This is done using qti_prop_LIMITED_EXPORTS.
              */
            QList<IExportable*> getLimitedExportsList(QList<QObject* > objects, IExportable::ExportMode export_mode, bool * incomplete = 0) const;

            // --------------------------------
            // All Data Stored For An Observer
            // --------------------------------
        public:
            PointerList                         subject_list;
            QList<AbstractSubjectFilter*>       subject_filters;
            int                                 subject_limit;
            int                                 subject_id_counter;
            int                                 observer_id;
            QString                             observer_description;
            QMutex                              observer_mutex;
            bool                                filter_subject_events_enabled;
            bool                                deliver_qtilities_property_changed_events;
            int                                 access_mode;
            int                                 access_mode_scope;
            // This list does NOT store all categories in the observer context,
            // only categories for which access modes were defined.
            QList<QtilitiesCategory>            categories;
            ObserverHints*                      display_hints;
            InstanceFactoryInfo                 factory_data;
            int                                 start_processing_cycle_count;
            bool                                process_cycle_active;
            bool                                is_modified;
            //! Keeps track of its parent observer:
            Observer*                           observer;
            int                                 object_deletion_policy;
            //! Used during processing cycles to store the number of subjects before the processing cycle started. This allows correct emission of numberOfSubjectsChanged() when the processing cycle ends.
            /*!
              \note When setting number_of_subjects_start_of_proc_cycle to -1, the numberOfSubjectsChanged() signal will not be emitted in endProcessingCycle(). This
              is used in the ~Observer destructor. We don't want to emit that the number of subjects changed in the destructor because all subjects will be removed.
              */
            int                                 number_of_subjects_start_of_proc_cycle;
            //! Used internally in Observer to optimize the number of times modification state changes are broadcast.
            bool                                broadcast_modification_state_changes;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(ObserverData::ExportItemFlags)
    }
}

#endif // OBSERVERDATA_H
