/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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
#include "HelpPluginConfig.h"

#include <ExtensionSystemConstants>
#include <Qtilities.h>
#include <QtilitiesApplication>

#include <QtPlugin>
#include <QIcon>
#include <QApplication>

using namespace Qtilities::ExtensionSystem::Interfaces;
using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;

struct Qtilities::Plugins::Help::HelpPluginData {
    HelpPluginData() : help_mode(0) {}

    HelpPluginConfig    help_plugin_config;
    HelpMode*           help_mode;
};

Qtilities::Plugins::Help::HelpPlugin::HelpPlugin(QObject* parent) : QObject(parent) {
    d = new HelpPluginData;
    setObjectName(pluginName());
}

Qtilities::Plugins::Help::HelpPlugin::~HelpPlugin() {
    delete d;
}

bool Qtilities::Plugins::Help::HelpPlugin::initialize(const QStringList &arguments, QStringList *error_strings) {
    Q_UNUSED(arguments)
    Q_UNUSED(error_strings)

    d->help_mode = new HelpMode();
    OBJECT_MANAGER->registerObject(d->help_mode,QtilitiesCategory("GUI::Application Modes (IMode)","::"));
    OBJECT_MANAGER->registerObject(&d->help_plugin_config,QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));

    return true;
}

bool Qtilities::Plugins::Help::HelpPlugin::initializeDependencies(QStringList *error_strings) {
    Q_UNUSED(error_strings)

    HELP_MANAGER->initialize();
    d->help_mode->initiallize();

    return true;
}

void Qtilities::Plugins::Help::HelpPlugin::finalize() {

}

QString Qtilities::Plugins::Help::HelpPlugin::pluginName() const {
    return "Help Plugin";
}

QtilitiesCategory Qtilities::Plugins::Help::HelpPlugin::pluginCategory() const {
    return QtilitiesCategory(QApplication::applicationName());
}

Qtilities::Core::VersionInformation Qtilities::Plugins::Help::HelpPlugin::pluginVersionInformation() const {
    VersionInformation version_info(qti_def_VERSION_MAJOR,qti_def_VERSION_MINOR,qti_def_VERSION_REVISION);
    return version_info;
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginPublisher() const {
    return "Floware Computing (Pty) Ltd";
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
    return QString(tr("Copyright") + " 2009-2013, Floware Computing (Pty) Ltd");
}

QString Qtilities::Plugins::Help::HelpPlugin::pluginLicense() const {
    return tr("See the Qtilities Libraries license");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    using namespace Qtilities::Plugins::Help;
    Q_EXPORT_PLUGIN2(HelpPlugin, HelpPlugin)
#endif

