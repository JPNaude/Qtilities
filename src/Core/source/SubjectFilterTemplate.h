/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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

#ifndef SUBJECTFILTERTEMPLATE_H
#define SUBJECTFILTERTEMPLATE_H

#include "AbstractSubjectFilter.h"
#include "IModificationNotifier.h"
#include "Factory.h"

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        /*!
          \struct SubjectFilterTemplatePrivateData
          \brief The SubjectFilterTemplatePrivateData class stores private data used by the SubjectFilterTemplate class.
         */
        struct SubjectFilterTemplatePrivateData;
	
        /*!
        \class SubjectFilterTemplate
        \brief A template subject filter.

        Creating custom subject filters is an easy task if you understand how the initialization and
        finalization diagrams shown in the \ref subject_filters section of the \ref page_observers article work.
        The SubjectFilterTemplate class should be used as a starting point when creating new subject filters.
        The new filter class needs to inherit from Qtilities::Core::AbstractSubjectFilter and reimplement the virtual
        abstract functions in order to work. The source code of the subject filters which comes as part of
        %Qtilities is a good place to start when looking for examples of filter implementations.
          */	
        class QTILIITES_CORE_SHARED_EXPORT SubjectFilterTemplate : public AbstractSubjectFilter, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
            SubjectFilterTemplate(QObject* parent = 0);
            ~SubjectFilterTemplate();
            QString filterName() { return tr("Subject Filter Template"); }

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<AbstractSubjectFilter, SubjectFilterTemplate> factory;

            // --------------------------------
            // AbstractSubjectFilter Implementation
            // --------------------------------
            AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj, QString* rejectMsg = 0, bool silent = false) const;
            bool initializeAttachment(QObject* obj, QString* rejectMsg, bool import_cycle = false);
            void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle = false);
            AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj, QString* rejectMsg = 0) const;
            bool initializeDetachment(QObject* obj, QString* rejectMsg = 0, bool subject_deleted = false);
            void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false);
            QString filterName() const { return "Subject Filter Template"; }
            QStringList monitoredProperties() const;
            QStringList reservedProperties() const;
        protected:
            bool handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        private:
            SubjectFilterTemplatePrivateData* d;
        };
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::SubjectFilterTemplate& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::SubjectFilterTemplate& stream_obj);

#endif // SUBJECTFILTERTEMPLATE_H
