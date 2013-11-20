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

#include "QtilitiesCoreApplication_p.h"
#include "ObjectManager.h"
#include "ContextManager.h"
#include "VersionInformation.h"

#include <LoggingConstants>
#include <Qtilities.h>

#include <QObject>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif

Qtilities::Core::QtilitiesCoreApplicationPrivate* Qtilities::Core::QtilitiesCoreApplicationPrivate::m_Instance = 0;

Qtilities::Core::QtilitiesCoreApplicationPrivate* Qtilities::Core::QtilitiesCoreApplicationPrivate::instance() {
    static QMutex mutex;
    if (!m_Instance)
    {
        mutex.lock();

        if (!m_Instance)
            m_Instance = new QtilitiesCoreApplicationPrivate;

        mutex.unlock();
    }

    return m_Instance;
}

Qtilities::Core::QtilitiesCoreApplicationPrivate::QtilitiesCoreApplicationPrivate() {
    // Object Manager
    d_objectManager = new ObjectManager;
    QObject* objectManagerQ = qobject_cast<QObject*> (d_objectManager);
    d_objectManagerIFace = qobject_cast<IObjectManager*> (objectManagerQ);

    // Context Manager
    d_contextManager = new ContextManager;
    QObject* contextManagerQ = qobject_cast<QObject*> (d_contextManager);
    d_contextManagerIFace = qobject_cast<IContextManager*> (contextManagerQ);
    QObject::connect(d_objectManager,SIGNAL(newObjectAdded(QObject*)),d_contextManager,SLOT(addContexts(QObject*)));

    // Task Manager
    d_taskManager = new TaskManager;
    QObject::connect(d_objectManager,SIGNAL(newObjectAdded(QObject*)),d_taskManager,SLOT(addTask(QObject*)));
    QObject::connect(d_objectManager,SIGNAL(objectRemoved(QObject*)),d_taskManager,SLOT(removeTask(QObject*)));

    // Register QList<QPointer<QObject> > in Meta Object System.
    qRegisterMetaType<QList<QPointer<QObject> > >("QList<QPointer<QObject> >");

    d_application_session_path = applicationSessionPathDefault();

    // Version related stuff:
    d_version_number.setVersionMajor(qti_def_VERSION_MAJOR);
    d_version_number.setVersionMinor(qti_def_VERSION_MINOR);
    d_version_number.setVersionRevision(qti_def_VERSION_REVISION);
    d_application_export_version = 0;

    // Settings related stuff:
    d_settings_enabled = true;

    // Application busy stuff:
    d_application_busy_count = 0;
}

Qtilities::Core::QtilitiesCoreApplicationPrivate::~QtilitiesCoreApplicationPrivate() {

}

Qtilities::Core::Interfaces::IObjectManager* Qtilities::Core::QtilitiesCoreApplicationPrivate::objectManager() const {
    return d_objectManagerIFace;
}

Qtilities::Core::Interfaces::IContextManager* Qtilities::Core::QtilitiesCoreApplicationPrivate::contextManager() const {
    return d_contextManagerIFace;
}

Qtilities::Core::Interfaces::TaskManager* Qtilities::Core::QtilitiesCoreApplicationPrivate::taskManager() const {
    return d_taskManager;
}

QString Qtilities::Core::QtilitiesCoreApplicationPrivate::qtilitiesVersionString() const {
    QString version_string;
    if (qti_def_VERSION_BETA != 0)
        version_string = QString("%1 Beta %2").arg(d_version_number.toString()).arg(qti_def_VERSION_BETA);
    else if (qti_def_VERSION_ALPHA != 0)
        version_string = QString("%1 Alpha %2").arg(d_version_number.toString()).arg(qti_def_VERSION_ALPHA);
    else
        version_string = d_version_number.toString();
    return version_string;
}

Qtilities::Core::VersionNumber Qtilities::Core::QtilitiesCoreApplicationPrivate::qtilitiesVersion() const {
    return d_version_number;
}

QString Qtilities::Core::QtilitiesCoreApplicationPrivate::applicationSessionPath() const {
    return d_application_session_path;
}

QString Qtilities::Core::QtilitiesCoreApplicationPrivate::applicationSessionPathDefault() const {
    #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return QString("%1%2%3").arg(QCoreApplication::applicationDirPath()).arg(QDir::separator()).arg(Qtilities::Logging::Constants::qti_def_PATH_SESSION);
    #else
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    #endif
}

void Qtilities::Core::QtilitiesCoreApplicationPrivate::setApplicationSessionPath(const QString& path) {
    d_application_session_path = path;
}

void Qtilities::Core::QtilitiesCoreApplicationPrivate::setApplicationExportVersion(quint32 application_export_version) {
    d_application_export_version = application_export_version;
}

quint32 Qtilities::Core::QtilitiesCoreApplicationPrivate::applicationExportVersion() const {
    return d_application_export_version;
}

QString Qtilities::Core::QtilitiesCoreApplicationPrivate::qtilitiesSettingsPath() {
    return d_application_session_path + QDir::separator() + "qtilities.ini";
}

void Qtilities::Core::QtilitiesCoreApplicationPrivate::setQtilitiesSettingsEnabled(bool is_enabled) {
    d_settings_enabled = is_enabled; 
}

bool Qtilities::Core::QtilitiesCoreApplicationPrivate::qtilitiesSettingsEnabled() const {
    return d_settings_enabled;
}

void Qtilities::Core::QtilitiesCoreApplicationPrivate::setApplicationBusy(bool is_busy) {    
    int previous_count = d_application_busy_count;

    if (is_busy) {
        ++d_application_busy_count;
    } else {
        if (d_application_busy_count > 0)
            --d_application_busy_count;
        else
            qWarning() << "setApplicationBusy(false) called too many times on QtilitiesCoreApplication";
    }

    #ifdef QTILITIES_BENCHMARKING
    qDebug() << "Settings application busy: " << d_application_busy_count;
    #endif

    if (previous_count == 0 && d_application_busy_count == 1) {
        // Application becomes busy:
        emit busyStateChanged(true);
    } else if (previous_count == 1 && d_application_busy_count == 0) {
        // Application not busy anymore:
        emit busyStateChanged(false);
    }
}

bool Qtilities::Core::QtilitiesCoreApplicationPrivate::applicationBusy() const {
    return (d_application_busy_count > 0);
}

