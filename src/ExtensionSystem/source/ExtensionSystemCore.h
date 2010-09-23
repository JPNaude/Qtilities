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

#ifndef EXTENSIONCORE_H
#define EXTENSIONCORE_H

#include "ExtensionSystem_global.h"

#include <QObject>
#include <QStringList>

namespace Qtilities {
    namespace ExtensionSystem {
        /*!
          \struct ExtensionSystemCoreData
          \brief The ExtensionSystemCoreData class stores data used by the ExtensionSystemCore class.
         */
        struct ExtensionSystemCoreData;

        /*!
          \class ExtensionSystemCore
          \brief The ExtensionSystemCore class is the core of the Qtilities extension system library.

          The extension system core class is a singleton which isused to load plugins.

          See the \ref page_extension_system article for more information about the %Qtilities extension system.

          \todo
          - We never finalize the plugins, this must be added.
         */
        class EXTENSION_SYSTEM_SHARED_EXPORT ExtensionSystemCore : public QObject
        {
            Q_OBJECT

        public:
            static ExtensionSystemCore* instance();
            ~ExtensionSystemCore();

            //! Loads plugins.
            /*!
              Will load all plugins in the specified plugin paths. Once an IPlugin interface is discovered, initialize() will be
              called on it. When all the plugins in the system are loaded, the initializeDependancies() funciton will be called on each plugin.
              */
            void loadPlugins();

            //! Returns a widget with information about loaded plugins.
            QWidget* configWidget();
            //! Function to add a custom plugin path.
            /*!
              By default plugins are loaded only from the /plugins folder in the application directory.
              Using this function it is possible to add as many paths as you wish to the plugin loading process.

              In order for \p path to be added it must exist and it should not exist as a custom plugin path yet.

              \note Add all the paths you need before you call loadPlugins().
              */
            void addPluginPath(const QString& path);
            //! Function to get the paths from where plugins are loaded.
            QStringList pluginPaths() const;

        signals:
            //! Progress messages submitted during application startup.
            void newProgressMessage(const QString& message);

        private:
            ExtensionSystemCore(QObject* parent = 0);

            static ExtensionSystemCore* m_Instance;
            ExtensionSystemCoreData* d;
        };
    }
}

#define EXTENSION_SYSTEM Qtilities::ExtensionSystem::ExtensionSystemCore::instance()

#endif // EXTENSIONCORE_H
