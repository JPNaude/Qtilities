/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
**
** This file is part of the Qtilities Debug Plugin.
** See Dependencies.pri for the licensing requirements of the Debug plugin.
** Alternatively, see http://www.qtilities.org/licensing.html.
**
****************************************************************************/

#ifndef DEBUG_PLUGIN_CORE_H
#define DEBUG_PLUGIN_CORE_H

#include "DebugPlugin_global.h"

#include <IPlugin>
#include <QtilitiesCategory>
#include <QObject>

namespace Qtilities {
    namespace Plugins {
        namespace Debug {
            using namespace ExtensionSystem::Interfaces;
            using namespace Qtilities::Core;

            /*!
              \struct DebugPluginPrivateData
              \brief The DebugPluginPrivateData struct stores private data used by the DebugPlugin class.
             */
            struct DebugPluginPrivateData;

            /*!
              \class DebugPlugin
              \brief A debug plugin which helps to debug %Qtilities applications.

              For more information see the \ref page_debugging article.
             */
            class DEBUG_PLUGIN_SHARED_EXPORT DebugPlugin : public QObject, public IPlugin
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
                DebugPluginPrivateData* d;
            };
        }
    }
}

#endif // DEBUG_PLUGIN_CORE_H
