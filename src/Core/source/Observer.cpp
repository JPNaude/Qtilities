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

#include "Observer.h"
#include "TreeIterator.h"
#include "QtilitiesCoreConstants.h"
#include "QtilitiesProperty.h"
#include "ActivityPolicyFilter.h"
#include "QtilitiesPropertyChangeEvent.h"
#include "ObserverMimeData.h"
#include "ObserverHints.h"
#include "IExportableFormatting.h"

#include <Logger>

#include <QMap>
#include <QVariant>
#include <QMetaType>
#include <QEvent>
#include <QDynamicPropertyChangeEvent>
#include <QCoreApplication>
#include <QMutableListIterator>
#include <QDomElement>
#include <QDomDocument>

using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core::Properties;

namespace Qtilities {
    namespace Core {
        FactoryItem<QObject, Observer> Observer::factory;
    }
}

Qtilities::Core::Observer::Observer(const QString& observer_name, const QString& observer_description, QObject* parent) : QObject(parent) {
    // Initialize observer data
    observerData = new ObserverData(this,observer_name);
    setObjectName(observer_name);
    observerData->observer_description = observer_description;
    observerData->access_mode_scope = GlobalScope;
    observerData->access_mode = FullAccess;
    observerData->object_deletion_policy = DeleteImmediately;
    observerData->filter_subject_events_enabled = true;
    connect(&observerData->subject_list,SIGNAL(objectDestroyed(QObject*)),SLOT(handle_deletedSubject(QObject*)));

    // Register this observer with the observer manager
    if (observer_name != QString(qti_def_GLOBAL_OBJECT_POOL))
        observerData->observer_id = OBJECT_MANAGER->registerObserver(this);
    else
        observerData->observer_id = 0;
}

Qtilities::Core::Observer::Observer(const Observer &other) : QObject(other.parent()) {
    // Initialize observer data
    observerData = new ObserverData(*other.observerData.data());
    setObjectName(other.objectName());

    connect(&observerData->subject_list,SIGNAL(objectDestroyed(QObject*)),SLOT(handle_deletedSubject(QObject*)));

    // Register this observer with the observer manager
    if (other.objectName() != QString(qti_def_GLOBAL_OBJECT_POOL))
        observerData->observer_id = OBJECT_MANAGER->registerObserver(this);
    else
        observerData->observer_id = 0;
}

Qtilities::Core::Observer::~Observer() {
    startProcessingCycle();

    emit aboutToBeDeleted();

    observerData->number_of_subjects_start_of_proc_cycle = -1;
    toggleBroadcastModificationStateChanges(false);

    if (objectName() != QString(qti_def_GLOBAL_OBJECT_POOL)) {
        observerData->deliver_qtilities_property_changed_events = false;

        // When this observer is deleted, we must check the ownership of all its subjects
        QVariant subject_ownership_variant;
        QVariant parent_observer_variant;

        LOG_TRACE(QString("Starting destruction of observer \"%1\":").arg(objectName()));
        LOG_TRACE("Deleting neccessary children:");

        QMutableListIterator<QObject*> i = observerData->subject_list.iterator();
        while (i.hasNext()) {
            QObject* obj = i.next();
            // If it is an observer we start a processing cycle on it:
            Observer* obs = qobject_cast<Observer*> (obj);
            if (obs)
                obs->startProcessingCycle();
            // TODO handle containment approach here as well, thus if contained, set processing cycle on contained observer as well.

            subject_ownership_variant = getMultiContextPropertyValue(obj,qti_prop_OWNERSHIP);
            parent_observer_variant = getMultiContextPropertyValue(obj,qti_prop_PARENT_ID);
            if ((subject_ownership_variant.toInt() == SpecificObserverOwnership) && (observerData->observer_id == parent_observer_variant.toInt())) {
                // Subjects with SpecificObserverOwnership must be deleted as soon as this observer is deleted if this observer is their parent.
               LOG_TRACE(QString("Object \"%1\" (aliased as %2 in this context) is owned by this observer, it will be deleted.").arg(obj->objectName()).arg(subjectNameInContext(obj)));
               if (!i.hasNext()) {
                   deleteObject(obj);
                   QCoreApplication::processEvents();
                   break;
               } else {
                   deleteObject(obj);
                   QCoreApplication::processEvents();
               }
            } else if ((subject_ownership_variant.toInt() == ObserverScopeOwnership) && (parentCount(obj) == 1)) {
                LOG_TRACE(QString("Object \"%1\" (aliased as %2 in this context) with ObserverScopeOwnership went out of scope, it will be deleted.").arg(obj->objectName()).arg(subjectNameInContext(obj)));
                if (!i.hasNext()) {
                    deleteObject(obj);
                    QCoreApplication::processEvents();
                    break;
                } else {
                    deleteObject(obj);
                    QCoreApplication::processEvents();
                }
           } else if ((subject_ownership_variant.toInt() == OwnedBySubjectOwnership) && (parentCount(obj) == 1)) {
                LOG_TRACE(QString("Object \"%1\" (aliased as %2 in this context) with OwnedBySubjectOwnership went out of scope, it will be deleted.").arg(obj->objectName()).arg(subjectNameInContext(obj)));
                if (!i.hasNext()) {
                    deleteObject(obj);
                    QCoreApplication::processEvents();
                    break;
                } else {
                    deleteObject(obj);
                    QCoreApplication::processEvents();
                }
            } else {
                detachSubject(obj);
            }
        }
    }

    // Delete subject filters
    LOG_TRACE("Deleting subject filters.");
    int filter_count = observerData->subject_filters.count();
    for (int i = 0; i < filter_count; i++) {
        delete observerData->subject_filters.at(0);
        observerData->subject_filters.pop_front();
    }

    if (objectName() != QString(qti_def_GLOBAL_OBJECT_POOL)) {
        LOG_TRACE("Removing any trace of this observer from remaining children.");
        int count = subjectCount();
        for (int i = 0; i < count; i++) {
            // In this case we need to remove any trace of this observer from the obj
            if (observerData->subject_list.at(i))
                removeQtilitiesProperties(observerData->subject_list.at(i));
        }
    }

    // We do not need to delete observerData here since it is a QSharedDataPointer.

    LOG_DEBUG(QString("Done with destruction of observer \"%1\".").arg(objectName()));
}

QStringList Qtilities::Core::Observer::monitoredProperties() const {
    QStringList properties;
    properties.append(QString(qti_prop_CATEGORY_MAP));
    properties.append(QString(qti_prop_ACCESS_MODE));
    // Role properties are also monitored. We will notify views to refresh when they change.
    properties.append(QString(qti_prop_TOOLTIP));
    properties.append(QString(qti_prop_DECORATION));
    properties.append(QString(qti_prop_FOREGROUND));
    properties.append(QString(qti_prop_BACKGROUND));
    properties.append(QString(qti_prop_TEXT_ALIGNMENT));
    properties.append(QString(qti_prop_FONT));
    properties.append(QString(qti_prop_SIZE_HINT));
    properties.append(QString(qti_prop_STATUSTIP));
    properties.append(QString(qti_prop_WHATS_THIS));

    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        properties << observerData->subject_filters.at(i)->monitoredProperties();
    }
    return properties;
}

QStringList Qtilities::Core::Observer::reservedProperties() const {
    QStringList properties;
    properties << QString(qti_prop_OBSERVER_MAP) << QString(qti_prop_OWNERSHIP) << QString(qti_prop_PARENT_ID) << QString(qti_prop_VISITOR_ID) << QString(qti_prop_LIMITED_EXPORTS);

    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        properties << observerData->subject_filters.at(i)->reservedProperties();
    }
    return properties;
}

void Qtilities::Core::Observer::toggleSubjectEventFiltering(bool toggle) {
    observerData->filter_subject_events_enabled = toggle;
}

bool Qtilities::Core::Observer::subjectEventFilteringEnabled() const {
    return observerData->filter_subject_events_enabled;
}

void Qtilities::Core::Observer::toggleQtilitiesPropertyChangeEvents(bool toggle) {
    observerData->deliver_qtilities_property_changed_events = toggle;
}

bool Qtilities::Core::Observer::qtilitiesPropertyChangeEventsEnabled() const {
    return observerData->deliver_qtilities_property_changed_events;
}

void Qtilities::Core::Observer::toggleBroadcastModificationStateChanges(bool toggle) {
    observerData->broadcast_modification_state_changes = toggle;
}

