/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#include "PluginTemplate.h"
#include "PluginTemplateConstants.h"

#include <ExtensionSystemConstants>
#include <QApplication>

#include <QtPlugin>
#include <QIcon>

using namespace Qtilities::ExtensionSystem;

struct Qtilities::Plugins::Template::PluginTemplatePrivateData {
    PluginTemplatePrivateData() {}
};

Qtilities::Plugins::Template::PluginTemplate::PluginTemplate(QObject* parent) : QObject(parent)
{
    d = new PluginTemplatePrivateData;
    setObjectName(pluginName());
}

Qtilities::Plugins::Template::PluginTemplate::~PluginTemplate()
{
    delete d;
}

bool Qtilities::Plugins::Template::PluginTemplate::initialize(const QStringList &arguments, QStringList *error_strings) {
    Q_UNUSED(arguments)
    Q_UNUSED(error_strings)

    return true;
}

bool Qtilities::Plugins::Template::PluginTemplate::initializeDependencies(QStringList *error_strings) {
    Q_UNUSED(error_strings)

    return true;
}

void Qtilities::Plugins::Template::PluginTemplate::finalize() {

}

QString Qtilities::Plugins::Template::PluginTemplate::pluginName() const {
    return "Plugin Template";
}

QtilitiesCategory Qtilities::Plugins::Template::PluginTemplate::pluginCategory() const {
    return QtilitiesCategory(QApplication::applicationName());
}

Qtilities::Core::VersionInformation Qtilities::Plugins::Template::PluginTemplate::pluginVersionInformation() const {
    VersionInformation version_info(qti_def_VERSION_MAJOR,qti_def_VERSION_MINOR,qti_def_VERSION_REVISION);
    return version_info;
}

QString Qtilities::Plugins::Template::PluginTemplate::pluginPublisher() const {
    return "Jaco Naude";
}

QString Qtilities::Plugins::Template::PluginTemplate::pluginPublisherWebsite() const {
    return "http://www.qtilities.org";
}

QString Qtilities::Plugins::Template::PluginTemplate::pluginPublisherContact() const {
    return "support@qtilities.org";
}

QString Qtilities::Plugins::Template::PluginTemplate::pluginDescription() const {
    return tr("An example Qtilities Extension System plugin.");
}

QString Qtilities::Plugins::Template::PluginTemplate::pluginCopyright() const {
    return QString(tr("Copyright") + " 2009-2013, Jaco Naude");
}

QString Qtilities::Plugins::Template::PluginTemplate::pluginLicense() const {
    return tr("See the Qtilities Libraries license");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    using namespace Qtilities::Plugins::Template;
    Q_EXPORT_PLUGIN2(PluginTemplate, PluginTemplate)
#endif
