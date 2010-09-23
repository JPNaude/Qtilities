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

#include "ObjectManager.h"
#include "ObserverProperty.h"
#include "QtilitiesCoreConstants.h"
#include "Observer.h"
#include "ObserverHints.h"
#include "SubjectTypeFilter.h"
#include "AbstractSubjectFilter.h"
#include "Factory.h"
#include "ActivityPolicyFilter.h"
#include "SubjectTypeFilter.h"
#include "ObserverRelationalTable.h"

#include <Logger.h>

#include <QVariant>
#include <QMap>
#include <QPointer>
#include <QtCore>

using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core::Interfaces;

struct Qtilities::Core::ObjectManagerData {
    ObjectManagerData() : object_pool(GLOBAL_OBJECT_POOL,QObject::tr("Pool of exposed global objects.")),
    id(1)  { }

    QMap<int,QPointer<Observer> >               observer_map;
    QMap<QString, IFactory*>                    factory_map;
    QMap<QString, QList<QPointer<QObject> > >   meta_type_map;
    Observer                                    object_pool;
    int                                         id;
    Factory<QObject>                            qtilities_factory;
};

Qtilities::Core::ObjectManager::ObjectManager(QObject* parent) : IObjectManager(parent)
{
    d = new ObjectManagerData;
    d->object_pool.startProcessingCycle();
    setObjectName("Object Manager");

    // Add the standard observer subject filters which comes with the Qtilities library here:
    // CoreGui filters are installed in QtilitiesApplication
    FactoryTag activity_policy_filter(FACTORY_TAG_ACTIVITY_POLICY_FILTER,QtilitiesCategory("Subject Filters"));
    d->qtilities_factory.registerFactoryInterface(&ActivityPolicyFilter::factory,activity_policy_filter);
    FactoryTag subject_type_filter(FACTORY_TAG_SUBJECT_TYPE_FILTER,QtilitiesCategory("Subject Filters"));
    d->qtilities_factory.registerFactoryInterface(&SubjectTypeFilter::factory,subject_type_filter);
    FactoryTag observer(FACTORY_TAG_OBSERVER,QtilitiesCategory("Core Classes"));
    d->qtilities_factory.registerFactoryInterface(&Observer::factory,observer);

    // Register the object manager, thus the Qtilities Factory in the list of available IFactories.
    registerIFactory(this);

    // Register some stream operators:
    qRegisterMetaType<Qtilities::Core::QtilitiesCategory>("Qtilities::Core::QtilitiesCategory");
    qRegisterMetaTypeStreamOperators<Qtilities::Core::QtilitiesCategory>("Qtilities::Core::QtilitiesCategory");
}

Qtilities::Core::ObjectManager::~ObjectManager()
{
    delete d;
}

int Qtilities::Core::ObjectManager::registerObserver(Observer* observer) {
    if (observer) {
        QPointer<Observer> q_pointer = observer;
        d->observer_map[d->id] = q_pointer;
        d->id = d->id + 1;
        return d->id-1;
    }

    return -1;
}

QStringList Qtilities::Core::ObjectManager::factoryNames() const {
    QStringList tags;
    tags << FACTORY_QTILITIES;
    return tags;
}

QStringList Qtilities::Core::ObjectManager::factoryTags(const QString& factory_name) const {
    if (factory_name == QString(FACTORY_QTILITIES))
        return d->qtilities_factory.tags();
    else
        return QStringList();
}

QObject* Qtilities::Core::ObjectManager::createInstance(const IFactoryTag& ifactory_data) {
    if (ifactory_data.d_factory_tag == QString(FACTORY_QTILITIES)) {
        QObject* obj = d->qtilities_factory.createInstance(ifactory_data.d_instance_tag);
        if (obj) {
            return obj;
        }
    }
    return 0;
}

Qtilities::Core::Observer* Qtilities::Core::ObjectManager::observerReference(int id) const {
    if (d->observer_map.contains(id)) {
        return d->observer_map.value(id);
    } else if (id == 0) {
        return &d->object_pool;
    } else
        return 0;
}

Qtilities::Core::Observer* const Qtilities::Core::ObjectManager::objectPool() {
    return &d->object_pool;
}

