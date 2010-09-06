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

#include "Observer.h"
#include "QtilitiesCoreConstants.h"
#include "ObserverProperty.h"
#include "ActivityPolicyFilter.h"
#include "QtilitiesPropertyChangeEvent.h"
#include "ObserverMimeData.h"
#include "ObserverHints.h"

#include <Logger.h>

#include <QMap>
#include <QVariant>
#include <QMetaType>
#include <QEvent>
#include <QDynamicPropertyChangeEvent>
#include <QCoreApplication>
#include <QMutableListIterator>

using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core::Properties;

Qtilities::Core::Observer::Observer(const QString& observer_name, const QString& observer_description, QObject* parent) : QObject(parent) {
    // Initialize observer data
    observerData = new ObserverData();
    setObjectName(observer_name);
    observerData->observer_description = observer_description;
    observerData->access_mode_scope = GlobalScope;
    observerData->access_mode = FullAccess;
    observerData->filter_subject_events_enabled = true;
    observerData->subject_limit = -1;
    observerData->subject_id_counter = 0;
    observerData->subject_list.setObjectName(QString(tr("%1 Pointer List")).arg(observer_name));
    connect(&observerData->subject_list,SIGNAL(objectDestroyed(QObject*)),SLOT(handle_deletedSubject(QObject*)));

    // Register this observer with the observer manager
    if (observer_name != QString(GLOBAL_OBJECT_POOL))
        observerData->observer_id = OBJECT_MANAGER->registerObserver(this);
    else
        observerData->observer_id = 0;

    // To catch name changes.
    installEventFilter(this);
}

Qtilities::Core::Observer::Observer(const Observer &other) : QObject(), observerData(other.observerData) {
    connect(&observerData->subject_list,SIGNAL(objectDestroyed(QObject*)),SLOT(handle_deletedSubject(QObject*)));
}

Qtilities::Core::Observer::~Observer() {
    startProcessingCycle();

    if (objectName() != QString(GLOBAL_OBJECT_POOL)) {
        //startProcessingCycle();
        observerData->deliver_qtilities_property_changed_events = false;

        // When this observer is deleted, we must check the ownership of all its subjects
        QVariant subject_ownership_variant;
        QVariant parent_observer_variant;

        LOG_TRACE(QString("Starting destruction of observer \"%1\":").arg(objectName()));
        LOG_TRACE("Deleting neccessary children.");

        QMutableListIterator<QObject*> i(observerData->subject_list);
        while (i.hasNext()) {
            QObject* obj = i.next();
            // If it is an observer we start a processing cycle on it:
            Observer* obs = qobject_cast<Observer*> (obj);
            if (obs)
                obs->startProcessingCycle();

            subject_ownership_variant = getObserverPropertyValue(obj,OWNERSHIP);
            parent_observer_variant = getObserverPropertyValue(obj,OBSERVER_PARENT);
            if ((subject_ownership_variant.toInt() == SpecificObserverOwnership) && (observerData->observer_id == parent_observer_variant.toInt())) {
                // Subjects with SpecificObserverOwnership must be deleted as soon as this observer is deleted if this observer is their parent.
               LOG_TRACE(QString("Object \"%1\" (aliased as %2 in this context) is owned by this observer, it will be deleted.").arg(obj->objectName()).arg(subjectNameInContext(obj)));
               if (!i.hasNext()) {
                   delete obj;
                   break;
               } else {
                   delete obj;
               }
            } else if ((subject_ownership_variant.toInt() == ObserverScopeOwnership) && (parentCount(obj) == 1)) {
               LOG_TRACE(QString("Object \"%1\" (aliased as %2 in this context) with ObserverScopeOwnership went out of scope, it will be deleted.").arg(obj->objectName()).arg(subjectNameInContext(obj)));
               if (!i.hasNext()) {
                   delete obj;
                   break;
               }
               else
                   delete obj;
           } else if ((subject_ownership_variant.toInt() == OwnedBySubjectOwnership) && (parentCount(obj) == 1)) {
              LOG_TRACE(QString("Object \"%1\" (aliased as %2 in this context) with OwnedBySubjectOwnership went out of scope, it will be deleted.").arg(obj->objectName()).arg(subjectNameInContext(obj)));
              if (!i.hasNext()) {
                  delete obj;
                  break;
              } else {
                  delete obj;
              }
           }
        }
        //endProcessingCycle();
    }

    // Delete subject filters
    LOG_TRACE("Deleting subject filters.");
    observerData->subject_filters.deleteAll();

    if (objectName() != QString(GLOBAL_OBJECT_POOL)) {
        LOG_TRACE("Removing any trace of this observer from remaining children.");
        int count = subjectCount();
        for (int i = 0; i < count; i++) {
            // In this case we need to remove any trace of this observer from the obj
            removeObserverProperties(observerData->subject_list.at(i));
        }
    }

    LOG_DEBUG(QString("Done with destruction of observer \"%1\".").arg(objectName()));
}

QStringList Qtilities::Core::Observer::monitoredProperties() const {
    QStringList properties;
    properties.append(QString(OBJECT_CATEGORY));
    properties.append(QString(OBJECT_ACCESS_MODE));
    // Role properties are also monitored. We will notify views to refresh when they change.
    properties.append(QString(OBJECT_ROLE_TOOLTIP));
    properties.append(QString(OBJECT_ROLE_DECORATION));
    properties.append(QString(OBJECT_ROLE_FOREGROUND));
    properties.append(QString(OBJECT_ROLE_BACKGROUND));
    properties.append(QString(OBJECT_ROLE_TEXT_ALIGNMENT));
    properties.append(QString(OBJECT_ROLE_FONT));
    properties.append(QString(OBJECT_ROLE_SIZE_HINT));
    properties.append(QString(OBJECT_ROLE_STATUSTIP));
    properties.append(QString(OBJECT_ROLE_WHATS_THIS));

    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        properties << observerData->subject_filters.at(i)->monitoredProperties();
    }
    return properties;
}