bool Qtilities::Core::Observer::broadcastModificationStateChangesEnabled() const {
    return observerData->broadcast_modification_state_changes;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::Observer::instanceFactoryInfo() const {
    InstanceFactoryInfo factory_data = observerData->factory_data;
    factory_data.d_instance_name = observerName();
    return factory_data;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::Observer::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

void Qtilities::Core::Observer::setExportVersion(Qtilities::ExportVersion version) {
    IExportable::setExportVersion(version);
    observerData->setExportVersion(version);
}

void Qtilities::Core::Observer::setExportTask(ITask* task) {
    observerData->setExportTask(task);
    IExportable::setExportTask(task);
}

void Qtilities::Core::Observer::clearExportTask() {
    observerData->clearExportTask();
    IExportable::clearExportTask();
}

void Qtilities::Core::Observer::setApplicationExportVersion(quint32 version) {
    IExportable::setApplicationExportVersion(version);
    observerData->setApplicationExportVersion(version);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::exportBinary(QDataStream& stream) const {
    return observerData->exportBinary(stream);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    return observerData->importBinary(stream,import_list);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    return observerData->exportXml(doc,object_node);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    return observerData->importXml(doc,object_node,import_list);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::exportBinaryExt(QDataStream& stream, ObserverData::ExportItemFlags export_flags) const {
    return observerData->exportBinaryExt(stream,export_flags);
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::exportXmlExt(QDomDocument* doc, QDomElement* object_node, ObserverData::ExportItemFlags export_flags) const {
    return observerData->exportXmlExt(doc,object_node,export_flags);
}

bool Qtilities::Core::Observer::isModified() const {
    if (!observerData->observer)
        return false;

    if (observerData->is_modified)
        return true;

    // Check if any subjects were modified.
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (observerData->subject_list.at(i));
        if (mod_iface) {
            if (mod_iface->isModified())
                return true;
        }
    }

    // Check if any subject filters were modified.
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        if (observerData->subject_filters.at(i)->isModificationStateMonitored()) {
            IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (observerData->subject_filters.at(i));
            if (mod_iface) {
                if (mod_iface->isModified())
                    return true;
            }
        }
    }

    // Check if the observer hints were modified.
    if (observerData->display_hints) {
        if (observerData->display_hints->isModified())
            return true;
    }
    return false;
}

void Qtilities::Core::Observer::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    if (!observerData->broadcast_modification_state_changes)
        return;

    if (notification_targets & IModificationNotifier::NotifySubjects) {
        // First notify all objects in this context.
        for (int i = 0; i < observerData->subject_list.count(); i++) {
            IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (observerData->subject_list.at(i));
            if (mod_iface) {
                mod_iface->setModificationState(new_state,notification_targets);
            }
        }
        // Also notify all subject filters.
        for (int i = 0; i < observerData->subject_filters.count(); i++) {
            IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (observerData->subject_filters.at(i));
            if (mod_iface) {
                mod_iface->setModificationState(new_state,IModificationNotifier::NotifyListeners);
            }
        }
        // Also notify observer hints.
        if (observerData->display_hints) {
            observerData->display_hints->setModificationState(new_state,notification_targets);
        }
    }

    // For observers we only notify targets if the actual state changed:
    if (observerData->is_modified != new_state || force_notifications) {
        observerData->is_modified = new_state;
        if (notification_targets & IModificationNotifier::NotifyListeners)
            emit modificationStateChanged(new_state);

        if (observerData->display_hints) {
            if (observerData->display_hints->modificationStateDisplayHint() != ObserverHints::NoModificationStateDisplayHint)
                refreshViewsData(); // Processing cycle checked inside refreshViewsData():
        }
    }
}

void Qtilities::Core::Observer::refreshViewsLayout(QList<QPointer<QObject> > new_selection, bool force) {
    if (!observerData->process_cycle_active || force)
        emit layoutChanged(new_selection);
}

void Qtilities::Core::Observer::refreshViewsData(bool force) {
    if (!observerData->process_cycle_active || force)
        emit dataChanged(this);
}

void Qtilities::Core::Observer::startProcessingCycle() {
    int previous_start_processing_cycle_count = observerData->start_processing_cycle_count;

    ++observerData->start_processing_cycle_count;

//    if (observerName() == "Observer Name")
//        qDebug() << "startProcessingCycle" << observerName() << observerData->start_processing_cycle_count;

    if (previous_start_processing_cycle_count == 0 && observerData->start_processing_cycle_count == 1) {
        observerData->number_of_subjects_start_of_proc_cycle = subjectCount();
        observerData->process_cycle_active = true;
        emit processingCycleStarted();
    }
}

void Qtilities::Core::Observer::endProcessingCycle(bool broadcast) {
    int previous_start_processing_cycle_count = observerData->start_processing_cycle_count;

    if (observerData->start_processing_cycle_count > 0)
        --observerData->start_processing_cycle_count;
    else
        qWarning() << "endProcessingCycle() called too many times on observer: " << observerName();

//    if (observerName() == "Observer Name")
//        qDebug() << "endProcessingCycle" << observerName() << observerData->start_processing_cycle_count;

    if (previous_start_processing_cycle_count == 1 && observerData->start_processing_cycle_count == 0) {
        // observerData->number_of_subjects_start_of_proc_cycle set to -1 in destructor.
        if (broadcast && (observerData->number_of_subjects_start_of_proc_cycle != -1)) {
            if (isModified())
                emit modificationStateChanged(true);

            if (observerData->number_of_subjects_start_of_proc_cycle > subjectCount()) {
                emit numberOfSubjectsChanged(Observer::SubjectRemoved);
                emit layoutChanged();
            }  else if (observerData->number_of_subjects_start_of_proc_cycle < subjectCount()) {
                emit numberOfSubjectsChanged(Observer::SubjectAdded);
                emit layoutChanged();
            }
        }

        // TODO: Send processing cycle end to subject filters in order for activity filter to emit the active subjects after the processing cycle if they changed. Note that TreeNode does this already.
        observerData->process_cycle_active = false;
        emit processingCycleEnded();
    }
}

bool Qtilities::Core::Observer::isProcessingCycleActive() const {
    return observerData->process_cycle_active;
}

void Qtilities::Core::Observer::startTreeProcessingCycle() {        
    QList<QObject*> obj_list = treeChildren();
    for (int i = 0; i < obj_list.count(); i++) {
        Observer* obs = qobject_cast<Observer*> (obj_list.at(i));
        if (obs)
            obs->startProcessingCycle();
    }

    startProcessingCycle();
}

void Qtilities::Core::Observer::endTreeProcessingCycle(bool broadcast) {
    QList<QObject*> obj_list = treeChildren();
    for (int i = 0; i < obj_list.count(); i++) {
        Observer* obs = qobject_cast<Observer*> (obj_list.at(i));
        if (obs) {
            obs->endProcessingCycle(false);
        }
    }

    endProcessingCycle(broadcast);
}

void Qtilities::Core::Observer::setFactoryData(Qtilities::Core::InstanceFactoryInfo factory_data) {
    if (factory_data.isValid())
        observerData->factory_data = factory_data;
}

bool Qtilities::Core::Observer::attachSubject(QObject* obj, Observer::ObjectOwnership object_ownership, QString* rejectMsg, bool import_cycle) {
    #ifndef QT_NO_DEBUG
    Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
    if (!obj) {
        if (rejectMsg)
            *rejectMsg = tr("Observer: Invalid object reference received. Attachment cannot be done.");
        return false;
    }
    #endif
    
    if (canAttach(obj,object_ownership,rejectMsg) == Rejected)
        return false;

    // Pass new object through all installed subject filters
    bool passed_filters = true;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        bool result = observerData->subject_filters.at(i)->initializeAttachment(obj,rejectMsg,import_cycle);
        if (passed_filters)
            passed_filters = result;
    }

    if (!passed_filters) {
        // Don't set change rejectMsg here, it will be set in initializeAttachment() above.
        LOG_DEBUG(QString("Observer (%1): Object (%2) attachment failed, attachment was rejected by one or more subject filter.").arg(objectName()).arg(obj->objectName()));
        for (int i = 0; i < observerData->subject_filters.count(); i++) {
            observerData->subject_filters.at(i)->finalizeAttachment(obj,false,import_cycle);
        }
        removeQtilitiesProperties(obj);
        return false;
    }

    // Details of the global object pool observer is not added to any objects.
    if (objectName() != QString(qti_def_GLOBAL_OBJECT_POOL)) {
        // Now, add observer details to needed properties
        // Add observer details to property: qti_prop_OBSERVER_MAP
        MultiContextProperty subject_id_property = ObjectManager::getMultiContextProperty(obj,qti_prop_OBSERVER_MAP);
        if (subject_id_property.isValid()) {
            // Thus, the property already exists
            subject_id_property.addContext(QVariant(observerData->subject_id_counter),observerData->observer_id);
            ObjectManager::setMultiContextProperty(obj,subject_id_property);
        } else {
            // We need to create the property and add it to the object
            MultiContextProperty new_subject_id_property(qti_prop_OBSERVER_MAP);
            new_subject_id_property.addContext(QVariant(observerData->subject_id_counter),observerData->observer_id);
            ObjectManager::setMultiContextProperty(obj,new_subject_id_property);
        }
        observerData->subject_id_counter += 1;

        // Now that the object has the properties needed, we add it
        observerData->subject_list.append(obj);

        // Handle object ownership
        #ifndef QT_NO_DEBUG
            QString management_policy_string;
        #endif
        // Check if the object is already managed, and if so with what ownership flag it was attached to those observers.
        if (parentCount(obj) > 1) {
            QVariant current_ownership = getMultiContextPropertyValue(obj,qti_prop_OWNERSHIP);
            if (current_ownership.toInt() != OwnedBySubjectOwnership) {
                if (object_ownership == ObserverScopeOwnership) {
                    // Update the ownership to ObserverScopeOwnership
                    setMultiContextPropertyValue(obj,qti_prop_OWNERSHIP,QVariant(ObserverScopeOwnership));
                    setMultiContextPropertyValue(obj,qti_prop_PARENT_ID,QVariant(-1));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Observer Scope Ownership";
                    #endif
                    obj->setParent(0);
                } else if (object_ownership == SpecificObserverOwnership) {
                    // Update the ownership to SpecificObserverOwnership
                    setMultiContextPropertyValue(obj,qti_prop_OWNERSHIP,QVariant(SpecificObserverOwnership));
                    setMultiContextPropertyValue(obj,qti_prop_PARENT_ID,QVariant(observerID()));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Specific Observer Ownership";
                    #endif
                    obj->setParent(this);
                } else if (object_ownership == ManualOwnership) {
                    #ifndef QT_NO_DEBUG
                        if (current_ownership.toInt() == SpecificObserverOwnership) {
                            // Leave it as it is, it will already be SpecificObserverOwnership.
                            management_policy_string = "Manual Ownership (current: Specific Observer Ownership)";
                        } else if (current_ownership.toInt() == ObserverScopeOwnership) {
                            // Leave it as it is, it will already be ObserverScopeOwnership.
                            management_policy_string = "Manual Ownership (current: Observer Scope Ownership)";
                        } else {
                            // Leave it as it is, it will already be manual ownership.
                            if (obj->parent())
                                management_policy_string = "Manual Ownership (current: Manual Ownership with parent())";
                            else
                                management_policy_string = "Manual Ownership (current: Manual Ownership without parent())";
                        }
                    #endif
                } else if (object_ownership == AutoOwnership) {
                    if (current_ownership.toInt() == SpecificObserverOwnership) {
                          // Leave it as it is, it will already be SpecificObserverOwnership.
                        #ifndef QT_NO_DEBUG
                            management_policy_string = "Auto Ownership (kept Specific Observer Ownership)";
                        #endif
                    } else if (current_ownership.toInt() == ObserverScopeOwnership) {
                        // Leave it as it is, it will already be ObserverScopeOwnership.
                        #ifndef QT_NO_DEBUG
                            management_policy_string = "Auto Ownership (kept Observer Scope Ownership)";
                        #endif
                    } else {
                        // Check if the object already has a parent, otherwise we handle it as ObserverScopeOwnership.
                        if (!obj->parent()) {
                            setMultiContextPropertyValue(obj,qti_prop_OWNERSHIP,QVariant(ObserverScopeOwnership));
                            #ifndef QT_NO_DEBUG
                                management_policy_string = "Auto Ownership (had no parent, using Observer Scope Ownership)";
                            #endif
                        } else {
                            setMultiContextPropertyValue(obj,qti_prop_OWNERSHIP,QVariant(ManualOwnership));
                            #ifndef QT_NO_DEBUG
                                management_policy_string = "Auto Ownership (had parent, leave as Manual Ownership)";
                            #endif
                        }
                        setMultiContextPropertyValue(obj,qti_prop_PARENT_ID,QVariant(-1));
                    }
                } else if (object_ownership == OwnedBySubjectOwnership) {
                    connect(obj,SIGNAL(destroyed()),SLOT(deleteLater()));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Owned By Subject Ownership (this context is now dependant on this subject, but the original ownership of the subject was not changed)";
                    #endif
                    obj->setParent(0);
                }
            } else {
                // When OwnedBySubjectOwnership, the new ownership is ignored. Thus when a subject was attached to a
                // context using OwnedBySubjectOwnership it is attached to all other contexts after that using OwnedBySubjectOwnership
                // as well.
                // This observer must be deleted as soon as this subject is deleted.
                connect(obj,SIGNAL(destroyed()),SLOT(deleteLater()));
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Owned By Subject Ownership (was already observed using this ownership type).";
                #endif
            }
        } else {
            SharedProperty ownership_property(qti_prop_OWNERSHIP,QVariant(object_ownership));
            ObjectManager::setSharedProperty(obj,ownership_property);
            SharedProperty observer_parent_property(qti_prop_PARENT_ID,QVariant(-1));
            ObjectManager::setSharedProperty(obj,observer_parent_property);
            if (object_ownership == ManualOwnership) {
                // We don't care about this object's lifetime, its up to the user to manage the lifetime of this object.
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Manual Ownership";
                #endif
            } else if (object_ownership == AutoOwnership) {
                // Check if the object already has a parent, otherwise we handle it as ObserverScopeOwnership.
                if (!obj->parent()) {
                    setMultiContextPropertyValue(obj,qti_prop_OWNERSHIP,QVariant(ObserverScopeOwnership));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Auto Ownership (had no parent, using Observer Scope Ownership)";
                    #endif
                } else {
                    setMultiContextPropertyValue(obj,qti_prop_OWNERSHIP,QVariant(ManualOwnership));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Auto Ownership (had parent, leave as Manual Ownership)";
                    #endif
                }
            } else if (object_ownership == SpecificObserverOwnership) {
                // This observer must be its parent.
                setMultiContextPropertyValue(obj,qti_prop_OWNERSHIP,QVariant(SpecificObserverOwnership));
                setMultiContextPropertyValue(obj,qti_prop_PARENT_ID,QVariant(observerID()));
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Specific Observer Ownership";
                #endif
                // QWidget's parent must be another QWidget, thus the rules don't apply to QWidgets.
                if (!obj->inherits("QWidget"))
                    obj->setParent(this);
            } else if (object_ownership == ObserverScopeOwnership) {
                // This object must be deleted as soon as its not observed by any observers any more.
                //obj->setParent(0);
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Observer Scope Ownership";
                #endif
                // Don't set parent to 0 if its a widget.
                if (!obj->inherits("QWidget"))
                    obj->setParent(0);
            } else if (object_ownership == OwnedBySubjectOwnership) {
                // This observer must be deleted as soon as this subject is deleted.
                connect(obj,SIGNAL(destroyed()),SLOT(deleteLater()));
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Owned By Subject Ownership";
                #endif
                if (!obj->inherits("QWidget"))
                    obj->setParent(0);
            }
        }

        // Install the observer as an eventFilter on the subject.
        // We do this last, otherwise all dynamic property changes will go through this event filter.
        if (obj->thread() == thread() && observerData->filter_subject_events_enabled)
            obj->installEventFilter(this);

        // Check if this is an observer:
        bool has_mod_iface = false;
        Observer* obs = qobject_cast<Observer*> (obj);
        if (!obs) {
            foreach(QObject* child, obj->children()) {
                obs = qobject_cast<Observer*> (child);
                if (obs) {
                    break;
                }
            }
        }
        if (obs) {
            has_mod_iface = true;
            connect(obs,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
            connect(obs,SIGNAL(dataChanged(Observer*)),SIGNAL(dataChanged(Observer*)));
            connect(obs,SIGNAL(layoutChanged(QList<QPointer<QObject> >)),SIGNAL(layoutChanged(QList<QPointer<QObject> >)));
        } else {
            // Check if the new subject implements the IModificationNotifier interface. If so we connect
            // to the modification changed signals:
            IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (obj);
            if (mod_iface) {
                connect(mod_iface->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
                has_mod_iface = true;
            }
        }

        // Emit neccesarry signals
        QList<QPointer<QObject> > objects;
        objects << obj;
        setModificationState(true);
        if (!observerData->process_cycle_active) {
            emit numberOfSubjectsChanged(Observer::SubjectAdded, objects);
            emit layoutChanged(objects);
        }

        #ifndef QT_NO_DEBUG
        if (!observerData->process_cycle_active) {
            if (has_mod_iface)
                LOG_TRACE(QString("Observer (%1): Now observing object \"%2\" with management policy: %3. This object's modification state is now monitored by this observer.").arg(objectName()).arg(obj->objectName()).arg(management_policy_string));
            else
                LOG_TRACE(QString("Observer (%1): Now observing object \"%2\" with management policy: %3.").arg(objectName()).arg(obj->objectName()).arg(management_policy_string));
        }
        #endif
    } else {
        // If it is the global object manager it will get here.
        observerData->subject_list.append(obj);

        // Install the observer as an eventFilter on the subject.
        // We do this last, otherwise all dynamic property changes will go through this event filter.
        if (obj->thread() == thread() && observerData->filter_subject_events_enabled)
            obj->installEventFilter(this);

        // Emit neccesarry signals
        QList<QPointer<QObject> > objects;
        objects << obj;
        setModificationState(true);
//        if (!observerData->process_cycle_active) {
//            emit numberOfSubjectsChanged(Observer::SubjectAdded, objects);
//            emit layoutChanged(objects);
//        }

        LOG_TRACE(QString("Object \"%1\" is now visible in the global object pool.").arg(obj->objectName()));
    }

    // Finalize the attachment in all subject filters, indicating that the attachment was succesfull.
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        observerData->subject_filters.at(i)->finalizeAttachment(obj,true,import_cycle);
    }

    return true;
}

QList<QPointer<QObject> > Qtilities::Core::Observer::attachSubjects(QList<QObject*> objects, Observer::ObjectOwnership ownership, QString* rejectMsg, bool import_cycle) {
    QList<QPointer<QObject> > success_list;
    startProcessingCycle();
    for (int i = 0; i < objects.count(); i++) {
        if (attachSubject(objects.at(i), ownership, rejectMsg, import_cycle))
            success_list << objects.at(i);
    }
    if (success_list.count() > 0) {
        emit numberOfSubjectsChanged(SubjectAdded, success_list);
        emit layoutChanged(success_list);
        setModificationState(true);
    }
    endProcessingCycle();
    return success_list;
}

QList<QPointer<QObject> > Qtilities::Core::Observer::attachSubjects(ObserverMimeData* mime_data_object, Observer::ObjectOwnership ownership, QString* rejectMsg, bool import_cycle) {
    QList<QPointer<QObject> > success_list;
    startProcessingCycle();
    for (int i = 0; i < mime_data_object->subjectList().count(); i++) {
        if (attachSubject(mime_data_object->subjectList().at(i), ownership, rejectMsg, import_cycle))
            success_list << mime_data_object->subjectList().at(i);
    }
    if (success_list.count() > 0) {
        emit numberOfSubjectsChanged(SubjectAdded, success_list);
        emit layoutChanged(success_list);
        setModificationState(true);
    }
    endProcessingCycle();
    return success_list;
}

Qtilities::Core::Observer::EvaluationResult Qtilities::Core::Observer::canAttach(QObject* obj, Observer::ObjectOwnership, QString* rejectMsg, bool silent) const {
    if (!obj) {
        if (rejectMsg)
            *rejectMsg = tr("Invalid object reference received. Attachment cannot be done.");
        return Observer::Rejected;
    }

    QVariant category_variant = getMultiContextPropertyValue(obj,qti_prop_CATEGORY_MAP);
    QtilitiesCategory category = category_variant.value<QtilitiesCategory>();
    if (isConst(category)) {
        QString reject_string = QString(tr("Attaching object \"%1\" to observer \"%2\" is not allowed. This observer is const for the recieved object.")).arg(obj->objectName()).arg(objectName());
        LOG_WARNING(reject_string);
        if (rejectMsg)
            *rejectMsg = reject_string;
        return Observer::Rejected;
    }

    // Check for circular dependancies:
    // TODO: Is this going to work for containment approach?
    const Observer* observer_cast = qobject_cast<Observer*> (obj);
    if (observer_cast) {
        if (isParentInHierarchy(observer_cast,this)) {
            QString reject_string = QString(tr("Attaching observer \"%1\" to observer \"%2\" will result in a circular dependancy.")).arg(obj->objectName()).arg(objectName());
            LOG_WARNING(reject_string);
            if (rejectMsg)
                *rejectMsg = reject_string;
            return Observer::Rejected;
        }
    }

    // First evaluate new subject from Observer side:
    if (observerData->subject_limit == observerData->subject_list.count()) {
        QString reject_string = QString(tr("Observer (%1): Object (%2) attachment failed, subject limit reached.")).arg(objectName()).arg(obj->objectName());
        LOG_WARNING(reject_string);
        if (rejectMsg)
            *rejectMsg = reject_string;
        return Observer::Rejected;
    }


    if (objectName() != QString(qti_def_GLOBAL_OBJECT_POOL)) {
        // Check if this subject is already monitored by this observer, if so abort.
        // This will ensure that no subject filters need to check for this, thus subject filters can assume that new attachments are actually new.
        MultiContextProperty observer_list = ObjectManager::getMultiContextProperty(obj,qti_prop_OBSERVER_MAP);
        if (observer_list.isValid()) {
            if (observer_list.hasContext(observerData->observer_id)) {
                QString reject_string = QString(tr("Observer (%1): Object (%2) attachment failed, object is already observed by this observer.")).arg(objectName()).arg(obj->objectName());
                LOG_WARNING(reject_string);
                if (rejectMsg)
                    *rejectMsg = reject_string;
                return Observer::Rejected;
            }
        }

        // Evaluate dynamic properties on the object:
        bool has_limit = false;
        int observer_limit = -1;
        int observer_count = parentCount(obj);

        QVariant observer_limit_variant = getMultiContextPropertyValue(obj,qti_prop_OBSERVER_LIMIT);
        if (observer_limit_variant.isValid()) {
            observer_limit = observer_limit_variant.toInt(&has_limit);
        }

        if (has_limit) {
            if (observer_count == -1) {
                observer_count = 0;
                // No count yet, check if the limit is > 0
                if ((observer_limit < 1) && (observer_limit != -1)){
                    QString reject_string = QString(tr("Observer (%1): Object (%2) attachment failed, observer limit (%3) reached.")).arg(objectName()).arg(obj->objectName()).arg(observer_limit);
                    LOG_WARNING(reject_string);
                    if (rejectMsg)
                        *rejectMsg = reject_string;
                    return Observer::Rejected;
                }
            } else {
                if (observer_count >= observer_limit) {
                    QString reject_string = QString(tr("Observer (%1): Object (%2) attachment failed, observer limit (%3) reached.")).arg(objectName()).arg(obj->objectName()).arg(observer_limit);
                    LOG_WARNING(reject_string);
                    if (rejectMsg)
                        *rejectMsg = reject_string;
                    return Observer::Rejected;
                }
            }
        }
    }

    // Evaluate attachment in all installed subject filters
    bool was_rejected = false;
    bool was_conditional = false;
    AbstractSubjectFilter::EvaluationResult current_filter_evaluation;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        current_filter_evaluation = observerData->subject_filters.at(i)->evaluateAttachment(obj,rejectMsg,silent);
        if (current_filter_evaluation == AbstractSubjectFilter::Rejected)
            was_rejected = true;
        if (current_filter_evaluation == AbstractSubjectFilter::Conditional)
            was_conditional = true;
    }

    if (was_rejected)
        return Observer::Rejected;

    if (was_conditional)
        return Observer::Conditional;

    return Observer::Allowed;
}

Qtilities::Core::Observer::EvaluationResult Qtilities::Core::Observer::canAttach(ObserverMimeData* mime_data_object, QString* rejectMsg, bool silent) const {
    Q_UNUSED(rejectMsg)

    if (!mime_data_object)
        return Observer::Rejected;

    bool success = true;
    int not_allowed_count = 0;
    for (int i = 0; i < mime_data_object->subjectList().count(); i++) {
        if (canAttach(mime_data_object->subjectList().at(i),Observer::ManualOwnership,0,silent) == Observer::Rejected) {
            success = false;
            ++not_allowed_count;
        }
    }

    if (success)
        return Observer::Allowed;
    else {
        if (not_allowed_count != mime_data_object->subjectList().count())
            return Observer::Conditional;
        else
            return Observer::Rejected;
    }
}

void Qtilities::Core::Observer::handle_deletedSubject(QObject* obj) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

    // Pass object through all installed subject filters
    bool passed_filters = true;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        passed_filters = observerData->subject_filters.at(i)->initializeDetachment(obj,0,true);
    }

    if (!passed_filters) {
        LOG_ERROR(QString(tr("Observer (%1): Error: Subject filter rejected detachment of deleted object (%2).")).arg(objectName()).arg(obj->objectName()));
    }

    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        observerData->subject_filters.at(i)->finalizeDetachment(obj,passed_filters,true);
    }

    LOG_TRACE(QString("Observer (%1) detected deletion of object (%2), updated observer context accordingly.").arg(objectName()).arg(obj->objectName()));

    emit subjectDeleted(obj);

    // Emit neccesarry signals
    setModificationState(true);
    if (!observerData->process_cycle_active) {
        QList<QPointer<QObject> > objects;
        objects << obj;
        emit numberOfSubjectsChanged(SubjectRemoved, objects);
        emit layoutChanged(QList<QPointer<QObject> >());
    }
}

