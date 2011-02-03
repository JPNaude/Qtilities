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

#include "ExtensionSystemCore.h"
#include "ExtensionSystemConstants.h"
#include "ExtensionSystemConfig.h"
#include "IPlugin.h"
#include "PluginTreeModel.h"

#include <QtilitiesCoreGui>

#include <QLayout>
#include <QMutex>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDomDocument>

using namespace QtilitiesCoreGui;
using namespace Qtilities::ExtensionSystem::Interfaces;

struct Qtilities::ExtensionSystem::ExtensionSystemCoreData {
    ExtensionSystemCoreData() : plugins("Plugins"),
    plugin_activity_filter(0),
    treeModel(0),
    is_initialized(false) { }

    TreeNode                plugins;
    ActivityPolicyFilter*   plugin_activity_filter;
    QDir                    pluginsDir;
    QStringList             customPluginPaths;
    QPointer<ExtensionSystemConfig> extension_system_config_widget;
    PluginTreeModel*        treeModel;

    QString                 active_configuration_file;
    QStringList             set_inactive_plugins;
    QStringList             set_filtered_plugins;

    QStringList             current_active_plugins;
    QStringList             current_inactive_plugins;
    QStringList             current_filtered_plugins;
    QStringList             core_plugins;

    bool                    is_initialized;
};

Qtilities::ExtensionSystem::ExtensionSystemCore* Qtilities::ExtensionSystem::ExtensionSystemCore::m_Instance = 0;

Qtilities::ExtensionSystem::ExtensionSystemCore* Qtilities::ExtensionSystem::ExtensionSystemCore::instance() {
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

Qtilities::ExtensionSystem::ExtensionSystemCore::ExtensionSystemCore(QObject* parent) : QObject(parent) {
    d = new ExtensionSystemCoreData;
    d->plugins.startProcessingCycle();

    d->plugin_activity_filter = d->plugins.enableActivityControl(ObserverHints::NoActivityDisplay,
                                                                 ObserverHints::NoActivityControl);
    d->plugin_activity_filter->setNewSubjectActivityPolicy(ActivityPolicyFilter::SetNewActive);

    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    d->plugins.displayHints()->setDisplayFlagsHint(display_flags);
    d->plugins.displayHints()->setActionHints(ObserverHints::ActionFindItem);
    d->plugins.displayHints()->setItemSelectionControlHint(ObserverHints::SelectableItems);
    d->plugins.displayHints()->setItemViewColumnHint(ObserverHints::ColumnNoHints);
    d->plugins.displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);

    d->plugins.endProcessingCycle();
    //d->plugins.toggleSubjectEventFiltering(false);

    // Make sure App_Path/plugins always exists:
    QDir pluginsDir = QDir(QCoreApplication::applicationDirPath() + "/plugins");
    if (!pluginsDir.exists()) {
        pluginsDir.cdUp();
        pluginsDir.mkdir("/plugins");
    }
    d->active_configuration_file = QCoreApplication::applicationDirPath() + "/plugins/default.pconfig";
}

