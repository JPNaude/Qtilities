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

#include "GenericPropertyPathEditor.h"
#include "ui_GenericPropertyPathEditor.h"

#ifdef QTILITIES_PROPERTY_BROWSER

#include "GenericPropertyPathEditorListWrapper.h"
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"
using namespace Qtilities::CoreGui::Icons;

#include <QFileDialog>

namespace Qtilities {
namespace CoreGui {

GenericPropertyPathEditor::GenericPropertyPathEditor(GenericProperty::PropertyType property_type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenericPropertyPathEditor)
{
    ui->setupUi(this);
    d_default_open_path = QtilitiesApplication::applicationSessionPath();
    d_property_type = property_type;
    d_list_separator_backend = "|";
    d_editable = true;

    if (d_property_type == GenericProperty::TypeFile || d_property_type == GenericProperty::TypePath)
        ui->btnBrowse->setIcon(QIcon(qti_icon_FILE_OPEN_16x16));
    else if (d_property_type == GenericProperty::TypeFileList || d_property_type == GenericProperty::TypePathList)
        ui->btnBrowse->setIcon(QIcon(qti_icon_PUSH_UP_CURRENT_16x16));

    connect(ui->txtEditor,SIGNAL(textChanged(QString)),SLOT(handleValueChanged(QString)));
}

GenericPropertyPathEditor::~GenericPropertyPathEditor() {
    delete ui;
}

void GenericPropertyPathEditor::setText(const QString &text) {
    ui->txtEditor->setText(text);
}

void GenericPropertyPathEditor::setItemFilter(const QString &file_filter) {
    d_file_filter = file_filter;
}

void GenericPropertyPathEditor::setEditable(bool editable) {
    d_editable = editable;
    ui->txtEditor->setReadOnly(!editable);
}

void GenericPropertyPathEditor::setPropertyName(const QString &name) {
    d_property_name = name;
}

void GenericPropertyPathEditor::setListSeperatorBackend(const QString &list_seperator) {
    setListSeparatorBackend(list_seperator);
}

void GenericPropertyPathEditor::setListSeparatorBackend(const QString &list_separator) {
    d_list_separator_backend = list_separator;
}

void GenericPropertyPathEditor::setDefaultOpenPath(const QString &default_open_path) {
    d_default_open_path = default_open_path;
}

void GenericPropertyPathEditor::setCurrentValues(const QStringList &current_values) {
    d_current_values = current_values;
    setText(d_current_values.join(d_list_separator_backend));
}

void GenericPropertyPathEditor::on_btnBrowse_clicked() {
    if (d_property_type == GenericProperty::TypeFile) {
        // Check fi the default path exists, if it does not we create it:
        if (!d_default_open_path.isEmpty()) {
            QDir dir(d_default_open_path);
            if (!dir.exists())
                dir.mkpath(d_default_open_path);
        }

        QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"),d_default_open_path,d_file_filter);
        if (!fileName.isEmpty())
            ui->txtEditor->setText(fileName);
    } else if (d_property_type == GenericProperty::TypePath) {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),d_default_open_path,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty())
            ui->txtEditor->setText(dir);
    } else if (d_property_type == GenericProperty::TypeFileList) {
        StringListWidget* string_list_widget = new StringListWidget;
        string_list_widget->setListType(StringListWidget::FilePaths);
        string_list_widget->setFileOpenDialogFilter(d_file_filter);
        string_list_widget->setFileOpenDialogPath(d_default_open_path);
        string_list_widget->setStringList(d_current_values);
        string_list_widget->setReadOnly(!d_editable);

        GenericPropertyPathEditorListWrapper dialog(string_list_widget);
        if (d_property_name.isEmpty())
            dialog.setWindowTitle(tr("File List"));
        else
            dialog.setWindowTitle(d_property_name);

        if (dialog.exec()) {
            ui->txtEditor->setText(string_list_widget->stringList().join(d_list_separator_backend));
        }
    } else if (d_property_type == GenericProperty::TypePathList) {
        StringListWidget* string_list_widget = new StringListWidget;
        string_list_widget->setListType(StringListWidget::Directories);
        string_list_widget->setFileOpenDialogFilter(d_file_filter);
        string_list_widget->setFileOpenDialogPath(d_default_open_path);
        string_list_widget->setStringList(d_current_values);
        string_list_widget->setReadOnly(!d_editable);

        GenericPropertyPathEditorListWrapper dialog(string_list_widget);
        if (d_property_name.isEmpty())
            dialog.setWindowTitle(tr("Path List"));
        else
            dialog.setWindowTitle(d_property_name);

        if (dialog.exec()) {
            ui->txtEditor->setText(string_list_widget->stringList().join(d_list_separator_backend));
        }
    }
}

void GenericPropertyPathEditor::handleValueChanged(const QString &value) {
    d_current_values = value.split(d_list_separator_backend,QString::SkipEmptyParts);
    emit valueChanged(value);
}

}
}

#endif
