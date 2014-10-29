/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of the Qtilities Debug Plugin.
** See Dependencies.pri for the licensing requirements of the Debug plugin.
** Alternatively, see http://www.qtilities.org/licensing.html.
**
****************************************************************************/

#include "DebugWidget.h"
#include "ui_DebugWidget.h"
#include "DropableListWidget.h"

#include <QPointer>
#include <QAction>
#include <QMessageBox>

#ifdef QTILITIES_CONAN
#include <Conan.h>
using namespace conan;
#endif

#include <QtilitiesExtensionSystem>
using namespace QtilitiesExtensionSystem;
#include <QtilitiesProjectManagement>
using namespace QtilitiesProjectManagement;

struct Qtilities::Testing::DebugWidgetPrivateData {
    DebugWidgetPrivateData() : object_pool_widget(0),
        plugin_edit_set_loaded(false) {}

    ObserverWidget*     object_pool_widget;
    ObserverWidget*     action_container_widget;
    QPointer<QObject>   current_object;
    ObjectScopeWidget   object_scope_widget;

    bool                plugin_edit_set_loaded;
    QStringListModel    active_plugins_model;
    QStringListModel    inactive_plugins_model;
    QStringListModel    filtered_plugins_model;
    qti_private_DropableListWidget* active_plugins_view;
    qti_private_DropableListWidget* inactive_plugins_view;
    qti_private_DropableListWidget* filtered_plugins_view;
    QTimer              plugin_msg_timer;

    QMainWindow         contexts_main_window;
    QDockWidget         contexts_dock_window;

    CommandEditor*      command_editor;
    QPointer<Command>   current_command;

    TaskSummaryWidget   task_summary_widget;

    #ifdef QTILITIES_CONAN
    QPointer<ConanWidget>   object_analysis_widget;
    QPointer<ConanWidget>   command_analysis_widget;
    QPointer<AboutDialog>   conan_about_dialog;
    #endif

    #ifdef QTILITIES_PROPERTY_BROWSER
    QPointer<ObjectPropertyBrowser>         object_property_browser;
    QPointer<ObjectDynamicPropertyBrowser>  object_dynamic_property_browser;
    #else
    QStringListModel    inheritanceModel;
    #endif
};