QStringList Qtilities::Core::Observer::reservedProperties() const {
    QStringList properties;
    properties << QString(OBSERVER_SUBJECT_IDS) << QString(OWNERSHIP) << QString(OBSERVER_PARENT) << QString(OBSERVER_VISITOR_ID) << QString(OBJECT_LIMITED_EXPORTS);

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

Qtilities::Core::Interfaces::IFactoryData Qtilities::Core::Observer::factoryData() const {
    IFactoryData factory_data = observerData->factory_data;
    factory_data.d_instance_name = observerName();
    return factory_data;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::Observer::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    return flags;
}

quint32 MARKER_OBSERVER_SECTION = 0xDEADBEEF;

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    Q_UNUSED(params)

    LOG_TRACE(tr("Binary export of observer ") + observerName() + tr(" section started."));

    // We define a succesfull operation as an export which is able to export all subjects.
    bool success = true;
    bool complete = true;

    // First export the factory data of this observer:
    IFactoryData factory_data = factoryData();
    factory_data.exportBinary(stream);

    // Stream the observerData class, this DOES NOT include the subjects itself, only the subject count.
    // It also excludes the factory data which was stream above.
    // This is neccessary because we want to keep track of the return values for subject IExportable interfaces.
    stream << MARKER_OBSERVER_SECTION;
    observerData->exportBinary(stream);
    stream << MARKER_OBSERVER_SECTION;

    // Stream details about the subject filters in to be added to the observer.
    // The number of exportable subject filters in this context:
    int exportable_filters_count = 0;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        if (observerData->subject_filters.at(i)->isExportable())
            ++exportable_filters_count;
    }

    stream << (quint32) exportable_filters_count;
    // Stream all subject filters:
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        if (observerData->subject_filters.at(i)->isExportable()) {
            observerData->subject_filters.at(i)->exportBinary(stream);
            LOG_TRACE(QString("%1/%2: Exporting subject filter \"%3\"...").arg(i+1).arg(observerData->subject_filters.count()).arg(observerData->subject_filters.at(i)->filterName()));
        }
    }
    stream << MARKER_OBSERVER_SECTION;

    // Count the number of IExportable subjects first and check if objects must be excluded.
    QList<IExportable*> exportable_list;
    qint32 iface_count = 0;
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QObject* obj = observerData->subject_list.at(i);
        IExportable* iface = qobject_cast<IExportable*> (obj);
        if (!iface) {
            foreach (QObject* child, obj->children()) {
                Observer* obs = qobject_cast<Observer*> (child);
                if (obs) {
                    iface = obs;
                    break;
                }
            }
        }

        if (iface) {
            if (iface->supportedFormats() & IExportable::Binary) {
                // Handle limited export object, thus they should only be exported once.
                bool ok = false;
                int export_count = getObserverPropertyValue(obj,OBJECT_LIMITED_EXPORTS).toInt(&ok);
                if (export_count == 0) {
                    if (ok)
                        setObserverPropertyValue(obj,OBJECT_LIMITED_EXPORTS,export_count+1);

                    exportable_list << iface;
                    ++iface_count;
                } else {
                    LOG_TRACE(QString("%1/%2: Limited export object \"%3\" excluded in this context...").arg(i).arg(iface_count).arg(subjectNameInContext(obj)));
                }
            } else {
                LOG_WARNING(tr("Binary export found an interface (") + subjectNameInContext(obj) + tr(" in context ") + observerName() + tr(") in context which does not support binary exporting. Binary export will be incomplete."));
                complete = false;
            }
        } else {
            LOG_WARNING(tr("Binary export found an object (") + subjectNameInContext(obj) + tr(" in context ") + observerName() + tr(") which does not implement an exportable interface. Binary export will be incomplete."));
            complete = false;
        }
    }

    // Write this count to the stream:
    stream << iface_count;
    LOG_TRACE(QString(tr("%1 exportable subjects found under this observer's level of hierarchy.")).arg(iface_count));

    // Now check all subjects for the IExportable interface.
    for (int i = 0; i < exportable_list.count(); i++) {
        QCoreApplication::processEvents();
        IExportable* iface = exportable_list.at(i);
        QObject* obj = iface->objectBase();
        LOG_TRACE(QString("%1/%2: Exporting \"%3\"...").arg(i).arg(iface_count).arg(subjectNameInContext(obj)));
        IExportable::Result result = iface->exportBinary(stream);

        // Now export the needed properties about this subject
        if (result == IExportable::Complete) {
            OBJECT_MANAGER->exportObjectProperties(obj,stream);
        } else if (result == IExportable::Incomplete) {
            OBJECT_MANAGER->exportObjectProperties(obj,stream);
            complete = false;
        } else if (result == IExportable::Failed)
            success = false;
    }
    stream << MARKER_OBSERVER_SECTION;
    if (success) {
        if (complete) {
            LOG_DEBUG(tr("Binary export of observer ") + observerName() + QString(tr(" was successfull (complete).")));
            return IExportable::Complete;
        } else {
            LOG_DEBUG(tr("Binary export of observer ") + observerName() + QString(tr(" was successfull (incomplete).")));
            return IExportable::Incomplete;
        }
    } else {
        LOG_WARNING(tr("Binary export of observer ") + observerName() + tr(" failed."));
        return IExportable::Failed;
    }
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Observer::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(params)

    LOG_TRACE(tr("Binary import of observer ") + observerName() + tr(" section started."));
    startProcessingCycle();

    // We define a succesfull operation as an import which is able to import all subjects.
    bool success = true;
    bool complete = true;

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_SECTION) {
        LOG_ERROR("Observer binary import failed to detect marker located after factory data. Import will fail.");
        endProcessingCycle();
        return IExportable::Failed;
    }
    // Stream the observerData class, this DOES NOT include the subjects itself, only the subject count.
    // This is neccessary because we want to keep track of the return values for subject IExportable interfaces.
    if (!observerData->importBinary(stream)) {
        LOG_ERROR("Observer binary import failed to during observer data import. Import will fail.");
        endProcessingCycle();
        return IExportable::Failed;
    }
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_SECTION) {
        LOG_ERROR("Observer binary import failed to detect marker located after observer data. Import will fail.");
        endProcessingCycle();
        return IExportable::Failed;
    }

    // Stream details about the subject filters to be added to the observer.
    // ToDo: in the future when the user has the ability to add custom subject filters, or
    // change subject filter details.
    stream >> ui32;
    int subject_filter_count = ui32;
    for (int i = 0; i < subject_filter_count; i++) {
        // Get the factory data of the subject filter:
        IFactoryData factoryData;
        if (!factoryData.importBinary(stream)) {
            endProcessingCycle();
            return IExportable::Failed;
        } else {
            AbstractSubjectFilter* new_filter = OBJECT_MANAGER->createSubjectFilter(factoryData.d_instance_tag);
            if (new_filter) {
                new_filter->setObjectName(factoryData.d_instance_name);
                LOG_TRACE(QString("%1/%2: Importing subject filter \"%3\"...").arg(i+1).arg(subject_filter_count).arg(factoryData.d_instance_name));
                new_filter->importBinary(stream,import_list);
                installSubjectFilter(new_filter);
            } else {
                LOG_ERROR(QString("%1/%2: Importing subject filter \"%3\" failed. Import cannot continue...").arg(i+1).arg(subject_filter_count).arg(factoryData.d_instance_name));
                endProcessingCycle();
                return IExportable::Failed;
            }
        }
    }

    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_SECTION) {
        LOG_ERROR("Observer binary import failed to detect marker located after subject filters. Import will fail.");
        endProcessingCycle();
        return IExportable::Failed;
    }


    // Count the number of IExportable subjects first.
    qint32 iface_count = 0;
    stream >> iface_count;
    LOG_TRACE(QString(tr("%1 exportable subject(s) found under this observer's level of hierarchy.")).arg(iface_count));

    // Now check all subjects for the IExportable interface.
    for (int i = 0; i < iface_count; i++) {
        QCoreApplication::processEvents();
        if (!success)
            break;

        IFactoryData factoryData;
        if (!factoryData.importBinary(stream)) {
            endProcessingCycle();
            return IExportable::Failed;
        }
        if (!factoryData.isValid()) {
            LOG_TRACE(QString(tr("%1/%2: Importing a standard observer...")).arg(i+1).arg(iface_count));
            Observer* new_obs = new Observer(factoryData.d_instance_name,QString());
            new_obs->setObjectName(factoryData.d_instance_name);
            success = attachSubject(new_obs,Observer::ManualOwnership,true);
            import_list.append(new_obs);
            IExportable::Result result = new_obs->importBinary(stream, import_list);
            if (result == IExportable::Complete) {
                OBJECT_MANAGER->importObjectProperties(new_obs,stream);
            } else if (result == IExportable::Incomplete) {
                OBJECT_MANAGER->importObjectProperties(new_obs,stream);
                complete = false;
            } else if (result == IExportable::Failed) {
                success = false;
            }
        } else {
            LOG_TRACE(QString(tr("%1/%2: Importing subject type \"%3\" in factory \"%4\"...")).arg(i+1).arg(iface_count).arg(factoryData.d_instance_tag).arg(factoryData.d_factory_tag));

            IFactory* ifactory = OBJECT_MANAGER->factoryReference(factoryData.d_factory_tag);
            if (ifactory) {
                factoryData.d_instance_context = observerData->observer_id;
                QObject* new_instance = ifactory->createInstance(factoryData);
                new_instance->setObjectName(factoryData.d_instance_name);
                if (new_instance) {
                    import_list.append(new_instance);
                    IExportable* exp_iface = qobject_cast<IExportable*> (new_instance);
                    if (exp_iface) {
                        IExportable::Result result = exp_iface->importBinary(stream, import_list);
                        if (result == IExportable::Complete) {
                            OBJECT_MANAGER->importObjectProperties(new_instance,stream);
                            success = attachSubject(new_instance,Observer::ObserverScopeOwnership,true);
                        } else if (result == IExportable::Incomplete) {
                            OBJECT_MANAGER->importObjectProperties(new_instance,stream);
                            success = attachSubject(new_instance,Observer::ObserverScopeOwnership,true);
                            complete = false;
                        } else if (result == IExportable::Failed) {
                            success = false;
                        }
                    } else {
                        // Handle deletion of import_list;
                        success = false;
                        break;
                    }
                } else {
                    // Handle deletion of import_list;
                    success = false;
                    break;
                }
            } else {
                endProcessingCycle();
                return IExportable::Failed;
            }
        }
    }
    stream >> ui32;
    if (ui32 != MARKER_OBSERVER_SECTION) {
        LOG_ERROR("Observer binary import failed to detect end marker. Import will fail.");
        endProcessingCycle();
        return IExportable::Failed;
    }

    if (success) {
        LOG_DEBUG(tr("Binary import of observer ") + observerName() + tr(" section was successfull."));
        endProcessingCycle();
        return IExportable::Complete;
    } else {
        LOG_WARNING(tr("Binary import of observer ") + observerName() + tr(" section failed."));
        endProcessingCycle();
        return IExportable::Failed;
    }
}

