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

#ifndef EXTENSIONCORE_H
#define EXTENSIONCORE_H

#include "ExtensionSystem_global.h"

#include <ObserverHints>

#include <QObject>
#include <QStringList>

namespace Qtilities {
    namespace ExtensionSystem {
        using namespace Qtilities::Core;

        /*!
          \struct ExtensionSystemCoreData
          \brief The ExtensionSystemCoreData class stores data used by the ExtensionSystemCore class.
         */
        struct ExtensionSystemCoreData;

        /*!
          \class ExtensionSystemCore
          \brief The ExtensionSystemCore class is the core of the Qtilities extension system library.

          The extension system core class is a singleton which is used to load plugins.

          See the \ref page_extension_system article for more information about the %Qtilities extension system.

          \todo
          - We never finalize the plugins, this must be added.
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
              Will load all plugins in the specified plugin paths. Once an IPlugin interface is discovered, initialize() will be
              called on it. When all the plugins in the system are loaded, the initializeDependancies() funciton will be called on each plugin.

              When enablePluginActivityControl() is true, the initialize() function will attempt to load the default plugin configuration set
              in the file specified by activePluginConfigurationFile(). If you want to load a file other than the default configuration file
              you can set it using setActivePluginConfigurationFile() before calling initialize().

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

            // --------------------------------
            // Plugin Paths
            // --------------------------------
            //! Function to add a custom plugin path.
            /*!
              By default plugins are loaded only from the /plugins folder in the application directory.
              Using this function it is possible to add as many paths as you wish to the plugin loading process.

              In order for \p path to be added it must exist and it should not exist as a custom plugin path yet.

              \note Add all the needed paths before calling loadPlugins().

              \sa pluginPaths()
              */
            void addPluginPath(const QString& path);
            //! Function to get the paths from where plugins are loaded.
            /*!
              \sa addPluginPath()
              */
            QStringList pluginPaths() const;

            // --------------------------------
            // Plugin Configuration Sets
            // --------------------------------
            //! Gets the name of the current plugin configuration set's file.
            /*!
              When loadPluginConfiguration() is loaded with a valid file, this function will return the file name that
              was loaded. Otherwise it will return \p App_Path/plugins/default.pconfig.
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
              \note This function only provided usefull information after initialize() have been called.
              */
            QStringList activePlugins() const;
            //! Function to return the names of all plugins that are loaded but not initialized at present.
            /*!
              \note This function only provided usefull information after initialize() have been called.
              */
            QStringList inactivePlugins() const;
            //! Function to return the file names of all plugins which as not loaded during the initialize() function call.
            QStringList filteredPlugins() const;
            //! Function to return the file names of all core plugins.
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
            //! Function to set the file names of all plugins which as not loaded during the initialize() function call.
            /*!
              Sets a list of <b>file names</b> which will be evaluated during initialize(). Each plugin file that
              is found will be checked if it starts with any of the file names specified and if so, it will not be loaded.

              \note This function only does something usefull when called before initialize().
              */
            void setFilteredPlugins(QStringList filtered_plugins);
            //! Function to set the names of all plugins which should be handled as core plugins. That is, they cannot be made inactive by an user.
            /*!
              Sets a list of plugin names which <b>corresponds to the IPlugin::pluginName()</b> implementation
              of the plugins which should be handled as core plugins.

              \note This function only does something usefull when called before initialize().
              */
            void setCorePlugins(QStringList core_plugins);

            //! Function which load a plugin configuration file.
            /*!
              \param file_name When empty the file name in activePluginConfigurationFile() will be used.

              \note This function only does something usefull when called before initialize().
              */
            bool loadPluginConfiguration(QString file_name = QString());
            //! Function which saves a plugin configuration file.
            /*!
              \param file_name When empty the file name in activePluginConfigurationFile() will be used.
              */
            bool savePluginConfiguration(QString file_name = QString()) const;

        signals:
            //! Progress messages submitted during application startup.
            void newProgressMessage(const QString& message);

        public slots:
            //! Handle plugin configuration changes.
            void handlePluginConfigurationChange(QList<QObject*> active_plugins, QList<QObject*> inactive_plugins);

        private:
            ExtensionSystemCore(QObject* parent = 0);

            static ExtensionSystemCore* m_Instance;
            ExtensionSystemCoreData* d;
        };
    }
}

#define EXTENSION_SYSTEM Qtilities::ExtensionSystem::ExtensionSystemCore::instance()

#endif // EXTENSIONCORE_H