Qtilities::Testing::DebugWidget::DebugWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    ui->btnRefreshViews->setIcon(QIcon(qti_icon_REFRESH_16x16));
    ui->btnRefreshViews->setText("Refresh Views");
    ui->btnRemoveFilterExpression->setIcon(QIcon(qti_icon_REMOVE_ONE_16x16));
    ui->btnAddFilterExpression->setIcon(QIcon(qti_icon_NEW_16x16));
    ui->btnAddActivePlugin->setIcon(QIcon(qti_icon_NEW_16x16));
    ui->btnRemoveActivePlugin->setIcon(QIcon(qti_icon_REMOVE_ONE_16x16));
    ui->btnAddInactivePlugin->setIcon(QIcon(qti_icon_NEW_16x16));
    ui->btnRemoveInactivePlugin->setIcon(QIcon(qti_icon_REMOVE_ONE_16x16));
    ui->lblPluginInfoIcon->setText("");
    ui->lblPluginInfoMessage->setText("");

    setObjectName("Qtilities Debug Widget");
    setWindowTitle(objectName());
    setAttribute(Qt::WA_QuitOnClose,false);

    d = new DebugWidgetPrivateData;
    connect(&d->plugin_msg_timer,SIGNAL(timeout()), ui->lblPluginInfoIcon, SLOT(hide()));
    connect(&d->plugin_msg_timer,SIGNAL(timeout()), ui->lblPluginInfoMessage, SLOT(clear()));

    d->command_editor = new CommandEditor;
    connect(d->command_editor->commandWidget(),SIGNAL(selectedObjectsChanged(QList<QObject*>)),SLOT(refreshCommandInformation()));

    // Version:
    ui->labelVersion->setText(QtilitiesApplication::qtilitiesVersionString());

    // Global Object Pool:
    d->object_pool_widget = new ObserverWidget(Qtilities::TreeView);
    d->object_pool_widget->setObserverContext(OBJECT_MANAGER->objectPool());

    if (ui->widgetObjectPoolHolder->layout())
        delete ui->widgetObjectPoolHolder->layout();

    QVBoxLayout* object_pool_layout = new QVBoxLayout(ui->widgetObjectPoolHolder);
    object_pool_layout->setMargin(0);
    object_pool_layout->addWidget(d->object_pool_widget);
    d->object_pool_widget->toggleUseObserverHints(false);

    ObserverHints* new_hints = new ObserverHints;
    ObserverHints::ActionHints action_hints = 0;
    action_hints |= ObserverHints::ActionFindItem;
    action_hints |= ObserverHints::ActionRefreshView;
    new_hints->setActionHints(action_hints);
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ActionToolBar;
    display_flags |= ObserverHints::ItemView;
    new_hints->setDisplayFlagsHint(display_flags);
    new_hints->setItemViewColumnHint(ObserverHints::ColumnAllHints);
    new_hints->setHierarchicalDisplayHint(ObserverHints::CategorizedHierarchy);
    d->object_pool_widget->setCustomHints(new_hints);

    d->object_pool_widget->initialize();
    d->object_pool_widget->show();
    d->object_pool_widget->toggleSearchBox();
    connect(d->object_pool_widget,SIGNAL(doubleClickRequest(QObject*)),SLOT(handle_objectPoolDoubleClick(QObject*)));
    connect(d->object_pool_widget,SIGNAL(selectedObjectsChanged(QList<QObject*>)),SLOT(handle_objectPoolSelectionChanged(QList<QObject*>)));

    // Conan Widgets:
    ui->lblConanLabel->setText("Signal and slot analysis is done using the <a href=\"http://sourceforge.net/projects/conanforqt\">Conan</a> library.");
    #ifdef QTILITIES_CONAN
        ui->btnAnalyzeCurrentObject->setEnabled(true);
        ui->btnAnalyzeAction->setEnabled(true);
        ui->lblConanExcluded->setVisible(false);
    #else
        ui->btnAnalyzeCurrentObject->setEnabled(false);
        ui->btnAnalyzeAction->setEnabled(false);
        ui->lblConanExcluded->setText("<font color=\"red\">Conan integration is disabled, see <b>QTILITIES_CONAN</b> for details.</font>");
        ui->lblConanExcluded->setVisible(true);
    #endif

    // Object Property Browser:
    #ifdef QTILITIES_PROPERTY_BROWSER
        // Object Property Browser:
        d->object_property_browser = new ObjectPropertyBrowser;

        if (ui->objectPropertyBrowserHolder->layout())
            delete ui->objectPropertyBrowserHolder->layout();

        QVBoxLayout* property_browser_layout = new QVBoxLayout(ui->objectPropertyBrowserHolder);
        property_browser_layout->setMargin(0);
        property_browser_layout->addWidget(d->object_property_browser);

        // Object Dynamic Property Browser:
        d->object_dynamic_property_browser = new ObjectDynamicPropertyBrowser;
        d->object_dynamic_property_browser->toggleQtilitiesProperties(true);

        if (ui->objectDynamicPropertyBrowserHolder->layout())
            delete ui->objectDynamicPropertyBrowserHolder->layout();

        QVBoxLayout* dynamic_property_browser_layout = new QVBoxLayout(ui->objectDynamicPropertyBrowserHolder);
        dynamic_property_browser_layout->setMargin(0);
        dynamic_property_browser_layout->addWidget(d->object_dynamic_property_browser);
        ui->chkRefreshProperties->setChecked(false);
        ui->groupBoxInheritanceList->setVisible(false);
        ui->groupBoxPropertyBrowser->setVisible(true);
    #else
        // Inheritance List:
        ui->objectInheritanceList->setModel(&d->inheritanceModel);
        ui->groupBoxPropertyBrowser->setVisible(false);
        ui->groupBoxInheritanceList->setVisible(true);
    #endif

    // Object Scope:
    if (ui->widgetObjectScopeHolder->layout())
        delete ui->widgetObjectPoolHolder->layout();

    QVBoxLayout* object_scope_layout = new QVBoxLayout(ui->widgetObjectScopeHolder);
    object_scope_layout->setMargin(0);
    d->object_scope_widget.setNameVisible(false);
    object_scope_layout->addWidget(&d->object_scope_widget);

    // Factories:
    connect(ui->listFactories,SIGNAL(currentTextChanged(QString)),SLOT(handle_factoryListSelectionChanged(QString)));

    // Modes:
    ui->tableModes->setAlternatingRowColors(true);
    ui->tableModes->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Tasks:
    if (ui->widgetTasksSummaryHolder->layout())
        delete ui->widgetTasksSummaryHolder->layout();

    QVBoxLayout* tasks_layout = new QVBoxLayout(ui->widgetTasksSummaryHolder);
    tasks_layout->setMargin(0);
    tasks_layout->addWidget(&d->task_summary_widget);

    d->task_summary_widget.setTaskDisplayOptions(TaskSummaryWidget::DisplayAllTasks);
    d->task_summary_widget.setTaskRemoveOptionFlags(TaskSummaryWidget::RemoveWhenDeleted);
    d->task_summary_widget.setNoActiveTaskHandling(TaskSummaryWidget::ShowSummaryWidget);
    d->task_summary_widget.findCurrentTasks();

    // Contexts:
    if (ui->widgetContextsHolder->layout())
        delete ui->widgetContextsHolder->layout();

    QVBoxLayout* contexts_layout = new QVBoxLayout(ui->widgetContextsHolder);
    contexts_layout->setMargin(0);
    contexts_layout->addWidget(&d->contexts_main_window);

    d->contexts_dock_window.setWindowTitle("Context Management");
    d->contexts_dock_window.setWidget(ui->widgetContextsContents);
    d->contexts_main_window.addDockWidget(Qt::TopDockWidgetArea,&d->contexts_dock_window);

    // Action Management:
    // Command Editor:
    if (ui->commandEditorHolder->layout())
        delete ui->commandEditorHolder->layout();
    QHBoxLayout* command_editor_layout = new QHBoxLayout(ui->commandEditorHolder);
    command_editor_layout->addWidget(d->command_editor);
    command_editor_layout->setMargin(0);
    if (d->command_editor->layout())
        d->command_editor->layout()->setMargin(0);
    d->command_editor->configPageInitialize();

    // Action Container Editor:
    if (ui->actionContainersHolder->layout())
        delete ui->actionContainersHolder->layout();
    QHBoxLayout* action_container_layout = new QHBoxLayout(ui->actionContainersHolder);
    d->action_container_widget = new ObserverWidget(ACTION_MANAGER->actionContainerObserver());
    if (d->action_container_widget->treeView())
        d->action_container_widget->treeView()->setAlternatingRowColors(true);
    action_container_layout->addWidget(d->action_container_widget);
    action_container_layout->setMargin(0);

    // Plugins:
    if (ui->listPluginsFilteredHolder->layout())
        delete ui->listPluginsFilteredHolder->layout();
    d->filtered_plugins_view = new qti_private_DropableListWidget("Filtered");
    connect(d->filtered_plugins_view,SIGNAL(newMessage(Logger::MessageType,QString)),SLOT(handleListMessage(Logger::MessageType,QString)));
    QHBoxLayout* filtered_plugins_layout = new QHBoxLayout(ui->listPluginsFilteredHolder);
    filtered_plugins_layout->addWidget(d->filtered_plugins_view);
    filtered_plugins_layout->setMargin(0);

    if (ui->listPluginsInactiveHolder->layout())
        delete ui->listPluginsInactiveHolder->layout();
    d->inactive_plugins_view = new qti_private_DropableListWidget("Inactive");
    connect(d->inactive_plugins_view,SIGNAL(newMessage(Logger::MessageType,QString)),SLOT(handleListMessage(Logger::MessageType,QString)));
    QHBoxLayout* inactive_plugins_layout = new QHBoxLayout(ui->listPluginsInactiveHolder);
    inactive_plugins_layout->addWidget(d->inactive_plugins_view);
    inactive_plugins_layout->setMargin(0);

    if (ui->listPluginsActiveHolder->layout())
        delete ui->listPluginsActiveHolder->layout();
    d->active_plugins_view = new qti_private_DropableListWidget("Active");
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

