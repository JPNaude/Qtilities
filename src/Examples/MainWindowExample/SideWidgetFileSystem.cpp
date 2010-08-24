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

#include "SideWidgetFileSystem.h"
#include "ui_SideWidgetFileSystem.h"
#include "ExampleMode.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <QFileDialog>
#include <QDir>
#include <QSettings>

namespace Qtilities {
    namespace Examples {
        namespace MainWindow {
            FactoryItem<QWidget, SideWidgetFileSystem> SideWidgetFileSystem::factory;
        }
    }
}

struct Qtilities::Examples::MainWindow::SideWidgetFileSystemData {
    SideWidgetFileSystemData(): model(0) {}

    QFileSystemModel *model;
};

Qtilities::Examples::MainWindow::SideWidgetFileSystem::SideWidgetFileSystem(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SideWidgetFileSystem)
{
    m_ui->setupUi(this);
    d = new SideWidgetFileSystemData;

    d->model = new QFileSystemModel;
    d->model->setRootPath(QDir::currentPath());
    m_ui->treeView->setModel(d->model);
    m_ui->treeView->hideColumn(1);
    m_ui->treeView->hideColumn(2);
    m_ui->treeView->hideColumn(3);
    m_ui->txtCurrentPath->setText(QDir::currentPath());
    m_ui->treeView->setRootIndex(d->model->index(QDir::currentPath()));
    connect(d->model,SIGNAL(rootPathChanged(QString)),SLOT(handleRootPathChanged(QString)));
    connect(m_ui->btnBrowse,SIGNAL(clicked()),SLOT(handleBtnBrowse()));
    connect(m_ui->treeView,SIGNAL(doubleClicked(QModelIndex)),SLOT(handleDoubleClicked(QModelIndex)));
}

void Qtilities::Examples::MainWindow::SideWidgetFileSystem::handleRootPathChanged(const QString& newPath) {
    QDir::setCurrent(newPath);
}

void Qtilities::Examples::MainWindow::SideWidgetFileSystem::handleBtnBrowse() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Path"),QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_ui->treeView->setRootIndex(d->model->index(dir));
    m_ui->txtCurrentPath->setText(dir);
}

void Qtilities::Examples::MainWindow::SideWidgetFileSystem::handleDoubleClicked(const QModelIndex & index) {
    if (!index.isValid())
        return;

    if (!d->model)
        return;

    if (d->model->isDir(index))
        return;

    QString file_path = d->model->filePath(index);
    emit requestEditor(file_path);
}
