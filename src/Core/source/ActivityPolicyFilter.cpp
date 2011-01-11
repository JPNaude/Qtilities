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

struct Qtilities::Core::ActivityPolicyFilterData {
    ActivityPolicyFilterData() : is_modified(false),
    is_exportable(false),
    is_modification_state_monitored(true) { }

    bool is_modified;
    bool is_exportable;
    bool is_modification_state_monitored;
    ActivityPolicyFilter::ActivityPolicy            activity_policy;
    ActivityPolicyFilter::MinimumActivityPolicy     minimum_activity_policy;
    ActivityPolicyFilter::NewSubjectActivityPolicy  new_subject_activity_policy;
    ActivityPolicyFilter::ParentTrackingPolicy      parent_tracking_policy;
};

Qtilities::Core::ActivityPolicyFilter::ActivityPolicyFilter(QObject* parent) : AbstractSubjectFilter(parent) {
    d = new ActivityPolicyFilterData;
    d->activity_policy = ActivityPolicyFilter::MultipleActivity;
    d->minimum_activity_policy = ActivityPolicyFilter::AllowNoneActive;
    d->new_subject_activity_policy = ActivityPolicyFilter::SetNewInactive;
    d->parent_tracking_policy = ActivityPolicyFilter::ParentIgnoreActivity;
}

Qtilities::Core::ActivityPolicyFilter::~ActivityPolicyFilter() {
    delete d;
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
        is_active = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY).toBool();
        if (is_active)
            ++count;
    }
    return count;
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::activeSubjects() const {
    QList<QObject*> list;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY).toBool();
        if (is_active)
            list.push_back(observer->subjectAt(i));
    }
    return list;
}

QList<QObject*> Qtilities::Core::ActivityPolicyFilter::inactiveSubjects() const {
    QList<QObject*> list;
    bool is_active = false;
    for (int i = 0; i < observer->subjectCount(); i++) {
        is_active = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY).toBool();
        if (!is_active)
            list.push_back(observer->subjectAt(i));
    }
    return list;
}

bool Qtilities::Core::ActivityPolicyFilter::isModified() const {
    return d->is_modified;
}

void Qtilities::Core::ActivityPolicyFilter::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets) {
    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

void Qtilities::Core::ActivityPolicyFilter::setActiveSubjects(QList<QObject*> objects) {
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

        if (!observer->contains(objects.at(i))) {
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

    // Now we know that the list is valid, lock the mutex
    filter_mutex.tryLock();
    // Set all objects as inactive
    for (int i = 0; i < observer->subjectCount(); i++) {
        observer->setObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY,QVariant(false));
    }
    // Set objects in the list as active
    for (int i = 0; i < objects.count(); i++) {
        observer->setObserverPropertyValue(objects.at(i),OBJECT_ACTIVITY,QVariant(true));
    }
    filter_mutex.unlock();

    // We need to do some things here:
    // 1. If enabled, post the QtilitiesPropertyChangeEvent:
    if (observer->qtilitiesPropertyChangeEventsEnabled()) {
        for (int i = 0; i < observer->subjectCount(); i++) {
            if (observer->subjectAt(i)->thread() == thread()) {
                QByteArray property_name_byte_array = QByteArray(OBJECT_ACTIVITY);
                QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                QCoreApplication::postEvent(observer->subjectAt(i),user_event);
                #ifndef QT_NO_DEBUG
                    // Get activity of object for debugging purposes
                    QVariant activity = observer->getObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY);
                    if (activity.isValid()) {
                        if (activity.toBool())
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity true").arg(OBJECT_ACTIVITY).arg(observer->subjectAt(i)->objectName()));
                        else
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity false").arg(OBJECT_ACTIVITY).arg(observer->subjectAt(i)->objectName()));
                    }
                #endif
            }
        }
    }

    // 2. Emit the monitoredPropertyChanged() signal:
    QList<QObject*> changed_objects;
    changed_objects << observer->subjectReferences();
    emit monitoredPropertyChanged(OBJECT_ACTIVITY,changed_objects);

    // 3. Emit the activeSubjectsChanged() signal:
    emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

    // 4. Change the modification state of the filter:
    setModificationState(true);

    // 5. Emit the dataChanged() signal on the observer context:
    observer->refreshViewsData();
}

void Qtilities::Core::ActivityPolicyFilter::setActiveSubjects(QList<QPointer<QObject> > objects) {
    QList<QObject*> simple_objects;
    for (int i = 0; i < objects.count(); i++)
        simple_objects << objects.at(i);
    setActiveSubjects(simple_objects);
}

