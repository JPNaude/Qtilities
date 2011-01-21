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

//#include <Conan.h>

#include <QtilitiesExtensionSystem>
using namespace QtilitiesExtensionSystem;

struct Qtilities::Plugins::Debug::DebugWidgetData {
    DebugWidgetData() : objectPoolWidget(0)/*,
                        conanWidget(0)*/ {}

    ObserverWidget* objectPoolWidget;
    //ConanWidget*    conanWidget;
};

Qtilities::Plugins::Debug::DebugWidget::DebugWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWidget)
{
    ui->setupUi(this);
    d = new DebugWidgetData;
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
    action_hints |= ObserverHints::ActionRefreshView;
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

    // Conan Widget:
    //d->conanWidget = new ConanWidget();

    // Splitter:
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(d->objectPoolWidget);
    //splitter->addWidget(d->conanWidget);

    // Layout:
    QVBoxLayout* layout = new QVBoxLayout(ui->widgetObjectPoolHolder);
    layout->setMargin(0);
    layout->addWidget(splitter);

    // Now that the widgets are in a layout we show & init them:
    d->objectPoolWidget->initialize();
    d->objectPoolWidget->show();
    d->objectPoolWidget->toggleSearchBox();
    connect(d->objectPoolWidget,SIGNAL(doubleClickRequest(QObject*)),SLOT(handle_objectPoolDoubleClick(QObject*)));
    //d->conanWidget->show();

    // Factories:
    connect(ui->btnRefreshFactories,SIGNAL(clicked()),SLOT(handle_factoryListRefresh()));
    connect(ui->listFactories,SIGNAL(currentTextChanged(QString)),SLOT(handle_factoryListSelectionChanged(QString)));
    ui->btnRefreshFactories->setIcon(QIcon(ICON_REFRESH_16x16));
    handle_factoryListRefresh();

    // Plugins:
    connect(ui->btnRefreshPluginInfo,SIGNAL(clicked()),SLOT(handle_pluginInfoRefresh()));
    ui->btnRefreshPluginInfo->setIcon(QIcon(ICON_REFRESH_16x16));
    handle_pluginInfoRefresh();

    // Modes:
    connect(ui->btnRefreshModes,SIGNAL(clicked()),SLOT(handle_modeInfoRefresh()));
    ui->btnRefreshModes->setIcon(QIcon(ICON_REFRESH_16x16));
    handle_modeInfoRefresh();
    ui->tableModes->setAlternatingRowColors(true);
    ui->tableModes->setSelectionBehavior(QAbstractItemView::SelectRows);
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

void Qtilities::Plugins::Debug::DebugWidget::handle_factoryListRefresh() {
    ui->listFactoryItemIDs->clear();
    ui->listFactories->clear();

    ui->listFactories->addItems(OBJECT_MANAGER->allFactoryNames());
    if (ui->listFactories->count() > 0)
        ui->listFactories->setCurrentRow(0);
}

void Qtilities::Plugins::Debug::DebugWidget::handle_objectPoolDoubleClick(QObject *object) {
    //d->conanWidget->AddRootObject(object);
}

void Qtilities::Plugins::Debug::DebugWidget::handle_pluginInfoRefresh() {
    if (EXTENSION_SYSTEM->activePluginConfigurationFile().isEmpty())
        ui->txtPluginsActiveSet->setText(tr("No plugin configuration set loaded"));
    else
        ui->txtPluginsActiveSet->setText(EXTENSION_SYSTEM->activePluginConfigurationFile());

    ui->listPluginsActive->clear();
    ui->listPluginsInactive->clear();
    ui->listPluginsFiltered->clear();

    ui->listPluginsActive->addItems(EXTENSION_SYSTEM->activePlugins());
    ui->listPluginsInactive->addItems(EXTENSION_SYSTEM->inactivePlugins());
    ui->listPluginsFiltered->addItems(EXTENSION_SYSTEM->filteredPlugins());
}

void Qtilities::Plugins::Debug::DebugWidget::handle_modeInfoRefresh() {
    ui->tableModes->clear();
    QStringList headers;
    headers << "Mode Name" << "Mode ID" << "Mode Shortcut" << "Context String" << "Help ID";
    ui->tableModes->setHorizontalHeaderLabels(headers);
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
