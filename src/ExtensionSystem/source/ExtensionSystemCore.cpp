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

#include <stdio.h>
#include <time.h>

using namespace QtilitiesCoreGui;
using namespace Qtilities::ExtensionSystem::Interfaces;
using namespace Qtilities::ExtensionSystem::Constants;
using namespace Qtilities::Logging::Constants;

struct Qtilities::ExtensionSystem::ExtensionSystemCorePrivateData {
    ExtensionSystemCorePrivateData() : plugins("Plugins"),
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
    d = new ExtensionSystemCorePrivateData;
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
}

Qtilities::ExtensionSystem::ExtensionSystemCore::~ExtensionSystemCore()
{
    if (d->extension_system_config_widget)
        delete d->extension_system_config_widget;
    delete d;
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::initialize() {
    // Start a processing cycle on the actions observer. Otherwise it will refresh the actions view everytime
    // an action is added in a plugin.
    OBJECT_MANAGER->objectPool()->startProcessingCycle();

    if (d->active_configuration_file.isEmpty()) {
        d->active_configuration_file = QtilitiesApplication::applicationDirPath() + QDir::separator() + "plugins" + QDir::separator() +  "default" + qti_def_SUFFIX_PLUGIN_CONFIG;
        QFileInfo fi(d->active_configuration_file);
        if (fi.exists())
            loadPluginConfiguration();
    } else
        loadPluginConfiguration();

    // Now go get all the plugins:
    d->pluginsDir = QDir(QCoreApplication::applicationDirPath());

    #if defined(Q_OS_WIN)
        if (d->pluginsDir.dirName().toLower() == QLatin1String("debug") || d->pluginsDir.dirName().toLower() == QLatin1String("release"))
            d->pluginsDir.cdUp();
    #elif defined(Q_OS_MAC)
        if (d->pluginsDir.dirName() == QLatin1String("MacOS")) {
            d->pluginsDir.cdUp();
            d->pluginsDir.cdUp();
            d->pluginsDir.cdUp();
        }
    #endif
    d->pluginsDir.cd("plugins");
    addPluginPath(d->pluginsDir.path());

    #ifndef QT_NO_DEBUG
    time_t start,end;
    time(&start);
    #endif

    emit pluginLoadingStarted();

    foreach (const QString& path, d->customPluginPaths) {
        emit newProgressMessage(QString("Searching for plugins in directory: %1").arg(path));
        LOG_INFO(QString("Searching for plugins in directory: %1").arg(path));
        QCoreApplication::processEvents();

        QDir dir(path);
        QStringList entry_list = dir.entryList(QDir::Files);
        QRegExp reg_exp("*SessionLogPlugin*",Qt::CaseInsensitive,QRegExp::Wildcard);
        int index_of_log = entry_list.indexOf(reg_exp);
        if (index_of_log != -1) {
            entry_list.move(index_of_log,0);
            //qDebug() << "Moving log plugin to the start of the plugin load-list.";
        }
        foreach (const QString& fileName, entry_list) {
            QFileInfo file_info(fileName);
            QString stripped_file_name = file_info.fileName();

            #if defined(Q_OS_UNIX)
                // Filter .so.x plugins on linux:
                if (file_info.completeSuffix().split(".").count() > 1 && file_info.completeSuffix().startsWith("so"))
                    continue;
            #endif

            bool is_filtered_plugin = false;
            foreach (const QString& expression, d->set_filtered_plugins) {
                QRegExp rx(expression);
                rx.setPatternSyntax(QRegExp::Wildcard);
                if (rx.exactMatch(stripped_file_name)) {
                    is_filtered_plugin = true;
                    break;
                }
            }

            if (!is_filtered_plugin) {
                if (QLibrary::isLibrary(dir.absoluteFilePath(fileName))) {
                    LOG_INFO("Found library: " + stripped_file_name);
                    QPluginLoader loader(dir.absoluteFilePath(fileName));
                    QObject *obj = loader.instance();
                    if (obj) {
                        // Check if the object implements IPlugin:
                        IPlugin* pluginIFace = qobject_cast<IPlugin*> (obj);
                        if (pluginIFace) {
                            emit newProgressMessage(QString("Loading plugin from file: %1").arg(stripped_file_name));
                            LOG_INFO(QString("Loading plugin from file: %1").arg(stripped_file_name));
                            QCoreApplication::processEvents();

                            // Check that the plugins with the same does not exist:
                            if (d->plugins.subjectNames().contains(pluginIFace->pluginName())) {
                                LOG_WARNING(QString("A plugin called %1 already exists. Plugin won't be loaded from file: %2").arg(pluginIFace->pluginName()).arg(stripped_file_name));
                                continue;
                            }

                            // Set the object name of the plugin:
                            pluginIFace->objectBase()->setObjectName(pluginIFace->pluginName());

                            // Set the category property of the plugin:
                            MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
                            category_property.setValue(qVariantFromValue(pluginIFace->pluginCategory()),d->plugins.observerID());
                            ObjectManager::setMultiContextProperty(pluginIFace->objectBase(),category_property);

                            // Store the file name:
                            pluginIFace->setPluginFileName(dir.absoluteFilePath(fileName));

                            // Do a plugin compatibility check here:
                            if (pluginIFace->pluginVersionInformation().hasSupportedVersions()) {
                                if (!pluginIFace->pluginVersionInformation().isSupportedVersion(QCoreApplication::applicationVersion())) {
                                    LOG_ERROR(QString("Incompatible plugin version of the following plugin detected (in file %1): Your application version (v%2) is not found in the list of compatible application versions that this plugin supports.").arg(stripped_file_name).arg(QCoreApplication::applicationVersion()));
                                    pluginIFace->addPluginState(IPlugin::IncompatibleState);
                                    pluginIFace->addErrorMessage(QString("Application version (v%2) is not found in the list of compatible application versions that this plugin supports.").arg(QCoreApplication::applicationVersion()));
                                }
                            }

                            d->plugins.attachSubject(obj);

                            bool is_inactive_plugin = false;
                            foreach (const QString& inactivePluginName, d->set_inactive_plugins) {
                                if (pluginIFace->pluginName() == inactivePluginName) {
                                    is_inactive_plugin = true;
                                    break;
                                }
                            }

                            if (!is_inactive_plugin) {
                                QStringList error_strings;
                                #ifdef QTILITIES_BENCHMARKING
                                time_t start_init,end_init;
                                time(&start_init);
                                #endif
                                if (!pluginIFace->initialize(QStringList(), &error_strings)) {
                                    LOG_ERROR("Plugin (" + stripped_file_name + ") failed during initialization with error(s): " + error_strings.join(","));
                                    pluginIFace->addPluginState(IPlugin::ErrorState);
                                    pluginIFace->addErrorMessages(error_strings);
                                } else {
                                    LOG_INFO("Successfully initialized plugin \"" + stripped_file_name + "\".");
                                }
                                #ifdef QTILITIES_BENCHMARKING
                                time(&end_init);
                                double diff_init = difftime(end_init,start_init);
                                LOG_TRACE(QString("Initializing plugin " + pluginIFace->pluginName() + " took " + QString::number(diff_init) + " seconds."));
                                #endif
                            }
                        } else {
                            LOG_ERROR("Plugin found which does not implement the expected IPlugin interface.");
                        }
                    } else {
                        LOG_ERROR(QString("Plugin could not be loaded: %1. Error: %2").arg(stripped_file_name).arg(loader.errorString()));
                        qDebug() << QString("Plugin could not be loaded: %1. Error: %2").arg(stripped_file_name).arg(loader.errorString());
                    }
                }
            } else {
                LOG_DEBUG("Skipped filtered plugin during plugin loading: " + stripped_file_name);
                d->current_filtered_plugins << stripped_file_name;
            }
        }

        emit newProgressMessage(QString("Finished loading plugins in directory:\n %1").arg(path));
    }

    // Now that all plugins were loaded, we call initializeDependencies() on all active ones:
    for (int i = 0; i < d->plugins.subjectCount(); ++i) {
        IPlugin* pluginIFace = qobject_cast<IPlugin*> (d->plugins.subjectAt(i));
        if (pluginIFace) {
            bool is_inactive_plugin = false;
            foreach (const QString& inactivePluginName, d->set_inactive_plugins) {
                if (pluginIFace->pluginName() == inactivePluginName) {
                    is_inactive_plugin = true;
                    break;
                }
            }

            if (!is_inactive_plugin) {
                QStringList error_strings;
                emit newProgressMessage(QString("Initializing dependencies in plugin: %1").arg(pluginIFace->pluginName()));
                QCoreApplication::processEvents();
                #ifdef QTILITIES_BENCHMARKING
                time_t start_init_dep,end_init_dep;
                time(&start_init_dep);
                #endif
                if (!pluginIFace->initializeDependencies(&error_strings)) {
                    pluginIFace->addPluginState(IPlugin::ErrorState);
                    pluginIFace->addErrorMessages(error_strings);
                    LOG_ERROR("Plugin (" + pluginIFace->pluginName() + ") failed during dependency initialization with error(s): " + error_strings.join(","));
                } else {
                    // Add it to the active list:
                    d->current_active_plugins << pluginIFace->pluginName();
                    LOG_INFO("Successfully initialized dependencies in plugin \"" + pluginIFace->pluginName() + "\".");
                }
                #ifdef QTILITIES_BENCHMARKING
                time(&end_init_dep);
                double diff_init_dep = difftime(end_init_dep,start_init_dep);
                qDebug() << QString("Initializing dependencies in plugin " + pluginIFace->pluginName() + " took " + QString::number(diff_init_dep) + " seconds.");
                #endif

                // Set the foreground color of core plugins:
                if (d->core_plugins.contains(pluginIFace->pluginName())) {
                    QBrush disabled_brush = QApplication::palette().brush(QPalette::Disabled,QPalette::Text);
                    SharedProperty property(qti_prop_FOREGROUND,disabled_brush);
                    ObjectManager::setSharedProperty(pluginIFace->objectBase(), property);
                }
            } else {
                // Set the default state of the plugin:
                pluginIFace->addPluginState(IPlugin::InActive);
                d->current_inactive_plugins << pluginIFace->pluginName();

                // Make it inactive:
                MultiContextProperty category_property(qti_prop_ACTIVITY_MAP);
                category_property.setValue(false,d->plugins.observerID());
                ObjectManager::setMultiContextProperty(pluginIFace->objectBase(),category_property);

                LOG_INFO(QString("Inactive plugin found which will not be initialized: %1").arg(pluginIFace->pluginName()));
            }
            OBJECT_MANAGER->registerObject(d->plugins.subjectAt(i),QtilitiesCategory("Core::Plugins (IPlugin)","::"));

            // Give the plugin an icon depending on its state:
            if (pluginIFace->pluginState() == IPlugin::Functional) {
                SharedProperty icon_property(qti_prop_DECORATION,QIcon(qti_icon_SUCCESS_16x16));
                ObjectManager::setSharedProperty(pluginIFace->objectBase(),icon_property);
            } else if (pluginIFace->pluginState() & IPlugin::ErrorState) {
                SharedProperty icon_property(qti_prop_DECORATION,QIcon(qti_icon_ERROR_16x16));
                ObjectManager::setSharedProperty(pluginIFace->objectBase(),icon_property);
            } else if (pluginIFace->pluginState() & IPlugin::IncompatibleState) {
                SharedProperty icon_property(qti_prop_DECORATION,QIcon(qti_icon_WARNING_16x16));
                ObjectManager::setSharedProperty(pluginIFace->objectBase(),icon_property);
            } else if (pluginIFace->pluginState() == IPlugin::InActive) {
                SharedProperty icon_property(qti_prop_DECORATION,QIcon(qti_icon_SUCCESS_16x16));
                ObjectManager::setSharedProperty(pluginIFace->objectBase(),icon_property);
            }
        }
    }

    #ifndef QT_NO_DEBUG
    time(&end);
    double diff = difftime(end,start);
    LOG_TRACE(QString("Extension system took %1 second(s) to load %2 plugins. They were initialized according to your active configuration set.").arg(QString::number(diff)).arg(QString::number(d->plugins.subjectCount())));
    #endif

    // Only connect here since the signal will be emitted in above code:
    connect(d->plugin_activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(handlePluginConfigurationChange(QList<QObject*>,QList<QObject*>)));

    // TODO: If there was errors or warnings, msgbox the user and ask if they want to review the errors.
    OBJECT_MANAGER->objectPool()->endProcessingCycle(false);

    emit newProgressMessage(QString("Finished loading plugins in %1 directories.").arg(d->customPluginPaths.count()));
    QCoreApplication::processEvents();

    d->is_initialized = true;

    emit pluginLoadingCompleted();
}

void Qtilities::ExtensionSystem::ExtensionSystemCore::finalize() {
    disconnect(d->plugin_activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),this,SLOT(handlePluginConfigurationChange(QList<QObject*>,QList<QObject*>)));

    // Loop through all plugins and call finalize on them:
    d->plugins.startProcessingCycle();
    int plugin_count = d->plugins.subjectCount();
    for (int i = 0; i < plugin_count; ++i) {
        IPlugin* pluginIFace = qobject_cast<IPlugin*> (d->plugins.subjectAt(0));

        if (pluginIFace) {
            emit newProgressMessage(QString("Finalizing plugin: %1").arg(pluginIFace->pluginName()));
            QCoreApplication::processEvents();

            // Check that it is active:
            if (d->current_active_plugins.contains(pluginIFace->pluginName()))
                pluginIFace->finalize();

            OBJECT_MANAGER->removeObject(d->plugins.subjectAt(0));
            d->plugins.detachSubject(d->plugins.subjectAt(0));
        }
    }
    d->plugins.endProcessingCycle(false);
}