bool Qtilities::Core::Observer::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(params)

    return false;
}

bool Qtilities::Core::Observer::importXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(params)

    return false;
}

bool Qtilities::Core::Observer::isModified() const {
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
        IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (observerData->subject_filters.at(i));
        if (mod_iface) {
            if (mod_iface->isModified())
                return true;
        }
    }
    // Check if the observer hints were modified.
    if (observerData->display_hints) {
        if (observerData->display_hints->isModified())
            return true;
    }
    return false;
}

void Qtilities::Core::Observer::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets) {
    observerData->is_modified = new_state;
    if ((notification_targets & IModificationNotifier::NotifyListeners) && !observerData->process_cycle_active) {
        emit modificationStateChanged(new_state);
    }
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
                mod_iface->setModificationState(new_state,notification_targets);
            }
        }
        // Also notify observer hints.
        if (observerData->display_hints) {
            observerData->display_hints->setModificationState(new_state,notification_targets);
        }
    }
}

void Qtilities::Core::Observer::refreshViewsLayout() {
    emit layoutChanged();
}

void Qtilities::Core::Observer::refreshViewsData() {
    emit dataChanged(this);
}

void Qtilities::Core::Observer::startProcessingCycle() {
    if (observerData->process_cycle_active)
        return;

    observerData->process_cycle_active = true;
}

void Qtilities::Core::Observer::endProcessingCycle() {
    if (!observerData->process_cycle_active)
        return;

    observerData->process_cycle_active = false;
}

bool Qtilities::Core::Observer::isProcessingCycleActive() const {
    return observerData->process_cycle_active;
}

void Qtilities::Core::Observer::setFactoryData(IFactoryData factory_data) {
    if (factory_data.isValid())
        observerData->factory_data = factory_data;
}

