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

#ifndef ABSTRACTSUBJECTFILTER_H
#define ABSTRACTSUBJECTFILTER_H

#include <QObject>
#include <QMutex>

#include "QtilitiesCore_global.h"
#include "IExportable.h"

class QDynamicPropertyChangeEvent;

namespace Qtilities {
    namespace Core {
        class Observer;
        using namespace Qtilities::Core::Interfaces;

        /*!
          \class AbstractSubjectFilter
          \brief The AbstractSubjectFilter class provides the abstract interface for observer subject filters.

          The AbstractSubjectFilter class defines the standard interface that subject filters must use to be able to interoperate with observers.
          It is not supposed and cannot to be instantiated directly. Instead, you should subclass it to create new subject filters.

          In short, an subject filter is used by an observer to evaulate specific actions performed on subjects by observers. Each
          subject filter needs an observer context which is set using setObserverContext(). Without the observer context set, the filter is not usable.
          When installing subject filters using the Qtilities::Core::Observer::installSubjectFilter() function the observer context
          will automatically be set. On the other hand, Qtilities::Core::Observer::uninstallSubjectFilter() will set the observer
          context to 0.

          For a detailed overview of subject filters, see the \ref subject_filters section of the \ref page_observers article.
          */
        class QTILIITES_CORE_SHARED_EXPORT AbstractSubjectFilter : public QObject, virtual public IExportable
        {
            Q_OBJECT
            Q_ENUMS(EvaluationResult)

            friend class Observer;

        public:
            AbstractSubjectFilter(QObject* parent = 0) : QObject(parent) { observer = 0; }
            virtual ~AbstractSubjectFilter() {}

            //! Enumeration which defines the possible results of subject filter evaluation operations.
            /*!
              \sa evaluateAttachment(), evaluateDetachment()
              */
            enum EvaluationResult {
                Allowed,        /*!< Successful validation. */
                Conditional,    /*!< Validation is conditional. Examples of this is when the user still needs to decide how to rename an object, thus it is not possible to know before hand what the result will be. */
                Rejected        /*!< Validation failed. */
            };

            //! Returns the name of the subject filter.
            virtual QString filterName() const = 0;
            //! Set if this subject filter must be exported.
            virtual void setIsExportable(bool is_exportable) = 0;
            //! Indicates if this subject filter must be exported.
            /*!
              Default is true.
              */
            virtual bool isExportable() const = 0;

            //! Evaluates the attachment of a new subject to the filter's observer context. Use this function to check how an attachment will be handled.
            virtual AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj, QString* rejectMsg = 0, bool silent = false) const = 0;
            //! Initialize the attachment of a new subject to the filter's observer context.
            /*!
              \note The object is not yet attached to the observer context when this function is called.

              \param obj The object to be added.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases.
              \return Return true if the attachment is allowed, false otherwise.
              */
            virtual bool initializeAttachment(QObject* obj, QString* rejectMsg = 0, bool import_cycle = false) = 0;
            //! Finalize the attachment of a the subject to the filter's observer context.
            /*!
              \note When \p attachment_successful is true, the object will already be attached to the observer context.

              \param obj The object to be added.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases.
              \param attachment_successful True if the attachment was successful, false otherwise.
              */
            virtual void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle = false) = 0;
            //! Evaluates the detachment of a subject from the filter's observer context. Use this function to check how an detachment will be handled.
            virtual AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj, QString* rejectMsg = 0) const = 0;
            //! Initialize the detachment of a subject from the filter's observer context.
            /*!
              \return Return true if the detachment is allowed, false otherwise.
              \param subject_deleted Indicates if the detachment operation is happening because the subject was deleted. This allows for optimization inside implementations of this function.
              */
            virtual bool initializeDetachment(QObject* obj, QString* rejectMsg = 0, bool subject_deleted = false) = 0;
            //! Finalize the detachment of a subject from the filter's observer context.
            /*!
              \param attachment_successful True if the detachment was successfull, false otherwise.
              \param subject_deleted Indicates if the detachment operation is happening because the subject was deleted. This allows for optimization inside implementations of this function.
              */
            virtual void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false) = 0;
        protected:
            //! Function which should react to QDynamicPropertyChangeEvents on properties which are reserved by the subject filter.
            /*!
              As soon as a subject is attached to an observer the observer installs a event filter on the subject which monitors dynamic property changes.
              When a dynamic property change event is received, the observer will route the event to the appropriate subject filter and call this function
              on the subject filter. Thus, this function will only be called when the property_name is a reserved property of the subject filter.

              \param obj The object on which the property change took place.
              \param property_name The property name which changed.
              \param propertyChangeEvent The QDynamicPropertyChangeEvent which triggered the event.

              \return Return true if the event should be filtered (thus, the event will not be allowed to finish and the property will not change), false otherwise when
              the change event should be allowed (thus, the event will not be filtered and the property will change if not filtered by any other subject filters).

              \sa monitoredProperties()
              */
            virtual bool handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) = 0;

        public:
            //! This function returns a QStringList with the names of all the properties which are monitored by this subject filter.
            /*!
              Monitored properties are all properties that you use on your subject filter that does not appear in the
              reservedProperties() list. All of these properties will be monitored by the observer context in which
              the subject filter is installed. The property change events will be delivered to handleMonitoredPropertyChange().

              When property changes are valid, the monitoredPropertyChanged() signal is emitted as soon as the property
              change is completed.

              \sa handleMonitoredPropertyChange(), monitoredPropertyChanged()
              */
            virtual QStringList monitoredProperties() const = 0;
            //! This function returns a QStringList with the names of all the reserved properties of this subject filter.
            /*!
              Reserved properties are internal properties that cannot be changed. The observer will filter any attempted changes to
              these properties. To check if a property is reserved, see the \p Permisson attribute in the property documentation.
              All %Qtilities properties are defined in the Qtilities::Core::Properties namespace.
              */
            virtual QStringList reservedProperties() const = 0;

            //! Set the observer context for the subject filter. A subject filter is not usable when an observer context to operate in has not been set.
            /*!
              \note The observer context of a subject filter can only be set once. Thus, you cannot use a subject filter more than once. Once you uninstalled it from an observer, you must delete it.
              */
            virtual bool setObserverContext(Observer* observer_context) {
                if (!observer_context)
                    return false;

                if (observer)
                    return false;

                observer = observer_context;
                return true;
            }

        signals:
            //! A signal which is emitted as soon as a monitored property of the observer or any of the installed subject filters changed.
            /*!
              The observer context in which the subject filter is installed will listen to this signal and emit
              the Qtilities::Core::Observer::monitoredPropertyChanged() signal whenever this signal is emitted.

              \param property_name The name of the property which changed.
              \param objects The objects on which the property changed.

              \sa monitoredProperties(), handleMonitoredPropertyChange()
              */
            void monitoredPropertyChanged(const char* property_name, QList<QObject*> objects = QList<QObject*>());
            //! A signal which is emitted as soon as an property change event is filtered.
            /*!
              This signal can be emitted in two scenarios:
              - When an attempt is made to modify a reserved property it will always be emitted on the observer level.
              - When a monitored property change is not allowed. An example of this is when a name change is rejected.

              \param property_name The name of the property on which the change was filtered.
              \param objects The objects on which the property was attempted.

              \sa reservedProperties(), monitoredProperties()
              */
            void propertyChangeFiltered(const char* property_name, QList<QObject*> objects = QList<QObject*>());

        protected:
            QMutex              filter_mutex;
            Observer*           observer;
        };
    }
}

#endif // ABSTRACTSUBJECTFILTER_H