void Qtilities::Core::ActivityPolicyFilter::setActiveSubject(QObject* obj) {
    QList<QObject*> objects;
    objects << obj;
    setActiveSubjects(objects);
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::ActivityPolicyFilter::evaluateAttachment(QObject* obj, QString* rejectMsg, bool silent) const {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)
    Q_UNUSED(silent)

    return AbstractSubjectFilter::Allowed;
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
                            observer->setObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY,QVariant(false));
                    }
                }
                new_activity = true;
            } else {
                new_activity = false;
            }
        }

        ObserverProperty subject_activity_property = observer->getObserverProperty(obj,OBJECT_ACTIVITY);
        if (subject_activity_property.isValid()) {
            // Thus, the property already exists
            subject_activity_property.addContext(QVariant(new_activity),observer->observerID());
            observer->setObserverProperty(obj,subject_activity_property);
        } else {
            // We need to create the property and add it to the object
            ObserverProperty new_subject_activity_property(OBJECT_ACTIVITY);
            new_subject_activity_property.setIsExportable(true);
            new_subject_activity_property.addContext(QVariant(new_activity),observer->observerID());
            observer->setObserverProperty(obj,new_subject_activity_property);
        }

        if (new_activity) {
            // We need to do some things here:
            // 1. If enabled, post the QtilitiesPropertyChangeEvent:
            if (obj->thread() == thread()) {
                if (observer->qtilitiesPropertyChangeEventsEnabled()) {
                    QByteArray property_name_byte_array = QByteArray(OBJECT_ACTIVITY);
                    QtilitiesPropertyChangeEvent* user_event = new QtilitiesPropertyChangeEvent(property_name_byte_array,observer->observerID());
                    QCoreApplication::postEvent(obj,user_event);
                    #ifndef QT_NO_DEBUG
                        if (new_activity)
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity true").arg(OBJECT_ACTIVITY).arg(obj->objectName()));
                        else
                            LOG_TRACE(QString("Posting QtilitiesPropertyChangeEvent (property: %1) to object (%2) with activity false").arg(OBJECT_ACTIVITY).arg(obj->objectName()));
                    #endif
                }
            }

            // 2. Emit the monitoredPropertyChanged() signal:
            // Note that the object which is attached is not yet in the observer context, thus we must add it to the active subject list.
            QList<QObject*> changed_objects;
            changed_objects << observer->subjectReferences();
            changed_objects.push_back(obj);
            emit monitoredPropertyChanged(OBJECT_ACTIVITY,changed_objects);

            // 3. Emit the activeSubjectsChanged() signal:
            emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());

            // 4. Change the modification state of the filter:
            setModificationState(true);

            // 5. Emit the dataChanged() signal on the observer context:
            observer->refreshViewsData();
        }
        filter_mutex.unlock();
    }
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::ActivityPolicyFilter::evaluateDetachment(QObject* obj, QString* rejectMsg) const {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)

    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::ActivityPolicyFilter::initializeDetachment(QObject* obj, QString* rejectMsg, bool subject_deleted) {
    Q_UNUSED(obj)
    Q_UNUSED(subject_deleted)
    Q_UNUSED(rejectMsg)

    return true;
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

    QString name = observer->observerName();
    QList<QObject*> list = observer->subjectReferences();
    if (observer->subjectCount() >= 1) {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive) {
            // Check if this subject was active.
            bool is_active = observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY).toBool();
            if (is_active && (numActiveSubjects() == 0)) {
                // We need to set a different subject to be active.
                // Important bug fixed: In the case where a naming policy filter overwrites a conflicting
                // object during attachment, we might get here before the activity on the new object
                // which is replacing the conflicting object has been set. In that case, there is no OBJECT_ACTIVITY
                // property yet. Thus check it first:
                if (Observer::propertyExists(observer->subjectAt(0),OBJECT_ACTIVITY))
                    observer->setObserverPropertyValue(observer->subjectAt(0),OBJECT_ACTIVITY, QVariant(true));
            }
        }

        emit activeSubjectsChanged(activeSubjects(),inactiveSubjects());
    }

    // Unlock the filter mutex.
    filter_mutex.unlock();
    setModificationState(true);
}

void Qtilities::Core::ActivityPolicyFilter::setIsModificationStateMonitored(bool is_monitored) {
    d->is_modification_state_monitored = is_monitored;
}

bool Qtilities::Core::ActivityPolicyFilter::isModificationStateMonitored() const {
    return d->is_modification_state_monitored;
}

void Qtilities::Core::ActivityPolicyFilter::setIsExportable(bool is_exportable) {
    d->is_exportable = is_exportable;
}

bool Qtilities::Core::ActivityPolicyFilter::isExportable() const {
    return d->is_exportable;
}

QStringList Qtilities::Core::ActivityPolicyFilter::monitoredProperties() const {
    QStringList reserved_properties;
    reserved_properties << QString(OBJECT_ACTIVITY);
    return reserved_properties;
}

QStringList Qtilities::Core::ActivityPolicyFilter::reservedProperties() const {
    return QStringList();
}

