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

#include "NamingPolicyFilter.h"
#include "QtilitiesCoreGuiConstants.h"
#include "NamingPolicyInputDialog.h"
#include "QtilitiesApplication.h"
#include "QtilitiesPropertyChangeEvent.h"

#include <Observer.h>
#include <QtilitiesCoreConstants.h>
#include <Logger.h>

#include <QMessageBox>
#include <QInputDialog>
#include <QAbstractButton>
#include <QPushButton>
#include <QMutex>
#include <QVariant>
#include <QRegExpValidator>
#include <QCoreApplication>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core::Constants;

// ----------------------------------------------------------------------------------------
// NAMING POLICY SUBJECT FILTER
// ----------------------------------------------------------------------------------------

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QObject, NamingPolicyFilter> NamingPolicyFilter::factory;
    }
}

struct Qtilities::CoreGui::NamingPolicyFilterData {
    NamingPolicyFilterData() : is_modified(false),
    is_exportable(true),
    conflicting_object(0) { }

    bool is_modified;
    bool is_exportable;
    QValidator* validator;
    NamingPolicyInputDialog* name_dialog;

    QString rollback_name;
    QPointer<QObject> conflicting_object;
    bool validation_cycle_active;
    NamingPolicyFilter::UniquenessPolicy uniqueness_policy;
    NamingPolicyFilter::ResolutionPolicy uniqueness_resolution_policy;
    NamingPolicyFilter::ResolutionPolicy validity_resolution_policy;
};

Qtilities::CoreGui::NamingPolicyFilter::NamingPolicyFilter(QObject* parent) : AbstractSubjectFilter(parent) {
    d = new NamingPolicyFilterData;
    d->uniqueness_policy = NamingPolicyFilter::ProhibitDuplicateNames;
    d->uniqueness_resolution_policy = NamingPolicyFilter::PromptUser;
    d->validity_resolution_policy = NamingPolicyFilter::PromptUser;
    const QRegExp default_expression(".{1,100}",Qt::CaseInsensitive);
    QRegExpValidator* default_validator = new QRegExpValidator(default_expression,0);
    d->validator = default_validator;
    d->name_dialog = new NamingPolicyInputDialog();
    d->name_dialog->setNamingPolicyFilter(this);

    d->validation_cycle_active = false;
}

Qtilities::CoreGui::NamingPolicyFilter::~NamingPolicyFilter() {
    if (d->validator)
        delete d->validator;

    // Check if this policy filter was the object name manager
    for (int i = 0; i < observer->subjectCount(); i++) {
        assignNewNameManager(observer->subjectAt(i));
    }
}

void Qtilities::CoreGui::NamingPolicyFilter::setUniquenessPolicy(NamingPolicyFilter::UniquenessPolicy uniqueness_policy) {
    // Only change policy if the observer context is not defined for the subject filter.
    if (!observer) {
        d->uniqueness_policy = uniqueness_policy;
    } else {
        if (observer->subjectCount() == 0)
            d->uniqueness_policy = uniqueness_policy;
    }
}

Qtilities::CoreGui::NamingPolicyFilter::UniquenessPolicy Qtilities::CoreGui::NamingPolicyFilter::uniquenessNamingPolicy() const {
    return d->uniqueness_policy;
}

void Qtilities::CoreGui::NamingPolicyFilter::setUniquenessResolutionPolicy(NamingPolicyFilter::ResolutionPolicy uniqueness_resolution_policy) {
    d->uniqueness_resolution_policy = uniqueness_resolution_policy;
}

Qtilities::CoreGui::NamingPolicyFilter::ResolutionPolicy Qtilities::CoreGui::NamingPolicyFilter::uniquenessResolutionPolicy() const {
    return d->uniqueness_resolution_policy;
}

void Qtilities::CoreGui::NamingPolicyFilter::setValidityResolutionPolicy(NamingPolicyFilter::ResolutionPolicy validity_resolution_policy) {
    if (validity_resolution_policy == Replace) {
        LOG_ERROR(tr("Error: Cannot set the validity resolution policy of a naming policy filter to Replace. The current resolution policy will not be changed."));
        return;
    }

    d->validity_resolution_policy = validity_resolution_policy;
}

Qtilities::CoreGui::NamingPolicyFilter::ResolutionPolicy Qtilities::CoreGui::NamingPolicyFilter::validityResolutionPolicy() const {
    return d->validity_resolution_policy;
}

Qtilities::CoreGui::NamingPolicyFilter::NameValidity Qtilities::CoreGui::NamingPolicyFilter::evaluateName(QString name) const {
    NamingPolicyFilter::NameValidity result = Acceptable;

    // Check uniqueness of name
    if (observer->subjectNames().contains(name) && d->uniqueness_policy == ProhibitDuplicateNames)
        result |= Duplicate;

    // Validate name using QValidator
    int pos;
    if (d->validator->validate(name,pos) != QValidator::Acceptable)
        result |= Invalid;

    return result;
}

