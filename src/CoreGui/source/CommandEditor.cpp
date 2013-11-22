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

    ObserverWidget*                         observer_widget;
    qti_private_CommandTreeModel*           model;
    qti_private_ShortcutEditorDelegate*     shortcut_delegate;
    QAction*                                action_restore_defaults;
    QAction*                                action_save;
    QAction*                                action_load;
};

Qtilities::CoreGui::CommandEditor::CommandEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandEditor)
{
    d = new CommandEditorPrivateData;
    d->observer_widget = new ObserverWidget;
    ui->setupUi(this);
}

Qtilities::CoreGui::CommandEditor::~CommandEditor() {
    delete d;
    delete ui;
}

Qtilities::CoreGui::ObserverWidget* Qtilities::CoreGui::CommandEditor::commandWidget() const {
    return d->observer_widget;
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
    if (IConfigPage::configPageCategory().isEmpty())
        return QtilitiesCategory(tr("General"));
    else
        return IConfigPage::configPageCategory();
}

void Qtilities::CoreGui::CommandEditor::configPageApply() {
    QString shortcut_mapping_file = QString("%1%3%2").arg(QtilitiesApplication::applicationSessionPath()).arg(qti_def_PATH_SHORTCUTS_FILE).arg(QDir::separator());
    if (!ACTION_MANAGER->saveShortcutMapping(shortcut_mapping_file)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Shortcut Export Failed"));
        msgBox.setText(tr("Shortcut mapping export failed. Please see the session log for details."));
        msgBox.exec();
    }
}

void Qtilities::CoreGui::CommandEditor::configPageInitialize() {
    if (ui->widgetCommandsHolder->layout())
        delete ui->widgetCommandsHolder->layout();

    QHBoxLayout* layout = new QHBoxLayout(ui->widgetCommandsHolder);
    layout->setMargin(0);
    layout->addWidget(d->observer_widget);

    d->model = new qti_private_CommandTreeModel(this);
    d->observer_widget->setCustomTreeModel(d->model);
    d->observer_widget->setObserverContext(ACTION_MANAGER->commandObserver());

    d->action_restore_defaults = new QAction(QIcon(qti_icon_EDIT_PASTE_16x16),"Restore Defaults",this);
    connect(d->action_restore_defaults,SIGNAL(triggered()),SLOT(restoreDefaults()));
    d->action_load = new QAction(QIcon(qti_icon_FILE_OPEN_16x16),"Load Previous Configuration",this);
    connect(d->action_load,SIGNAL(triggered()),SLOT(importConfiguration()));
    d->action_save = new QAction(QIcon(qti_icon_FILE_SAVE_16x16),"Save Current Configuration",this);
    connect(d->action_save,SIGNAL(triggered()),SLOT(exportConfiguration()));
    d->observer_widget->actionProvider()->addAction(d->action_restore_defaults,QtilitiesCategory("Current Configuration"));
    d->observer_widget->actionProvider()->addAction(d->action_load,QtilitiesCategory("Current Configuration"));
    d->observer_widget->actionProvider()->addAction(d->action_save,QtilitiesCategory("Current Configuration"));

    ACTION_MANAGER->commandObserver()->startProcessingCycle();
    d->observer_widget->initialize();
    ACTION_MANAGER->commandObserver()->endProcessingCycle(false);

    d->shortcut_delegate = new qti_private_ShortcutEditorDelegate(this);
    if (d->observer_widget->treeView()) {
        d->observer_widget->treeView()->setItemDelegate(d->shortcut_delegate);
        d->observer_widget->treeView()->setAlternatingRowColors(true);
        d->observer_widget->treeView()->sortByColumn(d->model->columnPosition(AbstractObserverItemModel::ColumnName));
        QHeaderView* table_header = d->observer_widget->treeView()->header();
        if (table_header) {
            #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            table_header->setResizeMode(d->model->columnPosition(AbstractObserverItemModel::ColumnLast)+1,QHeaderView::Stretch);
            #else
            table_header->setSectionResizeMode(d->model->columnPosition(AbstractObserverItemModel::ColumnLast)+1,QHeaderView::Stretch);
            #endif
        }
    }

    d->observer_widget->show();
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
    msgBox.setIcon(QMessageBox::Question);
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
            d->observer_widget->refresh();
            if (d->observer_widget->treeView())
                d->observer_widget->treeView()->sortByColumn(d->model->columnPosition(AbstractObserverItemModel::ColumnName));
        }
    }
}

