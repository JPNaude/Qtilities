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

#include "NamingPolicyFilter.h"
#include "QtilitiesCoreGuiConstants.h"
#include "NamingPolicyInputDialog.h"
#include "QtilitiesApplication.h"

#include <QtilitiesPropertyChangeEvent>
#include <Observer>
#include <QtilitiesCoreConstants>
#include <SubjectIterator>

#include <Logger>

#include <QMessageBox>
#include <QInputDialog>
#include <QAbstractButton>
#include <QPushButton>
#include <QMutex>
#include <QVariant>
#include <QRegExpValidator>
#include <QCoreApplication>
#include <QDomDocument>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core;

// ----------------------------------------------------------------------------------------
// NAMING POLICY SUBJECT FILTER
// ----------------------------------------------------------------------------------------

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QObject, NamingPolicyFilter> NamingPolicyFilter::factory;
    }
}

Qtilities::CoreGui::NamingPolicyFilter::NamingPolicyFilter(QObject* parent) : AbstractSubjectFilter(parent) {
    d = new NamingPolicyFilterData;
    d->uniqueness_policy = NamingPolicyFilter::ProhibitDuplicateNames;
    d->uniqueness_resolution_policy = NamingPolicyFilter::PromptUser;
    d->validity_resolution_policy = NamingPolicyFilter::PromptUser;
    d->processing_cycle_validation_check_flags = NamingPolicyFilter::AllChecks;
    d->validation_check_flags = NamingPolicyFilter::AllChecks;
    const QRegExp default_expression(".{1,255}",Qt::CaseInsensitive);
    QRegExpValidator* default_validator = new QRegExpValidator(default_expression,0);
    d->validator = default_validator;
    d->name_dialog = 0;

    d->validation_cycle_active = false;
}

Qtilities::CoreGui::NamingPolicyFilter::~NamingPolicyFilter() {
    if (d->validator)
        delete d->validator;

    // Check if this policy filter was the object name manager
    if (observer) {
        for (int i = 0; i < observer->subjectCount(); ++i) {
            assignNewNameManager(observer->subjectAt(i));
        }
    }
}

Qtilities::CoreGui::NamingPolicyFilter& Qtilities::CoreGui::NamingPolicyFilter::operator=(const NamingPolicyFilter& ref) {
    if (this==&ref) return *this;
    d->uniqueness_policy = ref.uniquenessNamingPolicy();
    d->uniqueness_resolution_policy = ref.uniquenessResolutionPolicy();
    d->validity_resolution_policy = ref.validityResolutionPolicy();
    d->processing_cycle_validation_check_flags = ref.processingCycleValidationChecks();
    d->validation_check_flags = ref.validationChecks();

    return *this;
}

bool Qtilities::CoreGui::NamingPolicyFilter::operator==(const NamingPolicyFilter& ref) const {
    if (d->uniqueness_policy != ref.uniquenessNamingPolicy())
        return false;
    if (d->uniqueness_resolution_policy != ref.uniquenessResolutionPolicy())
        return false;
    if (d->validity_resolution_policy != ref.validityResolutionPolicy())
        return false;
    if (d->processing_cycle_validation_check_flags != ref.processingCycleValidationChecks())
        return false;
    if (d->validation_check_flags != ref.validationChecks())
        return false;

    return true;
}

bool Qtilities::CoreGui::NamingPolicyFilter::operator!=(const NamingPolicyFilter& ref) const {
    return !(*this==ref);
}

QString Qtilities::CoreGui::NamingPolicyFilter::debugInfo() const {
    QString info;
    info.append(QString(tr("Debug information on naming policy filter: %1\n")).arg(objectName()));
    info.append(QString("--------------------------------------------\n"));
    info.append(QString(tr("   Uniqueness policy: %1\n")).arg(uniquenessPolicyToString(d->uniqueness_policy)));
    info.append(QString(tr("   Uniqueness resolution policy: %1\n")).arg(resolutionPolicyToString(d->uniqueness_resolution_policy)));
    info.append(QString(tr("   Uniqueness validation policy: %1\n")).arg(resolutionPolicyToString(d->validity_resolution_policy)));
    info.append(QString("--------------------------------------------\n"));
    return info;
}

QString Qtilities::CoreGui::NamingPolicyFilter::uniquenessPolicyToString(UniquenessPolicy uniqueness_policy) {
    if (uniqueness_policy == AllowDuplicateNames) {
        return "AllowDuplicateNames";
    } else if (uniqueness_policy == ProhibitDuplicateNames) {
        return "ProhibitDuplicateNames";
    } else if (uniqueness_policy == ProhibitDuplicateNamesCaseSensitive) {
        return "ProhibitDuplicateNamesCaseSensitive";
    }

    return QString();
}

Qtilities::CoreGui::NamingPolicyFilter::UniquenessPolicy Qtilities::CoreGui::NamingPolicyFilter::stringToUniquenessPolicy(const QString& uniqueness_policy_string) {
    if (uniqueness_policy_string == QLatin1String("AllowDuplicateNames")) {
        return AllowDuplicateNames;
    } else if (uniqueness_policy_string == QLatin1String("ProhibitDuplicateNames")) {
        return ProhibitDuplicateNames;
    } else if (uniqueness_policy_string == QLatin1String("ProhibitDuplicateNamesCaseSensitive")) {
        return ProhibitDuplicateNamesCaseSensitive;
    }
    return AllowDuplicateNames;
}