bool Qtilities::Core::Observer::attachSubject(QObject* obj, Observer::ObjectOwnership object_ownership, bool import_cycle) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif
    
    if (canAttach(obj,object_ownership) == Rejected)
        return false;

    // Pass new object through all installed subject filters
    bool passed_filters = true;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        bool result = observerData->subject_filters.at(i)->initializeAttachment(obj,import_cycle);
        if (passed_filters)
            passed_filters = result;
    }

    if (!passed_filters) {
        LOG_DEBUG(QString("Observer (%1): Object (%2) attachment failed, attachment was rejected by one or more subject filter.").arg(objectName()).arg(obj->objectName()));
        for (int i = 0; i < observerData->subject_filters.count(); i++) {
            observerData->subject_filters.at(i)->finalizeAttachment(obj,false,import_cycle);
        }
        removeObserverProperties(obj);
        return false;
    }

    // Details of the global object pool observer is not added to any objects.
    if (objectName() != QString(GLOBAL_OBJECT_POOL)) {
        // Now, add observer details to needed properties
        // Add observer details to property: OBSERVER_SUBJECT_IDS
        ObserverProperty subject_id_property = getObserverProperty(obj,OBSERVER_SUBJECT_IDS);
        if (subject_id_property.isValid()) {
            // Thus, the property already exists
            subject_id_property.addContext(QVariant(observerData->subject_id_counter),observerData->observer_id);
            setObserverProperty(obj,subject_id_property);
        } else {
            // We need to create the property and add it to the object
            ObserverProperty new_subject_id_property(OBSERVER_SUBJECT_IDS);
            new_subject_id_property.setIsExportable(false);
            new_subject_id_property.addContext(QVariant(observerData->subject_id_counter),observerData->observer_id);
            setObserverProperty(obj,new_subject_id_property);
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
            QVariant current_ownership = getObserverPropertyValue(obj,OWNERSHIP);
            if (current_ownership.toInt() != OwnedBySubjectOwnership) {
                if (object_ownership == ObserverScopeOwnership) {
                    // Update the ownership to ObserverScopeOwnership
                    setObserverPropertyValue(obj,OWNERSHIP,QVariant(ObserverScopeOwnership));
                    setObserverPropertyValue(obj,OBSERVER_PARENT,QVariant(-1));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Observer Scope Ownership";
                    #endif
                } else if (object_ownership == SpecificObserverOwnership) {
                    // Update the ownership to SpecificObserverOwnership
                    setObserverPropertyValue(obj,OWNERSHIP,QVariant(SpecificObserverOwnership));
                    setObserverPropertyValue(obj,OBSERVER_PARENT,QVariant(observerID()));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Specific Observer Ownership";
                    #endif
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
                            setObserverPropertyValue(obj,OWNERSHIP,QVariant(ObserverScopeOwnership));
                            #ifndef QT_NO_DEBUG
                                management_policy_string = "Auto Ownership (had no parent, using Observer Scope Ownership)";
                            #endif
                        } else {
                            setObserverPropertyValue(obj,OWNERSHIP,QVariant(ManualOwnership));
                            #ifndef QT_NO_DEBUG
                                management_policy_string = "Auto Ownership (had parent, leave as Manual Ownership)";
                            #endif
                        }
                        setObserverPropertyValue(obj,OBSERVER_PARENT,QVariant(-1));
                    }
                } else if (object_ownership == OwnedBySubjectOwnership) {
                    connect(obj,SIGNAL(destroyed()),SLOT(deleteLater()));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Owned By Subject Ownership (this context is now dependant on this subject, but the original ownership of the subject was not changed)";
                    #endif
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
            SharedObserverProperty ownership_property(QVariant(object_ownership),OWNERSHIP);
            ownership_property.setIsExportable(false);
            setSharedProperty(obj,ownership_property);
            SharedObserverProperty observer_parent_property(QVariant(-1),OBSERVER_PARENT);
            observer_parent_property.setIsExportable(false);
            setSharedProperty(obj,observer_parent_property);
            if (object_ownership == ManualOwnership) {
                // We don't care about this object's lifetime, its up to the user to manage the lifetime of this object.
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Manual Ownership";
                #endif
            } else if (object_ownership == AutoOwnership) {
                // Check if the object already has a parent, otherwise we handle it as ObserverScopeOwnership.
                if (!obj->parent()) {
                    setObserverPropertyValue(obj,OWNERSHIP,QVariant(ObserverScopeOwnership));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Auto Ownership (had no parent, using Observer Scope Ownership)";
                    #endif
                } else {
                    setObserverPropertyValue(obj,OWNERSHIP,QVariant(ManualOwnership));
                    #ifndef QT_NO_DEBUG
                        management_policy_string = "Auto Ownership (had parent, leave as Manual Ownership)";
                    #endif
                }
            } else if (object_ownership == SpecificObserverOwnership) {
                // This observer must be its parent.
                setObserverPropertyValue(obj,OWNERSHIP,QVariant(SpecificObserverOwnership));
                setObserverPropertyValue(obj,OBSERVER_PARENT,QVariant(observerID()));
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Specific Observer Ownership";
                #endif
            } else if (object_ownership == ObserverScopeOwnership) {
                // This object must be deleted as soon as its not observed by any observers any more.
                //obj->setParent(0);
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Observer Scope Ownership";
                #endif
            } else if (object_ownership == OwnedBySubjectOwnership) {
                // This observer must be deleted as soon as this subject is deleted.
                connect(obj,SIGNAL(destroyed()),SLOT(deleteLater()));
                #ifndef QT_NO_DEBUG
                    management_policy_string = "Owned By Subject Ownership";
                #endif
            }
        }

        // Install the observer as an eventFilter on the subject.
        // We do this last, otherwise all dynamic property changes will go through this event filter.
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
            connect(obs,SIGNAL(layoutChanged()),SIGNAL(layoutChanged()));
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
        QList<QObject*> objects;
        objects << obj;
        if (!observerData->process_cycle_active) {
            emit numberOfSubjectsChanged(Observer::SubjectAdded, objects);
            emit layoutChanged();
            setModificationState(true);
        }

        #ifndef QT_NO_DEBUG
            if (has_mod_iface)
                LOG_DEBUG(QString("Observer (%1): Now observing object \"%2\" with management policy: %3. This object's modification state is now monitored by this observer.").arg(objectName()).arg(obj->objectName()).arg(management_policy_string));
            else
                LOG_DEBUG(QString("Observer (%1): Now observing object \"%2\" with management policy: %3.").arg(objectName()).arg(obj->objectName()).arg(management_policy_string));
        #endif

        // Register object in global object pool
        OBJECT_MANAGER->registerObject(obj);
    } else {
        // If it is the global object manager it will get here.
        observerData->subject_list.append(obj);

        // Install the observer as an eventFilter on the subject.
        // We do this last, otherwise all dynamic property changes will go through this event filter.
        obj->installEventFilter(this);

        // Emit neccesarry signals
        QList<QObject*> objects;
        objects << obj;
        if (!observerData->process_cycle_active) {
            emit numberOfSubjectsChanged(Observer::SubjectAdded, objects);
            emit layoutChanged();
            setModificationState(true);
        }

        LOG_TRACE(QString("Object \"%1\" is now visible in the global object pool.").arg(obj->objectName()));
    }

    // Finalize the attachment in all subject filters, indicating that the attachment was succesfull.
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        observerData->subject_filters.at(i)->finalizeAttachment(obj,true,import_cycle);
    }

    return true;
}

