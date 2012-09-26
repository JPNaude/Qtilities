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

#include "SideWidgetFileSystem.h"
#include "ui_SideWidgetFileSystem.h"
#include "QtilitiesApplication"

#include <FileUtils>
using namespace Qtilities::Core;

#include <QFileSystemModel>
#include <QTreeView>
#include <QFileDialog>
#include <QDir>

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QWidget, SideWidgetFileSystem> SideWidgetFileSystem::factory;
    }
}

struct Qtilities::CoreGui::SideWidgetFileSystemPrivateData {
    SideWidgetFileSystemPrivateData(): model(0),
        open_file_on_double_click(true) {}

    QFileSystemModel*       model;
    QPoint                  drag_start_position;
    bool                    open_file_on_double_click;
};

Qtilities::CoreGui::SideWidgetFileSystem::SideWidgetFileSystem(const QString& start_path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SideWidgetFileSystem)
{
    ui->setupUi(this);
    d = new SideWidgetFileSystemPrivateData;

    d->model = new QFileSystemModel;

    // Set up drag ability:
    d->model->setSupportedDragActions(Qt::CopyAction);
    ui->treeView->setDragEnabled(true);

    // Set up model etc.:
    QDir dir(start_path);
    if (start_path.isEmpty() || !dir.exists())
        d->model->setRootPath(QtilitiesApplication::applicationSessionPath());
    else
        d->model->setRootPath(start_path);
    ui->treeView->setModel(d->model);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->txtCurrentPath->setText(QtilitiesApplication::applicationSessionPath());
    ui->treeView->setRootIndex(d->model->index(QtilitiesApplication::applicationSessionPath()));

    // Make neccesarry connections:
    connect(d->model,SIGNAL(rootPathChanged(QString)),SLOT(handleRootPathChanged(QString)));
    connect(ui->btnBrowse,SIGNAL(clicked()),SLOT(handleBtnBrowse()));
    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),SLOT(handleDoubleClicked(QModelIndex)));
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),SLOT(handleClicked(QModelIndex)));

    // Handle drag drop events manually:
    ui->treeView->viewport()->installEventFilter(this);
    setAcceptDrops(true);
}

Qtilities::CoreGui::SideWidgetFileSystem::~SideWidgetFileSystem() {
    delete d;
}

void Qtilities::CoreGui::SideWidgetFileSystem::releasePath() {
    ui->treeView->setModel(0);
    delete d->model;

    d->model = new QFileSystemModel;

    // Set up drag ability:
    d->model->setSupportedDragActions(Qt::CopyAction);
    d->model->setRootPath(QtilitiesApplication::applicationSessionPath());
    ui->treeView->setModel(d->model);
    ui->txtCurrentPath->setText(d->model->rootPath());
    ui->treeView->setEnabled(false);
    ui->txtCurrentPath->clear();
    ui->txtCurrentPath->setEnabled(false);
    ui->btnBrowse->setEnabled(false);
    ui->btnCdUp->setEnabled(false);
}

void Qtilities::CoreGui::SideWidgetFileSystem::dragEnterEvent(QDragEnterEvent *event) {
     if (event->mimeData()->hasUrls())
         event->acceptProposedAction();
 }

void Qtilities::CoreGui::SideWidgetFileSystem::dropEvent(QDropEvent *event) {
    if (event->source() == this) {
        event->ignore();
        return;
    }

    // For now we just copy files (not folders) dropped here:
    foreach (QUrl url, event->mimeData()->urls()) {
        // Create the source path:
        QString source_path = url.path();
        if (source_path.startsWith("/"))
            source_path.remove(0,1);

        QFile file(source_path);
        QFileInfo file_info(source_path);
        if (!file_info.isDir()) {
            // Create the destination path:
            QString dest_path = ui->txtCurrentPath->text();
            if (!dest_path.endsWith("/"))
                dest_path.append("/");
            dest_path.append(file_info.fileName());

            if (file.copy(dest_path))
                LOG_INFO_P(tr(QString("Successfully copied file from \"" + source_path + "\" to path \"" + dest_path + "\".").toStdString().data()));
            else
                LOG_ERROR_P(tr(QString("Failed to copy file from \"" + source_path + "\" to path \"" + dest_path + "\".").toStdString().data()));
        }
    }

    event->acceptProposedAction();
 }

void Qtilities::CoreGui::SideWidgetFileSystem::handleRootPathChanged(const QString& newPath) {
    QDir::setCurrent(newPath);
}

