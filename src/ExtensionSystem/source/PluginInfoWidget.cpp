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

#include "PluginInfoWidget.h"
#include "ui_PluginInfoWidget.h"

#include <QtilitiesCoreGuiConstants>

#include <QDesktopWidget>

using namespace Qtilities::CoreGui::Icons;

Qtilities::ExtensionSystem::PluginInfoWidget::PluginInfoWidget(IPlugin* plugin, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginInfoWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // Populate with plugin's contents:
    if (plugin) {
        ui->lblPluginName->setText(plugin->objectBase()->objectName());
        ui->lblContact->setText(plugin->pluginPublisherContact());
        ui->lblContact->setTextInteractionFlags(Qt::TextSelectableByMouse);
        ui->lblPublisher->setText(plugin->pluginPublisher());
        ui->lblState->setText(plugin->pluginStateString());
        ui->lblVersion->setText(QString("v%1").arg(plugin->pluginVersionInformation().version().toString()));
        ui->lblWebsite->setText(QString("<a href=\"%1\">%2</a>").arg(plugin->pluginPublisherWebsite()).arg(plugin->pluginPublisherWebsite()));
        ui->txtCopyright->setPlainText(plugin->pluginCopyright());
        ui->txtDescription->setPlainText(plugin->pluginDescription());
        ui->txtLicense->setPlainText(plugin->pluginLicense());
        ui->txtFileName->setText(plugin->pluginFileName());

        ui->listErrorMessages->clear();
        if (plugin->hasErrors()) {
            ui->listErrorMessages->addItems(plugin->errorMessages());
        } else {
            if (plugin->pluginState() == IPlugin::Functional)
                ui->listErrorMessages->addItem(tr("No errors detected."));
            else if (plugin->pluginState() == IPlugin::InActive)
                ui->listErrorMessages->addItem(tr("Inactive"));
        }

        if (plugin->pluginVersionInformation().hasSupportedVersions())
            ui->listCompatabilityVersions->addItems(plugin->pluginVersionInformation().supportedVersionString());
        else
            ui->listCompatabilityVersions->addItem(tr("This plugin does not depend on the application it is used in."));

        if (plugin->pluginState() == IPlugin::Functional)
            ui->lblStateImage->setPixmap(QIcon(qti_icon_SUCCESS_16x16).pixmap(16));
        else if (plugin->pluginState() & IPlugin::ErrorState)
            ui->lblStateImage->setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(16));
        else if (plugin->pluginState() & IPlugin::IncompatibleState)
            ui->lblStateImage->setPixmap(QIcon(qti_icon_WARNING_16x16).pixmap(16));
        else if (plugin->pluginState() == IPlugin::InActive) {
            ui->lblStateImage->setPixmap(QIcon(qti_icon_SUCCESS_16x16).pixmap(16));
            ui->lblStateImage->setEnabled(false);
        }
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
