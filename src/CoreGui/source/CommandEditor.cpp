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
#include "CommandTreeModel.h"
#include "QtilitiesApplication.h"
#include "ObjectPropertyBrowser.h"
#include "ui_CommandEditor.h"
#include "SearchBoxWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ObserverWidget.h"

#include <QGridLayout>
#include <QTableView>
#include <QMessageBox>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QHBoxLayout>

using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::CommandEditorPrivateData {
    CommandEditorPrivateData() { }

    ObserverWidget              observer_widget;
    CommandTreeModel*           model;
    ShortcutEditorDelegate*     shortcut_delegate;
    QAction*                    action_restore_defaults;
    QAction*                    action_save;
    QAction*                    action_load;
};

Qtilities::CoreGui::CommandEditor::CommandEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandEditor)
{
    ui->setupUi(this);
    d = new CommandEditorPrivateData;

    if (ui->widgetCommandsHolder->layout())
        delete ui->widgetCommandsHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->widgetCommandsHolder);
    layout->setMargin(0);
    layout->addWidget(&d->observer_widget);

    d->model = new CommandTreeModel(this);
    d->observer_widget.setCustomTreeModel(d->model);
    d->observer_widget.setObserverContext(ACTION_MANAGER->commandObserver());

    d->action_restore_defaults = new QAction(QIcon(qti_icon_EDIT_PASTE_16x16),"Restore Defaults",this);
    connect(d->action_restore_defaults,SIGNAL(triggered()),SLOT(restoreDefaults()));
    d->action_load = new QAction(QIcon(qti_icon_FILE_OPEN_16x16),"Load Previous Configuration",this);
    connect(d->action_load,SIGNAL(triggered()),SLOT(importConfiguration()));
    d->action_save = new QAction(QIcon(qti_icon_FILE_SAVE_16x16),"Save Current Configuration",this);
    connect(d->action_save,SIGNAL(triggered()),SLOT(exportConfiguration()));
    d->observer_widget.actionProvider()->addAction(d->action_restore_defaults,QtilitiesCategory("Current Configuration"));
    d->observer_widget.actionProvider()->addAction(d->action_load,QtilitiesCategory("Current Configuration"));
    d->observer_widget.actionProvider()->addAction(d->action_save,QtilitiesCategory("Current Configuration"));

    d->observer_widget.initialize();

    d->shortcut_delegate = new ShortcutEditorDelegate(this);
    if (d->observer_widget.treeView()) {
        d->observer_widget.treeView()->setItemDelegate(d->shortcut_delegate);
        d->observer_widget.treeView()->sortByColumn(d->model->columnPosition(AbstractObserverItemModel::ColumnName));
        QHeaderView* table_header = d->observer_widget.treeView()->header();
        table_header->setResizeMode(d->model->columnPosition(AbstractObserverItemModel::ColumnLast)+1,QHeaderView::Stretch);
    }

    d->observer_widget.show();
}

Qtilities::CoreGui::CommandEditor::~CommandEditor()
{
    delete ui;
    delete d;
}

QIcon Qtilities::CoreGui::CommandEditor::configPageIcon() const {
    return QIcon(qti_icon_CONFIG_SHORTCUTS_48x48);
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

void Qtilities::CoreGui::CommandEditor::restoreDefaults() {
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

void Qtilities::CoreGui::CommandEditor::exportConfiguration() {
    QString file_name = QFileDialog::getSaveFileName(this,tr("Export Shortcut Mapping"), QDir::currentPath(), tr("Shortcut Mapping File (*.smf)"));
    if (!file_name.isEmpty()) {
        if (!ACTION_MANAGER->saveShortcutMapping(file_name)) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Shortcut Export Failed");
            msgBox.setText(tr("Shortcut mapping export failed. Please see the session log for details."));
            msgBox.exec();
        }
    }
}

void Qtilities::CoreGui::CommandEditor::importConfiguration() {
    QString file_name = QFileDialog::getOpenFileName(this,tr("Import Shortcut Mapping"), QDir::currentPath(), tr("Shortcut Mapping File (*.smf)"));
    if (!file_name.isEmpty()) {
        if (!ACTION_MANAGER->loadShortcutMapping(file_name)) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Shortcut Import Failed");
            msgBox.setText(tr("Shortcut mapping import failed. Please see the session log for details."));
            msgBox.exec();
        } else {
            d->observer_widget.refresh();
            if (d->observer_widget.treeView())
                d->observer_widget.treeView()->sortByColumn(d->model->columnPosition(AbstractObserverItemModel::ColumnName));
        }
    }
}
