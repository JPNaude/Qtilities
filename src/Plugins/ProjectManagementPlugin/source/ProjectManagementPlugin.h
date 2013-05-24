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

#ifndef PROJECT_MANAGEMENT_PLUGIN_CORE_H
#define PROJECT_MANAGEMENT_PLUGIN_CORE_H

#include "ProjectManagementPlugin_global.h"

#include <IPlugin>
#include <QtilitiesCategory>
#include <IProject>
#include <QObject>

namespace Qtilities {
    namespace Plugins {
        namespace ProjectManagement {
            using namespace Qtilities::ExtensionSystem::Interfaces;
            using namespace Qtilities::ProjectManagement::Interfaces;
            using namespace Qtilities::Core;

            /*!
              \struct ProjectManagementPluginPrivateData
              \brief The ProjectManagementPluginPrivateData struct stores private data used by the ProjectManagementPlugin class.
             */
            struct ProjectManagementPluginPrivateData;

            /*!
              \class ProjectManagementPlugin
              \brief A plugin which adds project management capabilities to the application.
             */
            class PROJECT_MANAGEMENT_PLUGIN_SHARED_EXPORT ProjectManagementPlugin : public QObject, public IPlugin
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::ExtensionSystem::Interfaces::IPlugin)
                #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
                Q_PLUGIN_METADATA(IID "org.qtilities.ProjectManagementPlugin")
                #endif

            public:
                ProjectManagementPlugin(QObject* parent = 0);
                ~ProjectManagementPlugin();

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

            public slots:
                void handle_projectStateChanged();

            private:
                ProjectManagementPluginPrivateData* d;
            };
        }
    }
}

#endif // PROJECT_MANAGEMENT_PLUGIN_CORE_H