QObject* Qtilities::CoreGui::NamingPolicyFilter::getConflictingObject(QString name) const {
    if (observer->subjectNames().contains(name) && d->uniqueness_policy == ProhibitDuplicateNames) {
        for (int i = 0; i < observer->subjectCount(); i++) {
            if (observer->subjectNames().at(i) == name)
                return observer->subjectAt(i);
        }
    }

    return 0;
}

Qtilities::CoreGui::AbstractSubjectFilter::EvaluationResult Qtilities::CoreGui::NamingPolicyFilter::evaluateAttachment(QObject* obj) const {
    // Check the validity of obj's name:
    NamingPolicyFilter::NameValidity validity_result = evaluateName(obj->objectName());

    if ((validity_result & Invalid) && d->validity_resolution_policy == Reject) {
        return AbstractSubjectFilter::Rejected;
    } else if ((validity_result & Invalid) && d->validity_resolution_policy == PromptUser) {
        return AbstractSubjectFilter::Conditional;
    }

    if ((validity_result & Duplicate) && d->uniqueness_resolution_policy == Reject) {
        return AbstractSubjectFilter::Rejected;
    } else if ((validity_result & Duplicate) && d->uniqueness_resolution_policy == PromptUser) {
        return AbstractSubjectFilter::Conditional;
    }

    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::CoreGui::NamingPolicyFilter::initializeAttachment(QObject* obj, bool import_cycle) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    if (!observer) {
        LOG_TRACE("Cannot evaluate an attachment in a subject filter without an observer context.");
        return false;
    }

    if (import_cycle)
        return true;

    d->rollback_name = obj->objectName();

    // Get name of new subject/object
    // New names are extracted in the following order
    // 1. obj->property(OBJECT_NAME)
    // 2. If (1) does not exist, we take obj->objectName()
    // This function, as well as the NamingPolicyInputDialog uses the OBJECT_NAME property throughout, and then syncs it with objectName() at the end of the function.
    QString new_name = obj->objectName();;
    bool validation_result = true;
    QVariant name_property = observer->getObserverPropertyValue(obj,OBJECT_NAME);
    if (!name_property.isValid()) {
        // In this case, we create the needed properties and add it to the object.
        // It will be removed if attachment fails anywhere.
        SharedObserverProperty new_subject_name_property(QVariant(new_name),OBJECT_NAME);
        new_subject_name_property.setIsExportable(true);
        observer->setSharedProperty(obj,new_subject_name_property);
        SharedObserverProperty object_name_manager_property(QVariant(observer->observerID()),OBJECT_NAME_MANAGER_ID);
        object_name_manager_property.setIsExportable(true);
        observer->setSharedProperty(obj,object_name_manager_property);

        // Check validity of the name.
        validation_result = validateNamePropertyChange(obj,OBJECT_NAME);
    } else {
        new_name = name_property.toString();

        // Check if it does not have a name manager yet, in that case we add a name manager.
        QVariant name_property = observer->getObserverPropertyValue(obj,OBJECT_NAME_MANAGER_ID);
        if (!name_property.isValid()) {
            SharedObserverProperty object_name_manager_property(QVariant(observer->observerID()),OBJECT_NAME_MANAGER_ID);
            object_name_manager_property.setIsExportable(true);
            observer->setSharedProperty(obj,object_name_manager_property);
        }
    }

    // Check if an instance name must be created.
    // The object manager uses OBJECT_NAME, thus we don't create an instance for it ever, only do it if this observer is not the manager.
    if (!isObjectNameManager(obj)) {
        if (d->uniqueness_policy == ProhibitDuplicateNames) {
            ObserverProperty current_instance_names_property = observer->getObserverProperty(obj,INSTANCE_NAMES);
            if (current_instance_names_property.isValid()) {
                // Thus, the property already exists
                current_instance_names_property.addContext(QVariant(new_name),observer->observerID());
                observer->setObserverProperty(obj,current_instance_names_property);
            } else {
                // We need to create the property and add it to the object.
                ObserverProperty new_instance_names_property(INSTANCE_NAMES);
                new_instance_names_property.setIsExportable(true);
                new_instance_names_property.addContext(QVariant(new_name),observer->observerID());
                observer->setObserverProperty(obj,new_instance_names_property);
            }

            // Check validity of the name.
            validation_result = validateNamePropertyChange(obj,OBJECT_NAME);
        }
    }

    // Sync objectName() with the OBJECT_NAME property since the event filter is not installed yet.
    // Only do this if this observer is the object name manager.
    if (isObjectNameManager(obj)) {
        obj->setObjectName(observer->getObserverPropertyValue(obj,OBJECT_NAME).toString());
        if (obj->thread() == thread()) {
            if (observer->qtilitiesPropertyChangeEventsEnabled()) {
                // Post an QtilitiesPropertyChangeEvent on this object notifying that the name changed.
                QByteArray property_name_byte_array = QByteArray(OBJECT_NAME);
                QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                QCoreApplication::postEvent(obj,user_event);
                LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(OBJECT_NAME).arg(obj->objectName()));
            }
        }
    }
    return validation_result;
}

