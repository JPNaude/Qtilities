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

#include "PluginTemplate.h"
#include "PluginTemplateConstants.h"

#include <ExtensionSystemConstants.h>
#include <Qtilities.h>
#include <QtilitiesCoreApplication.h>

#include <QtPlugin>
#include <QIcon>

using namespace Qtilities::ExtensionSystem::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::Plugins::Template::PluginTemplateData {
    PluginTemplateData() {}
};

Qtilities::Plugins::Template::PluginTemplate::PluginTemplate(QObject* parent) : QObject(parent)
{
    d = new PluginTemplateData;
    setObjectName(pluginName());
}

Qtilities::Plugins::Template::PluginTemplate::~PluginTemplate()
{
    delete d;
}

bool Qtilities::Plugins::Template::PluginTemplate::initialize(const QStringList &arguments, QString *errorString) {
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    return true;
}

bool Qtilities::Plugins::Template::PluginTemplate::initializeDependancies(QString *errorString) {
    Q_UNUSED(errorString)

    return true;
}

void Qtilities::Plugins::Template::PluginTemplate::finalize() {

}

QString Qtilities::Plugins::Template::PluginTemplate::pluginName() const {
    return "Plugin Template";
}

QtilitiesCategory Qtilities::Plugins::Template::PluginTemplate::pluginCategory() const {
    return QtilitiesCategory(tr("General"));
}

double Qtilities::Plugins::Template::PluginTemplate::pluginVersion() const {
    return (QString("%1.%2").arg(PLUGIN_TEMPLATE_VERSION_MAJOR).arg(PLUGIN_TEMPLATE_VERSION_MINOR)).toDouble();
}

QStringList Qtilities::Plugins::Template::PluginTemplate::pluginCompatibilityVersions() const {
    // Returning an empty QStringList() indicates that the plugin can be used in any application, thus
    // it is not dependent on the application it is used. Examples are the Session Log and Project
    // Management plugins that comes with %Qtilities.
    return QStringList();
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
    return QString(tr("Copyright") + " 2010, Jaco Naude");
}

QString Qtilities::Plugins::Template::PluginTemplate::pluginLicense() const {
    return tr("See the Qtilities Libraries license");
}

using namespace Qtilities::Plugins::Template;
Q_EXPORT_PLUGIN2(PluginTemplate, PluginTemplate);