QList<QObject*> Qtilities::Core::Observer::attachSubjects(QList<QObject*> objects, Observer::ObjectOwnership ownership, bool import_cycle) {
    QList<QObject*> success_list;
    startProcessingCycle();
    for (int i = 0; i < objects.count(); i++) {
        if (attachSubject(objects.at(i), ownership, import_cycle))
            success_list << objects.at(i);
    }
    endProcessingCycle();
    if (success_list.count() > 0) {
        emit numberOfSubjectsChanged(SubjectAdded, success_list);
        emit layoutChanged();
        setModificationState(true);
    }
    return success_list;
}

QList<QObject*> Qtilities::Core::Observer::attachSubjects(ObserverMimeData* mime_data_object, Observer::ObjectOwnership ownership, bool import_cycle) {
    QList<QObject*> success_list;
    startProcessingCycle();
    for (int i = 0; i < mime_data_object->subjectList().count(); i++) {
        if (attachSubject(mime_data_object->subjectList().at(i), ownership, import_cycle))
            success_list << mime_data_object->subjectList().at(i);
    }
    endProcessingCycle();
    if (success_list.count() > 0) {
        emit numberOfSubjectsChanged(SubjectAdded, success_list);
        emit layoutChanged();
        setModificationState(true);
    }
    return success_list;
}