void Qtilities::CoreGui::NamingPolicyFilter::finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle) {
    if (import_cycle)
        return;

    if (!attachment_successful) {
        // Undo possible name changes that happened in initializeAttachment()
        if (isObjectNameManager(obj)) {
            observer->setObserverPropertyValue(obj,OBJECT_NAME,QVariant(d->rollback_name));
            // Assign a new object name manager:
            assignNewNameManager(obj);
        } else {
            // First check if the object has a instance names property then
            if (d->uniqueness_policy == ProhibitDuplicateNames)
                observer->setObserverPropertyValue(obj,INSTANCE_NAMES,QVariant(d->rollback_name));
        }
        // If the attachment failed, we must set d->conflicting_object = 0 again.
        d->conflicting_object = 0;
    } else {
        // Important: If d->conflicting_object is an object when we get here, we delete it. Replace policies
        // would have set it during initialization:
        if (d->conflicting_object)
            delete d->conflicting_object; // It's a QPointer so we don't need to set it = 0.
    }
}

Qtilities::CoreGui::AbstractSubjectFilter::EvaluationResult Qtilities::CoreGui::NamingPolicyFilter::evaluateDetachment(QObject* obj) const {
    Q_UNUSED(obj)

    return AbstractSubjectFilter::Allowed;
}

void Qtilities::CoreGui::NamingPolicyFilter::finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted) {
    if (detachment_successful && !subject_deleted)
        assignNewNameManager(obj);
}

void Qtilities::CoreGui::NamingPolicyFilter::setIsExportable(bool is_exportable) {
    d->is_exportable = is_exportable;
}

bool Qtilities::CoreGui::NamingPolicyFilter::isExportable() const {
    return d->is_exportable;
}

QStringList Qtilities::CoreGui::NamingPolicyFilter::monitoredProperties() const {
    QStringList reserved_properties;
    reserved_properties << QString(OBJECT_NAME) << QString(INSTANCE_NAMES);
    return reserved_properties;
}

QStringList Qtilities::CoreGui::NamingPolicyFilter::reservedProperties() const {
    QStringList reserved_properties;
    reserved_properties << QString(OBJECT_NAME_MANAGER_ID);
    return reserved_properties;
}