QString Qtilities::CoreGui::NamingPolicyFilter::resolutionPolicyToString(ResolutionPolicy resolution_policy) {
    if (resolution_policy == AutoRename) {
        return "AutoRename";
    } else if (resolution_policy == PromptUser) {
        return "PromptUser";
    } else if (resolution_policy == Replace) {
        return "Replace";
    } else if (resolution_policy == Reject) {
        return "Reject";
    }

    return QString();
}

Qtilities::CoreGui::NamingPolicyFilter::ResolutionPolicy Qtilities::CoreGui::NamingPolicyFilter::stringToResolutionPolicy(const QString& resolution_policy_string) {
    if (resolution_policy_string == QLatin1String("AutoRename")) {
        return AutoRename;
    } else if (resolution_policy_string == QLatin1String("PromptUser")) {
        return PromptUser;
    } else if (resolution_policy_string == QLatin1String("Replace")) {
        return Replace;
    } else if (resolution_policy_string == QLatin1String("Reject")) {
        return Reject;
    }

    return AutoRename;
}

QString Qtilities::CoreGui::NamingPolicyFilter::validationCheckFlagsToString(ValidationCheckFlags validation_checks) {
    if (validation_checks == NoChecks) {
        return "NoChecks";
    } else if (validation_checks == Validity) {
        return "Validity";
    } else if (validation_checks == Uniqueness) {
        return "Uniqueness";
    } else if (validation_checks == AllChecks) {
        return "AllChecks";
    }

    return QString();
}

Qtilities::CoreGui::NamingPolicyFilter::ValidationCheckFlags Qtilities::CoreGui::NamingPolicyFilter::stringToValidationCheckFlags(const QString& validation_checks) {
    if (validation_checks == QLatin1String("NoChecks")) {
        return NoChecks;
    } else if (validation_checks == QLatin1String("Validity")) {
        return Validity;
    } else if (validation_checks == QLatin1String("Uniqueness")) {
        return Uniqueness;
    } else if (validation_checks == QLatin1String("AllChecks")) {
        return AllChecks;
    }

    return NoChecks;
}

void Qtilities::CoreGui::NamingPolicyFilter::setProcessingCycleValidationChecks(NamingPolicyFilter::ValidationCheckFlags validation_checks) {
    d->processing_cycle_validation_check_flags = validation_checks;
}

Qtilities::CoreGui::NamingPolicyFilter::ValidationCheckFlags Qtilities::CoreGui::NamingPolicyFilter::processingCycleValidationChecks() const {
    return d->processing_cycle_validation_check_flags;
}

void Qtilities::CoreGui::NamingPolicyFilter::setValidationChecks(NamingPolicyFilter::ValidationCheckFlags validation_checks) {
    d->validation_check_flags = validation_checks;
}

