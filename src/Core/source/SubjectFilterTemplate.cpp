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

#include "SubjectFilterTemplate.h"
#include "Observer.h"


namespace Qtilities {
    namespace Core {
        FactoryItem<AbstractSubjectFilter, SubjectFilterTemplate> SubjectFilterTemplate::factory;
    }
}

struct Qtilities::Core::SubjectFilterTemplatePrivateData {
    SubjectFilterTemplatePrivateData() : is_modified(false) {}

    bool is_modified;
};

Qtilities::Core::SubjectFilterTemplate::SubjectFilterTemplate(QObject* parent) : AbstractSubjectFilter(parent) {
    d = new SubjectFilterTemplatePrivateData;
}

Qtilities::Core::SubjectFilterTemplate::~SubjectFilterTemplate() {
    delete d;
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::SubjectFilterTemplate::evaluateAttachment(QObject* obj, QString* rejectMsg, bool silent) const {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)
    Q_UNUSED(silent)

    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::SubjectFilterTemplate::initializeAttachment(QObject* obj, QString* rejectMsg, bool import_cycle) {
    Q_UNUSED(obj)
    Q_UNUSED(import_cycle)

    #ifndef QT_NO_DEBUG
    Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
    if (!obj) {
        if (rejectMsg)
            *rejectMsg = QString(tr("Subject Filter Template: Invalid object reference received. Attachment cannot be done.."));
        return false;
    }
    #endif

    if (!observer) {
        if (rejectMsg)
            *rejectMsg = QString(tr("Cannot evaluate an attachment in a subject filter without an observer context."));
        LOG_TRACE("Cannot evaluate an attachment in a subject filter without an observer context.");
        return false;
    }

    return true;
}

void Qtilities::Core::SubjectFilterTemplate::finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle) {
    Q_UNUSED(obj)
    Q_UNUSED(attachment_successful)
    Q_UNUSED(import_cycle)
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::SubjectFilterTemplate::evaluateDetachment(QObject* obj, QString* rejectMsg) const {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)

    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::SubjectFilterTemplate::initializeDetachment(QObject* obj, QString* rejectMsg, bool subject_deleted) {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)
    Q_UNUSED(subject_deleted)

    return true;
}

void Qtilities::Core::SubjectFilterTemplate::finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted) {
    Q_UNUSED(obj)
    Q_UNUSED(detachment_successful)
    Q_UNUSED(subject_deleted)
}

QStringList Qtilities::Core::SubjectFilterTemplate::monitoredProperties() const {
    return QStringList();
}

QStringList Qtilities::Core::SubjectFilterTemplate::reservedProperties() const {
    return QStringList();
}

bool Qtilities::Core::SubjectFilterTemplate::handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    Q_UNUSED(obj)
    Q_UNUSED(property_name)
    Q_UNUSED(propertyChangeEvent)

    return false;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::SubjectFilterTemplate::instanceFactoryInfo() const {
    InstanceFactoryInfo instanceFactoryInfo(qti_def_FACTORY_QTILITIES,"Subject Filter Template","Subject Filter Template");
    return instanceFactoryInfo;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::SubjectFilterTemplate::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

bool Qtilities::Core::SubjectFilterTemplate::isModified() const {
    return d->is_modified;
}

void Qtilities::Core::SubjectFilterTemplate::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::SubjectFilterTemplate& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::SubjectFilterTemplate& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}