bool Qtilities::CoreGui::NamingPolicyFilter::handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    if (!filter_mutex.tryLock())
        return true;

    if (!strcmp(property_name,OBJECT_NAME)) {
        // If OBJECT_NAME changed and this observer is the object name manager, we need to react to this change.
        if (isObjectNameManager(obj)) {
            // Since this observer is the object manager it will make sure that objectName() match the OBJECT_NAME property
            if (!isObjectNameDirty(obj)) {
                // Ok, we know that the property did not change, or its invalid, thus its being added or removed. We never block these action.
                filter_mutex.unlock();
                return false;
            }

            bool return_value = validateNamePropertyChange(obj,OBJECT_NAME);
            if (return_value) {
                // Important: If d->conflicting_object is an object when we get here, we delete it. Replace policies
                // would have set it during initialization:
                bool layout_changed = false;
                if (d->conflicting_object) {
                    delete d->conflicting_object; // It's a QPointer so we don't need to set it = 0.
                    layout_changed = true;
                }

                QString new_name = observer->getObserverPropertyValue(obj,OBJECT_NAME).toString();
                if (!new_name.isEmpty()) {
                    LOG_DEBUG("Sync'ed objectName() with OBJECT_NAME property -> " + new_name);
                    obj->setObjectName(new_name);

                    // What we do here is to change the property value and filter the actual event.
                    // If we don't do this, the notifications below will happen before the property event
                    // is executed. This will only happen when the eventFilter on the observer is finished.
                    observer->setObserverPropertyValue(obj,OBJECT_NAME,QVariant(new_name));

                    // We need to do some things here:
                    // 1. If enabled, post the QtilitiesPropertyChangeEvent:
                    if (obj->thread() == thread()) {
                        if (observer->qtilitiesPropertyChangeEventsEnabled()) {
                            QByteArray property_name_byte_array = QByteArray(propertyChangeEvent->propertyName().data());
                            QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                            QCoreApplication::postEvent(obj,user_event);
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(obj->objectName()));
                        }
                    }

                    // 2. Emit the monitoredPropertyChanged() signal:
                    QList<QObject*> changed_objects;
                    changed_objects << obj;
                    emit monitoredPropertyChanged(propertyChangeEvent->propertyName(),changed_objects);

                    // 3. Change the modification state of the filter:
                    setModificationState(true);

                    // 4. Emit the dataChanged() signal on the observer context:
                    if (layout_changed)
                        observer->refreshViewsLayout();
                    else
                        observer->refreshViewsData();
                }
            } else {
                LOG_WARNING(QString(tr("Property change event from objectName() = %1 to OBJECT_NAME property = %2 aborted.")).arg(obj->objectName()).arg(observer->getObserverPropertyValue(obj,OBJECT_NAME).toString()));
            }

            filter_mutex.unlock();
            return (!return_value);
        } else
            filter_mutex.unlock();
            return false;
    } else if (!strcmp(property_name,INSTANCE_NAMES)) {
        ObserverProperty instance_property = observer->getObserverProperty(obj,INSTANCE_NAMES);
        Q_ASSERT(instance_property.isValid());

        if (instance_property.lastChangedContext() == observer->observerID()) {
            bool return_value = validateNamePropertyChange(obj,INSTANCE_NAMES);
            if (return_value) {
                // Important: If d->conflicting_object is an object when we get here, we delete it. Replace policies
                // would have set it during initialization:
                bool layout_changed = false;
                if (d->conflicting_object) {
                    delete d->conflicting_object; // It's a QPointer so we don't need to set it = 0.
                    layout_changed = true;
                }

                LOG_DEBUG(QString("Detected and handled INSTANCE_NAMES property change to \"%1\" within context \"%2\"").arg(observer->getObserverPropertyValue(obj,OBJECT_NAME).toString()).arg(observer->observerName()));

                // We need to do some things here:
                // 1. If enabled, post the QtilitiesPropertyChangeEvent:
                if (obj->thread() == thread()) {
                    if (observer->qtilitiesPropertyChangeEventsEnabled()) {
                        QByteArray property_name_byte_array = QByteArray(propertyChangeEvent->propertyName().data());
                        QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                        QCoreApplication::postEvent(obj,user_event);
                        LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(obj->objectName()));
                    }
                }

                // 2. Emit the monitoredPropertyChanged() signal:
                QList<QObject*> changed_objects;
                changed_objects << obj;
                emit monitoredPropertyChanged(propertyChangeEvent->propertyName(),changed_objects);

                // 3. Change the modification state of the filter:
                setModificationState(true);

                // 4. Emit the dataChanged() signal on the observer context:
                // First check if the layout changed as well:
                if (layout_changed)
                    observer->refreshViewsLayout();
                else
                    observer->refreshViewsData();

            } else {
                LOG_WARNING(QString(tr("Aborted INSTANCE_NAMES property change event (attempted change to \"%1\" within context \"%2\").")).arg(observer->getObserverPropertyValue(obj,OBJECT_NAME).toString()).arg(observer->observerName()));
            }

            filter_mutex.unlock();
            return (!return_value);
        }
    } else if (!strcmp(property_name,OBJECT_NAME_MANAGER_ID)) {
        // Use makeNameManager() function to do this.
        return true;
    }

    filter_mutex.unlock();
    return false;
}

Qtilities::Core::Interfaces::IFactoryData Qtilities::CoreGui::NamingPolicyFilter::factoryData() const {
    IFactoryData factoryData(FACTORY_QTILITIES,FACTORY_TAG_NAMING_POLICY_FILTER,FACTORY_TAG_NAMING_POLICY_FILTER);
    return factoryData;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::CoreGui::NamingPolicyFilter::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::NamingPolicyFilter::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    Q_UNUSED(params)

    IFactoryData factory_data = factoryData();
    factory_data.exportBinary(stream);

    stream << d->rollback_name;
    stream << (quint32) d->uniqueness_policy;
    stream << (quint32) d->uniqueness_resolution_policy;
    stream << (quint32) d->validity_resolution_policy;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::NamingPolicyFilter::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(import_list)
    Q_UNUSED(params)

    stream >> d->rollback_name;
    quint32 ui32;
    stream >> ui32;
    d->uniqueness_policy = (UniquenessPolicy) ui32;
    stream >> ui32;
    d->uniqueness_resolution_policy = (ResolutionPolicy) ui32;
    stream >> ui32;
    d->validity_resolution_policy = (ResolutionPolicy) ui32;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::NamingPolicyFilter::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(params)

    return IExportable::Incomplete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::NamingPolicyFilter::importXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(params)

    return IExportable::Incomplete;
}

void Qtilities::CoreGui::NamingPolicyFilter::setConflictingObject(QObject* obj) {
    d->conflicting_object = obj;
}

