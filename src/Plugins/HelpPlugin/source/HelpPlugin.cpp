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

#include "HelpPlugin.h"
#include "HelpPluginConstants.h"
#include "HelpMode.h"

#include <ExtensionSystemConstants.h>
#include <Qtilities.h>
#include <QtilitiesCoreApplication.h>

#include <QtPlugin>
#include <QIcon>
#include <QtHelp>

using namespace Qtilities::ExtensionSystem::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::Plugins::Help::HelpPluginData {
    HelpPluginData() {}
};

Qtilities::Plugins::Help::HelpPlugin::HelpPlugin(QObject* parent) : QObject(parent) {
    d = new HelpPluginData;
    setObjectName(pluginName());
}

Qtilities::Plugins::Help::HelpPlugin::~HelpPlugin() {
    delete d;
}

bool Qtilities::Plugins::Help::HelpPlugin::initialize(const QStringList &arguments, QString *errorString) {
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    HelpMode* help_mode = new HelpMode();
    OBJECT_MANAGER->registerObject(help_mode,QtilitiesCategory("GUI::Application Modes (IMode)","::"));

    return true;
}

bool Qtilities::Plugins::Help::HelpPlugin::initializeDependancies(QString *errorString) {
    Q_UNUSED(errorString)

    return true;
}

void Qtilities::Plugins::Help::HelpPlugin::finalize() {

}

QString Qtilities::Plugins::Help::HelpPlugin::pluginName() const {
    return "Help Plugin";
}

QtilitiesCategory Qtilities::Plugins::Help::HelpPlugin::pluginCategory() const {
    return QtilitiesCategory(tr("General"));
}

double Qtilities::Plugins::Help::HelpPlugin::pluginVersion() const {
    return (QString("%1.%2").arg(HELP_PLUGIN_VERSION_MAJOR).arg(HELP_PLUGIN_VERSION_MINOR)).toDouble();
}

QStringList Qtilities::Plugins::Help::HelpPlugin::pluginCompatibilityVersions() const {
    QStringList compatible_versions;
    compatible_versions << QtilitiesCoreApplication::qtilitiesVersion();
    return compatible_versions;
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginPublisher() const {
    return "Jaco Naude";
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginPublisherWebsite() const {
    return "http://www.qtilities.org";
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginPublisherContact() const {
    return "support@qtilities.org";
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginDescription() const {
    return tr("Provides a help mode to the application.");
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginCopyright() const {
    return QString(tr("Copyright") + " 2010, Jaco Naude");
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginLicense() const {
    return tr("See the Qtilities Libraries license");
}

using namespace Qtilities::Plugins::Help;
Q_EXPORT_PLUGIN2(HelpPlugin, HelpPlugin);