void Qtilities::CoreGui::SideWidgetFileSystem::handleBtnBrowse() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Path"),ui->txtCurrentPath->text(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        QDir dir(path);
        if (dir.exists()) {
            ui->treeView->setRootIndex(d->model->index(dir.path()));
            ui->txtCurrentPath->setText(d->model->rootPath());

            if (dir.isRoot())
                ui->btnCdUp->setEnabled(false);
            else
                ui->btnCdUp->setEnabled(true);
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

void Qtilities::CoreGui::SideWidgetFileSystem::handleClicked(const QModelIndex& index) {
    if (!index.isValid())
        return;

    if (!d->model)
        return;

    if (d->model->isDir(index)) {
        QString dir_path = d->model->filePath(index);
        ui->txtCurrentPath->setText(dir_path);
        return;
    }
}

void Qtilities::CoreGui::SideWidgetFileSystem::setPath(const QString& path) {
    ui->treeView->setRootIndex(d->model->index(path));
    ui->txtCurrentPath->setText(FileUtils::toNativeSeparators(path));
    ui->treeView->setEnabled(true);
    ui->txtCurrentPath->setEnabled(true);
    ui->btnBrowse->setEnabled(true);
    ui->btnCdUp->setEnabled(true);
}

QString Qtilities::CoreGui::SideWidgetFileSystem::path() const {
    return ui->txtCurrentPath->text();
}

QString Qtilities::CoreGui::SideWidgetFileSystem::filePath() const {
    QModelIndex index = ui->treeView->currentIndex();

    if (!index.isValid())
        return QString();

    if (!d->model)
        return QString();

    if (d->model->isDir(index))
        return QString();

    return d->model->filePath(index);
}

void Qtilities::CoreGui::SideWidgetFileSystem::toggleDoubleClickFileOpen(bool open_file) {
    d->open_file_on_double_click = open_file;
}

void Qtilities::CoreGui::SideWidgetFileSystem::on_btnCdUp_clicked() {
    QDir dir(ui->txtCurrentPath->text());
    dir.cdUp();
    if (dir.exists()) {
        ui->treeView->setRootIndex(d->model->index(dir.path()));
        ui->txtCurrentPath->setText(dir.path());

        if (dir.isRoot())
            ui->btnCdUp->setEnabled(false);
    }
}

void Qtilities::CoreGui::SideWidgetFileSystem::on_txtCurrentPath_editingFinished() {
    QDir dir(ui->txtCurrentPath->text());
    if (dir.exists()) {
        ui->treeView->setRootIndex(d->model->index(dir.path()));
        ui->txtCurrentPath->setText(dir.path());

        if (dir.isRoot())
            ui->btnCdUp->setEnabled(false);
    }
}

bool Qtilities::CoreGui::SideWidgetFileSystem::eventFilter(QObject *object, QEvent *event) {
    if (object == ui->treeView->viewport()) {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (!(mouseEvent->buttons() & Qt::LeftButton || mouseEvent->buttons() & Qt::RightButton))
                return false;
            if ((mouseEvent->pos() - d->drag_start_position).manhattanLength() < QApplication::startDragDistance())
                return false;

            QDrag *drag = new QDrag(this);

            // Get the urls currently selected:
            QList<QUrl> urls;
            foreach (QModelIndex index, ui->treeView->selectionModel()->selectedIndexes()) {
                if (index.column() == 0)
                    urls << QUrl::fromLocalFile(d->model->filePath(index));
            }

            QMimeData *mimeData = new QMimeData;
            mimeData->setUrls(urls);
            drag->setMimeData(mimeData);

            if (mouseEvent->buttons() == Qt::LeftButton)
               drag->exec(Qt::CopyAction);
            else if (mouseEvent->buttons() == Qt::RightButton)
               drag->exec(Qt::MoveAction);
            return false;
        } else if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if ((mouseEvent->buttons() & Qt::LeftButton || mouseEvent->buttons() & Qt::RightButton))
                d->drag_start_position = mouseEvent->pos();
        }
    }

    return false;
}

void Qtilities::CoreGui::SideWidgetFileSystem::on_treeView_doubleClicked(QModelIndex index) {
    if (!d->open_file_on_double_click)
        return;

    if (!index.isValid())
        return;

    if (!d->model)
        return;

    if (!d->model->isDir(index)) {
        QString file_path = d->model->filePath(index);
        QDesktopServices explorer_service;
        if (!explorer_service.openUrl(QUrl(QUrl::fromLocalFile(file_path)))) {
            LOG_ERROR(QString("Failed to open file: %1").arg(file_path));
        }
        else
            LOG_INFO(QString("Successfully opened file: %1").arg(file_path));
    }
}