bool Qtilities::Core::Observer::detachSubject(QObject* obj) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    if (canDetach(obj) == Rejected)
        return false;

    bool currrent_filter_subject_events_enabled = observerData->filter_subject_events_enabled;
    observerData->filter_subject_events_enabled = false;

    // Pass object through all installed subject filters
    bool passed_filters = true;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        passed_filters = observerData->subject_filters.at(i)->initializeDetachment(obj);
        if (!passed_filters)
            break;
    }

    if (!passed_filters) {
        LOG_WARNING(QString(tr("Observer (%1): Object (%2) detachment failed, detachment was rejected by one or more subject filters.")).arg(objectName()).arg(obj->objectName()));
        for (int i = 0; i < observerData->subject_filters.count(); i++) {
            observerData->subject_filters.at(i)->finalizeDetachment(obj,false);
        }
        return false;
    } else {
        for (int i = 0; i < observerData->subject_filters.count(); i++) {
            observerData->subject_filters.at(i)->finalizeDetachment(obj,true);
        }
    }

    // Keeps track if the object went out of scope:
    bool lost_scope = false;

    if (objectName() != QString(qti_def_GLOBAL_OBJECT_POOL)) {
        #ifndef QT_NO_DEBUG
            QString debug_name = obj->objectName();
        #endif

        // Check the ownership property of this object
        QVariant ownership_variant = getMultiContextPropertyValue(obj,qti_prop_OWNERSHIP);
        if (ownership_variant.isValid() && ((ObjectOwnership) ownership_variant.toInt() == ObserverScopeOwnership)) {
            if ((parentCount(obj) == 1) && obj) {
                LOG_DEBUG(QString("Object (%1) went out of scope, it will be deleted.").arg(obj->objectName()));
                deleteObject(obj);
                obj = 0;
                QCoreApplication::processEvents();
                lost_scope = true;
            } else {
                removeQtilitiesProperties(obj);
                observerData->subject_list.removeOne(obj);
            }
        } else if (ownership_variant.isValid() && ((ObjectOwnership) ownership_variant.toInt() == SpecificObserverOwnership)) {
            QVariant observer_parent = getMultiContextPropertyValue(obj,qti_prop_PARENT_ID);
            if (observer_parent.isValid() && (observer_parent.toInt() == observerID()) && obj) {
                deleteObject(obj);
                obj = 0;
                QCoreApplication::processEvents();
                lost_scope = true;
            } else {
                removeQtilitiesProperties(obj);
                observerData->subject_list.removeOne(obj);
            }
        } else {
            removeQtilitiesProperties(obj);
            observerData->subject_list.removeOne(obj);
        }

        #ifndef QT_NO_DEBUG
             LOG_DEBUG(QString("Observer (%1): Not observing object (%2) anymore.").arg(objectName()).arg(debug_name));
        #endif
    }

    // Disconnect all signals on this object:
    if (obj) {
        obj->disconnect(this);
        obj->removeEventFilter(this);
    }

    // Broadcast if neccesarry:
    setModificationState(true);
    if (!observerData->process_cycle_active) {
        QList<QPointer<QObject> > objects;
        if (!lost_scope)
            objects << obj;
        emit numberOfSubjectsChanged(SubjectRemoved, objects);
        emit layoutChanged(QList<QPointer<QObject> >());
    }

    observerData->filter_subject_events_enabled = currrent_filter_subject_events_enabled;
    return true;
}