bool Qtilities::Core::ActivityPolicyFilter::handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    Q_UNUSED(property_name)

    if (!filter_mutex.tryLock())
        return true;

    bool single_object_change_only = true;
    bool new_activity = observer->getObserverPropertyValue(obj,OBJECT_ACTIVITY).toBool();
    if (new_activity) {
        if (d->activity_policy == ActivityPolicyFilter::UniqueActivity) {
            single_object_change_only = false;
            for (int i = 0; i < observer->subjectCount(); i++) {
                if (observer->subjectAt(i) != obj) {
                    observer->setObserverPropertyValue(observer->subjectAt(i),OBJECT_ACTIVITY, QVariant(false));
                }
            }
        }
    } else {
        if (d->minimum_activity_policy == ActivityPolicyFilter::ProhibitNoneActive && (numActiveSubjects() == 0)) {
            // In this case, we allow the change to go through but we change the value here.
            observer->setObserverPropertyValue(obj,OBJECT_ACTIVITY, QVariant(true));
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

    // 4. Change the modification state of the filter:
    setModificationState(true);

    // 5. Emit the dataChanged() signal on the observer context:
    observer->refreshViewsData();

    filter_mutex.unlock();
    return false;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::ActivityPolicyFilter::instanceFactoryInfo() const {
    InstanceFactoryInfo instanceFactoryInfo(FACTORY_QTILITIES,FACTORY_TAG_ACTIVITY_POLICY_FILTER,FACTORY_TAG_ACTIVITY_POLICY_FILTER);
    return instanceFactoryInfo;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::ActivityPolicyFilter::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    Q_UNUSED(params)

    InstanceFactoryInfo factory_data = instanceFactoryInfo();
    factory_data.exportBinary(stream);

    stream << (quint32) d->activity_policy;
    stream << (quint32) d->minimum_activity_policy;
    stream << (quint32) d->new_subject_activity_policy;
    stream << (quint32) d->parent_tracking_policy;
    stream << (quint32) d->is_modification_state_monitored;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(import_list)
    Q_UNUSED(params)

    quint32 ui32;
    stream >> ui32;
    d->activity_policy = (ActivityPolicy) ui32;
    stream >> ui32;
    d->minimum_activity_policy = (MinimumActivityPolicy) ui32;
    stream >> ui32;
    d->new_subject_activity_policy = (NewSubjectActivityPolicy) ui32;
    stream >> ui32;
    d->parent_tracking_policy = (ParentTrackingPolicy) ui32;
    stream >> ui32;
    d->is_modification_state_monitored = ui32;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    QDomElement filter_data = doc->createElement("Data");
    object_node->appendChild(filter_data);
    filter_data.setAttribute("ActivityPolicy",activityPolicyToString(d->activity_policy));
    filter_data.setAttribute("MinimumActivityPolicy",minimumActivityPolicyToString(d->minimum_activity_policy));
    filter_data.setAttribute("NewSubjectActivityPolicy",newSubjectActivityPolicyToString(d->new_subject_activity_policy));
    filter_data.setAttribute("ParentTrackingPolicy",parentTrackingPolicyToString(d->parent_tracking_policy));
    if (!d->is_modification_state_monitored)
        filter_data.setAttribute("IsModificationStateMonitored","false");
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ActivityPolicyFilter::importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(params)
    Q_UNUSED(import_list)

    IExportable::Result result = IExportable::Incomplete;

    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); i++)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "Data") {
            if (child.hasAttribute("ActivityPolicy"))
                d->activity_policy = stringToActivityPolicy(child.attribute("ActivityPolicy"));
            if (child.hasAttribute("MinimumActivityPolicy"))
                d->minimum_activity_policy = stringToMinimumActivityPolicy(child.attribute("MinimumActivityPolicy"));
            if (child.hasAttribute("NewSubjectActivityPolicy"))
                d->new_subject_activity_policy = stringToNewSubjectActivityPolicy(child.attribute("NewSubjectActivityPolicy"));
            if (child.hasAttribute("ParentTrackingPolicy"))
                d->parent_tracking_policy = stringToParentTrackingPolicy(child.attribute("ParentTrackingPolicy"));
            if (child.hasAttribute("IsModificationStateMonitored")) {
                if (child.attribute("IsModificationStateMonitored") == "true")
                    d->is_modification_state_monitored = true;
                else
                    d->is_modification_state_monitored = false;
            }
            result = IExportable::Complete;
            continue;
        }
    }

    return result;
}

bool Qtilities::Core::ActivityPolicyFilter::eventFilter(QObject *object, QEvent *event) {
    if (object == observer && event->type() == QEvent::User) {
        QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
        if (qtilities_event) {
            if (!qstrcmp(qtilities_event->propertyName().data(),OBJECT_ACTIVITY)) {
                // Now we need to check the following:
                // 1. observer can only have one parent.
                if (Observer::parentCount(observer) == 1) {
                    ObserverProperty observer_property = Observer::getObserverProperty(observer,OBJECT_ACTIVITY);
                    if (observer_property.isValid()) {
                        if (observer_property.observerMap().count() > 0) {
                            bool activity = observer_property.observerMap().values().at(0).toBool();

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
