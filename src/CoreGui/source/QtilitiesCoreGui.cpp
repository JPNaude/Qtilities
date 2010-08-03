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

#include "QtilitiesCoreGui.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ActionManager.h"
#include "ClipboardManager.h"
#include "AboutWindow.h"

#include <QtilitiesCore.h>
#include <Qtilities.h>

#include <QMutex>
#include <QMessageBox>

using namespace Qtilities::Core;

struct Qtilities::CoreGui::QtilitiesCoreGuiData {
    QtilitiesCoreGuiData() : mainWindow(0),
    actionManager(0),
    actionManagerIFace(0),
    clipboardManager(0),
    clipboardManagerIFace(0) { }

    QMainWindow* mainWindow;
    ActionManager* actionManager;
    IActionManager* actionManagerIFace;
    ClipboardManager* clipboardManager;
    IClipboard* clipboardManagerIFace;
    bool in_startup;
};

Qtilities::CoreGui::QtilitiesCoreGui* Qtilities::CoreGui::QtilitiesCoreGui::m_Instance = 0;

Qtilities::CoreGui::QtilitiesCoreGui* Qtilities::CoreGui::QtilitiesCoreGui::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new QtilitiesCoreGui;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::CoreGui::QtilitiesCoreGui::QtilitiesCoreGui(QObject* parent) : QObject(parent)
{
    d = new QtilitiesCoreGuiData;

    // Create managers
    d->actionManager = new ActionManager();
    d->clipboardManager = new ClipboardManager();

    // Cast to interfaces of managers
    QObject* actionManager = qobject_cast<QObject*> (d->actionManager);
    d->actionManagerIFace = qobject_cast<IActionManager*> (actionManager);
    QObject* clipboardManager = qobject_cast<QObject*> (d->clipboardManager);
    d->clipboardManagerIFace = qobject_cast<IClipboard*> (clipboardManager);

    // Some signal connections
    connect(QtilitiesCore::instance()->contextManager(),SIGNAL(contextChanged(QList<int>)),d->actionManager,SLOT(handleContextChanged(QList<int>)));
}

Qtilities::CoreGui::QtilitiesCoreGui::~QtilitiesCoreGui()
{
    delete d;
}

Qtilities::CoreGui::Interfaces::IActionManager* Qtilities::CoreGui::QtilitiesCoreGui::actionManager() {
    Q_ASSERT(d->actionManagerIFace);

    return d->actionManagerIFace;
}

Qtilities::CoreGui::Interfaces::IClipboard* Qtilities::CoreGui::QtilitiesCoreGui::clipboardManager() {
    Q_ASSERT(d->clipboardManagerIFace);

    return d->clipboardManagerIFace;
}

void Qtilities::CoreGui::QtilitiesCoreGui::setMainWindow(QMainWindow* mainWindow) {
    if (mainWindow) {
        d->mainWindow = mainWindow;
    }
}

QMainWindow* Qtilities::CoreGui::QtilitiesCoreGui::mainWindow() {
    return d->mainWindow;
}

void Qtilities::CoreGui::QtilitiesCoreGui::aboutQtilities() {
    AboutWindow* about_window = new AboutWindow();
    about_window->setWebsite("http://www.qtilities.org");
    about_window->setAttribute(Qt::WA_DeleteOnClose);
    about_window->setExtendedDescription(tr("This application uses the Qtilities libraries. For more information see the link below."));
    about_window->setVersionString("v" + QtilitiesCore::instance()->version());

    about_window->setWindowTitle(tr("About Qtilities"));
    about_window->show();
}
