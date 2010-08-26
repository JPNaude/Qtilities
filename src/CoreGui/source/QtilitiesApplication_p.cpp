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

#include "QtilitiesApplication_p.h"
#include "QtilitiesCoreApplication_p.h"
#include "ObjectManager.h"
#include "ContextManager.h"

#include <QObject>

using namespace Qtilities::Core;

Qtilities::CoreGui::QtilitiesApplicationPrivate* Qtilities::CoreGui::QtilitiesApplicationPrivate::m_Instance = 0;

Qtilities::CoreGui::QtilitiesApplicationPrivate* Qtilities::CoreGui::QtilitiesApplicationPrivate::instance() {
    static QMutex mutex;
    if (!m_Instance)
    {
        mutex.lock();

        if (!m_Instance)
            m_Instance = new QtilitiesApplicationPrivate;

        mutex.unlock();
    }

    return m_Instance;
}

Qtilities::CoreGui::QtilitiesApplicationPrivate::QtilitiesApplicationPrivate() {
    // Action Manager
    d_actionManager = new ActionManager();
    QObject* actionManagerQ = qobject_cast<QObject*> (d_actionManager);
    d_actionManagerIFace = qobject_cast<IActionManager*> (actionManagerQ);
    QObject::connect(QtilitiesCoreApplicationPrivate::instance()->contextManager(),SIGNAL(contextChanged(QList<int>)),d_actionManager,SLOT(handleContextChanged(QList<int>)));

    // Clipboard Manager
    d_clipboardManager = new ClipboardManager();
    QObject* clipboardManagerQ = qobject_cast<QObject*> (d_clipboardManager);
    d_clipboardManagerIFace = qobject_cast<IClipboard*> (clipboardManagerQ);
}

Qtilities::CoreGui::QtilitiesApplicationPrivate::~QtilitiesApplicationPrivate() {

}

Qtilities::CoreGui::Interfaces::IActionManager* const Qtilities::CoreGui::QtilitiesApplicationPrivate::actionManager() const {
    return d_actionManagerIFace;
}

Qtilities::CoreGui::Interfaces::IClipboard* const Qtilities::CoreGui::QtilitiesApplicationPrivate::clipboardManager() const {
    return d_clipboardManagerIFace;
}

void Qtilities::CoreGui::QtilitiesApplicationPrivate::setMainWindow(QWidget* mainWindow) {
    d_mainWindow = mainWindow;
}

QWidget* const Qtilities::CoreGui::QtilitiesApplicationPrivate::mainWindow() const {
    return d_mainWindow;
}