bool Qtilities::CoreGui::NamingPolicyFilter::validateNamePropertyChange(QObject* obj, const char* property_name) {
    QString changed_name = observer->getObserverPropertyValue(obj,property_name).toString();
    NamingPolicyFilter::NameValidity validity_result = evaluateName(changed_name);
    bool return_value;
    if (changed_name.isEmpty())
        return_value = false;
    else
        return_value = true;

    // Ok invalid names must be handled first
    if (validity_result & Invalid) {
        if (d->validity_resolution_policy == PromptUser) {
            if (d->validation_cycle_active && d->name_dialog->useCycleResolution()) {
                if (d->name_dialog->selectedResolution() == Reject)
                    return_value = false;
                else {
                    d->name_dialog->setObject(obj);
                    d->name_dialog->setContext(observer->observerID(),observer->observerName());
                    // The initialize call will recalculate a valid name if needed.
                    d->name_dialog->initialize(validity_result);
                    // Next we set the name of the object using the d->name_dialog
                    d->name_dialog->setName(d->name_dialog->autoGeneratedName());
                    return_value = true;
                }
            } else {
                d->name_dialog->setObject(obj);
                d->name_dialog->setContext(observer->observerID(),observer->observerName());
                d->name_dialog->initialize(validity_result);
                if (d->name_dialog->exec()) {
                    if (d->name_dialog->selectedResolution() == Reject)
                        return_value = false;
                    else
                        return_value = true;
                } else
                    return_value = false;
            }
        } else if (d->validity_resolution_policy == AutoRename) {
            QString valid_name = generateValidName(changed_name);
            if (valid_name.isEmpty())
                return_value = false;
            observer->setObserverPropertyValue(obj,property_name,QVariant(valid_name));
            return_value = true;
        } else if (d->validity_resolution_policy == Reject) {
            return_value = false;
        }
    // Next handle duplicate names.
    } else if ((validity_result & Duplicate) && (d->uniqueness_policy == ProhibitDuplicateNames) && (getConflictingObject(changed_name) != obj)) {
        if (d->uniqueness_resolution_policy == PromptUser) {
            if (d->validation_cycle_active && d->name_dialog->useCycleResolution()) {
                if (d->name_dialog->selectedResolution() == Reject)
                    return_value = false;
                else {
                    d->name_dialog->setObject(obj);
                    d->name_dialog->setContext(observer->observerID(),observer->observerName());
                    // The initialize call will recalculate a valid name if needed.
                    d->name_dialog->initialize(validity_result);
                    // Next we set the name of the object using the d->name_dialog
                    d->name_dialog->setName(d->name_dialog->autoGeneratedName());
                    return_value = true;
                }
            } else {
                d->name_dialog->setObject(obj);
                d->name_dialog->setContext(observer->observerID(),observer->observerName());
                d->name_dialog->initialize(validity_result);
                if (d->name_dialog->exec()) {
                    if (d->name_dialog->selectedResolution() == Reject)
                        return_value = false;
                    else
                        return_value = true;
                } else
                    return_value = false;
            }
        } else if (d->uniqueness_resolution_policy == AutoRename) {
            QString valid_name = generateValidName(changed_name);
            if (valid_name.isEmpty())
                return_value = false;
            observer->setObserverPropertyValue(obj,property_name,QVariant(valid_name));
            return_value = true;
        } else if (d->uniqueness_resolution_policy == Reject) {
            return_value = false;
        } else if (d->uniqueness_resolution_policy == Replace) {
            // Gets the conflicting name:
            QString conflicting_name = QString();
            // Checks if the subject filter is the name manager of the object, in that case
            // it uses OBJECT_NAME. If not, it uses INSTANCE_NAMES with the subject filter's observer context ID.
            // We can't use Observer::subjectNameInContext() here since this function is called during initializeAttachment()
            // as well when the subject is not yet attached to the observer context.
            if (isObjectNameManager(obj)) {
                // We use the OBJECT_NAME property:
                QVariant object_name_prop;
                object_name_prop = obj->property(OBJECT_NAME);
                if (object_name_prop.isValid() && object_name_prop.canConvert<SharedObserverProperty>())
                        conflicting_name = (object_name_prop.value<SharedObserverProperty>()).value().toString();
            } else {
                // We use the INSTANCE_NAMES property:
                QVariant instance_names_prop;
                instance_names_prop = obj->property(INSTANCE_NAMES);
                if (instance_names_prop.isValid() && instance_names_prop.canConvert<ObserverProperty>())
                    conflicting_name = (instance_names_prop.value<ObserverProperty>()).value(observer->observerID()).toString();
            }

            QObject* confliciting_object = getConflictingObject(conflicting_name);
            if (confliciting_object) {
                int parent_count = Observer::parentCount(confliciting_object);
                // Check if the parent observer is the conflicting object's only context:
                if (parent_count == 1 && observer->contains(confliciting_object)) {
                    d->conflicting_object = confliciting_object;
                    return_value = true;
                } else {
                    return_value = false;
                }
            } else
                return_value = false;
        }
    }
    return return_value;
}

