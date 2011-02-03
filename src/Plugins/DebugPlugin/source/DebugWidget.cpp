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

#include "DebugWidget.h"
#include "ui_DebugWidget.h"
#include "DropableListWidget.h"

#include <QPointer>
#include <QAction>

//#include <Conan.h>

#include <QtilitiesExtensionSystem>
using namespace QtilitiesExtensionSystem;

struct Qtilities::Plugins::Debug::DebugWidgetData {
    DebugWidgetData() : objectPoolWidget(0),plugin_edit_set_loaded(false),command_editor(true)/*,
                        conanWidget(0)*/ {}

    ObserverWidget*     objectPoolWidget;
    //ConanWidget*    conanWidget;
    bool                plugin_edit_set_loaded;
    QStringListModel    active_plugins_model;
    QStringListModel    inactive_plugins_model;
    QStringListModel    filtered_plugins_model;
    DropableListWidget* active_plugins_view;
    DropableListWidget* inactive_plugins_view;
    DropableListWidget* filtered_plugins_view;
    QTimer              plugin_msg_timer;
    CommandEditor       command_editor;
    ObjectScopeWidget   object_scope_widget;
};

Qtilities::Plugins::Debug::DebugWidget::DebugWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    ui->btnRefreshViews->setIcon(QIcon(ICON_REFRESH_16x16));
    ui->btnRefreshViews->setText(tr("Refresh Views"));
    ui->btnRemoveFilterExpression->setIcon(QIcon(ICON_REMOVE_ONE_16x16));
    ui->btnAddFilterExpression->setIcon(QIcon(ICON_NEW_16x16));
    ui->btnAddActivePlugin->setIcon(QIcon(ICON_NEW_16x16));
    ui->btnRemoveActivePlugin->setIcon(QIcon(ICON_REMOVE_ONE_16x16));
    ui->btnAddInactivePlugin->setIcon(QIcon(ICON_NEW_16x16));
    ui->btnRemoveInactivePlugin->setIcon(QIcon(ICON_REMOVE_ONE_16x16));
    ui->lblPluginInfoIcon->setText("");
    ui->lblPluginInfoMessage->setText("");

    d = new DebugWidgetData;
    connect(&d->plugin_msg_timer,SIGNAL(timeout()), ui->lblPluginInfoIcon, SLOT(hide()));
    connect(&d->plugin_msg_timer,SIGNAL(timeout()), ui->lblPluginInfoMessage, SLOT(clear()));
    connect(&d->command_editor,SIGNAL(selectedCommandChanged(Command*)),SLOT(refreshCommandInformation(Command*)));

    setObjectName("Qtilities Debug Mode");

    // Version:
    ui->labelVersion->setText(QtilitiesApplication::qtilitiesVersion());

    // Global Object Pool:
    d->objectPoolWidget = new ObserverWidget(Qtilities::TreeView);
    d->objectPoolWidget->setObserverContext(OBJECT_MANAGER->objectPool());
    OBJECT_MANAGER->objectPool()->endProcessingCycle();
    OBJECT_MANAGER->objectPool()->useDisplayHints();
    OBJECT_MANAGER->objectPool()->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::ActionFindItem;
    OBJECT_MANAGER->objectPool()->displayHints()->setActionHints(action_hints);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ActionToolBar;
    display_flags |= ObserverHints::ItemView;
    OBJECT_MANAGER->objectPool()->displayHints()->setDisplayFlagsHint(display_flags);
    OBJECT_MANAGER->objectPool()->displayHints()->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    OBJECT_MANAGER->objectPool()->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    OBJECT_MANAGER->objectPool()->refreshViewsLayout();

    if (ui->widgetObjectPoolHolder->layout())
        delete ui->widgetObjectPoolHolder->layout();

    QVBoxLayout* object_pool_layout = new QVBoxLayout(ui->widgetObjectPoolHolder);
    object_pool_layout->setMargin(0);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    object_pool_layout->addWidget(splitter);

    d->objectPoolWidget->initialize();
    d->objectPoolWidget->show();
    d->objectPoolWidget->toggleSearchBox();
    connect(d->objectPoolWidget,SIGNAL(doubleClickRequest(QObject*)),SLOT(handle_objectPoolDoubleClick(QObject*)));
    connect(d->objectPoolWidget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),SLOT(handle_objectPoolSelectionChanged(QList<QObject*>)));

    // Conan Widget:
    //d->conanWidget = new ConanWidget();
    //d->conanWidget->show();

    // Splitter
    splitter->addWidget(d->objectPoolWidget);
    splitter->addWidget(&d->object_scope_widget);

    // Factories:
    connect(ui->listFactories,SIGNAL(currentTextChanged(QString)),SLOT(handle_factoryListSelectionChanged(QString)));

    // Modes:
    ui->tableModes->setAlternatingRowColors(true);
    ui->tableModes->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Action Management
    if (ui->commandEditorHolder->layout())
        delete ui->commandEditorHolder->layout();
    QHBoxLayout* command_editor_layout = new QHBoxLayout(ui->commandEditorHolder);
    command_editor_layout->addWidget(&d->command_editor);
    command_editor_layout->setMargin(0);
    if (d->command_editor.layout())
        d->command_editor.layout()->setMargin(0);

    // Plugins:
    if (ui->listPluginsFilteredHolder->layout())
        delete ui->listPluginsFilteredHolder->layout();
    d->filtered_plugins_view = new DropableListWidget("Filtered");
    connect(d->filtered_plugins_view,SIGNAL(newMessage(Logger::MessageType,QString)),SLOT(handleListMessage(Logger::MessageType,QString)));
    QHBoxLayout* filtered_plugins_layout = new QHBoxLayout(ui->listPluginsFilteredHolder);
    filtered_plugins_layout->addWidget(d->filtered_plugins_view);
    filtered_plugins_layout->setMargin(0);

    if (ui->listPluginsInactiveHolder->layout())
        delete ui->listPluginsInactiveHolder->layout();
    d->inactive_plugins_view = new DropableListWidget("Inactive");
    connect(d->inactive_plugins_view,SIGNAL(newMessage(Logger::MessageType,QString)),SLOT(handleListMessage(Logger::MessageType,QString)));
    QHBoxLayout* inactive_plugins_layout = new QHBoxLayout(ui->listPluginsInactiveHolder);
    inactive_plugins_layout->addWidget(d->inactive_plugins_view);
    inactive_plugins_layout->setMargin(0);

    if (ui->listPluginsActiveHolder->layout())
        delete ui->listPluginsActiveHolder->layout();
    d->active_plugins_view = new DropableListWidget("Active");
    connect(d->active_plugins_view,SIGNAL(newMessage(Logger::MessageType,QString)),SLOT(handleListMessage(Logger::MessageType,QString)));
    QHBoxLayout* active_plugins_layout = new QHBoxLayout(ui->listPluginsActiveHolder);
    active_plugins_layout->addWidget(d->active_plugins_view);
    active_plugins_layout->setMargin(0);

    d->active_plugins_view->setModel(&d->active_plugins_model);
    d->inactive_plugins_view->setModel(&d->inactive_plugins_model);
    d->filtered_plugins_view->setModel(&d->filtered_plugins_model);

    // Lastly we refresh all views:
    on_btnRefreshViews_clicked();
}

