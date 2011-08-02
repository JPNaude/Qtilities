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

#include "ActivityPolicyFilter.h"
#include "QtilitiesCoreConstants.h"
#include "Observer.h"
#include "QtilitiesPropertyChangeEvent.h"

#include <Logger.h>

#include <QVariant>
#include <QCoreApplication>
#include <QDomElement>

using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core::Constants;

namespace Qtilities {
    namespace Core {
        FactoryItem<QObject, ActivityPolicyFilter> ActivityPolicyFilter::factory;
    }
}

struct Qtilities::Core::ActivityPolicyFilterPrivateData {
    ActivityPolicyFilterPrivateData() : is_modified(false) { }

    bool is_modified;
    ActivityPolicyFilter::ActivityPolicy            activity_policy;
    ActivityPolicyFilter::MinimumActivityPolicy     minimum_activity_policy;
    ActivityPolicyFilter::NewSubjectActivityPolicy  new_subject_activity_policy;
    ActivityPolicyFilter::ParentTrackingPolicy      parent_tracking_policy;
};

Qtilities::Core::ActivityPolicyFilter::ActivityPolicyFilter(QObject* parent) : AbstractSubjectFilter(parent) {
    d = new ActivityPolicyFilterPrivateData;
    d->activity_policy = ActivityPolicyFilter::MultipleActivity;
    d->minimum_activity_policy = ActivityPolicyFilter::AllowNoneActive;
    d->new_subject_activity_policy = ActivityPolicyFilter::SetNewInactive;
    d->parent_tracking_policy = ActivityPolicyFilter::ParentIgnoreActivity;
}

Qtilities::Core::ActivityPolicyFilter::~ActivityPolicyFilter() {
    delete d;
}

void Qtilities::Core::ActivityPolicyFilter::operator=(const ActivityPolicyFilter& ref) {
    d->activity_policy = ref.activityPolicy();
    d->minimum_activity_policy = ref.minimumActivityPolicy();
    d->new_subject_activity_policy = ref.newSubjectActivityPolicy();
    d->parent_tracking_policy = ref.parentTrackingPolicy();
    filter_is_modification_state_monitored = ref.isModificationStateMonitored();
}

