/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of the Qtilities Debug Plugin.
** See Dependencies.pri for the licensing requirements of the Debug plugin.
** Alternatively, see http://www.qtilities.org/licensing.html.
**
****************************************************************************/

#include "DebugPlugin.h"
#include "DebugPluginConstants.h"

#include <ExtensionSystemConstants>
#include <Qtilities.h>
#include <QtilitiesTesting>

#include <QtPlugin>
#include <QIcon>

using namespace QtilitiesTesting;

struct Qtilities::Plugins::Debug::DebugPluginPrivateData {
    DebugPluginPrivateData() : debug_mode(0) {}

    QPointer<DebugWidget> debug_mode;
};

Qtilities::Plugins::Debug::DebugPlugin::DebugPlugin(QObject* parent) : QObject(parent) {
    d = new DebugPluginPrivateData;
    setObjectName(pluginName());
}

Qtilities::Plugins::Debug::DebugPlugin::~DebugPlugin() {
    // Uncommenting the following crashes:
    // See https://github.com/JPNaude/Qtilities/issues/100.
    // if (d->debug_mode)
    //     delete d->debug_mode;
    delete d;
}

bool Qtilities::Plugins::Debug::DebugPlugin::initialize(const QStringList &arguments, QStringList *error_strings) {
    Q_UNUSED(arguments)
    Q_UNUSED(error_strings)

    // Add the session log mode to the global object pool:
    d->debug_mode = new DebugWidget;
    OBJECT_MANAGER->registerObject(d->debug_mode,QtilitiesCategory("GUI::Application Modes (IMode)","::"));

    // Register the context of the debug mode:
    // For now uncomment, the debug mode does not have a context at this stage.
    // CONTEXT_MANAGER->registerContext(d->debug_mode->contextString());

    return true;
}

bool Qtilities::Plugins::Debug::DebugPlugin::initializeDependencies(QStringList *error_strings) {
    Q_UNUSED(error_strings)

    d->debug_mode->finalizeMode();
    return true;
}

void Qtilities::Plugins::Debug::DebugPlugin::finalize() {

}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginName() const {
    return "Qtilities Debug Plugin";
}

QtilitiesCategory Qtilities::Plugins::Debug::DebugPlugin::pluginCategory() const {
    return QtilitiesCategory("Debugging");
}

Qtilities::Core::VersionInformation Qtilities::Plugins::Debug::DebugPlugin::pluginVersionInformation() const {
    VersionInformation version_info(qti_def_VERSION_MAJOR,qti_def_VERSION_MINOR,qti_def_VERSION_REVISION);
    return version_info;
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginPublisher() const {
    return "Jaco Naudé";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginPublisherWebsite() const {
    return "http://www.qtilities.org";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginPublisherContact() const {
    return "support@qtilities.org";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginDescription() const {
    return "A plugin which helps to debug Qtilities applications.";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginCopyright() const {
    return "Copyright 2009-2013, Jaco Naudé";
}

QString Qtilities::Plugins::Debug::DebugPlugin::pluginLicense() const  {
    return tr("See the Qtilities Libraries license");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    using namespace Qtilities::Plugins::Debug;
    Q_EXPORT_PLUGIN2(DebugPlugin, DebugPlugin)
#endif

