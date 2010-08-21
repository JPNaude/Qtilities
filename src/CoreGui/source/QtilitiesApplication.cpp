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

#include "QtilitiesApplication.h"
#include "QtilitiesApplication_p.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ActionManager.h"
#include "ClipboardManager.h"
#include "AboutWindow.h"
#include "NamingPolicyFilter.h"

#include <QtilitiesCoreApplication_p.h>
#include <Qtilities.h>
#include <Factory>

#include <QMutex>
#include <QMessageBox>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui::Constants;

Qtilities::CoreGui::QtilitiesApplication* Qtilities::CoreGui::QtilitiesApplication::m_Instance = 0;

Qtilities::CoreGui::QtilitiesApplication::QtilitiesApplication(int &argc, char ** argv) : QApplication(argc, argv) {
    if (!m_Instance) {
        m_Instance = this;

        // Register the naming policy filter in the object manager:
        FactoryInterfaceData naming_policy_filter(FACTORY_TAG_NAMING_POLICY_FILTER,QStringList(tr("Subject Filters")));
        QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerSubjectFilter(&NamingPolicyFilter::factory,naming_policy_filter);
    } else {
        qWarning() << QString(tr("An instance was already created for QtilitiesApplication"));
    }
}

Qtilities::CoreGui::QtilitiesApplication::~QtilitiesApplication() {

}

Qtilities::Core::Interfaces::IObjectManager* const Qtilities::CoreGui::QtilitiesApplication::objectManager() {
     return QtilitiesCoreApplicationPrivate::instance()->objectManager();
}

Qtilities::Core::Interfaces::IContextManager* const Qtilities::CoreGui::QtilitiesApplication::contextManager() {
    return QtilitiesCoreApplicationPrivate::instance()->contextManager();
}

Qtilities::CoreGui::Interfaces::IActionManager* const Qtilities::CoreGui::QtilitiesApplication::actionManager() {
    return QtilitiesApplicationPrivate::instance()->actionManager();
}

Qtilities::CoreGui::Interfaces::IClipboard* const Qtilities::CoreGui::QtilitiesApplication::clipboardManager() {
    return QtilitiesApplicationPrivate::instance()->clipboardManager();
}

void Qtilities::CoreGui::QtilitiesApplication::setMainWindow(QMainWindow* mainWindow) {
    QtilitiesApplicationPrivate::instance()->setMainWindow(mainWindow);
}

QMainWindow* const Qtilities::CoreGui::QtilitiesApplication::mainWindow() {
    return QtilitiesApplicationPrivate::instance()->mainWindow();
}

QString Qtilities::CoreGui::QtilitiesApplication::qtilitiesVersion() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersion();
}

Qtilities::CoreGui::QtilitiesApplication* Qtilities::CoreGui::QtilitiesApplication::instance() {
    if (!QtilitiesApplication::hasInstance("instance"))
        return 0;
    else
        return m_Instance;
}

bool Qtilities::CoreGui::QtilitiesApplication::hasInstance(const char *function) {
    bool instance_exists = (QtilitiesApplication::m_Instance != 0);
    if (!instance_exists)
        qWarning("QtilitiesApplication::%s: Please instantiate the QtilitiesApplication object before attempting to use it.", function);
    return instance_exists;
}

bool Qtilities::CoreGui::QtilitiesApplication::notify(QObject * object, QEvent * event) {
    try
    {
        return QApplication::notify(object, event);
    }
    catch (...)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Exception Caught"));
        msgBox.setText("The context you are trying to access is locked.");
        msgBox.exec();
        LOG_FATAL(tr("QtilitiesApplication Caught Exception..."));
    }
    return false;
}

void Qtilities::CoreGui::QtilitiesApplication::aboutQtilities() {
    AboutWindow* about_window = new AboutWindow();
    about_window->setWebsite("http://www.qtilities.org");
    about_window->setAttribute(Qt::WA_DeleteOnClose);
    about_window->setExtendedDescription(tr("This application uses the Qtilities libraries. For more information see the link below."));
    about_window->setVersionString("v" + QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersion());

    about_window->setWindowTitle(tr("About Qtilities"));
    about_window->show();
}

