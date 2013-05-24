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

#include "ProjectManagementConfig.h"
#include "ui_ProjectManagementConfig.h"
#include "ProjectManager.h"

#include <QtilitiesFileInfo>
#include <FileUtils.h>

#include <QtilitiesApplication>
#include <QtilitiesCoreGuiConstants>

#include <QFileDialog>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;

Qtilities::ProjectManagement::ProjectManagementConfig::ProjectManagementConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectManagementConfig)
{
    ui->setupUi(this);
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
    return tr(qti_config_page_PROJECTS);
}

Qtilities::Core::QtilitiesCategory Qtilities::ProjectManagement::ProjectManagementConfig::configPageCategory() const {
    if (IConfigPage::configPageCategory().isEmpty())
        return QtilitiesCategory(tr(qti_config_page_DEFAULT_CAT));
    else
        return IConfigPage::configPageCategory();
}

void Qtilities::ProjectManagement::ProjectManagementConfig::configPageApply() {
    saveCustomProjectsPaths();
    PROJECT_MANAGER->writeSettings();
}

void Qtilities::ProjectManagement::ProjectManagementConfig::configPageInitialize() {
    ui->chkOpenLastProject->setChecked(PROJECT_MANAGER->openLastProjectOnStartup());
    ui->chkCreateNewOnStartup->setChecked(PROJECT_MANAGER->createNewProjectOnStartup());
    ui->chkSaveModifiedProjects->setChecked(PROJECT_MANAGER->checkModifiedOpenProjects());
    ui->chkUseCustomProjectsPath->setChecked(PROJECT_MANAGER->useCustomProjectsPath());

    default_category_item = 0;
    active_category_item = 0;

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

    connect(ui->btnClearRecentProjectList,SIGNAL(clicked()),SLOT(handle_btnClearRecentProjectList()));
    connect(ui->chkCreateNewOnStartup,SIGNAL(toggled(bool)),SLOT(handle_chkCreateNewOnStartup(bool)));
    connect(ui->chkOpenLastProject,SIGNAL(toggled(bool)),SLOT(handle_chkOpenLastProject(bool)));
    connect(ui->chkSaveModifiedProjects,SIGNAL(toggled(bool)),SLOT(handle_chkSaveModifiedProjects(bool)));
    connect(ui->chkUseCustomProjectsPath,SIGNAL(toggled(bool)),SLOT(handle_chkUseCustomProjectsPath(bool)));
    connect(ui->radioPromptUserToSave,SIGNAL(toggled(bool)),SLOT(handle_radioPromptUserToSave(bool)));
    connect(ui->radioSaveAutomatically,SIGNAL(toggled(bool)),SLOT(handle_radioSaveAutomatically(bool)));

    connect(ui->tableCustomPaths,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(handleActiveCustomProjectPathChanged(QTableWidgetItem*)));
    connect(ui->tableCustomPaths,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),SLOT(handleCurrentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));

    connect(PROJECT_MANAGER,SIGNAL(customProjectPathsChanged()),SLOT(refreshCustomProjectPaths()));
    refreshCustomProjectPaths();
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

void Qtilities::ProjectManagement::ProjectManagementConfig::on_btnRemove_clicked() {
    // Find the name of the category at currentRow():
    QTableWidgetItem* current_category_name = ui->tableCustomPaths->item(ui->tableCustomPaths->currentRow(),0);
    if (current_category_name) {
        if (current_category_name->text() == active_category) {
            active_category = "Default";
            active_category_item = default_category_item;
            active_category_item->setCheckState(Qt::Checked);
        }
        custom_paths.remove(current_category_name->text());
        ui->tableCustomPaths->removeRow(ui->tableCustomPaths->currentRow());
        saveCustomProjectsPaths();
    }
}

void Qtilities::ProjectManagement::ProjectManagementConfig::on_btnAdd_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Specify Custom Path"),QtilitiesApplication::applicationSessionPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        QString category;
        if (custom_paths.isEmpty())
            category = "Default";
        else
            category = "New Category";
        int count = 1;
        while (PROJECT_MANAGER->customProjectCategories().contains(category)) {
            QString count_string = QString::number(count);
            if (category.endsWith(count_string))
                category.chop(count_string.length());
            ++count;
            category.append(count_string);
        }



        custom_paths[category] = FileUtils::toNativeSeparators(QDir::cleanPath(dir));

        saveCustomProjectsPaths();
        refreshCustomProjectPaths();
    }
}