QList<QPointer<QObject> > Qtilities::Core::Observer::detachSubjects(QList<QObject*> objects) {
    QList<QPointer<QObject> > success_list;
    startProcessingCycle();

    QList<QPointer<QObject> > safe_list;
    for (int i = 0; i < objects.count(); i++) {
        if (objects.at(i)) {
            safe_list << objects.at(i);
            if (detachSubject(safe_list.at(i))) {
                // The object could have been deleted in detach subject, thus we must use some
                // safe QPointers here:
                if (safe_list.at(i))
                    success_list << safe_list.at(i);
            }
        }
    }

    // Broadcast if neccesarry
    if (success_list.count() > 0) {
        setModificationState(true);
        emit numberOfSubjectsChanged(SubjectRemoved, success_list);
        emit layoutChanged(QList<QPointer<QObject> >());
    }

    endProcessingCycle();
    return success_list;
}

Qtilities::Core::Observer::EvaluationResult Qtilities::Core::Observer::canDetach(QObject* obj) const {
    if (objectName() != QString(qti_def_GLOBAL_OBJECT_POOL)) {
        // Check if this subject is observed by this observer. If its not observed by this observer, we can't detach it.
        MultiContextProperty observer_list_variant = ObjectManager::getMultiContextProperty(obj,qti_prop_OBSERVER_MAP);
        if (observer_list_variant.isValid()) {
            if (!observer_list_variant.hasContext(observerData->observer_id)) {
                LOG_DEBUG(QString("Observer (%1): Object (%2) detachment is not allowed, object is not observed by this observer.").arg(observerData->observer_id).arg(obj->objectName()));
                return Observer::Rejected;
            }
        } else {
            // This subject is not observed by anything, or obj points to a deleted object, thus just quit.
            return Observer::Rejected;
        }

        // Validate operation against access mode
        QVariant category_variant = getMultiContextPropertyValue(obj,qti_prop_CATEGORY_MAP);
        QtilitiesCategory category = category_variant.value<QtilitiesCategory>();
        if (isConst(category)) {
            LOG_DEBUG(QString("Detaching object \"%1\" from observer \"%2\" is not allowed. This observer is const for the recieved object.").arg(obj->objectName()).arg(objectName()));
            return Observer::Rejected;
        }

        // Check the ownership property of this object
        QVariant ownership_variant = getMultiContextPropertyValue(obj,qti_prop_OWNERSHIP);
        if (ownership_variant.isValid() && (ownership_variant.toInt() == ObserverScopeOwnership)) {
            if (parentCount(obj) == 1)
                return Observer::LastScopedObserver;
        } else if (ownership_variant.isValid() && (ownership_variant.toInt() == SpecificObserverOwnership)) {
            QVariant observer_parent = getMultiContextPropertyValue(obj,qti_prop_PARENT_ID);
            if (observer_parent.isValid() && (observer_parent.toInt() == observerID()) && obj) {
                return Observer::IsParentObserver;
            }
        } else if (ownership_variant.isValid() && (ownership_variant.toInt() == OwnedBySubjectOwnership)) {
            LOG_DEBUG(QString("Detaching object \"%1\" from observer \"%2\" is not allowed. This observer is dependant on this subject. To remove the subject permanently, delete it.").arg(obj->objectName()).arg(objectName()));
            return Observer::Rejected;
        }
    }

    // Evaluate detachment in all installed subject filters
    bool was_rejected = false;
    bool was_conditional = false;
    AbstractSubjectFilter::EvaluationResult current_filter_evaluation;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        current_filter_evaluation = observerData->subject_filters.at(i)->evaluateDetachment(obj);
        if (current_filter_evaluation == AbstractSubjectFilter::Rejected)
            was_rejected = true;
        if (current_filter_evaluation == AbstractSubjectFilter::Conditional)
            was_conditional = true;
    }

    if (was_rejected)
        return Observer::Rejected;

    if (was_conditional)
        return Observer::Conditional;

    return Observer::Allowed;
}