Qtilities::ExtensionSystem::ExtensionSystemCore::~ExtensionSystemCore()
{
    if (d->extension_system_config_widget)
        delete d->extension_system_config_widget;
    delete d;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::initialize() {
    // Check if isPluginActivityControlEnabled() is true and that a default plugin file exists.
    // In that case, load the default plugin file:
    if (isPluginActivityControlEnabled()) {
        // This will fail if there was no default file:
        loadPluginConfiguration();
    }

    // Now go get all the plugins:
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

            #if defined(Q_OS_UNIX)
                // Filter .so.x plugins on linux:
                if (file_info.completeSuffix().split(".").count() > 1 && file_info.completeSuffix().startsWith("so"))
                    continue;
            #endif

            bool is_filtered_plugin = false;
            foreach (QString expression, d->set_filtered_plugins) {
                QRegExp rx(expression);
                rx.setPatternSyntax(QRegExp::Wildcard);
                if (rx.exactMatch(stripped_file_name)) {
                    is_filtered_plugin = true;
                    break;
                }
            }

            if (!is_filtered_plugin) {
                if (QLibrary::isLibrary(dir.absoluteFilePath(fileName))) {
                    LOG_INFO(tr("Found library: ") + stripped_file_name);
                    QPluginLoader loader(dir.absoluteFilePath(fileName));
                    QObject *obj = loader.instance();
                    if (obj) {
                        // Check if the object implements IPlugin:
                        IPlugin* pluginIFace = qobject_cast<IPlugin*> (obj);
                        if (pluginIFace) {
                            emit newProgressMessage(QString(tr("Loading plugin from file: %1")).arg(stripped_file_name));
                            LOG_INFO(QString(tr("Loading plugin from file: %1")).arg(stripped_file_name));
                            QCoreApplication::processEvents();

                            // Set the object name of the plugin:
                            pluginIFace->objectBase()->setObjectName(pluginIFace->pluginName());

                            // Set the category property of the plugin:
                            ObserverProperty category_property(OBJECT_CATEGORY);
                            category_property.setValue(qVariantFromValue(pluginIFace->pluginCategory()),d->plugins.observerID());
                            Observer::setObserverProperty(pluginIFace->objectBase(),category_property);

                            // Store the file name:
                            pluginIFace->setPluginFileName(dir.absoluteFilePath(fileName));

                            // Do a plugin compatibility check here:
                            pluginIFace->pluginVersion();
                            if (!pluginIFace->pluginCompatibilityVersions().isEmpty()) {
                                if (!pluginIFace->pluginCompatibilityVersions().contains(QCoreApplication::applicationVersion())) {
                                    LOG_ERROR(QString(tr("Incompatible plugin version of the following plugin detected (in file %1): Your application version (v%2) is not found in the list of compatible application versions that this plugin supports.")).arg(stripped_file_name).arg(QCoreApplication::applicationVersion()));
                                    pluginIFace->setPluginState(IPlugin::CompatibilityError);
                                    pluginIFace->setErrorString(tr("The plugin is loaded but it indicated that it is not fully compatible with the current version of your application. The plugin might not work as intended. If you have problems with the plugin, it is recommended to remove it from your plugin directory."));
                                    SharedObserverProperty icon_property(QIcon(ICON_WARNING_16x16),OBJECT_ROLE_DECORATION);
                                    Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);
                                }
                            }

                            d->plugins.attachSubject(obj);

                            bool is_inactive_plugin = false;
                            foreach (QString inactivePluginName, d->set_inactive_plugins) {
                                if (pluginIFace->pluginName() == inactivePluginName) {
                                    is_inactive_plugin = true;
                                    break;
                                }
                            }

                            if (!is_inactive_plugin) {
                                QString error_string;
                                if (!pluginIFace->initialize(QStringList(), &error_string)) {
                                    LOG_ERROR(tr("Plugin (") + stripped_file_name + tr(") failed during initialization with error: ") + error_string);
                                    pluginIFace->setPluginState(IPlugin::InitializationError);
                                    pluginIFace->setErrorString(error_string);
                                    SharedObserverProperty icon_property(QIcon(ICON_ERROR_16x16),OBJECT_ROLE_DECORATION);
                                    Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);
                                } else {
                                    LOG_DEBUG(tr("Plugin (") + stripped_file_name + tr(") initialized successfully."));
                                }
                            }
                        } else {
                            LOG_ERROR(tr("Plugin found which does not implement the expected IPlugin interface."));
                        }
                    } else {
                        LOG_ERROR(tr("Plugin could not be loaded: ") + stripped_file_name + tr(". Common causes of this is that the plugin was built in a different mode (release or debug) as your application, or it was built with a different compiler."));
                    }
                }
            } else {
                LOG_DEBUG("Skipped filtered plugin during plugin loading: " + stripped_file_name);
                d->current_filtered_plugins << stripped_file_name;
            }
        }

        emit newProgressMessage(QString(tr("Finished loading plugins in directory:\n %1")).arg(path));
    }

    // Now that all plugins were loaded, we call initializeDependancies() on all active ones:
    for (int i = 0; i < d->plugins.subjectCount(); i++) {
        IPlugin* pluginIFace = qobject_cast<IPlugin*> (d->plugins.subjectAt(i));
        if (pluginIFace) {
            bool is_inactive_plugin = false;
            foreach (QString inactivePluginName, d->set_inactive_plugins) {
                if (pluginIFace->pluginName() == inactivePluginName) {
                    is_inactive_plugin = true;
                    break;
                }
            }

            if (!is_inactive_plugin) {
                QString error_string;
                emit newProgressMessage(QString(tr("Initializing dependencies in plugin: %1")).arg(pluginIFace->pluginName()));
                QCoreApplication::processEvents();
                if (!pluginIFace->initializeDependancies(&error_string)) {
                    LOG_ERROR(error_string);
                    pluginIFace->setPluginState(IPlugin::DependancyError);
                    pluginIFace->setErrorString(error_string);
                    SharedObserverProperty icon_property(QIcon(ICON_ERROR_16x16),OBJECT_ROLE_DECORATION);
                    Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);
                } else {
                    // Set the default state of the plugin:
                    pluginIFace->setPluginState(IPlugin::Functional);
                    pluginIFace->setErrorString(tr("No errors detected."));

                    // Give it a success icon by default:
                    SharedObserverProperty icon_property(QIcon(ICON_SUCCESS_16x16),OBJECT_ROLE_DECORATION);
                    Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);

                    // Add it to the active list:
                    d->current_active_plugins << pluginIFace->pluginName();
                }

                // Set the foreground color of core plugins:
                if (d->core_plugins.contains(pluginIFace->pluginName())) {
                    QBrush disabled_brush = QApplication::palette().brush(QPalette::Disabled,QPalette::Text);
                    SharedObserverProperty property(disabled_brush,OBJECT_ROLE_FOREGROUND);
                    Observer::setSharedProperty(pluginIFace->objectBase(), property);
                }
            } else {
                // Set the default state of the plugin:
                pluginIFace->setPluginState(IPlugin::InActive);
                pluginIFace->setErrorString(tr("Inactive."));
                d->current_inactive_plugins << pluginIFace->pluginName();

                // Make it inactive:
                ObserverProperty category_property(OBJECT_ACTIVITY);
                category_property.setValue(false,d->plugins.observerID());
                Observer::setObserverProperty(pluginIFace->objectBase(),category_property);

                // Give it a success icon by default:
                SharedObserverProperty icon_property(QIcon(ICON_SUCCESS_16x16),OBJECT_ROLE_DECORATION);
                Observer::setSharedProperty(pluginIFace->objectBase(),icon_property);

                LOG_INFO(QString(tr("Inactive plugin found which will not be initialized: %1")).arg(pluginIFace->pluginName()));
            }
            OBJECT_MANAGER->registerObject(d->plugins.subjectAt(i),QtilitiesCategory("Core::Plugins (IPlugin)","::"));
        }
    }

    // Only connect here since the signal will be emitted in above code:
    connect(d->plugin_activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(handlePluginConfigurationChange(QList<QObject*>,QList<QObject*>)));

    emit newProgressMessage(QString(tr("Finished loading plugins in %1 directories.")).arg(d->customPluginPaths.count()));
    QCoreApplication::processEvents();

    d->is_initialized = true;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::finalize() {
    // Loop through all plugins and call finalize on them:
    int plugin_count = d->plugins.subjectCount();
    for (int i = 0; i < plugin_count; i++) {
        IPlugin* pluginIFace = qobject_cast<IPlugin*> (d->plugins.subjectAt(0));
        if (pluginIFace) {
            emit newProgressMessage(QString(tr("Finalizing plugin: %1")).arg(pluginIFace->pluginName()));
            QCoreApplication::processEvents();
            pluginIFace->finalize();
            OBJECT_MANAGER->removeObject(d->plugins.subjectAt(0));
            d->plugins.detachSubject(d->plugins.subjectAt(0));
        }
    }
}

