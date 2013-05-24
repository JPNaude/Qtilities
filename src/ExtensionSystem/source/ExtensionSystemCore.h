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

#ifndef EXTENSIONCORE_H
#define EXTENSIONCORE_H

#include "ExtensionSystem_global.h"

#include <ObserverHints>

#include <QObject>
#include <QStringList>

namespace Qtilities {
    namespace ExtensionSystem {
        using namespace Qtilities::Core;
        namespace Interfaces {
            class IPlugin;
        }

        /*!
          \struct ExtensionSystemCorePrivateData
          \brief The ExtensionSystemCorePrivateData class stores data used by the ExtensionSystemCore class.
         */
        struct ExtensionSystemCorePrivateData;

        /*!
          \class ExtensionSystemCore
          \brief The ExtensionSystemCore class is the core of the Qtilities extension system library.

          The extension system core class is a singleton which is used to load plugins.

          See the \ref page_extension_system article for more information about the %Qtilities extension system.
         */
        class EXTENSION_SYSTEM_SHARED_EXPORT ExtensionSystemCore : public QObject
        {
            Q_OBJECT

        public:
            // --------------------------------
            // Core Functions
            // --------------------------------
            static ExtensionSystemCore* instance();
            ~ExtensionSystemCore();

            //! Initializes the plugin manager by loading all found plugins.
            /*!
              Will load all plugins in the specified plugin paths. Once an IPlugin interface is discovered, initialize() will be called on it. When all the plugins in the system are loaded, the initializeDependencies() funciton will be called on each plugin.

              When enablePluginActivityControl() is true, the initialize() function will attempt to load the default plugin configuration set in the file specified by activePluginConfigurationFile(). If you want to load a file other than the default configuration file you can set it using setActivePluginConfigurationFile() before calling initialize().

              It is important to note that the IPlugin implementations loaded through initialize() should live in the same thread as the extension system core instance. Thus you should not move your plugin to a different thread during its lifetime.

              \sa pluginPaths()
              */
            void initialize();
            //! Finalizes the plugin manager by unloading and finalizing all plugins.
            /*!
              Will call finalize on all plugins in the order that they were loaded. Thus the order can be random.
              */
            void finalize();

            //! Returns a widget with information about loaded plugins.
            QWidget* configWidget();

            //! Function which finds the plugin with the given \p plugin_name and returns its plugin interface. If no plugin exists with that name in the set of loaded plugins (active and inactive plugins), null is returned.
            Interfaces::IPlugin* findPlugin(const QString& plugin_name) const;

            // --------------------------------
            // Plugin Paths
            // --------------------------------
            //! Function to add a custom plugin path.
            /*!
              By default plugins are loaded only from the /plugins folder in the application directory.
              Using this function it is possible to add as many paths as you wish to the plugin loading process.

              In order for \p path to be added it must exist and it should not exist as a custom plugin path yet.

              \note Add all the needed paths before calling loadPlugins().

              \sa pluginPaths(), pluginPathsChanged()
              */
            void addPluginPath(const QString& path);
            //! Function to get the paths from where plugins are loaded.
            /*!
              \sa addPluginPath(), pluginPathsChanged()
              */
            QStringList pluginPaths() const;

            // --------------------------------
            // Plugin Configuration Sets
            // --------------------------------
            //! Gets the name of the current plugin configuration set's file.
            /*!
              When loadPluginConfiguration() is loaded with a valid file, this function will return the file name that
              was loaded. Otherwise it will return "App_Path/plugins/default" + qti_def_SUFFIX_PLUGIN_CONFIG.
              */
            QString activePluginConfigurationFile() const;
            //! Sets the name of the current plugin configuration set's file.
            void setActivePluginConfigurationFile(const QString& file_name);

