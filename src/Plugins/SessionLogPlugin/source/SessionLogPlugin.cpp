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

#include "SessionLogPlugin.h"
#include "SessionLogPluginConstants.h"
#include "SessionLogMode.h"

#include <Qtilities.h>
#include <QtilitiesCoreApplication.h>
#include <LoggerGui.h>

#include <QtPlugin>
#include <QIcon>
#include <QLabel>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;

struct Qtilities::Plugins::SessionLog::SessionLogPluginPrivateData {
    SessionLogPluginPrivateData() {}
};

Qtilities::Plugins::SessionLog::SessionLogPlugin::SessionLogPlugin(QObject* parent) : QObject(parent)
{
    d = new SessionLogPluginPrivateData;
    setObjectName(pluginName());
}

Qtilities::Plugins::SessionLog::SessionLogPlugin::~SessionLogPlugin()
{
    delete d;
}

bool Qtilities::Plugins::SessionLog::SessionLogPlugin::initialize(const QStringList &arguments, QString *errorString) {
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Add the session log mode to the global object pool:
    SessionLogMode* session_log_mode = new SessionLogMode();
    OBJECT_MANAGER->registerObject(session_log_mode,QtilitiesCategory("GUI::Application Modes (IMode)","::"));

    // Register the context of the session log mode:
    CONTEXT_MANAGER->registerContext(session_log_mode->contextString());

    // Logger Config Gui
    OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget(false),QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));

    return true;
}

bool Qtilities::Plugins::SessionLog::SessionLogPlugin::initializeDependancies(QString *errorString) {
    Q_UNUSED(errorString)

    return true;
}

void Qtilities::Plugins::SessionLog::SessionLogPlugin::finalize() {
    LOG_INFO("Finalizing session log plugin");
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginName() const {
    return "Session Log Plugin";
}

QtilitiesCategory Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginCategory() const {
    return QtilitiesCategory(tr("General"));
}

double Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginVersion() const {
    return (QString("%1.%2%3").arg(qti_def_VERSION_MAJOR).arg(qti_def_VERSION_MINOR).arg(qti_def_VERSION_REVISION)).toDouble();
}

QStringList Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginCompatibilityVersions() const {
    return QStringList();
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginPublisher() const {
    return "Jaco Naude";
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginPublisherWebsite() const {
    return "http://www.qtilities.org";
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginPublisherContact() const {
    return "support@qtilities.org";
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginDescription() const {
    return tr("A plugin which adds a session log mode and a configuration page for the logger.");
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginCopyright() const {
    return QString(tr("Copyright") + " 2010-2011, Jaco Naude");
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginLicense() const {
    return tr("See the Qtilities Libraries license");
}

using namespace Qtilities::Plugins::SessionLog;
Q_EXPORT_PLUGIN2(SessionLogPlugin, SessionLogPlugin);
