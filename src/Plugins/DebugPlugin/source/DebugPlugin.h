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

#ifndef DEBUG_PLUGIN_CORE_H
#define DEBUG_PLUGIN_CORE_H

#include "DebugPlugin_global.h"

#include <IPlugin.h>
#include <QObject>

namespace Qtilities {
    namespace Plugins {
        namespace Debug {
            using namespace ExtensionSystem::Interfaces;

            /*!
              \struct DebugPluginData
              \brief The DebugPluginData struct stores private data used by the DebugPlugin class.
             */
            struct DebugPluginData;

            /*!
              \class DebugPlugin
              \brief A debug plugin which helps to debug %Qtilities applications.
             */
            class DEBUG_PLUGIN_SHARED_EXPORT DebugPlugin : public IPlugin
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::ExtensionSystem::Interfaces::IPlugin)

            public:
                DebugPlugin(QObject* parent = 0);
                ~DebugPlugin();

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // IPlugin Implementation
                // --------------------------------------------
                bool initialize(const QStringList &arguments, QString *errorString);
                bool initializeDependancies(QString *errorString);
                void finalize();
                double pluginVersion() const;
                QStringList pluginCompatibilityVersions() const;
                QString pluginPublisher() const;
                QString pluginPublisherWebsite() const;
                QString pluginPublisherContact() const;
                QString pluginDescription() const;
                QString pluginCopyright() const;
                QString pluginLicense() const;

            private:
                DebugPluginData* d;
            };
        }
    }
}

#endif // DEBUG_PLUGIN_CORE_H