bool Qtilities::Core::ActivityPolicyFilter::operator==(const ActivityPolicyFilter& ref) const {
    if (d->activity_policy != ref.activityPolicy())
        return false;
    if (d->minimum_activity_policy != ref.minimumActivityPolicy())
        return false;
    if (d->new_subject_activity_policy != ref.newSubjectActivityPolicy())
        return false;
    if (d->parent_tracking_policy != ref.parentTrackingPolicy())
        return false;
    if (filter_is_modification_state_monitored != ref.isModificationStateMonitored())
        return false;

    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::operator!=(const ActivityPolicyFilter& ref) const {
    return !(*this==ref);
}

QString Qtilities::Core::ActivityPolicyFilter::activityPolicyToString(ActivityPolicy activity_policy) {
    if (activity_policy == UniqueActivity) {
        return "UniqueActivity";
    } else if (activity_policy == MultipleActivity) {
        return "MultipleActivity";
    }

    return QString();
}

Qtilities::Core::ActivityPolicyFilter::ActivityPolicy Qtilities::Core::ActivityPolicyFilter::stringToActivityPolicy(const QString& activity_policy_string) {
    if (activity_policy_string == "UniqueActivity") {
        return UniqueActivity;
    } else if (activity_policy_string == "MultipleActivity") {
        return MultipleActivity;
    }
    Q_ASSERT(0);
    return UniqueActivity;
}

QString Qtilities::Core::ActivityPolicyFilter::parentTrackingPolicyToString(ParentTrackingPolicy parent_tracking_policy) {
    if (parent_tracking_policy == ParentIgnoreActivity) {
        return "ParentIgnoreActivity";
    } else if (parent_tracking_policy == ParentFollowActivity) {
        return "ParentFollowActivity";
    }

    return QString();
}

Qtilities::Core::ActivityPolicyFilter::ParentTrackingPolicy Qtilities::Core::ActivityPolicyFilter::stringToParentTrackingPolicy(const QString& parent_tracking_policy_string) {
    if (parent_tracking_policy_string == "ParentIgnoreActivity") {
        return ParentIgnoreActivity;
    } else if (parent_tracking_policy_string == "ParentFollowActivity") {
        return ParentFollowActivity;
    }
    Q_ASSERT(0);
    return ParentIgnoreActivity;
}

QString Qtilities::Core::ActivityPolicyFilter::minimumActivityPolicyToString(MinimumActivityPolicy minimum_activity_policy) {
    if (minimum_activity_policy == AllowNoneActive) {
        return "AllowNoneActive";
    } else if (minimum_activity_policy == ProhibitNoneActive) {
        return "ProhibitNoneActive";
    }

    return QString();
}

Qtilities::Core::ActivityPolicyFilter::MinimumActivityPolicy Qtilities::Core::ActivityPolicyFilter::stringToMinimumActivityPolicy(const QString& minimum_activity_policy_string) {
    if (minimum_activity_policy_string == "AllowNoneActive") {
        return AllowNoneActive;
    } else if (minimum_activity_policy_string == "ProhibitNoneActive") {
        return ProhibitNoneActive;
    }
    Q_ASSERT(0);
    return AllowNoneActive;
}

QString Qtilities::Core::ActivityPolicyFilter::newSubjectActivityPolicyToString(NewSubjectActivityPolicy new_subject_activity_policy) {
    if (new_subject_activity_policy == SetNewActive) {
        return "SetNewActive";
    } else if (new_subject_activity_policy == SetNewInactive) {
        return "SetNewInactive";
    }

    return QString();
}

Qtilities::Core::ActivityPolicyFilter::NewSubjectActivityPolicy Qtilities::Core::ActivityPolicyFilter::stringToNewSubjectActivityPolicy(const QString& new_subject_activity_policy_string) {
    if (new_subject_activity_policy_string == "SetNewActive") {
        return SetNewActive;
    } else if (new_subject_activity_policy_string == "SetNewInactive") {
        return SetNewInactive;
    }
    Q_ASSERT(0);
    return SetNewActive;
}

void Qtilities::Core::ActivityPolicyFilter::setActivityPolicy(ActivityPolicyFilter::ActivityPolicy activity_policy) {
    if (!observer) {
        d->activity_policy = activity_policy;
    } else {
        if (observer->subjectCount() == 0)
            d->activity_policy = activity_policy;
    }
}

Qtilities::Core::ActivityPolicyFilter::ActivityPolicy Qtilities::Core::ActivityPolicyFilter::activityPolicy() const {
    return d->activity_policy;
}

void Qtilities::Core::ActivityPolicyFilter::setParentTrackingPolicy(ActivityPolicyFilter::ParentTrackingPolicy parent_tracking_policy) {
    if (!observer) {
        d->parent_tracking_policy = parent_tracking_policy;
    } else {
        if (observer->subjectCount() == 0)
            d->parent_tracking_policy = parent_tracking_policy;
    }
}

Qtilities::Core::ActivityPolicyFilter::ActivityPolicyFilter::ParentTrackingPolicy Qtilities::Core::ActivityPolicyFilter::parentTrackingPolicy() const {
    return d->parent_tracking_policy;
}

void Qtilities::Core::ActivityPolicyFilter::setMinimumActivityPolicy(ActivityPolicyFilter::MinimumActivityPolicy minimum_activity_policy) {
    if (!observer) {
        d->minimum_activity_policy = minimum_activity_policy;
    } else {
        if (observer->subjectCount() == 0)
            d->minimum_activity_policy = minimum_activity_policy;
    }
}

Qtilities::Core::ActivityPolicyFilter::MinimumActivityPolicy Qtilities::Core::ActivityPolicyFilter::minimumActivityPolicy() const {
    return d->minimum_activity_policy;
}

void Qtilities::Core::ActivityPolicyFilter::setNewSubjectActivityPolicy(ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy) {  
    if (!observer) {
        d->new_subject_activity_policy = new_subject_activity_policy;
    } else {
        if (observer->subjectCount() == 0)
            d->new_subject_activity_policy = new_subject_activity_policy;
    }
}

Qtilities::Core::ActivityPolicyFilter::NewSubjectActivityPolicy Qtilities::Core::ActivityPolicyFilter::newSubjectActivityPolicy() const {
    return d->new_subject_activity_policy;
}

int Qtilities::Core::ActivityPolicyFilter::numActiveSubjects() const {
    int count = 0;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getQtilitiesPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP).toBool();
        if (is_active)
            ++count;
    }
    return count;
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::activeSubjects() const {
    QList<QObject*> list;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getQtilitiesPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP).toBool();
        if (is_active)
            list.push_back(observer->subjectAt(i));
    }
    return list;
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::inactiveSubjects() const {
    QList<QObject*> list;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getQtilitiesPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP).toBool();
        if (!is_active)
            list.push_back(observer->subjectAt(i));
    }
    return list;
}

