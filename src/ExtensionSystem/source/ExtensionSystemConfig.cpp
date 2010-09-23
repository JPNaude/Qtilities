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

#include "ExtensionSystemConfig.h"
#include "ui_ExtensionSystemConfig.h"
#include "ExtensionSystemCore.h"
#include "PluginInfoWidget.h"

#include <QtilitiesCoreGuiConstants>
using namespace Qtilities::CoreGui::Icons;

Qtilities::ExtensionSystem::ExtensionSystemConfig::ExtensionSystemConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExtensionSystemConfig)
{
    ui->setupUi(this);
    if (ExtensionSystemCore::instance()->pluginPaths().count() == 1)
        ui->labelPluginPaths->setText(QString(tr("Plugins loaded from %1 path.")).arg(ExtensionSystemCore::instance()->pluginPaths().count()));
    else
        ui->labelPluginPaths->setText(QString(tr("Plugins loaded from %1 paths.")).arg(ExtensionSystemCore::instance()->pluginPaths().count()));
    ui->listPluginPaths->addItems(ExtensionSystemCore::instance()->pluginPaths());
    connect(ui->btnPluginDetails,SIGNAL(clicked()),SLOT(handleBtnDetailsClicked()));
}

Qtilities::ExtensionSystem::ExtensionSystemConfig::~ExtensionSystemConfig()
{
    delete ui;
}

QIcon Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageIcon() const {
    return QIcon(ICON_CONFIG_PLUGINS_48x48);
}

QWidget* Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageWidget() {
    return this;
}

QString Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageTitle() const {
    return tr("Plugins");
}

Qtilities::Core::QtilitiesCategory Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageCategory() const {
    return QtilitiesCategory(tr("General"));
}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageApply() {

}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::setPluginListWidget(QWidget* plugin_list_widget) {
    // Get config page from extension system
    if (ui->widgetPluginListHolder->layout())
        delete ui->widgetPluginListHolder->layout();

    QHBoxLayout* new_layout = new QHBoxLayout(ui->widgetPluginListHolder);
    new_layout->addWidget(plugin_list_widget);
    new_layout->setMargin(0);

    observer_widget = qobject_cast<ObserverWidget*> (plugin_list_widget);
    if (observer_widget)
        connect(observer_widget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),SLOT(handleSelectionChanged(QList<QObject*>)));
}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::handleBtnDetailsClicked() {
    if (observer_widget->selectedObjects().count() == 1) {
        IPlugin* plugin_iface = qobject_cast<IPlugin*> (observer_widget->selectedObjects().front());
        if (plugin_iface) {
            PluginInfoWidget* info_widget = new PluginInfoWidget(plugin_iface);
            info_widget->show();
        }
    }

}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::handleSelectionChanged(QList<QObject*> selection) {
    if (selection.count() == 1)
        ui->btnPluginDetails->setEnabled(true);
    else
        ui->btnPluginDetails->setEnabled(false);
}
