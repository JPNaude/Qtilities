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

#ifndef OBSERVER_H
#define OBSERVER_H

#include "QtilitiesCore_global.h"
#include "QtilitiesCoreApplication.h"
#include "QtilitiesCoreConstants.h"
#include "PointerList.h"
#include "ObserverProperty.h"
#include "ObserverData.h"
#include "IExportable.h"
#include "IFactory.h"
#include "IModificationNotifier.h"
#include "SubjectTypeFilter.h"
#include "QtilitiesCategory.h"

#include <QObject>
#include <QString>
#include <QPointer>
#include <QList>
#include <QStringList>
#include <QSharedDataPointer>

namespace Qtilities {
    namespace Core {
        class AbstractSubjectFilter;
        class ObserverMimeData;

        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::Core::Properties;

        /*!
        \class Observer
        \brief The observer class is an extended implementation of an observer in the subject-observer pattern.

        See the \ref page_observers article for more information.
        */
        class QTILIITES_CORE_SHARED_EXPORT Observer : public QObject, public IExportable, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)
            Q_ENUMS(SubjectChangeIndication);
            Q_ENUMS(ObjectOwnership);            
            Q_ENUMS(EvaluationResult)
            Q_ENUMS(AccessMode)

            Q_PROPERTY(QString Name READ observerName);
            Q_PROPERTY(QString Description READ observerDescription);
            Q_PROPERTY(int SubjectLimit READ subjectLimit WRITE setSubjectLimit);
            Q_PROPERTY(int SubjectCount READ subjectCount);
            Q_PROPERTY(AccessMode Access READ accessMode);
            Q_PROPERTY(int ID READ observerID);
            Q_PROPERTY(bool Modified READ isModified());