void Qtilities::Core::Observer::detachAll() {
    emit allSubjectsAboutToBeDetached();

    int start_count = subjectCount();
    bool current_broadcast = broadcastModificationStateChangesEnabled();
    toggleBroadcastModificationStateChanges(false);

    // Updating is done in detachSubjects().
    detachSubjects(subjectReferences());

    toggleBroadcastModificationStateChanges(current_broadcast);

    int end_count = subjectCount();
    if (start_count != end_count)
        setModificationState(true);

    emit allSubjectsDetached();
}

void Qtilities::Core::Observer::deleteAll() {  
    int total = observerData->subject_list.count();
    if (total == 0)
        return;

    emit allSubjectsAboutToBeDeleted();

    bool current_broadcast = broadcastModificationStateChangesEnabled();
    toggleBroadcastModificationStateChanges(false);

    startProcessingCycle();
    for (int i = 0; i < total; i++) {
        // Validate operation against access mode if access mode scope is category:
        QVariant category_variant = getMultiContextPropertyValue(observerData->subject_list.at(0),qti_prop_CATEGORY_MAP);
        QtilitiesCategory category = category_variant.value<QtilitiesCategory>();
        if (!isConst(category)) {
            deleteObject(observerData->subject_list.at(0));
        }
    }

    QCoreApplication::processEvents();
    toggleBroadcastModificationStateChanges(current_broadcast);

    int end_count = subjectCount();
    if (total != end_count)
        setModificationState(true);

    endProcessingCycle();
    emit allSubjectsDeleted();
}

QVariant Qtilities::Core::Observer::getMultiContextPropertyValue(const QObject* obj, const char* property_name) const {
    #ifndef QT_NO_DEBUG
        if (!obj)
            qDebug() << "Failed to get property \"" << property_name << "\" on null object";
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return QVariant();
    #endif

    QVariant prop;
    prop = obj->property(property_name);

    if (prop.isValid() && prop.canConvert<SharedProperty>()) {
        // This is a shared property
        return (prop.value<SharedProperty>()).value();
    } else if (prop.isValid() && prop.canConvert<MultiContextProperty>()) {
        // This is a normal multi context property (not shared)
        return (prop.value<MultiContextProperty>()).value(observerData->observer_id);
    } else if (!prop.isValid()) {
        return QVariant();
    } else {
        LOG_TRACE(QString("Observer (%1): Getting of property (%2) failed, property not recognized as a qtilities property type.").arg(objectName()).arg(property_name));
        return QVariant();
    }
}

bool Qtilities::Core::Observer::setMultiContextPropertyValue(QObject* obj, const char* property_name, const QVariant& new_value) const {
    #ifndef QT_NO_DEBUG
        if (!obj)
            qDebug() << "Failed to set property \"" << property_name  << "\" on null object";
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    QVariant prop;
    prop = obj->property(property_name);

    // Important, we do not just use the setValue() functions on the ObserverProperties, we call
    // obj->setProperty to make sure the QDynamicPropertyChangeEvent event is triggered.
    if (prop.isValid() && prop.canConvert<SharedProperty>()) {
        // This is a shared property
        SharedProperty shared_property = prop.value<SharedProperty>();
        shared_property.setValue(new_value);
        ObjectManager::setSharedProperty(obj,shared_property);
        return true;
    } else if (prop.isValid() && prop.canConvert<MultiContextProperty>()) {
        // This is a normal multi context property (not shared)
        MultiContextProperty observer_property = prop.value<MultiContextProperty>();
        observer_property.setValue(new_value,observerData->observer_id);
        ObjectManager::setMultiContextProperty(obj,observer_property);
        return true;
    } else {
        QString error_str = QString(tr("Observer (%1): Setting the value of property (%2) failed. This property is not yet set as an MultiContextProperty type class.")).arg(objectName()).arg(property_name);
        LOG_FATAL(error_str);
        qDebug() << error_str;
        // Assert here, otherwise you will think that the property is being set and you won't understand why something else does not work.
        // If you get here, you need to create the property you need, and then set it using the setMultiContextProperty() or setSharedProperty() calls.
        // This function is not the correct one to use in your situation.
        Q_ASSERT(0);
    }

    return false;
}

void Qtilities::Core::Observer::removeQtilitiesProperties(QObject* obj) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

    bool currrent_filter_subject_events_enabled = observerData->filter_subject_events_enabled;
    bool currrent_deliver_qtilities_property_changed_events = observerData->deliver_qtilities_property_changed_events;
    observerData->filter_subject_events_enabled = false;
    observerData->deliver_qtilities_property_changed_events = false;

    // This is usefull when you are adding properties and you encounter an error, in that case this function
    // can be used as sort of a rollback, removing the property changes that have been made up to that point.

    // Build up string list with all properties:
    QStringList added_properties;
    added_properties.append(monitoredProperties());
    added_properties.append(reservedProperties());

    // Remove all the contexts first.
    foreach (QString property_name, added_properties) {
        MultiContextProperty prop = ObjectManager::getMultiContextProperty(obj, property_name.toStdString().data());
        if (prop.isValid()) {
            // If it exists, we remove this observer context:
            prop.removeContext(observerData->observer_id);
            ObjectManager::setMultiContextProperty(obj, prop);
        }
    }

    // If the count is zero after removing the contexts, remove all properties:
    if (parentCount(obj) == 0) {
        foreach (QString property_name, added_properties) {
            if (property_name != QString(qti_prop_NAME))
                obj->setProperty(property_name.toStdString().data(),QVariant());
        }
    }

    observerData->filter_subject_events_enabled = currrent_filter_subject_events_enabled;
    observerData->deliver_qtilities_property_changed_events = currrent_deliver_qtilities_property_changed_events;
}

bool Qtilities::Core::Observer::isParentInHierarchy(const Observer* obj_to_check, const Observer* observer) {
    // Get all the parents of observer
    MultiContextProperty context_map_prop = ObjectManager::getMultiContextProperty(observer, qti_prop_OBSERVER_MAP);
    int observer_count;
    bool is_parent = false;

    if (context_map_prop.isValid()) {
        observer_count = context_map_prop.contextMap().count();
        // Check all direct parents:
        for (int i = 0; i < observer_count; i++) {
            Observer* parent = OBJECT_MANAGER->observerReference(context_map_prop.contextMap().keys().at(i));
            if (parent != obj_to_check) {
                is_parent = isParentInHierarchy(obj_to_check,parent);
                if (is_parent)
                    break;
            } else
                return true;
        }
    } else {
        // Check above all contained observer parents:
        if (observer->parent()) {
            MultiContextProperty parent_context_map_prop = ObjectManager::getMultiContextProperty(observer->parent(), qti_prop_OBSERVER_MAP);
            int observer_count;
            if (parent_context_map_prop.isValid())
                observer_count = parent_context_map_prop.contextMap().count();
            else
                return false;

            // Check all direct parents:
            for (int i = 0; i < observer_count; i++) {
                Observer* parent = OBJECT_MANAGER->observerReference(parent_context_map_prop.contextMap().keys().at(i));
                if (parent != obj_to_check) {
                    is_parent = isParentInHierarchy(obj_to_check,parent);
                    if (is_parent)
                        break;
                } else
                    return true;
            }
        }
    }

    return is_parent;
}

bool Qtilities::Core::Observer::isConst(const QtilitiesCategory& category) const {
    AccessMode mode;
    if (category.isEmpty() || (observerData->access_mode_scope == GlobalScope)) {
        mode = (AccessMode) observerData->access_mode;
    } else {
        if (hasCategory(category)) {
            mode = categoryAccessMode(category);
        } else {
            mode = (AccessMode) observerData->access_mode;
        }
    }

    if (mode == LockedAccess || mode == ReadOnlyAccess)
        return true;
    else
        return false;
}

bool Qtilities::Core::Observer::setSubjectLimit(int subject_limit) {
    // Check if this observer is read only
    if ((observerData->access_mode == ReadOnlyAccess || observerData->access_mode == LockedAccess) && observerData->access_mode_scope == GlobalScope) {
        LOG_ERROR(QString(tr("Setting the subject limit for observer \"%1\" failed. This observer is read only / locked.").arg(objectName())));
        return false;
    }

    if ((subject_limit < observerData->subject_list.count()) && (subject_limit != -1)) {
        LOG_ERROR(QString(tr("Setting the subject limit for observer \"%1\" failed, this observer is currently observing more subjects than the desired new limit.").arg(objectName())));
        return false;
    } else {
        observerData->subject_limit = subject_limit;
        setModificationState(true);
        return true;
    }
}