            //! Disables plugin activity display in the plugin details widget.
            void enablePluginActivityDisplay();
            //! Disables plugin activity display in the plugin details widget.
            void disablePluginActivityDisplay();
            //! Indicates if plugin activity is displayed in the plugin details widget.
            /*!
              False by default.

              \sa enablePluginActivityDisplay(), disablePluginActivityDisplay(), setInactivePlugins()
              */
            bool isPluginActivityDisplayEnabled() const;

            //! Disables plugin activity control in the plugin details widget.
            void enablePluginActivityControl();
            //! Disables plugin activity control in the plugin details widget.
            void disablePluginActivityControl();
            //! Indicates if plugin activity can be changed by the user in the plugin details widget.
            /*!
              False by default.

              \note isPluginActivityDisplayEnabled() must also be true for the activity control to be available, thus
              if plugin activity display is not yet enabled, this function call will enabled it.

              \sa enablePluginActivityControl(), disablePluginActivityControl(), setInactivePlugins()
              */
            bool isPluginActivityControlEnabled() const;

            //! Function to return the names of all plugins that are loaded and initialized at present.
            /*!
              \note This function only provides usefull information after initialize() have been called.

              \sa activePlugins()
              */
            QStringList activePlugins() const;
            //! Function to return the names of all plugins that are loaded but not initialized at present.
            /*!
              \note This function only provides usefull information after initialize() have been called.

              \sa inactivePlugins()
              */
            QStringList inactivePlugins() const;
            //! Function to return the file names of all plugins which as not loaded during the initialize() function call.
            /*!
              \sa setFilteredPlugins()
              */
            QStringList filteredPlugins() const;
            //! Function to return the names of all core plugins.
            /*!
              \sa setCorePlugins()
              */
            QStringList corePlugins() const;

            //! Function to set the names of all plugins that is loaded but not initialized at present.
            /*!
              Sets a list of plugin names which <b>corresponds to the IPlugin::pluginName()</b> implementation
              of the plugins which should be inactive on startup.

              \note This function only does something usefull when called before initialize().
              */
            void setInactivePlugins(QStringList inactive_plugins);
            //! Function to set the file names of all plugins which should not be loaded during the initialize() function call.
            /*!
              Sets a list of wildcard mode <b>regular expressions</b> which will be evaluated during initialize(). Each plugin file that
              is found will be checked against this wildcard mode regular expression and if it matches the expression, it will not be loaded. See QRegExp::Wildcard for more information.

              \note This function only does something usefull when called before initialize().
              */
            void setFilteredPlugins(QStringList filtered_plugins);
            //! Function to set the names of all plugins which should be handled as core plugins. That is, they cannot be made inactive by an user.
            /*!
              Sets a list of plugin names which <b>corresponds to the IPlugin::pluginName()</b> implementation
              of the plugins which should be handled as core plugins. Note that the setInactivePlugins() function checks that you don't set a core plugin as inactive but the setFilteredPlugins() function does not do that. Thus it is possible to filter a core plugin. The developer must make sure this does not happen.

              \note This function only does something usefull when called before initialize().

              Below is an example of how you can verify that all core plugins loaded successfully:
\code
QStringList core_plugins;
core_plugins << "Project Management Plugin";
EXTENSION_SYSTEM->setCorePlugins(core_plugins);
EXTENSION_SYSTEM->initialize();

// Give error message if any of the core plugins were not found:
bool core_plugins_ok = true;
QStringList missing_plugins;
foreach (QString core_plugin, core_plugins) {
    if (!EXTENSION_SYSTEM->activePlugins().contains(core_plugin)) {
        core_plugins_ok = false;
        missing_plugins << core_plugin;
    }
}

if (!core_plugins_ok) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("All Core Plugins Not Found");
    msgBox.setText("All core plugins could not be loaded properly. Everything might not work as expected.<br><br>The following core plugin(s) were not found: <br><br>" + missing_plugins.join("<br>") + "<br><br>The following plugin(s) were loaded and set active: <br><br>" + EXTENSION_SYSTEM->activePlugins().join("<br>"));
    msgBox.exec();
}
\endcode
              */
            void setCorePlugins(QStringList core_plugins);

