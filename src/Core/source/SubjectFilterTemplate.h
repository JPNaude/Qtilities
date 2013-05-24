/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
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
