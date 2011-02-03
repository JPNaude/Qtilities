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

#include "CommandEditor.h"
#include "CommandTableModel.h"
#include "QtilitiesApplication.h"
#include "ObjectPropertyBrowser.h"
#include "ui_CommandEditor.h"
#include "SearchBoxWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QGridLayout>
#include <QTableView>
#include <QMessageBox>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QHBoxLayout>

using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::CommandEditorData {
    CommandEditorData() : model(0),
    proxy_model(0)  { }

    CommandTableModel* model;
    QTableView* table_view;
    #ifndef QTILITIES_NO_PROPERTY_BROWSER
    ObjectPropertyBrowser* property_browser;
    #endif
    QSortFilterProxyModel* proxy_model;
};

Qtilities::CoreGui::CommandEditor::CommandEditor(bool debug_mode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandEditor)
{
    ui->setupUi(this);
    ui->lblSearchIcon->setPixmap(QPixmap(ICON_FIND_16x16));
    connect(ui->txtSearchString,SIGNAL(textChanged(const QString&)),SLOT(handleSearchStringChanged(const QString&)));
    connect(ui->commandTable->verticalHeader(),SIGNAL(sectionCountChanged(int,int)),SLOT(resizeCommandTableRows()));

    d = new CommandEditorData;

    // Create Model & View
    d->model = new CommandTableModel(this);
    if (d->model) {
        // Create the proxy model
        d->proxy_model = new QSortFilterProxyModel(this);
        d->proxy_model->setDynamicSortFilter(true);
        d->proxy_model->setSourceModel(d->model);
        d->proxy_model->setFilterKeyColumn(0);
        ui->commandTable->setModel(d->proxy_model);
        ui->commandTable->resizeRowsToContents();
        ui->commandTable->resizeColumnsToContents();
        ui->commandTable->horizontalHeader()->setStretchLastSection(true);
        ui->commandTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->commandTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        ui->commandTable->resizeColumnsToContents();
        QHeaderView* table_header = ui->commandTable->horizontalHeader();
        table_header->setResizeMode(1,QHeaderView::Stretch);
    }

    // Create Property Browser
    #ifndef QTILITIES_NO_PROPERTY_BROWSER
    d->property_browser = new ObjectPropertyBrowser(ObjectPropertyBrowser::GroupBoxBrowser,0);
    if (d->property_browser) {
        QStringList filter_list;
        filter_list << "QObject" << "Action" << "MultiContextAction" << "ShortcutCommand";
        d->property_browser->setFilterList(filter_list);
        d->property_browser->layout()->setMargin(0);
    }
    #endif

    // Create Layout
    QGridLayout* layout = new QGridLayout();
    if (ui->widgetPropertyEditor->layout())
        delete ui->widgetPropertyEditor->layout();
    #ifndef QTILITIES_NO_PROPERTY_BROWSER
    layout->addWidget(d->property_browser);
    #endif
    layout->setMargin(0);
    ui->widgetPropertyEditor->setLayout(layout);
    ui->commandTable->setSortingEnabled(true);
    ui->commandTable->sortByColumn(0,Qt::AscendingOrder);

    // Select first item
    connect(ui->commandTable->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),SLOT(handleCurrentRowChanged(QModelIndex,QModelIndex)));
    ui->commandTable->setCurrentIndex(d->model->index(0,0));

    if (debug_mode) {
        ui->groupCurrentConfiguration->setVisible(false);
        ui->widgetSearchBox->setVisible(false);
        ui->widgetPropertyEditor->setVisible(false);
    }
}

Qtilities::CoreGui::CommandEditor::~CommandEditor()
{
    delete ui;
    delete d;
}

void Qtilities::CoreGui::CommandEditor::resizeCommandTableRows() {
    ui->commandTable->sortByColumn(0,Qt::AscendingOrder);
    for (int i = 0; i < d->model->rowCount(); i++) {
        ui->commandTable->setRowHeight(i,17);
    }
}

QIcon Qtilities::CoreGui::CommandEditor::configPageIcon() const {
    return QIcon(ICON_CONFIG_SHORTCUTS_48x48);
}

QWidget* Qtilities::CoreGui::CommandEditor::configPageWidget() {
    return this;
}

QString Qtilities::CoreGui::CommandEditor::configPageTitle() const {
    return tr("Shortcuts");
}

QtilitiesCategory Qtilities::CoreGui::CommandEditor::configPageCategory() const {
    return QtilitiesCategory("General");
}

void Qtilities::CoreGui::CommandEditor::configPageApply() {

}

void Qtilities::CoreGui::CommandEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::CoreGui::CommandEditor::handleCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous)

    if (d->proxy_model) {
        if (current.row() >= 0 && current.row() < ACTION_MANAGER->commandMap().count()) {
            QModelIndex original_index = d->proxy_model->mapToSource(current);
            #ifndef QTILITIES_NO_PROPERTY_BROWSER
            if (original_index.isValid())
                d->property_browser->setObject(ACTION_MANAGER->commandMap().values().at(d->proxy_model->mapToSource(current).row()));
            #endif

            emit selectedCommandChanged(ACTION_MANAGER->commandMap().values().at(d->proxy_model->mapToSource(current).row()));
        }
    }
}

void Qtilities::CoreGui::CommandEditor::on_btnDefaults_clicked() {
    QMessageBox msgBox;
    msgBox.setText(tr("Restore Default Shortcuts"));
    msgBox.setInformativeText(tr("This will overwrite your current shortcut configuration.\n\nDo you want to continue?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    switch (ret) {
        case QMessageBox::Yes:
             ACTION_MANAGER->restoreDefaultShortcuts();
             break;
        case QMessageBox::No:
             break;
        default:
             // should never be reached
             break;
    }
}

void Qtilities::CoreGui::CommandEditor::on_btnExport_clicked() {
    QString file_name = QFileDialog::getSaveFileName(this,tr("Export Shortcut Mapping"), QDir::currentPath(), tr("Shortcut Mapping File (*.smf)"));
    if (!file_name.isEmpty()) {
        if (!ACTION_MANAGER->exportShortcutMapping(file_name)) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Shortcut Export Failed");
            msgBox.setText(tr("Shortcut mapping export failed. Please see the session log for details."));
            msgBox.exec();
        }
    }
}

void Qtilities::CoreGui::CommandEditor::on_btnImport_clicked() {
    QString file_name = QFileDialog::getOpenFileName(this,tr("Import Shortcut Mapping"), QDir::currentPath(), tr("Shortcut Mapping File (*.smf)"));
    if (!file_name.isEmpty()) {
        if (!ACTION_MANAGER->importShortcutMapping(file_name)) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Shortcut Import Failed");
            msgBox.setText(tr("Shortcut mapping import failed. Please see the session log for details."));
            msgBox.exec();
        } else
            d->model->refreshModel();
    }
}

void Qtilities::CoreGui::CommandEditor::handleSearchStringChanged(const QString& text) {
    QRegExp::PatternSyntax syntax = QRegExp::PatternSyntax(QRegExp::FixedString);
    Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;;
    QRegExp regExp(text, caseSensitivity, syntax);
    d->proxy_model->setFilterRegExp(regExp);
    ui->commandTable->resizeRowsToContents();
}
