/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#include "SideWidgetFileSystem.h"
#include "ui_SideWidgetFileSystem.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <QFileDialog>
#include <QDir>
#include <QSettings>

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QWidget, SideWidgetFileSystem> SideWidgetFileSystem::factory;
    }
}

struct Qtilities::CoreGui::SideWidgetFileSystemData {
    SideWidgetFileSystemData(): model(0) {}

    QFileSystemModel *model;
};

Qtilities::CoreGui::SideWidgetFileSystem::SideWidgetFileSystem(const QString& start_path, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SideWidgetFileSystem)
{
    m_ui->setupUi(this);
    d = new SideWidgetFileSystemData;

    d->model = new QFileSystemModel;

    // Set up drag ability:
    d->model->setSupportedDragActions(Qt::CopyAction);
    m_ui->treeView->setDragEnabled(true);

    // Set up model etc.:
    QDir dir(start_path);
    if (start_path.isEmpty() || !dir.exists(start_path))
        d->model->setRootPath(QDir::currentPath());
    else
        d->model->setRootPath(start_path);
    m_ui->treeView->setModel(d->model);
    m_ui->treeView->hideColumn(1);
    m_ui->treeView->hideColumn(2);
    m_ui->treeView->hideColumn(3);
    m_ui->txtCurrentPath->setText(QDir::currentPath());
    m_ui->treeView->setRootIndex(d->model->index(QDir::currentPath()));

    // Make neccesarry connections:
    connect(d->model,SIGNAL(rootPathChanged(QString)),SLOT(handleRootPathChanged(QString)));
    connect(m_ui->btnBrowse,SIGNAL(clicked()),SLOT(handleBtnBrowse()));
    connect(m_ui->treeView,SIGNAL(doubleClicked(QModelIndex)),SLOT(handleDoubleClicked(QModelIndex)));
}

void Qtilities::CoreGui::SideWidgetFileSystem::handleRootPathChanged(const QString& newPath) {
    QDir::setCurrent(newPath);
}

void Qtilities::CoreGui::SideWidgetFileSystem::handleBtnBrowse() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Path"),m_ui->txtCurrentPath->text(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        QDir dir(path);
        if (dir.exists()) {
            m_ui->treeView->setRootIndex(d->model->index(dir.path()));
            m_ui->txtCurrentPath->setText(dir.path());

            if (dir.isRoot())
                m_ui->btnCdUp->setEnabled(false);
            else
                m_ui->btnCdUp->setEnabled(true);
        }
    }
}

void Qtilities::CoreGui::SideWidgetFileSystem::handleDoubleClicked(const QModelIndex & index) {
    if (!index.isValid())
        return;

    if (!d->model)
        return;

    if (d->model->isDir(index))
        return;

    QString file_path = d->model->filePath(index);
    emit requestEditor(file_path);
}

void Qtilities::CoreGui::SideWidgetFileSystem::setPath(const QString& path) {
    m_ui->treeView->setRootIndex(d->model->index(path));
    m_ui->txtCurrentPath->setText(path);
}

QString Qtilities::CoreGui::SideWidgetFileSystem::path() const {
    return m_ui->txtCurrentPath->text();
}

void Qtilities::CoreGui::SideWidgetFileSystem::on_btnCdUp_clicked()
{
    QDir dir(m_ui->txtCurrentPath->text());
    dir.cdUp();
    if (dir.exists()) {
        m_ui->treeView->setRootIndex(d->model->index(dir.path()));
        m_ui->txtCurrentPath->setText(dir.path());

        if (dir.isRoot())
            m_ui->btnCdUp->setEnabled(false);
    }
}
