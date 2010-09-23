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

#include "ExtensionSystemCore.h"
#include "ExtensionSystemConstants.h"
#include "ExtensionSystemConfig.h"
#include "IPlugin.h"

#include <QtilitiesCoreGui>

#include <QLayout>
#include <QMutex>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

using namespace QtilitiesCoreGui;
using namespace Qtilities::ExtensionSystem::Interfaces;

struct Qtilities::ExtensionSystem::ExtensionSystemCoreData {
    ExtensionSystemCoreData() : plugins("Plugins","Manages loaded plugins in the application."),
    extension_system_config_widget(0) { }

    Observer plugins;
    QDir pluginsDir;
    QStringList customPluginPaths;
    ExtensionSystemConfig* extension_system_config_widget;
};

Qtilities::ExtensionSystem::ExtensionSystemCore* Qtilities::ExtensionSystem::ExtensionSystemCore::m_Instance = 0;

Qtilities::ExtensionSystem::ExtensionSystemCore* Qtilities::ExtensionSystem::ExtensionSystemCore::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new ExtensionSystemCore;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::ExtensionSystem::ExtensionSystemCore::ExtensionSystemCore(QObject* parent) : QObject(parent)
{
    d = new ExtensionSystemCoreData;
    d->plugins.useDisplayHints();

    d->plugins.startProcessingCycle();

    // Setup the plugin pool observer
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    d->plugins.displayHints()->setDisplayFlagsHint(display_flags);
    d->plugins.displayHints()->setActionHints(ObserverHints::ActionFindItem);
    d->plugins.displayHints()->setItemSelectionControlHint(ObserverHints::SelectableItems);
    SubjectTypeFilter* type_filter = new SubjectTypeFilter(tr("Loaded Plugins"));
    type_filter->addSubjectType(SubjectTypeInfo("Qtilities::ExtensionSystem::Interfaces::IPlugin",tr("Plugins")));
    d->plugins.installSubjectFilter(type_filter);
    d->plugins.displayHints()->setItemViewColumnHint(ObserverHints::ColumnNoHints);

    d->plugins.endProcessingCycle();
}

