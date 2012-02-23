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

#include "StringListWidget.h"
#include "ui_StringListWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"

#include <QInputDialog>
#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>

using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::StringListWidgetPrivateData {
    StringListWidgetPrivateData() : list_type(StringListWidget::PlainStrings) {}

    QStringListModel                model;
    QAction*                        actionAddString;
    QAction*                        actionRemoveString;
    QString                         string_type;
    StringListWidget::ListType      list_type;
    QStringList                     non_removable_strings;

    QString                         open_dialog_filter;
    QString                         open_dialog_path;

    Qt::ToolBarArea                 toolbar_area;
};

Qtilities::CoreGui::StringListWidget::StringListWidget(const QStringList& string_list, Qt::ToolBarArea toolbar_area, QWidget* parent, Qt::WindowFlags flags) :
        QMainWindow(parent, flags), ui(new Ui::StringListWidget)
{
    ui->setupUi(this);
    d = new StringListWidgetPrivateData;
    d->toolbar_area = toolbar_area;
    d->model.setStringList(string_list);
    ui->listView->setModel(&d->model);

    d->actionAddString = new QAction(QIcon(qti_icon_NEW_16x16),"Add",this);
    d->actionRemoveString = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),"Remove",this);

    QToolBar* toolbar = new QToolBar("List Modification Toolbar");
    addToolBar(d->toolbar_area,toolbar);
    toolbar->setObjectName(tr("List Modification Toolbar"));
    toolbar->addAction(d->actionAddString);
    toolbar->addAction(d->actionRemoveString);
    connect(d->actionAddString,SIGNAL(triggered()),SLOT(handleAddString()));
    connect(d->actionRemoveString,SIGNAL(triggered()),SLOT(handleRemoveString()));

    d->open_dialog_filter = tr("All Files (*.*)");
    d->open_dialog_path = QtilitiesApplication::applicationSessionPath();

    // TODO: Bad way to do it, but workaround for now:
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),SIGNAL(selectionChanged()));
}

Qtilities::CoreGui::StringListWidget::~StringListWidget() {
    delete ui;
    delete d;
}

QStringList Qtilities::CoreGui::StringListWidget::stringList() const {
    return d->model.stringList();
}

void Qtilities::CoreGui::StringListWidget::setStringList(const QStringList& string_list) {
    d->model.setStringList(string_list);

    emit stringListChanged(d->model.stringList());
}

QStringList Qtilities::CoreGui::StringListWidget::nonRemovableStringList() const {
    return d->non_removable_strings;
}

void Qtilities::CoreGui::StringListWidget::setNonRemovableStringList(const QStringList &string_list) {
    d->non_removable_strings = string_list;
}

QString Qtilities::CoreGui::StringListWidget::stringType() const {
    return d->string_type;
}

void Qtilities::CoreGui::StringListWidget::setStringType(const QString& string_type) {
    d->string_type = string_type;
}

Qtilities::CoreGui::StringListWidget::ListType Qtilities::CoreGui::StringListWidget::listType() const {
    return d->list_type;
}

void Qtilities::CoreGui::StringListWidget::setListType(const ListType& list_type) {
    d->list_type = list_type;
}

QString Qtilities::CoreGui::StringListWidget::fileOpenDialogPath() const {
    return d->open_dialog_path;
}

void Qtilities::CoreGui::StringListWidget::setFileOpenDialogPath(const QString& open_dialog_path) {
    d->open_dialog_path = open_dialog_path;
}

QString Qtilities::CoreGui::StringListWidget::fileOpenDialogFilter() const {
    return d->open_dialog_filter;
}

void Qtilities::CoreGui::StringListWidget::setFileOpenDialogFilter(const QString& open_dialog_filter) {
    d->open_dialog_filter = open_dialog_filter;
}

void Qtilities::CoreGui::StringListWidget::handleAddString() {
    bool ok;

    QString string_type;
    if (d->string_type.isEmpty())
        string_type = "Add New Item";
    else
        string_type = QString(tr("Add New %1")).arg(d->string_type);

    if (d->list_type == PlainStrings) {
        QString text = QInputDialog::getText(this, string_type,tr("New Item:"), QLineEdit::Normal,"", &ok);
        if (ok && !text.isEmpty()) {
            QStringList list = d->model.stringList();
            list << text;
            list.removeDuplicates();
            d->model.setStringList(list);
            emit stringListChanged(list);
        }
    } else if (d->list_type == FilePaths) {            
        QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),QDir::cleanPath(d->open_dialog_path),d->open_dialog_filter);
        foreach (QString file_name, fileNames) {
            QStringList list = d->model.stringList();
            list << QDir::toNativeSeparators(file_name);
            list.removeDuplicates();
            d->model.setStringList(list);
            emit stringListChanged(list);
        }
    } else if (d->list_type == Directories) {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),d->open_dialog_path,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) {
            QStringList list = d->model.stringList();
            list << QDir::toNativeSeparators(dir);
            list.removeDuplicates();
            d->model.setStringList(list);
            emit stringListChanged(list);
        }
    }
}

void Qtilities::CoreGui::StringListWidget::handleRemoveString() {
    if (ui->listView->selectionModel()->selectedIndexes().count() > 0) {
        QStringList selected_list;
        for (int i = 0; i < ui->listView->selectionModel()->selectedIndexes().count(); i++)
            selected_list << d->model.data(ui->listView->selectionModel()->selectedIndexes().at(i),Qt::DisplayRole).toString();

        QStringList new_list = d->model.stringList();
        foreach (QString string, selected_list) {
            if (!d->non_removable_strings.contains(string)) {
                new_list.removeOne(string);
            } else {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText("The following item cannot be removed:<br><br>" + string + "<br><br>It was marked as non-removable.");
                msgBox.exec();
            }
        }
        d->model.setStringList(new_list);
    }
}

QListView* Qtilities::CoreGui::StringListWidget::listView() {
    return ui->listView;
}

QStringListModel* Qtilities::CoreGui::StringListWidget::stringListModel() {
    return &d->model;
}
