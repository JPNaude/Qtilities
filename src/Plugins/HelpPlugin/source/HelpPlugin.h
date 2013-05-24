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
                #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
                Q_PLUGIN_METADATA(IID "org.qtilities.HelpPlugin")
                #endif

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