Qtilities::Testing::DebugWidget::~DebugWidget() {
    delete d;
    delete ui;
}

QWidget* Qtilities::Testing::DebugWidget::modeWidget() {
    return this;
}

QIcon Qtilities::Testing::DebugWidget::modeIcon() const {
    return QIcon(qti_icon_DEBUG_48x48);
}

QString Qtilities::Testing::DebugWidget::modeName() const {
    return "Debugging";
}

void Qtilities::Testing::DebugWidget::aboutToBeActivated() {
    on_btnRefreshViews_clicked();
}

void Qtilities::Testing::DebugWidget::finalizeMode() {
    if (d->object_pool_widget->treeView())
        d->object_pool_widget->treeView()->expandAll();
    OBJECT_MANAGER->objectPool()->startProcessingCycle();
    if (OBJECT_MANAGER->objectPool()->subjectCount() > 0) {
        QList<QObject*> object_list;
        object_list << OBJECT_MANAGER->objectPool()->subjectAt(0);
        d->object_pool_widget->selectObjects(object_list);
    }
}

void Qtilities::Testing::DebugWidget::handle_factoryListSelectionChanged(const QString& factory_name) {
    ui->listFactoryItemIDs->clear();
    ui->listFactoryItemIDs->addItems(OBJECT_MANAGER->tagsForFactory(factory_name));
}

void Qtilities::Testing::DebugWidget::handle_objectPoolDoubleClick(QObject *object) {
    Q_UNUSED(object)
}