QWidget* Qtilities::ExtensionSystem::ExtensionSystemCore::configWidget() {
    if (!d->extension_system_config_widget) {
        ObserverWidget* observer_widget = new ObserverWidget();
        observer_widget->setObserverContext(&d->plugins);
        d->treeModel = new PluginTreeModel;
        observer_widget->setCustomTreeModel(d->treeModel);
        observer_widget->initialize();
        observer_widget->layout()->setMargin(0);

        d->extension_system_config_widget = new ExtensionSystemConfig();
        d->extension_system_config_widget->configPageInitialize();
        d->extension_system_config_widget->setPluginListWidget(observer_widget);

        if (observer_widget->treeView()) {
            observer_widget->treeView()->expandAll();
            //observer_widget->treeView()->setRootIsDecorated(false);
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
    foreach (const QString& existing_path, d->customPluginPaths) {
        if (FileUtils::comparePaths(path,existing_path))
            return;
    }

    QDir dir(path);
    if (dir.exists()) {
        d->customPluginPaths << path;
        emit pluginPathsChanged(d->customPluginPaths);
    }
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

bool Qtilities::ExtensionSystem::ExtensionSystemCore::savePluginConfiguration(QString file_name,
                                                                              QStringList* inactive_plugins,
                                                                              QStringList* filtered_plugins,
                                                                              Qtilities::ExportVersion version,
                                                                              QString* errorMsg) const {
    if (file_name.isEmpty())
        file_name = d->active_configuration_file;

    QFile file(file_name);
    QFileInfo fi(file_name);

    QDir dir(fi.path());
    dir.mkpath(fi.path());

    if (!file.open(QFile::WriteOnly)) {
        if (errorMsg)
            *errorMsg = QString("Failed to open target plugin configuration file in write mode:<br>%1").arg(file_name);
        return false;
    }

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
    root.setAttribute("ExportVersion",QString::number(version));
    root.setAttribute("QtilitiesVersion",CoreGui::QtilitiesApplication::qtilitiesVersionString());
    doc.appendChild(root);

    // ---------------------------------------------------
    // Do the actual export:
    // ---------------------------------------------------
    // Do XML of inactive and filter lists:
    // Inactive Plugins:
    QDomElement inactive_node = doc.createElement("InactivePlugins");
    root.appendChild(inactive_node);
    foreach (const QString& name, final_inactive) {
        QDomElement inactive_item = doc.createElement("PluginName");
        inactive_item.setAttribute("Value",name);
        inactive_node.appendChild(inactive_item);
    }

    // Filtered Plugins:
    QDomElement filtered_node = doc.createElement("FilteredPlugins");
    root.appendChild(filtered_node);
    foreach (const QString& name, final_filtered) {
        QDomElement filtered_item = doc.createElement("Expression");
        filtered_item.setAttribute("Value",regExpToXml(name));
        filtered_node.appendChild(filtered_item);
    }

    // Put the complete doc in a string and save it to the file:
    // Still write it even if it fails so that we can check the output file for debugging purposes.
    QString docStr = doc.toString(2);
    docStr.prepend("<!--Created by " + QApplication::applicationName() + " v" + QApplication::applicationVersion() + " on " + QDateTime::currentDateTime().toString() + "-->\n");
    docStr.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    file.write(docStr.toUtf8());
    file.close();

    return true;
}

bool Qtilities::ExtensionSystem::ExtensionSystemCore::loadPluginConfiguration(QString file_name, QStringList* inactive_plugins, QStringList* filtered_plugins) {
    if (d->is_initialized && (!inactive_plugins || !filtered_plugins)) {
        LOG_DEBUG(QString("Failed to load plugin configuration from file: %1 . The extension system is already initialized.").arg(file_name));
        return false;
    }

    if (file_name.isEmpty())
        file_name = QDir::toNativeSeparators(d->active_configuration_file);

    // Load the file into doc:
    QDomDocument doc("QtilitiesPluginConfiguration");
    QFile file(file_name);

    if (!file.exists()) {
        LOG_WARNING(QString("Plugin configuration file does not exist, it will not be loaded: %1").arg(file_name));
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        LOG_WARNING(QString("Failed to load plugin configuration from file: %1").arg(file_name));
        return false;
    }

    QString docStr = file.readAll();
    QString error_string;
    int error_line;
    int error_column;
    if (!doc.setContent(docStr,&error_string,&error_line,&error_column)) {
        LOG_WARNING(QString("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3").arg(error_line).arg(error_column).arg(error_string));
        file.close();
        LOG_WARNING(QString("Failed to load plugin configuration from file: %1").arg(file_name));
        return false;
    }
    file.close();

    // ---------------------------------------------------
    // Inspect file format:
    // ---------------------------------------------------
    QDomElement root = doc.documentElement();
    Qtilities::ExportVersion read_version;
    // Check the document version:
    if (root.hasAttribute("ExportVersion")) {
        read_version = (Qtilities::ExportVersion) root.attribute("ExportVersion").toInt();
    } else {
        LOG_ERROR("The ExportVersion of the input file could not be determined. This might indicate that the input file is in the wrong format. The plugin configuration will not be parsed.");
        LOG_ERROR("Failed to load plugin configuration from file: " + file_name);
        return false;
    }

    // ---------------------------------------------------
    // Check if input format is supported:
    // ---------------------------------------------------
    bool is_supported_format = false;
    if (!(read_version < Qtilities::Qtilities_1_0 || read_version > Qtilities::Qtilities_Latest))
        is_supported_format = true;

    if (!is_supported_format) {
        LOG_ERROR(QString("Unsupported plugin configuration file found with export version: %1. The file will not be parsed.").arg(read_version));
        return false;
    }

    // ---------------------------------------------------
    // Do the actual import:
    // ---------------------------------------------------
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
    for(int i = 0; i < childNodes.count(); ++i) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == QLatin1String("InactivePlugins")) {
            QDomNodeList inactiveItems = child.childNodes();
            for(int i = 0; i < inactiveItems.count(); ++i) {
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
                            LOG_ERROR(QString("ExtensionSystemCore::loadPluginConfiguration(): %1 is a core plugin. The plugin configuration attempted to set it as an inactive plugin. This is not allowed for core plugins.").arg(plugin_name));
                    } else {
                        inactive_plugins->append(plugin_name);
                    }
                }
            }
        }

        if (child.tagName() == QLatin1String("FilteredPlugins")) {
            QDomNodeList filteredItems = child.childNodes();
            for(int i = 0; i < filteredItems.count(); ++i) {
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
        LOG_INFO("Successfully loaded plugin configuration from file: " + file_name);
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
        foreach (const QString& core_plugin, d->core_plugins) {
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

    for (int i = 0; i < inactive_plugins.count(); ++i) {
         IPlugin* iface = qobject_cast<IPlugin*> (inactive_plugins.at(i));
         if (iface) {
             // Check if any core plugins were made inactive, if so we notify the user and make them active again:
             if (d->core_plugins.contains(iface->pluginName())) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(tr("%1 is a core plugin and must be active at all times.").arg(iface->pluginName()));
                msgBox.exec();

                MultiContextProperty category_property(qti_prop_ACTIVITY_MAP);
                category_property.setValue(true,d->plugins.observerID());
                ObjectManager::setMultiContextProperty(iface->objectBase(),category_property);
                d->plugins.refreshViewsData();
             } else
                new_inactive_plugins << iface->pluginName();
         }
    }

    // If the inactive plugins are different from the current plugins:
    if (new_inactive_plugins != d->set_inactive_plugins) {
        // Now write the settings to the default configuration file, unless a config file was loaded, in that
        // case ask the user if they want to save it.
        QString errorMsg;
        if (savePluginConfiguration(d->active_configuration_file,
                                    &new_inactive_plugins,
                                    &d->set_filtered_plugins,
                                    Qtilities::Qtilities_Latest,
                                    &errorMsg)) {
            // If there is a config widget, we update its status message:
            if (d->extension_system_config_widget)
                d->extension_system_config_widget->setStatusMessage(tr("<font color=\"red\">Application Restart Required</font>"));
        } else {
            // If there is a config widget, we update its status message:
            if (d->extension_system_config_widget)
                d->extension_system_config_widget->setStatusMessage(tr("<font color='red'>Failed to save new configuration.<br>%1</font>").arg(errorMsg));
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