void Qtilities::ProjectManagement::ProjectManagementConfig::refreshCustomProjectPaths() {
    if (custom_paths.keys() != PROJECT_MANAGER->customProjectCategories()) {
        custom_paths.clear();
        foreach (const QString& cat, PROJECT_MANAGER->customProjectCategories())
            custom_paths[cat] = PROJECT_MANAGER->customProjectsPath(cat);
    }

    QStringList categories = custom_paths.keys();

    ui->tableCustomPaths->clear();
    ui->tableCustomPaths->setRowCount(categories.count());
    ui->tableCustomPaths->setColumnCount(2);

    QStringList headers;
    headers << tr("Category") << tr("Path");
    ui->tableCustomPaths->setHorizontalHeaderLabels(headers);
    ui->tableCustomPaths->verticalHeader()->hide();

    ui->tableCustomPaths->blockSignals(true);

    for (int i = 0; i < categories.count(); ++i) {
        bool is_editable = (categories.at(i) != QString("Default"));

        // Category column:
        QTableWidgetItem* newItem = new QTableWidgetItem(categories.at(i));
        if (categories.at(i) == PROJECT_MANAGER->defaultCustomProjectsCategory()) {
            newItem->setCheckState(Qt::Checked);
            active_category_item = newItem;
        } else
            newItem->setCheckState(Qt::Unchecked);

        Qt::ItemFlags item_flags = newItem->flags();
        if (!is_editable) {
            item_flags &= ~Qt::ItemIsEditable;
            newItem->setFlags(item_flags);
            newItem->setToolTip("Default custom path category.");
            default_category_item = newItem;
        }
        ui->tableCustomPaths->setItem(i, 0, newItem);

        // Path column:
        QString path = PROJECT_MANAGER->customProjectsPath(categories.at(i));
        if (!path.isEmpty()) {
            QDir dir(path);
            if (dir.exists()) {
                newItem = new QTableWidgetItem(path);
                newItem->setForeground(QBrush(QColor("#00aa00")));
            } else {
                newItem = new QTableWidgetItem(path);
                newItem->setForeground(QBrush(Qt::red));
            }
        } else {
            path = "None specified";
            newItem = new QTableWidgetItem(path);
            newItem->setForeground(QBrush(Qt::red));
        }
        ui->tableCustomPaths->setItem(i, 1, newItem);
        ui->tableCustomPaths->setRowHeight(i,17);

        if (i == categories.count()-1)
            ui->tableCustomPaths->blockSignals(false);
    }

    ui->tableCustomPaths->resizeColumnsToContents();
    ui->tableCustomPaths->horizontalHeader()->setStretchLastSection(true);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::saveCustomProjectsPaths() {
    disconnect(PROJECT_MANAGER,SIGNAL(customProjectPathsChanged()),this,SLOT(refreshCustomProjectPaths()));
    PROJECT_MANAGER->setDefaultCustomProjectsCategory(active_category);
    PROJECT_MANAGER->clearCustomProjectsPaths();
    foreach (const QString& key, custom_paths.keys()) {
        PROJECT_MANAGER->setCustomProjectsPath(custom_paths[key],key);
    }
    connect(PROJECT_MANAGER,SIGNAL(customProjectPathsChanged()),this,SLOT(refreshCustomProjectPaths()));
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handleCurrentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous) {
    Q_UNUSED(previous)

    if (current) {
        QTableWidgetItem* current_category = ui->tableCustomPaths->item(current->row(),0);
        if (current_category->text() != "Default") {
            ui->btnRemove->setEnabled(true);
            return;
        }
    }
    ui->btnRemove->setEnabled(false);
}

void Qtilities::ProjectManagement::ProjectManagementConfig::handleActiveCustomProjectPathChanged(QTableWidgetItem * item) {
    QString item_version = item->text();

    // Update The Map:
    if (item->column() == 0) {
        // Find the path to look it up in the map:
        QTableWidgetItem* lookup_item = ui->tableCustomPaths->item(item->row(),1);
        if (lookup_item) {
            QString old_name = custom_paths.key(lookup_item->text());
            custom_paths.remove(old_name);
            custom_paths[item->text()] = lookup_item->text();
            if (active_category == old_name)
                active_category = item->text();
        }
    } else if (item->column() == 1) {
        // Find the name to look it up in the map:
        QTableWidgetItem* lookup_item = ui->tableCustomPaths->item(item->row(),0);
        if (lookup_item) {
            if (!item->text().isEmpty()) {
                QDir dir(item->text());
                if (dir.exists())
                    item->setForeground(QBrush(QColor("#00aa00")));
                else
                    item->setForeground(QBrush(Qt::red));
            } else {
                item->setText("None specified");
                item->setForeground(QBrush(Qt::red));
            }

            custom_paths[lookup_item->text()] = item->text();
        }
    }

    // Update Activity:
    if (item->checkState() == Qt::Checked) {
        if (active_category != item_version) {
            if (active_category_item)
                active_category_item->setCheckState(Qt::Unchecked);
            active_category_item = item;
            active_category = item_version;
            saveCustomProjectsPaths();
        }
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

