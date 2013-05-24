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

#include "HelpPluginConfig.h"
#include "ui_HelpPluginConfig.h"
#include "HelpPluginConstants.h"

#include <QtilitiesApplication>
#include <QtilitiesCoreGuiConstants>

using namespace Qtilities::Plugins::Help::Constants;
using namespace Qtilities::CoreGui::Constants;

Qtilities::Plugins::Help::HelpPluginConfig::HelpPluginConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpPluginConfig)
{
    ui->setupUi(this);
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
    return tr(qti_config_page_HELP);
}

Qtilities::Core::QtilitiesCategory Qtilities::Plugins::Help::HelpPluginConfig::configPageCategory() const {
    if (IConfigPage::configPageCategory().isEmpty())
        return QtilitiesCategory(tr(qti_config_page_DEFAULT_CAT));
    else
        return IConfigPage::configPageCategory();
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

void Qtilities::Plugins::Help::HelpPluginConfig::configPageInitialize() {
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
    foreach (const QString& file, files) {
        if (file.startsWith(":/"))
            read_only_paths << HelpManager::formatFileName(file);
    }
    files_widget.setStringList(files);
    files_widget.setNonRemovableStringList(read_only_paths);
}
