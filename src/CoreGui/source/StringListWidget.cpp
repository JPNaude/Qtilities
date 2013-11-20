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

#include "StringListWidget.h"
#include "ui_StringListWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"
#include "FileUtils.h"

#include <QInputDialog>
#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::StringListWidgetPrivateData {
    StringListWidgetPrivateData() : list_type(StringListWidget::PlainStrings),
        open_on_double_click(false),
        read_only(false) {}

    QStringListModel                model;
    QAction*                        actionAddString;
    QAction*                        actionRemoveString;
    QString                         string_type;
    StringListWidget::ListType      list_type;
    QStringList                     non_removable_strings;

    QString                         open_dialog_filter;
    QString                         open_dialog_path;

    Qt::ToolBarArea                 toolbar_area;
    bool                            open_on_double_click;
    bool                            read_only;
};

Qtilities::CoreGui::StringListWidget::StringListWidget(const QStringList& string_list, Qt::ToolBarArea toolbar_area, QWidget* parent, Qt::WindowFlags flags) :
        QMainWindow(parent, flags), ui(new Ui::StringListWidget)
{
    ui->setupUi(this);
    d = new StringListWidgetPrivateData;
    d->toolbar_area = toolbar_area;
    d->model.setStringList(string_list);
    ui->listView->setModel(&d->model);
    d->model.sort(0);

    d->actionAddString = new QAction(QIcon(qti_icon_NEW_16x16),"Add",this);
    d->actionRemoveString = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),"Remove",this);

    QToolBar* toolbar = new QToolBar("List Modification Toolbar");
    addToolBar(d->toolbar_area,toolbar);
    toolbar->setObjectName("List Modification Toolbar");
    toolbar->addAction(d->actionAddString);
    toolbar->addAction(d->actionRemoveString);
    connect(d->actionAddString,SIGNAL(triggered()),SLOT(handleAddString()));
    connect(d->actionRemoveString,SIGNAL(triggered()),SLOT(handleRemoveString()));

    d->open_dialog_filter = "All Files (*.*)";
    d->open_dialog_path = QtilitiesApplication::applicationSessionPath();

    // TODO: Bad way to do it, but workaround for now:
    connect(ui->listView,SIGNAL(clicked(QModelIndex)),SIGNAL(selectionChanged()));
    connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),SLOT(handleDoubleClick(QModelIndex)));
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
    d->model.sort(0);
}

QStringList Qtilities::CoreGui::StringListWidget::nonRemovableStringList() const {
    return d->non_removable_strings;
}

void Qtilities::CoreGui::StringListWidget::setNonRemovableStringList(const QStringList &string_list) {
    d->non_removable_strings = string_list;
}

void Qtilities::CoreGui::StringListWidget::setReadOnly(bool read_only) {
    if (d->read_only != read_only) {
        d->read_only = read_only;
        d->actionAddString->setEnabled(!d->read_only);
        d->actionRemoveString->setEnabled(!d->read_only);
    }
}

bool Qtilities::CoreGui::StringListWidget::readOnly() const {
    return d->read_only;
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
        string_type = tr("Add New Item");
    else
        string_type = tr("Add New %1").arg(d->string_type);

    if (d->list_type == PlainStrings) {
        QString text = QInputDialog::getText(this, string_type,tr("New Item"), QLineEdit::Normal,"", &ok);
        if (ok && !text.isEmpty()) {
            QStringList list = d->model.stringList();
            list << text;
            list.removeDuplicates();
            d->model.setStringList(list);
            emit stringListChanged(list);
        }
    } else if (d->list_type == FilePaths) {            
        QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),QDir::cleanPath(d->open_dialog_path),d->open_dialog_filter);
        int process_count = 0;
        foreach (const QString& file_name, fileNames) {
            if (process_count == 0) {
                QFileInfo fi(file_name);
                d->open_dialog_path = fi.path();
            }
            ++process_count;

            QStringList list = d->model.stringList();
            list << FileUtils::toNativeSeparators(file_name);
            list.removeDuplicates();
            d->model.setStringList(list);
            emit stringListChanged(list);
        }
    } else if (d->list_type == Directories) {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),d->open_dialog_path,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) {
            QDir new_default_dir(dir);
            new_default_dir.cdUp();
            d->open_dialog_path = new_default_dir.path();

            QStringList list = d->model.stringList();
            list << FileUtils::toNativeSeparators(dir);
            list.removeDuplicates();
            d->model.setStringList(list);
            emit stringListChanged(list);
        }
    }
    d->model.sort(0);
}

void Qtilities::CoreGui::StringListWidget::handleRemoveString() {
    if (ui->listView->selectionModel()->selectedIndexes().count() > 0) {
        QStringList selected_list;
        for (int i = 0; i < ui->listView->selectionModel()->selectedIndexes().count(); ++i)
            selected_list << d->model.data(ui->listView->selectionModel()->selectedIndexes().at(i),Qt::DisplayRole).toString();

        QStringList new_list = d->model.stringList();
        foreach (const QString& string, selected_list) {
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
    d->model.sort(0);
}

void Qtilities::CoreGui::StringListWidget::handleDoubleClick(QModelIndex index) {
    if (d->open_on_double_click) {
        QString current_selection = d->model.data(index,Qt::DisplayRole).toString();
        if (d->list_type == FilePaths) {
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(current_selection)))
                LOG_ERROR(QString("Failed to open file: %1").arg(current_selection));
            else
                LOG_INFO(QString("Successfully opened file: %1").arg(current_selection));
        } else if (d->list_type == Directories) {
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(current_selection)))
                LOG_ERROR(QString("Failed to path at: %1").arg(current_selection));
            else
                LOG_INFO(QString("Successfully opened path at: %1").arg(current_selection));
        }
    }
}

QListView* Qtilities::CoreGui::StringListWidget::listView() {
    return ui->listView;
}

QStringListModel* Qtilities::CoreGui::StringListWidget::stringListModel() {
    return &d->model;
}

void Qtilities::CoreGui::StringListWidget::setOpenOnDoubleClick(bool open_on_double_click) {
    d->open_on_double_click = open_on_double_click;
}

bool Qtilities::CoreGui::StringListWidget::openOnDoubleClick() const {
    return d->open_on_double_click;
}