void Qtilities::Core::Observer::setAccessMode(AccessMode mode, QtilitiesCategory category) {
    // Check if this observer is read only
    if ((observerData->access_mode == ReadOnlyAccess || observerData->access_mode == LockedAccess) && observerData->access_mode_scope == GlobalScope) {
        LOG_ERROR(QString(tr("Setting the access mode for observer \"%1\" failed. This observer is read only / locked.")).arg(objectName()));
        return;
    }

    if (category.isEmpty())
        observerData->access_mode = (int) mode;
    else {
        // Check if this category exists in this observer context:
        if (!hasCategory(category)) {
            LOG_ERROR(QString(tr("Observer \"%1\" does not have category \"%2\", access mode cannot be set.")).arg(objectName()).arg(category.toString(",")));
            return;
        }

        // Set the scope for the category
        for (int i = 0; i < observerData->categories.count(); i++) {
            if (observerData->categories.at(i) == category) {
                observerData->categories.removeAt(i);
                break;
            }
        }

        category.setAccessMode((int) mode);
        observerData->categories.push_back(category);
    }

    emit layoutChanged(QList<QPointer<QObject> >());
    setModificationState(true);
}

Qtilities::Core::Observer::AccessMode Qtilities::Core::Observer::accessMode(QtilitiesCategory category) const {
    if (category.isEmpty())
        return (AccessMode) observerData->access_mode;
    else {
        if (!hasCategory(category)) {
            return Observer::InvalidAccess;
        }

        // Loop through the categories list until we find the category:
        for (int i = 0; i < observerData->categories.count(); i++) {
            if (observerData->categories.at(i) == category) {
                return (AccessMode) observerData->categories.at(i).accessMode();
            }
        }
    }

    return Observer::InvalidAccess;
}

void Qtilities::Core::Observer::setObjectDeletionPolicy(ObjectDeletionPolicy object_deletion_policy) {
    observerData->object_deletion_policy = (int) object_deletion_policy;
}


Qtilities::Core::Observer::ObjectDeletionPolicy Qtilities::Core::Observer::objectDeletionPolicy() const {
    return (ObjectDeletionPolicy) observerData->object_deletion_policy;
}

void Qtilities::Core::Observer::setAccessModeScope(AccessModeScope access_mode_scope) {
    if (observerData->access_mode_scope == access_mode_scope)
        return;

    observerData->access_mode_scope = (int) access_mode_scope;
    emit layoutChanged(QList<QPointer<QObject> >());
    setModificationState(true);
}

QString Qtilities::Core::Observer::observerName(int parent_id) const {
    if (parent_id == -1)
        return objectName();
    else {
        const Observer* obs = OBJECT_MANAGER->observerReference(parent_id);
        if (obs) {
            if (obs->contains(obs))
                return obs->subjectNameInContext(this);
            else
                return obs->subjectNameInContext(parent());
        } else {
            return objectName();
        }
    }
}

QString Qtilities::Core::Observer::subjectNameInContext(const QObject* obj) const {
    if (!obj)
        return QString();

    // Check if the object is in this context:
    if (contains(obj) || contains(obj->parent())) {
        // We need to check if a subject has a instance name in this context. If so, we use the instance name, not the objectName().
        QVariant instance_name = getMultiContextPropertyValue(obj,qti_prop_ALIAS_MAP);
        if (instance_name.isValid())
            return instance_name.toString();
        else
            return obj->objectName();
    }

    return QString();
}

Qtilities::Core::QtilitiesCategory Qtilities::Core::Observer::subjectCategoryInContext(const QObject* obj) const {
    if (!obj)
        return QtilitiesCategory();

    // Check if the object is in this context:
    if (contains(obj) || contains(obj->parent())) {
        // We need to check if a subject has a category name in this context. If so, we use the instance name, not the objectName().
        QVariant category_property = getMultiContextPropertyValue(obj,qti_prop_CATEGORY_MAP);
        if (category_property.isValid()) {
            return (category_property.value<QtilitiesCategory>());
        } else
            return QtilitiesCategory();
    }

    return QtilitiesCategory();
}

QString Qtilities::Core::Observer::subjectDisplayedNameInContext(const QObject* obj) const {
    if (!obj)
        return QString();

    // Check if the object is in this context:
    if (contains(obj) || contains(obj->parent())) {
        // We need to check if a subject has a instance name in this context. If so, we use the instance name, not the objectName().
        QVariant instance_name = getMultiContextPropertyValue(obj,qti_prop_DISPLAYED_ALIAS_MAP);
        if (instance_name.isValid())
            return instance_name.toString();
        else {
            // We need to check if a subject has a instance name in this context. If so, we use the instance name, not the objectName().
            QVariant instance_name = getMultiContextPropertyValue(obj,qti_prop_ALIAS_MAP);
            if (instance_name.isValid())
                return instance_name.toString();
            else
                return obj->objectName();
        }
    }

    return QString();
}

Qtilities::Core::Observer::ObjectOwnership Qtilities::Core::Observer::subjectOwnershipInContext(const QObject* obj) const {
    if (!obj)
        return ManualOwnership;

    // Check if the object is in this context:
    if (contains(obj) || contains(obj->parent())) {
        QVariant current_ownership = getMultiContextPropertyValue(obj,qti_prop_OWNERSHIP);
        Observer::ObjectOwnership ownership = (Observer::ObjectOwnership) current_ownership.toInt();
        return ownership;
    }

    return ManualOwnership;
}

int Qtilities::Core::Observer::treeCount(const QString& base_class_name) const {
    return treeChildren(base_class_name).count();
}

int Qtilities::Core::Observer::subjectCount(const QString& base_class_name) const {
    return subjectReferences(base_class_name).count();
}

QObject* Qtilities::Core::Observer::treeAt(int i) const {
    if (i < 0)
        return 0;

    QList<QObject*> list = treeChildren();
    if (i >= list.count())
        return 0;

    return list.at(i);
}

bool Qtilities::Core::Observer::treeContains(QObject* tree_item) const {
     return treeChildren().contains(tree_item);
}

QList<QObject*> Qtilities::Core::Observer::treeChildren(const QString& iface) const {
    QList<QObject*> children;

    TreeIterator itr(this);
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        if (iface.isEmpty())
            children << obj;
        else {
            if (obj->inherits(iface.toAscii().data()))
                children << obj;
        }
    }

    return children;
}

QStringList Qtilities::Core::Observer::subjectNames(const QString& iface) const {
    QStringList subject_names;

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        if (observerData->subject_list.at(i)->inherits(iface.toAscii().data()) || iface.isEmpty()) {
            // We need to check if a subject has an instance name in this context. If so, we use the instance name, not the objectName().
            QVariant instance_name = getMultiContextPropertyValue(observerData->subject_list.at(i),qti_prop_ALIAS_MAP);
            if (instance_name.isValid())
                subject_names << instance_name.toString();
            else
                subject_names << observerData->subject_list.at(i)->objectName();
        }
    }
    return subject_names;
}

QStringList Qtilities::Core::Observer::subjectDisplayedNames(const QString& iface) const {
    QStringList subject_names;

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        if (observerData->subject_list.at(i)->inherits(iface.toAscii().data()) || iface.isEmpty()) {
            // We need to check if a subject has an instance name in this context. If so, we use the instance name, not the objectName().
            QVariant instance_name = getMultiContextPropertyValue(observerData->subject_list.at(i),qti_prop_DISPLAYED_ALIAS_MAP);
            if (instance_name.isValid())
                subject_names << instance_name.toString();
            else {
                // We need to check if a subject has an instance name in this context. If so, we use the instance name, not the objectName().
                QVariant instance_name = getMultiContextPropertyValue(observerData->subject_list.at(i),qti_prop_ALIAS_MAP);
                if (instance_name.isValid())
                    subject_names << instance_name.toString();
                else
                    subject_names << observerData->subject_list.at(i)->objectName();
            }
        }
    }
    return subject_names;
}

QStringList Qtilities::Core::Observer::subjectNamesByCategory(const QtilitiesCategory& category) const {
    QStringList subject_names;

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QVariant category_variant = getMultiContextPropertyValue(subjectAt(i),qti_prop_CATEGORY_MAP);
        // Handles cases where category is valid, thus it contains levels.
        if (category_variant.isValid()) {
            QtilitiesCategory current_category = category_variant.value<QtilitiesCategory>();
            if (current_category == category) {
                // We need to check if a subject has an instance name in this context. If so, we use the instance name, not the objectName().
                QVariant instance_name = getMultiContextPropertyValue(observerData->subject_list.at(i),qti_prop_ALIAS_MAP);
                if (instance_name.isValid())
                    subject_names << instance_name.toString();
                else
                    subject_names << observerData->subject_list.at(i)->objectName();
            }
        } else {
            // Handle cases where the category is not valid on a subject.
            // Thus subjects without a category specified for them.
            if (!category.isValid()) {
                // We need to check if a subject has an instance name in this context. If so, we use the instance name, not the objectName().
                QVariant instance_name = getMultiContextPropertyValue(observerData->subject_list.at(i),qti_prop_ALIAS_MAP);
                if (instance_name.isValid())
                    subject_names << instance_name.toString();
                else
                    subject_names << observerData->subject_list.at(i)->objectName();
            }
        }
    }

    return subject_names;
}

bool Qtilities::Core::Observer::hasCategory(const QtilitiesCategory& category) const {
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QVariant category_variant = getMultiContextPropertyValue(subjectAt(i),qti_prop_CATEGORY_MAP);
        // Check if a category property exists:
        if (category_variant.isValid()) {
            QtilitiesCategory current_category = category_variant.value<QtilitiesCategory>();
            if (current_category == category)
                return true;
        }
    }

    return false;
}