bool Qtilities::Core::ObjectManager::moveSubjects(QList<QObject*> objects, int source_observer_id, int destination_observer_id, bool silent) {
    // Get observer references
    Observer* source_observer = observerReference(source_observer_id);
    Observer* destination_observer = observerReference(destination_observer_id);

    if (!source_observer || !destination_observer)
        return false;

    bool none_failed = true;

    // For now we discard objects that cause problems during attachment and detachment
    for (int i = 0; i < objects.count(); i++) {
        // Check if the destination observer will accept it
        Observer::EvaluationResult result = destination_observer->canAttach(objects.at(i),Observer::ManualOwnership,0,silent);
        if (result == Observer::Rejected) {           
            break;
        } else {
            // Detach from source
            result = source_observer->canDetach(objects.at(i));
            if (result == Observer::Rejected) {
                LOG_ERROR(QString(QObject::tr("The move operation could not be completed. The object you are trying to move was rejected by the destination observer. Check the session log for more details.")));
                none_failed = false;
                break;
            } else if (result == Observer::IsParentObserver) {
                LOG_ERROR(QString(QObject::tr("The move operation could not be completed. The object you are trying to move cannot be removed from the source observer which is defined to be its owner.\n\nTry to share this object with the destination observer instead.")));
                none_failed = false;
                break;
            } else if (result == Observer::LastScopedObserver) {
                destination_observer->setObserverPropertyValue(objects.at(i),OWNERSHIP,QVariant(Observer::ManualOwnership));
                if (!source_observer->detachSubject(objects.at(i))) {
                    destination_observer->setObserverPropertyValue(objects.at(i),OWNERSHIP,QVariant(Observer::ObserverScopeOwnership));
                    none_failed = false;
                    break;
                } else {
                    if (!destination_observer->attachSubject(objects.at(i),Observer::ObserverScopeOwnership))
                        source_observer->attachSubject(objects.at(i),Observer::ObserverScopeOwnership);
                }
            } else {
                if (!source_observer->detachSubject(objects.at(i))) {
                    none_failed = false;
                    break;
                } else {
                    if (!destination_observer->attachSubject(objects.at(i)))
                        source_observer->attachSubject(objects.at(i));
                }
            }
        }
    }

    return none_failed;
}

bool Qtilities::Core::ObjectManager::moveSubjects(QList<QPointer<QObject> > objects, int source_observer_id, int destination_observer_id, bool silent) {
    QList<QObject*> simple_objects;
    for (int i = 0; i < objects.count(); i++)
        simple_objects << objects.at(i);
    return moveSubjects(simple_objects,source_observer_id,destination_observer_id,silent);
}

void Qtilities::Core::ObjectManager::registerObject(QObject* obj, QtilitiesCategory category) {
    if (category.isValid()) {
        if (Observer::propertyExists(obj,OBJECT_CATEGORY)) {
            ObserverProperty category_property = Observer::getObserverProperty(obj,OBJECT_CATEGORY);
            category_property.setValue(qVariantFromValue(category),d->object_pool.observerID());
            Observer::setObserverProperty(obj,category_property);
        } else {
            ObserverProperty category_property(OBJECT_CATEGORY);
            category_property.setValue(qVariantFromValue(category),d->object_pool.observerID());
            Observer::setObserverProperty(obj,category_property);
        }
    }
    if (d->object_pool.attachSubject(obj))
        emit newObjectAdded(obj);
}

void Qtilities::Core::ObjectManager::registerFactoryInterface(FactoryInterface<QObject>* interface, FactoryTag iface_tag) {
    d->qtilities_factory.registerFactoryInterface(interface,iface_tag);
}

bool Qtilities::Core::ObjectManager::registerIFactory(IFactory* factory_iface) {
    if (!factory_iface)
        return false;

    // Check if all factory names provided through factory_iface are unique first:
    foreach(QString name, factory_iface->factoryNames()) {
        if (d->factory_map.keys().contains(name)) {
            LOG_ERROR(QString(tr("Object Manager: Ambiguous factory name \"%1\" detected in registerIFactory(). This IFactory interface will not be registered.")).arg(name));
            return false;
        }
    }

    foreach(QString name, factory_iface->factoryNames()) {
        if (!d->factory_map.keys().contains(name)) {
            d->factory_map[name] = factory_iface;
        }
    }

    return true;
}

Qtilities::Core::Interfaces::IFactory* Qtilities::Core::ObjectManager::referenceIFactory(const QString& tag) const {
    if (d->factory_map.contains(tag))
        return d->factory_map[tag];
    else
        return 0;
}