Qtilities::ExtensionSystem::ExtensionSystemCore::~ExtensionSystemCore()
{
    delete d;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::loadPlugins() {
    d->pluginsDir = QDir(QCoreApplication::applicationDirPath());

    #if defined(Q_OS_WIN)
        if (d->pluginsDir.dirName().toLower() == "debug" || d->pluginsDir.dirName().toLower() == "release")
            d->pluginsDir.cdUp();
    #elif defined(Q_OS_MAC)
        if (d->pluginsDir.dirName() == "MacOS") {
            d->pluginsDir.cdUp();
            d->pluginsDir.cdUp();
            d->pluginsDir.cdUp();
        }
    #endif
    d->pluginsDir.cd("plugins");
    if (!d->customPluginPaths.contains(d->pluginsDir.path()))
        d->customPluginPaths << d->pluginsDir.path();

    foreach (QString path, d->customPluginPaths) {
        emit newProgressMessage(QString(tr("Searching for plugins in directory: %1")).arg(path));
        LOG_INFO(QString(tr("Searching for plugins in directory: %1")).arg(path));
        QCoreApplication::processEvents();

        QDir dir(path);
        foreach (QString fileName, dir.entryList(QDir::Files)) {
            QFileInfo file_info(fileName);
            QString stripped_file_name = file_info.fileName();
            if (QLibrary::isLibrary(dir.absoluteFilePath(fileName))) {
                LOG_INFO(tr("Found library: ") + stripped_file_name);
                QPluginLoader loader(dir.absoluteFilePath(fileName));
                QObject *obj = loader.instance();
                if (obj) {
                    // Check if the object implements IPlugin:
                    IPlugin* pluginIFace = qobject_cast<IPlugin*> (obj);
                    if (pluginIFace) {
                        emit newProgressMessage(QString(tr("Initializing plugin from file: %1")).arg(stripped_file_name));
                        LOG_INFO(QString(tr("Initializing plugin from file: %1")).arg(stripped_file_name));
                        QCoreApplication::processEvents();

                        // Give it a success icon by default:
                        SharedObserverProperty icon_property(QIcon(ICON_SUCCESS_16x16),OBJECT_ROLE_DECORATION);
                        Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);

                        // Store the file name:
                        pluginIFace->setPluginFileName(stripped_file_name);

                        // Do a plugin version check here:
                        pluginIFace->pluginVersion();
                        if (!pluginIFace->pluginCompatibilityVersions().contains(QCoreApplication::applicationVersion())) {
                            LOG_ERROR(QString(tr("Incompatible plugin version of the following plugin detected (in file %1): Your application version (v%2) is not found in the list of compatible application versions that this plugin supports.")).arg(stripped_file_name).arg(QCoreApplication::applicationVersion()));
                            pluginIFace->setPluginState(IPlugin::CompatibilityError);
                            pluginIFace->setErrorString(tr("The plugin is loaded but it indicated that it is not fully compatible with the current version of your application. The plugin might not work as intended. If you have problems with the plugin, it is recommended to remove it from your plugin directory."));
                            SharedObserverProperty icon_property(QIcon(ICON_WARNING_16x16),OBJECT_ROLE_DECORATION);
                            Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);
                        } else {
                            pluginIFace->setPluginState(IPlugin::Functional);
                            pluginIFace->setErrorString(tr("No errors detected"));
                        }

                        d->plugins.attachSubject(obj);

                        QString error_string;
                        if (!pluginIFace->initialize(QStringList(), &error_string)) {
                            LOG_ERROR(tr("Plugin (") + stripped_file_name + tr(") failed during initialization with error: ") + error_string);
                            pluginIFace->setPluginState(IPlugin::InitializationError);
                            pluginIFace->setErrorString(error_string);
                            SharedObserverProperty icon_property(QIcon(ICON_ERROR_16x16),OBJECT_ROLE_DECORATION);
                            Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);
                        }
                    } else {
                        LOG_ERROR(tr("Plugin found which does not implement the expected IPlugin interface."));
                    }
                } else {
                    LOG_ERROR(tr("Plugin could not be loaded: ") + stripped_file_name);
                }
            }
        }

        emit newProgressMessage(QString(tr("Finished loading plugins in directory:\n %1")).arg(path));
    }

    // Now that all plugins were loaded, we call initializeDependancies() on all of them
    for (int i = 0; i < d->plugins.subjectCount(); i++) {
        IPlugin* pluginIFace = qobject_cast<IPlugin*> (d->plugins.subjectAt(i));
        if (pluginIFace) {
            QString error_string;
            emit newProgressMessage(QString(tr("Initializing dependencies in plugin: %1")).arg(pluginIFace->objectName()));
            QCoreApplication::processEvents();
            if (!pluginIFace->initializeDependancies(&error_string)) {
                LOG_ERROR(error_string);
                pluginIFace->setPluginState(IPlugin::DependancyError);
                pluginIFace->setErrorString(error_string);
                SharedObserverProperty icon_property(QIcon(ICON_ERROR_16x16),OBJECT_ROLE_DECORATION);
                Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);
            }
            OBJECT_MANAGER->registerObject(d->plugins.subjectAt(i),QtilitiesCategory("Core::Plugins (IPlugin)","::"));
        }
    }

    emit newProgressMessage(QString(tr("Finished loading plugins in %1 directories.")).arg(d->customPluginPaths.count()));
    QCoreApplication::processEvents();
}

QWidget* Qtilities::ExtensionSystem::ExtensionSystemCore::configWidget() {
    if (!d->extension_system_config_widget) {
        ObserverWidget* observer_widget = new ObserverWidget(Qtilities::TableView);
        observer_widget->setObserverContext(&d->plugins);
        observer_widget->initialize();
        observer_widget->layout()->setMargin(0);

        d->extension_system_config_widget = new ExtensionSystemConfig();
        d->extension_system_config_widget->setPluginListWidget(observer_widget);
    }

    return d->extension_system_config_widget;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::addPluginPath(const QString& path) {
    if (d->customPluginPaths.contains(path))
        return;

    QDir dir(path);
    if (dir.exists())
        d->customPluginPaths << path;
}

QStringList Qtilities::ExtensionSystem::ExtensionSystemCore::pluginPaths() const {
    return d->customPluginPaths;
}
