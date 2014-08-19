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
    ActivityPolicyFilterPrivateData() : is_modified(false),
        enforce_activity_policy(true),
        ignore_parent_tracking_changes(false),
        ignore_subject_tracking_changes(false) { }

    bool                                            is_modified;
    bool                                            enforce_activity_policy;
    bool                                            ignore_parent_tracking_changes;
    bool                                            ignore_subject_tracking_changes;
    ActivityPolicyFilter::ActivityPolicy            activity_policy;
    ActivityPolicyFilter::MinimumActivityPolicy     minimum_activity_policy;
    ActivityPolicyFilter::NewSubjectActivityPolicy  new_subject_activity_policy;
    ActivityPolicyFilter::ParentTrackingPolicy      parent_tracking_policy;
    QList<QPointer<QObject> >                       processing_cycle_start_active_subjects;
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

ActivityPolicyFilter& Qtilities::Core::ActivityPolicyFilter::operator=(const ActivityPolicyFilter& ref) {
    if (this==&ref) return *this;

    d->activity_policy = ref.activityPolicy();
    d->minimum_activity_policy = ref.minimumActivityPolicy();
    d->new_subject_activity_policy = ref.newSubjectActivityPolicy();
    d->parent_tracking_policy = ref.parentTrackingPolicy();
    filter_is_modification_state_monitored = ref.isModificationStateMonitored();

    return *this;
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
    if (activity_policy_string == QLatin1String("UniqueActivity")) {
        return UniqueActivity;
    } else if (activity_policy_string == QLatin1String("MultipleActivity")) {
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
    if (parent_tracking_policy_string == QLatin1String("ParentIgnoreActivity")) {
        return ParentIgnoreActivity;
    } else if (parent_tracking_policy_string == QLatin1String("ParentFollowActivity")) {
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
    if (minimum_activity_policy_string == QLatin1String("AllowNoneActive")) {
        return AllowNoneActive;
    } else if (minimum_activity_policy_string == QLatin1String("ProhibitNoneActive")) {
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
    if (new_subject_activity_policy_string == QLatin1String("SetNewActive")) {
        return SetNewActive;
    } else if (new_subject_activity_policy_string == QLatin1String("SetNewInactive")) {
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

Qtilities::Core::ActivityPolicyFilter::ParentTrackingPolicy Qtilities::Core::ActivityPolicyFilter::parentTrackingPolicy() const {
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
    d->new_subject_activity_policy = new_subject_activity_policy;
}

Qtilities::Core::ActivityPolicyFilter::NewSubjectActivityPolicy Qtilities::Core::ActivityPolicyFilter::newSubjectActivityPolicy() const {
    return d->new_subject_activity_policy;
}

int Qtilities::Core::ActivityPolicyFilter::numActiveSubjects() const {
    return activeSubjects().count();
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::activeSubjects() const {
    QList<QObject*> list;
    int count = observer->subjectCount();
    for (int i = 0; i < count; ++i) {
        QObject* obj = observer->subjectAt(i);
        if (observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP).toBool())
            list.push_back(obj);
    }
    return list;
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::inactiveSubjects() const {
    QList<QObject*> list;
    int count = observer->subjectCount();
    for (int i = 0; i < count; ++i) {
        QObject* obj = observer->subjectAt(i);
        if (!observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP).toBool())
            list.push_back(obj);
    }
    return list;
}

QStringList Qtilities::Core::ActivityPolicyFilter::activeSubjectNames() const {
    QList<QObject*> list = activeSubjects();
    QStringList names;
    foreach (QObject* obj, list) {
        names << observer->subjectNameInContext(obj);
    }
    return names;
}

QStringList Qtilities::Core::ActivityPolicyFilter::inactiveSubjectNames() const {
    QList<QObject*> list = inactiveSubjects();
    QStringList names;
    foreach (QObject* obj, list) {
        names << observer->subjectNameInContext(obj);
    }
    return names;
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

bool Qtilities::Core::ActivityPolicyFilter::setActiveSubjects(QList<QObject*> objects, bool broadcast) {
    if (!observer) {
        LOG_TRACE("Cannot set active objects in an activity subject filter without an observer context.");
        return false;
    }

    // Check if the new set of active subjects is the same as the current active objects, if so return:
    // TODO: Add this, but first fix problem where FollowSelection does not select the correct objects the first time this function is called.
    //    if (activeSubjects() == objects)
    //        return;

    // Make sure all objects in the list is observed by this observer context.
    // Skip this, we are taking very long in here...
//    for (int i = 0; i < objects.count(); ++i) {
//        if (!objects.at(i)) {
//            LOG_TRACE(QString("Invalid objects in list sent to setActiveSubjects(). Null pointer to object detected at list position %1.").arg(i));
//            return false;
//        }

//        if (!observer->contains(objects.at(i)) && objects.at(i)) {
//            LOG_TRACE(QString("Invalid objects in list sent to setActiveSubjects(). Object %1 is not observed in this context (%2).").arg(objects.at(i)->objectName()).arg(observer->observerName()));
//            return false;
//        }
//    }

    // Check the number of objects in the list against the policies of this filter.
    if (objects.count() == 0) {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive) {
            // If the observer context is still empty, this selection must be allowed:
            if (observerContext()->subjectCount() == 0)
                return true;
            else
                return false;
        }
    }
    if (objects.count() > 1) {
        if (d->activity_policy == ActivityPolicyFilter::UniqueActivity)
            return false;
    }

    // Now we know that the list is valid, lock the mutex so that property changes will be blocked.
    filter_mutex.tryLock();
    // Set all objects as inactive
    int subject_count = observer->subjectCount();
    for (int i = 0; i < subject_count; ++i)
        observer->setMultiContextPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP,QVariant(false));

    // Set objects in the list as active
    int objects_count = objects.count();
    for (int i = 0; i < objects_count; ++i)
        observer->setMultiContextPropertyValue(objects.at(i),qti_prop_ACTIVITY_MAP,QVariant(true));

    filter_mutex.unlock();

    // We need to do some things here:
    // - If enabled, post the QtilitiesPropertyChangeEvent:
    if (observer->qtilitiesPropertyChangeEventsEnabled()) {
        for (int i = 0; i < subject_count; ++i) {
            if (observer->subjectAt(i)->thread() == thread()) {
                QByteArray property_name_byte_array = QByteArray(qti_prop_ACTIVITY_MAP);
                QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                QCoreApplication::postEvent(observer->subjectAt(i),user_event);
                #ifndef QT_NO_DEBUG
                    // Get activity of object for debugging purposes
                    QVariant activity = observer->getMultiContextPropertyValue(observer->subjectAt(i),qti_prop_ACTIVITY_MAP);
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

    // - Emit the monitoredPropertyChanged() signal:
    QList<QObject*> changed_objects;
    changed_objects << observer->subjectReferences();
    emit monitoredPropertyChanged(qti_prop_ACTIVITY_MAP,changed_objects); 

    if (broadcast && !observer->isProcessingCycleActive()) {
        // - Emit the activeSubjectsChanged() signal:
        emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

        // - Change the modification state of the filter:
        setModificationState(true);

        // - Emit the dataChanged() signal on the observer context:
        observer->refreshViewsData();
    } else
        setModificationState(true,IModificationNotifier::NotifyNone);

    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::setActiveSubjects(QList<QPointer<QObject> > objects, bool broadcast) {
    QList<QObject*> simple_objects;
    int count = objects.count();
    for (int i = 0; i < count; ++i)
        simple_objects << objects.at(i);
    return setActiveSubjects(simple_objects,broadcast);
}

bool Qtilities::Core::ActivityPolicyFilter::setActiveSubject(QObject* obj, bool broadcast) {
    QList<QObject*> objects;
    objects << obj;
    return setActiveSubjects(objects,broadcast);
}

bool Qtilities::Core::ActivityPolicyFilter::toggleSubjectActivity(QObject* obj) {
    // Get the activity within this context:
    QVariant activity = observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP);
    if (activity.isValid()) {
        // The property change will be picked up and all policies will be checked there.
        return observer->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP,!activity.toBool());
    } else
        return false;
}

bool Qtilities::Core::ActivityPolicyFilter::setSubjectActivity(QObject* obj, bool is_active) {
    return observer->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP,is_active);
}

bool Qtilities::Core::ActivityPolicyFilter::getSubjectActivity(const QObject* obj, bool* ok) const {
    if (ok) {
        QVariant activity = observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP);
        if (activity.isValid()) {
            *ok = true;
            return activity.toBool();
        } else {
            *ok = false;
            return false;
        }
    } else {
        QVariant activity = observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP);
        if (activity.isValid()) {
            return activity.toBool();
        } else
            return false;
    }
}

bool Qtilities::Core::ActivityPolicyFilter::invertActivity() {
    if (!canInvertActivity())
        return false;

    // Now do the inversion:
    setActiveSubjects(inactiveSubjects());
    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::setAllActive() {
    if (!canSetAllActive())
        return false;

    // Now do the inversion:
    setActiveSubjects(observer->subjectReferences());
    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::setNoneActive() {
    if (!canSetNoneActive())
        return false;

    // Now do the inversion:
    setActiveSubjects(QList<QObject*>());
    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::canInvertActivity() const {
    // Check requirements:
    if (!(d->minimum_activity_policy == AllowNoneActive))
        return false;
    if (!(d->activity_policy == MultipleActivity))
        return false;
    if (!(d->parent_tracking_policy == ParentIgnoreActivity))
        return false;

    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::canSetAllActive() const {
    // Check requirements:
    if (!(d->activity_policy == MultipleActivity))
        return false;

    return true;
}

bool Qtilities::Core::ActivityPolicyFilter::canSetNoneActive() const {
    // Check requirements:
    if (!(d->minimum_activity_policy == AllowNoneActive))
        return false;

    return true;
}

void ActivityPolicyFilter::handleProcessingCycleStarted() {
    d->processing_cycle_start_active_subjects = ObjectManager::convNormalObjectsToSafe(activeSubjects());
}

void ActivityPolicyFilter::handleProcessingCycleEnded() {
    QList<QObject*> previous_active_subjects = ObjectManager::convSafeObjectsToNormal(d->processing_cycle_start_active_subjects);
    if (previous_active_subjects != activeSubjects()) {
        emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());
        setModificationState(true,IModificationNotifier::NotifyListeners,true);
        observer->setModificationState(true,IModificationNotifier::NotifyListeners,true);
    }
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

        bool new_activity = true;
        // First determine the activity of the new subject
        // At this stage the object is not yet attached to the observer, thus dynamic property changes are not handled, we need
        // to do everyhing manually here.
        int subject_count = observer->subjectCount();
        if (subject_count == 1) {
            if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive || d->new_subject_activity_policy == ActivityPolicyFilter::SetNewActive) {
                new_activity = true;
            } else {
                new_activity = false;
            }
        } else {
            if (d->new_subject_activity_policy == ActivityPolicyFilter::SetNewActive) {
                if (d->activity_policy == ActivityPolicyFilter::UniqueActivity && d->enforce_activity_policy) {
                    for (int i = 0; i < subject_count; ++i) {
                        QObject* obj_at = observer->subjectAt(i);
                        if (obj_at != obj)
                            observer->setMultiContextPropertyValue(obj_at,qti_prop_ACTIVITY_MAP,QVariant(false));
                    }
                }
                new_activity = true;
            } else {
                new_activity = false;
            }
        }

        bool current_subject_event_filter = observer->subjectEventFilteringEnabled();
        observer->toggleSubjectEventFiltering(false);
        MultiContextProperty subject_activity_property = ObjectManager::getMultiContextProperty(obj,qti_prop_ACTIVITY_MAP);
        if (subject_activity_property.isValid()) {
            // Thus, the property already exists
            subject_activity_property.addContext(new_activity,observer->observerID());
            ObjectManager::setMultiContextProperty(obj,subject_activity_property);
        } else {
            // We need to create the property and add it to the object
            MultiContextProperty new_subject_activity_property(qti_prop_ACTIVITY_MAP);
            new_subject_activity_property.setIsExportable(true);
            new_subject_activity_property.addContext(new_activity,observer->observerID());         
            ObjectManager::setMultiContextProperty(obj,new_subject_activity_property);
        }
        observer->toggleSubjectEventFiltering(current_subject_event_filter);

        // When tracking parent activity, we need to listen to activity changes on the subjects
        // in order to make parent partially checked if needed to:
        if (d->parent_tracking_policy == ActivityPolicyFilter::ParentFollowActivity) {
            obj->installEventFilter(this);
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

            if (!observer->isProcessingCycleActive()) {
                // 2. Emit the monitoredPropertyChanged() signal:
                // Note that the object which is attached is not yet in the observer context, thus we must add it to the active subject list.
                QList<QObject*> changed_objects;
                changed_objects << observer->subjectReferences();
                changed_objects.push_back(obj);
                emit monitoredPropertyChanged(qti_prop_ACTIVITY_MAP,changed_objects);

                // 3. Emit the activeSubjectsChanged() signal:
                emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

                // 4. Change the modification state of the filter:
                setModificationState(true);

                // 5. We don't emit the dataChanged() signal on the observer context.
                //    It will be done automatically in the observer's attach function
                //    using layoutChanged().
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
    bool set_0_index_active = false;
    int subject_count = observer->subjectCount();
    if (subject_count >= 1) {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive) {
            // Check if this subject was active.
            bool is_active = observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP).toBool();
            if (is_active && (numActiveSubjects() == 0)) {
                // We need to set a different subject to be active.
                // Important bug fixed: In the case where a naming policy filter overwrites a conflicting
                // object during attachment, we might get here before the activity on the new object
                // which is replacing the conflicting object has been set. In that case, there is no qti_prop_ACTIVITY_MAP
                // property yet. Thus check it first:
                if (ObjectManager::propertyExists(observer->subjectAt(0),qti_prop_ACTIVITY_MAP))
                    set_0_index_active = true;
            }
        }
    }

    // Unlock the filter mutex.
    filter_mutex.unlock();

    if (set_0_index_active) {
        // Do this after the mutex was unlocked in order for FollowSelection views to update properly:
        // TODO: This still breaks... Why does FollowSelection not work when first object in list is deleted?
        setActiveSubject(observer->subjectAt(0));
    }

    if (!observer->isProcessingCycleActive()) {
        if (subject_count >= 1)
            emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());
        else
            emit activeSubjectsChanged(QList<QObject*>(),QList<QObject*>());
         setModificationState(true);
    } else
        setModificationState(true,IModificationNotifier::NotifyNone);
}

QStringList Qtilities::Core::ActivityPolicyFilter::monitoredProperties() const {
    QStringList reserved_properties;
    reserved_properties << QString(qti_prop_ACTIVITY_MAP);
    return reserved_properties;
}

bool Qtilities::Core::ActivityPolicyFilter::handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    Q_UNUSED(property_name)

    QTime time;
    time.start();

    if (!d->enforce_activity_policy)
        return true;

    if (!filter_mutex.tryLock())
        return false;

    bool single_object_change_only = true;
    int subject_count = observer->subjectCount();
    bool new_activity = observer->getMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP).toBool();
    if (new_activity) {
        if (d->activity_policy == ActivityPolicyFilter::UniqueActivity) {
            single_object_change_only = false;
            for (int i = 0; i < subject_count; ++i) {
                QObject* current_obj = observer->subjectAt(i);
                if (current_obj != obj)
                    observer->setMultiContextPropertyValue(current_obj,qti_prop_ACTIVITY_MAP, QVariant(false));
            }
        }
    } else {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive && (numActiveSubjects() == 0)) {
            // In this case, we allow the change to go through but we change the value here.
            observer->setMultiContextPropertyValue(obj,qti_prop_ACTIVITY_MAP, QVariant(true));
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
                for (int i = 0; i < subject_count; ++i)    {
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

    // 3. Change the modification state of the filter and object:
    setModificationState(true);
    if (isModificationStateMonitored()) {
        IModificationNotifier* mod_notify = qobject_cast<IModificationNotifier*> (obj);
        if (mod_notify)
            mod_notify->setModificationState(true);
    }

    if (!observer->isProcessingCycleActive()) {
        // 4. Emit the activeSubjectsChanged() signal:
        emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

        // 5. Emit the dataChanged() signal on the observer context:
        observer->refreshViewsData();
    }

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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ActivityPolicyFilter::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << (quint32) d->activity_policy;
    stream << (quint32) d->minimum_activity_policy;
    stream << (quint32) d->new_subject_activity_policy;
    stream << (quint32) d->parent_tracking_policy;
    stream << filter_is_modification_state_monitored;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ActivityPolicyFilter::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ActivityPolicyFilter::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    object_node->setAttribute("ActivityPolicy",activityPolicyToString(d->activity_policy));
    object_node->setAttribute("MinimumActivityPolicy",minimumActivityPolicyToString(d->minimum_activity_policy));
    object_node->setAttribute("NewSubjectActivityPolicy",newSubjectActivityPolicyToString(d->new_subject_activity_policy));
    object_node->setAttribute("ParentTrackingPolicy",parentTrackingPolicyToString(d->parent_tracking_policy));
    if (!filter_is_modification_state_monitored)
        object_node->setAttribute("IsModificationStateMonitored","false");
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::ActivityPolicyFilter::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (object_node->hasAttribute("ActivityPolicy"))
        d->activity_policy = stringToActivityPolicy(object_node->attribute("ActivityPolicy"));
    if (object_node->hasAttribute("MinimumActivityPolicy"))
        d->minimum_activity_policy = stringToMinimumActivityPolicy(object_node->attribute("MinimumActivityPolicy"));
    if (object_node->hasAttribute("NewSubjectActivityPolicy"))
        d->new_subject_activity_policy = stringToNewSubjectActivityPolicy(object_node->attribute("NewSubjectActivityPolicy"));
    if (object_node->hasAttribute("ParentTrackingPolicy"))
        d->parent_tracking_policy = stringToParentTrackingPolicy(object_node->attribute("ParentTrackingPolicy"));
    if (object_node->hasAttribute("IsModificationStateMonitored")) {
        if (object_node->attribute("IsModificationStateMonitored") == QLatin1String("true"))
            filter_is_modification_state_monitored = true;
        else
            filter_is_modification_state_monitored = false;
    }

    return IExportable::Complete;
}

bool Qtilities::Core::ActivityPolicyFilter::eventFilter(QObject *object, QEvent *event) {
    if (!observer)
        return false;

    if (object == observer && event->type() == QEvent::User) {
        // TODO: We get in here twice, which means views will also be updated twice...
        QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
        if (qtilities_event) {
            if (!qstrcmp(qtilities_event->propertyName().data(),qti_prop_ACTIVITY_MAP)) {
                if (d->parent_tracking_policy == ActivityPolicyFilter::ParentFollowActivity && !d->ignore_parent_tracking_changes) {
                    // Now we need to check the following:
                    // 1. Observer can only have one parent.
                    if (Observer::parentCount(observer) == 1) {
                        MultiContextProperty observer_property = ObjectManager::getMultiContextProperty(observer,qti_prop_ACTIVITY_MAP);
                        if (observer_property.isValid()) {
                            if (observer_property.contextMap().count() > 0) {
                                bool activity = observer_property.contextMap().values().at(0).toBool();

                                d->ignore_subject_tracking_changes = true;
                                //qDebug() << "setting activity on subjects" << observer;
                                if (activity) {
                                    setActiveSubjects(observer->subjectReferences());
                                } else {
                                    setActiveSubjects(QList<QObject*>());
                                }
                                d->ignore_subject_tracking_changes = false;
                            }
                        }
                    }
                }
            }
        }
    }/* else if (observer->contains(object) && event->type() == QEvent::User) {
        QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
        if (qtilities_event) {
            if (!qstrcmp(qtilities_event->propertyName().data(),qti_prop_ACTIVITY_MAP)) {
                if (d->parent_tracking_policy == ActivityPolicyFilter::ParentFollowActivity && !d->ignore_subject_tracking_changes) {
                    // Check if partial subjects are active:
                    if (Observer::parentCount(object) == 1) {
                        //qDebug() << "## 1" << object << d->ignore_subject_tracking_changes;
                        MultiContextProperty observer_property = ObjectManager::getMultiContextProperty(object,qti_prop_ACTIVITY_MAP);
                        if (observer_property.isValid()) {
                            //qDebug() << "## 2";
                            if (observer_property.contextMap().count() > 0) {
                               // qDebug() << "## 3";
                                bool activity = observer_property.value(observer->observerID()).toBool();
                                // Only when its inactive we must set the parent to be partially active.
                                if (!activity) {
                                    d->ignore_parent_tracking_changes = true;
                                    // Needs to be done
                                    // TODO: Fix, we don't get in here for sub node 1 in Parent Tracking Activity tree example...
                                    qDebug() << Q_FUNC_INFO <<  ": Need to set parent as partially active still";
                                    d->ignore_parent_tracking_changes = false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }*/
    return false;
}

bool Qtilities::Core::ActivityPolicyFilter::setObserverContext(Observer* observer_context) {
    if (observerContext())
        observerContext()->disconnect(this);

    if (AbstractSubjectFilter::setObserverContext(observer_context)) {
        observer_context->installEventFilter(this);
        connect(observer_context,SIGNAL(processingCycleStarted()),SLOT(handleProcessingCycleStarted()));
        connect(observer_context,SIGNAL(processingCycleEnded()),SLOT(handleProcessingCycleEnded()));
        return true;
    } else
        return false;
}

void Qtilities::Core::ActivityPolicyFilter::disableActivityPolicyEnforcement() {
    d->enforce_activity_policy = false;
}

void Qtilities::Core::ActivityPolicyFilter::enableActivityPolicyEnforcement() {
    d->enforce_activity_policy = true;
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