QStringList Qtilities::Core::ObjectManager::allFactoryNames() const {
    QStringList names;
    QStringList ifactory_keys = d->factory_map.keys();

    foreach (QString ifactory_key, ifactory_keys) {
        IFactory* ifactory = referenceIFactory(ifactory_key);
        if (ifactory)
            names << ifactory->factoryNames();
    }

    return names;
}

QStringList Qtilities::Core::ObjectManager::tagsForFactory(const QString& factory_name) const {
    QStringList tags;
    QStringList ifactory_keys = d->factory_map.keys();

    foreach (QString ifactory_key, ifactory_keys) {
        IFactory* ifactory = referenceIFactory(ifactory_key);
        if (ifactory) {
            foreach (QString factory_name, ifactory->factoryNames())
                tags << ifactory->factoryTags(factory_name);
        }
    }

    return tags;
}

QList<QObject*> Qtilities::Core::ObjectManager::registeredInterfaces(const QString& iface) const {
    return d->object_pool.subjectReferences(iface);
}

void Qtilities::Core::ObjectManager::setMetaTypeActiveObjects(QList<QObject*> objects, const QString& meta_type) {
    QList<QPointer<QObject> > smart_objects;
    for (int i = 0; i < objects.count(); i++)
        smart_objects << objects.at(i);

    d->meta_type_map[meta_type] = smart_objects;
    emit metaTypeActiveObjectsChanged(smart_objects,meta_type);
}

void Qtilities::Core::ObjectManager::setMetaTypeActiveObjects(QList<QPointer<QObject> > objects, const QString& meta_type) {
    d->meta_type_map[meta_type] = objects;
    emit metaTypeActiveObjectsChanged(objects,meta_type);
}

QList<QPointer<QObject> > Qtilities::Core::ObjectManager::metaTypeActiveObjects(const QString& subject_type) const {
    return d->meta_type_map[subject_type];
}

quint32 MARKER_OBJECT_PROPERTY_SECTION = 0xCCCCCCCC;

bool Qtilities::Core::ObjectManager::exportObjectProperties(QObject* obj, QDataStream& stream, PropertyTypeFlags property_types) const {
    if (!obj) {
        Q_ASSERT(obj);
        return false;
    }

    stream << MARKER_OBJECT_PROPERTY_SECTION;
    // First count the number of properties to be exported
    quint32 observer_property_count = 0;
    quint32 shared_property_count = 0;
    QList<ObserverProperty> observer_property_list;
    QList<SharedObserverProperty> shared_property_list;
    // Loop through all dynamic properties and check their exportability.
    for (int p = 0; p < obj->dynamicPropertyNames().count(); p++) {
        ObserverProperty observer_property = Observer::getObserverProperty(obj,obj->dynamicPropertyNames().at(p));
        if (observer_property.isValid() && observer_property.isExportable()) {
            ++observer_property_count;
            observer_property_list << observer_property;
        } else {
            SharedObserverProperty shared_property = Observer::getSharedProperty(obj,obj->dynamicPropertyNames().at(p));
            if (shared_property.isValid() && shared_property.isExportable()) {
                ++shared_property_count;
                shared_property_list << shared_property;
            }
        }
    }

    // Write the shared properties count.
    bool found_visitor_id = false;
    if (property_types & IObjectManager::SharedProperties) {
        stream << shared_property_count;
        LOG_TRACE(QString(tr("Streaming %1 shared properties.")).arg(shared_property_count));
        // Now stream the shared properties.
        for (quint32 p = 0; p < shared_property_count; p++) {
            LOG_TRACE(QString(tr("Streaming shared property: \"%1\"")).arg(QString(shared_property_list.at(p).propertyName())));
            SharedObserverProperty tmp_prop  = shared_property_list.at(p);
            tmp_prop.exportSharedPropertyBinary(stream);
            QString s1 = tmp_prop.propertyName();
            QString s2 = OBSERVER_VISITOR_ID;
            if (s1 == s2)
                found_visitor_id = true;
        }
    } else
        stream << (quint32) 0;

    if (!found_visitor_id)
        LOG_WARNING(QString(tr("No visitor ID property found on object %1")).arg(obj->objectName()));

    stream << MARKER_OBJECT_PROPERTY_SECTION;

    // Write the observer properties count.
    if (property_types & IObjectManager::ObserverProperties) {
        stream << observer_property_count;
        LOG_TRACE(QString(tr("Streaming %1 observer properties.")).arg(observer_property_count));
        // Now stream the observer properties
        for (quint32 p = 0; p < observer_property_count; p++) {
            LOG_TRACE(QString(tr("Streaming observer property: \"%1\"")).arg(QString(observer_property_list.at(p).propertyName())));
            ObserverProperty tmp_prop  = observer_property_list.at(p);
            tmp_prop.exportObserverPropertyBinary(stream);
        }
    } else
        stream << (quint32) 0;

    stream << MARKER_OBJECT_PROPERTY_SECTION;
    return true;
}