        public:
            // --------------------------------
            // Enumerations
            // --------------------------------
            //! This enumeration is used to return results when validating attaching and detaching of subjects.
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
                CycleProcess                /*!< Indicates that the number of subjects changed during a cyclic process. The subject count can either be less or more than before the cyclic operation. \sa startProcessingCycle(), endProcessingCycle()*/
            };
            //! The possible ownerships with which subjects can be attached to an observer.
            /*!
              See the \ref object_lifetimes section of the \ref page_observers article for a detailed discussion.
              */
            enum ObjectOwnership {
                ManualOwnership,            /*!< Manaul ownership means that the object won't be managed by the observer, thus the ownership will be managed the normal Qt way. If \p parent() = 0, it will not be managed, if \p parent() is an QObject, the subject will be deleted when its parent is deleted. */
                AutoOwnership,              /*!< Auto ownership means that the observer will automatically decide how to manage the subject. The observer checks if the object already has a \p parent(), if so Observer::ManualOwnership is used. If no \p parent() is specified yet, the observer will attach the subject using Observer::ObserverScopeOwnership. */
                SpecificObserverOwnership,  /*!< The observer becomes the parent of the subject. That is, when the observer is deleted, the subject is also deleted. */
                ObserverScopeOwnership,     /*!< The subject is deleted as soon as it is detached from the last observer managing it. */
                OwnedBySubjectOwnership     /*!< The observer is dependent on the subject, thus the subject effectively owns the observer. When the subject is deleted, the observer is also deleted. When the observer is deleted it checks if the subject is attached to any other observers and if not it deletes the subject as well. If the subject is attached to any other observers, the subject is not deleted. When Observer::OwnedBySubjectOwnership, the new ownership is ignored. Thus when a subject was attached to a context using Observer::OwnedBySubjectOwnership it is attached to all other contexts after that using Observer::OwnedBySubjectOwnership as well. On the other hand, when a subject is already attached to one or more observer contexts and it is attached to a new observer using Observer::OwnedBySubjectOwnership, the old ownership is kept and the observer only connects the destroyed() signal on the object to its own deleteLater() signal. */
            };
            //! The possible access modes of the observer.
            /*!
              \sa setAccessMode(), accessMode()
              */
            enum AccessMode {
                FullAccess = 0,             /*!< All observer operations are available to the user (Attachment, Detachement etc.). */
                ReadOnlyAccess = 1,         /*!< The observer is read only to the user. */
                LockedAccess = 2,           /*!< The observer is read only and locked. Item views presenting this observer to the user will respect the LockedAccess mode and will not display the contents of the observer to the user. */
                InvalidAccess = 3           /*!< An invalid access mode. This access mode is returned in functions where the access mode is requested for a category that does not exist, for example categoryAccessMode(). */
            };
            //! The access mode scope of the observer.
            /*! When using categories in an observer, access modes can be set for each individual category.
              Categories which does not have access modes set will use the global access mode.
              \sa setAccessMode(), setAccessModeScope(), accessModeScope()
              */
            enum AccessModeScope {
                GlobalScope = 0,            /*!< The global access mode is used for all categories when categories are used. */
                CategorizedScope = 1        /*!< Access modes are category specific. */
            };

            // --------------------------------
            // Core Functions
            // --------------------------------
            //! Default constructor.
            Observer(const QString& observer_name, const QString& observer_description, QObject* parent = 0);
            //! Copy constructor.
            Observer(const Observer &other);
            virtual ~Observer();
            bool eventFilter(QObject *object, QEvent *event);
            //! This function toggles event filtering on objects.
            /*!
              It is recommended to always keep event filtering enabled. However in some cases, like
              object reconstruction in ObjectManager::constructRelationships() it is neccesarry to
              manually edit read only properties (like ownership etc.).

              In these cases filtering is disabled temporarily. Note that disabling also disables event
              filtering in all subject filters.

              \param toggle True is event filtering is enabled, thus property changes are monitored by the observer. False otherwise.

              \note Event filtering is enabled by default.
              */
            void toggleSubjectEventFiltering(bool toggle);
            //! Indicates if subject event filtering is enabled.
            bool subjectEventFilteringEnabled() const;
            //! This function toggles delivery of QtilitiesPropertyChangeEvents on objects when property changes occurs.
            /*!
              See monitoredPropertyChanged() for more details on when property change events can be used. If your implementation
              does not use property change events, you should disabled the events to optimize performance.

              \note These events are disabled by default.
              */
            void toggleQtilitiesPropertyChangeEvents(bool toggle);
            //! Indicates if QtilitiesPropertyChangeEvents are enabled.
            bool qtilitiesPropertyChangeEventsEnabled() const;

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            IFactoryData factoryData() const;
            virtual IExportable::Result exportBinary(QDataStream& stream, QList<QVariant> params = QList<QVariant>()) const;
            virtual IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());
            virtual Result exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            virtual Result importXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>());

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            /*!
              For observers, the listeners will only be notified if a processing cycle is not active.

              \sa isProcessingCycleActive(), startProcessingCycle(), endProcessingCycle()
              */
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners);
        signals:
            void modificationStateChanged(bool is_modified) const;

        protected:
            //! Sets the factory data which is neccesarry to reconstruct this observer.
            /*!
              When subclassing from Observer, it might be neccesarry to set use different factory data settings from the default
              data used for a normal observer. Call this function in your subclass constructor to change your object's factory data.
              */
            void setFactoryData(IFactoryData factory_data);

            // --------------------------------
            // Functions related to item views viewing this observer and signal emission.
            // --------------------------------
        public:
            //! Function to refresh the layout views showing this observer.
            /*!
              This function will emit the layoutChanged() signal.
              */
            void refreshViewsLayout();
            //! Function to refresh the data views showing this observer.
            /*!
              This function will emit the dataChanged(this) signal.
              */
            void refreshViewsData();
            //! Starts a processing cycle.
            /*!
              When adding/removing many subjects to the observer it makes sense to only let item views know
              that the observer changed when all subjects have been added/removed. This function will disable
              the change notification signals of this observer until endProcessingCycle() is called.

              Note that this function only affects signals emitted by the observer. The observer still monitors
              dynamic property changes on all attached subjects during a processing cycle. To control the
              event filtering, see subjectEventFilteringEnabled() and toggleSubjectEventFiltering().
              
              If a processing cycle was already started, this function does nothing.
              
              \note It is very important to note that the observer's layoutChanged() and dataChanged() signals are not
              emitted during processing cycles. Thus, views displaying the observer are not going to update until
              during processing cycles. Therefore you need to call the refreshViewsLayout() or refreshViewsData() after
              your processing cycle is completed. The function you must call will depend on what you changed during the
              processing cycle.

              \sa endProcessingCycle(), subjectEventFilteringEnabled(), toggleSubjectEventFiltering()
              */
            void startProcessingCycle();
            //! Ends a processing cycle.
            /*!
              Ends a processing cycle started with startProcessingCycle(). If a processing cycle was not started
              when calling this function, it does nothing.

              Note that you must manually call the needed update signals refreshViewsData() or refreshViewsLayout()
              after ending a processing cycle.

              \sa startProcessingCycle();
              */
            void endProcessingCycle();
            //! Indicates if a processing cycle is active.
            /*!
              \sa startProcessingCycle(), endProcessingCycle()
              */
            bool isProcessingCycleActive() const;

            // --------------------------------
            // Functions to attach / detach subjects
            // --------------------------------
            //! Will attempt to attach the specified object to the observer. The success of this operation depends on the installed subject filters, as well as the dynamic properties defined for the object to be attached.
            /*!
              \param obj The object to be attached.
              \param ownership The ownership that the observer should use to manage the object. The default is Observer::ManualOwnership.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases. False by default.
              \returns True is succesfull, false otherwise.

              \sa attachSubjects(), startProcessingCycle(), endProcessingCycle()
              */
            virtual bool attachSubject(QObject* obj, Observer::ObjectOwnership ownership = Observer::ManualOwnership, bool import_cycle = false);
            //! Will attempt to attach the specified objects to the observer.
            /*!
              This function will call startProcessingCycle() when it starts and endProcessingCycle() when it is done.

              \param objects A list of objects which must be attached.
              \param ownership The ownership that the observer should use to manage the object. The default is Observer::ManualOwnership.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases. False by default.
              \returns A list of objects which was succesfully added. Thus if the list has the same amount of items in \p objects, the operation was succesfull on all objects.

              \sa attachSubject(), startProcessingCycle(), endProcessingCycle()
              */
            virtual QList<QObject*> attachSubjects(QList<QObject*> objects, Observer::ObjectOwnership ownership = Observer::ManualOwnership, bool import_cycle = false);
            //! Will attempt to attach the specified objects in a ObserverMimeData object.
            /*!
              This function will call startProcessingCycle() when it starts and endProcessingCycle() when it is done.

              \param obj The object to be attached.
              \param ownership The ownership that the observer should use to manage the object. The default is Observer::ManualOwnership.
              \param import_cycle Indicates if the attachment call was made during an observer import cycle. In such cases the subject filter must not add exportable properties to the object since these properties will be added from the import source. Also, it is not neccesarry to validate the context in such cases. False by default.
              \returns A list of objects which was succesfully added. Thus if the list has the same amount of items in \p objects, the operation was succesfull on all objects.

              \sa attachSubject(), startProcessingCycle(), endProcessingCycle()
              */
            virtual QList<QObject*> attachSubjects(ObserverMimeData* mime_data_object, Observer::ObjectOwnership ownership = Observer::ManualOwnership, bool import_cycle = false);
            //! A function which checks if the new object can be attached to the observer. This function also validates the attachment operation inside all installed subject filters. Note that this function does not attach it.
            Observer::EvaluationResult canAttach(QObject* obj, Observer::ObjectOwnership ownership = Observer::ManualOwnership) const;
            //! A function which checks if the objects in the ObserverMimeData object can be attached to the observer. This function also validates the attachment operation inside all installed subject filters. Note that this function does not attach it.
            Observer::EvaluationResult canAttach(ObserverMimeData* mime_data_object) const;

        public slots:
            //! Will attempt to detach the specified object from the observer.
            /*!
              \param obj The object to be detached.
              \returns True if successful, false otherwise.
              */
            virtual bool detachSubject(QObject* obj);
            //! Will attempt to detach the specified object objects in the list from the observer.
            /*!
              \param objects A list of objects which must be detached.
              \returns A list of objects which was succesfully detached.
              */
            virtual QList<QObject*> detachSubjects(QList<QObject*> objects);
            //! A function which checks if the object can be dettached from the observer. This function also validates the detachment operation inside all installed subject filters. Note that this function does not detach it.
            Observer::EvaluationResult canDetach(QObject* obj) const;
            //! Function to detach all currently observed subjects.
            virtual void detachAll();
            //! Function to delete all currenlty observed subjects.
            virtual void deleteAll();

        private slots:
            //! Will handle an object which has been deleted somewhere else in the application.
            void handle_deletedSubject(QObject* obj);

            // --------------------------------
            // Observer property related functions
            // --------------------------------
        public:
            //! Convenience function which will get the value of a ObserverProperty based dynamic property, and not the observer property itself.
            /*!
              If the property_name does not refer to a shared property, the observer context of the observer on which this function
              is called will be used to define the observer context for which this function will get the property's value.
              If the property_name reffers to a shared property, the shared property's value will be returned.
              */
            QVariant getObserverPropertyValue(const QObject* obj, const char* property_name) const;
            //! Convenience function which will set the value of a ObserverProperty based dynamic property, and not the observer property itself.
            /*!
              If the property_name does not refer to a shared property, the observer context of the observer on which this function
              is called will be used to define the observer context for which this function will set the property's value.
              If the property_name reffers to a shared property, the shared property's value will be returned.
              */
            bool setObserverPropertyValue(QObject* obj, const char* property_name, const QVariant& new_value) const;
            //! Convenience function which will get the specified ObserverProperty of the specified object.
            static ObserverProperty getObserverProperty(const QObject* obj, const char* property_name) {
                #ifndef QT_NO_DEBUG
                    Q_ASSERT(obj != 0);
                #endif
                #ifdef QT_NO_DEBUG
                    if (!obj)
                        return ObserverProperty();
                #endif

                QVariant prop = obj->property(property_name);
                if (prop.isValid() && prop.canConvert<ObserverProperty>())
                    return prop.value<ObserverProperty>();
                else
                    return ObserverProperty();
                            }

            //! Convenience function which will set the specified ObserverProperty on the specified object.
            static bool setObserverProperty(QObject* obj, ObserverProperty observer_property) {
                if (!observer_property.isValid() || !obj) {
                    Q_ASSERT(observer_property.isValid());
                    return false;
                }

                QVariant property = qVariantFromValue(observer_property);
                obj->setProperty(observer_property.propertyName(),property);
                return true;
            }

            //! Convenience function which will get the specified SharedObserverProperty of the specified object.
            static SharedObserverProperty getSharedProperty(const QObject* obj, const char* property_name) {
                #ifndef QT_NO_DEBUG
                    Q_ASSERT(obj != 0);
                #endif
                #ifdef QT_NO_DEBUG
                    if (!obj)
                        return SharedObserverProperty();
                #endif

                QVariant prop = obj->property(property_name);
                if (prop.isValid() && prop.canConvert<SharedObserverProperty>())
                    return prop.value<SharedObserverProperty>();
                else
                    return SharedObserverProperty();
            }

            //! Convenience function which will set the specified SharedObserverProperty on the specified object.
            static bool setSharedProperty(QObject* obj, SharedObserverProperty shared_property) {
                if (!shared_property.isValid() || !obj) {
                    Q_ASSERT(shared_property.isValid());
                    return false;
                }

                QVariant property = qVariantFromValue(shared_property);
                obj->setProperty(shared_property.propertyName(),property);
                return true;
            }

            //! Convenience function to get the number of observers observing the specified object. Thus the number of parents of this object.
            static int parentCount(const QObject* obj) {
                if (!obj)
                    return -1;

                ObserverProperty prop = getObserverProperty(obj, Qtilities::Core::Properties::OBSERVER_SUBJECT_IDS);
                if (prop.isValid()) {
                    return prop.observerMap().count();
                }

                return 0;
            }

            //! Convenience function to get the a list of parent observers for this object.
            static QList<Observer*> parentReferences(const QObject* obj) {
                QList<Observer*> parents;
                if (!obj)
                    return parents;

                ObserverProperty prop = getObserverProperty(obj, Qtilities::Core::Properties::OBSERVER_SUBJECT_IDS);
                if (prop.isValid()) {
                    for (int i = 0; i < prop.observerMap().count(); i++) {
                        Observer* obs = OBJECT_MANAGER->observerReference(prop.observerMap().keys().at(i));
                        if (obs)
                            parents << obs;
                    }
                }

                return parents;
            }

            //! Convenience function to check if a property exists on a object.
            static bool propertyExists(const QObject* obj, const char* property_name) {
                if (!obj)
                    return false;

                QVariant prop = obj->property(property_name);
                return prop.isValid();
            }        

            //! Function to check if a meta_type is supprted by an observer. Note that an observer must have a subject type filter which knows about the type in order for the function to return true.
            /*!
              \sa Qtilities::Core::SubjectTypeInfo
              */
            static bool isSupportedType(const QString& meta_type, Observer* observer) {
                if (!observer)
                    return false;

                // Check if this observer has a subject type filter installed
                for (int i = 0; i < observer->subjectFilters().count(); i++) {
                    SubjectTypeFilter* subject_type_filter = qobject_cast<SubjectTypeFilter*> (observer->subjectFilters().at(i));
                    if (subject_type_filter) {
                        if (subject_type_filter->isKnownType(meta_type)) {
                            return true;
                        }
                        break;
                    }
                }

                return false;
            }

        private:
            //! This function will remove this observer from all the properties which it might have added to an obj.
            void removeObserverProperties(QObject* obj);

            // --------------------------------
            // General functions providing information about this observer's state and observed subjects
            // --------------------------------
            //! This function will check if obj_to_check is a parent of observer in the parent hierachy of observer. Use this function to avoid circular dependancies.
            bool isParentInHierarchy(const Observer* obj_to_check, const Observer* observer) const;

        public:
            //! This function will validate changes to the observer, or to a specific observer category if specified.
            bool isConst(const QtilitiesCategory& access_mode = QtilitiesCategory()) const;
            //! Returns the observer's subject limit.
            inline int subjectLimit() const { return observerData->subject_limit; }
            //! Function to set the subject limit of this observer.
            /*!
              \return Returns true if the limit was set succesfully, otherwise false.
              */
            bool setSubjectLimit(int subject_limit);
            //! Function to set the observer's access mode. Set the access mode after construction. When subclassing Observer, set it in your constructor.
            /*!
              \param category Only used when accessModeScope() is categorized. Categories which does not have an access mode set for them will use the global access mode. The global access mode can be set by passing QString() as category.
              */
            void setAccessMode(AccessMode mode, QtilitiesCategory category = QtilitiesCategory());
            //! Function to get the observer's access mode.
            /*!
              \return The global access mode when category an empty category is used for \p category. Otherwise the access mode for a specific category. The global access mode is FullAccess by default. When an invalid category (does not exist in this context) is sent Observer::InvalidAccess is returned and an error message is printed.
              */
            AccessMode accessMode(QtilitiesCategory category = QtilitiesCategory()) const;
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
            //! Returns the name used for the specified object in this context. QString() is returned if the object is not valid.
            QString subjectNameInContext(const QObject* obj) const;
            //! Returns the observer's description. For example, a variable workspace, or a logger engine manager etc.
            inline QString observerDescription() const { return observerData->observer_description; }
            //! Returns the uqniue ID assigned to this observer by the ObjectManager.
            inline int observerID() const { return observerData->observer_id; }
            //! Returns the number of subjects currently observed by the observer. This function is different from getChildCount() which gets all the children underneath an observer (Thus, children of children etc.)
            inline int subjectCount() const { return observerData->subject_list.count(); }
            //! Function to get the number of children under the specified observer. This count includes the children of children as well.
            int childCount(const Observer* observer = 0) const;
            //! Returns a list with the names of all the current observed subjects which implements a specific interface. By default all subject names are returned.
            QStringList subjectNames(const QString& iface = QString()) const;
            //! Returns the subject reference at a given position.
            QObject* subjectAt(int i) const;
            //! Returns the ID of the object at the specified position of the Observer's pointer list, returns -1 if the object was not found.
            int subjectID(int i) const;
            //! Returns a list with the subject references of all the observed subjects which implements a given interface. If you don't specify an interface, all objects in the observer are returned.
            QList<QObject*> subjectReferences(const QString& iface = QString()) const;
            //! Return a QMap with references to all subjects as keys with the names used for the subjects in this context as values.
            QMap<QPointer<QObject>, QString> subjectMap();
            //! Gets the subject reference for a specific, unique subject ID.
            QObject* subjectReference(int ID) const;
            //! Gets the subject reference for a specific object name.
            /*!
              \note Only depend on this function (where you specify the object using the object's name) when you are sure that
              objects have unique names. This can be achieved by installing a NamingPolicyFilter in your observer. If names
              are not unique, the first match of the given subject_name will be used. If you don't care about unique subject names,
              rather use subjectReference(int ID) to get subject references.
              */
            QObject* subjectReference(const QString& subject_name) const;
            //! Returns true if a given subject is currently observed by the observer.
            bool contains(const QObject* object) const;

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
            ObserverHints* const displayHints() const;
            //! Function which sets the display hints used by this observer.
            /*!
              Note that display hints can only be set when the observer has no subjects attached to it. If the observer
              already has display hints, they will be deleted.

              \returns True if successful, false otherwise.

              \sa useDisplayHints(), displayHints()
              */
            bool setDisplayHints(ObserverHints* display_hints);
            //! Function which constructs hints for this observer.
            /*!
              If the observer does not have any hints, this function will construct an ObserverHints instance
              and assign it to this observer. If the observer already has hints associated with it, this function does
              nothing.

              \returns The constructed hints instance, if hints were already present, return 0.

              \sa displayHints(), setDisplayHints()
              */
            ObserverHints* useDisplayHints();

            // --------------------------------
            // Subject category related functions
            // --------------------------------
            //! Checks if the specified category exists in this observer context.
            bool hasCategory(const QtilitiesCategory& category) const;
            //! Returns the access mode for a specific category.
            /*!
              If the category does not exist, Observer::InvalidAccess is returned.
              */
            AccessMode categoryAccessMode(const QtilitiesCategory& category) const;
            //! Returns a QStringList with all the categories found in the OBJECT_CATEGORY properties of all attached subjects.
            /*!
              This function does not take the category filtering options of the observer into account, for that functionality see displayedCategories()
              on the observer hints for this observer provided by hints(). Category filtering is only related to the displaying of the observer.
              */
            QList<QtilitiesCategory> subjectCategories() const;
            //! Returns a list with the names of all the current observed subjects which belongs to a specific category.
            QStringList subjectNamesByCategory(const QtilitiesCategory& category) const;
            //! Returns a list with the subject references of all the observed subjects which has the specified categroy set as an OBJECT_CATEGORY shared observer property.
            QList<QObject*> subjectReferencesByCategory(const QtilitiesCategory& category) const;

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

              \sa monitoredPropertyChanged(), enableSubjectEventFiltering(), propertyChangeFiltered()
              */
            QStringList monitoredProperties() const;
            //! This function returns a QStringList with the names of all the reserved properties inside this observer context.
            /*!
              Reserved properties are internal properties that cannot be changed. The observer will filter any attempted changes to
              these properties. To check if a property is reserved, see the \p Permisson attribute in the property documentation.
              All %Qtilities properties are defined in the Qtilities::Core::Properties namespace.

              \note The list of reserved properties includes reserved properties of any installed subject filters.

              \sa propertyChangeFiltered()
              */
            QStringList reservedProperties() const;
        signals:
            //! A signal which is emitted as soon as a monitored property of the observer or any of the installed subject filters changed.
            /*!
              This signal is usefull when you want to monitor a specific object. This can be done in two ways:
              - By filtering the QtilitiesPropertyChangeEvent on the object. However events can only be delivered to objects in the same thread. Therefore, if the object you are interested in is in a different thread than the observer context, this will not work.
              - The alternative is to connect to this signal and check if the object you are interested in is in the list of objects on which the property changed.

              \param property_name The name of the property which changed.
              \param objects The objects on which the property changed.

              \sa monitoredProperties(), enableSubjectEventFiltering(), propertyChangeFiltered()
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

              \param change_indication Slots can use this indicator to know what change occured.
              \param objects A list of objects which was added/removed. When the list contains null items, these objects were deleted.
              */
            void numberOfSubjectsChanged(Observer::SubjectChangeIndication change_indication, QList<QObject*> objects = QList<QObject*>());
            //! A signal which is emitted when the layout of the observer or the tree underneath it changes.
            /*!
              This signal will be emitted whenever the layout of an observer or the tree underneath it changes.

              This will happen when any of the following happens inside the observer context:
              - Number of subjects changes in an observer context which is part of the tree.
              - Observer hints changes. Since observer hints define how item models displays (lays out) an observer context, changes to these hints will trigger the layoutChanged() signal.
              - Replace operations triggered by NamingPolicyFilter filters.
              - Observer access mode and access mode scope changes. \sa accessMode(), accessModeScope()
              - Non-observer access mode changes. \sa Qtilities::Core::Properties::AccessMode

              \note When creating models for observers, this signal should be connected to the layoutChanged() signal of your model.
              */
            void layoutChanged();
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

        protected:
            QSharedDataPointer<ObserverData> observerData;
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
            ObserverAwareBase() { d_observer = 0; }
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
