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

bool Qtilities::Plugins::SessionLog::SessionLogPlugin::initialize(const QStringList &arguments, QStringList *error_strings) {
    Q_UNUSED(arguments)
    Q_UNUSED(error_strings)

    // Add the session log mode to the global object pool:
    SessionLogMode* session_log_mode = new SessionLogMode();
    OBJECT_MANAGER->registerObject(session_log_mode,QtilitiesCategory("GUI::Application Modes (IMode)","::"));

    // Register the context of the session log mode:
    CONTEXT_MANAGER->registerContext(session_log_mode->contextString());

    // Logger Config Gui
    OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget(false),QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));

    return true;
}

bool Qtilities::Plugins::SessionLog::SessionLogPlugin::initializeDependencies(QStringList *error_strings) {
    Q_UNUSED(error_strings)

    return true;
}

void Qtilities::Plugins::SessionLog::SessionLogPlugin::finalize() {
    LOG_INFO("Finalizing session log plugin");
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginName() const {
    return "Session Log Plugin";
}

QtilitiesCategory Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginCategory() const {
    return QtilitiesCategory(QApplication::applicationName());
}

Qtilities::Core::VersionInformation Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginVersionInformation() const {
    VersionInformation version_info(qti_def_VERSION_MAJOR,qti_def_VERSION_MINOR,qti_def_VERSION_REVISION);
    return version_info;
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginPublisher() const {
    return "Jaco Naudé";
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
    return QString(tr("Copyright") + " 2009-2013, Jaco Naudé");
}

QString Qtilities::Plugins::SessionLog::SessionLogPlugin::pluginLicense() const {
    return tr("See the Qtilities Libraries license");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    using namespace Qtilities::Plugins::SessionLog;
    Q_EXPORT_PLUGIN2(SessionLogPlugin, SessionLogPlugin)
#endif
