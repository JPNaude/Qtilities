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

#include "ProjectsBrowser.h"
#include "ui_ProjectsBrowser.h"
#include "ProjectManager.h"

#include <QtilitiesApplication>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;

Qtilities::ProjectManagement::ProjectsBrowser::ProjectsBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectsBrowser)
{
    ui->setupUi(this);

    connect(PROJECT_MANAGER,SIGNAL(recentProjectsChanged(QStringList,QStringList)),SLOT(refreshContents()));
    connect(PROJECT_MANAGER,SIGNAL(customProjectPathsChanged()),SLOT(refreshContents()));

    file_system_browser = new SideWidgetFileSystem(QtilitiesApplication::applicationSessionPath());
    connect(file_system_browser,SIGNAL(requestEditor(QString)),SLOT(openProjectAtPath(QString)));
    ui->widgetCustomBrowser->setEnabled(false);

    if (ui->widgetCustomBrowser->layout())
        delete ui->widgetCustomBrowser->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->widgetCustomBrowser);
    layout->setMargin(0);
    layout->addWidget(file_system_browser);
    file_system_browser->show();
    file_system_browser->toggleDoubleClickFileOpen(false);

    // Put the widget in the center of the screen:
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    setAttribute(Qt::WA_QuitOnClose,false);
    refreshContents();
}

Qtilities::ProjectManagement::ProjectsBrowser::~ProjectsBrowser() {
    delete ui;
}

void Qtilities::ProjectManagement::ProjectsBrowser::refreshContents() {
    // Add list of recent files:
    ui->listWidgetRecent->clear();
    for (int i = 0; i < PROJECT_MANAGER->recentProjectNames().count(); ++i) {
        QString name = PROJECT_MANAGER->recentProjectNames().at(i);
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(name);
        newItem->setToolTip(PROJECT_MANAGER->recentProjectPath(name));

        if (name == PROJECT_MANAGER->defaultCustomProjectsCategory()) {
            newItem->setForeground(Qt::red);
        }

        ui->listWidgetRecent->insertItem(i, newItem);
    }

    // Refresh project categories list:
    ui->listCustomCategories->clear();
    QListWidgetItem* current_item = 0;
    for (int i = 0; i < PROJECT_MANAGER->customProjectCategories().count(); ++i) {
        QString category = PROJECT_MANAGER->customProjectCategories().at(i);
        QListWidgetItem *newItem = new QListWidgetItem;
        QString current_path = PROJECT_MANAGER->customProjectsPath(category);
        if (category == PROJECT_MANAGER->defaultCustomProjectsCategory()) {
            current_item = newItem;
            file_system_browser->setPath(current_path);
            ui->widgetCustomBrowser->setEnabled(true);
        }
        newItem->setText(category);
        newItem->setToolTip(current_path);
        ui->listCustomCategories->insertItem(i, newItem);
    }
    ui->listCustomCategories->setCurrentItem(current_item);
}

void Qtilities::ProjectManagement::ProjectsBrowser::on_btnClearRecent_clicked() {
    PROJECT_MANAGER->clearRecentProjects();
}

void Qtilities::ProjectManagement::ProjectsBrowser::on_buttonBox_accepted() {
    QString project_path;
    if (ui->tabWidget->currentIndex() == 0) {
        QListWidgetItem* item = ui->listWidgetRecent->currentItem();
        if (item)
            project_path = item->toolTip();
    } else if (ui->tabWidget->currentIndex() == 1) {
        project_path = file_system_browser->filePath();
    }
    openProjectAtPath(project_path);
}

void Qtilities::ProjectManagement::ProjectsBrowser::on_listCustomCategories_itemClicked(QListWidgetItem *item) {
    if (item) {
        ui->widgetCustomBrowser->setEnabled(true);
        file_system_browser->setPath(item->toolTip());
    } else {
        ui->widgetCustomBrowser->setEnabled(false);
    }
}

void Qtilities::ProjectManagement::ProjectsBrowser::on_listWidgetRecent_itemDoubleClicked(QListWidgetItem *item) {
    QString project_path;
    if (item)
        project_path = item->toolTip();
    openProjectAtPath(project_path);
}

void Qtilities::ProjectManagement::ProjectsBrowser::openProjectAtPath(const QString &project_path) {
    if (project_path.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(tr("You need to make a selection before attempting to open a project."));
        msgBox.exec();
    } else {
        setEnabled(false);
        if (PROJECT_MANAGER->openProject(project_path)) {
            close();
        }
        setEnabled(true);
    }
}

void Qtilities::ProjectManagement::ProjectsBrowser::on_buttonBox_rejected() {
    close();
}

void Qtilities::ProjectManagement::ProjectsBrowser::on_btnRemoveNonExisting_clicked() {
    PROJECT_MANAGER->removeNonExistingRecentProjects();
}