bool Qtilities::Core::ObjectManager::importObjectProperties(QObject* new_instance, QDataStream& stream) const {
    Q_ASSERT(new_instance);

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_OBJECT_PROPERTY_SECTION) {
        LOG_ERROR("ObjectManager::importObjectProperties binary import failed to detect start marker. Import will fail.");
        return false;
    }
    // Now read the properties
    quint32 shared_property_count;
    stream >> shared_property_count;
    bool found_visitor_id = false;
    LOG_TRACE(QString(tr("Streaming %1 shared properties.")).arg(shared_property_count));
    for (int p = 0; p < (int) shared_property_count; p++) {
        SharedObserverProperty shared_property;
        if (!shared_property.importSharedPropertyBinary(stream))
            return false;
        if (shared_property.isValid()) {
            QVariant property = qVariantFromValue(shared_property);
            new_instance->setProperty(shared_property.propertyName(),property);
            QString s1 = shared_property.propertyName();
            QString s2 = OBSERVER_VISITOR_ID;
            if (s1 == s2)
                found_visitor_id = true;
        } else {
            LOG_ERROR("ObjectManager::importObjectProperties binary import detected an invalid shared property. Import will fail.");
            return false;
        }
    }
    if (!found_visitor_id)
        LOG_WARNING(QString(tr("No visitor ID property found on object %1")).arg(new_instance->objectName()));

    stream >> ui32;
    if (ui32 != MARKER_OBJECT_PROPERTY_SECTION) {
        LOG_ERROR("ObjectManager::importObjectProperties binary import failed to detect middle marker. Import will fail.");
        return false;
    }
    quint32 observer_property_count;
    stream >> observer_property_count;
    LOG_TRACE(QString(tr("Streaming %1 observer properties.")).arg(observer_property_count));

    for (int p = 0; p < (int) observer_property_count; p++) {
        ObserverProperty observer_property;
        if (!observer_property.importObserverPropertyBinary(stream))
            return false;
        if (observer_property.isValid()) {
            // Create a new property with the observer property.
            // At this stage, the session IDs will be wrong when importing in a different application session.
            // This is fixed in the relational observer table's constructRelationships() function.

            // Important: We must check if a property with the same name already exists. If so, we just
            // print an error message for now:
            if (Observer::propertyExists(new_instance,observer_property.propertyName()))
                LOG_ERROR(QString(tr("While importing properties on object \"%1\", property (%2) which was found in the import binary already existed on the object. This property will be replaced.")).arg(new_instance->objectName()).arg(observer_property.propertyName()));

            QVariant property = qVariantFromValue(observer_property);
            new_instance->setProperty(observer_property.propertyName(),property);
        } else {
            LOG_ERROR("ObjectManager::importObjectProperties binary import detected invalid observer property. Import will fail.");
            return false;
        }
    }
    stream >> ui32;
    if (ui32 != MARKER_OBJECT_PROPERTY_SECTION) {
        LOG_ERROR("ObjectManager::importObjectProperties binary import failed to detect end marker. Import will fail.");
        return false;
    }
    return true;
}

