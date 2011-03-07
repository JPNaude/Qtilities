/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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
#include "QtilitiesProperty.h"
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

struct Qtilities::Core::ObjectManagerPrivateData {
    ObjectManagerPrivateData() : object_pool(qti_def_GLOBAL_OBJECT_POOL,QObject::tr("Pool of exposed global objects.")),
    id(1)  { }

    QMap<int,QPointer<Observer> >               context_map;
    QMap<QString, IFactoryProvider*>            factory_map;
    QMap<QString, QList<QPointer<QObject> > >   meta_type_map;
    Observer                                    object_pool;
    int                                         id;
    Factory<QObject>                            qtilities_factory;
};

Qtilities::Core::ObjectManager::ObjectManager(QObject* parent) : IObjectManager(parent)
{
    d = new ObjectManagerPrivateData;
    d->object_pool.startProcessingCycle();
    setObjectName("Object Manager");

    // Add the standard observer subject filters which comes with the Qtilities library here:
    // CoreGui filters are installed in QtilitiesApplication
    FactoryItemID activity_policy_filter(qti_def_FACTORY_TAG_ACTIVITY_FILTER,QtilitiesCategory("Subject Filters"));
    d->qtilities_factory.registerFactoryInterface(&ActivityPolicyFilter::factory,activity_policy_filter);
    FactoryItemID subject_type_filter(qti_def_FACTORY_TAG_SUBJECT_TYPE_FILTER,QtilitiesCategory("Subject Filters"));
    d->qtilities_factory.registerFactoryInterface(&SubjectTypeFilter::factory,subject_type_filter);
    FactoryItemID observer(qti_def_FACTORY_TAG_OBSERVER,QtilitiesCategory("Core Classes"));
    d->qtilities_factory.registerFactoryInterface(&Observer::factory,observer);

    // Register the object manager, thus the Qtilities Factory in the list of available IFactories.
    registerIFactoryProvider(this);

    // Register some stream operators:
    qRegisterMetaType<Qtilities::Core::QtilitiesCategory>("Qtilities::Core::QtilitiesCategory");
    //qRegisterMetaTypeStreamOperators<Qtilities::Core::QtilitiesCategory>("Qtilities::Core::QtilitiesCategory");
}

Qtilities::Core::ObjectManager::~ObjectManager()
{
    delete d;
}

int Qtilities::Core::ObjectManager::registerObserver(Observer* observer) {
    if (observer) {
        QPointer<Observer> q_pointer = observer;
        d->context_map[d->id] = q_pointer;
        d->id = d->id + 1;
        return d->id-1;
    }

    return -1;
}

QStringList Qtilities::Core::ObjectManager::providedFactories() const {
    QStringList tags;
    tags << qti_def_FACTORY_QTILITIES;
    return tags;
}

QStringList Qtilities::Core::ObjectManager::providedFactoryTags(const QString& factory_name) const {
    if (factory_name == QString(qti_def_FACTORY_QTILITIES))
        return d->qtilities_factory.tags();
    else
        return QStringList();
}

QObject* Qtilities::Core::ObjectManager::createInstance(const InstanceFactoryInfo& ifactory_data) {
    if (ifactory_data.d_factory_tag == QString(qti_def_FACTORY_QTILITIES)) {
        QObject* obj = d->qtilities_factory.createInstance(ifactory_data.d_instance_tag);
        if (obj) {
            return obj;
        }
    }
    return 0;
}

Qtilities::Core::Observer* Qtilities::Core::ObjectManager::observerReference(int id) const {
    if (d->context_map.contains(id)) {
        return d->context_map.value(id);
    } else if (id == 0) {
        return &d->object_pool;
    } else
        return 0;
}

const Qtilities::Core::Observer* Qtilities::Core::ObjectManager::objectPool() {
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
                destination_observer->setQtilitiesPropertyValue(objects.at(i),qti_prop_OWNERSHIP,QVariant(Observer::ManualOwnership));
                if (!source_observer->detachSubject(objects.at(i))) {
                    destination_observer->setQtilitiesPropertyValue(objects.at(i),qti_prop_OWNERSHIP,QVariant(Observer::ObserverScopeOwnership));
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
        if (Observer::propertyExists(obj,qti_prop_CATEGORY_MAP)) {
            MultiContextProperty category_property = Observer::getMultiContextProperty(obj,qti_prop_CATEGORY_MAP);
            category_property.setValue(qVariantFromValue(category),d->object_pool.observerID());
            Observer::setMultiContextProperty(obj,category_property);
        } else {
            MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
            category_property.setValue(qVariantFromValue(category),d->object_pool.observerID());
            Observer::setMultiContextProperty(obj,category_property);
        }
    }
    if (d->object_pool.attachSubject(obj))
        emit newObjectAdded(obj);
}

void Qtilities::Core::ObjectManager::removeObject(QObject* obj) {
    d->object_pool.detachSubject(obj);
}

void Qtilities::Core::ObjectManager::registerFactoryInterface(FactoryInterface<QObject>* interface, FactoryItemID iface_tag) {
    d->qtilities_factory.registerFactoryInterface(interface,iface_tag);
}

bool Qtilities::Core::ObjectManager::registerIFactoryProvider(IFactoryProvider* factory_iface) {
    if (!factory_iface)
        return false;

    // Check if all factory names provided through factory_iface are unique first:
    foreach(QString name, factory_iface->providedFactories()) {
        if (d->factory_map.keys().contains(name)) {
            LOG_ERROR(QString(tr("Object Manager: Ambiguous factory name \"%1\" detected in registerIFactoryProvider(). This IFactoryProvider interface will not be registered.")).arg(name));
            return false;
        }
    }

    foreach(QString name, factory_iface->providedFactories()) {
        if (!d->factory_map.keys().contains(name)) {
            d->factory_map[name] = factory_iface;
        }
    }

    return true;
}

