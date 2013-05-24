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

#ifndef SESSION_LOG_PLUGIN_CORE_H
#define SESSION_LOG_PLUGIN_CORE_H

#include "SessionLogPlugin_global.h"

#include <IPlugin>
#include <QtilitiesCategory>
#include <QObject>

namespace Qtilities {
    namespace Plugins {
        namespace SessionLog {
            using namespace ExtensionSystem::Interfaces;
            using namespace Qtilities::Core;

            /*!
              \struct SessionLogPluginPrivateData
              \brief The SessionLogPluginPrivateData struct stores private data used by the SessionLogPlugin class.
             */
            struct SessionLogPluginPrivateData;

            /*!
              \class SessionLogPlugin
              \brief A plugin which provides a session log mode to the application.
             */
            class SESSION_LOG_PLUGIN_SHARED_EXPORT SessionLogPlugin : public QObject, public IPlugin
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::ExtensionSystem::Interfaces::IPlugin)
                #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
                Q_PLUGIN_METADATA(IID "org.qtilities.SessionLogPlugin")
                #endif

            public:
                SessionLogPlugin(QObject* parent = 0);
                ~SessionLogPlugin();

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
                SessionLogPluginPrivateData* d;
            };
        }
    }
}

#endif // SESSION_LOG_PLUGIN_CORE_H
