/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#ifndef HELP_PLUGIN_H
#define HELP_PLUGIN_H

#include "HelpPlugin_global.h"

#include <IPlugin>
#include <QtilitiesCategory>
#include <QObject>

namespace Qtilities {
    namespace Plugins {
        namespace Help {
            using namespace ExtensionSystem::Interfaces;
            using namespace Qtilities::Core;

            /*!
              \struct HelpPluginData
              \brief The HelpPluginData struct stores private data used by the HelpPlugin class.
             */
            struct HelpPluginData;

            /*!
              \class HelpPlugin
              \brief A help plugin for a Qtilities application.

                This plugin provides a help mode in the %Qtilities main window which allows Qt Assistant help files to be shown.

                \image html plugins_help.jpg "Help Plugin Showing GUI Frontend For The Help Manager"

                In addition, it adds a configuration page for the help system to the user.

                \image html config_page_help.jpg "Help Configuration Page"

                The help plugin works through the help engine provided by the \p HELP_MANAGER (See Qtilities::CoreGui::QtilitiesApplication::helpManager() for more information).
             */
            class HELP_PLUGIN_SHARED_EXPORT HelpPlugin : public QObject, public IPlugin
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::ExtensionSystem::Interfaces::IPlugin)

            public:
                HelpPlugin(QObject* parent = 0);
                ~HelpPlugin();

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // IPlugin Implementation
                // --------------------------------------------
                bool initialize(const QStringList &arguments, QStringList *error_strings);
                bool initializeDependencies(QStringList *error_strings);
                void finalize();
                QString pluginName() const;
                QtilitiesCategory pluginCategory() const;
                VersionInformation pluginVersionInformation() const;
                QString pluginPublisher() const;
                QString pluginPublisherWebsite() const;
                QString pluginPublisherContact() const;
                QString pluginDescription() const;
                QString pluginCopyright() const;
                QString pluginLicense() const;

            private:
                HelpPluginData* d;
            };
        }
    }
}

#endif // HELP_PLUGIN_H