Qtilities::Core::Interfaces::IFactoryProvider* Qtilities::Core::ObjectManager::referenceIFactoryProvider(const QString& tag) const {
    if (d->factory_map.contains(tag))
        return d->factory_map[tag];
    else
        return 0;
}

QStringList Qtilities::Core::ObjectManager::allFactoryNames() const {
    QStringList names;
    QStringList ifactory_keys = d->factory_map.keys();

    foreach (QString ifactory_key, ifactory_keys) {
        IFactoryProvider* ifactory = referenceIFactoryProvider(ifactory_key);
        if (ifactory)
            names << ifactory->providedFactories();
    }

    return names;
}

QStringList Qtilities::Core::ObjectManager::tagsForFactory(const QString& factory_name) const {
    QStringList tags;
    QStringList ifactory_keys = d->factory_map.keys();

    foreach (QString ifactory_key, ifactory_keys) {
        IFactoryProvider* ifactory = referenceIFactoryProvider(ifactory_key);
        if (ifactory) {
            foreach (QString factory_name_int, ifactory->providedFactories()) {
                if (factory_name_int == factory_name) {
                    tags << ifactory->providedFactoryTags(factory_name);
                    break;
                }
            }
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

quint32 MARKER_OBJECT_PROPERTY_SECTION = 0xA0B0C0D0;

bool Qtilities::Core::ObjectManager::exportObjectProperties(QObject* obj, QDataStream& stream, Qtilities::ExportVersion version, PropertyTypeFlags property_types) const {

    if (!obj) {
        Q_ASSERT(obj);
        return false;
    }

    stream << MARKER_OBJECT_PROPERTY_SECTION;
    // First count the number of properties to be exported
    quint32 multi_context_property_count = 0;
    quint32 shared_property_count = 0;
    QList<MultiContextProperty> multi_context_property_list;
    QList<SharedProperty> shared_property_list;
    // Loop through all dynamic properties and check their exportability.
    for (int p = 0; p < obj->dynamicPropertyNames().count(); p++) {
        MultiContextProperty multi_context_property = Observer::getMultiContextProperty(obj,obj->dynamicPropertyNames().at(p));
        if (multi_context_property.isValid() && multi_context_property.isExportable()) {
            ++multi_context_property_count;
            multi_context_property_list << multi_context_property;
        } else {
            SharedProperty shared_property = Observer::getSharedProperty(obj,obj->dynamicPropertyNames().at(p));
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
            SharedProperty tmp_prop  = shared_property_list.at(p);
            tmp_prop.setExportVersion(version);
            tmp_prop.exportBinary(stream);
            QString s1 = tmp_prop.propertyName();
            QString s2 = qti_prop_VISITOR_ID;
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
        stream << multi_context_property_count;
        LOG_TRACE(QString(tr("Streaming %1 observer properties.")).arg(multi_context_property_count));
        // Now stream the observer properties
        for (quint32 p = 0; p < multi_context_property_count; p++) {
            LOG_TRACE(QString(tr("Streaming observer property: \"%1\"")).arg(QString(multi_context_property_list.at(p).propertyName())));
            MultiContextProperty tmp_prop  = multi_context_property_list.at(p);
            tmp_prop.setExportVersion(version);
            tmp_prop.exportBinary(stream);
        }
    } else
        stream << (quint32) 0;

    stream << MARKER_OBJECT_PROPERTY_SECTION;
    return true;
}

bool Qtilities::Core::ObjectManager::importObjectProperties(QObject* new_instance, QDataStream& stream, Qtilities::ExportVersion version) const {
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
        SharedProperty shared_property;
        shared_property.setExportVersion(version);
        QList<QPointer<QObject> > import_list;
        if (!shared_property.importBinary(stream,import_list))
            return false;
        if (shared_property.isValid()) {
            QVariant property = qVariantFromValue(shared_property);
            new_instance->setProperty(shared_property.propertyName(),property);
            QString s1 = shared_property.propertyName();
            QString s2 = qti_prop_VISITOR_ID;
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
    quint32 multi_context_property_count;
    stream >> multi_context_property_count;
    LOG_TRACE(QString(tr("Streaming %1 observer properties.")).arg(multi_context_property_count));

    for (int p = 0; p < (int) multi_context_property_count; p++) {
        MultiContextProperty multi_context_property;
        multi_context_property.setExportVersion(version);
        QList<QPointer<QObject> > import_list;
        if (!multi_context_property.importBinary(stream,import_list))
            return false;
        if (multi_context_property.isValid()) {
            // Create a new property with the observer property.
            // At this stage, the session IDs will be wrong when importing in a different application session.
            // This is fixed in the relational observer table's constructRelationships() function.

            // Important: We must check if a property with the same name already exists. If so, we just
            // print an error message for now:
            if (Observer::propertyExists(new_instance,multi_context_property.propertyName()))
                LOG_ERROR(QString(tr("While importing properties on object \"%1\", property (%2) which was found in the import binary already existed on the object. This property will be replaced.")).arg(new_instance->objectName()).arg(multi_context_property.propertyName()));

            QVariant property = qVariantFromValue(multi_context_property);
            new_instance->setProperty(multi_context_property.propertyName(),property);
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