QList<QPointer<QObject> > Qtilities::Core::Observer::renameCategory(const QtilitiesCategory& old_category,const QtilitiesCategory& new_category, bool match_exactly) {
    QList<QPointer<QObject> > renamed_list;

    startProcessingCycle();
    // Check the category on all subjects:
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QVariant category_variant = getMultiContextPropertyValue(subjectAt(i),qti_prop_CATEGORY_MAP);
        // Check if a category property exists:
        if (category_variant.isValid()) {
            QtilitiesCategory current_category = category_variant.value<QtilitiesCategory>();
            // Skip if current category is same as new category:
            if (current_category == new_category)
                continue;

            // Check if we match exactly. In that case, the depths must match:
            if (match_exactly) {
                if (current_category.categoryDepth() != old_category.categoryDepth())
                    continue;
            }

            QStringList subject_cat_list = current_category.toStringList(old_category.categoryDepth());

            // Check if we must update it:
            if (subject_cat_list == old_category.toStringList()) {
                // Construct the renamed category:
                QString new_cat_string = current_category.toString();
                new_cat_string.replace(old_category.toString(),new_category.toString());
                QtilitiesCategory renamed_category(new_cat_string,QString("::"));

                // Skip if renamed category is same as current category:
                if (renamed_category == current_category)
                    continue;

                // We need to update the access mode settings:
                bool took_cat = false;
                for (int c = 0; c < observerData->categories.count(); c++) {
                    if (observerData->categories.at(c) == current_category) {
                        observerData->categories.takeAt(c);
                        took_cat = true;
                        break;
                    }
                }
                if (took_cat)
                    observerData->categories << renamed_category;

                // Update the property on the current subject:
                setMultiContextPropertyValue(subjectAt(i),qti_prop_CATEGORY_MAP,qVariantFromValue(renamed_category));
                renamed_list << subjectAt(i);
            }
        }
    }
    endProcessingCycle(false);
    if (renamed_list.count() > 0)
        refreshViewsLayout();

    return renamed_list;
}

Qtilities::Core::Observer::AccessMode Qtilities::Core::Observer::categoryAccessMode(const QtilitiesCategory& category) const {
    // Check if this category exists in this observer context:
    if (!hasCategory(category)) {
        LOG_ERROR(QString(tr("Observer \"%1\" does not have category \"%2\", access mode cannot be set.")).arg(objectName()).arg(category.toString(",")));
        return InvalidAccess;
    }

    // Loop through the categories list until we find the category:
    for (int i = 0; i < observerData->categories.count(); i++) {
        if (observerData->categories.at(i) == category) {
            return (AccessMode) observerData->categories.at(i).accessMode();
        }
    }

    return InvalidAccess;
}

QList<Qtilities::Core::QtilitiesCategory> Qtilities::Core::Observer::subjectCategories() const {
    QList<QtilitiesCategory> subject_categories;

    for (int i = 0; i < observerData->subject_list.count(); i++) {    
        QVariant category_variant = getMultiContextPropertyValue(subjectAt(i),qti_prop_CATEGORY_MAP);
        // Check if a category property exists:
        if (category_variant.isValid()) {
            QtilitiesCategory current_category = category_variant.value<QtilitiesCategory>();
            // Check if the category is empty, if so add it to the uncategorized category:
            if (!current_category.isEmpty()) {
                if (!subject_categories.contains(current_category))
                    subject_categories << current_category;
            }
        }
    }

    return subject_categories;
}

QObject* Qtilities::Core::Observer::subjectAt(int i) const {
    if ((i < 0) || i >= subjectCount())
        return 0;
    else
        return observerData->subject_list.at(i);
}

int Qtilities::Core::Observer::subjectID(int i) const {
    if (i < subjectCount()) {
        QVariant prop = getMultiContextPropertyValue(observerData->subject_list.at(i),qti_prop_OBSERVER_MAP);
        return prop.toInt();
    } else
        return -1;
}

int Qtilities::Core::Observer::subjectID(const QString& subject_name, Qt::CaseSensitivity cs) const {
    if (containsSubjectWithName(subject_name,cs)) {
        QVariant prop = getMultiContextPropertyValue(subjectReference(subject_name,cs),qti_prop_OBSERVER_MAP);
        return prop.toInt();
    } else
        return -1;
}

QList<int> Qtilities::Core::Observer::subjectIDs() const {
    QList<int> subject_ids;
    for (int i = 0; i < observerData->subject_list.count(); i++)
        subject_ids << getMultiContextPropertyValue(observerData->subject_list.at(i),qti_prop_OBSERVER_MAP).toInt();
    return subject_ids;
}

QList<QObject*> Qtilities::Core::Observer::subjectReferences(const QString& iface) const {
    QList<QObject*> subjects;
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        if (iface.isEmpty())
            subjects << observerData->subject_list.at(i);
        else {
            if (observerData->subject_list.at(i)->inherits(iface.toAscii().data()))
                subjects << observerData->subject_list.at(i);
        }
    }
    return subjects;
}

QList<QObject*> Qtilities::Core::Observer::subjectReferencesByCategory(const QtilitiesCategory& category) const {
    // Get all subjects which has the qti_prop_CATEGORY_MAP property set to category.
    QList<QObject*> list;

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QVariant category_variant = getMultiContextPropertyValue(subjectAt(i),qti_prop_CATEGORY_MAP);
        if (category_variant.isValid()) {
            QtilitiesCategory current_category = category_variant.value<QtilitiesCategory>();
            if (current_category == category)
                list << subjectAt(i);
        } else {
            if (!category.isValid())
                list << subjectAt(i);
        }
    }

    return list;
}


QMap<QPointer<QObject>, QString> Qtilities::Core::Observer::subjectMap() {
    QMap<QPointer<QObject>, QString> subject_map;
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QPointer<QObject> object_ptr = observerData->subject_list.at(i);
        subject_map[object_ptr] = subjectNameInContext(observerData->subject_list.at(i));
    }
    return subject_map;
}

QObject* Qtilities::Core::Observer::subjectReference(int ID) const {
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QObject* obj = observerData->subject_list.at(i);
        QVariant prop = getMultiContextPropertyValue(obj,qti_prop_OBSERVER_MAP);
        if (!prop.isValid()) {
            LOG_TRACE(QString("Observer (%1): Looking for subject ID (%2) failed, property 'Subject ID' contains invalid variant for this context.").arg(objectName()).arg(ID));
            return 0;
        }
        if (prop.toInt() == ID)
            return obj;
    }
    return 0;
}

QObject* Qtilities::Core::Observer::subjectReference(const QString& subject_name, Qt::CaseSensitivity cs) const {
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QObject* obj = observerData->subject_list.at(i);
        QVariant prop = getMultiContextPropertyValue(obj,qti_prop_NAME);
        if (!prop.isValid()) {
            if (observerData->subject_list.at(i)->objectName().compare(subject_name,cs) == 0)
                return observerData->subject_list.at(i);
        } else {
            if (prop.toString().compare(subject_name,cs) == 0)
                return obj;
        }
    }
    return 0;
}

bool Qtilities::Core::Observer::contains(const QObject* object) const {
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        if (observerData->subject_list.at(i) == object)
            return true;
    }

    return false;
}

bool Qtilities::Core::Observer::containsSubjectWithName(const QString& subject_name, Qt::CaseSensitivity cs) const {
    if (subjectReference(subject_name,cs))
        return true;
    else
        return false;
}

bool Qtilities::Core::Observer::installSubjectFilter(AbstractSubjectFilter* subject_filter) {
    if (!subject_filter)
        return false;

    if (subjectCount() > 0) {
        LOG_ERROR(QString(tr("Observer (%1): Subject filter installation failed. Can't install subject filters if subjects is already attached to an observer.")).arg(objectName()));
        return false;
    }

    if (hasSubjectFilter(subject_filter->filterName()))
        return false;

    observerData->subject_filters.append(subject_filter);

    // Set the observer context of the filter
    if (!subject_filter->setObserverContext(this)) {
        LOG_ERROR(QString(tr("Observer (%1): Subject filter installation failed. Setting the observer context on the subject filter failed.")).arg(objectName()));
        return false;
    }

    subject_filter->setParent(this);

    // Check if the new subject filter implements the IModificationNotifier interface. If so we connect
    // to the modification changed signal:
    if (subject_filter->isModificationStateMonitored()) {
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (subject_filter);
        if (mod_iface) {
            connect(mod_iface->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
        }
    }

    // We need to connect to the property related signals on this subject filter:
    connect(subject_filter,SIGNAL(monitoredPropertyChanged(const char*,QList<QObject*>)),SIGNAL(monitoredPropertyChanged(const char*,QList<QObject*>)));
    connect(subject_filter,SIGNAL(propertyChangeFiltered(const char*,QList<QObject*>)),SIGNAL(propertyChangeFiltered(const char*,QList<QObject*>)));

    setModificationState(true);
    return true;
}

bool Qtilities::Core::Observer::uninstallSubjectFilter(AbstractSubjectFilter* subject_filter) {
    if (subjectCount() > 0) {
        LOG_ERROR(QString(tr("Observer (%1): Subject filter uninstall failed. Can't uninstall subject filters if subjects is already attached to an observer.")).arg(objectName()));
        return false;
    }

    observerData->subject_filters.removeOne(subject_filter);
    subject_filter->disconnect(this);
    delete subject_filter;
    subject_filter = 0;

    setModificationState(true);
    return true;
}

QList<Qtilities::Core::AbstractSubjectFilter*> Qtilities::Core::Observer::subjectFilters() const {
    return observerData->subject_filters;
}

bool Qtilities::Core::Observer::hasSubjectFilter(const QString& filter_name) const {
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        if (observerData->subject_filters.at(i)->filterName() == filter_name)
            return true;
    }

    return false;
}

Qtilities::Core::ObserverHints* Qtilities::Core::Observer::displayHints() const {
    return observerData->display_hints;
}

bool Qtilities::Core::Observer::copyHints(ObserverHints* display_hints) {
    if (!display_hints)
        return false;

    if (!observerData->display_hints)
        useDisplayHints();

    *observerData->display_hints = *display_hints;
    return true;
}