void Qtilities::CoreGui::NamingPolicyFilter::setValidator(QValidator* valid_naming_validator) {
    if (!valid_naming_validator)
        return;

    if (observer->subjectCount() > 0)
        return;

    d->validator = valid_naming_validator;
}

QValidator* const Qtilities::CoreGui::NamingPolicyFilter::getValidator() {
    return d->validator;
}

void Qtilities::CoreGui::NamingPolicyFilter::makeNameManager(QObject* obj) {
    // Ok, check if this observer context is observing this object, if not we can't make it a name manager
    ObserverProperty observer_list = observer->getObserverProperty(obj,OBSERVER_SUBJECT_IDS);
    if (observer_list.isValid()) {
        if (!observer_list.hasContext(observer->observerID())) {
            LOG_WARNING(QString(tr("Cannot make observer (%1) the name manager of object (%2). This observer is not currently observing this object.")).arg(observer->observerName()).arg(obj->objectName()));
            return;
        }
    } else {
        LOG_WARNING(QString(tr("Cannot make observer (%1) the name manager of object (%2). This observer is not currently observing this object.")).arg(observer->observerName()).arg(obj->objectName()));
        return;
    }

    // Check if it has a name manager already, if so we add it to the instance names list
    SharedObserverProperty current_manager_id = observer->getSharedProperty(obj,OBJECT_NAME_MANAGER_ID);
    current_manager_id.setIsExportable(true);
    if (current_manager_id.isValid()) {
        if (current_manager_id.value().toInt() == observer->observerID()) {
            LOG_WARNING(QString(tr("Cannot make observer (%1) the name manager of object (%2). This observer is currently the name manager for this object.")).arg(observer->observerName()).arg(obj->objectName()));
            return;
        } else {
            Observer* current_manager = OBJECT_MANAGER->observerReference(current_manager_id.value().toInt());
            Q_ASSERT(current_manager);
            NamingPolicyFilter* naming_filter = 0;
            for (int i = 0; i < current_manager->subjectFilters().count(); i++) {
                // Check if it is a naming policy subject filter
                naming_filter = qobject_cast<NamingPolicyFilter*> (current_manager->subjectFilters().at(i));
            }

            // Add it to the instance name list only if the current manager has a unique naming policy filter
            if (naming_filter) {
                if (naming_filter->uniquenessNamingPolicy() == ProhibitDuplicateNames) {
                    ObserverProperty current_instance_names_property = observer->getObserverProperty(obj,INSTANCE_NAMES);
                    if (current_instance_names_property.isValid()) {
                        current_instance_names_property.addContext(QVariant(obj->objectName()),current_manager->observerID());
                        observer->setObserverProperty(obj,current_instance_names_property);
                    } else {
                        // We need to create the property and add it to the object
                        ObserverProperty new_instance_names_property(INSTANCE_NAMES);
                        new_instance_names_property.setIsExportable(true);
                        new_instance_names_property.addContext(QVariant(obj->objectName()),observer->observerID());
                        observer->setObserverProperty(obj,new_instance_names_property);
                    }
                }
            }
        }
    }

    // Set this naming policy filter as the new name manager
    QString new_managed_name;

    // If this filter has a unique policy, we need to get the new name from the instance name list and remove this context
    if (d->uniqueness_policy == ProhibitDuplicateNames) {
        ObserverProperty current_instance_names_property = observer->getObserverProperty(obj,INSTANCE_NAMES);
        if (current_instance_names_property.isValid()) {
            new_managed_name = current_instance_names_property.value(observer->observerID()).toString();
            current_instance_names_property.removeContext(observer->observerID());
            observer->setObserverProperty(obj,current_instance_names_property);
        }
        obj->setObjectName(new_managed_name);
        observer->setObserverPropertyValue(obj,OBJECT_NAME,new_managed_name);
    }

    observer->setObserverPropertyValue(obj,OBJECT_NAME_MANAGER_ID,observer->observerID());
    //emit notifyDirtyProperty(OBJECT_NAME);
}

void Qtilities::CoreGui::NamingPolicyFilter::startValidationCycle() {
    d->validation_cycle_active = true;
}

void Qtilities::CoreGui::NamingPolicyFilter::endValidationCycle() {
    d->validation_cycle_active = false;
    d->name_dialog->endValidationCycle();
}

bool Qtilities::CoreGui::NamingPolicyFilter::isValidationCycleActive() const {
    return d->validation_cycle_active;
}