Qtilities::Plugins::Debug::DebugWidget::~DebugWidget()
{
    delete ui;
}

QWidget* Qtilities::Plugins::Debug::DebugWidget::modeWidget() {
    return this;
}

QIcon Qtilities::Plugins::Debug::DebugWidget::modeIcon() const {
    return QIcon(":/debug_log_mode.png");
}

QString Qtilities::Plugins::Debug::DebugWidget::modeName() const {
    return tr("Qtilities Debugging");
}

void Qtilities::Plugins::Debug::DebugWidget::aboutToBeActivated() {
    on_btnRefreshViews_clicked();
}

void Qtilities::Plugins::Debug::DebugWidget::finalizeMode() {
    if (d->objectPoolWidget->treeView())
        d->objectPoolWidget->treeView()->expandAll();
    OBJECT_MANAGER->objectPool()->startProcessingCycle();
    if (OBJECT_MANAGER->objectPool()->subjectCount() > 0) {
        QList<QObject*> object_list;
        object_list << OBJECT_MANAGER->objectPool()->subjectAt(0);
        d->objectPoolWidget->selectObjects(object_list);
    }
}

void Qtilities::Plugins::Debug::DebugWidget::handle_factoryListSelectionChanged(const QString& factory_name) {
    ui->listFactoryItemIDs->clear();
    ui->listFactoryItemIDs->addItems(OBJECT_MANAGER->tagsForFactory(factory_name));
}

