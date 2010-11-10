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

#include "QtilitiesCoreApplication_p.h"
#include "ObjectManager.h"
#include "ContextManager.h"

#include <Qtilities.h>

#include <QObject>

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
    d_objectManager = new ObjectManager();
    QObject* objectManagerQ = qobject_cast<QObject*> (d_objectManager);
    d_objectManagerIFace = qobject_cast<IObjectManager*> (objectManagerQ);

    // Context Manager
    d_contextManager = new ContextManager();
    QObject* contextManagerQ = qobject_cast<QObject*> (d_contextManager);
    d_contextManagerIFace = qobject_cast<IContextManager*> (contextManagerQ);
    QObject::connect(d_objectManager,SIGNAL(newObjectAdded(QObject*)),d_contextManager,SLOT(addContexts(QObject*)));
}

Qtilities::Core::QtilitiesCoreApplicationPrivate::~QtilitiesCoreApplicationPrivate() {

}

Qtilities::Core::Interfaces::IObjectManager* Qtilities::Core::QtilitiesCoreApplicationPrivate::objectManager() const {
    return d_objectManagerIFace;
}

Qtilities::Core::Interfaces::IContextManager* Qtilities::Core::QtilitiesCoreApplicationPrivate::contextManager() const {
    return d_contextManagerIFace;
}

QString Qtilities::Core::QtilitiesCoreApplicationPrivate::qtilitiesVersion() const {
    QString version_string;
    if (QTILITIES_VERSION_BETA != 0)
        version_string = QString(QObject::tr("%1.%2.%3 Beta %4")).arg(QTILITIES_VERSION_MAJOR).arg(QTILITIES_VERSION_MINOR).arg(QTILITIES_VERSION_REVISION).arg(QTILITIES_VERSION_BETA);
    else if (QTILITIES_VERSION_ALPHA != 0)
        version_string = QString(QObject::tr("%1.%2.%3 Alpha %4")).arg(QTILITIES_VERSION_MAJOR).arg(QTILITIES_VERSION_MINOR).arg(QTILITIES_VERSION_REVISION).arg(QTILITIES_VERSION_ALPHA);
    else
        version_string = QString("%1.%2.%3").arg(QTILITIES_VERSION_MAJOR).arg(QTILITIES_VERSION_MINOR).arg(QTILITIES_VERSION_REVISION);
    return version_string;
}
