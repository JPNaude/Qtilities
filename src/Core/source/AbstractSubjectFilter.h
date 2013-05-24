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

          In short, a subject filter is used by an observer to evaulate specific operations performed on subjects by observers. Each
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
            AbstractSubjectFilter(QObject* parent = 0) : QObject(parent) {
                observer = 0;
                filter_is_modification_state_monitored = true;
            }
            virtual ~AbstractSubjectFilter() {}

            //! Enumeration which defines the possible results of subject filter evaluation operations.
            /*!
                \sa evaluateAttachment(), evaluateDetachment()
              */
            enum EvaluationResult {
                Allowed,        /*!< Successful validation. */
                Conditional,    /*!< Validation is conditional. %Examples of this is when the user still needs to decide how to rename an object, thus it is not possible to know before hand what the result will be. */
                Rejected        /*!< Validation failed. */
            };

            //! Returns the name of the subject filter.
            virtual QString filterName() const = 0;
            //! Set if this subject filter's modification state is monitored by its observer context.
            /*!
                \note Make sure to set this before you attach the subject filter to your observer context in order for it to work properly.
                \note By default the base class does nothing when this function is called.
              */
            virtual void setIsModificationStateMonitored(bool is_monitored) {
                filter_is_modification_state_monitored = is_monitored;
            }
            //! Get if this subject filter's modification state is monitored by its observer context.
            /*!
                \return True by default.
              */
            virtual bool isModificationStateMonitored() const {
                return filter_is_modification_state_monitored;
            }

            //! Evaluates the attachment of a new subject to the filter's observer context. Use this function to check how an attachment will be handled.
            /*!
                \param obj The object to be evaluated
                \param rejectMsg Provides a reject message when the evaluation failed.
                \param silent When true, the attachment must be done in a silent way. That is, it should not show any dialogs. This is usefull when objects must be attached to observers without calling exec() on a dialog (such as the user input dialog for NamingPolicyFilters) which can lead to crashes when for example attaching objects inside an event filter. For more information on this topic see <a href="http://labs.qt.nokia.com/2010/02/23/unpredictable-exec/">Unpredictable exec()</a>.
                \returns AbstractSubjectFilter::EvaluationResult indicating the result of the evaluation.

                \note By default AbstractSubjectFilter::Allowed is returned by the base class.
              */
            virtual AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj, QString* rejectMsg = 0, bool silent = false) const {
                Q_UNUSED(obj)
                Q_UNUSED(rejectMsg)
                Q_UNUSED(silent)
                return AbstractSubjectFilter::Allowed;
            }
            //! Initialize the attachment of a new subject to the filter's observer context.
            /*!
                \note The object is not yet attached to the observer context when this function is called.

                \param obj The object to be added.
                \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases.
                \return Return true if the attachment is allowed, false otherwise.
                \returns True if the attachment was successfully initialized, false otherwise.

                \note By default true is returned by the base class.
              */
            virtual bool initializeAttachment(QObject* obj, QString* rejectMsg = 0, bool import_cycle = false) {
                Q_UNUSED(obj)
                Q_UNUSED(rejectMsg)
                Q_UNUSED(import_cycle)
                return true;
            }

            //! Finalize the attachment of a the subject to the filter's observer context.
            /*!
                \note When \p attachment_successful is true, the object will already be attached to the observer context.

                \param obj The object to be added.
                \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases.
                \param attachment_successful True if the attachment was successful, false otherwise.

                \note By default does nothing in the base class.
              */
            virtual void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle = false) {
                Q_UNUSED(obj)
                Q_UNUSED(attachment_successful)
                Q_UNUSED(import_cycle)
            }

            //! Evaluates the detachment of a subject from the filter's observer context. Use this function to check how an detachment will be handled.
            /*!
                \param obj The object to be evaluated
                \param rejectMsg Provides a reject message when the evaluation failed.
                \returns AbstractSubjectFilter::EvaluationResult indicating the result of the evaluation.

                \note By default AbstractSubjectFilter::Allowed is returned by the base class.
              */
            virtual AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj, QString* rejectMsg = 0) const {
                Q_UNUSED(obj)
                Q_UNUSED(rejectMsg)
                return AbstractSubjectFilter::Allowed;
            }

            //! Initialize the detachment of a subject from the filter's observer context.
            /*!
                \param obj The object to be detached.
                \param rejectMsg A reject message when the initialization fails.
                \param subject_deleted Indicates if the detachment operation is happening because the subject was deleted. This allows for optimization inside implementations of this function.
                \returns True if the detachment is allowed, false otherwise.

                \note By default true is returned by the base class.
              */
            virtual bool initializeDetachment(QObject* obj, QString* rejectMsg = 0, bool subject_deleted = false) {
                Q_UNUSED(obj)
                Q_UNUSED(rejectMsg)
                Q_UNUSED(subject_deleted)
                return true;
            }

            //! Finalize the detachment of a subject from the filter's observer context.
            /*!
                \param obj The object to be detached.
                \param detachment_successful True if the detachment was successfull, false otherwise.
                \param subject_deleted Indicates if the detachment operation is happening because the subject was deleted. This allows for optimization inside implementations of this function.

                \note By default does nothing in the base class.
              */
            virtual void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false) {
                Q_UNUSED(obj)
                Q_UNUSED(detachment_successful)
                Q_UNUSED(subject_deleted)
            }

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

                \note By default false is returned by the base class.

                \sa monitoredProperties()
              */
            virtual bool handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
                Q_UNUSED(obj)
                Q_UNUSED(property_name)
                Q_UNUSED(propertyChangeEvent)
                return false;
            }

        public:
            //! This function returns a QStringList with the names of all the properties which are monitored by this subject filter.
            /*!
                Monitored properties are all properties that you use on your subject filter that does not appear in the
                reservedProperties() list. All of these properties will be monitored by the observer context in which
                the subject filter is installed. The property change events will be delivered to handleMonitoredPropertyChange().

                When property changes are valid, the monitoredPropertyChanged() signal is emitted as soon as the property
                change is completed.

                \note By default an empty QStringList() is returned by the base class.

                \sa handleMonitoredPropertyChange(), monitoredPropertyChanged()
              */
            virtual QStringList monitoredProperties() const {
                return QStringList();
            }
            //! This function returns a QStringList with the names of all the reserved properties of this subject filter.
            /*!
                Reserved properties are internal properties that cannot be changed. The observer will filter any attempted changes to
                these properties. To check if a property is reserved, see the \p Permisson attribute in the property documentation.
                All %Qtilities properties are defined in the Qtilities::Core::Properties namespace.

                \note By default an empty QStringList() is returned by the base class.
              */
            virtual QStringList reservedProperties() const {
                return QStringList();
            }

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

            //! Returns the observer context that this subject filter is attached to.
            Observer* observerContext() const {
                return observer;
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
            //! The observer context to which the subject filter is attached.
            Observer*           observer;
            QMutex              filter_mutex;
            //! Indicates if the modification state of the filter is monitored by it's observer context.
            bool                filter_is_modification_state_monitored;
        };
    }
}

#endif // ABSTRACTSUBJECTFILTER_H