Qtilities::Core::Observer::EvaluationResult Qtilities::Core::Observer::canAttach(QObject* obj, Observer::ObjectOwnership) const {
    if (!obj)
        return Observer::Rejected;

    QVariant category_variant = this->getObserverPropertyValue(obj,OBJECT_CATEGORY);
    QString category_string = category_variant.toString();
    if (isConst(category_string)) {
        LOG_WARNING(QString(tr("Attaching object \"%1\" to observer \"%2\" is not allowed. This observer is const for the recieved object.")).arg(obj->objectName()).arg(objectName()));
        return Observer::Rejected;
    }

    // Check for circular dependancies
    const Observer* observer_cast = qobject_cast<Observer*> (obj);
    if (observer_cast) {
        if (isParentInHierarchy(observer_cast,this)) {
            LOG_WARNING(QString(tr("Attaching observer \"%1\" to observer \"%2\" will result in a circular dependancy.")).arg(obj->objectName()).arg(objectName()));
            return Observer::Rejected;
        }
    }

    // First evaluate new subject from Observer side
    if (observerData->subject_limit == observerData->subject_list.count()) {
        LOG_WARNING(QString(tr("Observer (%1): Object (%2) attachment failed, subject limit reached.")).arg(objectName()).arg(obj->objectName()));
        return Observer::Rejected;
    }


    if (objectName() != QString(GLOBAL_OBJECT_POOL)) {
        // Check if this subject is already monitored by this observer, if so abort.
        // This will ensure that no subject filters need to check for this, thus subject filters can assume that new attachments are actually new.
        ObserverProperty observer_list = getObserverProperty(obj,OBSERVER_SUBJECT_IDS);
        if (observer_list.isValid()) {
            if (observer_list.hasContext(observerData->observer_id)) {
                LOG_WARNING(QString(tr("Observer (%1): Object (%2) attachment failed, object is already observed by this observer.")).arg(objectName()).arg(obj->objectName()));
                return Observer::Rejected;
            }
        }

        // Evaluate dynamic properties on the object
        bool has_limit = false;
        int observer_limit;
        int observer_count = parentCount(obj);

        QVariant observer_limit_variant = getObserverPropertyValue(obj,OBSERVER_LIMIT);
        if (observer_limit_variant.isValid()) {
            observer_limit = observer_limit_variant.toInt(&has_limit);
        }

        if (has_limit) {
            if (observer_count == -1) {
                observer_count = 0;
                // No count yet, check if the limit is > 0
                if ((observer_limit < 1) && (observer_limit != -1)){
                    LOG_WARNING(QString(tr("Observer (%1): Object (%2) attachment failed, observer limit (%3) reached.")).arg(objectName()).arg(obj->objectName()).arg(observer_limit));
                    return Observer::Rejected;
                }
            } else {
                if (observer_count >= observer_limit) {
                    LOG_WARNING(QString(tr("Observer (%1): Object (%2) attachment failed, observer limit (%3) reached.")).arg(objectName()).arg(obj->objectName()).arg(observer_limit));
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
        current_filter_evaluation = observerData->subject_filters.at(i)->evaluateAttachment(obj);
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

Qtilities::Core::Observer::EvaluationResult Qtilities::Core::Observer::canAttach(ObserverMimeData* mime_data_object) const {
    if (!mime_data_object)
        return Observer::Rejected;

    bool success = true;
    int not_allowed_count = 0;
    for (int i = 0; i < mime_data_object->subjectList().count(); i++) {
        if (canAttach(mime_data_object->subjectList().at(i)) == Observer::Rejected) {
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
        passed_filters = observerData->subject_filters.at(i)->initializeDetachment(obj,true);
    }

    if (!passed_filters) {
        LOG_ERROR(QString(tr("Observer (%1): Warning: Subject filter rejected detachment of deleted object (%2).")).arg(objectName()).arg(obj->objectName()));
    }

    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        observerData->subject_filters.at(i)->finalizeDetachment(obj,passed_filters,true);
    }

    LOG_DEBUG(QString("Observer (%1) detected deletion of object (%2), updated observer context accordingly.").arg(objectName()).arg(obj->objectName()));

    // Emit neccesarry signals
    QList<QObject*> objects;
    objects << obj;
    if (!observerData->process_cycle_active) {
        emit numberOfSubjectsChanged(SubjectRemoved, objects);
        emit layoutChanged();
        setModificationState(true);
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

    observerData->filter_subject_events_enabled = false;

    // Pass object through all installed subject filters
    bool passed_filters = true;
    for (int i = 0; i < observerData->subject_filters.count(); i++) {
        passed_filters = observerData->subject_filters.at(i)->initializeDetachment(obj);
        if (!passed_filters)
            break;
    }

    if (!passed_filters) {
        LOG_WARNING(QString(tr("Observer (%1): Object (%2) detachment failed, detachment was rejected by one or more subject filter.")).arg(objectName()).arg(obj->objectName()));
        for (int i = 0; i < observerData->subject_filters.count(); i++) {
            observerData->subject_filters.at(i)->finalizeDetachment(obj,false);
        }
        return false;
    } else {
        for (int i = 0; i < observerData->subject_filters.count(); i++) {
            observerData->subject_filters.at(i)->finalizeDetachment(obj,true);
        }
    }

    if (objectName() != QString(GLOBAL_OBJECT_POOL)) {
        #ifndef QT_NO_DEBUG
            QString debug_name = obj->objectName();
        #endif

        // Check the ownership property of this object
        QVariant ownership_variant = getObserverPropertyValue(obj,OWNERSHIP);
        if (ownership_variant.isValid() && (ownership_variant.toInt() == ObserverScopeOwnership)) {
            if ((parentCount(obj) == 1) && obj) {
                LOG_DEBUG(QString("Object (%1) went out of scope, it will be deleted.").arg(obj->objectName()));
                delete obj;
                obj = 0;
            } else {
                removeObserverProperties(obj);
                observerData->subject_list.removeOne(obj);
            }
        } else if (ownership_variant.isValid() && (ownership_variant.toInt() == SpecificObserverOwnership)) {
            QVariant observer_parent = getObserverPropertyValue(obj,OBSERVER_PARENT);
            if (observer_parent.isValid() && (observer_parent.toInt() == observerID()) && obj) {
                delete obj;
                obj = 0;
            } else {
                removeObserverProperties(obj);
                observerData->subject_list.removeOne(obj);
            }
        } else {
            removeObserverProperties(obj);
            observerData->subject_list.removeOne(obj);
        }

        #ifndef QT_NO_DEBUG
             LOG_DEBUG(QString("Observer (%1): Not observing object (%2) anymore.").arg(objectName()).arg(debug_name));
        #endif
    }

    // Disconnect all signals in this object:
    if (obj)
        obj->disconnect(this);

    // Broadcast if neccesarry
    if (!observerData->process_cycle_active) {
        QList<QObject*> objects;
        objects << obj;
        emit numberOfSubjectsChanged(SubjectRemoved, objects);
        emit layoutChanged();
        setModificationState(true);
    }

    observerData->filter_subject_events_enabled = true;
    return true;
}

QList<QObject*> Qtilities::Core::Observer::detachSubjects(QList<QObject*> objects) {
    QList<QObject*> success_list;
    startProcessingCycle();
    for (int i = 0; i < objects.count(); i++) {
        if (detachSubject(objects.at(i)))
            success_list << objects.at(i);
    }
    endProcessingCycle();
    // Broadcast if neccesarry
    if (success_list.count() > 0) {
        emit numberOfSubjectsChanged(SubjectRemoved, success_list);
        emit layoutChanged();
        setModificationState(true);
    }
    return success_list;
}

Qtilities::Core::Observer::EvaluationResult Qtilities::Core::Observer::canDetach(QObject* obj) const {
    if (objectName() != QString(GLOBAL_OBJECT_POOL)) {
        // Check if this subject is observed by this observer. If its not observed by this observer, we can't detach it.
        ObserverProperty observer_list_variant = getObserverProperty(obj,OBSERVER_SUBJECT_IDS);
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
        QVariant category_variant = this->getObserverPropertyValue(obj,OBJECT_CATEGORY);
        QString category_string = category_variant.toString();
        if (isConst(category_string)) {
            LOG_DEBUG(QString("Detaching object \"%1\" from observer \"%2\" is not allowed. This observer is const for the recieved object.").arg(obj->objectName()).arg(objectName()));
            return Observer::Rejected;
        }

        // Check the ownership property of this object
        QVariant ownership_variant = getObserverPropertyValue(obj,OWNERSHIP);
        if (ownership_variant.isValid() && (ownership_variant.toInt() == ObserverScopeOwnership)) {
            if (parentCount(obj) == 1)
                return Observer::LastScopedObserver;
        } else if (ownership_variant.isValid() && (ownership_variant.toInt() == SpecificObserverOwnership)) {
            QVariant observer_parent = getObserverPropertyValue(obj,OBSERVER_PARENT);
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
    detachSubjects(subjectReferences());
    // Updating is done in detachSubjects().
}

void Qtilities::Core::Observer::deleteAll() {
    int total = observerData->subject_list.count();
    if (total == 0)
        return;

    startProcessingCycle();
    for (int i = 0; i < total; i++) {
        // Validate operation against access mode if access mode scope is category
        QVariant category_variant = getObserverPropertyValue(observerData->subject_list.at(0),OBJECT_CATEGORY);
        QString category_string = category_variant.toString();
        if (!isConst(category_string)) {
            delete observerData->subject_list.at(0);
        }
    }
    endProcessingCycle();
    emit numberOfSubjectsChanged(SubjectRemoved, QList<QObject*>());
    emit layoutChanged();
}

QVariant Qtilities::Core::Observer::getObserverPropertyValue(const QObject* obj, const char* property_name) const {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return QVariant();
    #endif

    QVariant prop;
    prop = obj->property(property_name);

    if (prop.isValid() && prop.canConvert<SharedObserverProperty>()) {
        // This is a shared property
        return (prop.value<SharedObserverProperty>()).value();
    } else if (prop.isValid() && prop.canConvert<ObserverProperty>()) {
        // This is a normal observer property (not shared)
        return (prop.value<ObserverProperty>()).value(observerData->observer_id);
    } else if (!prop.isValid()) {
        return QVariant();
    } else {
        LOG_TRACE(QString("Observer (%1): Getting of property (%2) failed, property not recognized as an observer property type.").arg(objectName()).arg(property_name));
        return QVariant();
    }
}

bool Qtilities::Core::Observer::setObserverPropertyValue(QObject* obj, const char* property_name, const QVariant& new_value) const {
    #ifndef QT_NO_DEBUG
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
    if (prop.isValid() && prop.canConvert<SharedObserverProperty>()) {
        // This is a shared property
        SharedObserverProperty shared_property = prop.value<SharedObserverProperty>();
        shared_property.setValue(new_value);
        setSharedProperty(obj,shared_property);
        return true;
    } else if (prop.isValid() && prop.canConvert<ObserverProperty>()) {
        // This is a normal observer property (not shared)
        ObserverProperty observer_property = prop.value<ObserverProperty>();
        observer_property.setValue(new_value,observerData->observer_id);
        setObserverProperty(obj,observer_property);
        return true;
    } else {
        LOG_FATAL(QString(tr("Observer (%1): Setting the value of property (%2) failed. This property is not yet set as an ObserverProperty type class.")).arg(objectName()).arg(property_name));
        // Assert here, otherwise you will think that the property is being set and you won't understand why something else does not work.
        // If you get here, you need to create the property you need, and then set it using the setObserverProperty() or setSharedProperty() calls.
        // This function is not the correct one to use in your situation.
        Q_ASSERT(0);
    }

    return false;
}

void Qtilities::Core::Observer::removeObserverProperties(QObject* obj) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(obj != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

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
        ObserverProperty prop = getObserverProperty(obj, property_name.toStdString().data());
        if (prop.isValid()) {
            // If it exists, we remove this observer context
            prop.removeContext(observerData->observer_id);
            setObserverProperty(obj, prop);
        }
    }

    // If the count is zero after removing the contexts, remove all properties
    if (parentCount(obj) == 0) {
        foreach (QString property_name, added_properties) {
            if (property_name != QString(OBJECT_NAME))
                obj->setProperty(property_name.toStdString().data(),QVariant());
        }
    }

    observerData->filter_subject_events_enabled = true;
    observerData->deliver_qtilities_property_changed_events = true;
}

bool Qtilities::Core::Observer::isParentInHierarchy(const Observer* obj_to_check, const Observer* observer) const {
    // Get all the parents of observer
    ObserverProperty observer_map_prop = getObserverProperty(observer, OBSERVER_SUBJECT_IDS);
    int observer_count;
    if (observer_map_prop.isValid())
        observer_count = observer_map_prop.observerMap().count();
    else
        return false;

    if (observer_count == 0)
        return false;

    bool is_parent = false;
    for (int i = 0; i < observer_count; i++) {
        Observer* parent = OBJECT_MANAGER->observerReference(observer_map_prop.observerMap().keys().at(i));

        if (parent != obj_to_check) {
            is_parent = isParentInHierarchy(obj_to_check,parent);
            if (is_parent)
                break;
        } else
            return true;
    }

    return is_parent;
}

bool Qtilities::Core::Observer::isConst(const QString& category) const {
    AccessMode mode;
    if (category.isEmpty() || (observerData->access_mode_scope == GlobalScope)) {
        mode = (AccessMode) observerData->access_mode;
    } else {
        if (observerData->category_access.keys().contains(category)) {
            mode = (AccessMode) observerData->category_access[category];
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

void Qtilities::Core::Observer::setAccessMode(AccessMode mode, const QString& category) {
    // Check if this observer is read only
    if (observerData->access_mode == ReadOnlyAccess || observerData->access_mode == LockedAccess) {
        LOG_ERROR(QString("Setting the access mode for observer \"%1\" failed. This observer is read only / locked.").arg(objectName()));
        return;
    }

    if (category.isEmpty())
        observerData->access_mode = (int) mode;
    else {
        observerData->category_access[category] = (int) mode;
    }
    emit layoutChanged();
    setModificationState(true);
}

Qtilities::Core::Observer::AccessMode Qtilities::Core::Observer::accessMode(const QString& category) const {
    if (category.isEmpty())
        return (AccessMode) observerData->access_mode;
    else {
        return (AccessMode) observerData->category_access[category];
    }
}

void Qtilities::Core::Observer::setAccessModeScope(AccessModeScope access_mode_scope) {
    observerData->access_mode_scope = (int) access_mode_scope;
    emit layoutChanged();
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

    // Check if the object is in this context
    if (contains(obj) || contains(obj->parent())) {
        // We need to check if a subject has a instance name in this context. If so, we use the instance name, not the objectName().
        QVariant instance_name = getObserverPropertyValue(obj,INSTANCE_NAMES);
        if (instance_name.isValid())
            return instance_name.toString();
        else
            return obj->objectName();
    }

    return QString();
}

int Qtilities::Core::Observer::childCount(const Observer* observer) const {
    static int child_count;

    // We need to iterate over all child observers recursively
    if (!observer) {
        child_count = 0;
        observer = this;
    }

    Observer* child_observer = 0;

    for (int i = 0; i < observer->subjectCount(); i++) {
        ++child_count;

        // Handle the case where the child is an observer.
        child_observer = qobject_cast<Observer*> (observer->subjectAt(i));
        if (child_observer)
            childCount(child_observer);
        else {
            // Handle the case where the child is the parent of an observer
            foreach (QObject* child, observer->subjectAt(i)->children()) {
                child_observer = qobject_cast<Observer*> (child);
                if (child_observer)
                    childCount(child_observer);
            }
        }
    }

    return child_count;
}

QStringList Qtilities::Core::Observer::subjectNames(const QString& iface) const {
    QStringList subject_names;

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        if (observerData->subject_list.at(i)->inherits(iface.toAscii().data()) || iface.isEmpty()) {
            // We need to check if a subject has a instance name in this context. If so, we use the instance name, not the objectName().
            QVariant instance_name = getObserverPropertyValue(observerData->subject_list.at(i),INSTANCE_NAMES);
            if (instance_name.isValid())
                subject_names << instance_name.toString();
            else
                subject_names << observerData->subject_list.at(i)->objectName();
        }
    }
    return subject_names;
}

QStringList Qtilities::Core::Observer::subjectNamesByCategory(const QString& category) const {
    QStringList subject_names;

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        ObserverProperty category_property = getObserverProperty(subjectAt(i),OBJECT_CATEGORY);
        if (category_property.isValid() && !category_property.value(observerID()).toString().isEmpty()) {
            if (category_property.value(observerID()).toString() == category) {
                // We need to check if a subject has a instance name in this context. If so, we use the instance name, not the objectName().
                QVariant instance_name = getObserverPropertyValue(observerData->subject_list.at(i),INSTANCE_NAMES);
                if (instance_name.isValid())
                    subject_names << instance_name.toString();
                else
                    subject_names << observerData->subject_list.at(i)->objectName();
            }
        } else {
            if (category == (QString(OBSERVER_UNCATEGORIZED_CATEGORY))) {
                // We need to check if a subject has a instance name in this context. If so, we use the instance name, not the objectName().
                QVariant instance_name = getObserverPropertyValue(observerData->subject_list.at(i),INSTANCE_NAMES);
                if (instance_name.isValid())
                    subject_names << instance_name.toString();
                else
                    subject_names << observerData->subject_list.at(i)->objectName();
            }
        }
    }

    return subject_names;
}

QStringList Qtilities::Core::Observer::subjectCategories() const {
    QStringList subject_names;

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        ObserverProperty category_property = getObserverProperty(subjectAt(i),OBJECT_CATEGORY);
        if (category_property.isValid() && !category_property.value(observerID()).toString().isEmpty()) {
            if (!subject_names.contains(category_property.value(observerID()).toString()))
                subject_names << category_property.value(observerID()).toString();
        } else {
            if (!subject_names.contains(QString(OBSERVER_UNCATEGORIZED_CATEGORY)))
                subject_names << QString(OBSERVER_UNCATEGORIZED_CATEGORY);
        }
    }

    return subject_names;
}

QObject* Qtilities::Core::Observer::subjectAt(int i) const {
    if ((i < 0) || i >= subjectCount())
        return 0;
    else
        return observerData->subject_list.at(i);
}

int Qtilities::Core::Observer::subjectID(int i) const {
    if (i < subjectCount()) {
        QVariant prop = getObserverPropertyValue(observerData->subject_list.at(i),OBSERVER_SUBJECT_IDS);
        return prop.toInt();
    } else
        return -1;
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

QList<QObject*> Qtilities::Core::Observer::subjectReferencesByCategory(const QString& category) const {
    // Get all subjects which has the OBJECT_CATEGORY property set to category.
    QList<QObject*> list;
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        ObserverProperty category_property = getObserverProperty(subjectAt(i),OBJECT_CATEGORY);
        if (category_property.isValid() && !category_property.value(observerID()).toString().isEmpty()) {
            if (category_property.value(observerID()).toString() == category)
                list << subjectAt(i);
        } else {
            if (category == QString(OBSERVER_UNCATEGORIZED_CATEGORY))
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
        QVariant prop = getObserverPropertyValue(obj,OBSERVER_SUBJECT_IDS);
        if (!prop.isValid()) {
            LOG_TRACE(QString("Observer (%1): Looking for subject ID (%2) failed, property 'Subject ID' contains invalid variant for this context.").arg(objectName()).arg(ID));
            return 0;
        }
        if (prop.toInt() == ID)
            return obj;
    }
    return 0;
}

QObject* Qtilities::Core::Observer::subjectReference(const QString& subject_name) const {
    for (int i = 0; i < observerData->subject_list.count(); i++) {
        QObject* obj = observerData->subject_list.at(i);
        QVariant prop = getObserverPropertyValue(obj,OBJECT_NAME);
        if (!prop.isValid()) {
            LOG_TRACE(QString("Observer (%1): Looking for subject name (%2) failed, property 'Subject Name' contains invalid variant for this context.").arg(objectName()).arg(subject_name));
            return 0;
        }
        if (prop.toString() == subject_name)
            return obj;
    }
    return 0;
}

bool Qtilities::Core::Observer::contains(const QObject* object) const {
    // Problems with const, temp fix
    // return observerData->subject_list.contains(object);

    for (int i = 0; i < observerData->subject_list.count(); i++) {
        if (observerData->subject_list.at(i) == object)
            return true;
    }

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
    IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (subject_filter);
    if (mod_iface) {
        connect(mod_iface->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
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

Qtilities::Core::ObserverHints* const Qtilities::Core::Observer::displayHints() const {
    return observerData->display_hints;
}

bool Qtilities::Core::Observer::setDisplayHints(ObserverHints* display_hints) {
    if (observerData->subject_list.count() > 0 || !display_hints)
        return false;

    if (observerData->display_hints) {
        observerData->display_hints->disconnect(this);
        delete observerData->display_hints;
        observerData->display_hints = 0;
    }

    observerData->display_hints = display_hints;
    return true;
}

Qtilities::Core::ObserverHints* Qtilities::Core::Observer::useDisplayHints() {
    if (!observerData->display_hints) {
        observerData->display_hints = new ObserverHints(this);

        // We need to connect to the modification related signals to this observer:
        connect(observerData->display_hints,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
        return observerData->display_hints;
    }
    return 0;
}

bool Qtilities::Core::Observer::eventFilter(QObject *object, QEvent *event)
{
    if ((event->type() == QEvent::DynamicPropertyChange)) {
        // Get the even tin the correct format
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
            // Check if property change monitoring is enabled.
            // Property changes from within observer sets this to true before changing properties.
            if (!observerData->filter_subject_events_enabled) {
                return false;
            }

            // Handle changes from different threads
            if (!observerData->observer_mutex.tryLock()) {
                QList<QObject*> filtered_list;
                filtered_list << object;
                emit propertyChangeFiltered(propertyChangeEvent->propertyName().data(),filtered_list);
                return true;
            }

            // We now route the event that changed to the subject filter responsible for this property to validate the change.
            // If no subject filter is responsible, the observer needs to handle it itself.
            bool filter_event = false;
            bool is_filter_property = false;
            for (int i = 0; i < observerData->subject_filters.count(); i++) {
                if (observerData->subject_filters.at(i)) {
                    if (observerData->subject_filters.at(i)->monitoredProperties().contains(QString(propertyChangeEvent->propertyName().data()))) {
                        is_filter_property = true;
                        filter_event = observerData->subject_filters.at(i)->handleMonitoredPropertyChange(object, propertyChangeEvent->propertyName().data(),propertyChangeEvent);
                    }
                }
            }

            // If the event should not be filtered, we need to post a user event on the object which will indicate
            // that the property change was valid and succesfull.
            // Note that subject filters must emit do the following themselves. Although this makes implementation
            // of subject filters more difficult, it is more powerfull in this way since one property change can
            // affect other objects as well and only the subject filter will have knowledge about this.
            if (!filter_event && !is_filter_property) {
                // We need to do two things here:
                // 1. If enabled, post the QtilitiesPropertyChangeEvent:
                // First check if this object is in the same thread as this observer:
                if (object->thread() == thread()) {
                    if (observerData->deliver_qtilities_property_changed_events) {
                        QByteArray property_name_byte_array = QByteArray(propertyChangeEvent->propertyName().data());
                        QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observerID());
                        QCoreApplication::postEvent(object,user_event);
                        LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(object->objectName()));
                    }
                } else {
                    LOG_TRACE(QString("Failed to post QtilitiesPropertyChangeEvent (property: %1) to object (%2). The object is not in the same thread.").arg(QString(propertyChangeEvent->propertyName().data())).arg(object->objectName()));
                }

                // 2. Emit the monitoredPropertyChanged() signal:
                QList<QObject*> changed_objects;
                changed_objects << object;
                emit monitoredPropertyChanged(propertyChangeEvent->propertyName(),changed_objects);
            }

            // Unlock the mutex and return
            observerData->observer_mutex.unlock();
            return filter_event;
        }
    }
    return false;
}
