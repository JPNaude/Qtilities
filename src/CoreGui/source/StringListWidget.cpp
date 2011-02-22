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

#include "StringListWidget.h"
#include "ui_StringListWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QInputDialog>
#include <QStringListModel>


using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::StringListWidgetPrivateData {
    StringListWidgetPrivateData()  {}

    QStringListModel model;
    QAction* actionAddString;
    QAction* actionRemoveString;
    QString string_type;

};

Qtilities::CoreGui::StringListWidget::StringListWidget(const QStringList& string_list, const QString& string_type, QWidget* parent, Qt::WindowFlags flags) :
        QMainWindow(parent, flags), ui(new Ui::StringListWidget)
{
    ui->setupUi(this);
    d = new StringListWidgetPrivateData;
    d->model.setStringList(string_list);
    ui->listView->setModel(&d->model);

    if (string_type.isEmpty()) {
        d->actionAddString = new QAction(QIcon(qti_icon_NEW_16x16),"Add",this);
        d->actionRemoveString = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),"Remove",this);
    } else {
        d->actionAddString = new QAction(QIcon(qti_icon_NEW_16x16),"Add " + string_type,this);
        d->actionRemoveString = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),"Remove" + string_type,this);
    }

    ui->toolBar->setObjectName(tr("List Modification Toolbar"));
    ui->toolBar->addAction(d->actionAddString);
    ui->toolBar->addAction(d->actionRemoveString);
    connect(d->actionAddString,SIGNAL(triggered()),SLOT(handleAddString()));
    connect(d->actionRemoveString,SIGNAL(triggered()),SLOT(handleRemoveString()));
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

QString Qtilities::CoreGui::StringListWidget::stringType() const {
    return d->string_type;
}

void Qtilities::CoreGui::StringListWidget::setStringType(const QString& string_type) {
    d->string_type = string_type;
}

void Qtilities::CoreGui::StringListWidget::handleAddString() {
    bool ok;

    if (d->string_type.isEmpty()) {
        QString text = QInputDialog::getText(this, tr("Add New"),tr("New Item:"), QLineEdit::Normal,"", &ok);
        if (ok && !text.isEmpty()) {
            QStringList list = d->model.stringList();
            list << text;
            list.removeDuplicates();
            d->model.setStringList(list);
        }
    } else {
        QString text = QInputDialog::getText(this, QString(tr("Add New %1")).arg(d->string_type),tr("New Item"), QLineEdit::Normal,"", &ok);
        if (ok && !text.isEmpty()) {
            QStringList list = d->model.stringList();
            list << text;
            list.removeDuplicates();
            d->model.setStringList(list);
        }
    }
}

void Qtilities::CoreGui::StringListWidget::handleRemoveString() {
    if (ui->listView->selectionModel()->selectedIndexes().count() > 0) {
        QStringList selected_list;
        for (int i = 0; i < ui->listView->selectionModel()->selectedIndexes().count(); i++)
            selected_list << d->model.data(ui->listView->selectionModel()->selectedIndexes().at(i),Qt::DisplayRole).toString();

        QStringList new_list = d->model.stringList();
        foreach (QString string, selected_list)
            new_list.removeOne(string);
        d->model.setStringList(new_list);
    }
}
