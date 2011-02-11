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

#include "DebugPlugin.h"
#include "DebugPluginConstants.h"
#include "DebugWidget.h"

#include <ExtensionSystemConstants.h>
#include <Qtilities.h>
#include <QtilitiesCoreApplication.h>

#include <QtPlugin>
#include <QIcon>

using namespace Qtilities::ExtensionSystem::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::Plugins::Debug::DebugPluginPrivateData {
    DebugPluginPrivateData() : debug_mode(0) {}

    DebugWidget* debug_mode;
};

Qtilities::Plugins::Debug::DebugPlugin::DebugPlugin(QObject* parent) : QObject(parent) {
    d = new DebugPluginPrivateData;
    setObjectName(pluginName());
}

Qtilities::Plugins::Debug::DebugPlugin::~DebugPlugin() {
    delete d;
}

bool Qtilities::Plugins::Debug::DebugPlugin::initialize(const QStringList &arguments, QString *errorString) {
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Add the session log mode to the global object pool:
    d->debug_mode = new DebugWidget();
    OBJECT_MANAGER->registerObject(d->debug_mode,QtilitiesCategory("GUI::Application Modes (IMode)","::"));

    // Register the context of the debug mode:
    // For now uncomment, the debug mode does not have a context at this stage.
    // CONTEXT_MANAGER->registerContext(d->debug_mode->contextString());

    return true;
}

bool Qtilities::Plugins::Debug::DebugPlugin::initializeDependancies(QString *errorString) {
    Q_UNUSED(errorString)

    d->debug_mode->finalizeMode();
    return true;
}

void Qtilities::Plugins::Debug::DebugPlugin::finalize() {

}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginName() const {
    return "Qtilities Debug Plugin";
}

QtilitiesCategory Qtilities::Plugins::Debug::DebugPlugin::pluginCategory() const {
    return QtilitiesCategory(tr("Debugging"));
}

double Qtilities::Plugins::Debug::DebugPlugin::pluginVersion() const {
    return (QString("%1.%2%3").arg(qti_def_VERSION_MAJOR).arg(qti_def_VERSION_MINOR).arg(qti_def_VERSION_REVISION)).toDouble();
}

QStringList Qtilities::Plugins::Debug::DebugPlugin::pluginCompatibilityVersions() const {
    return QStringList();
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginPublisher() const {
    return "Jaco Naude";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginPublisherWebsite() const {
    return "http://www.qtilities.org";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginPublisherContact() const {
    return "support@qtilities.org";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginDescription() const {
    return tr("A plugin which helps to debug Qtilities applications.");
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginCopyright() const {
    return QString(tr("Copyright") + " 2010-2011, Jaco Naude");
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginLicense() const  {
    return tr("See the Qtilities Libraries license");
}

using namespace Qtilities::Plugins::Debug;
Q_EXPORT_PLUGIN2(DebugPlugin, DebugPlugin);