Qtilities::Core::ObserverHints* Qtilities::Core::Observer::useDisplayHints() {
    if (!observerData->display_hints) {
        observerData->display_hints = new ObserverHints(this);
        observerData->display_hints->setObjectName(objectName());

        // We need to connect to the modification related signals to this observer:
        connect(observerData->display_hints,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
        return observerData->display_hints;
    }
    return 0;
}

bool Qtilities::Core::Observer::eventFilter(QObject *object, QEvent *event) {
//    if (observerName() != "qti.def.ObjectPool")
//        qDebug() << "Observer::eventFilter(): " << observerName() << ", filter subject events enabled: " << observerData->filter_subject_events_enabled;
    if ((event->type() == QEvent::DynamicPropertyChange) && observerData->filter_subject_events_enabled) {
        // Get the event in the correct format
        QDynamicPropertyChangeEvent* propertyChangeEvent = static_cast<QDynamicPropertyChangeEvent *>(event);

        // First check is to see if it is a reserved property. In that case we filter it directly.
        if (contains(object) && reservedProperties().contains(QString(propertyChangeEvent->propertyName().data()))) {
            QList<QObject*> filtered_list;
            filtered_list << object;
            emit propertyChangeFiltered(propertyChangeEvent->propertyName().data(),filtered_list);
            return true;
        }

        // Next check if it is a monitored property.
        if (contains(object) && monitoredProperties().contains(QString(propertyChangeEvent->propertyName().data()))) {
            // Handle changes from different threads:
            if (!observerData->observer_mutex.tryLock()) {
                QList<QObject*> filtered_list;
                filtered_list << object;
                emit propertyChangeFiltered(propertyChangeEvent->propertyName().data(),filtered_list);
                return true;
            }

            // We now route the event that changed to the subject filter responsible for this property to validate the change.
            // If no subject filter is responsible, the observer needs to handle it itself.
            bool filter_event = false;
            for (int i = 0; i < observerData->subject_filters.count(); i++) {
                if (observerData->subject_filters.at(i)) {
                    if (observerData->subject_filters.at(i)->monitoredProperties().contains(QString(propertyChangeEvent->propertyName().data()))) {
                        filter_event = observerData->subject_filters.at(i)->handleMonitoredPropertyChange(object, propertyChangeEvent->propertyName().data(),propertyChangeEvent);
                    }
                }
            }

            // Check if internal properties added by the Observer is being changed:
            // Todo - Check if this is done in above mutex check?

            // If the event should not be filtered, we need to post a user event on the object which will indicate
            // that the property change was valid and succesfull.
            // Note that subject filters must do the following themselves. Although this makes implementation
            // of subject filters more difficult, it is more powerfull in this way since one property change can
            // affect other objects as well and only the subject filter will have knowledge about this.
            if (!filter_event) {
                // We need to do a few things here:
                // 1. If enabled, post the QtilitiesPropertyChangeEvent:
                // First check if this object is in the same thread as this observer:
                if (object->thread() == thread()) {
                    if (observerData->deliver_qtilities_property_changed_events) {
                        QByteArray property_name_byte_array = QByteArray(propertyChangeEvent->propertyName().data());
                        QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observerID());
                        QCoreApplication::postEvent(object,user_event);
                        //qDebug() << QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(object->objectName());
                        LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(object->objectName()));
                    }
                } else {
                    LOG_TRACE(QString("Failed to post QtilitiesPropertyChangeEvent (property: %1) to object (%2). The object is not in the same thread.").arg(QString(propertyChangeEvent->propertyName().data())).arg(object->objectName()));
                }

                // 2. Emit the monitoredPropertyChanged() signal:
                QList<QObject*> changed_objects;
                changed_objects << object;
                emit monitoredPropertyChanged(propertyChangeEvent->propertyName(),changed_objects);

                // 3. For specific role properties, we need to notify views that the data changed:
                if ((!qstrcmp(propertyChangeEvent->propertyName().data(),qti_prop_DECORATION)) ||
                    (!qstrcmp(propertyChangeEvent->propertyName().data(),qti_prop_FOREGROUND)) ||
                    (!qstrcmp(propertyChangeEvent->propertyName().data(),qti_prop_BACKGROUND)) ||
                    (!qstrcmp(propertyChangeEvent->propertyName().data(),qti_prop_TEXT_ALIGNMENT)) ||
                    (!qstrcmp(propertyChangeEvent->propertyName().data(),qti_prop_FONT)) ||
                    (!qstrcmp(propertyChangeEvent->propertyName().data(),qti_prop_SIZE_HINT))) {

                    refreshViewsData();
                }

                // 4. For specific role properties, we need to notify views that layout changed:
                if (!qstrcmp(propertyChangeEvent->propertyName().data(),qti_prop_CATEGORY_MAP)) {
                    // Get the property and check its last changed context:
                    MultiContextProperty prop = ObjectManager::getMultiContextProperty(object,qti_prop_CATEGORY_MAP);
                    if (prop.isValid()) {
                        if (prop.lastChangedContext() == observerID())
                            refreshViewsLayout();
                    }
                }
            }

            // Unlock the mutex and return
            observerData->observer_mutex.unlock();
            return filter_event;
        }
    }
    return false;
}

QString Qtilities::Core::Observer::objectOwnershipToString(ObjectOwnership ownership) {
    if (ownership == ManualOwnership) {
        return "ManualOwnership";
    } else if (ownership == AutoOwnership) {
        return "AutoOwnership";
    } else if (ownership == SpecificObserverOwnership) {
        return "SpecificObserverOwnership";
    } else if (ownership == ObserverScopeOwnership) {
        return "ObserverScopeOwnership";
    } else if (ownership == OwnedBySubjectOwnership) {
        return "OwnedBySubjectOwnership";
    }

    return QString();
}

Qtilities::Core::Observer::ObjectOwnership Qtilities::Core::Observer::stringToObjectOwnership(const QString& ownership_string) {
    if (ownership_string == "ManualOwnership") {
        return ManualOwnership;
    } else if (ownership_string == "AutoOwnership") {
        return AutoOwnership;
    } else if (ownership_string == "SpecificObserverOwnership") {
        return SpecificObserverOwnership;
    } else if (ownership_string == "ObserverScopeOwnership") {
        return ObserverScopeOwnership;
    } else if (ownership_string == "OwnedBySubjectOwnership") {
        return OwnedBySubjectOwnership;
    }

    Q_ASSERT(0);
    return ManualOwnership;
}

QString Qtilities::Core::Observer::accessModeToString(AccessMode access_mode) {
    if (access_mode == FullAccess) {
        return "FullAccess";
    } else if (access_mode == ReadOnlyAccess) {
        return "ReadOnlyAccess";
    } else if (access_mode == LockedAccess) {
        return "LockedAccess";
    } else if (access_mode == InvalidAccess) {
        return "InvalidAccess";
    }

    return QString();
}

Qtilities::Core::Observer::AccessMode Qtilities::Core::Observer::stringToAccessMode(const QString& access_mode_string) {
    if (access_mode_string == "FullAccess") {
        return FullAccess;
    } else if (access_mode_string == "ReadOnlyAccess") {
        return ReadOnlyAccess;
    } else if (access_mode_string == "LockedAccess") {
        return LockedAccess;
    } else if (access_mode_string == "InvalidAccess") {
        return InvalidAccess;
    }

    Q_ASSERT(0);
    return InvalidAccess;
}

QString Qtilities::Core::Observer::objectDeletionPolicyToString(ObjectDeletionPolicy object_deletion_policy) {
    if (object_deletion_policy == DeleteImmediately) {
        return "DeleteImmediately";
    } else if (object_deletion_policy == DeleteLater) {
        return "DeleteLater";
    }

    return QString();
}

Qtilities::Core::Observer::ObjectDeletionPolicy Qtilities::Core::Observer::stringToObjectDeletionPolicy(const QString& object_deletion_policy_string) {
    if (object_deletion_policy_string == "DeleteImmediately") {
        return DeleteImmediately;
    } else if (object_deletion_policy_string == "DeleteLater") {
        return DeleteLater;
    }
    Q_ASSERT(0);
    return DeleteLater;
}

QString Qtilities::Core::Observer::accessModeScopeToString(AccessModeScope access_mode_scope) {
    if (access_mode_scope == GlobalScope) {
        return "GlobalScope";
    } else if (access_mode_scope == CategorizedScope) {
        return "CategorizedScope";
    }

    return QString();
}

Qtilities::Core::Observer::AccessModeScope Qtilities::Core::Observer::stringToAccessModeScope(const QString& access_mode_scope_string) {
    if (access_mode_scope_string == "GlobalScope") {
        return GlobalScope;
    } else if (access_mode_scope_string == "CategorizedScope") {
        return CategorizedScope;
    }

    Q_ASSERT(0);
    return GlobalScope;
}

// ---------------------------------------
// Static Functions
// ---------------------------------------
QList<Qtilities::Core::Observer*> Qtilities::Core::Observer::observerList(QList<QPointer<QObject> >& object_list) {
    QList<Observer*> observer_list;
    for (int i = 0; i < object_list.count(); i++) {
        Observer* obs = qobject_cast<Observer*> (object_list.at(i));
        if (!obs) {
            // Check children of the object.
            foreach (QObject* child, object_list.at(i)->children()) {
                obs = qobject_cast<Observer*> (child);
                if (obs)
                    break;
            }
        }
        if (obs)
            observer_list << obs;
    }
    return observer_list;
}

int Qtilities::Core::Observer::parentCount(const QObject* obj) {
    if (!obj)
        return -1;

    MultiContextProperty prop = ObjectManager::getMultiContextProperty(obj, Qtilities::Core::Properties::qti_prop_OBSERVER_MAP);
    if (prop.isValid()) {
        return prop.contextMap().count();
    }

    return 0;
}

QList<Qtilities::Core::Observer*> Qtilities::Core::Observer::parentReferences(const QObject* obj) {
    QList<Observer*> parents;
    if (!obj)
        return parents;

    MultiContextProperty prop = ObjectManager::getMultiContextProperty(obj, Qtilities::Core::Properties::qti_prop_OBSERVER_MAP);
    if (prop.isValid()) {
        for (int i = 0; i < prop.contextMap().count(); i++) {
            Observer* obs = OBJECT_MANAGER->observerReference(prop.contextMap().keys().at(i));
            if (obs)
                parents << obs;
        }
    }

    return parents;
}

bool Qtilities::Core::Observer::isSupportedType(const QString& meta_type, Observer* observer) {
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

void Qtilities::Core::Observer::deleteObject(QObject* object) {
    if (!object)
        return;

    object->disconnect(this);
    object->removeEventFilter(this);

    if ((ObjectDeletionPolicy) observerData->object_deletion_policy == DeleteLater) {
        observerData->subject_list.removeOne(object);
        object->deleteLater();
    } else if ((ObjectDeletionPolicy) observerData->object_deletion_policy == DeleteImmediately) {
        // The destroyed() signal on the object will cause it to be removed from the subject_list immediately.
        delete object;
    }
}
