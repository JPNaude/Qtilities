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

#ifndef PLUGIN_TEMPLATE_CORE_H
#define PLUGIN_TEMPLATE_CORE_H

#include "PluginTemplate_global.h"

#include <IPlugin>
#include <QtilitiesCategory>
#include <QObject>

namespace Qtilities {
    namespace Plugins {
        namespace Template {
            using namespace ExtensionSystem::Interfaces;
            using namespace Qtilities::Core;

            /*!
              \struct PluginTemplateData
              \brief The PluginTemplateData struct stores private data used by the PluginTemplate class.
             */
            struct PluginTemplateData;

            /*!
              \class PluginTemplate
              \brief A template which can is a starting point to creating plugins to be used with the Qtilities extension system.

              This plugin provides a template which can be used to create plugins based on the Qtilities
              extension system. This class is the entry point into the plugin and is an implementation of the
              Qtilities::ExtensionSystem::Interfaces::IPlugin interface.
             */
            class PLUGIN_TEMPLATE_SHARED_EXPORT PluginTemplate : public QObject, public IPlugin
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::ExtensionSystem::Interfaces::IPlugin)

            public:
                PluginTemplate(QObject* parent = 0);
                ~PluginTemplate();

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
                double pluginVersion() const;
                QStringList pluginCompatibilityVersions() const;
                QString pluginPublisher() const;
                QString pluginPublisherWebsite() const;
                QString pluginPublisherContact() const;
                QString pluginDescription() const;
                QString pluginCopyright() const;
                QString pluginLicense() const;

            private:
                PluginTemplateData* d;
            };
        }
    }
}

#endif // PLUGIN_TEMPLATE_CORE_H
