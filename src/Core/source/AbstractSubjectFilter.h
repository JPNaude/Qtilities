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

class QDynamicPropertyChangeEvent;

namespace Qtilities {
    namespace Core {
        class Observer;

        /*!
          \class Qtilities::Core::AbstractSubjectFilter
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
        class QTILIITES_CORE_SHARED_EXPORT AbstractSubjectFilter : public QObject
        {
            Q_OBJECT
            Q_ENUMS(EvaluationResult)

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
            virtual QString filterName() = 0;

            //! Evaluates the attachment of a new subject to the filter's observer context. Use this function to check how an attachment will be handled.
            virtual AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj) const = 0;
            //! Initialize the attachment of a new subject to the filter's observer context.
            /*!
              \param obj The object to be added.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases.
              \return Return true if the attachment is allowed, false otherwise.
              */
            virtual bool initializeAttachment(QObject* obj, bool import_cycle = false) = 0;
            //! Finalize the attachment of a the subject to the filter's observer context.
            /*!
              \param obj The object to be added.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases.
              \param attachment_successful True if the attachment was successfull, false otherwise.
              */
            virtual void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle = false) = 0;
            //! Evaluates the detachment of a subject from the filter's observer context. Use this function to check how an detachment will be handled.
            virtual AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj) const = 0;
            //! Initialize the detachment of a subject from the filter's observer context.
            /*!
              \return Return true if the detachment is allowed, false otherwise.
              \param subject_deleted Indicates if the detachment operation is happening because the subject was deleted. This allows for optimization inside implementations of this function.
              */
            virtual bool initializeDetachment(QObject* obj, bool subject_deleted = false) = 0;
            //! Finalize the detachment of a subject from the filter's observer context.
            /*!
              \param attachment_successful True if the detachment was successfull, false otherwise.
              \param subject_deleted Indicates if the detachment operation is happening because the subject was deleted. This allows for optimization inside implementations of this function.
              */
            virtual void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false) = 0;
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
            virtual bool monitoredPropertyChanged(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) = 0;

            //! Function which returns a list of all the reserved properties monitored and used by this subject filter.
            virtual QStringList monitoredProperties() = 0;

            //! Set the observer context for the subject filter. A subject filter is not usable when an observer context to operate in has not been set.
            void setObserverContext(Observer* observer_context) {
                if (!observer_context)
                    return;

                // At present, it is not possible to set an observer context twice.
                if (observer)
                    return;

                observer = observer_context;
            }

            //! Reimplement this function to export specific binary data about your subject filter type.
            virtual bool exportFilterSpecificBinary(QDataStream& stream) const = 0;
            //! Reimplement this function to import specific binary data about your subject filter type.
            virtual bool importFilterSpecificBinary(QDataStream& stream) = 0;

        signals:
            //! Signal which needs to be emitted when a reserved property changed.
            /*!
                Observer's monitors this signal of all their installed subject filters and in turn emits the propertyBecameDirty() signal which can be used by
                item models to update views as needed when properties change.

                When the property change only affects a single object, the details of this object can be passed on using the obj parameter.
            */
            void notifyDirtyProperty(const char* property_name, QObject* obj = 0);

        protected:
            QMutex filter_mutex;
            Observer* observer;
        };
    }
}

//QDataStream &operator<<(QDataStream &ds,Qtilities::Core::AbstractSubjectFilter &s);
//QDataStream &operator>>(QDataStream &ds,Qtilities::Core::AbstractSubjectFilter &s);

#endif // ABSTRACTSUBJECTFILTER_H