bool Qtilities::Core::ObjectManager::constructRelationships(QList<QPointer<QObject> >& objects, ObserverRelationalTable& table) const {
    LOG_TRACE(QString(tr("Starting observer relationship construction on %1 object(s).")).arg(objects.count()));

    // First check if all the objects in the pointer list are present in the table.
    if (!table.compareObjects(objects)) {
        LOG_ERROR(QString(tr("Relational table comparison failed. Relationship construction aborted.")));
        return false;
    } else
        LOG_TRACE("Table comparison successfull.");

    QList<Observer*> observer_list = observerList(objects);
    // Disable subject event filtering on all observers in list:
    for (int i = 0; i < observer_list.count(); i++) {
        observer_list.at(i)->toggleSubjectEventFiltering(false);
    }

    // Fill in all the session ID fields with the current session information
    // and populate the previous session ID field.
    LOG_TRACE("Populating current session ID fields.");
    for (int i = 0; i < objects.count(); i++) {
        int visitor_id = ObserverRelationalTable::getVisitorID(objects.at(i));
        RelationalTableEntry* entry = table.entryWithVisitorID(visitor_id);
        Observer* obs = qobject_cast<Observer*> (objects.at(i));
        if (!obs) {
            // Check children of the object.
            foreach (QObject* child, objects.at(i)->children()) {
                obs = qobject_cast<Observer*> (child);
                if (obs)
                    break;
            }
        }

        if (obs) {
            LOG_DEBUG(QString("Doing session ID mapping on observer \"%1\": Previous ID: %2, Current ID: %3").arg(obs->observerName()).arg(entry->d_sessionID).arg(obs->observerID()));
            entry->d_previousSessionID = entry->d_sessionID;
            entry->d_sessionID = obs->observerID();
        } else {
            entry->d_previousSessionID = -1;
            entry->d_sessionID = -1;
        }
    }

    // Correct the session IDs of all observer properties.
    // Binary exports of observer properties (not shared) stream the complete observer map of the property.
    // Here we need to correct the observer IDs (session IDs) for the current session and remove
    // contexts which was not part of the export.
    LOG_TRACE("Correcting observer property observer ID fields.");
    for (int i = 0; i < objects.count(); i++) {
        // Loop through all dynamic properties and get all the exportable observer properties.
        int observer_property_count = 0;
        QList<ObserverProperty> observer_property_list;
        QObject* obj = objects.at(i);
        for (int p = 0; p < obj->dynamicPropertyNames().count(); p++) {
            ObserverProperty observer_property = Observer::getObserverProperty(obj,obj->dynamicPropertyNames().at(p));
            // Only exportable properties must be modified here:
            if (observer_property.isValid() && observer_property.isExportable()) {
                ++observer_property_count;
                observer_property_list << observer_property;
            }
        }

        // We need to map each observer ID in the observer map to the current session ID for that observer.
        // We do this using the following steps:
        // -) Loop through all properties.
        // -) For each property, get each observer ID (previous session ID) in the observer map.
        // -) Find the object with the previous session ID in the table.
        // -) Get its current observer ID by casting it to an observer.
        // -) Create a new property with current observer ID and values from current property.
        // -) Lastly replace the property with the new property.
        for (int p = 0; p < observer_property_count; p++) {
            ObserverProperty current_property = observer_property_list.at(p);
            ObserverProperty new_property(current_property.propertyName());
            QMap<int, QVariant> local_map = current_property.observerMap();
            for (int m = 0; m < local_map.count(); m++) {
                int prev_session_id = local_map.keys().at(m);
                RelationalTableEntry* entry = table.entryWithPreviousSessionID(prev_session_id);
                if (!entry) {
                    LOG_ERROR(QString(QObject::tr("ObjectManager::constructRelationships() failed during observer property reconstruction. Failed to find relational table entry for previous session id: %1")).arg(prev_session_id));
                    // If you get here on custom properties added by subject filters, make sure you handle the
                    // import_cycle parameter correctly when initializing and finalizing attachments. What normally
                    // happens is that the filter adds the property again with the current session ID. This check
                    // will notice this because it looks at the new property, not the old correct property.
                    for (int i = 0; i < observer_list.count(); i++) {
                        observer_list.at(i)->toggleSubjectEventFiltering(true);
                    }
                    return false;
                }

                int current_session_id = entry->d_sessionID;
                new_property.addContext(current_property.value(prev_session_id),current_session_id);
            }
            obj->setProperty(current_property.propertyName(),QVariant());
            Observer::setObserverProperty(obj,new_property);
        }
    }

    // Now construct the relationships.
    // We do this by taking the following steps:
    // 1. Go through the list and attach each item to all of its parents.
    //    If it is already attached to a parent, the attachment will just fail.
    //    We get the parent by looking it up in the object list. The lookup is performed by
    //    getting the visitor ID on each object in the list until we find a match.
    //    While going through the list we fill in the sessionID fields of each entry in the relational table.
    //    The sessionID is used again in step 3.
    // 2. Once the parents are sorted out, we need to sort out the object ownership.
    //    This is done by simply setting the OBSERVER_OWNERSHIP property on the object.
    //    If the ownership is SpecificObserverOwnership, we need to set the OBSERVER_PARENT property
    //    as well.
    // 3. Correct the names of each object in all the contexts to which it is attached.
    bool success = true;

    LOG_TRACE("Processing objects in construction list:");
    for (int i = 0; i < objects.count(); i++) {
        Q_ASSERT(objects.at(i));

        // Get the object Visitor ID property:
        int visitor_id = ObserverRelationalTable::getVisitorID(objects.at(i));
        LOG_TRACE(QString(tr("Busy with object %1/%2: %3")).arg(i+1).arg(objects.count()).arg(objects.at(i)->objectName()));

        // Now get this entry in the table:
        RelationalTableEntry* entry = table.entryWithVisitorID(visitor_id);
        if (!entry) {
            LOG_ERROR(tr("Observer relationship construction failed on object: ") + objects.at(i)->objectName() + tr(". An attempt will be made to continue with the rest of the relational table."));
            break;
        }

        // Now attach this subject to each parent using ManualOwnership:
        LOG_TRACE("> Attaching object to all needed contexts.");
        for (int e = 0; e < entry->d_parents.count(); e++) {
            // First get the actual session id (observer ID) for the parent:
            int session_id = table.entryWithVisitorID(entry->d_parents.at(e))->d_sessionID;
            Observer* obs = observerReference(session_id);
            if (obs) {
                // If it was already attached we skip this step.
                if (!obs->contains(objects.at(i))) {
                    obs->attachSubject(objects.at(i));
                    LOG_TRACE(">> Attaching object to context: " + obs->observerName());
                } else {
                    LOG_TRACE(">> Object already attached to context: " + obs->observerName());
                }
            } else {
                LOG_ERROR(QString(tr("Observer ID \"%1\" invalid on object: ")).arg(entry->d_parents.at(e)) + objects.at(i)->objectName() + tr(". An attempt will be made to continue with the rest of the relational table."));
                success = false;
            }
        }

        // Now set the ownership property on the object:
        LOG_TRACE("> Restoring correct ownership for object.");
        if ((Observer::ObjectOwnership) entry->d_ownership == Observer::ManualOwnership) {
            SharedObserverProperty ownership_property(QVariant(Observer::ManualOwnership),OWNERSHIP);
            ownership_property.setIsExportable(false);
            Observer::setSharedProperty(objects.at(i),ownership_property);
            SharedObserverProperty observer_parent_property(QVariant(-1),OBSERVER_PARENT);
            observer_parent_property.setIsExportable(false);
            Observer::setSharedProperty(objects.at(i),observer_parent_property);
            LOG_TRACE(">> Restored object ownership is ManualOwnership.");
        } else if ((Observer::ObjectOwnership) entry->d_ownership == Observer::ObserverScopeOwnership) {
            SharedObserverProperty ownership_property(QVariant(Observer::ObserverScopeOwnership),OWNERSHIP);
            ownership_property.setIsExportable(false);
            Observer::setSharedProperty(objects.at(i),ownership_property);
            SharedObserverProperty observer_parent_property(QVariant(-1),OBSERVER_PARENT);
            observer_parent_property.setIsExportable(false);
            Observer::setSharedProperty(objects.at(i),observer_parent_property);
            LOG_TRACE(">> Restored object ownership is ObserverScopeOwnership.");
        } else if ((Observer::ObjectOwnership) entry->d_ownership == Observer::SpecificObserverOwnership) {
            // Get the session ID of the parent observer:
            RelationalTableEntry* parent_entry = table.entryWithVisitorID(entry->d_parentVisitorID);
            if (parent_entry) {
                int session_id = parent_entry->d_sessionID;
                SharedObserverProperty ownership_property(QVariant(Observer::SpecificObserverOwnership),OWNERSHIP);
                ownership_property.setIsExportable(false);
                Observer::setSharedProperty(objects.at(i),ownership_property);
                SharedObserverProperty observer_parent_property(QVariant(session_id),OBSERVER_PARENT);
                observer_parent_property.setIsExportable(false);
                Observer::setSharedProperty(objects.at(i),observer_parent_property);
                LOG_TRACE(">> Restored object ownership is SpecificObserverOwnership. Owner context ID: " + QString("%1").arg(session_id));
            } else {
                // This will happen when the object is the top level observer which was exported. In this
                // case we need to check if the object has any parents in the observer relational table entry.
                // If so we flag it as an error, else we know that it is not a problem:
                if (entry->d_parents.count() > 0) {
                    LOG_ERROR(QString(QObject::tr("Could not find parent with visitor ID (%1) to which object (%2) must be attached with SpecificObserverOwnership.")).arg(entry->d_parentVisitorID).arg(objects.at(i)->objectName()));
                    success = false;
                }
            }
        } else if ((Observer::ObjectOwnership) entry->d_ownership == Observer::OwnedBySubjectOwnership) {
            SharedObserverProperty ownership_property(QVariant(Observer::OwnedBySubjectOwnership),OWNERSHIP);
            ownership_property.setIsExportable(false);
            Observer::setSharedProperty(objects.at(i),ownership_property);
            SharedObserverProperty observer_parent_property(QVariant(-1),OBSERVER_PARENT);
            observer_parent_property.setIsExportable(false);
            Observer::setSharedProperty(objects.at(i),observer_parent_property);
            LOG_TRACE(">> Restored object ownership is OwnedBySubjectOwnership");
        } else {
            if (entry->d_parents.count() > 0)
                LOG_WARNING(QString(QObject::tr("Could not determine correct ownership for object: %1")).arg(objects.at(i)->objectName()));
        }

        LOG_TRACE("> Restoring instance names across contexts.");
        // Instance names should be correct after the observer IDs have been corrected.
        // Here we just need to correct the OBJECT_NAME_MANAGER_ID property and sync the object name with the OBJECT_NAME property.
        // 1. Correct OBJECT_NAME_MANAGER_ID:
        SharedObserverProperty object_name_manager_id = Observer::getSharedProperty(objects.at(i),OBJECT_NAME_MANAGER_ID);
        if (object_name_manager_id.isValid()) {
            int prev_session_id = object_name_manager_id.value().toInt();
            RelationalTableEntry* entry = table.entryWithPreviousSessionID(prev_session_id);
            if (entry) {
                // The previous name manager was part of this export:
                int current_session_id = entry->d_sessionID;
                SharedObserverProperty new_name_manager_id(current_session_id,OBJECT_NAME_MANAGER_ID);
                new_name_manager_id.setIsExportable(true);
                Observer::setSharedProperty(objects.at(i),new_name_manager_id);
                LOG_TRACE(">> OBJECT_NAME_MANAGER_ID:  Restored name manager successfuly.");
            } else {
                // The previous name manager was not part of this export:
                // Assign a new name manager.
                LOG_TRACE(">> OBJECT_NAME_MANAGER_ID: Name manager was not part of this export. Assigning a new name manager.");
                // Still todo.
            }

            // 2. Sync OBJECT_NAME:
            SharedObserverProperty object_name = Observer::getSharedProperty(objects.at(i),OBJECT_NAME);
            if (object_name.isValid()) {
                objects.at(i)->setObjectName(object_name.value().toString());
                LOG_TRACE(">> OBJECT_NAME_MANAGER_ID : Sync'ed object name with OBJECT_NAME property.");
            }
        } else {
            LOG_TRACE("> OBJECT_NAME_MANAGER_ID : Property not found, nothing to restore.");
        }
    }

    // Enable subject event filtering on all observers in the objects list,
    for (int i = 0; i < observer_list.count(); i++) {
        if (i == 0) {
            // Only one observer has to indicate that it's layout changed:
            observer_list.at(0)->refreshViewsLayout();
        }
        observer_list.at(i)->toggleSubjectEventFiltering(true);
    }

    return success;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObjectManager::exportObserverBinary(QDataStream& stream, Observer* obs, bool verbose_output, QList<QVariant> params) const {
    Q_UNUSED(params)

    if (!obs)
        return IExportable::Failed;

    if (!(obs->supportedFormats() & IExportable::Binary))
        return IExportable::Failed;

    // Export relational data about the observer:
    ObserverRelationalTable table(obs,true);
    if (verbose_output) {
        // Stream the table to a file, and read it back. Then compare it to verify the streaming:
        QTemporaryFile test_file;
        test_file.open();
        QDataStream test_stream_out(&test_file);
        table.exportBinary(test_stream_out);
        test_file.close();
        test_file.open();
        QDataStream test_stream_in(&test_file);    // read the data serialized from the file
        ObserverRelationalTable readback_table;
        readback_table.importBinary(test_stream_in);
        //if (verbose_output)
        //    readback_table.dumpTableInfo();
        test_file.close();
        if (!table.compare(readback_table)) {
            LOG_ERROR(QString(tr("Observer relational table comparison failed. Observer (%1) will not be exported.").arg(obs->observerName())));
            return IExportable::Failed;
        }
    }

    LOG_DEBUG("Exporting observer relational table for design: " + obs->observerName());
    if (verbose_output)
        table.dumpTableInfo();
    table.exportBinary(stream);

    // Now export the observer itself:
    IExportable::Result result = obs->exportBinary(stream,params);
    OBJECT_MANAGER->exportObjectProperties(obs,stream);

    // Check result
    if (result == IExportable::Complete) {
        obs->setModificationState(false, IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
    } else if (result == IExportable::Incomplete) {
        LOG_WARNING(tr("Observer (") + obs->objectName() + tr(") was only partially saved. Saved project will be incomplete."));
        obs->setModificationState(false, IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
    }

    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObjectManager::importObserverBinary(QDataStream& stream, Observer* obs, bool verbose_output, QList<QVariant> params) {
    Q_UNUSED(params)

    if (!obs)
        return IExportable::Failed;

    if (!(obs->supportedFormats() & IExportable::Binary))
        return IExportable::Failed;

    // First stream the relational table
    ObserverRelationalTable readback_table;
    if (!readback_table.importBinary(stream))
        return IExportable::Failed;
    if (verbose_output)
        readback_table.dumpTableInfo();

    // We must stream the factory data for the observer first.
    QList<QPointer<QObject> > internal_import_list;
    IFactoryTag factoryData;
    if (!factoryData.importBinary(stream))
        return IExportable::Failed;

    IExportable::Result result = obs->importBinary(stream, internal_import_list, params);
    if (result == IExportable::Failed)
        return result;
    OBJECT_MANAGER->importObjectProperties(obs,stream);
    internal_import_list.append(obs);

    // Construct relationships:
    if (!OBJECT_MANAGER->constructRelationships(internal_import_list,readback_table))
        result = IExportable::Incomplete;    

    // Cross-check the constructed table:
    ObserverRelationalTable constructed_table(obs,true);
    if (verbose_output) {
        LOG_INFO(QString(tr("Relational verification completed on observer: %1. Here is the contents of the reconstructed table.")).arg(obs->observerName()));
        constructed_table.dumpTableInfo();
    }
    if (!constructed_table.compare(readback_table)) {
        LOG_WARNING(QString(tr("Relational verification failed on observer: %1")).arg(obs->observerName()));
        result = IExportable::Incomplete;
    } else {
        LOG_INFO(QString(tr("Relational verification successful on observer: %1")).arg(obs->observerName()));
    }

    // Remove all relational properties used.
    ObserverRelationalTable::removeRelationalProperties(obs);

    // Once everything is done we look at result to see if it was succesfull.
    if (result == IExportable::Incomplete) {
        LOG_WARNING(tr("Observer (") + obs->observerName() + tr(") was partially reconstructed. Reconstructed context will be incomplete."));
    } else if (result == IExportable::Failed) {
        // Handle deletion of internal_import_list;
        // Delete the first item in the list (the top item) and the rest should be deleted.
        // For the subjects with manual ownership we delete the remaining items in the list manually.
        while (internal_import_list.count() > 0) {
            if (internal_import_list.at(0) != 0) {
                delete internal_import_list.at(0);
                internal_import_list.removeAt(0);
            } else{
                internal_import_list.removeAt(0);
            }
        }
    }
    return result;
}

QList<Qtilities::Core::Observer*> Qtilities::Core::ObjectManager::observerList(QList<QPointer<QObject> >& object_list) const {
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

QDataStream &operator<<(QDataStream &ds, Qtilities::Core::SubjectTypeInfo &s) {
    ds << s.d_meta_type;
    ds << s.d_name;
    return(ds);
}

QDataStream &operator>>(QDataStream &ds, Qtilities::Core::SubjectTypeInfo &s) {
    ds >> s.d_meta_type;
    ds >> s.d_name;
    return(ds);
}