QStringList Qtilities::Core::ActivityPolicyFilter::activeSubjectNames() const {

}

QStringList Qtilities::Core::ActivityPolicyFilter::inactiveSubjectNames() const {

}

bool Qtilities::Core::ActivityPolicyFilter::isModified() const {
    return d->is_modified;
}

void Qtilities::Core::ActivityPolicyFilter::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

void Qtilities::Core::ActivityPolicyFilter::setActiveSubjects(QList<QObject*> objects, bool broadcast) {
    if (!observer) {
        LOG_TRACE("Cannot set active objects in an activity subject filter without an observer context.");
        return;
    }

    // Make sure all objects in the list is observed by this observer context.
    for (int i = 0; i < objects.count(); i++) {
        if (!objects.at(i)) {
            LOG_TRACE(QString("Invalid objects in list sent to setActiveSubjects(). Null pointer to object detected at list position %1.").arg(i));
            return;
        }

        if (!observer->contains(objects.at(i)) && objects.at(i)) {
            LOG_TRACE(QString("Invalid objects in list sent to setActiveSubjects(). Object %1 is not observed in this context.").arg(objects.at(i)->objectName()));
            return;
        }
    }

    // Check the number of objects in the list against the policies of this filter.
    if (objects.count() == 0) {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive)
            return;
    }
    if (objects.count() > 1) {
        if (d->activity_policy == ActivityPolicyFilter::UniqueActivity)
            return;
    }

    // Now we know that the list is valid, lock the mutex so that property changes will be blocked.
    filter_mutex.tryLock();
    // Set all objects as inactive
    for (int i = 0; i < observer->subjectCount(); i++) {
        observer->setQtilitiesPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP,QVariant(false));
    }
    // Set objects in the list as active
    for (int i = 0; i < objects.count(); i++) {
        if (objects.at(i))
            observer->setQtilitiesPropertyValue(objects.at(i),qti_prop_ACTIVITY_MAP,QVariant(true));
    }
    filter_mutex.unlock();

    // We need to do some things here:
    // 1. If enabled, post the QtilitiesPropertyChangeEvent:
    if (observer->qtilitiesPropertyChangeEventsEnabled()) {
        for (int i = 0; i < observer->subjectCount(); i++) {
            if (observer->subjectAt(i)->thread() == thread()) {
                QByteArray property_name_byte_array = QByteArray(qti_prop_ACTIVITY_MAP);
                QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                QCoreApplication::postEvent(observer->subjectAt(i),user_event);
                #ifndef QT_NO_DEBUG
                    // Get activity of object for debugging purposes
                    QVariant activity = observer->getQtilitiesPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP);
                    if (activity.isValid()) {
                        if (activity.toBool())
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity true").arg(qti_prop_ACTIVITY_MAP).arg(observer->subjectAt(i)->objectName()));
                        else
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity false").arg(qti_prop_ACTIVITY_MAP).arg(observer->subjectAt(i)->objectName()));
                    }
                #endif
            }
        }
    }

    // 2. Emit the monitoredPropertyChanged() signal:
    QList<QObject*> changed_objects;
    changed_objects << observer->subjectReferences();
    emit monitoredPropertyChanged(qti_prop_ACTIVITY_MAP,changed_objects);

    if (broadcast) {
        // 3. Emit the activeSubjectsChanged() signal:
        emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

        // 4. Change the modification state of the filter:
        setModificationState(true);

        // 5. Emit the dataChanged() signal on the observer context:
        observer->refreshViewsData();
    } else {
        setModificationState(true,IModificationNotifier::NotifyNone);
    }
}