bool Qtilities::CoreGui::NamingPolicyFilter::isModified() const {
    return d->is_modified;
}

void Qtilities::CoreGui::NamingPolicyFilter::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets) {
    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

void Qtilities::CoreGui::NamingPolicyFilter::assignNewNameManager(QObject* obj) {
    if (isObjectNameManager(obj)) {
        // Get the next available observer with a naming policy subject filter
        ObserverProperty observer_list = observer->getObserverProperty(obj,OBSERVER_SUBJECT_IDS);
        Observer* next_observer = 0;
        bool found = false;
        if (observer_list.isValid()) {
            for (int i = 0; i < observer_list.observerMap().count(); i++) {
                if (observer_list.observerMap().keys().at(i) != observer->observerID()) {
                    next_observer = OBJECT_MANAGER->observerReference(observer_list.observerMap().keys().at(i));
                    if (next_observer) {
                        for (int i = 0; i < next_observer->subjectFilters().count(); i++) {
                            // Check if it is a naming policy subject filter
                            NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (next_observer->subjectFilters().at(i));
                            if (naming_filter) {
                                found = true;
                                // MOD, a quicker way might be: (But not tested)
                                // next_observer->setObserverPropertyValue(obj,OBJECT_NAME_MANAGER_ID,-1);
                                naming_filter->makeNameManager(obj);
                                LOG_INFO(QString(tr("The name manager (%1) of object (%2) not observing this object any more. Observer (%3) was selected to be the new name manager for this object.")).arg(observer->observerName()).arg(obj->objectName()).arg(next_observer->observerName()));
                            }
                        }
                    }
                }
            }
        }

        if (!found || !next_observer) {
            // An alternative was not found
            obj->setProperty(INSTANCE_NAMES,QVariant());
            obj->setProperty(OBJECT_NAME_MANAGER_ID,QVariant());
            LOG_WARNING(QString(tr("The name manager (%1) of object (%2) is not observing this object any more. An alternative name manager could not be found. This object's name won't be managed until it is attached to a new observer with a naming policy subject filter.")).arg(observer->observerName()).arg(obj->objectName()));
        }
    }
}

bool Qtilities::CoreGui::NamingPolicyFilter::isObjectNameManager(QObject* obj) const {
    QVariant object_name_manager_variant = observer->getObserverPropertyValue(obj,OBJECT_NAME_MANAGER_ID);
    if (object_name_manager_variant.isValid()) {
        return (object_name_manager_variant.toInt() == observer->observerID());
    } else
        return false;
}

bool Qtilities::CoreGui::NamingPolicyFilter::isObjectNameDirty(QObject* obj) const {
    QString changed_name = observer->getObserverPropertyValue(obj,OBJECT_NAME).toString();
    QVariant observer_property = obj->property(OBJECT_NAME);
    if (changed_name == obj->objectName() || !(observer_property.isValid()))
        return false;
    else
        return true;
}

QString Qtilities::CoreGui::NamingPolicyFilter::generateValidName(QString input_name, bool force_change) {
    if (input_name.isNull())
        input_name = QString("new_object");

    // Check, if it is valid just return it.
    NamingPolicyFilter::NameValidity validity_result = evaluateName(input_name);
    if (validity_result == Acceptable) {
        if (!force_change)
            return input_name;
    }

    // If it's invalid try to fix up the string using QValidator
    if (validity_result & Invalid) {
        d->validator->fixup(input_name);
        validity_result = evaluateName(input_name);
        if (validity_result == Acceptable) {
            // Ok, fixup fixed it. Send back the fixed value
            return input_name;
        }
    }

    QString new_name;
    if (!(validity_result & Invalid) && ((validity_result & Duplicate) || force_change)) {
        // Fixup made it valid, but a duplicate exists, thus append the value of a counter
        int counter = 0;

        QString section = input_name.section("_",-1);
        bool ok = false;
        bool use_space = false;
        if (section.toInt(&ok))
            counter = section.toInt(&ok);
        else {
            section = input_name.section(" ",-1);
            if (section.toInt(&ok)) {
                counter = section.toInt(&ok);
                use_space = true;
            } else
                section = input_name;
        }

        ++counter;
        if (section.size() != input_name.size()) {
            if (use_space)
                new_name = QString("%1 %2").arg(input_name.left(input_name.size()-section.size()-1)).arg(counter);
            else
                new_name = QString("%1_%2").arg(input_name.left(input_name.size()-section.size()-1)).arg(counter);
        } else
            new_name = QString("%1_%2").arg(input_name).arg(counter);

        while ((evaluateName(new_name) != Acceptable) || (input_name == new_name)) {
            if (use_space)
                section = new_name.section(" ",-1);
            else
                section = new_name.section("_",-1);
            new_name = new_name.left(new_name.size()-section.size()-1);
            ++counter;
            if (use_space)
                new_name = QString("%1 %2").arg(new_name).arg(counter);
            else
                new_name = QString("%1_%2").arg(new_name).arg(counter);
        }
    } else if ((validity_result & Invalid) && !(validity_result & Duplicate)) {
        // Since fixup did not know how to fix it, we will try a few things
        // If you get here, it is probably better to write your own QValidator
        // and provide a proper fixup implementation
        QString experiment_string = input_name;

        // Try 1: Remove whitespaces
        experiment_string.remove(QChar(' '), Qt::CaseInsensitive);
        if (evaluateName(experiment_string) == Acceptable)
            return experiment_string;

        // Try 2: Try a simple string
        experiment_string = QString("new_object");
        if (evaluateName(experiment_string) == Acceptable)
            return experiment_string;

        // Try 3: Try a simple string without an underscore
        experiment_string = QString("NewObject");
        if (evaluateName(experiment_string) == Acceptable)
            return experiment_string;
    }

    return new_name;
}

//------------------------------------------------------------------------
//
// Naming Policy Delegate
//
//------------------------------------------------------------------------

struct Qtilities::CoreGui::NamingPolicyDelegateData {
    QMutex editing_mutex;
    QPointer<NamingPolicyFilter> naming_filter;
    Observer* observer;
    QString entry_string;
    QObject* obj;
};

Qtilities::CoreGui::NamingPolicyDelegate::NamingPolicyDelegate(QObject *parent) {
    Q_UNUSED(parent)

    d = new NamingPolicyDelegateData;
    d->observer = 0;
    d->naming_filter = 0;
    d->obj = 0;
}

void Qtilities::CoreGui::NamingPolicyDelegate::setObserverContext(Observer* observer) {
    d->observer = observer;

    if (d->observer) {
        // Look which known subject filters are installed in this observer
        for (int i = 0; i < observer->subjectFilters().count(); i++) {
            // Check if it is a naming policy subject filter
            NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (observer->subjectFilters().at(i));
            if (naming_filter)
                d->naming_filter = naming_filter;
        }
    }
}

Qtilities::Core::Observer* Qtilities::CoreGui::NamingPolicyDelegate::observerContext() const {
    return d->observer;
}

QWidget *Qtilities::CoreGui::NamingPolicyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(option)
    Q_UNUSED(index)

    QLineEdit *editor = new QLineEdit(parent);
    connect(editor,SIGNAL(textChanged(QString)),SLOT(on_LineEdit_TextChanged(QString)));

    if (d->observer && d->naming_filter) {
        if (d->naming_filter->getValidator()) {
            editor->setValidator(d->naming_filter->getValidator());
        }
    }

    return editor;
}

