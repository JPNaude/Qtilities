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

#include "SessionLogPlugin.h"
#include "SessionLogPluginConstants.h"
#include "SessionLogMode.h"

#include <Qtilities.h>
#include <QtilitiesCore.h>
#include <LoggerGui.h>

#include <QtPlugin>
#include <QIcon>
#include <QLabel>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;

struct Qtilities::Plugins::SessionLogPluginData {
    SessionLogPluginData() {}
};

Qtilities::Plugins::SessionLogPlugin::SessionLogPlugin(QObject* parent) : IPlugin(parent)
{
    d = new SessionLogPluginData;
    setObjectName("Session Log Plugin");
}

Qtilities::Plugins::SessionLogPlugin::~SessionLogPlugin()
{
    delete d;
}

bool Qtilities::Plugins::SessionLogPlugin::initialize(const QStringList &arguments, QString *errorString) {
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Add the session log mode to the global object pool:
    SessionLogMode* session_log_mode = new SessionLogMode();
    QtilitiesCore::instance()->objectManager()->registerObject(session_log_mode);

    // Register the context of the session log mode:
    QtilitiesCore::instance()->contextManager()->registerContext(session_log_mode->contextString());

    // Logger Config Gui
    QtilitiesCore::instance()->objectManager()->registerObject(LoggerGui::createLoggerConfigWidget(false));

    return true;
}

bool Qtilities::Plugins::SessionLogPlugin::initializeDependancies(QString *errorString) {
    Q_UNUSED(errorString)

    return true;
}

void Qtilities::Plugins::SessionLogPlugin::finalize() {

}

double Qtilities::Plugins::SessionLogPlugin::pluginVersion() {
    return (QString("%1.%2").arg(SESSION_LOG_PLUGIN_VERSION_MAJOR).arg(SESSION_LOG_PLUGIN_VERSION_MINOR)).toDouble();
}

QStringList Qtilities::Plugins::SessionLogPlugin::pluginCompatibilityVersions() {
    QStringList compatible_versions;
    compatible_versions << QtilitiesCore::instance()->version();
    return compatible_versions;
}

QString Qtilities::Plugins::SessionLogPlugin::pluginPublisher() {
    return "Jaco Naude";
}

QString Qtilities::Plugins::SessionLogPlugin::pluginPublisherWebsite() {
    return "";
}

QString Qtilities::Plugins::SessionLogPlugin::pluginPublisherContact() {
    return "naude.jaco@gmail.com";
}

QString Qtilities::Plugins::SessionLogPlugin::pluginDescription() {
    return tr("A plugin which adds a session log mode and a configuration page for the logger.");
}

QString Qtilities::Plugins::SessionLogPlugin::pluginCopyright() {
    return QString(tr("Copyright") + " 2010, Jaco Naude");
}

QString Qtilities::Plugins::SessionLogPlugin::pluginLicense()  {
    return tr("See the Qtilities Libraries license");
}

using namespace Qtilities::Plugins;
Q_EXPORT_PLUGIN2(SessionLogPlugin, SessionLogPlugin);