void Qtilities::Core::ActivityPolicyFilter::setActiveSubjects(QList<QPointer<QObject> > objects, bool broadcast) {
    QList<QObject*> simple_objects;
    for (int i = 0; i < objects.count(); i++)
        simple_objects << objects.at(i);
    setActiveSubjects(simple_objects,broadcast);
}

void Qtilities::Core::ActivityPolicyFilter::setActiveSubject(QObject* obj, bool broadcast) {
    QList<QObject*> objects;
    objects << obj;
    setActiveSubjects(objects,broadcast);
}

bool Qtilities::Core::ActivityPolicyFilter::initializeAttachment(QObject* obj, QString* rejectMsg, bool import_cycle) {
    Q_UNUSED(obj)
    Q_UNUSED(import_cycle)

    #ifndef QT_NO_DEBUG
    Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
    if (!obj) {
        if (rejectMsg)
            *rejectMsg = QString(tr("Actvity Policy Filter: Invalid object reference received. Attachment cannot be done."));
        return false;
    }
    #endif

    if (!observer) {
        if (rejectMsg)
            *rejectMsg = QString(tr("Actvity Policy Filter: Cannot evaluate an attachment in a subject filter without an observer context."));
        LOG_TRACE(QString(tr("Cannot evaluate an attachment in a subject filter without an observer context.")));
        return false;
    } else
        return true;
}

void Qtilities::Core::ActivityPolicyFilter::finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

    if (!observer) {
        LOG_TRACE(QString(tr("Cannot evaluate an attachment in a subject filter without an observer context.")));
        return;
    }

    if (!attachment_successful)
        return;

    if (!import_cycle) {
        // Ensure that property changes are not handled by the QDynamicPropertyChangeEvent handler.
        filter_mutex.tryLock();

        bool new_activity;
        // First determine the activity of the new subject
        // At this stage the object is not yet attached to the observer, thus dynamic property changes are not handled, we need
        // to do everyhing manually here.
        if (observer->subjectCount() == 1) {
            if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive || d->new_subject_activity_policy == ActivityPolicyFilter::SetNewActive) {
                new_activity = true;
            } else {
                new_activity = false;
            }
        } else {
            if (d->new_subject_activity_policy == ActivityPolicyFilter::SetNewActive) {
                if (d->activity_policy == ActivityPolicyFilter::UniqueActivity) {
                    for (int i = 0; i < observer->subjectCount(); i++) {
                        if (observer->subjectAt(i) != obj)
                            observer->setQtilitiesPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP,QVariant(false));
                    }
                }
                new_activity = true;
            } else {
                new_activity = false;
            }
        }

        MultiContextProperty subject_activity_property = observer->getMultiContextProperty(obj,qti_prop_ACTIVITY_MAP);
        if (subject_activity_property.isValid()) {
            // Thus, the property already exists
            subject_activity_property.addContext(QVariant(new_activity),observer->observerID());
            observer->setMultiContextProperty(obj,subject_activity_property);
        } else {
            // We need to create the property and add it to the object
            MultiContextProperty new_subject_activity_property(qti_prop_ACTIVITY_MAP);
            new_subject_activity_property.setIsExportable(true);
            new_subject_activity_property.addContext(QVariant(new_activity),observer->observerID());
            observer->setMultiContextProperty(obj,new_subject_activity_property);
        }

        if (new_activity) {
            // We need to do some things here:
            // 1. If enabled, post the QtilitiesPropertyChangeEvent:
            if (obj->thread() == thread()) {
                if (observer->qtilitiesPropertyChangeEventsEnabled()) {
                    QByteArray property_name_byte_array = QByteArray(qti_prop_ACTIVITY_MAP);
                    QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                    QCoreApplication::postEvent(obj,user_event);
                    #ifndef QT_NO_DEBUG
                        if (new_activity)
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity true").arg(qti_prop_ACTIVITY_MAP).arg(obj->objectName()));
                        else
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity false").arg(qti_prop_ACTIVITY_MAP).arg(obj->objectName()));
                    #endif
                }
            }

            // 2. Emit the monitoredPropertyChanged() signal:
            // Note that the object which is attached is not yet in the observer context, thus we must add it to the active subject list.
            QList<QObject*> changed_objects;
            changed_objects << observer->subjectReferences();
            changed_objects.push_back(obj);
            emit monitoredPropertyChanged(qti_prop_ACTIVITY_MAP,changed_objects);


            if (!observer->isProcessingCycleActive()) {
                // 3. Emit the activeSubjectsChanged() signal:
                emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

                // 4. Change the modification state of the filter:
                setModificationState(true);

                // 5. Emit the dataChanged() signal on the observer context:
                observer->refreshViewsData();
            } else {
                setModificationState(true,IModificationNotifier::NotifyNone);
            }
        }
        filter_mutex.unlock();
    }
}

