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

#include "ProjectManagementConfig.h"
#include "ui_ProjectManagementConfig.h"
#include "ProjectManager.h"

#include <QtilitiesFileInfo>

#include <QtilitiesApplication>
#include <QtilitiesCoreGuiConstants>

#include <QFileDialog>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Icons;

Qtilities::ProjectManagement::ProjectManagementConfig::ProjectManagementConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectManagementConfig)
{
    ui->setupUi(this);
    ui->btnOpenProjectsPath->setIcon(QIcon(qti_icon_FILE_OPEN_16x16));
    ui->btnRestoreDefaultPath->setIcon(QIcon(qti_icon_EDIT_UNDO_16x16));
    ui->chkOpenLastProject->setChecked(PROJECT_MANAGER->openLastProjectOnStartup());
    ui->chkCreateNewOnStartup->setChecked(PROJECT_MANAGER->createNewProjectOnStartup());
    ui->chkSaveModifiedProjects->setChecked(PROJECT_MANAGER->checkModifiedOpenProjects());
    ui->chkUseCustomProjectsPath->setChecked(PROJECT_MANAGER->useCustomProjectsPath());

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

    connect(ui->btnClearRecentProjectList,SIGNAL(clicked()),SLOT(handle_btnClearRecentProjectList()));
    connect(ui->chkCreateNewOnStartup,SIGNAL(toggled(bool)),SLOT(handle_chkCreateNewOnStartup(bool)));
    connect(ui->chkOpenLastProject,SIGNAL(toggled(bool)),SLOT(handle_chkOpenLastProject(bool)));
    connect(ui->chkSaveModifiedProjects,SIGNAL(toggled(bool)),SLOT(handle_chkSaveModifiedProjects(bool)));
    connect(ui->chkUseCustomProjectsPath,SIGNAL(toggled(bool)),SLOT(handle_chkUseCustomProjectsPath(bool)));
    connect(ui->radioPromptUserToSave,SIGNAL(toggled(bool)),SLOT(handle_radioPromptUserToSave(bool)));
    connect(ui->radioSaveAutomatically,SIGNAL(toggled(bool)),SLOT(handle_radioSaveAutomatically(bool)));
    connect(ui->txtCustomProjectsPath,SIGNAL(textChanged(QString)),SLOT(handle_txtCustomProjectsPathTextChanged(QString)));
    connect(ui->btnOpenProjectsPath,SIGNAL(clicked()),SLOT(handle_btnOpenProjectsPath()));

    ui->txtCustomProjectsPath->setText(PROJECT_MANAGER->customProjectsPath());
}

Qtilities::ProjectManagement::ProjectManagementConfig::~ProjectManagementConfig()
{
    delete ui;
}

QIcon Qtilities::ProjectManagement::ProjectManagementConfig::configPageIcon() const {
    return QIcon(qti_icon_CONFIG_PROJECTS_48x48);
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
    // Create projects path if it does not exist:
    if (!ui->txtCustomProjectsPath->text().isEmpty()) {

        #ifdef Q_OS_WIN
        if (QtilitiesFileInfo::isValidFilePath(ui->txtCustomProjectsPath->text())) {
        #else
        if (!ui->txtCustomProjectsPath->text().isEmpty()) {
        #endif
            QDir dir(ui->txtCustomProjectsPath->text());
            if (!dir.exists()) {
                if (dir.mkpath(ui->txtCustomProjectsPath->text())) {
                    LOG_INFO_P("Successfully created custom projects path at: " + ui->txtCustomProjectsPath->text());
                }
            }
        }
    }
    handle_txtCustomProjectsPathTextChanged();

    PROJECT_MANAGER->writeSettings();
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_btnClearRecentProjectList() {
    PROJECT_MANAGER->clearRecentProjects();
}

void Qtilities::ProjectManagement::ProjectManagementConfig::changeEvent(QEvent *e) {
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

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_btnOpenProjectsPath() {
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(0,
                                tr("Select Custom Projects Path"),
                                ui->txtCustomProjectsPath->text(),
                                options);
    ui->txtCustomProjectsPath->setText(directory);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handle_txtCustomProjectsPathTextChanged(QString new_path) {
    Q_UNUSED(new_path)

    if (ui->txtCustomProjectsPath->text().isEmpty()) {
        ui->lblPathMessageIcon->setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(16));
        ui->lblPathMessageText->setText("Your custom projects path cannot be empty.");
        ui->txtCustomProjectsPath->setStyleSheet("color: red");
    } else {
        if (QtilitiesFileInfo::isValidFilePath(ui->txtCustomProjectsPath->text())) {
            QDir dir(ui->txtCustomProjectsPath->text());
            if (dir.exists()) {
                ui->lblPathMessageIcon->setPixmap(QIcon(qti_icon_SUCCESS_16x16).pixmap(16));
                ui->lblPathMessageText->setText("Your custom projects path exists and is correct.");
                ui->txtCustomProjectsPath->setStyleSheet("color: black");
            } else {
                ui->lblPathMessageIcon->setPixmap(QIcon(qti_icon_WARNING_16x16).pixmap(16));
                ui->lblPathMessageText->setText("Your custom projects path does not exists. It will be created as soon as you Apply.");
                ui->txtCustomProjectsPath->setStyleSheet("color: black");
            }
        } else {
            ui->lblPathMessageIcon->setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(16));
            ui->lblPathMessageText->setText("Your path contains invalid characters: " + QtilitiesFileInfo::invalidFilePathCharacters());
            ui->txtCustomProjectsPath->setStyleSheet("color: red");
        }
    }

    PROJECT_MANAGER->setCustomProjectsPath(ui->txtCustomProjectsPath->text());
}

void Qtilities::ProjectManagement::ProjectManagementConfig::on_btnRestoreDefaultPath_clicked() {
    ui->txtCustomProjectsPath->setText(QtilitiesApplication::applicationSessionPath() + QDir::separator() + "Projects");
}
