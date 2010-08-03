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

#include "QtilitiesCore.h"
#include "QtilitiesCoreConstants.h"
#include "ObjectManager.h"
#include "ContextManager.h"

#include <Qtilities.h>

#include <QVariant>
#include <QMap>
#include <QPointer>

using namespace Qtilities::Core::Constants;

struct Qtilities::Core::QtilitiesCoreData {
    QtilitiesCoreData() : objectManager(0),
    objectManagerIFace(0),
    contextManager(0),
    contextManagerIFace(0),
    in_startup(false) { }

    ObjectManager* objectManager;
    IObjectManager* objectManagerIFace;
    ContextManager* contextManager;
    IContextManager* contextManagerIFace;
    bool in_startup;
};

Qtilities::Core::QtilitiesCore* Qtilities::Core::QtilitiesCore::m_Instance = 0;

Qtilities::Core::QtilitiesCore* Qtilities::Core::QtilitiesCore::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new QtilitiesCore;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::Core::QtilitiesCore::QtilitiesCore(QObject* parent) : QObject(parent)
{
    d = new QtilitiesCoreData;

    // Create managers
    d->objectManager = new ObjectManager();
    d->contextManager = new ContextManager();

    // Cast to interfaces of managers
    QObject* objectManager = qobject_cast<QObject*> (d->objectManager);
    d->objectManagerIFace = qobject_cast<IObjectManager*> (objectManager);
    QObject* contextManager = qobject_cast<QObject*> (d->contextManager);
    d->contextManagerIFace = qobject_cast<IContextManager*> (contextManager);

    // Some signal connections
    connect(d->objectManager,SIGNAL(newObjectAdded(QObject*)),d->contextManager,SLOT(addContexts(QObject*)));
    // TRACK connect(d->contextManager,SIGNAL(contextChanged(QList<int>)),d->actionManager,SLOT(handleContextChanged(QList<int>)));
}

Qtilities::Core::QtilitiesCore::~QtilitiesCore()
{
    delete d;
}

Qtilities::Core::Interfaces::IObjectManager* Qtilities::Core::QtilitiesCore::objectManager() {
    Q_ASSERT(d->objectManagerIFace);

    return d->objectManagerIFace;
}

Qtilities::Core::Interfaces::IContextManager* Qtilities::Core::QtilitiesCore::contextManager() {
    Q_ASSERT(d->contextManagerIFace);

    return d->contextManagerIFace;
}

void Qtilities::Core::QtilitiesCore::setStartupState(bool in_startup) {
    d->in_startup = in_startup;
    if (d->contextManagerIFace && !in_startup) {
        d->contextManagerIFace->setNewContext(CONTEXT_STANDARD);
    }
}

bool Qtilities::Core::QtilitiesCore::startupFinished() {
    return !d->in_startup;
}

QString Qtilities::Core::QtilitiesCore::version() const {
    QString version_string;
    if (QTILITIES_VERSION_BETA != 0)
        version_string = QString(tr("%1.%2 Beta %3")).arg(QTILITIES_VERSION_MAJOR).arg(QTILITIES_VERSION_MINOR).arg(QTILITIES_VERSION_BETA);
    else if (QTILITIES_VERSION_ALPHA != 0)
        version_string = QString(tr("%1.%2 Alpha %3")).arg(QTILITIES_VERSION_MAJOR).arg(QTILITIES_VERSION_MINOR).arg(QTILITIES_VERSION_ALPHA);
    else
        version_string = QString("%1.%2").arg(QTILITIES_VERSION_MAJOR).arg(QTILITIES_VERSION_MINOR);
    return version_string;
}
