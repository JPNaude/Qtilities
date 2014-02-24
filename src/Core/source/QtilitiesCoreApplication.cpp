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

#include "QtilitiesCoreApplication.h"
#include "QtilitiesCoreApplication_p.h"
#include "QtilitiesCoreConstants.h"
#include "ObjectManager.h"
#include "ContextManager.h"

#include <QVariant>
#include <QMap>
#include <QPointer>
#include <QtDebug>

using namespace Qtilities::Core::Constants;

Qtilities::Core::QtilitiesCoreApplication* Qtilities::Core::QtilitiesCoreApplication::m_Instance = 0;

Qtilities::Core::QtilitiesCoreApplication::QtilitiesCoreApplication(int &argc, char ** argv) : QCoreApplication(argc, argv) {
    if (!m_Instance) {
        m_Instance = this;
        connect(QtilitiesCoreApplicationPrivate::instance(),SIGNAL(busyStateChanged(bool)),this,SIGNAL(busyStateChanged(bool)));
    } else {
        qWarning() << QString("An instance was already created for QtilitiesCoreApplication");
    }
}

Qtilities::Core::QtilitiesCoreApplication::~QtilitiesCoreApplication() {

}

Qtilities::Core::Interfaces::IObjectManager* Qtilities::Core::QtilitiesCoreApplication::objectManager() {
    return QtilitiesCoreApplicationPrivate::instance()->objectManager();
}

Qtilities::Core::Interfaces::IContextManager* Qtilities::Core::QtilitiesCoreApplication::contextManager() {
    return QtilitiesCoreApplicationPrivate::instance()->contextManager();
}

Qtilities::Core::Interfaces::TaskManager* Qtilities::Core::QtilitiesCoreApplication::taskManager() {
    return QtilitiesCoreApplicationPrivate::instance()->taskManager();
}

QString Qtilities::Core::QtilitiesCoreApplication::qtilitiesVersionString() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersionString();
}

Qtilities::Core::VersionNumber Qtilities::Core::QtilitiesCoreApplication::qtilitiesVersion() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersion();
}

void Qtilities::Core::QtilitiesCoreApplication::setApplicationExportVersion(quint32 application_export_version) {
    QtilitiesCoreApplicationPrivate::instance()->setApplicationExportVersion(application_export_version);
}

quint32 Qtilities::Core::QtilitiesCoreApplication::applicationExportVersion() {
    return QtilitiesCoreApplicationPrivate::instance()->applicationExportVersion();
}

bool Qtilities::Core::QtilitiesCoreApplication::notify(QObject * object, QEvent * event)
{
    try
    {
        return QCoreApplication::notify(object, event);
    }
    catch (...)
    {
        LOG_FATAL(tr("QtilitiesCoreApplication Caught Exception..."));
    }
    return false;
}


Qtilities::Core::QtilitiesCoreApplication* Qtilities::Core::QtilitiesCoreApplication::instance(bool silent) {
    if (!QtilitiesCoreApplication::hasInstance("instance",silent))
        return 0;
    else
        return m_Instance;
}

bool Qtilities::Core::QtilitiesCoreApplication::hasInstance(const char *function, bool silent) {
    bool instance_exists = (QtilitiesCoreApplication::m_Instance != 0);
    if (!instance_exists & !silent)
        qWarning("QtilitiesCoreApplication::%s: Please instantiate the QtilitiesCoreApplication object before attempting to use it.", function);
    return instance_exists;
}

QString Qtilities::Core::QtilitiesCoreApplication::applicationSessionPath() {
    return QtilitiesCoreApplicationPrivate::instance()->applicationSessionPath();
}

void Qtilities::Core::QtilitiesCoreApplication::setApplicationSessionPath(const QString& path) {
     QtilitiesCoreApplicationPrivate::instance()->setApplicationSessionPath(path);
     Log->setLoggerSessionConfigPath(path);
}

QString Qtilities::Core::QtilitiesCoreApplication::qtilitiesSettingsPath() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesSettingsPath();
}

void Qtilities::Core::QtilitiesCoreApplication::setQtilitiesSettingsEnabled(bool is_enabled) {
    QtilitiesCoreApplicationPrivate::instance()->setQtilitiesSettingsEnabled(is_enabled);
    Log->setLoggerSettingsEnabled(is_enabled);
}

bool Qtilities::Core::QtilitiesCoreApplication::qtilitiesSettingsEnabled() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesSettingsEnabled();
}

void Qtilities::Core::QtilitiesCoreApplication::setApplicationBusy(bool is_busy) {
    QtilitiesCoreApplicationPrivate::instance()->setApplicationBusy(is_busy);
}

bool Qtilities::Core::QtilitiesCoreApplication::applicationBusy() {
    return QtilitiesCoreApplicationPrivate::instance()->applicationBusy();
}