            //! Function to return the filter expression of the current plugin configuration set.
            QStringList inactivePluginsCurrentSet() const;
            //! Function to return the filter expression of the current plugin configuration set.
            QStringList filteredPluginsCurrentSet() const;

            //! Function which loads a plugin configuration file.
            /*!
                Loads a plugin configuration from a file. Before initialization the function can be called with \p inactive_plugins = 0 and \p filtered_plugins = 0 in order to load the plugin configuration to be used in initialize(). After initialization you must pass proper values for \p inactive_plugins and \p filtered_plugins in order for the function to work. False will be returned when this requirement is not met. The reason for this is that the extension system's configuration set cannot be changed again after initialization since the plugins would have been loaded at that time.

                \param file_name When empty the file name in activePluginConfigurationFile() will be used. When empty the default plugin configuration file is used. The default plugin file is located at QtilitiesApplication::applicationSessionPath()/Plugins/default.pconfig. For more information, see Qtilities::CoreGui::QtilitiesApplication::applicationSessionPath().
                \param inactive_plugins The QStringList to be populated with the inactive plugin names found in the input file. When null, the inactive plugins in the extension system's active plugin configuration will be set. See setInactivePlugins() for more information.
                \param filtered_plugins The QStringList to be populated with the plugin filter expressions found in the input file. When null, the filtered plugins in the extension system's active plugin configuration will be set. See setFilteredPlugins() for more information.

                \sa savePluginConfiguration()
              */
            bool loadPluginConfiguration(QString file_name = QString(), QStringList* inactive_plugins = 0, QStringList* filtered_plugins = 0);
            //! Function which saves a plugin configuration file for the given parameters.
            /*!
                \param file_name When empty the file name in activePluginConfigurationFile() will be used. When empty the default plugin configuration file is used. The default plugin file is located at QtilitiesApplication::applicationSessionPath()/Plugins/default.pconfig. For more information, see Qtilities::CoreGui::QtilitiesApplication::applicationSessionPath().
                \param inactive_plugins The list of plugins which must be inactive plugins in the saved configuration file. When null, extension system's active plugin configuration's inactive plugins will be used. See inactivePlugins() for more information.
                \param filtered_plugins The list of plugin filter expressions which must be used the saved configuration file. When null, extension system's active plugin configuration's filtered plugins will be used. See filteredPlugins() for more information.

                \sa loadPluginConfiguration()
              */
            bool savePluginConfiguration(QString file_name = QString(),
                                         QStringList* inactive_plugins = 0,
                                         QStringList* filtered_plugins = 0,
                                         Qtilities::ExportVersion version = Qtilities::Qtilities_Latest,
                                         QString* errorMsg = 0) const;

        signals:
            //! Progress messages submitted during application startup.
            void newProgressMessage(const QString& message);
            //! Signal which is emitted when the plugin loading started.
            /*!
             * <i>This class was added in %Qtilities v1.1.</i>
             */
            void pluginLoadingStarted();
            //! Signal which is emitted when the plugin loading completed.
            /*!
             * <i>This class was added in %Qtilities v1.1.</i>
             */
            void pluginLoadingCompleted();
            //! Signal which is emitted when the plugin paths changed.
            /*!
             * <i>This class was added in %Qtilities v1.3.</i>
             */
            void pluginPathsChanged(const QStringList& plugin_paths);

        public slots:
            //! Handle plugin configuration changes.
            void handlePluginConfigurationChange(QList<QObject*> active_plugins, QList<QObject*> inactive_plugins);

        private:
            QString regExpToXml(QString pattern) const;
            QString xmlToRegExp(QString xml) const;
            ExtensionSystemCore(QObject* parent = 0);

            static ExtensionSystemCore* m_Instance;
            ExtensionSystemCorePrivateData* d;
        };
    }
}

#define EXTENSION_SYSTEM Qtilities::ExtensionSystem::ExtensionSystemCore::instance()

#endif // EXTENSIONCORE_H