void Qtilities::Core::ActivityPolicyFilter::finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

    if (!detachment_successful && !subject_deleted)
        return;

    // Ensure that property changes are not handled by the QDynamicPropertyChangeEvent handler.
    filter_mutex.tryLock();
    if (observer->subjectCount() >= 1) {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive) {
            // Check if this subject was active.
            bool is_active = observer->getQtilitiesPropertyValue(obj,qti_prop_ACTIVITY_MAP).toBool();
            if (is_active && (numActiveSubjects() == 0)) {
                // We need to set a different subject to be active.
                // Important bug fixed: In the case where a naming policy filter overwrites a conflicting
                // object during attachment, we might get here before the activity on the new object
                // which is replacing the conflicting object has been set. In that case, there is no qti_prop_ACTIVITY_MAP
                // property yet. Thus check it first:
                if (Observer::propertyExists(observer->subjectAt(0),qti_prop_ACTIVITY_MAP))
                    observer->setQtilitiesPropertyValue(observer->subjectAt(0),qti_prop_ACTIVITY_MAP, QVariant(true));
            }
        }

        if (!observer->isProcessingCycleActive())
            emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());
    }

    // Unlock the filter mutex.
    filter_mutex.unlock();
    if (!observer->isProcessingCycleActive())
        setModificationState(true);
    else
        setModificationState(true,IModificationNotifier::NotifyNone);
}

QStringList Qtilities::Core::ActivityPolicyFilter::monitoredProperties() const {
    QStringList reserved_properties;
    reserved_properties << QString(qti_prop_ACTIVITY_MAP);
    return reserved_properties;
}

