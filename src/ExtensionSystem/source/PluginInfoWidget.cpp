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

#include "PluginInfoWidget.h"
#include "ui_PluginInfoWidget.h"

#include <QDesktopWidget>

Qtilities::ExtensionSystem::PluginInfoWidget::PluginInfoWidget(IPlugin* plugin, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginInfoWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // Populate with plugin's contents:
    if (plugin) {
        ui->lblPluginName->setText(plugin->objectBase()->objectName());
        ui->lblErrorString->setText(plugin->errorString());
        ui->lblContact->setText(plugin->pluginPublisherContact());
        ui->lblPublisher->setText(plugin->pluginPublisher());
        ui->lblState->setText(plugin->pluginStateString());
        ui->lblVersion->setText(QString("v%1").arg(plugin->pluginVersion()));
        ui->lblWebsite->setText(QString("<a href=\"%1\">%2</a>").arg(plugin->pluginPublisherWebsite()).arg(plugin->pluginPublisherWebsite()));
        ui->txtCopyright->setPlainText(plugin->pluginCopyright());
        ui->txtDescription->setPlainText(plugin->pluginDescription());
        ui->txtLicense->setPlainText(plugin->pluginLicense());
        ui->txtFileName->setText(plugin->pluginFileName());
    }

    connect(ui->btnClose,SIGNAL(clicked()),SLOT(close()));
    // Put the widget in the center of the screen:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());
}

Qtilities::ExtensionSystem::PluginInfoWidget::~PluginInfoWidget()
{
    delete ui;
}

void Qtilities::ExtensionSystem::PluginInfoWidget::changeEvent(QEvent *e)
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
