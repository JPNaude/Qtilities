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

#include "ProjectManagementConfig.h"
#include "ui_ProjectManagementConfig.h"
#include "ProjectManager.h"

#include <QFileDialog>

Qtilities::ProjectManagement::ProjectManagementConfig::ProjectManagementConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectManagementConfig)
{
    ui->setupUi(this);
    ui->chkOpenLastProject->setChecked(PROJECT_MANAGER->openLastProjectOnStartup());
    ui->chkCreateNewOnStartup->setChecked(PROJECT_MANAGER->createNewProjectOnStartup());
    ui->chkSaveModifiedProjects->setChecked(PROJECT_MANAGER->checkModifiedOpenProjects());
    ui->chkUseCustomProjectsPath->setChecked(PROJECT_MANAGER->useCustomProjectsPath());
    ui->checkVerboseLogging->setChecked(PROJECT_MANAGER->verboseLogging());
    ui->txtCustomProjectsPath->setText(PROJECT_MANAGER->customProjectsPath());
    if (ui->chkOpenLastProject->isChecked()) {
        ui->chkCreateNewOnStartup->setEnabled(true);
    } else {
        ui->chkCreateNewOnStartup->setEnabled(false);
    }
    if (ui->chkSaveModifiedProjects->isChecked()) {
        ui->radioPromptUserToSave->setEnabled(true);
        ui->radioSaveAutomatically->setEnabled(true);
    } else {
        ui->radioPromptUserToSave->setEnabled(false);
        ui->radioSaveAutomatically->setEnabled(false);
    }
    if (PROJECT_MANAGER->modifiedProjectsHandlingPolicy() == ProjectManager::AutoSave) {
        ui->radioSaveAutomatically->setChecked(true);
    } else if (PROJECT_MANAGER->modifiedProjectsHandlingPolicy() == ProjectManager::PromptUser){
        ui->radioPromptUserToSave->setChecked(true);
    }
    if (ui->chkUseCustomProjectsPath->isChecked()) {
        ui->labelCustomProjectsPath->setEnabled(true);
        ui->txtCustomProjectsPath->setEnabled(true);
        ui->btnOpenProjectsPath->setEnabled(true);
    } else {
        ui->labelCustomProjectsPath->setEnabled(false);
        ui->txtCustomProjectsPath->setEnabled(false);
        ui->btnOpenProjectsPath->setEnabled(false);
    }

    connect(ui->checkVerboseLogging,SIGNAL(toggled(bool)),SLOT(handle_checkVerboseLogging(bool)));
    connect(ui->btnClearRecentProjectList,SIGNAL(clicked()),SLOT(handle_btnClearRecentProjectList()));
    connect(ui->chkCreateNewOnStartup,SIGNAL(toggled(bool)),SLOT(handle_chkCreateNewOnStartup(bool)));
    connect(ui->chkOpenLastProject,SIGNAL(toggled(bool)),SLOT(handle_chkOpenLastProject(bool)));
    connect(ui->chkSaveModifiedProjects,SIGNAL(toggled(bool)),SLOT(handle_chkSaveModifiedProjects(bool)));
    connect(ui->chkUseCustomProjectsPath,SIGNAL(toggled(bool)),SLOT(handle_chkUseCustomProjectsPath(bool)));
    connect(ui->radioPromptUserToSave,SIGNAL(toggled(bool)),SLOT(handle_radioPromptUserToSave(bool)));
    connect(ui->radioSaveAutomatically,SIGNAL(toggled(bool)),SLOT(handle_radioSaveAutomatically(bool)));
    connect(ui->txtCustomProjectsPath,SIGNAL(textChanged(QString)),SLOT(handle_txtCustomProjectsPathTextChanged(QString)));
    connect(ui->btnOpenProjectsPath,SIGNAL(clicked()),SLOT(handle_btnOpenProjectsPath()));
}

Qtilities::ProjectManagement::ProjectManagementConfig::~ProjectManagementConfig()
{
    delete ui;
}

QIcon Qtilities::ProjectManagement::ProjectManagementConfig::configPageIcon() const {
    return QIcon();
}

QWidget* Qtilities::ProjectManagement::ProjectManagementConfig::configPageWidget() {
    return this;
}

QString Qtilities::ProjectManagement::ProjectManagementConfig::configPageTitle() const {
    return tr("Projects");
}

Qtilities::Core::QtilitiesCategory Qtilities::ProjectManagement::ProjectManagementConfig::configPageCategory() const {
    return QtilitiesCategory("General");
}

void Qtilities::ProjectManagement::ProjectManagementConfig::configPageApply() {
    PROJECT_MANAGER->writeSettings();
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_btnClearRecentProjectList() {
    PROJECT_MANAGER->clearRecentProjects();
}

void Qtilities::ProjectManagement::ProjectManagementConfig::changeEvent(QEvent *e)
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

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_chkOpenLastProject(bool toggle) {
    PROJECT_MANAGER->setOpenLastProjectOnStartup(toggle);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_chkCreateNewOnStartup(bool toggle) {
    PROJECT_MANAGER->setCreateNewProjectOnStartup(toggle);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_chkSaveModifiedProjects(bool toggle) {
    PROJECT_MANAGER->setCheckModifiedOpenProjects(toggle);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_radioPromptUserToSave(bool toggle) {
    if (toggle)
        PROJECT_MANAGER->setModifiedProjectsHandlingPolicy(ProjectManager::PromptUser);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_radioSaveAutomatically(bool toggle) {
    if (toggle)
        PROJECT_MANAGER->setModifiedProjectsHandlingPolicy(ProjectManager::AutoSave);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_chkUseCustomProjectsPath(bool toggle) {
    PROJECT_MANAGER->setUseCustomProjectsPath(toggle);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_checkVerboseLogging(bool toggle) {
    PROJECT_MANAGER->setVerboseLogging(toggle);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_btnOpenProjectsPath() {
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(0,
                                tr("Select Custom Projects Path"),
                                ui->txtCustomProjectsPath->text(),
                                options);
    ui->txtCustomProjectsPath->setText(directory);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_txtCustomProjectsPathTextChanged(QString new_path) {
    QDir new_dir(new_path);
    if (new_dir.exists()) {
        ui->txtCustomProjectsPath->setStyleSheet("color: black");
        PROJECT_MANAGER->setCustomProjectsPath(new_path);
    } else {
        ui->txtCustomProjectsPath->setStyleSheet("color: red");
    }
}