void Qtilities::Testing::DebugWidget::handle_objectPoolSelectionChanged(QList<QObject*> objects) {
    if (objects.count() == 1) {
        QObject* obj = objects.front();
        if (!obj)
            return;

        d->current_object = obj;
        d->object_scope_widget.setObject(obj);

        #ifdef QTILITIES_PROPERTY_BROWSER
        if (ui->chkRefreshProperties->isChecked()) {
            d->object_property_browser->setObject(obj);
            d->object_dynamic_property_browser->setObject(obj);
        }
        #else
        // Inheritance Data:
        QStringList inheritance_list;
        const QMetaObject* metaObject = obj->metaObject();
        while (metaObject) {
            inheritance_list.append(metaObject->className());
            metaObject = metaObject->superClass();
        }
        d->inheritanceModel.setStringList(inheritance_list);
        #endif
    }
}

void Qtilities::Testing::DebugWidget::changeEvent(QEvent *e)
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

void Qtilities::Testing::DebugWidget::on_btnRefreshViews_clicked() {
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
    refreshCommandInformation();

    // ===============================
    // Refresh Factories:
    // ===============================
    refreshFactories();

    // ===============================
    // Refresh Projects:
    // ===============================
    refreshProjectsState();

    // ===============================
    // Refresh Global Object Pool:
    // ===============================
    d->object_pool_widget->refresh();
    d->object_pool_widget->viewExpandAll();
}

void Qtilities::Testing::DebugWidget::on_btnExplorePluginConfigSetPath_clicked() {
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QFileInfo fi(ui->txtPluginsActiveSet->text());
    if (!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(fi.path())))){
        LOG_ERROR_P(QString("Failed to open folder: %1").arg(fi.path()));
    }
    else
        LOG_INFO_P(QString("Successfully opened folder: %1").arg(fi.path()));
}

bool Qtilities::Testing::DebugWidget::on_btnSavePluginConfigSet_clicked()
{
    QStringList inactive_list = d->inactive_plugins_model.stringList();
    QStringList filter_list = d->filtered_plugins_model.stringList();
    return EXTENSION_SYSTEM->savePluginConfiguration(ui->txtPluginsActiveSet->text(),&inactive_list,&filter_list);
}

bool Qtilities::Testing::DebugWidget::on_btnSaveNewPluginConfigSet_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(0, "Save Plugin Configuration",QString("%1/new_plugin_configuration").arg(QtilitiesApplication::applicationSessionPath()),QString("Plugin Configurations (*%1)").arg(qti_def_SUFFIX_PLUGIN_CONFIG));
    if (!fileName.isEmpty()) {
        QStringList inactive_list = d->inactive_plugins_model.stringList();
        QStringList filter_list = d->filtered_plugins_model.stringList();
        return EXTENSION_SYSTEM->savePluginConfiguration(fileName,&inactive_list,&filter_list);
        ui->txtPluginsEditSet->setText(fileName);
        return true;
    } else
        return false;
}

void Qtilities::Testing::DebugWidget::on_btnEditPluginConfigSet_clicked()
{
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QFileInfo fi(ui->txtPluginsActiveSet->text());
    if (!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(fi.filePath())))){
        LOG_ERROR_P(QString("Failed to open folder: %1").arg(fi.filePath()));
    }
    else
        LOG_INFO_P(QString("Successfully opened folder: %1").arg(fi.filePath()));
}

void Qtilities::Testing::DebugWidget::on_btnExplorePluginCurrentConfigSetPath_clicked()
{
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QFileInfo fi(ui->txtPluginsEditSet->text());
    if (!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(fi.path())))){
        LOG_ERROR_P(QString("Failed to open folder: %1").arg(fi.path()));
    }
    else
        LOG_INFO_P(QString("Successfully opened folder: %1").arg(fi.path()));
}

void Qtilities::Testing::DebugWidget::on_btnEditPluginCurrentConfigSet_clicked()
{
    if (ui->txtPluginsActiveSet->text().isEmpty())
        return;

    QFileInfo fi(ui->txtPluginsEditSet->text());
    if (!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(fi.filePath())))){
        LOG_ERROR_P(QString("Failed to open file: %1").arg(fi.filePath()));
    }
    else
        LOG_INFO_P(QString("Successfully opened file: %1").arg(fi.filePath()));
}


void Qtilities::Testing::DebugWidget::on_btnOpenPluginConfigSet_clicked()
{
    QString open_path = QtilitiesApplication::applicationSessionPath();
    if (!ui->txtPluginsActiveSet->text().isEmpty()) {
        QFileInfo fi(ui->txtPluginsActiveSet->text());
        open_path = fi.path();
    }
    QString fileName = QFileDialog::getOpenFileName(0, "Open Plugin Configuration",open_path,QString("Plugin Configurations (*%1)").arg(qti_def_SUFFIX_PLUGIN_CONFIG));
    if (!fileName.isEmpty()) {
        QStringList inactive_list;
        QStringList filter_list;
        EXTENSION_SYSTEM->loadPluginConfiguration(fileName,&inactive_list,&filter_list);
        ui->txtPluginsEditSet->setText(fileName);
        d->inactive_plugins_model.setStringList(inactive_list);
        d->filtered_plugins_model.setStringList(filter_list);
    }
}