bool Qtilities::Core::ActivityPolicyFilter::handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    Q_UNUSED(property_name)

    if (!filter_mutex.tryLock())
        return true;

    bool single_object_change_only = true;
    bool new_activity = observer->getQtilitiesPropertyValue(obj,qti_prop_ACTIVITY_MAP).toBool();
    if (new_activity) {
        if (d->activity_policy == ActivityPolicyFilter::UniqueActivity) {
            single_object_change_only = false;
            for (int i = 0; i < observer->subjectCount(); i++) {
                if (observer->subjectAt(i) != obj) {
                    observer->setQtilitiesPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP, QVariant(false));
                }
            }
        }
    } else {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive && (numActiveSubjects() == 0)) {
            // In this case, we allow the change to go through but we change the value here.
            observer->setQtilitiesPropertyValue(obj,qti_prop_ACTIVITY_MAP, QVariant(true));
        }
    }

    // We need to do some things here:
    // 1. If enabled, post the QtilitiesPropertyChangeEvent:
    if (obj->thread() == thread()) {
        if (observer->qtilitiesPropertyChangeEventsEnabled()) {
            QByteArray property_name_byte_array = QByteArray(propertyChangeEvent->propertyName().data());
            if (single_object_change_only) {
                QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                QCoreApplication::postEvent(obj,user_event);
                LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(obj->objectName()));
            } else {
                for (int i = 0; i < observer->subjectCount(); i++)    {
                    QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                    QCoreApplication::postEvent(observer->subjectAt(i),user_event);
                    LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2)").arg(QString(propertyChangeEvent->propertyName().data())).arg(observer->subjectAt(i)->objectName()));
                }
            }
        }
    }

    // 2. Emit the monitoredPropertyChanged() signal:
    QList<QObject*> changed_objects;
    if (single_object_change_only)
        changed_objects << obj;
    else
        changed_objects << observer->subjectReferences();
    emit monitoredPropertyChanged(propertyChangeEvent->propertyName(),changed_objects);

    // 3. Emit the activeSubjectsChanged() signal:
    emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

    // 4. Change the modification state of the filter and object:
    setModificationState(true);
    if (isModificationStateMonitored()) {
        IModificationNotifier* mod_notify = qobject_cast<IModificationNotifier*> (obj);
        if (mod_notify)
            mod_notify->setModificationState(true);
    }

    // 5. Emit the dataChanged() signal on the observer context:
    observer->refreshViewsData();

    filter_mutex.unlock();
    return false;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::ActivityPolicyFilter::instanceFactoryInfo() const {
    InstanceFactoryInfo instanceFactoryInfo(qti_def_FACTORY_QTILITIES,qti_def_FACTORY_TAG_ACTIVITY_FILTER,objectName());
    return instanceFactoryInfo;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::ActivityPolicyFilter::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::exportBinary(QDataStream& stream) const {
    stream << (quint32) d->activity_policy;
    stream << (quint32) d->minimum_activity_policy;
    stream << (quint32) d->new_subject_activity_policy;
    stream << (quint32) d->parent_tracking_policy;
    stream << filter_is_modification_state_monitored;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    quint32 ui32;
    stream >> ui32;
    d->activity_policy = (ActivityPolicy) ui32;
    stream >> ui32;
    d->minimum_activity_policy = (MinimumActivityPolicy) ui32;
    stream >> ui32;
    d->new_subject_activity_policy = (NewSubjectActivityPolicy) ui32;
    stream >> ui32;
    d->parent_tracking_policy = (ParentTrackingPolicy) ui32;
    stream >> filter_is_modification_state_monitored;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    object_node->setAttribute("ActivityPolicy",activityPolicyToString(d->activity_policy));
    object_node->setAttribute("MinimumActivityPolicy",minimumActivityPolicyToString(d->minimum_activity_policy));
    object_node->setAttribute("NewSubjectActivityPolicy",newSubjectActivityPolicyToString(d->new_subject_activity_policy));
    object_node->setAttribute("ParentTrackingPolicy",parentTrackingPolicyToString(d->parent_tracking_policy));
    if (!filter_is_modification_state_monitored)
        object_node->setAttribute("IsModificationStateMonitored","false");
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)  
    Q_UNUSED(import_list)

    if (object_node->hasAttribute("ActivityPolicy"))
        d->activity_policy = stringToActivityPolicy(object_node->attribute("ActivityPolicy"));
    if (object_node->hasAttribute("MinimumActivityPolicy"))
        d->minimum_activity_policy = stringToMinimumActivityPolicy(object_node->attribute("MinimumActivityPolicy"));
    if (object_node->hasAttribute("NewSubjectActivityPolicy"))
        d->new_subject_activity_policy = stringToNewSubjectActivityPolicy(object_node->attribute("NewSubjectActivityPolicy"));
    if (object_node->hasAttribute("ParentTrackingPolicy"))
        d->parent_tracking_policy = stringToParentTrackingPolicy(object_node->attribute("ParentTrackingPolicy"));
    if (object_node->hasAttribute("IsModificationStateMonitored")) {
        if (object_node->attribute("IsModificationStateMonitored") == "true")
            filter_is_modification_state_monitored = true;
        else
            filter_is_modification_state_monitored = false;
    }

    return IExportable::Complete;
}

bool Qtilities::Core::ActivityPolicyFilter::eventFilter(QObject *object, QEvent *event) {
    if (object == observer && event->type() == QEvent::User) {
        QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
        if (qtilities_event) {
            if (!qstrcmp(qtilities_event->propertyName().data(),qti_prop_ACTIVITY_MAP)) {
                // Now we need to check the following:
                // 1. observer can only have one parent.
                if (Observer::parentCount(observer) == 1) {
                    MultiContextProperty observer_property = Observer::getMultiContextProperty(observer,qti_prop_ACTIVITY_MAP);
                    if (observer_property.isValid()) {
                        if (observer_property.contextMap().count() > 0) {
                            bool activity = observer_property.contextMap().values().at(0).toBool();

                            if (activity) {
                                setActiveSubjects(observer->subjectReferences());
                            } else {
                                setActiveSubjects(QList<QObject*>());
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Qtilities::Core::ActivityPolicyFilter::setObserverContext(Observer* observer_context) {
    if (AbstractSubjectFilter::setObserverContext(observer_context)) {
        observer_context->installEventFilter(this);
        return true;
    } else
        return false;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::ActivityPolicyFilter& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::ActivityPolicyFilter& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}
