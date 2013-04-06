/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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

#include <QDesktopWidget>

#include "ExtensionSystemConfig.h"
#include "ui_ExtensionSystemConfig.h"
#include "ExtensionSystemCore.h"
#include "PluginInfoWidget.h"

#include <QtilitiesCoreGuiConstants>
using namespace Qtilities::CoreGui::Icons;

Qtilities::ExtensionSystem::ExtensionSystemConfig::ExtensionSystemConfig(QWidget *parent) :
    QWidget(parent),
    initialized(false),
    ui(new Ui::ExtensionSystemConfig)
{
    ui->setupUi(this);
}

Qtilities::ExtensionSystem::ExtensionSystemConfig::~ExtensionSystemConfig()
{
    delete ui;
}

QIcon Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageIcon() const {
    return QIcon(qti_icon_CONFIG_PLUGINS_48x48);
}

QWidget* Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageWidget() {
    return this;
}

QString Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageTitle() const {
    return tr(qti_config_page_PLUGINS);
}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageInitialize() {
    if (initialized)
        return;

    // Put the widget in the center of the screen:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());
    connect(ui->btnPluginDetails,SIGNAL(clicked()),SLOT(handleBtnDetailsClicked()));

    connect(EXTENSION_SYSTEM,SIGNAL(pluginPathsChanged(QStringList)),SLOT(updatePluginPaths()));
    updatePluginPaths();

    initialized = true;
}

Qtilities::Core::QtilitiesCategory Qtilities::ExtensionSystem::ExtensionSystemConfig::configPageCategory() const {
    if (IConfigPage::configPageCategory().isEmpty())
        return QtilitiesCategory(tr(qti_config_page_DEFAULT_CAT));
    else
        return IConfigPage::configPageCategory();
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
    if (observer_widget) {
        connect(observer_widget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),SLOT(handleSelectionChanged(QList<QObject*>)));
        connect(observer_widget,SIGNAL(doubleClickRequest(QObject*)),SLOT(handleSelectionDoubleClicked(QObject*)));
    }
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
    if (selection.count() == 1) {
        IPlugin* plugin_iface = qobject_cast<IPlugin*> (observer_widget->selectedObjects().front());
        if (plugin_iface)
            ui->btnPluginDetails->setEnabled(true);
        else
            ui->btnPluginDetails->setEnabled(false);
    } else
        ui->btnPluginDetails->setEnabled(false);
}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::handleSelectionDoubleClicked(QObject* selection) {
    IPlugin* plugin_iface = qobject_cast<IPlugin*> (selection);
    if (plugin_iface) {
        PluginInfoWidget* info_widget = new PluginInfoWidget(plugin_iface);
        info_widget->show();
    }
}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::setStatusMessage(const QString& status_message) {
    ui->lblStatusMessage->setText(status_message);
}

void Qtilities::ExtensionSystem::ExtensionSystemConfig::updatePluginPaths() {
    ui->listPluginPaths->clear();
    if (EXTENSION_SYSTEM->pluginPaths().count() == 1)
        ui->labelPluginPaths->setText(QString(tr("Plugins loaded from %1 path.")).arg(EXTENSION_SYSTEM->pluginPaths().count()));
    else
        ui->labelPluginPaths->setText(QString(tr("Plugins loaded from %1 paths.")).arg(EXTENSION_SYSTEM->pluginPaths().count()));
    ui->listPluginPaths->addItems(EXTENSION_SYSTEM->pluginPaths());
}