Qtilities::CoreGui::NamingPolicyFilter::ValidationCheckFlags Qtilities::CoreGui::NamingPolicyFilter::validationChecks() const {
    return d->validation_check_flags;
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

Qtilities::CoreGui::NamingPolicyFilter::NameValidity Qtilities::CoreGui::NamingPolicyFilter::evaluateName(const QString& name, QObject* object) const {
    NamingPolicyFilter::NameValidity result = Acceptable;

    bool do_uniqueness_test = true;
    if (observer->isProcessingCycleActive() && !(d->processing_cycle_validation_check_flags & Uniqueness))
        do_uniqueness_test = false;
    else if (!observer->isProcessingCycleActive() && !(d->validation_check_flags & Uniqueness))
        do_uniqueness_test = false;

    if (do_uniqueness_test && (d->uniqueness_policy == ProhibitDuplicateNames || d->uniqueness_policy == ProhibitDuplicateNamesCaseSensitive)) {
        Qt::CaseSensitivity case_sensitivity = Qt::CaseSensitive;
        if (d->uniqueness_policy == ProhibitDuplicateNames)
            case_sensitivity = Qt::CaseInsensitive;
        else if (d->uniqueness_policy == ProhibitDuplicateNamesCaseSensitive)
            case_sensitivity = Qt::CaseSensitive;

        if (!object) {
            // Check uniqueness of name:
            QStringList string_list = observer->subjectNames();
            if (string_list.contains(name,case_sensitivity))
                result |= Duplicate;
        } else {
            if (observer->subjectCount() > 0) {
                // Check if the observer has an object model with this name:
                SubjectIterator<QObject> itr(observer->subjectAt(0),
                                             observer);

                QMap<QString,QObject*> actual_paths;
                actual_paths[itr.current()->objectName()] = itr.current();
                while (itr.next()) {
                    actual_paths[itr.current()->objectName()] = itr.current();
                }

                // Check uniqueness of name
                QStringList string_list = QStringList(actual_paths.keys());
                if (string_list.contains(name,case_sensitivity)) {
                    if (actual_paths[name] != object)
                        result |= Duplicate;
                }
            }
        }
    }

    bool do_validation_test = true;
    if (observer->isProcessingCycleActive() && !(d->processing_cycle_validation_check_flags & Validity))
        do_validation_test = false;
    else if (!observer->isProcessingCycleActive() && !(d->validation_check_flags & Validity))
        do_validation_test = false;

    Q_ASSERT(d->validator);
    if (do_validation_test && d->validator) {
        // Validate name using QValidator:
        int pos;
        if (d->validator->validate(*(const_cast<QString*> (&name)),pos) != QValidator::Acceptable)
            result |= Invalid;
    }

    return result;
}

QObject* Qtilities::CoreGui::NamingPolicyFilter::getConflictingObject(const QString& name) const {
    if (observer->subjectNames().contains(name) && d->uniqueness_policy == ProhibitDuplicateNames) {
        for (int i = 0; i < observer->subjectCount(); ++i) {
            if (observer->subjectNames().at(i) == name)
                return observer->subjectAt(i);
        }
    }

    return 0;
}

Qtilities::CoreGui::AbstractSubjectFilter::EvaluationResult Qtilities::CoreGui::NamingPolicyFilter::evaluateAttachment(QObject* obj, QString* rejectMsg, bool silent) const {
    // Check the validity of obj's name:
    QString evaluation_name = getEvaluationName(obj);
    if (evaluation_name.isEmpty())
        evaluation_name = obj->objectName();
    NamingPolicyFilter::NameValidity validity_result = evaluateName(evaluation_name);

    if ((validity_result & Invalid) && d->validity_resolution_policy == Reject) {
        if (rejectMsg)
            *rejectMsg = QString(tr("Naming Policy Filter: Subject name \"%1\" is not valid in this context.")).arg(obj->objectName());
        LOG_INFO(QString(tr("Naming Policy Filter: Subject name \"%1\" is not valid in this context.")).arg(obj->objectName()));
        return AbstractSubjectFilter::Rejected;
    } else if ((validity_result & Invalid) && d->validity_resolution_policy == PromptUser) {
        if (silent) {
            if (rejectMsg)
                *rejectMsg = QString(tr("Naming Policy Filter: Subject name \"%1\" is not valid in this context. The validity resolution policy of this filter is set to PromptUser and a silent attachement was requested. Thus the attachment will be rejected.")).arg(obj->objectName());
            LOG_INFO(QString(tr("Naming Policy Filter: Subject name \"%1\" is not valid in this context. The validity resolution policy of this filter is set to PromptUser and a silent attachement was requested. Thus the attachment will be rejected.")).arg(obj->objectName()));
            LOG_ERROR_P(tr("Attachment now allowed since empty subject names are not valid in this context."));
            return AbstractSubjectFilter::Rejected;
        } else
            return AbstractSubjectFilter::Conditional;
    }

    if (d->uniqueness_policy == ProhibitDuplicateNames || d->uniqueness_policy == ProhibitDuplicateNamesCaseSensitive) {
        if ((validity_result & Duplicate) && d->uniqueness_resolution_policy == Reject) {
            if (rejectMsg)
                *rejectMsg = QString(tr("Naming Policy Filter: Subject name \"%1\" is not unique in this context.")).arg(obj->objectName());
            return AbstractSubjectFilter::Rejected;
        } else if ((validity_result & Duplicate) && d->uniqueness_resolution_policy == PromptUser) {
            if (silent) {
                if (rejectMsg)
                    *rejectMsg = QString(tr("Naming Policy Filter: Subject name \"%1\" is not unique in this context. The uniqueness resolution policy of this filter is set to PromptUser and a silent attachement was requested. Thus the attachment will be rejected.")).arg(obj->objectName());
                LOG_INFO(QString(tr("Naming Policy Filter: Subject name \"%1\" is not unique in this context. The uniqueness resolution policy of this filter is set to PromptUser and a silent attachement was requested. Thus the attachment will be rejected.")).arg(obj->objectName()));
                LOG_ERROR_P(tr("Attachment now allowed since it will result in duplicate entries."));
                return AbstractSubjectFilter::Rejected;
            } else
                return AbstractSubjectFilter::Conditional;
        }
        // Rename and replace will take care of it, thus we allow it.
    }

    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::CoreGui::NamingPolicyFilter::initializeAttachment(QObject* obj, QString* rejectMsg, bool import_cycle) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj) {
            if (rejectMsg)
                *rejectMsg = tr("Naming Policy Filter: Invalid object reference received. Attachment cannot be done.");
            return false;
        }
    #endif

    if (!observer) {
        if (rejectMsg)
            *rejectMsg = tr("Naming Policy Filter: Cannot evaluate an attachment in a subject filter without an observer context.");
        LOG_TRACE(tr("Cannot evaluate an attachment in a subject filter without an observer context."));
        return false;
    }

    if (import_cycle)
        return true;

    d->rollback_name = obj->objectName();
    QPointer<QObject> safe_obj = obj;

    // Get name of new subject/object
    // New names are extracted in the following order
    // 1. obj->property(qti_prop_NAME)
    // 2. If (1) does not exist, we take obj->objectName()
    // This function, as well as the NamingPolicyInputDialog uses the qti_prop_NAME property throughout, and then syncs it with objectName() at the end of the function.
    QString new_name = obj->objectName();;
    bool validation_result = true;
    QVariant name_property = observer->getMultiContextPropertyValue(obj,qti_prop_NAME);
    if (!name_property.isValid()) {
        // In this case, we create the needed properties and add it to the object.
        // It will be removed if attachment fails anywhere.
        SharedProperty new_subject_name_property(qti_prop_NAME,QVariant(new_name));
        ObjectManager::setSharedProperty(obj,new_subject_name_property);
        SharedProperty object_name_manager_property(qti_prop_NAME_MANAGER_ID,QVariant(observer->observerID()));
        ObjectManager::setSharedProperty(obj,object_name_manager_property);

        // Check validity of the name.
        validation_result = validateNamePropertyChange(obj,qti_prop_NAME);
    } else {
        new_name = name_property.toString();

        // Check if it does not have a name manager yet, in that case we add a name manager.
        QVariant name_property = observer->getMultiContextPropertyValue(obj,qti_prop_NAME_MANAGER_ID);
        if (!name_property.isValid()) {
            SharedProperty object_name_manager_property(qti_prop_NAME_MANAGER_ID,QVariant(observer->observerID()));
            ObjectManager::setSharedProperty(obj,object_name_manager_property);
        }
    }

    if (!safe_obj)
        return false;

    // Check if an instance name must be created.
    // The object manager uses qti_prop_NAME, thus we don't create an instance for it ever, only do it if this observer is not the manager.
    if (!isObjectNameManager(obj)) {
        if (d->uniqueness_policy == ProhibitDuplicateNames) {
            MultiContextProperty current_instance_names_property = ObjectManager::getMultiContextProperty(obj,qti_prop_ALIAS_MAP);
            if (current_instance_names_property.isValid()) {
                // Thus, the property already exists
                current_instance_names_property.addContext(QVariant(new_name),observer->observerID());
                ObjectManager::setMultiContextProperty(obj,current_instance_names_property);
            } else {
                // We need to create the property and add it to the object.
                MultiContextProperty new_instance_names_property(qti_prop_ALIAS_MAP);
                new_instance_names_property.addContext(QVariant(new_name),observer->observerID());
                ObjectManager::setMultiContextProperty(obj,new_instance_names_property);
            }

            // Check validity of the name.
            validation_result = validateNamePropertyChange(obj,qti_prop_NAME);
        }
    }

    // Sync objectName() with the qti_prop_NAME property since the event filter is not installed yet.
    // Only do this if this observer is the object name manager.
    if (isObjectNameManager(obj)) {
        obj->setObjectName(observer->getMultiContextPropertyValue(obj,qti_prop_NAME).toString());
        if (obj->thread() == thread()) {
            if (observer->qtilitiesPropertyChangeEventsEnabled()) {
                // Post a QtilitiesPropertyChangeEvent on this object notifying that the name changed.
                QByteArray property_name_byte_array = QByteArray(qti_prop_NAME);
                QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                QCoreApplication::postEvent(obj,user_event);
                LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(qti_prop_NAME).arg(obj->objectName()));
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
            observer->setMultiContextPropertyValue(obj,qti_prop_NAME,QVariant(d->rollback_name));
            // Assign a new object name manager:
            assignNewNameManager(obj);
        } else {
            // First check if the object has a instance names property then
            if (d->uniqueness_policy == ProhibitDuplicateNames)
                observer->setMultiContextPropertyValue(obj,qti_prop_ALIAS_MAP,QVariant(d->rollback_name));
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

void Qtilities::CoreGui::NamingPolicyFilter::finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted) {
    if (detachment_successful && !subject_deleted)
        assignNewNameManager(obj);
}

QStringList Qtilities::CoreGui::NamingPolicyFilter::monitoredProperties() const {
    QStringList reserved_properties;
    reserved_properties << QString(qti_prop_NAME) << QString(qti_prop_ALIAS_MAP);
    return reserved_properties;
}

QStringList Qtilities::CoreGui::NamingPolicyFilter::reservedProperties() const {
    QStringList reserved_properties;
    reserved_properties << QString(qti_prop_NAME_MANAGER_ID);
    return reserved_properties;
}

bool Qtilities::CoreGui::NamingPolicyFilter::handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    if (!filter_mutex.tryLock())
        return false;

    if (!strcmp(property_name,qti_prop_NAME)) {
        // If qti_prop_NAME changed and this observer is the object name manager, we need to react to this change.
        if (isObjectNameManager(obj)) {
            // Since this observer is the object manager it will make sure that objectName() match the qti_prop_NAME property
            if (!isObjectNameDirty(obj)) {
                // Ok, we know that the property did not change, or its invalid, thus its being added or removed. We never block these actions.
                filter_mutex.unlock();
                return false;
            }

            QPointer<QObject> safe_obj;
            safe_obj = obj;

            bool return_value = validateNamePropertyChange(obj,qti_prop_NAME);
            if (return_value) {
                // Important: If d->conflicting_object is an object when we get here, we delete it. Replace policies
                // would have set it during initialization:
                bool layout_changed = false;
                if (d->conflicting_object) {
                    Q_ASSERT(d->conflicting_object != obj);
                    delete d->conflicting_object; // It's a QPointer so we don't need to set it = 0.
                    layout_changed = true;
                }

                if (!safe_obj)
                    return false;

                QString new_name = observer->getMultiContextPropertyValue(obj,qti_prop_NAME).toString();
                if (!new_name.isEmpty()) {
                    QString old_name = obj->objectName();

                    LOG_DEBUG("Sync'ed objectName() with qti_prop_NAME property. New name \"" + new_name + "\", Old name \"" + old_name);
                    obj->setObjectName(new_name);

                    // What we do here is to change the property value and filter the actual event.
                    // If we don't do this, the notifications below will happen before the property event
                    // is executed. This will only happen when the eventFilter on the observer is finished.
                    observer->setMultiContextPropertyValue(obj,qti_prop_NAME,QVariant(new_name));

                    // We need to do some things here:
                    // 1. If enabled, post the QtilitiesPropertyChangeEvent:
//                    if (obj->thread() == thread()) {
//                        if (observer->qtilitiesPropertyChangeEventsEnabled()) {
//                            QByteArray property_name_byte_array = QByteArray(propertyChangeEvent->propertyName().data());
//                            QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
//                            QCoreApplication::postEvent(obj,user_event);
//                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(obj->objectName()));
//                        }
//                    }

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

                    // 5. Emit the subjectNameChanged() signal:
                    emit subjectNameChanged(obj,old_name,new_name);
                }
            } else {
                LOG_WARNING(QString(tr("Property change event from objectName() = %1 to qti_prop_NAME property = %2 aborted.")).arg(obj->objectName()).arg(observer->getMultiContextPropertyValue(obj,qti_prop_NAME).toString()));
            }

            filter_mutex.unlock();
            return (!return_value);
        } else
            filter_mutex.unlock();
            return false;
    } else if (!strcmp(property_name,qti_prop_ALIAS_MAP)) {
        MultiContextProperty instance_property = ObjectManager::getMultiContextProperty(obj,qti_prop_ALIAS_MAP);
        #ifndef QT_NO_DEBUG
            if (!instance_property.isValid())
                return false;
        #else
            Q_ASSERT(instance_property.isValid());
        #endif


        if (instance_property.lastChangedContext() == observer->observerID()) {
            bool return_value = validateNamePropertyChange(obj,qti_prop_ALIAS_MAP);
            if (return_value) {
                // Important: If d->conflicting_object is an object when we get here, we delete it. Replace policies
                // would have set it during initialization:
                bool layout_changed = false;
                if (d->conflicting_object) {
                    delete d->conflicting_object; // It's a QPointer so we don't need to set it = 0.
                    layout_changed = true;
                }

                LOG_DEBUG(QString("Detected and handled qti_prop_ALIAS_MAP property change to \"%1\" within context \"%2\"").arg(observer->getMultiContextPropertyValue(obj,qti_prop_NAME).toString()).arg(observer->observerName()));

                // We need to do some things here:
                // 1. If enabled, post the QtilitiesPropertyChangeEvent:
//                if (obj->thread() == thread()) {
//                    if (observer->qtilitiesPropertyChangeEventsEnabled()) {
//                        QByteArray property_name_byte_array = QByteArray(propertyChangeEvent->propertyName().data());
//                        QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
//                        QCoreApplication::postEvent(obj,user_event);
//                        LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(obj->objectName()));
//                    }
//                }

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
                LOG_WARNING(QString(tr("Aborted qti_prop_ALIAS_MAP property change event (attempted change to \"%1\" within context \"%2\").")).arg(observer->getMultiContextPropertyValue(obj,qti_prop_NAME).toString()).arg(observer->observerName()));
            }

            filter_mutex.unlock();
            return (!return_value);
        }
    } else if (!strcmp(property_name,qti_prop_NAME_MANAGER_ID)) {
        // Use makeNameManager() function to do this.
        return true;
    }

    filter_mutex.unlock();
    return false;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::CoreGui::NamingPolicyFilter::instanceFactoryInfo() const {
    InstanceFactoryInfo instanceFactoryInfo(qti_def_FACTORY_QTILITIES,qti_def_FACTORY_TAG_NAMING_FILTER,qti_def_FACTORY_TAG_NAMING_FILTER);
    return instanceFactoryInfo;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::CoreGui::NamingPolicyFilter::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::NamingPolicyFilter::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << (quint32) d->uniqueness_policy;
    stream << (quint32) d->uniqueness_resolution_policy;
    stream << (quint32) d->validity_resolution_policy;
    stream << (quint32) d->processing_cycle_validation_check_flags;
    stream << (quint32) d->validation_check_flags;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::NamingPolicyFilter::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)
     
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    quint32 ui32;
    stream >> ui32;
    d->uniqueness_policy = (UniquenessPolicy) ui32;
    stream >> ui32;
    d->uniqueness_resolution_policy = (ResolutionPolicy) ui32;
    stream >> ui32;
    d->validity_resolution_policy = (ResolutionPolicy) ui32;
    stream >> ui32;
    d->processing_cycle_validation_check_flags = (ValidationCheckFlags) ui32;
    stream >> ui32;
    d->validation_check_flags = (ValidationCheckFlags) ui32;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::NamingPolicyFilter::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    object_node->setAttribute("UniquenessPolicy",uniquenessPolicyToString(d->uniqueness_policy));
    object_node->setAttribute("ValidityResolutionPolicy",resolutionPolicyToString(d->validity_resolution_policy));
    object_node->setAttribute("UniquenessResolutionPolicy",resolutionPolicyToString(d->uniqueness_resolution_policy));
    object_node->setAttribute("ProcessingCycleValidationCheckFlags",validationCheckFlagsToString(d->processing_cycle_validation_check_flags));
    object_node->setAttribute("ValidationCheckFlags",validationCheckFlagsToString(d->validation_check_flags));
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::NamingPolicyFilter::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {    
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (object_node->hasAttribute("NewSubjectActivityPolicy"))
        d->uniqueness_policy = stringToUniquenessPolicy(object_node->attribute("UniquenessPolicy"));
    if (object_node->hasAttribute("ValidityResolutionPolicy"))
        d->validity_resolution_policy = stringToResolutionPolicy(object_node->attribute("ValidityResolutionPolicy"));
    if (object_node->hasAttribute("UniquenessResolutionPolicy"))
        d->uniqueness_resolution_policy = stringToResolutionPolicy(object_node->attribute("UniquenessResolutionPolicy"));
    if (object_node->hasAttribute("ProcessingCycleValidationCheckFlags"))
        d->processing_cycle_validation_check_flags = stringToValidationCheckFlags(object_node->attribute("ProcessingCycleValidationCheckFlags"));
    if (object_node->hasAttribute("ValidationCheckFlags"))
        d->validation_check_flags = stringToValidationCheckFlags(object_node->attribute("ValidationCheckFlags"));


    return IExportable::Complete;
}

void Qtilities::CoreGui::NamingPolicyFilter::setConflictingObject(QObject* obj) {
    d->conflicting_object = obj;
}

bool Qtilities::CoreGui::NamingPolicyFilter::validateNamePropertyChange(QObject* obj, const char* property_name) {
    QString evaluation_name = getEvaluationName(obj);
    if (evaluation_name.isEmpty())
        evaluation_name = observer->getMultiContextPropertyValue(obj,property_name).toString();
    NamingPolicyFilter::NameValidity validity_result = evaluateName(evaluation_name,obj);
    bool return_value = true;

    // Invalid names must be handled first:
    if (validity_result & Invalid) {
        QRegExpValidator* reg_exp_validator = qobject_cast<QRegExpValidator*> (d->validator);
        if (reg_exp_validator)
            LOG_WARNING(QString("Naming Policy Filter: Validation of name property change detected an invalid name in context: \"%1\". Invalid Name = \"%2\", Validation Expression = \"%3\". This property change will be rejected.").arg(observer->objectName()).arg(evaluation_name).arg(reg_exp_validator->regExp().pattern()));
        else
            LOG_WARNING(QString("Naming Policy Filter: Validation of name property change detected an invalid name in context: \"%1\". Invalid Name = \"%2\". This property change will be rejected.").arg(observer->objectName()).arg(evaluation_name));
        if (d->validity_resolution_policy == PromptUser) {
            if (!d->name_dialog) {
                d->name_dialog = constructUserDialog();
                d->name_dialog->setNamingPolicyFilter(this);
            }

            if (d->validation_cycle_active && d->name_dialog->useCycleResolution()) {
                if (d->name_dialog->selectedResolution() == Reject)
                    return_value = false;
                else {
                    d->name_dialog->setObject(obj);
                    d->name_dialog->setContext(observer->observerID(),observer->observerName());
                    // The initialize call will recalculate a valid name if needed.
                    d->name_dialog->initialize(validity_result);
                    // Next we set the name of the object using the d->name_dialog
                    setName(obj,d->name_dialog->autoGeneratedName());
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
            QString valid_name = generateValidName(evaluation_name);
            if (valid_name.isEmpty()) {
                return_value = false;
            } else {
                observer->setMultiContextPropertyValue(obj,property_name,QVariant(valid_name));
                return_value = true;
            }
        } else if (d->validity_resolution_policy == Reject) {
            return_value = false;
        }
    // Next handle duplicate names:
    } else if ((validity_result & Duplicate) && (d->uniqueness_policy == ProhibitDuplicateNames) && (getConflictingObject(evaluation_name) != obj)) {
        LOG_WARNING(QString("Naming Policy Filter: Validation of name property change detected a duplicate name in context: \"%1\". Duplicate Name = \"%2\". This property change will be rejected.").arg(observer->objectName()).arg(evaluation_name));
        if (d->uniqueness_resolution_policy == PromptUser) {
            if (!d->name_dialog) {
                d->name_dialog = constructUserDialog();
                d->name_dialog->setNamingPolicyFilter(this);
            }
            if (d->validation_cycle_active && d->name_dialog->useCycleResolution()) {
                if (d->name_dialog->selectedResolution() == Reject)
                    return_value = false;
                else {
                    d->name_dialog->setObject(obj);
                    d->name_dialog->setContext(observer->observerID(),observer->observerName());
                    // The initialize call will recalculate a valid name if needed.
                    d->name_dialog->initialize(validity_result);
                    // Next we set the name of the object using the d->name_dialog
                    setName(obj,d->name_dialog->autoGeneratedName());
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
            QString valid_name = generateValidName(evaluation_name);
            if (valid_name.isEmpty())
                return_value = false;

            // Check if we must use the instance name or the object name.
            if (isObjectNameManager(obj)) {
                // We use the qti_prop_NAME property:
                QVariant object_name_prop;
                object_name_prop = obj->property(qti_prop_NAME);
                if (object_name_prop.isValid() && object_name_prop.canConvert<SharedProperty>()) {
                    SharedProperty name_property(qti_prop_NAME,QVariant(valid_name));
                    QVariant property = qVariantFromValue(name_property);
                    obj->setProperty(qti_prop_NAME,QVariant(property));
                    return_value = true;
                } else
                    return_value = false;
            } else {
                // We use the qti_prop_ALIAS_MAP property:
                QVariant instance_names_prop;
                instance_names_prop = obj->property(qti_prop_ALIAS_MAP);
                if (instance_names_prop.isValid() && instance_names_prop.canConvert<MultiContextProperty>()) {
                    MultiContextProperty new_instance_name = instance_names_prop.value<MultiContextProperty>();
                    new_instance_name.setValue(QVariant(valid_name),observer->observerID());
                    obj->setProperty(qti_prop_ALIAS_MAP,qVariantFromValue(new_instance_name));
                    return_value = true;
                } else
                    return_value = false;
            }
        } else if (d->uniqueness_resolution_policy == Reject) {
            return_value = false;
        } else if (d->uniqueness_resolution_policy == Replace) {
            // Gets the conflicting name:
            QString conflicting_name = QString();
            // Checks if the subject filter is the name manager of the object, in that case
            // it uses qti_prop_NAME. If not, it uses qti_prop_ALIAS_MAP with the subject filter's observer context ID.
            // We can't use Observer::subjectNameInContext() here since this function is called during initializeAttachment()
            // as well when the subject is not yet attached to the observer context.
            if (isObjectNameManager(obj)) {
                // We use the qti_prop_NAME property:
                QVariant object_name_prop;
                object_name_prop = obj->property(qti_prop_NAME);
                if (object_name_prop.isValid() && object_name_prop.canConvert<SharedProperty>())
                        conflicting_name = (object_name_prop.value<SharedProperty>()).value().toString();
            } else {
                // We use the qti_prop_ALIAS_MAP property:
                QVariant instance_names_prop;
                instance_names_prop = obj->property(qti_prop_ALIAS_MAP);
                if (instance_names_prop.isValid() && instance_names_prop.canConvert<MultiContextProperty>())
                    conflicting_name = (instance_names_prop.value<MultiContextProperty>()).value(observer->observerID()).toString();
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

    if (observer) {
        if (observer->subjectCount() > 0)
            return;
    }

    d->validator = valid_naming_validator;
}

QValidator* Qtilities::CoreGui::NamingPolicyFilter::getValidator() {
    return d->validator;
}

void Qtilities::CoreGui::NamingPolicyFilter::makeNameManager(QObject* obj) {
    // Ok, check if this observer context is observing this object, if not we can't make it a name manager
    MultiContextProperty observer_list = ObjectManager::getMultiContextProperty(obj,qti_prop_OBSERVER_MAP);
    if (observer_list.isValid()) {
        if (!observer_list.hasContext(observer->observerID())) {
            LOG_DEBUG(QString(tr("Cannot make observer (%1) the name manager of object (%2). This observer is not currently observing this object.")).arg(observer->observerName()).arg(obj->objectName()));
            return;
        }
    } else {
        LOG_DEBUG(QString(tr("Cannot make observer (%1) the name manager of object (%2). This observer is not currently observing this object.")).arg(observer->observerName()).arg(obj->objectName()));
        return;
    }

    // Check if it has a name manager already, if so we add it to the instance names list
    SharedProperty current_manager_id = ObjectManager::getSharedProperty(obj,qti_prop_NAME_MANAGER_ID);
    if (current_manager_id.isValid()) {
        if (current_manager_id.value().toInt() == observer->observerID()) {
            LOG_DEBUG(QString(tr("Cannot make observer (%1) the name manager of object (%2). This observer is currently the name manager for this object.")).arg(observer->observerName()).arg(obj->objectName()));
            return;
        } else {
            Observer* current_manager = OBJECT_MANAGER->observerReference(current_manager_id.value().toInt());
            Q_ASSERT(current_manager);
            NamingPolicyFilter* naming_filter = 0;
            for (int i = 0; i < current_manager->subjectFilters().count(); ++i) {
                // Check if it is a naming policy subject filter
                naming_filter = qobject_cast<NamingPolicyFilter*> (current_manager->subjectFilters().at(i));
            }

            // Add it to the instance name list only if the current manager has a unique naming policy filter
            if (naming_filter) {
                if (naming_filter->uniquenessNamingPolicy() == ProhibitDuplicateNames) {
                    MultiContextProperty current_instance_names_property = ObjectManager::getMultiContextProperty(obj,qti_prop_ALIAS_MAP);
                    if (current_instance_names_property.isValid()) {
                        current_instance_names_property.addContext(QVariant(obj->objectName()),current_manager->observerID());
                        ObjectManager::setMultiContextProperty(obj,current_instance_names_property);
                    } else {
                        // We need to create the property and add it to the object
                        MultiContextProperty new_instance_names_property(qti_prop_ALIAS_MAP);
                        new_instance_names_property.addContext(QVariant(obj->objectName()),observer->observerID());
                        ObjectManager::setMultiContextProperty(obj,new_instance_names_property);
                    }
                }
            }
        }
    }

    // Set this naming policy filter as the new name manager
    QString new_managed_name;

    // If this filter has a unique policy, we need to get the new name from the instance name list and remove this context
    if (d->uniqueness_policy == ProhibitDuplicateNames) {
        MultiContextProperty current_instance_names_property = ObjectManager::getMultiContextProperty(obj,qti_prop_ALIAS_MAP);
        if (current_instance_names_property.isValid()) {
            new_managed_name = current_instance_names_property.value(observer->observerID()).toString();
            current_instance_names_property.removeContext(observer->observerID());
            ObjectManager::setMultiContextProperty(obj,current_instance_names_property);
        }
        obj->setObjectName(new_managed_name);
        observer->setMultiContextPropertyValue(obj,qti_prop_NAME,new_managed_name);
    }

    observer->setMultiContextPropertyValue(obj,qti_prop_NAME_MANAGER_ID,observer->observerID());
}

Qtilities::CoreGui::Interfaces::INamingPolicyDialog* Qtilities::CoreGui::NamingPolicyFilter::constructUserDialog() const {
    NamingPolicyInputDialog* name_dialog = new NamingPolicyInputDialog;
    return name_dialog;
}

void Qtilities::CoreGui::NamingPolicyFilter::startValidationCycle() {
    d->validation_cycle_active = true;
}

void Qtilities::CoreGui::NamingPolicyFilter::endValidationCycle() {
    d->validation_cycle_active = false;
    if (d->name_dialog)
        d->name_dialog->endValidationCycle();
}

bool Qtilities::CoreGui::NamingPolicyFilter::isValidationCycleActive() const {
    return d->validation_cycle_active;
}

bool Qtilities::CoreGui::NamingPolicyFilter::isModified() const {
    return d->is_modified;
}

void Qtilities::CoreGui::NamingPolicyFilter::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

void Qtilities::CoreGui::NamingPolicyFilter::assignNewNameManager(QObject* obj) {
    if (!obj)
        return;

    if (isObjectNameManager(obj)) {
        // Get the next available observer with a naming policy subject filter
        MultiContextProperty observer_list = ObjectManager::getMultiContextProperty(obj,qti_prop_OBSERVER_MAP);
        Observer* next_observer = 0;
        bool found = false;
        if (observer_list.isValid()) {
            QList<quint32> keys = observer_list.contextMap().keys();
            int count = keys.count();
            for (int i = 0; i < count; ++i) {
                if ((int) keys.at(i) != observer->observerID()) {
                    next_observer = OBJECT_MANAGER->observerReference(keys.at(i));
                    if (next_observer) {
                        for (int i = 0; i < next_observer->subjectFilters().count(); ++i) {
                            // Check if it is a naming policy subject filter
                            NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (next_observer->subjectFilters().at(i));
                            if (naming_filter) {
                                found = true;
                                // MOD, a quicker way might be: (But not tested)
                                // next_observer->setMultiContextPropertyValue(obj,qti_prop_NAME_MANAGER_ID,-1);
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
            obj->setProperty(qti_prop_ALIAS_MAP,QVariant());
            obj->setProperty(qti_prop_NAME_MANAGER_ID,QVariant());
            LOG_TRACE(QString(tr("The name manager (%1) of object (%2) is not observing this object any more. An alternative name manager could not be found. This object's name won't be managed until it is attached to a new observer with a naming policy subject filter.")).arg(observer->observerName()).arg(obj->objectName()));
        }
    }
}

bool Qtilities::CoreGui::NamingPolicyFilter::isObjectNameManager(QObject* obj) const {
    QVariant object_name_manager_variant = observer->getMultiContextPropertyValue(obj,qti_prop_NAME_MANAGER_ID);
    if (object_name_manager_variant.isValid()) {
        return (object_name_manager_variant.toInt() == observer->observerID());
    } else
        return false;
}

bool Qtilities::CoreGui::NamingPolicyFilter::isObjectNameDirty(QObject* obj) const {
    QString evaluation_name = observer->getMultiContextPropertyValue(obj,qti_prop_NAME).toString();
    QVariant observer_property = obj->property(qti_prop_NAME);
    if (evaluation_name == obj->objectName() || !(observer_property.isValid()))
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

void Qtilities::CoreGui::NamingPolicyFilter::setName(QObject* object, const QString& new_name) {
    if (!object)
        return;

    // Check if we must use the instance name or the object name.
    if (isObjectNameManager(object)) {
        // We use the qti_prop_NAME property:
        QVariant object_name_prop;
        object_name_prop = object->property(qti_prop_NAME);
        if (object_name_prop.isValid() && object_name_prop.canConvert<SharedProperty>()) {
            SharedProperty name_property(qti_prop_NAME,QVariant(new_name));
            QVariant property = qVariantFromValue(name_property);
            object->setProperty(qti_prop_NAME,QVariant(property));
        }
    } else {
        // We use the qti_prop_ALIAS_MAP property:
        QVariant instance_names_prop;
        instance_names_prop = object->property(qti_prop_ALIAS_MAP);
        if (instance_names_prop.isValid() && instance_names_prop.canConvert<MultiContextProperty>()) {
            MultiContextProperty new_instance_name = instance_names_prop.value<MultiContextProperty>();
            new_instance_name.setValue(QVariant(new_name),observer->observerID());
            object->setProperty(qti_prop_ALIAS_MAP,qVariantFromValue(new_instance_name));
        }
    }
}

QString Qtilities::CoreGui::NamingPolicyFilter::getName(QObject* object) {
    Q_ASSERT(observer);

    if (!object)
        return QString();

    return observer->subjectNameInContext(object);
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

Qtilities::CoreGui::NamingPolicyDelegate::NamingPolicyDelegate(QObject *parent) : QItemDelegate(parent) {
    d = new NamingPolicyDelegateData;
    d->observer = 0;
    d->naming_filter = 0;
    d->obj = 0;
}

Qtilities::CoreGui::NamingPolicyDelegate::~NamingPolicyDelegate() {
    delete d;
}

void Qtilities::CoreGui::NamingPolicyDelegate::setObserverContext(Observer* observer) {
    if (d->observer == observer)
        return;

    d->observer = observer;

    if (d->observer) {
        // Look which known subject filters are installed in this observer
        for (int i = 0; i < observer->subjectFilters().count(); ++i) {
            // Check if it is a naming policy subject filter
            NamingPolicyFilter* naming_filter = qobject_cast<NamingPolicyFilter*> (observer->subjectFilters().at(i));
            if (naming_filter) {
                d->naming_filter = naming_filter;
                return;
            }
        }
    }

    d->naming_filter = 0;
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

            NamingPolicyFilter::NameValidity validity_result = d->naming_filter->evaluateName(text,d->obj);
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

QDataStream & operator<< (QDataStream& stream, const Qtilities::CoreGui::NamingPolicyFilter& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::CoreGui::NamingPolicyFilter& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}