void Qtilities::CoreGui::NamingPolicyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QString value = index.model()->data(index, Qt::EditRole).toString();

    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
    d->entry_string = value;
}

void Qtilities::CoreGui::NamingPolicyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
     QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
     QString value = lineEdit->text();

     if (d->observer) {
        LOG_TRACE(QString("Naming control delegate delegated object name within context (%1).").arg(d->observer->observerName()));
     }
     model->setData(index, value, Qt::EditRole);
}

void Qtilities::CoreGui::NamingPolicyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(index)

     editor->setGeometry(option.rect);
}

void Qtilities::CoreGui::NamingPolicyDelegate::on_LineEdit_TextChanged(const QString & text) {
    if (d->observer && d->naming_filter) {
        if (d->naming_filter->uniquenessNamingPolicy() == NamingPolicyFilter::ProhibitDuplicateNames) {
            if (!d->editing_mutex.tryLock())
                return;

            QLineEdit* editor = qobject_cast<QLineEdit*> (sender());
            if (!editor || (text.length() == 0) || (text == d->entry_string)) {
                d->editing_mutex.unlock();
                return;
            }

            editor->setStyleSheet("color: black");
            editor->setToolTip(tr(""));

            NamingPolicyFilter::NameValidity validity_result = d->naming_filter->evaluateName(text);
            if (validity_result != NamingPolicyFilter::Acceptable) {
                if (d->naming_filter->getConflictingObject(text) != d->obj) {
                    editor->setStyleSheet("color: red");
                    if (validity_result == NamingPolicyFilter::Duplicate)
                        editor->setToolTip(tr("The name already exists. Duplicate names are not allowed in this context."));
                    else if (validity_result == NamingPolicyFilter::Invalid)
                        editor->setToolTip(tr("The name contains invalid characters for this context."));
                }
            }

            d->editing_mutex.unlock();
        }
    }
}

void Qtilities::CoreGui::NamingPolicyDelegate::handleCurrentObjectChanged(QList<QObject*> object_list) {
    if (object_list.count() == 1)
        d->obj = object_list.front();
}
