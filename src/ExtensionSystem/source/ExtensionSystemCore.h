/****************************************************************************
**
** Copyright  © 2009-2010, Jaco Naude
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library;  If not, see <http://www.gnu.org/licenses/>.
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