void Qtilities::Testing::DebugWidget::on_btnRemoveActivePlugin_clicked()
{
    d->active_plugins_model.removeRow(d->active_plugins_view->currentIndex().row());
}

void Qtilities::Testing::DebugWidget::on_btnAddActivePlugin_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Add active plugin","Plugin name:", QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()) {
        QStringList list = d->active_plugins_model.stringList();
        list << text;
        list.removeDuplicates();
        d->active_plugins_model.setStringList(list);
    }
}

void Qtilities::Testing::DebugWidget::on_btnRemoveInactivePlugin_clicked()
{
    d->inactive_plugins_model.removeRow(d->inactive_plugins_view->currentIndex().row());
}

void Qtilities::Testing::DebugWidget::on_btnAddInactivePlugin_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Add inactive plugin","Plugin name:", QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()) {
        QStringList list = d->inactive_plugins_model.stringList();
        list << text;
        list.removeDuplicates();
        d->inactive_plugins_model.setStringList(list);
    }
}

void Qtilities::Testing::DebugWidget::on_btnRemoveFilterExpression_clicked()
{
    d->filtered_plugins_model.removeRow(d->filtered_plugins_view->currentIndex().row());
}

void Qtilities::Testing::DebugWidget::on_btnAddFilterExpression_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Add filter expression","Filter expression:", QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()) {
        QRegExp reg_exp(text);
        reg_exp.setPatternSyntax(QRegExp::Wildcard);
        while (!reg_exp.isValid()) {
            QString msg_string = QString("You entered an invalid regular expression:\n\nError: %1\n\nNew expression:").arg(reg_exp.errorString());
            QString text = QInputDialog::getText(this, "Expression Is Not Valid",msg_string, QLineEdit::Normal,reg_exp.pattern(), &ok);
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

void Qtilities::Testing::DebugWidget::handleListMessage(Logger::MessageType type, const QString& msg) {
    if (type == Logger::Warning) {
        ui->lblPluginInfoIcon->setPixmap(QIcon(qti_icon_WARNING_16x16).pixmap(12));
        ui->lblPluginInfoIcon->setVisible(true);
        ui->lblPluginInfoMessage->setText(QString("<font color='red'>%1</font>").arg(msg));
    } else if (type == Logger::Error || type == Logger::Fatal) {
        ui->lblPluginInfoIcon->setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(12));
        ui->lblPluginInfoIcon->setVisible(true);
        ui->lblPluginInfoMessage->setText(QString("<font color='red'>%1</font>").arg(msg));
    } else {
        if (msg.startsWith("Successfully"))
            ui->lblPluginInfoIcon->setPixmap(QIcon(qti_icon_SUCCESS_16x16).pixmap(12));
        else
            ui->lblPluginInfoIcon->setPixmap(QIcon(qti_icon_INFO_16x16).pixmap(12));
        ui->lblPluginInfoIcon->setVisible(true);
        ui->lblPluginInfoMessage->setText(msg);
    }

    d->plugin_msg_timer.start(30000);
}

void Qtilities::Testing::DebugWidget::on_btnContextSetActive_clicked()
{
    QList<QTableWidgetItem*> selected_items = ui->tableContextsAll->selectedItems();
    CONTEXT_MANAGER->setNewContext(Qtilities::Core::Constants::qti_def_CONTEXT_STANDARD);
    for (int i = 0; i < selected_items.count(); ++i) {
        CONTEXT_MANAGER->appendContext(selected_items.at(i)->text());
    }
    refreshContexts();
    refreshCommandInformation();
}

void Qtilities::Testing::DebugWidget::on_btnContextsClear_clicked()
{
    CONTEXT_MANAGER->setNewContext(Qtilities::Core::Constants::qti_def_CONTEXT_STANDARD);
    refreshContexts();
    refreshCommandInformation();
}

void Qtilities::Testing::DebugWidget::on_btnContextsBroadcast_clicked()
{
    CONTEXT_MANAGER->broadcastState();
    refreshContexts();
}