void Qtilities::Plugins::Debug::DebugWidget::handle_objectPoolDoubleClick(QObject *object) {
    //d->conanWidget->AddRootObject(object);
}

void Qtilities::Plugins::Debug::DebugWidget::handle_objectPoolSelectionChanged(QList<QObject*> objects) {
    if (objects.count() == 1) {
        d->object_scope_widget.setObject(objects.front());
    }
}

void Qtilities::Plugins::Debug::DebugWidget::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnRefreshViews_clicked() {
    // ===============================
    // Refresh Plugins:
    // ===============================
    refreshCurrentPluginState();
    refreshCurrentPluginSet();
    refreshEditedPluginState();

    // ===============================
    // Refresh Modes:
    // ===============================
    refreshModes();

    // ===============================
    // Refresh Contexts:
    // ===============================
    refreshContexts();

    // ===============================
    // Action Management:
    // ===============================
    refreshCommandInformation(0);

    // ===============================
    // Refresh Factories:
    // ===============================
    refreshFactories();

    // Refresh Global Object Pool:
    d->objectPoolWidget->refresh();
    d->objectPoolWidget->viewExpandAll();
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnExplorePluginConfigSetPath_clicked()
{
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QDesktopServices explorer_service;
    QFileInfo fi(ui->txtPluginsActiveSet->text());
    if (!explorer_service.openUrl(QUrl(QUrl::fromLocalFile(fi.path())))){
        LOG_ERROR_P(QString("Failed to open folder: %1").arg(fi.path()));
    }
    else
        LOG_INFO_P(QString("Successfully opened folder: %1").arg(fi.path()));
}

bool Qtilities::Plugins::Debug::DebugWidget::on_btnSavePluginConfigSet_clicked()
{
    QStringList inactive_list = d->inactive_plugins_model.stringList();
    QStringList filter_list = d->filtered_plugins_model.stringList();
    return EXTENSION_SYSTEM->savePluginConfiguration(ui->txtPluginsActiveSet->text(),&inactive_list,&filter_list);
}

bool Qtilities::Plugins::Debug::DebugWidget::on_btnSaveNewPluginConfigSet_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(0, tr("Save Plugin Configuration"),QApplication::applicationDirPath() + "/plugins",tr("Plugin Configurations (*.pconfig)"));
    if (!fileName.isEmpty()) {
        QStringList inactive_list = d->inactive_plugins_model.stringList();
        QStringList filter_list = d->filtered_plugins_model.stringList();
        return EXTENSION_SYSTEM->savePluginConfiguration(fileName,&inactive_list,&filter_list);
        ui->txtPluginsEditSet->setText(fileName);
        return true;
    } else
        return false;
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnEditPluginConfigSet_clicked()
{
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QDesktopServices explorer_service;
    QFileInfo fi(ui->txtPluginsActiveSet->text());
    if (!explorer_service.openUrl(QUrl(QUrl::fromLocalFile(fi.filePath())))){
        LOG_ERROR_P(QString("Failed to open folder: %1").arg(fi.filePath()));
    }
    else
        LOG_INFO_P(QString("Successfully opened folder: %1").arg(fi.filePath()));
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnExplorePluginCurrentConfigSetPath_clicked()
{
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QDesktopServices explorer_service;
    QFileInfo fi(ui->txtPluginsEditSet->text());
    if (!explorer_service.openUrl(QUrl(QUrl::fromLocalFile(fi.path())))){
        LOG_ERROR_P(QString("Failed to open folder: %1").arg(fi.path()));
    }
    else
        LOG_INFO_P(QString("Successfully opened folder: %1").arg(fi.path()));
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnEditPluginCurrentConfigSet_clicked()
{
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QDesktopServices explorer_service;
    QFileInfo fi(ui->txtPluginsEditSet->text());
    if (!explorer_service.openUrl(QUrl(QUrl::fromLocalFile(fi.filePath())))){
        LOG_ERROR_P(QString("Failed to open file: %1").arg(fi.filePath()));
    }
    else
        LOG_INFO_P(QString("Successfully opened file: %1").arg(fi.filePath()));
}


void Qtilities::Plugins::Debug::DebugWidget::on_btnOpenPluginConfigSet_clicked()
{
    QString open_path = QApplication::applicationDirPath();
    if (!ui->txtPluginsActiveSet->text().isEmpty()) {
        QFileInfo fi(ui->txtPluginsActiveSet->text());
        open_path = fi.path();
    }
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open Plugin Configuration"),open_path,tr("Plugin Configurations (*.pconfig)"));
    if (!fileName.isEmpty()) {
        QStringList inactive_list;
        QStringList filter_list;
        EXTENSION_SYSTEM->loadPluginConfiguration(fileName,&inactive_list,&filter_list);
        ui->txtPluginsEditSet->setText(fileName);
        d->inactive_plugins_model.setStringList(inactive_list);
        d->filtered_plugins_model.setStringList(filter_list);
    }
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnRemoveActivePlugin_clicked()
{
    d->active_plugins_model.removeRow(d->active_plugins_view->currentIndex().row());
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnAddActivePlugin_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add active plugin"),tr("Plugin name:"), QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()) {
        QStringList list = d->active_plugins_model.stringList();
        list << text;
        list.removeDuplicates();
        d->active_plugins_model.setStringList(list);
    }
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnRemoveInactivePlugin_clicked()
{
    d->inactive_plugins_model.removeRow(d->inactive_plugins_view->currentIndex().row());
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnAddInactivePlugin_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add inactive plugin"),tr("Plugin name:"), QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()) {
        QStringList list = d->inactive_plugins_model.stringList();
        list << text;
        list.removeDuplicates();
        d->inactive_plugins_model.setStringList(list);
    }
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnRemoveFilterExpression_clicked()
{
    d->filtered_plugins_model.removeRow(d->filtered_plugins_view->currentIndex().row());
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnAddFilterExpression_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add filter expression"),tr("Filter expression:"), QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()) {
        QRegExp reg_exp(text);
        reg_exp.setPatternSyntax(QRegExp::Wildcard);
        while (!reg_exp.isValid()) {
            QString msg_string = QString(tr("You entered an invalid regular expression:\n\nError: %1\n\nNew expression:")).arg(reg_exp.errorString());
            QString text = QInputDialog::getText(this, tr("Expression Is Not Valid"),msg_string, QLineEdit::Normal,reg_exp.pattern(), &ok);
            if (ok && !text.isEmpty())
                reg_exp.setPattern(text);
            else
                return;
        }

        QStringList list = d->filtered_plugins_model.stringList();
        list << text;
        list.removeDuplicates();
        d->filtered_plugins_model.setStringList(list);
    }
}

void Qtilities::Plugins::Debug::DebugWidget::handleListMessage(Logger::MessageType type, const QString& msg) {
    if (type == Logger::Warning) {
        ui->lblPluginInfoIcon->setPixmap(QIcon(ICON_WARNING_16x16).pixmap(12));
        ui->lblPluginInfoIcon->setVisible(true);
        ui->lblPluginInfoMessage->setText(QString("<font color='red'>%1</font>").arg(msg));
    } else if (type == Logger::Error || type == Logger::Fatal) {
        ui->lblPluginInfoIcon->setPixmap(QIcon(ICON_ERROR_16x16).pixmap(12));
        ui->lblPluginInfoIcon->setVisible(true);
        ui->lblPluginInfoMessage->setText(QString("<font color='red'>%1</font>").arg(msg));
    } else {
        if (msg.startsWith(tr("Successfully")))
            ui->lblPluginInfoIcon->setPixmap(QIcon(ICON_SUCCESS_16x16).pixmap(12));
        else
            ui->lblPluginInfoIcon->setPixmap(QIcon(ICON_INFO_16x16).pixmap(12));
        ui->lblPluginInfoIcon->setVisible(true);
        ui->lblPluginInfoMessage->setText(msg);
    }

    d->plugin_msg_timer.start(30000);
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnContextSetActive_clicked()
{
    QList<QTableWidgetItem*> selected_items = ui->tableContextsAll->selectedItems();
    CONTEXT_MANAGER->setNewContext(Qtilities::Core::Constants::CONTEXT_STANDARD);
    for (int i = 0; i < selected_items.count(); i++) {
        CONTEXT_MANAGER->appendContext(selected_items.at(i)->text());
    }
    refreshContexts();
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnContextsClear_clicked()
{
    CONTEXT_MANAGER->setNewContext(Qtilities::Core::Constants::CONTEXT_STANDARD);
    refreshContexts();
}

void Qtilities::Plugins::Debug::DebugWidget::on_btnContextsBroadcast_clicked()
{
    CONTEXT_MANAGER->broadcastState();
    refreshContexts();
}

void Qtilities::Plugins::Debug::DebugWidget::refreshModes() {
    ui->tableModes->clear();
    QStringList mode_headers;
    mode_headers << "Mode Name" << "Mode ID" << "Mode Shortcut" << "Context String" << "Help ID";
    ui->tableModes->setHorizontalHeaderLabels(mode_headers);
    ui->tableModes->setSortingEnabled(false);

    QtilitiesMainWindow* mainWindow = qobject_cast<QtilitiesMainWindow*> (QtilitiesApplication::mainWindow());
    if (mainWindow) {
        ui->tableModes->setRowCount(mainWindow->modeManager()->modes().count());
        for (int i = 0; i < mainWindow->modeManager()->modes().count(); i++) {
            IMode* mode = mainWindow->modeManager()->modes().at(i);

            // Mode Name
            QTableWidgetItem *newItem = new QTableWidgetItem(mode->modeName());
            ui->tableModes->setItem(i, 0, newItem);
            // Mode ID
            newItem = new QTableWidgetItem(QString::number(mode->modeID()));
            ui->tableModes->setItem(i, 1, newItem);
            // Mode Shortcut
            newItem = new QTableWidgetItem(mainWindow->modeManager()->modeShortcut(mode->modeID()));
            ui->tableModes->setItem(i, 2, newItem);
            // Mode Context String
            newItem = new QTableWidgetItem(mode->contextString());
            ui->tableModes->setItem(i, 3, newItem);
            // Mode Help ID
            newItem = new QTableWidgetItem(mode->contextHelpId());
            ui->tableModes->setItem(i, 4, newItem);

            ui->tableModes->setRowHeight(i,17);
        }
    }

    ui->tableModes->resizeColumnsToContents();
    ui->tableModes->horizontalHeader()->setStretchLastSection(true);
    ui->tableModes->setSortingEnabled(true);
    ui->tableModes->sortByColumn(0,Qt::AscendingOrder);
    ui->tableModes->setShowGrid(false);
    ui->tableModes->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Qtilities::Plugins::Debug::DebugWidget::refreshContexts() {
    ui->tableContextsAll->clear();
    ui->tableContextsActive->clear();
    QStringList context_headers;
    context_headers << "Context ID" << "Context Name";
    ui->tableContextsAll->setHorizontalHeaderLabels(context_headers);
    ui->tableContextsAll->setSortingEnabled(false);
    ui->tableContextsActive->setHorizontalHeaderLabels(context_headers);
    ui->tableContextsActive->setSortingEnabled(false);

    // First do all contexts:
    QList<int> all_contexts = CONTEXT_MANAGER->allContexts();
    ui->tableContextsAll->setRowCount(all_contexts.count());
    for (int i = 0; i < all_contexts.count(); i++) {
        int current_id = all_contexts.at(i);
        // Context ID:
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(current_id));
        ui->tableContextsAll->setItem(i, 0, newItem);
        // Context Name:
        newItem = new QTableWidgetItem(CONTEXT_MANAGER->contextString(current_id));
        ui->tableContextsAll->setItem(i, 1, newItem);
        ui->tableContextsAll->setRowHeight(i,17);
    }

    // Next do active contexts:
    QList<int> active_contexts = CONTEXT_MANAGER->activeContexts();
    ui->tableContextsActive->setRowCount(active_contexts.count());
    for (int i = 0; i < active_contexts.count(); i++) {
        int current_id = active_contexts.at(i);
        // Context ID:
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(current_id));
        ui->tableContextsActive->setItem(i, 0, newItem);
        // Context Name:
        newItem = new QTableWidgetItem(CONTEXT_MANAGER->contextString(current_id));
        ui->tableContextsActive->setItem(i, 1, newItem);
        ui->tableContextsActive->setRowHeight(i,17);
    }

    ui->tableContextsAll->horizontalHeader()->setStretchLastSection(true);
    ui->tableContextsAll->sortByColumn(0,Qt::AscendingOrder);
    ui->tableContextsAll->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableContextsAll->setShowGrid(false);
    ui->tableContextsAll->setAlternatingRowColors(true);
    ui->tableContextsAll->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableContextsAll->setSortingEnabled(true);
    ui->tableContextsActive->horizontalHeader()->setStretchLastSection(true);
    ui->tableContextsActive->sortByColumn(0,Qt::AscendingOrder);
    ui->tableContextsActive->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableContextsActive->setShowGrid(false);
    ui->tableContextsActive->setAlternatingRowColors(true);
    ui->tableContextsActive->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableContextsActive->setSortingEnabled(true);
}

void Qtilities::Plugins::Debug::DebugWidget::refreshCommandInformation(Command* command) {
    if (!command) {
        ui->tableSelectedActionOverview->clear();
        ui->lblCommandType->setText("No command selected.");
        ui->tableSelectedActionOverview->setEnabled(false);
        return;
    }

    // Check what type of command it is:
    MultiContextAction* multi_action = qobject_cast<MultiContextAction*> (command);
    if (multi_action) {
        ui->lblCommandType->setText("Command Type: Multi Context Action");

        // ===============================
        // Refresh Table:
        // ===============================
        ui->tableSelectedActionOverview->clear();
        QStringList context_headers;
        ui->tableSelectedActionOverview->setColumnCount(8);
        context_headers << "Backend Action Text" << "Parent" << "Is Active" << "Target Contexts" << "Shortcut" << "Default Shortcut" << "Icon" << "Tooltip";
        ui->tableSelectedActionOverview->setHorizontalHeaderLabels(context_headers);
        ui->tableSelectedActionOverview->setSortingEnabled(false);

        QHash<int, QPointer<QAction> > id_action_map = multi_action->contextIDActionMap();
        ui->tableSelectedActionOverview->setRowCount(id_action_map.count());
        for (int i = 0; i < id_action_map.count(); i++) {
            int current_id = id_action_map.keys().at(i);
            QPointer<QAction> current_action = id_action_map[id_action_map.keys().at(i)];
            if (!current_action)
                continue;
            // Backend Action Name:
            QTableWidgetItem *newItem = new QTableWidgetItem(current_action->text());
            ui->tableSelectedActionOverview->setItem(i, 0, newItem);
            // Parent Name:
            QString parent_name = "No Parent";
            if (current_action->parent())
                parent_name = current_action->parent()->objectName();
            newItem = new QTableWidgetItem(parent_name);
            ui->tableSelectedActionOverview->setItem(i, 1, newItem);
            // Is Active:
            QString is_active_text = "No";
            if (current_action == multi_action->activeBackendAction())
                is_active_text = "Yes";
            newItem = new QTableWidgetItem(is_active_text);
            ui->tableSelectedActionOverview->setItem(i, 2, newItem);
            // Target Context:
            newItem = new QTableWidgetItem(QString::number(current_id));
            ui->tableSelectedActionOverview->setItem(i, 3, newItem);
            // Shortcut:
            newItem = new QTableWidgetItem(current_action->shortcut().toString());
            ui->tableSelectedActionOverview->setItem(i, 4, newItem);
            // Default Shortcut:
            newItem = new QTableWidgetItem(command->defaultKeySequence().toString());
            ui->tableSelectedActionOverview->setItem(i, 5, newItem);
            // Icon:
            newItem = new QTableWidgetItem(current_action->icon(),"");
            ui->tableSelectedActionOverview->setItem(i, 6, newItem);
            // Tooltip:
            newItem = new QTableWidgetItem(current_action->toolTip());
            ui->tableSelectedActionOverview->setItem(i, 7, newItem);
            ui->tableSelectedActionOverview->setRowHeight(i,17);
        }
    }

    ShortcutCommand* shortcut = qobject_cast<ShortcutCommand*> (command);
    if (shortcut) {
        ui->lblCommandType->setText("Command Type: Shortcut");
        ui->tableSelectedActionOverview->clear();
        ui->tableSelectedActionOverview->setColumnCount(6);
        ui->tableSelectedActionOverview->setRowCount(1);
        QStringList context_headers;
        context_headers << "User Text" << "Parent" << "Is Active" << "Target Contexts" << "Shortcut" << "Default Shortcut";
        ui->tableSelectedActionOverview->setHorizontalHeaderLabels(context_headers);
        ui->tableSelectedActionOverview->setSortingEnabled(false);

        // Action Name:
        QTableWidgetItem *newItem = new QTableWidgetItem(shortcut->text());
        ui->tableSelectedActionOverview->setItem(0, 0, newItem);
        // Parent Name:
        QString parent_name = "No Parent";
        if (shortcut->parent())
            parent_name = shortcut->parent()->objectName();
        newItem = new QTableWidgetItem(parent_name);
        ui->tableSelectedActionOverview->setItem(0, 1, newItem);
        // Is Active:
        QString is_active_text = "No";
        if (shortcut->isActive())
            is_active_text = "Yes";
        newItem = new QTableWidgetItem(is_active_text);
        ui->tableSelectedActionOverview->setItem(0, 2, newItem);
        // Target Contexts:
        QStringList active_contexts;
        foreach (int context, shortcut->activeContexts())
            active_contexts << QString::number(context);
        newItem = new QTableWidgetItem(active_contexts.join(","));
        ui->tableSelectedActionOverview->setItem(0, 3, newItem);
        // Shortcut:
        newItem = new QTableWidgetItem(shortcut->shortcut()->key().toString());
        ui->tableSelectedActionOverview->setItem(0, 4, newItem);
        // Default Shortcut:
        newItem = new QTableWidgetItem(shortcut->defaultKeySequence().toString());
        ui->tableSelectedActionOverview->setItem(0, 5, newItem);
        ui->tableSelectedActionOverview->setRowHeight(0,17);
    }

    ui->tableSelectedActionOverview->resizeColumnsToContents();
    QHeaderView* table_header = ui->tableSelectedActionOverview->horizontalHeader();
    table_header->setResizeMode(0,QHeaderView::Stretch);
    ui->tableSelectedActionOverview->sortByColumn(0,Qt::AscendingOrder);
    ui->tableSelectedActionOverview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableSelectedActionOverview->setShowGrid(false);
    ui->tableSelectedActionOverview->setAlternatingRowColors(true);
    ui->tableSelectedActionOverview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableSelectedActionOverview->setSortingEnabled(true);
    ui->tableSelectedActionOverview->setEnabled(true);
}

void Qtilities::Plugins::Debug::DebugWidget::refreshCurrentPluginState() {
    ui->listPluginsCoreCurrent->clear();
    ui->listPluginsActiveCurrent->clear();
    ui->listPluginsInactiveCurrent->clear();
    ui->listPluginsFilteredCurrent->clear();
    ui->listPluginsCoreCurrent->addItems(EXTENSION_SYSTEM->corePlugins());
    ui->listPluginsActiveCurrent->addItems(EXTENSION_SYSTEM->activePlugins());
    ui->listPluginsInactiveCurrent->addItems(EXTENSION_SYSTEM->inactivePlugins());
    ui->listPluginsFilteredCurrent->addItems(EXTENSION_SYSTEM->filteredPlugins());
    ui->listPluginsCoreCurrent->sortItems();
    ui->listPluginsActiveCurrent->sortItems();
    ui->listPluginsInactiveCurrent->sortItems();
    ui->listPluginsFilteredCurrent->sortItems();
}

void Qtilities::Plugins::Debug::DebugWidget::refreshCurrentPluginSet() {
    if (EXTENSION_SYSTEM->activePluginConfigurationFile().isEmpty())
        ui->txtPluginsActiveSet->setText(tr("No plugin configuration set loaded"));
    else
        ui->txtPluginsActiveSet->setText(EXTENSION_SYSTEM->activePluginConfigurationFile());

    ui->listPluginsCoreCurrentSet->clear();
    ui->listPluginsActiveCurrentSet->clear();
    ui->listPluginsInactiveCurrentSet->clear();
    ui->listPluginsFilteredCurrentSet->clear();
    ui->listPluginsCoreCurrentSet->addItems(EXTENSION_SYSTEM->corePlugins());
    ui->listPluginsActiveCurrentSet->addItems(EXTENSION_SYSTEM->activePlugins());
    ui->listPluginsInactiveCurrentSet->addItems(EXTENSION_SYSTEM->inactivePluginsCurrentSet());
    ui->listPluginsFilteredCurrentSet->addItems(EXTENSION_SYSTEM->filteredPluginsCurrentSet());
    ui->listPluginsCoreCurrentSet->sortItems();
    ui->listPluginsActiveCurrentSet->sortItems();
    ui->listPluginsInactiveCurrentSet->sortItems();
    ui->listPluginsFilteredCurrentSet->sortItems();
}

void Qtilities::Plugins::Debug::DebugWidget::refreshEditedPluginState() {
    if (d->plugin_edit_set_loaded && !d->active_plugins_model.stringList().isEmpty() && !d->inactive_plugins_model.stringList().isEmpty() && !d->filtered_plugins_model.stringList().isEmpty())
        return;

    if (EXTENSION_SYSTEM->activePluginConfigurationFile().isEmpty())
        ui->txtPluginsEditSet->setText(tr("No plugin configuration set loaded"));
    else
        ui->txtPluginsEditSet->setText(EXTENSION_SYSTEM->activePluginConfigurationFile());

    d->active_plugins_model.setStringList(EXTENSION_SYSTEM->activePlugins());
    d->inactive_plugins_model.setStringList(EXTENSION_SYSTEM->inactivePlugins());
    d->filtered_plugins_model.setStringList(EXTENSION_SYSTEM->filteredPluginsCurrentSet());
    d->active_plugins_model.sort(0);
    d->inactive_plugins_model.sort(0);
    d->filtered_plugins_model.sort(0);
    ui->listPluginsCoreEditSet->clear();
    ui->listPluginsCoreEditSet->addItems(EXTENSION_SYSTEM->corePlugins());
    ui->listPluginsCoreEditSet->sortItems();

    if (EXTENSION_SYSTEM->activePluginConfigurationFile().isEmpty()) {
        ui->btnExplorePluginConfigSetPath->setEnabled(false);
        ui->btnEditPluginConfigSet->setEnabled(false);
        ui->btnSavePluginConfigSet->setEnabled(false);
    } else {
        ui->btnExplorePluginConfigSetPath->setEnabled(true);
        ui->btnEditPluginConfigSet->setEnabled(true);
        ui->btnSavePluginConfigSet->setEnabled(true);
    }

    d->plugin_edit_set_loaded = true;
}

void Qtilities::Plugins::Debug::DebugWidget::refreshFactories() {
    ui->listFactoryItemIDs->clear();
    ui->listFactories->clear();

    ui->listFactories->addItems(OBJECT_MANAGER->allFactoryNames());
    if (ui->listFactories->count() > 0)
        ui->listFactories->setCurrentRow(0);
}
