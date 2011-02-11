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
    } else {
        qWarning() << QString(tr("An instance was already created for QtilitiesCoreApplication"));
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

QString Qtilities::Core::QtilitiesCoreApplication::qtilitiesVersion() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersion();
}

bool Qtilities::Core::QtilitiesCoreApplication::notify(QObject * object, QEvent * event)
{
    try
    {
        return QtilitiesCoreApplication::notify(object, event);
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
}