QWidget* Qtilities::ExtensionSystem::ExtensionSystemCore::configWidget() {
    if (!d->extension_system_config_widget) {
        ObserverWidget* observer_widget = new ObserverWidget();
        observer_widget->setObserverContext(&d->plugins);
        d->treeModel = new PluginTreeModel();
        observer_widget->setCustomTreeModel(d->treeModel);
        observer_widget->initialize();
        observer_widget->layout()->setMargin(0);

        d->extension_system_config_widget = new ExtensionSystemConfig();
        d->extension_system_config_widget->setPluginListWidget(observer_widget);

        if (observer_widget->treeView()) {
            observer_widget->treeView()->expandAll();
            observer_widget->treeView()->setRootIsDecorated(false);
            observer_widget->treeView()->setSelectionBehavior(QAbstractItemView::SelectRows);
        }
    }

    return d->extension_system_config_widget;
}

Qtilities::ExtensionSystem::Interfaces::IPlugin* Qtilities::ExtensionSystem::ExtensionSystemCore::findPlugin(const QString& plugin_name) const {
    QObject* obj = d->plugins.subjectReference(plugin_name);
    if (obj) {
        IPlugin* plugin = qobject_cast<IPlugin*> (obj);
        return plugin;
    } else
        return 0;
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

QString Qtilities::ExtensionSystem::ExtensionSystemCore::activePluginConfigurationFile() const {
    return d->active_configuration_file;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::setActivePluginConfigurationFile(const QString& file_name) {
    d->active_configuration_file = file_name;
}

QStringList Qtilities::ExtensionSystem::ExtensionSystemCore::activePlugins() const {
    return d->current_active_plugins;
}

QStringList Qtilities::ExtensionSystem::ExtensionSystemCore::inactivePlugins() const {
    return d->current_inactive_plugins;
}

QStringList Qtilities::ExtensionSystem::ExtensionSystemCore::filteredPlugins() const {
    return d->current_filtered_plugins;
}

QStringList Qtilities::ExtensionSystem::ExtensionSystemCore::corePlugins() const{
    return d->core_plugins;
}

QStringList Qtilities::ExtensionSystem::ExtensionSystemCore::inactivePluginsCurrentSet() const {
    return d->set_inactive_plugins;
}

QStringList Qtilities::ExtensionSystem::ExtensionSystemCore::filteredPluginsCurrentSet() const {
    return d->set_filtered_plugins;
}

bool Qtilities::ExtensionSystem::ExtensionSystemCore::savePluginConfiguration(QString file_name, QStringList* inactive_plugins, QStringList* filtered_plugins) const {
    if (file_name.isEmpty())
        file_name = d->active_configuration_file;

    QFile file(file_name);
    if(!file.open(QFile::WriteOnly))
        return false;

    // Determine which set of plugins must be used:
    QStringList final_inactive;
    if (inactive_plugins) {
        final_inactive = *inactive_plugins;
    } else {
        final_inactive = d->set_filtered_plugins;
    }
    QStringList final_filtered;
    if (filtered_plugins) {
        final_filtered = *filtered_plugins;
    } else {
        final_filtered = d->set_filtered_plugins;
    }

    // Create the QDomDocument:
    QDomDocument doc("QtilitiesPluginConfiguration");
    QDomElement root = doc.createElement("QtilitiesPluginConfiguration");
    root.setAttribute("DocumentVersion",QTILITIES_PLUGIN_CONFIG_FORMAT);
    doc.appendChild(root);

    // Do XML of inactive and filter lists:
    // Inactive Plugins:
    QDomElement inactive_node = doc.createElement("InactivePlugins");
    root.appendChild(inactive_node);
    foreach (QString name, final_inactive) {
        QDomElement inactive_item = doc.createElement("PluginName");
        inactive_item.setAttribute("Value",name);
        inactive_node.appendChild(inactive_item);
    }

    // Filtered Plugins:
    QDomElement filtered_node = doc.createElement("FilteredPlugins");
    root.appendChild(filtered_node);
    foreach (QString name, final_filtered) {
        QDomElement filtered_item = doc.createElement("Expression");
        filtered_item.setAttribute("Value",regExpToXml(name));
        filtered_node.appendChild(filtered_item);
    }

    // Put the complete doc in a string and save it to the file:
    // Still write it even if it fails so that we can check the output file for debugging purposes.
    QString docStr = doc.toString(2);
    file.write(docStr.toAscii());
    file.close();

    return true;
}

bool Qtilities::ExtensionSystem::ExtensionSystemCore::loadPluginConfiguration(QString file_name, QStringList* inactive_plugins, QStringList* filtered_plugins) {
    if (d->is_initialized && (!inactive_plugins || !filtered_plugins)) {
        LOG_DEBUG("Failed to load plugin configuration from file: " + file_name + ". The extension system is already initialized. ");
        return false;
    }

    if (file_name.isEmpty())
        file_name = d->active_configuration_file;

    // Load the file into doc:
    QDomDocument doc("QtilitiesPluginConfiguration");
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_ERROR(tr("Failed to load plugin configuration from file: ") + file_name);
        return false;
    }

    QString docStr = file.readAll();
    QString error_string;
    int error_line;
    int error_column;
    if (!doc.setContent(docStr,&error_string,&error_line,&error_column)) {
        LOG_ERROR(QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string));
        file.close();
        LOG_ERROR(tr("Failed to load plugin configuration from file: ") + file_name);
        return false;
    }
    file.close();

    // Interpret the loaded doc:
    QDomElement root = doc.documentElement();

    // Check the document version:
    if (root.hasAttribute("DocumentVersion")) {
        QString document_version = root.attribute("DocumentVersion");
        if (document_version.toInt() > QTILITIES_PLUGIN_CONFIG_FORMAT) {
            LOG_ERROR(QString(tr("The DocumentVersion of the input file is not supported by this version of your application. The document version of the input file is %1, while supported versions are versions up to %2. The document will not be parsed.")).arg(document_version.toInt()).arg(QTILITIES_PLUGIN_CONFIG_FORMAT));
            LOG_ERROR(tr("Failed to load plugin configuration from file: ") + file_name);
            return false;
        }
    } else {
        LOG_ERROR(QString(tr("The DocumentVersion of the input file could not be determined. This might indicate that the input file is in the wrong format. The document will not be parsed.")));
        LOG_ERROR(tr("Failed to load plugin configuration from file: ") + file_name);
        return false;
    }

    if (!inactive_plugins)
        d->set_inactive_plugins.clear();
    else
        inactive_plugins->clear();
    if (!filtered_plugins)
        d->set_filtered_plugins.clear();
    else
        filtered_plugins->clear();

    // Now check out all the children below the root node:
    QDomNodeList childNodes = root.childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "InactivePlugins") {
            QDomNodeList inactiveItems = child.childNodes();
            for(int i = 0; i < inactiveItems.count(); i++) {
                QDomNode inactiveNode = inactiveItems.item(i);
                QDomElement inactiveItem = inactiveNode.toElement();

                if (inactiveItem.isNull())
                    continue;

                if (inactiveItem.hasAttribute("Value")) {
                    QString plugin_name = inactiveItem.attribute("Value");
                    if (!inactive_plugins) {
                        if (!d->core_plugins.contains(plugin_name)) {
                            d->set_inactive_plugins << plugin_name;
                        } else
                            LOG_ERROR(QString(tr("ExtensionSystemCore::loadPluginConfiguration(): %1 is a core plugin. The plugin configuration attempted to set it as an inactive plugin. This is not allowed for core plugins.")).arg(plugin_name));
                    } else {
                        inactive_plugins->append(plugin_name);
                    }
                }
            }
        }

        if (child.tagName() == "FilteredPlugins") {
            QDomNodeList filteredItems = child.childNodes();
            for(int i = 0; i < filteredItems.count(); i++) {
                QDomNode filteredNode = filteredItems.item(i);
                QDomElement filteredItem = filteredNode.toElement();

                if (filteredItem.isNull())
                    continue;

                if (filteredItem.hasAttribute("Value")) {
                    QString plugin_name = filteredItem.attribute("Value");
                    if (!filtered_plugins) {
                        d->set_filtered_plugins << xmlToRegExp(plugin_name);
                    } else {
                        filtered_plugins->append(plugin_name);
                    }

                }
            }
        }
    }

    if (!inactive_plugins)
        d->set_inactive_plugins.removeDuplicates();
    else
        inactive_plugins->removeDuplicates();
    if (!filtered_plugins)
        d->set_filtered_plugins.removeDuplicates();
    else
        filtered_plugins->removeDuplicates();

    if (!d->is_initialized) {
        LOG_INFO(tr("Successfully loaded plugin configuration from file: ") + file_name);
        LOG_DEBUG("Inactive Plugins: " + d->set_inactive_plugins.join(","));
        LOG_DEBUG("Filtered Plugins: " + d->set_filtered_plugins.join(","));
        LOG_DEBUG("Core Plugins: " + d->core_plugins.join(","));
    }

    d->active_configuration_file = file_name;
    return true;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::setInactivePlugins(QStringList inactive_plugins) {
    if (d->is_initialized)
        return;

    d->set_inactive_plugins = inactive_plugins;
    d->set_inactive_plugins.removeDuplicates();
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::setFilteredPlugins(QStringList filtered_plugins) {
    if (d->is_initialized)
        return;

    d->set_filtered_plugins = filtered_plugins;
    d->set_filtered_plugins.removeDuplicates();
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::setCorePlugins(QStringList core_plugins) {
    if (!d->is_initialized) {
        d->core_plugins = core_plugins;

        // Make sure no core plugins are found in the inactive or filter plugin lists:
        foreach (QString core_plugin, d->core_plugins) {
            if (d->set_inactive_plugins.contains(core_plugin)) {
                d->set_inactive_plugins.removeOne(core_plugin);
                LOG_DEBUG("ExtensionSystemCore::setCorePlugins() removed plugin " + core_plugin + " from the list of inactive plugins.");
            }
        }
    }
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::handlePluginConfigurationChange(QList<QObject*> active_plugins, QList<QObject*> inactive_plugins) {
    Q_UNUSED(active_plugins)
    QStringList new_inactive_plugins;

    for (int i = 0; i < inactive_plugins.count(); i++) {
         IPlugin* iface = qobject_cast<IPlugin*> (inactive_plugins.at(i));
         if (iface) {
             // Check if any core plugins were made inactive, if so we notify the user and make them active again:
             if (d->core_plugins.contains(iface->pluginName())) {
                QMessageBox msgBox;
                msgBox.setText(QString(tr("%1 is a core plugin and must be active at all times.")).arg(iface->pluginName()));
                msgBox.exec();

                ObserverProperty category_property(OBJECT_ACTIVITY);
                category_property.setValue(true,d->plugins.observerID());
                Observer::setObserverProperty(iface->objectBase(),category_property);
                d->plugins.refreshViewsData();
             } else
                new_inactive_plugins << iface->pluginName();
         }
    }

    // If the inactive plugins are different from the current plugins:
    if (new_inactive_plugins != d->set_inactive_plugins) {
        // Now write the settings to the default configuration file, unless a config file was loaded, in that
        // case ask the user if they want to save it.
        if (savePluginConfiguration(d->active_configuration_file,&new_inactive_plugins,&d->set_filtered_plugins)) {
            // If there is a config widget, we update its status message:
            if (d->extension_system_config_widget)
                d->extension_system_config_widget->setStatusMessage("Restart Required");
        } else {
            // If there is a config widget, we update its status message:
            if (d->extension_system_config_widget)
                d->extension_system_config_widget->setStatusMessage("<font color='red'>Failed to save new configuration</font>");
        }
    }

}

void Qtilities::ExtensionSystem::ExtensionSystemCore::enablePluginActivityDisplay() {
    d->plugins.displayHints()->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::disablePluginActivityDisplay() {
    d->plugins.displayHints()->setActivityDisplayHint(ObserverHints::NoActivityDisplay);
}

bool Qtilities::ExtensionSystem::ExtensionSystemCore::isPluginActivityDisplayEnabled() const {
    if (d->plugins.displayHints()->activityDisplayHint() == ObserverHints::CheckboxActivityDisplay)
        return true;
    else
        return false;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::enablePluginActivityControl() {
    d->plugins.displayHints()->setActivityDisplayHint(ObserverHints::CheckboxActivityDisplay);
    d->plugins.displayHints()->setActivityControlHint(ObserverHints::CheckboxTriggered);
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::disablePluginActivityControl() {
    d->plugins.displayHints()->setActivityControlHint(ObserverHints::NoActivityControl);
}

bool Qtilities::ExtensionSystem::ExtensionSystemCore::isPluginActivityControlEnabled() const {
    if (d->plugins.displayHints()->activityControlHint() == ObserverHints::CheckboxTriggered)
        return true;
    else
        return false;
}

QString Qtilities::ExtensionSystem::ExtensionSystemCore::regExpToXml(QString pattern) const {
    pattern.replace(">","&gt;");
    return pattern;
}

QString Qtilities::ExtensionSystem::ExtensionSystemCore::xmlToRegExp(QString xml) const {
    xml.replace("&gt;",">");
    return xml;
}
