/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#include "HelpPluginConfig.h"
#include "ui_HelpPluginConfig.h"
#include "HelpPluginConstants.h"

#include <QtilitiesApplication>

using namespace Qtilities::Plugins::Help::Constants;

Qtilities::Plugins::Help::HelpPluginConfig::HelpPluginConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpPluginConfig)
{
    ui->setupUi(this);

    if (ui->widgetFilesHolder->layout())
        delete ui->widgetFilesHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->widgetFilesHolder);
    layout->addWidget(&files_widget);
    layout->setMargin(0);
    files_widget.show();
    files_widget.setListType(StringListWidget::FilePaths);
    files_widget.setFileOpenDialogFilter("Help Files (*.qch)");

    connect(HELP_MANAGER,SIGNAL(registeredFilesChanged(QStringList)),SLOT(handleFilesChanged(QStringList)));
    handleFilesChanged(HELP_MANAGER->registeredFiles());
}

Qtilities::Plugins::Help::HelpPluginConfig::~HelpPluginConfig() {
    delete ui;
}

QIcon Qtilities::Plugins::Help::HelpPluginConfig::configPageIcon() const {
    return QIcon(HELP_MODE_ICON_48x48);
}

QWidget* Qtilities::Plugins::Help::HelpPluginConfig::configPageWidget() {
    return this;
}

QString Qtilities::Plugins::Help::HelpPluginConfig::configPageTitle() const {
    return tr("Help");
}

Qtilities::Core::QtilitiesCategory Qtilities::Plugins::Help::HelpPluginConfig::configPageCategory() const {
    return QtilitiesCategory("General");
}

void Qtilities::Plugins::Help::HelpPluginConfig::configPageApply() {
    // Only refresh when registered files changed:
    if (files_widget.stringList() == HELP_MANAGER->registeredFiles())
        return;

    disconnect(HELP_MANAGER,SIGNAL(registeredFilesChanged(QStringList)),this,SLOT(handleFilesChanged(QStringList)));
    HELP_MANAGER->clearRegisteredFiles(false);
    HELP_MANAGER->registerFiles(files_widget.stringList());
    connect(HELP_MANAGER,SIGNAL(registeredFilesChanged(QStringList)),SLOT(handleFilesChanged(QStringList)));
    HELP_MANAGER->writeSettings();
}

void Qtilities::Plugins::Help::HelpPluginConfig::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::Plugins::Help::HelpPluginConfig::handleFilesChanged(const QStringList& files) {
    // All paths in resources, thus starting with :/ are relative.
    QStringList read_only_paths;
    foreach (QString file, files) {
        if (file.startsWith(":/"))
            read_only_paths << HelpManager::formatFileName(file);
    }
    files_widget.setStringList(files);
    files_widget.setNonRemovableStringList(read_only_paths);
}