void Qtilities::Testing::DebugWidget::refreshModes() {
    ui->tableModes->clear();
    QStringList mode_headers;
    mode_headers << "Mode Name" << "Mode ID" << "Mode Shortcut" << "Context String" << "Help ID";
    ui->tableModes->setHorizontalHeaderLabels(mode_headers);
    ui->tableModes->setSortingEnabled(false);

    QtilitiesMainWindow* mainWindow = qobject_cast<QtilitiesMainWindow*> (QtilitiesApplication::mainWindow());
    if (mainWindow) {
        if (mainWindow->modeManager()) {
            ui->tableModes->setRowCount(mainWindow->modeManager()->modes().count());
            for (int i = 0; i < mainWindow->modeManager()->modes().count(); ++i) {
                IMode* mode = mainWindow->modeManager()->modes().at(i);
                if (!mode)
                    continue;

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
    }

    ui->tableModes->resizeColumnsToContents();
    ui->tableModes->horizontalHeader()->setStretchLastSection(true);
    ui->tableModes->setSortingEnabled(true);
    ui->tableModes->sortByColumn(0,Qt::AscendingOrder);
    ui->tableModes->setShowGrid(false);
    ui->tableModes->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Qtilities::Testing::DebugWidget::refreshContexts() {
    ui->tableContextsAll->clear();
    ui->tableContextsActive->clear();
    QStringList context_headers;
    context_headers << "Context ID" << "Context Name" << "Help ID";
    ui->tableContextsAll->setHorizontalHeaderLabels(context_headers);
    ui->tableContextsAll->setSortingEnabled(false);
    ui->tableContextsActive->setHorizontalHeaderLabels(context_headers);
    ui->tableContextsActive->setSortingEnabled(false);

    // First do all contexts:
    QList<int> all_contexts = CONTEXT_MANAGER->allContexts();
    ui->tableContextsAll->setRowCount(all_contexts.count());
    ui->tableContextsAll->setColumnCount(3);
    for (int i = 0; i < all_contexts.count(); ++i) {
        int current_id = all_contexts.at(i);
        // Context ID:
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(current_id));
        ui->tableContextsAll->setItem(i, 0, newItem);
        // Context Name:
        newItem = new QTableWidgetItem(CONTEXT_MANAGER->contextString(current_id));
        ui->tableContextsAll->setItem(i, 1, newItem);
        // Context Help ID:
        newItem = new QTableWidgetItem(CONTEXT_MANAGER->contextHelpID(current_id));
        ui->tableContextsAll->setItem(i, 2, newItem);
        ui->tableContextsAll->setRowHeight(i,17);
    }

    // Next do active contexts:
    QList<int> active_contexts = CONTEXT_MANAGER->activeContexts();
    ui->tableContextsActive->setRowCount(active_contexts.count());
    ui->tableContextsActive->setColumnCount(3);
    for (int i = 0; i < active_contexts.count(); ++i) {
        int current_id = active_contexts.at(i);
        // Context ID:
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(current_id));
        ui->tableContextsActive->setItem(i, 0, newItem);
        // Context Name:
        newItem = new QTableWidgetItem(CONTEXT_MANAGER->contextString(current_id));
        ui->tableContextsActive->setItem(i, 1, newItem);
        // Context Help ID:
        newItem = new QTableWidgetItem(CONTEXT_MANAGER->contextHelpID(current_id));
        ui->tableContextsAll->setItem(i, 2, newItem);
        ui->tableContextsActive->setRowHeight(i,17);
    }

    ui->tableContextsAll->horizontalHeader()->setStretchLastSection(true);
    ui->tableContextsAll->sortByColumn(0,Qt::AscendingOrder);
    ui->tableContextsAll->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableContextsAll->setShowGrid(false);
    ui->tableContextsAll->setAlternatingRowColors(true);
    ui->tableContextsAll->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableContextsAll->setSortingEnabled(true);
    ui->tableContextsAll->sortItems(1);
    ui->tableContextsActive->horizontalHeader()->setStretchLastSection(true);
    ui->tableContextsActive->sortByColumn(0,Qt::AscendingOrder);
    ui->tableContextsActive->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableContextsActive->setShowGrid(false);
    ui->tableContextsActive->setAlternatingRowColors(true);
    ui->tableContextsActive->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableContextsActive->setSortingEnabled(true);
    ui->tableContextsActive->sortItems(1);
}

void Qtilities::Testing::DebugWidget::refreshCommandInformation() {
    QList<QObject*> commands = d->command_editor->commandWidget()->selectedObjects();
    if (commands.isEmpty()) {
        ui->tableSelectedActionOverview->clear();
        ui->lblCommandType->setText("No command selected.");
        ui->tableSelectedActionOverview->setEnabled(false);
        return;
    }

    // Get the front command:
    d->current_command = qobject_cast<Command*> (commands.front());

    if (!d->current_command) {
        ui->tableSelectedActionOverview->clear();
        ui->lblCommandType->setText("No command selected.");
        ui->tableSelectedActionOverview->setEnabled(false);
        return;
    }

    // Check what type of command it is:
    ProxyAction* multi_action = qobject_cast<ProxyAction*> (d->current_command);
    if (multi_action) {
        ui->lblCommandType->setText("Command Type: Proxy Action");

        // ===============================
        // Refresh Table:
        // ===============================
        ui->tableSelectedActionOverview->clear();
        QStringList context_headers;
        ui->tableSelectedActionOverview->setColumnCount(10);
        context_headers << "Backend Action Text" << "Backend Action Address" << "Parent" << "Parent Address" << "Is Active" << "Target Contexts" << "Shortcut" << "Default Shortcut" << "Icon" << "Tooltip";
        ui->tableSelectedActionOverview->setHorizontalHeaderLabels(context_headers);
        ui->tableSelectedActionOverview->setSortingEnabled(false);

        QHash<int, QPointer<QAction> > id_action_map = multi_action->contextIDActionMap();
        ui->tableSelectedActionOverview->setRowCount(id_action_map.count());
        for (int i = 0; i < id_action_map.count(); ++i) {
            int current_id = id_action_map.keys().at(i);
            QPointer<QAction> current_command = id_action_map[id_action_map.keys().at(i)];
            if (!current_command)
                continue;
            // Backend Action Name:
            QTableWidgetItem *newItem = new QTableWidgetItem(current_command->text());
            ui->tableSelectedActionOverview->setItem(i, 0, newItem);
            // Backend Action Address:
            QAction* tmp_action = current_command;
            newItem = new QTableWidgetItem(objectAddress(tmp_action));
            ui->tableSelectedActionOverview->setItem(i, 1, newItem);
            // Parent Name:
            QString parent_name = "No Parent";
            if (current_command->parent())
                parent_name = current_command->parent()->objectName();
            newItem = new QTableWidgetItem(parent_name);
            ui->tableSelectedActionOverview->setItem(i, 2, newItem);
            // Parent Address:
            QString parent_address = "No Parent";
            if (current_command->parent())
                parent_address = objectAddress(current_command->parent());
            newItem = new QTableWidgetItem(parent_address);
            ui->tableSelectedActionOverview->setItem(i, 3, newItem);
            // Is Active:
            QString is_active_text = "No";
            if (current_command == multi_action->activeBackendAction())
                is_active_text = "Yes";
            newItem = new QTableWidgetItem(is_active_text);
            ui->tableSelectedActionOverview->setItem(i, 4, newItem);
            // Target Context:
            newItem = new QTableWidgetItem(QString::number(current_id));
            ui->tableSelectedActionOverview->setItem(i, 5, newItem);
            // Shortcut:
            newItem = new QTableWidgetItem(current_command->shortcut().toString());
            ui->tableSelectedActionOverview->setItem(i, 6, newItem);
            // Default Shortcut:
            newItem = new QTableWidgetItem(d->current_command->defaultKeySequence().toString());
            ui->tableSelectedActionOverview->setItem(i, 7, newItem);
            // Icon:
            newItem = new QTableWidgetItem(current_command->icon(),"");
            ui->tableSelectedActionOverview->setItem(i, 8, newItem);
            // Tooltip:
            newItem = new QTableWidgetItem(current_command->toolTip());
            ui->tableSelectedActionOverview->setItem(i, 9, newItem);
            ui->tableSelectedActionOverview->setRowHeight(i,17);
        }
    }

    ShortcutCommand* shortcut = qobject_cast<ShortcutCommand*> (d->current_command);
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
    #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    table_header->setResizeMode(0,QHeaderView::Stretch);
    #else
    table_header->setSectionResizeMode(0,QHeaderView::Stretch);
    #endif
    ui->tableSelectedActionOverview->sortByColumn(0,Qt::AscendingOrder);
    ui->tableSelectedActionOverview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableSelectedActionOverview->setShowGrid(false);
    ui->tableSelectedActionOverview->setAlternatingRowColors(true);
    ui->tableSelectedActionOverview->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableSelectedActionOverview->setSortingEnabled(true);
    ui->tableSelectedActionOverview->setEnabled(true);
}

void Qtilities::Testing::DebugWidget::refreshCurrentPluginState() {
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

void Qtilities::Testing::DebugWidget::refreshCurrentPluginSet() {
    if (EXTENSION_SYSTEM->activePluginConfigurationFile().isEmpty())
        ui->txtPluginsActiveSet->setText("No plugin configuration set loaded");
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

void Qtilities::Testing::DebugWidget::refreshEditedPluginState() {
    if (d->plugin_edit_set_loaded && !d->active_plugins_model.stringList().isEmpty() && !d->inactive_plugins_model.stringList().isEmpty() && !d->filtered_plugins_model.stringList().isEmpty())
        return;

    if (EXTENSION_SYSTEM->activePluginConfigurationFile().isEmpty())
        ui->txtPluginsEditSet->setText("No plugin configuration set loaded");
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

void Qtilities::Testing::DebugWidget::refreshFactories() {
    ui->listFactoryItemIDs->clear();
    ui->listFactories->clear();

    ui->listFactories->addItems(OBJECT_MANAGER->allFactoryNames());
    if (ui->listFactories->count() > 0)
        ui->listFactories->setCurrentRow(0);
}

void Qtilities::Testing::DebugWidget::on_btnAnalyzeCurrentObject_clicked()
{
    #ifdef QTILITIES_CONAN
    if (d->current_object) {
        if (!d->object_analysis_widget) {
            d->object_analysis_widget = new ConanWidget();
            //d->object_analysis_widget->setAttribute(Qt::WA_DeleteOnClose,true);
            d->object_analysis_widget->setAttribute(Qt::WA_QuitOnClose,false);
        }

        d->object_analysis_widget->AddRootObject(d->current_object);
        d->object_analysis_widget->show();
    }
    #endif
}

void Qtilities::Testing::DebugWidget::on_btnAnalyzeAction_clicked()
{
    #ifdef QTILITIES_CONAN
    if (d->current_command) {
        if (!d->command_analysis_widget) {
            d->command_analysis_widget = new ConanWidget;
            //d->command_analysis_widget->setAttribute(Qt::WA_DeleteOnClose,true);
            d->command_analysis_widget->setAttribute(Qt::WA_QuitOnClose,false);
        }

        d->command_analysis_widget->AddRootObject(d->current_command);
        d->command_analysis_widget->show();
    }
    #endif
}

void Qtilities::Testing::DebugWidget::on_chkRefreshProperties_toggled(bool checked)
{
    Q_UNUSED(checked)

    #ifdef QTILITIES_PROPERTY_BROWSER
    if (ui->chkRefreshProperties->isChecked()) {
        d->object_property_browser->setObject(d->current_object);
        d->object_dynamic_property_browser->setObject(d->current_object);
    }
    #endif
}

QString Qtilities::Testing::DebugWidget::objectAddress(QObject* obj) const {
    return QString ("0x%1").arg (reinterpret_cast <quintptr> (obj), sizeof(quintptr)*2, 16, QChar('0'));
}

void Qtilities::Testing::DebugWidget::on_btnProjectsCurrentOpenPath_clicked()
{
    if (ui->txtProjectsCurrentFileName->text().isEmpty())
        return;

    QFileInfo fi(ui->txtProjectsCurrentFileName->text());
    if (!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(fi.path())))){
        LOG_ERROR_P(QString("Failed to open folder: %1").arg(fi.path()));
    }
    else
        LOG_INFO_P(QString("Successfully opened folder: %1").arg(fi.path()));
}

void Qtilities::Testing::DebugWidget::on_btnProjectsCurrentOpenFile_clicked() {
    if (ui->txtProjectsCurrentFileName->text().isEmpty())
        return;

    QFileInfo fi(ui->txtProjectsCurrentFileName->text());
    if (!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(fi.filePath())))){
        LOG_ERROR_P(QString("Failed to open file: %1").arg(fi.filePath()));
    }
    else
        LOG_INFO_P(QString("Successfully opened file: %1").arg(fi.filePath()));
}

