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

#ifndef OBSERVER_H
#define OBSERVER_H

#include "QtilitiesCore_global.h"
#include "QtilitiesCoreApplication.h"
#include "QtilitiesCoreConstants.h"
#include "PointerList.h"
#include "QtilitiesProperty.h"
#include "ObserverData.h"
#include "IExportable.h"
#include "IFactoryProvider.h"
#include "IModificationNotifier.h"
#include "SubjectTypeFilter.h"
#include "QtilitiesCategory.h"
#include "IExportableObserver.h"

#include <QObject>
#include <QString>
#include <QPointer>
#include <QList>
#include <QStringList>

namespace Qtilities {
    namespace Core {
        class AbstractSubjectFilter;
        class ObserverMimeData;

        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::Core::Properties;

        /*!
        \class Observer
        \brief The observer class is an extended implementation of an observer in the subject-observer pattern.

        The observer class provides a powerful implementation of the subject - observer programming pattern. The observer class is the observer in this implementation (as the name suggests) and any QObject based class can be a subject. One way to think about an observer is to think of it as a context in your application to which certain subjects can be attached or detached. An example of such a context is the observer which manages plugins in the %Qtilities extension system. When plugins are loaded they are attached to this context and become visible in the list of loaded plugins. Another example would be different instances of a scripting engine. When new objects are created inside the scripting engine they exist in that context. It can then for example be possible to have multiple scripting engines within the same application, where different objects belongs to different script engines or are shared between the engines using Observers.

        The example below shows how to create your first observer class and attach objects to it.

\code
// Create the observer:
Observer* observerA = new Observer;

// Create the objects:
QPointer<QObject> object1 = new QObject();
QPointer<QObject> object2 = new QObject();

// Attach objects to observers:
observerA->attachSubject(object1);
observerA->attachSubject(object2);
\endcode

        \section observer_displaying Displaying the context of an Observer

        One of the original goals of the observer implementation was to make it easy, in fact very easy to display the contents of any Observer context. For this purpose the Qtilities::CoreGui::ObserverWidget class was created which allows you to display the contents of an observer context using only a couple lines of code.

        For example:
\code
// Create the observer:
Observer* observerA = new Observer;

// Create the objects:
QPointer<QObject> object1 = new QObject();
QPointer<QObject> object2 = new QObject();

// Attach objects to observers:
observerA->attachSubject(object1);
observerA->attachSubject(object2);

// Display the observer context:
ObserverWidget observerWidget(observerA);
observerWidget.show();
\endcode

        %Observer widgets are very powerful and observers can specify display hints to views that display their contents using the Qtilities::Core::ObserverHints class. This interaction is a topic on its own and the \ref page_observer_widgets page discusses this in detail. Observer widgets supports both Table and Tree views and the Qtilities::CoreGui::TreeNode class is a subclass of observer that makes building of trees using observer easier. The next section describes such trees in more detail.

        \section observer_trees Observer Trees

        Since an observer is able to manage any QObject based class, it is also capable of observing other observers. This feature allows us to build complex hierarchical tree data structures very easily with the only requirements that leaf nodes in our trees must inherit QObject. Using observer widgets you can easily display and tree data structure that you have created and you can even make dot graphs of your tree structures using ObserverDotWriter.

        Lets look at a simple observer tree structure and the functions provided by observer:

        \image html observer_tree_classification.jpg "Observer Trees"

        In the above tree \p Root, \p A and \p B are all observers with \p 1-4 being any QObject based classes. On the \p Root observer it is possible to access its subjects using functions on observer that start with \p subject. For example: subjectCount(), subjectLimit(), subjectAt() etc. Thus these functions allow you to access the objects directly underneath an observer. On the other hand, function starting with \p tree allow you to operate on the complete tree underneath an observer. For example: treeCount(), treeAt() etc.

        %Qtilities provides the SubjectIterator and TreeIterator iterators which allows you to iterate over different parts of the tree structure underneath an observer.

        A common request by Qt developers is the ability to easily create and display data based on arbitrary tree structures to users, and to easily interact with users through these trees. It is of course doable using Qt's QAbstractItemModel but its not a trivial tasks and can take time to get right. %Qtilities attempts to provide an easy solution to this request in the form of ready to use classes which allows you to build trees easily. The \ref page_tree_structures page provides a detailed overview of this solution.

        \section observer_subject_filters Subject filters

    Subject filters are a feature of the observer architecture which allows control over object attachment and detachment, as well as monitoring of properties introduced by the subject filter. For more information on this see the \ref subject_filters section of the \ref page_observers page.

        \section observer_threads Observers and threads

        Since observers manage objects which can live in different threads it is important to take care when using observers outside of the GUI thread. The following considerations must be taken into account when using observers in threads outside of the GUI thread and attaching objects to observers that live in different threads:
        - Observers monitor QDynamicPropertyChange events on objects that it manages, and send QtilitiesPropertyChangeEvent events on specific internal %Qtilities properties to objects that it manages. Since properties cannot be posted to objects living in other threads, and events cannot be filtered on objects in a different thread, this functionality of observers cannot be used when using threads as specified above. Event filtering is enabled by default, thus if you intend to use observers which manage objects in different threads, disable this using toggleSubjectEventFiltering().
        - When using observer's with naming policy filters installed outside of the GUI thread, make sure you don't use the Qtilities::CoreGui::NamingPolicyFilter::PromptUser resolution policy since it will attempt to construct a QWidget under specific circumstances and you application will crash.

        Observer by itself is not thread-safe.

        \section observer_under_the_hood Under The Hood: How observers work behind the scenes.

        From a user perspective the observer API attempts to hide the complexities of how object management is done. Behind the scenes there is a lot that is happening and lots of complex features that allows you to customize the way you use observers. The \ref page_observers article is a good place to start exploring these details.
        */
        class QTILIITES_CORE_SHARED_EXPORT Observer : public QObject, public IExportable, public IModificationNotifier, public IExportableObserver
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportableObserver)
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)
            Q_ENUMS(SubjectChangeIndication)
            Q_ENUMS(ObjectOwnership)
            Q_ENUMS(EvaluationResult)
            Q_ENUMS(AccessMode)

            Q_PROPERTY(QString Name READ observerName)
            Q_PROPERTY(QString Description READ observerDescription)
            Q_PROPERTY(int SubjectLimit READ subjectLimit WRITE setSubjectLimit)
            Q_PROPERTY(int SubjectCount READ subjectCount)
            Q_PROPERTY(AccessMode Access READ accessMode)
            Q_PROPERTY(int ID READ observerID)
            Q_PROPERTY(bool Modified READ isModified())
            Q_PROPERTY(int ProcCycleCount READ processingCycleCount())

        public:
            // --------------------------------
            // Enumerations
            // --------------------------------
            //! This enumeration is used to return results when validating attachment and detachment of subjects.
            /*!
              \sa canAttach(), canDetach()
              */
            enum EvaluationResult {
                Allowed,                    /*!< Indicates that the attachment/detachment operation will be valid. */
                Conditional,                /*!< Indicates that the attachment/detachment operation's validity will be dependent on the user input. An example of this is when the object's name is not valid in the context and the naming policy filter is set to prompt the user for the action to take (Reject, Rename etc.). */
                Rejected,                   /*!< Indicates that the attachment/detachment operation will be invalid. */
                IsParentObserver,           /*!< Only used during detachment. Indicates that the observer is the parent of the object. This result takes priority over the other possible results. The use case is where the Object Manager attempts to move objects between observers. When attempting to move objects between two observers it will not move subjects which returns this during their detachment validation. */
                LastScopedObserver          /*!< Only used during detachment. Indicates that the observer is the last scoped parent of the object. This result takes priority over the other possible results except IsParentObserver. The use case is where the Object Manager attempts to move objects between observers. When attempting to move objects between two observers it will not move subjects which returns this during their detachment validation. */
            };
            //! The possible indications that can be returned when the number of subjects in the observer changes.
            /*!
              \sa numberOfSubjectsChanged()
              */
            enum SubjectChangeIndication {
                SubjectAdded,               /*!< Indicates that subjects were added. */
                SubjectRemoved,             /*!< Indicates that subjects were removed. */
                CyclicProcess               /*!< Indicates that the number of subjects changed during a cyclic process. The subject count can either be less or more than before the cyclic operation. \sa startProcessingCycle(), endProcessingCycle() */
            };
            //! The possible ownerships with which subjects can be attached to an observer.
            /*!
              See the \ref object_lifetimes section of the \ref page_observers article for a detailed discussion.
              */
            enum ObjectOwnership {
                ManualOwnership,            /*!<
Manual ownership means that the object won't be managed by the observer, thus the ownership will be managed the normal Qt way. If \p parent() = 0, it will not be managed, if \p parent() is an QObject, the subject will be deleted when its parent is deleted.

\code
// Create the observer
Observer* observerA = new Observer("Observer A","My first observer");

// Create the objects
QPointer<QObject> object1 = new QObject();
object1->setObjectName("Object 1");
QPointer<QObject> object2 = new QObject();
object2->setObjectName("Object 2");

// Attach objects to observers
observerA->attachSubject(object1,Observer::ManualOwnership);
observerA->attachSubject(object2,Observer::ManualOwnership);

// Now delete observers
delete observerA;

// Check the validity of the objects
if (object1) {
    // We get here in this example.
} else {

}
if (object2) {
    // We get here in this example.
} else {

}
\endcode

After deleting the two observers in the example above, \p object1 and \p object2 will still be valid, thus they will not be deleted. Note that subjects are attached to observers using manual ownership by default.
                */
                AutoOwnership,              /*!<
Auto ownership means that the observer will automatically decide how to manage the subject. The observer checks if the object already has a parent(), if so \p ManualOwnership is used. If no \p parent() is specified yet, the observer will attach the subject using \p ObserverScopeOwnership.

\code
// Create the observer
Observer* observerA = new Observer("Observer A","My first observer");

// Create the objects
QPointer<QObject> object1 = new QObject();
object1->setObjectName("Object 1");
QPointer<QObject> object2 = new QObject();
object2->setObjectName("Object 2");
QPointer<QObject> parentObject = new QObject();
object2->setParent(parentObject);

// Attach objects to observers
observerA->attachSubject(object1,Observer::AutoOwnership);
observerA->attachSubject(object2,Observer::AutoOwnership);

// Now delete observers
delete observerA;

// Check the validity of the objects
if (object1) {

} else {
    // We get here in this example.
}
if (object2) {
    // We get here in this example.
} else {

}
\endcode

Since \p object2 has a parent, it will be attached using \p ManualOwnership and \p ObserverScopeOwnership will be used for \p object1. Therefore after deleting the observers, \p object1 will be null (see \ref observer_scope_ownership) and \p object2 will still be valid.

                */
                SpecificObserverOwnership,  /*!<
The observer becomes the parent of the subject (by calling setParent() on the object). That is, when the observer is deleted, the subject is also deleted.

\code
// Create the observer
Observer* observerA = new Observer("Observer A","My first observer");

// Create the objects
QPointer<QObject> object1 = new QObject();
object1->setObjectName("Object 1");
QPointer<QObject> object2 = new QObject();
object2->setObjectName("Object 2");

// Attach objects to observers
observerA->attachSubject(object1,Observer::SpecificObserverOwnership);
observerA->attachSubject(object2,Observer::SpecificObserverOwnership);

// Now delete observer
delete observerA;

// Check the validity of the objects
if (object1) {

} else {
    // We get here in this example.
}
if (object2) {

} else {
    // We get here in this example.
}
\endcode

Both objects will become null after the observers are deleted since \p observerA becomes the specific parent of both objects.

\note QWidget's parent must be another QWidget, thus these rules don't apply to QWidgets.
                */
                ObserverScopeOwnership,     /*!<
The object must have at least one Observer parent at any time. That is, when the object is attached to multiple observers, it will stay valid until it goes out of scope. This can happen because all its observer parents gets deleted, or it is detached from all contexts.

\code
// Create the observer
Observer* observerA = new Observer("Observer A","My first observer");
Observer* observerB = new Observer("Observer B","My second observer");

// Create the objects
QPointer<QObject> object1 = new QObject();
object1->setObjectName("Object 1");
QPointer<QObject> object2 = new QObject();
object2->setObjectName("Object 2");

// Attach objects to observers
observerA->attachSubject(object1,Observer::ObserverScopeOwnership);
observerA->attachSubject(object2,Observer::ObserverScopeOwnership);
observerB->attachSubject(object1,Observer::ObserverScopeOwnership);
observerB->attachSubject(object2,Observer::ObserverScopeOwnership);

// Now delete observer A
delete observerA;

// Check the validity of the objects
if (object1) {
    // We get here in this example.
} else {

}
if (object2) {
    // We get here in this example.
} else {

}

// Now delete observer B
delete observerB;

// Check the validity of the objects
if (object1) {

} else {
    // We get here in this example.
}
if (object2) {

} else {
    // We get here in this example.
}
\endcode

Both objects will still be valid after deleting \p observerA since it they are still visible in the scope of \p observerB. After \p observerB is deleted both objects will be null.

\note QWidget's parent must be another QWidget, thus these rules don't apply to QWidgets.
                */
                OwnedBySubjectOwnership     /*!<
The observer is dependent on the subject, thus the subject effectively owns the observer. When the subject is deleted, the observer is also deleted. When the observer is deleted it checks if the subject is attached to any other observers and if not it deletes the subject as well. If the subject is attached to any other observers, the subject is not deleted. When the current ownership of a subject is \p OwnedBySubjectOwnership and it is attached to more contexts, the new ownership is ignored during attachment to the new contexts. Thus when a subject was attached to a context using \p OwnedBySubjectOwnership it is attached to all other contexts after that using \p OwnedBySubjectOwnership as well. On the other hand, when a subject is already attached to one or more observer contexts and it is attached to a new observer using \p OwnedBySubjectOwnership, the old ownership is kept and the observer only connects the destroyed() signal on the object to its own deleteLater() signal.

\code
// Create the observer
QPointer<Observer> observerA = new Observer("Observer A","My first observer");

// Create the objects
QPointer<QObject> object1 = new QObject();
object1->setObjectName("Object 1");

// Attach objects to observers
observerA->attachSubject(object1,Observer::OwnedBySubjectOwnership);

// Now delete the object
delete object1;

// Check the validity of the observer
if (observerA) {

} else {
    // We get here in this example.
}
\endcode

In this example \p observerA will be deleted as soon as \p object1 is deleted.

\note QWidget's parent must be another QWidget, thus these rules don't apply to QWidgets.
                */
            };
            //! Function which returns a string associated with a specific ObjectOwnership.
            static QString objectOwnershipToString(ObjectOwnership ownership);
            //! Function which returns the ObjectOwnership associated with a string.
            static ObjectOwnership stringToObjectOwnership(const QString& ownership_string);
            //! The possible access modes of the observer.
            /*!
              \sa setAccessMode(), accessMode()
              */
            enum AccessMode {
                FullAccess = 0,             /*!< All observer operations are available to the user (Attachment, Detachment etc.). */
                ReadOnlyAccess = 1,         /*!< The observer is read only to the user. */
                LockedAccess = 2,           /*!< The observer is read only and locked. Item views presenting this observer to the user will respect the LockedAccess mode and will not display the contents of the observer to the user. */
                InvalidAccess = 3           /*!< An invalid access mode. This access mode is returned in functions where the access mode is requested for a category that does not exist, for example categoryAccessMode(). */
            };
            //! Function which returns a string associated with a specific AccessMode.
            static QString accessModeToString(AccessMode access_mode);
            //! Function which returns the AccessMode associated with a string.
            static AccessMode stringToAccessMode(const QString& access_mode_string);
            //! The possible deletion methods that Observer can use when deleting objects attached to it.
            /*!
              By default DeleteLater is used, thus it is safe when deleting objects living in threads other than the observer itself. One drawback of using DeleteLater is that the ObserverWidgets viewing your Observer will only be updated when your object is actually deleted. This can cause your views to take a while to be updated. If you are sure that your objects live in the same thread as the observer you should use DeleteImmediately to avoid this behaviour.

              \sa setObjectDeletionPolicy(), objectDeletionPolicy()
              */
            enum ObjectDeletionPolicy {
                DeleteImmediately = 0,     /*!< When subjects are deleted by this observer, the normal \p delete operator is used. */
                DeleteLater = 1            /*!< When subjects are deleted by this observer, deleteLater() are called on them. */
            };
            //! Function which returns a string associated with a specific ObjectDeletionPolicy.
            static QString objectDeletionPolicyToString(ObjectDeletionPolicy object_deletion_policy);
            //! Function which returns the ObjectDeletionPolicy associated with a string.
            static ObjectDeletionPolicy stringToObjectDeletionPolicy(const QString& object_deletion_policy_string);
            //! The access mode scope of the observer.
            /*! When using categories in an observer, access modes can be set for each individual category.
              Categories which does not have access modes set will use the global access mode.
              \sa setAccessMode(), setAccessModeScope(), accessModeScope()
              */
            enum AccessModeScope {
                GlobalScope = 0,            /*!< The global access mode is used for all categories when categories are used. */
                CategorizedScope = 1        /*!< Access modes are category specific. */
            };
            //! Function which returns a string associated with a specific AccessModeScope.
            static QString accessModeScopeToString(AccessModeScope access_mode_scope);
            //! Function which returns the AccessModeScope associated with a string.
            static AccessModeScope stringToAccessModeScope(const QString& access_mode_scope_string);

            // --------------------------------
            // Core Functions
            // --------------------------------
            //! Default constructor.
            Observer(const QString& observer_name = QString(), const QString& observer_description = QString(), QObject* parent = 0);
            //! Copy constructor.
            Observer(const Observer &other);
            //! Observer destructor.
            /*!
             * The destructor will delete necesarry objects, thus it will check the ownership of each subject and delete it when
             * it has SpecificObserverOwnership set to this Observer, or when it has ObserverScopeOwnership and this
             * is the last observer that it is attached to. Note that the deletion method used depends on the
             * objectDeletionPolicy() of this observer.
             */
            virtual ~Observer();
            //! Overload << operator so that we can attach subjects using the operator.
            inline Observer& operator<<(QObject* subject)
            {
               attachSubject(subject);
               return *this;
            }

            //! Event filter filters property change events on all subjects.
            /*!
              \sa toggleSubjectEventFiltering(), toggleQtilitiesPropertyChangeEvents()
              */
            bool eventFilter(QObject *object, QEvent *event);
            //! This function toggles event filtering on objects.
            /*!
              It is recommended to always keep event filtering enabled. However in some cases, like object reconstruction in ObjectManager::constructRelationships() it is necessary to manually edit read only properties (like ownership etc.). Another example is when you want to attach objects in threads other than the observer's thread to it. In such cases you can disable subject event filtering on objects attached to your subjects.

              \param toggle True is event filtering is enabled, thus property changes are monitored by the observer. False otherwise.

              \note Event filtering is enabled by default. It is also important to know that events won't be monitored on subjects which were attached while subject event filtering was disabled, even if you turn on subject event filtering again at a later stage. Therefore it is best to call this function only once, before attaching any objects to your observer.

              \sa subjectEventFilteringEnabled(), qtilitiesPropertyChangeEventsEnabled()
              */
            void toggleSubjectEventFiltering(bool toggle);
            //! Indicates if subject event filtering is enabled.
            /*!
              \sa toggleSubjectEventFiltering(), qtilitiesPropertyChangeEventsEnabled()
              */
            bool subjectEventFilteringEnabled() const;
            //! This function enables/disables delivery of QtilitiesPropertyChangeEvents on objects when property changes occurs.
            /*!
              \param toggle When true, change events are delivered. When false they are not delivered.

              See monitoredPropertyChanged() for more details on when property change events can be used. If your implementation does not use property change events, you should disabled the events to optimize performance.

              \note These events are disabled by default.

              \sa qtilitiesPropertyChangeEventsEnabled(), toggleSubjectEventFiltering()
              */
            void toggleQtilitiesPropertyChangeEvents(bool toggle);
            //! Indicates if QtilitiesPropertyChangeEvents are enabled.
            /*!
              \sa toggleQtilitiesPropertyChangeEvents(), subjectEventFilteringEnabled()
              */
            bool qtilitiesPropertyChangeEventsEnabled() const;
            //! This function enables/disables broadcasting of modification state changes Observer.
            /*!
                If you don't use modification state changes turning this off will increase performance.

                \sa broadcastModificationStateChangesEnabled()
              */
            void toggleBroadcastModificationStateChanges(bool toggle);
            //! Indicates if broadcasting of modification state changes are enabled.
            /*!
              \sa toggleBroadcastModificationStateChanges()
              */
            bool broadcastModificationStateChangesEnabled() const;

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, Observer> factory;

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
            virtual void setExportVersion(Qtilities::ExportVersion version);
            virtual void setExportTask(ITask* task);
            virtual void clearExportTask();
            virtual void setApplicationExportVersion(quint32 version);
            virtual IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            /*!
              For subjects, the sequence in which the object reconstruction happens is as follows:
              - Construct object and set object name according to factory data.
              - Attach object.
              - Set object category if it exists.
              - Call the importXml() function on the object's IExportable implementation.
              - Set object activity.

              \note This function does not call detachAll() before doing the import.
              */
            virtual IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            /*!
              For subjects, the sequence in which the object reconstruction happens is as follows:
              - Construct object and set object name according to factory data.
              - Attach object.
              - Set object category if it exists.
              - Call the importXml() function on the object's IExportable implementation.
              - Set object activity.

              \note This function does not call detachAll() before doing the import.
              */
            virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // IExportableObserver Implementation
            // --------------------------------
            virtual IExportable::ExportResultFlags exportBinaryExt(QDataStream& stream, ObserverData::ExportItemFlags export_flags = ObserverData::ExportData) const;
            virtual IExportable::ExportResultFlags exportXmlExt(QDomDocument* doc, QDomElement* object_node, ObserverData::ExportItemFlags export_flags = ObserverData::ExportData) const;

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            //! Sets if a subject's modification state must be monitored.
            /*!
             *\return True when the needed property was set succesfully on the subject. False otherwise, or when the subject is not attached to this context.
             *
             *\sa monitorSubjectModificationState()
             *
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            bool setMonitorSubjectModificationState(QObject* obj, bool monitor);
            //! Gets if a subject's modification state must be monitored.
            /*!
             *\return True when the modification state of the object is monitored. False otherwise, or when the subject is not attached to this context.
             *
             *\sa setMonitorSubjectModificationState()
             *
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            bool monitorSubjectModificationState(QObject* obj);

            bool isModified() const;
        public slots:
            /*!
              For observers, the listeners will only be notified if a processing cycle is not active or if it is forced using \p force_notifications.

              \sa isProcessingCycleActive(), startProcessingCycle(), endProcessingCycle()
              */
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        protected:
            //! Sets the factory data which is necessary to reconstruct this observer.
            /*!
              When sub classing from Observer, it might be necessary to set use different factory data settings from the default
              data used for a normal observer. Call this function in your subclass constructor to change your object's factory data.
              */
            void setFactoryData(InstanceFactoryInfo factory_data);

            // --------------------------------
            // Functions related to item views viewing this observer and signal emission.
            // --------------------------------
        public:
            //! Function to refresh the layout views showing this observer.
            /*!
              This function will emit the layoutChanged() signal with the new_selection parameter.

              \param force When true views will be updated even if a processing cycle is currently active on the observer. When false the processing cycle will be respected.
              */
            void refreshViewsLayout(QList<QPointer<QObject> > new_selection = QList<QPointer<QObject> >(), bool force = false);
            //! Function to refresh the data views showing this observer.
            /*!
              This function will emit the dataChanged(this) signal.

              \param force When true views will be updated even if a processing cycle is currently active on the observer. When false the processing cycle will be respected.
              */
            void refreshViewsData(bool force = false);
            //! Starts a processing cycle.
            /*!
              When adding/removing many subjects to the observer it makes sense to only let item views know
              that the observer changed when all subjects have been added/removed. This function will disable
              the change notification signals of this observer until endProcessingCycle() is called.

              Note that this function only affects signals emitted by the observer. The observer still monitors
              dynamic property changes on all attached subjects during a processing cycle. To control the
              event filtering, see subjectEventFilteringEnabled() and toggleSubjectEventFiltering().

              If a processing cycle was already started, this function does nothing.

              Internally observers keep track of how many times you start and end processing cycles. To stop a processing cycle you need to call endProcessingCycle() the same number of times you
              called startProcessingCycle(). Thus, you don't need to keep track if a processing cycle was already started in the beginning of your function (you don't have to care about it). For example:

\code
Observer obs;
obs.startProcessingCycle(); // Internal count = 1;
obs.startProcessingCycle(); // Internal count = 2;

// Some function gets the observer without any knowledge of previous start/end processing cycle calls:
// It is not necessary to do the following:
{
    bool is_cycle_active = obs.isProcessingCycleActive();
    obs.startProcessingCycle(); // Internal count = 3;

    // Do some batch processing.
    // If we did not have an internal counter the correct way to end it in a function would be:
    if (!is_cycle_active)
        obs.endProcessingCycle(); // Internal count = 2;

    // However, since there is an internal counter we can ignore the is_cycle_active declaration in the above code and just call:
    obs.endProcessingCycle();
}

obs.endProcessingCycle(); // Internal count = 1;
obs.endProcessingCycle(); // Internal count = 0;
\endcode

              \note It is very important to note that the observer's layoutChanged() and dataChanged() signals are not
              emitted during processing cycles. Thus, views displaying the observer are not going to update
              during processing cycles. When calling endProcessingCycle with the \p broadcast parameter set to true (the default) the
              correct signals will be emitted only if the number of subjects changed during the processing cycle. See endProcessingCycle() for more
              information.

              \sa endProcessingCycle(), subjectEventFilteringEnabled(), toggleSubjectEventFiltering(), isProcessingCycleActive(), processingCycleStarted(), startTreeProcessingCyle()
              */
            virtual void startProcessingCycle();
            //! Ends a processing cycle.
            /*!
              Ends a processing cycle started with startProcessingCycle(). The processing cycle will only be stopped when the number of endProcessingCycle() calls matches the number of
              startProcessingCycle() calls. If you call endProcessingCycle() too many times an error message will be printed using qWarning().

              \param broadcast If the number of subjects changed during the processing cycle (thus, since startProcessingCycle() was called the first time), this function will
              automatically emit numberOfSubjectsChanged() and refreshLayout() when broadcast is true. When false, none of these signals are emitted. Also,
              if the observer's modification state is true after the processing cycle ended, the modificationStateChanged() signal will automatically be called when broadcast
              is true, and not when broadcast is false.

              \note When emitting numberOfSubjectsChanged() the objects parameter will be empty even when the number of subjects changed.

              \sa startProcessingCycle(), isProcessingCycleActive(), processingCycleEnded(), endTreeProcessingCycle()
              */
            virtual void endProcessingCycle(bool broadcast = true);
            //! Function which returns the number of times processing cycles has been started.
            int processingCycleCount() const;
            //! Function which resets the processing cycle count on the observer.
            void resetProcessingCycleCount(bool broadcast = true);
            //! Indicates if a processing cycle is active.
            /*!
              \sa startProcessingCycle(), endProcessingCycle()
              */
            bool isProcessingCycleActive() const;
            //! Starts a processing cycle.
            /*!
              Same behaviour as startProcessingCyle(), but starts a processing cycle on the complete tree underneath the observer. Thus, processing cycles are started on all observers attached
              to this observer and those attached to that observer etc. It is important to node that you should use endTreeProcessingCycle() when using this function.

              \sa endTreeProcessingCycle(), endProcessingCycle(), subjectEventFilteringEnabled(), toggleSubjectEventFiltering(), isProcessingCycleActive(), processingCycleStarted()
              */
            virtual void startTreeProcessingCycle();
            //! Starts a processing cycle.
            /*!
              Same behaviour as startProcessingCyle(), but starts a processing cycle on the complete tree underneath the observer. Thus, processing cycles are started on all observers attached
              to this observer and those attached to that observer etc. It is important to node that you should use endTreeProcessingCycle() when using this function.

              \sa startTreeProcessingCycle(), endProcessingCycle(), subjectEventFilteringEnabled(), toggleSubjectEventFiltering(), isProcessingCycleActive(), processingCycleStarted()
              */
            virtual void endTreeProcessingCycle(bool broadcast = true);

            // --------------------------------
            // Functions to attach / detach subjects
            // --------------------------------
            //! Will attempt to attach the specified object to the observer. The success of this operation depends on the installed subject filters, as well as the dynamic properties defined for the object to be attached.
            /*!
              When Successful \p obj will be part of this observer context until it is detached again or deleted.

              \param obj The object to be attached.
              \param ownership The ownership that the observer should use to manage the object. The default is Observer::ManualOwnership.
              \param rejectMsg When this function fails and rejectMsg will be populated with an rejection message when valid.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not necessary to validate the context in such cases. False by default.
              \returns True is succesful, false otherwise.

              \note When subjectEventFilteringEnabled() is true, the observer will be installed as an event filter on \p obj. If you do not want this to happen you can turn it off using toggleSubjectEventFiltering(). See the toggleSubjectEventFiltering() function documentation for more information on this.
              \note When obj lives in a different thread than this observer an attempt won't be made to install this observer as an event filter.
              \note When obj->objectName() is empty, this function will set the object name to the className() of the object.

              \sa attachSubjects(), startProcessingCycle(), endProcessingCycle()
              */
            virtual bool attachSubject(QObject* obj, Observer::ObjectOwnership ownership = Observer::ManualOwnership, QString* rejectMsg = 0, bool import_cycle = false);
            //! Will attempt to attach the specified objects to the observer.
            /*!
              This function will call startProcessingCycle() when it starts and endProcessingCycle() when it is done.

              \param objects A list of objects which must be attached.
              \param ownership The ownership that the observer should use to manage the object. The default is Observer::ManualOwnership.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not necessary to validate the context in such cases. False by default.
              \param rejectMsg When this function fails and rejectMsg will be populated with an rejection message when valid.
              \returns A list of objects that was successfully added. Thus if the list has the same amount of items in \p objects, the operation was succesful on all objects.

              \sa attachSubject(), startProcessingCycle(), endProcessingCycle()
              */
            virtual QList<QPointer<QObject> > attachSubjects(QList<QObject*> objects, Observer::ObjectOwnership ownership = Observer::ManualOwnership, QString* rejectMsg = 0, bool import_cycle = false);
            //! Will attempt to attach the specified objects in a ObserverMimeData object.
            /*!
              This function will call startProcessingCycle() when it starts and endProcessingCycle() when it is done.

              \param obj The object to be attached.
              \param ownership The ownership that the observer should use to manage the object. The default is Observer::ManualOwnership.
              \param rejectMsg When this function fails and rejectMsg will be populated with an rejection message when valid.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not necessary to validate the context in such cases. False by default.
              \returns A list of objects which was successfully added. Thus if the list has the same amount of items in \p objects, the operation was succesful on all objects.

              \sa attachSubject(), startProcessingCycle(), endProcessingCycle()
              */
            virtual QList<QPointer<QObject> > attachSubjects(ObserverMimeData* mime_data_object, Observer::ObjectOwnership ownership = Observer::ManualOwnership, QString* rejectMsg = 0, bool import_cycle = false);
            //! A function which checks if the new object can be attached to the observer. This function also validates the attachment operation inside all installed subject filters. Note that this function does not attach it.
            /*!
              \param obj The object to test attachment of.
              \param rejectMsg Rejection message. If the attachment cannot be done, thus it returns Observer::Rejected, you can get the reason through this error message.
              \param ownership This parameter allows you to specify the ownership to use during attachment. By default Observer::ManualOwnership.
              \param silent When true the function checks if the attachment can be done without using any dialog boxes. This is usefull when you need to attach subjects in an event filter where showing a dialog is a problem. An example of this is drag/drop operations in ObserverWidgets.
              */
            Observer::EvaluationResult canAttach(QObject* obj, Observer::ObjectOwnership ownership = Observer::ManualOwnership, QString* rejectMsg = 0, bool silent = false) const;
            //! A function which checks if the objects in the ObserverMimeData object can be attached to the observer. This function also validates the attachment operation inside all installed subject filters. Note that this function does not attach it.
            /*!
              \param mime_data_object The mime data object to test attachment of.
              \param rejectMsg Rejection message. If the attachment cannot be done, thus it returns Observer::Rejected, you can get the reason through this error message.
              \param silent When true the function checks if the attachment can be done without using any dialog boxes. This is useful when you need to attach subjects in an event filter where showing a dialog is a problem. An example of this is drag/drop operations in ObserverWidgets.
              */
            Observer::EvaluationResult canAttach(ObserverMimeData* mime_data_object, QString* rejectMsg = 0, bool silent = false) const;
            //! A function which checks if the object can be detached from the observer. This function also validates the detachment operation inside all installed subject filters. Note that this function does not detach it.
            /*!
             * \param obj The object to test detachment of.
             * \param rejectMsg Rejection message. If the attachment cannot be done, thus it returns Observer::Rejected, you can get the reason through this error message.
             */
            Observer::EvaluationResult canDetach(QObject* obj, QString* rejectMsg = 0) const;

        public slots:
            //! Will attempt to detach the specified object from the observer.
            /*!
             * This function will delete obj if necessary, thus it will check its ownership and delete it when
             * it has SpecificObserverOwnership set to this Observer, or when it has ObserverScopeOwnership and this
             * is the last observer that it is attached to. Note that the deletion method used depends on the
             * objectDeletionPolicy() of this observer.
             *
              \param obj The object to be detached.
              \param rejectMsg When this function fails and rejectMsg will be populated with an rejection message when valid.
              \returns True if successful, false otherwise.
              */
            virtual bool detachSubject(QObject* obj, QString* rejectMsg = 0);
            //! Will attempt to detach the specified object objects in the list from the observer.
            /*!
             * This function will delete necessary objects, thus it will check the ownership of each subject and delete it when
             * it has SpecificObserverOwnership set to this Observer, or when it has ObserverScopeOwnership and this
             * is the last observer that it is attached to. Note that the deletion method used depends on the
             * objectDeletionPolicy() of this observer.
             *
              \param objects A list of objects which must be detached.
              \param rejectMsg When this function fails and rejectMsg will be populated with an rejection message when valid.
              \returns A list of objects which was successfully detached.
              */
            virtual QList<QPointer<QObject> > detachSubjects(QList<QObject*> objects, QString* rejectMsg = 0);
            //! Function to detach all currently observed subjects.
            /*!
             * This function will delete necessary objects, thus it will check the ownership of each subject and delete it when
             * it has SpecificObserverOwnership set to this Observer, or when it has ObserverScopeOwnership and this
             * is the last observer that it is attached to. Note that the deletion method used depends on the
             * objectDeletionPolicy() of this observer.
             */
            virtual void detachAll();
            //! Function to delete all currenlty observed subjects.
            /*!
              \param base_class_name Specifies that only items inheriting the specified base class must be deleted.
              \param refresh_views Indicates if this function must refresh all observer views when done.
              */
            virtual void deleteAll(const QString &base_class_name = "QObject", bool refresh_views = true);

        private slots:
            //! Will handle an object which has been deleted somewhere else in the application.
            void handle_deletedSubject(QObject* obj);
        signals:
            //! Will be emitted when a subject is deleted.
            void subjectDeleted(QObject* obj);

            // --------------------------------
            // Observer property related functions
            // --------------------------------
        public:
            //! Convenience function which will get the value of a MultiContextProperty based dynamic property, and not the observer property itself.
            /*!
              If the property_name does not refer to a shared property, the observer context of the observer on which this function
              is called will be used to define the observer context for which this function will get the property's value.
              If the \p property_name refers to a shared property, the shared property's value will be returned.
              */
            QVariant getMultiContextPropertyValue(const QObject* obj, const char* property_name) const;
            //! Convenience function which will set the value of a MultiContextProperty based dynamic property, and not the observer property itself.
            /*!
              If the property_name does not refer to a shared property, the observer context of the observer on which this function
              is called will be used to define the observer context for which this function will set the property's value.
              If the \p property_name refers to a shared property, the shared property's value will be returned.
              */
            bool setMultiContextPropertyValue(QObject* obj, const char* property_name, const QVariant& new_value) const;

            // --------------------------------
            // Static functions
            // --------------------------------
            //! Function which returns all the observers in a QList<QObject*> input list.
            static QList<Observer*> observerList(QList<QPointer<QObject> >& object_list);
            //! Convenience function to get the number of observers observing the specified object. Thus the number of parents of this object.
            /*!
              \sa parentReferences()
              */
            static int parentCount(const QObject* obj);
            //! Convenience function to get the a list of parent observers for this object.
            /*!
              \sa parentCount()
              */
            static QList<Observer*> parentReferences(const QObject* obj);
            //! Function to check if a meta_type is supported by an observer. Note that an observer must have a subject type filter which knows about the type in order for the function to return true.
            /*!
              \sa Qtilities::Core::SubjectTypeInfo
              */
            static bool isSupportedType(const QString& meta_type, Observer* observer);

        private:
            //! This function will remove all the properties which this observer might have added to an obj.
            void removeQtilitiesProperties(QObject* obj);

        public:
            // --------------------------------
            // General functions providing information about this observer's state and observed subjects
            // --------------------------------
            //! This function will check if obj_to_check is a parent of observer in the parent hierarchy of observer. Use this function to avoid circular dependencies.
            static bool isParentInHierarchy(const Observer* obj_to_check, const Observer* observer);
            //! This function will validate changes to the observer, or to a specific observer category if specified.
            bool isConst(const QtilitiesCategory& access_mode = QtilitiesCategory()) const;
            //! Returns the observer's subject limit.
            inline int subjectLimit() const { return observerData->subject_limit; }
            //! Function to set the subject limit of this observer.
            /*!
              \return Returns true if the limit was set successfully, otherwise false.
              */
            bool setSubjectLimit(int subject_limit);
            //! Function to set the observer's access mode. Set the access mode after construction. When subclassing Observer, set it in your constructor.
            /*!
              \note It is only possible to set the access mode for a category that exist. Thus an object must exist in the observer with the category you want to set the access mode for.
              \param category Only used when accessModeScope() is categorized. Categories which does not have an access mode set for them will use the global access mode. The global access mode can be set by passing QString() as category.
              */
            void setAccessMode(AccessMode mode, QtilitiesCategory category = QtilitiesCategory());
            //! Function to get the observer's access mode.
            /*!
              \return The global access mode when category an empty category is used for \p category. Otherwise the access mode for a specific category. The global access mode is FullAccess by default. When an invalid category (does not exist in this context) is sent Observer::InvalidAccess is returned and an error message is printed.
              */
            AccessMode accessMode(QtilitiesCategory category = QtilitiesCategory()) const;
            //! Function to set the observer's object deletion policy.
            /*!
              \sa objectDeletionPolicy()
              */
            void setObjectDeletionPolicy(ObjectDeletionPolicy object_deletion_policy);
            //! Function to get the observer's object deletion policy.
            /*!
              Default is DeleteLater.

              \sa setObjectDeletionPolicy()
              */
            ObjectDeletionPolicy objectDeletionPolicy() const;
            //! Function to set the observer's access mode scope.
            void setAccessModeScope(AccessModeScope access_mode_scope);
            //! Function to return the access mode scope of the observer.
            /*!
              \return The access mode scope. Global by default.
              */
            inline AccessModeScope accessModeScope() const { return (AccessModeScope) observerData->access_mode_scope; }
            //! Returns the observer's description. For example, a variable workspace, or a logger engine manager etc.
            void setObserverDescription(const QString& description) { observerData->observer_description = description; }
            //! Returns the observer's name within a context. If a context is not specified, the objectName() of the observer is returned.
            QString observerName(int parent_id = -1) const;
            //! Returns the name used for the specified object in this context. QString() is returned if the object is not valid or not attached to this observer.
            QString subjectNameInContext(const QObject* obj) const;
            //! Returns the category used for the specified object in this context. QtilitiesCategory() is returned if the object is not valid, not attached to this observer or does not have a category.
            QtilitiesCategory subjectCategoryInContext(const QObject* obj) const;
            //! Returns the displayed name used for the specified object in this context. QString() is returned if the object is not valid or not attached to this observer.
            /*!
              The displayed name is found using the following sequence, returning the first match:
              - Check if the object has the qti_prop_DISPLAYED_ALIAS_MAP property
              - Check if the object has the qti_prop_ALIAS_MAP property
              - Lastly, just return the objectName()

              \param obj The object for which the name must be fetched.
              \param check_displayed_name_property Indicates if the qti_prop_DISPLAYED_ALIAS_MAP property must be checked.
              \param validate_object When true, the function will first check if the object exists in the context.

              \return The name if the object was found, and empty QString() otherwise.
              */
            QString subjectDisplayedNameInContext(const QObject* obj, bool check_displayed_name_property = true, bool validate_object = false) const;
            //! Returns the ownership used for the specified object in this context.
            /*!
                If \p obj is not valid or contained in this context, ManualOwnership is returned. Thus you should
                check if the object exists in this context using contains() before calling this function.
              */
            ObjectOwnership subjectOwnershipInContext(const QObject* obj) const;
            //! Returns the observer's description. For example, a variable workspace, or a logger engine manager etc.
            inline QString observerDescription() const { return observerData->observer_description; }
            //! Returns the uqniue ID assigned to this observer by the ObjectManager.
            inline int observerID() const { return observerData->observer_id; }
            //! Returns the number of subjects currently observed by the observer.
            /*!
                This function is different from treeCount() which gets all the children underneath an observer (Thus, children of children etc.).
                */
            int subjectCount(const QString& base_class_name = QString()) const;
            //! Function to get the number of children under the specified observer.
            /*!
                This count includes the children of children as well. To get the number of subjects only in this context use subjectCount().
                \note This observer itself is not counted.
                */
            int treeCount(const QString& base_class_name = QString());
            //! Function to get a QObject reference at a specific location in the tree underneath this observer.
            /*!
              If \p i is < 0 or bigger than or equal to the number of items retuned by allChildren() this function returns 0.
              */
            QObject* treeAt(int i) const;
            //! Function to check if a specific AbstractTreeItem is contained in the tree underneath this node.
            bool treeContains(QObject* tree_item) const;
            //! Function to get the QObject references of all items in the tree underneath this observer.
            /*!
              Returns a list of QObjects* in tree underneath this observer where the list is populated in the same order in which Qtilities::Core::TreeIterator iterates through the tree.

              \param base_class_name The name of the base class of children you are looking for. By default, all children underneath this observer is returned.
              \param limit When defined, the tree children will be search up until the limit count is reached. This allows you to stop when a tree gets too big. By default all children are returned.
              *\param iterator_id Internal iterator ID. You should never use this directly.

              For example:

\code
QList<QObject*> children;
TreeNode* rootNode = new TreeNode("Root");
TreeNode* parentNode1 = rootNode->addNode("Parent 1");
TreeNode* parentNode2 = rootNode->addNode("Parent 2");

parentNode1->addItem("Child 1");
parentNode1->addItem("Child 2");
parentNode2->addItem("Child 3");
parentNode2->addItem("Child 5");

// The following call will return
QList<QObject*> nodes_verify = rootNode->treeChildren("Qtilities::CoreGui::TreeNode");
QVERIFY(nodes_verify.count() == 2);
QList<QObject*> items_verify = rootNode->treeChildren("Qtilities::CoreGui::TreeItem");
QVERIFY(items_verify.count() == 5);
\endcode

              \note This observer itself is not part of the list.
              */
            QList<QObject*> treeChildren(const QString& base_class_name = "QObject",
                                         int limit = -1,
                                         int iterator_id = -1) const;
            //! Returns a list with the names of all the current observed subjects which inherits a specific base class. By default all subjects' names are returned.
            QStringList subjectNames(const QString& base_class_name = "QObject") const;
            //! Returns a list with the displayed names of all the current observed subjects which inherits a specific base class. By default all subjects' displayed names are returned.
            QStringList subjectDisplayedNames(const QString& base_class_name = "QObject") const;
            //! Returns the subject reference at a given position.
            QObject* subjectAt(int i) const;
            //! Returns the ID of the object at the specified position of the Observer's pointer list, returns -1 if the object was not found.
            int subjectID(int i) const;
            //! Returns the ID associated with a specific subject.
            /*!
              \note Only depend on this function (where you specify the object using the object's name) when you are sure that
              objects have unique names. This can be achieved by installing a NamingPolicyFilter in your observer. If names
              are not unique, the first match of the given subject_name will be used. If you don't care about unique subject names,
              rather use subjectReference(int ID) to get subject references.
              */
            int subjectID(const QString& subject_name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
            //! Returns the IDs for all the attached subjects.
            QList<int> subjectIDs() const;
            //! Returns a list with the subject references of all the observed subjects which inherits a specific base class. If you don't specify an interface, all QObjects in the observer are returned.
            QList<QObject*> subjectReferences(const QString& base_class_name = QString()) const;
            //! Return a QMap with references to all subjects as keys with the names used for the subjects in this context as values.
            QMap<QPointer<QObject>, QString> subjectMap();
            //! Returns a list of observers under this observer.
            /*!
              This is an optimized function to get observer references under this observer. Thus, its much faster than
              something like subjectReferences("Observer") or subjectReferences("TreeNode").
              */
            QList<QPointer<Observer> > subjectObserverReferences() const;
            //! Gets the subject reference for a specific, unique subject ID.
            QObject* subjectReference(int ID) const;
            //! Gets the subject reference for a specific object name.
            /*!
              \note Only depend on this function (where you specify the object using the object's name) when you are sure that
              objects have unique names. This can be achieved by installing a NamingPolicyFilter in your observer. If names
              are not unique, the first match of the given subject_name will be used. If you don't care about unique subject names,
              rather use subjectReference(int ID) to get subject references.
              */
            QObject* subjectReference(const QString& subject_name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
            //! Returns true if a given subject is currently observed by the observer.
            bool contains(const QObject* object) const;
            //! Returns true if a subject with the specified name is currently observed by the observer.
            /*!
              \note Only depend on this function (where you specify the object using the object's name) when you are sure that
              objects have unique names. This can be achieved by installing a NamingPolicyFilter in your observer. If names
              are not unique, the first match of the given subject_name will be used. If you don't care about unique subject names,
              rather use subjectReference(int ID) to get subject references.
              */
            bool containsSubjectWithName(const QString& subject_name, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

            // --------------------------------
            // Subject filter related functions
            // --------------------------------
            //! Installs a new subject filter.
            /*!
              The observer will take ownership of the subject filter object and delete it in its constructor.
              Subject filters can only be installed when the observer has no subjects attached to it.

              Subject filter names are unique in the context of an observer. This ensures that two filters of the
              same type cannot be installed in the same context. This function will check that the filterName() of
              \p subject_filter is unique and then add it. If it is not, the function will fail.
              */
            bool installSubjectFilter(AbstractSubjectFilter* subject_filter);
            //! Uninstalls a subject filter.
            /*!
              This function will delete the subject filter since subject filters can only be used once at present.
              */
            bool uninstallSubjectFilter(AbstractSubjectFilter* subject_filter);
            //! Provides a list of all installed subject filters.
            QList<AbstractSubjectFilter*> subjectFilters() const;
            //! Function to check if a subject filter with the specified name already exists in this context.
            bool hasSubjectFilter(const QString& filter_name) const;

            // --------------------------------
            // Observer hints related functions
            // --------------------------------
            //! Function which returns a pointer to the ObserverHints used by this observer.
            /*!
              If no observer hints are used, 0 is returned.

              \sa useDisplayHints(), setDisplayHints()
              */
            ObserverHints* displayHints() const;
            //! Function which constructs hints for this observer.
            /*!
              If the observer does not have any hints, this function will construct an ObserverHints instance
              and assign it to this observer. If the observer already has hints associated with it, this function does
              nothing.

              \returns The constructed hints instance, if hints were already present, returns 0.

              \sa displayHints(), setDisplayHints()
              */
            ObserverHints* useDisplayHints();
            //! Function to let this observer copy a set of display hints.
            /*!
              This function allows you to copy the hints used by this Observer from a different ObserverHints instance.

              \note If you call this function on the observer which does not have any displayHints() yet (thus useDisplayHints()
              have not been called yet) this function will call useDisplayHints() before inheriting the hints.

              \returns True if successful, false otherwise.

              \sa useDisplayHints(), displayHints()
              */
            bool copyHints(ObserverHints* display_hints);

            // --------------------------------
            // Subject category related functions
            // --------------------------------
            //! Checks if the specified category exists in this observer context.
            bool hasCategory(const QtilitiesCategory& category) const;
            //! Renames \p old_category to \p new_category in this observer context.
            /*!
              \param old_category The category to rename.
              \param new_category The new name that must be given to the category.
              \param match_exactly When true, the old_category must match the new category exactly (thus the depths must match as well). When false, all categories that start with or match
               the old_category will be updated to start with or match the new category.
               \returns A list with the subjects for which the category was renamed.
              */
            QList<QPointer<QObject> > renameCategory(const QtilitiesCategory& old_category,const QtilitiesCategory& new_category, bool match_exactly = true);
            //! Returns the access mode for a specific category.
            /*!
              If the category does not exist, Observer::InvalidAccess is returned.
              */
            AccessMode categoryAccessMode(const QtilitiesCategory& category) const;
            //! Returns a QStringList with all the categories found in the qti_prop_CATEGORY_MAP properties of all attached subjects.
            /*!
              This function does not take the category filtering options of the observer into account, for that functionality see displayedCategories()
              on the observer hints for this observer provided by hints(). Category filtering is only related to the displaying of the observer.
              */
            QList<QtilitiesCategory> subjectCategories() const;
            //! Returns a list with the names of all the current observed subjects which belongs to a specific category.
            QStringList subjectNamesByCategory(const QtilitiesCategory& category) const;
            //! Returns a list with the subject references of all the observed subjects which has the specified category set as an qti_prop_CATEGORY_MAP shared observer property.
            QList<QObject*> subjectReferencesByCategory(const QtilitiesCategory& category) const;
            //! Returns a QMap with each object in this observer mapped to its category's string representation (using toString("::") function on QtilitiesCategory).
            QMap<QPointer<QObject>,QString> subjectReferenceCategoryMap() const;

            // --------------------------------
            // Property related functions
            // --------------------------------
            //! This function returns a QStringList with the names of all the properties which are monitored by this observer.
            /*!
              Monitored properties are properties that are monitored for changes by the event filter
              of the observer and Qtilities::Core::AbstractSubjectFilter::handleMonitoredPropertyChange() functions on
              all subject filters installed in the observer context. The observer's event filter will automatically
              route the changes to monitored properties of installed subject filters to the correct subject filters.

              When property changes are valid, the monitoredPropertyChanged() signal is emitted as soon as the property change
              is completed.

              \note The list of monitored properties includes monitored properties of any installed subject filters.

              \sa monitoredPropertyChanged(), toggleSubjectEventFiltering(), propertyChangeFiltered()
              */
            QStringList monitoredProperties() const;
            //! This function returns a QStringList with the names of all the reserved properties inside this observer context.
            /*!
              Reserved properties are internal properties that cannot be changed. The observer will filter any attempted changes to
              these properties. To check if a property is reserved, see the \p Permission attribute in the property documentation.
              All %Qtilities properties are defined in the Qtilities::Core::Properties namespace.

              \note The list of reserved properties includes reserved properties of any installed subject filters.

              \sa propertyChangeFiltered()
              */
            QStringList reservedProperties() const;

        signals:
            //! This signal is emitted when an observer is about to be deleted, thus its emitted in the beginning of the observer's destructor.
            void aboutToBeDeleted();
            //! This signal is emitted just before all subjects are about to be deleted.
            void allSubjectsAboutToBeDeleted();
            //! This signal is emitted after all subjects were deleted.
            void allSubjectsDeleted();
            //! This signal is emitted just before all subjects are about to be detached.
            void allSubjectsAboutToBeDetached();
            //! This signal is emitted after all subjects were to be detached.
            void allSubjectsDetached();
            //! A signal which is emitted as soon as a monitored property of the observer or any of the installed subject filters changed.
            /*!
              This signal is usefull when you want to monitor a specific object. This can be done in two ways:
              - By filtering the QtilitiesPropertyChangeEvent on the object. However events can only be delivered to objects in the same thread. Therefore, if the object you are interested in is in a different thread than the observer context, this will not work.
              - The alternative is to connect to this signal and check if the object you are interested in is in the list of objects on which the property changed.

              \param property_name The name of the property which changed.
              \param objects The objects on which the property changed.

              \sa monitoredProperties(), toggleSubjectEventFiltering(), propertyChangeFiltered()
              */
            void monitoredPropertyChanged(const char* property_name, QList<QObject*> objects = QList<QObject*>());
            //! A signal which is emitted as soon as an property change event is filtered.
            /*!
              This signal can be emitted in two scenarios:
              - When an attempt is made to modify a reserved property it will always be emitted.
              - When a monitored property change is not allowed. An example of this is when a name change is rejected.

              \param property_name The name of the property on which the change was filtered.
              \param objects The objects on which the property was attempted.

              \sa reservedProperties(), monitoredProperties()
              */
            void propertyChangeFiltered(const char* property_name, QList<QObject*> objects = QList<QObject*>());

            // --------------------------------
            // General observer notification signals
            // --------------------------------
            //! A signal which is emitted when the number of subjects change in this observer context.
            /*!
              This signal only indicates if the number of subjects in this observer changes. To monitor number of subject changes
              in the complete tree under the observer, see layoutChanged().

              Thus, for table views this is enough, for tree views use layoutChanged().

              \param change_indication Slots can use this indicator to know what change occurred.
              \param objects A list of objects which was added/removed. When the list contains null items, these objects were deleted and the observer picked it up and removed them. When this signal is emitted in endProcessingCycle() this list will be empty.

              \note Whenever it is needed to emit this signal, Observer will first set the modification state of the Observer to true and then emit the signal.
              \note When a processing cycle is active on the observer, this signal will be emitted at the end of the processing cycle in endProcessingCycle only if
              the number of subjects changed during the processing cycle. See endProcessingCycle() for more information.
              */
            void numberOfSubjectsChanged(Observer::SubjectChangeIndication change_indication, QList<QPointer<QObject> > objects = QList<QPointer<QObject> >());

            //! A signal which is emitted when the layout of the observer or the tree underneath it changes.
            /*!
              This signal will be emitted whenever the layout of an observer or the tree underneath it changes.

              This will happen when any of the following happens inside the observer context:
              - Number of subjects changes in an observer context which is part of the tree.
              - Observer hints changes. Since observer hints define how item models displays (lays out) an observer context, changes to these hints will trigger the layoutChanged() signal.
              - Replace operations triggered by NamingPolicyFilter filters.
              - Observer access mode and access mode scope changes.
              - Subject access mode changes.

              \param new_selection The new desired selection in any views that are refreshed due to the layout changed.
              \note When creating models for observers, this signal should be connected to the layoutChanged() signal of your model for tree models. For table
                    models connecting to numberOfSubjectsChanged() should be enough.
              \sa accessMode(), accessModeScope(), Qtilities::Core::Properties::qti_prop_ACCESS_MODE
              */
            void layoutChanged(QList<QPointer<QObject> > new_selection = QList<QPointer<QObject> >());
            //! A signal which is emitted when the data in the observer or the tree underneath it changes.
            /*!
              This signal will be emitted whenever the data of the observer or any subjects in the tree underneath the observer changes.

              This will happen when any of the following happens inside the observer context:
              - Activity changes happens.
              - Names of observers or subjects changes.

              \param observer When the data that changed is related to a specific observer context, for example if the activity of all subjects inside a context changed, the observer can be passed as the \p observer parameter.

              \note When creating models for observers, this signal should be connected to the dataChanged() signal of your model.
              */
            void dataChanged(Observer* observer = 0);
            //! Signal which is emitted when this observer enters a processing cycle.
            void processingCycleStarted();
            //! Signal which is emitted when this observer exists a processing cycle.
            void processingCycleEnded();

        private:
            //! Performs a delete on an object in a thread-safe way.
            void deleteObject(QObject* object);

        protected:
            ObserverData* observerData;
        };

        /*!
        \class Qtilities::Core::ObserverAwareBase
        \brief The ObserverAwareBase class provides everything that is needed to make an object aware of an observer.

         This class is a convenience base class which can be used by objects when they depend on an observer context.

         \sa Qtilities::Core::Observer
          */
        class QTILIITES_CORE_SHARED_EXPORT ObserverAwareBase
        {
        public:
            ObserverAwareBase() { }
            virtual ~ObserverAwareBase() {}

            //! Sets the observer context.
            virtual bool setObserverContext(Observer* observer_ptr) {
                if (observer_ptr) {
                    d_observer = observer_ptr;
                    return true;
                } else {
                    return false;
                }
            }
            //! Gets a pointer to the observer context.
            Observer* observerContext() const { return d_observer; }

        protected:
            QPointer<Observer> d_observer;
        };
    }
}

#endif // ABSTRACTOBSERVER_H