void Qtilities::Testing::DebugWidget::refreshProjectsState() {
    if (PROJECT_MANAGER->currentProjectFileName().isEmpty()) {
        ui->txtProjectsCurrentFileName->clear();
        ui->txtProjectsCurrentFileName->setEnabled(false);
        ui->btnProjectsCurrentOpenFile->setEnabled(false);
        ui->btnProjectsCurrentOpenPath->setEnabled(false);
    } else {
        ui->txtProjectsCurrentFileName->setText(PROJECT_MANAGER->currentProjectFileName());
        ui->txtProjectsCurrentFileName->setEnabled(true);
        ui->btnProjectsCurrentOpenFile->setEnabled(true);
        ui->btnProjectsCurrentOpenPath->setEnabled(true);
    }

    ui->listProjectsCurrentItems->clear();
    ui->listProjectsRecent->clear();
    ui->listProjectsRegisteredItems->clear();

    if (PROJECT_MANAGER->currentProject()) {
        ui->listProjectsCurrentItems->addItems(PROJECT_MANAGER->currentProject()->projectItemNames());
    }

    ui->listProjectsRecent->addItems(PROJECT_MANAGER->recentProjectPaths());
    ui->listProjectsRegisteredItems->addItems(PROJECT_MANAGER->registeredProjectItemNames());

}

void Qtilities::Testing::DebugWidget::on_btnContextUnregisterSelected_clicked() {
    QList<QTableWidgetItem*> selected_items = ui->tableContextsActive->selectedItems();
    for (int i = 0; i < selected_items.count(); ++i) {
        CONTEXT_MANAGER->unregisterContext(selected_items.at(i)->text());
    }
    refreshContexts();
    refreshCommandInformation();
}
