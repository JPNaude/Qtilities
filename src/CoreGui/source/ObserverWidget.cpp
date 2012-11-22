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

#include "ObserverWidget.h"
#include "ui_ObserverWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ObserverTableModel.h"
#include "ObserverTreeModel.h"
#include "NamingPolicyFilter.h"
#include "QtilitiesApplication.h"
#include "ObjectScopeWidget.h"
#include "ObjectHierarchyNavigator.h"
#include "ObjectPropertyBrowser.h"
#include "QtilitiesPropertyChangeEvent.h"
#include "SearchBoxWidget.h"
#include "ObserverTableModelProxyFilter.h"
#include "ObserverTreeModelProxyFilter.h"
#include "ActionProvider.h"
#include "ObserverTreeItem.h"
#include "QtilitiesMainWindow.h"
#include "SingleTaskWidget.h"
#include "TaskManagerGui.h"

#include <ActivityPolicyFilter>
#include <ObserverHints>
#include <ObserverMimeData>
#include <QtilitiesCoreConstants>
#include <Logger>

#include <QAction>
#include <QMessageBox>
#include <QStack>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMessageBox>
#include <QDragMoveEvent>
#include <QTreeView>
#include <QTableView>
#include <QDockWidget>
#include <QLabel>
#include <QInputDialog>
#include <QClipboard>
#include <QSortFilterProxyModel>
#include <QSettings>
#include <QToolBar>
#include <QDrag>

#include <stdio.h>
#include <time.h>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core::Properties;

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QWidget, ObserverWidget> ObserverWidget::factory;
    }
}

struct Qtilities::CoreGui::ObserverWidgetData {
    ObserverWidgetData() : actionRemoveItem(0),
        actionRemoveAll(0),
        actionDeleteItem(0),
        actionDeleteAll(0),
        actionNewItem(0),
        actionRefreshView(0),
        actionPushUp(0),
        actionPushUpNew(0),
        actionPushDown(0),
        actionPushDownNew(0),
        actionSwitchView(0),
        actionExpandAll(0),
        actionCollapseAll(0),
        actionFindItem(0),
        navigation_bar(0),
        display_mode(Qtilities::TreeView),
        table_view(0),
        table_model(0),
        custom_table_proxy_model(0),
        tree_view(0),
        tree_model(0),
        custom_tree_proxy_model(0),
        tree_proxy_model(0),
        table_proxy_model(0),
        table_name_column_delegate(0),
        tree_name_column_delegate(0),
        activity_filter(0),
        top_level_observer(0),
        initialized(false),
        read_only(false),
        update_selection_activity(true),
        is_expand_collapse_visible(true),
        hints_selection_parent(0),
        use_observer_hints(true),
        update_global_active_objects(false),
        append_selected_contexts(false),
        action_provider(0),
        default_row_height(17),
        confirm_deletes(true),
        searchBoxWidget(0),
        actionFilterNodes(0),
        actionFilterItems(0),
        actionFilterCategories(0),
        actionFilterTypeSeperator(0),
        last_display_flags(ObserverHints::NoDisplayFlagsHint),
        do_column_resizing(true),
        task_widget(0),
        button_move(Qt::RightButton),
        button_copy(Qt::LeftButton),
        disable_proxy_models(false),
        lazy_init(false),
        search_item_filter_flags(ObserverTreeItem::TreeItem){ }

    QAction* actionRemoveItem;
    QAction* actionRemoveAll;
    QAction* actionDeleteItem;
    QAction* actionDeleteAll;
    QAction* actionNewItem;
    QAction* actionRefreshView;
    QAction* actionPushUp;
    QAction* actionPushUpNew;
    QAction* actionPushDown;
    QAction* actionPushDownNew;
    QAction* actionSwitchView;
    QAction* actionExpandAll;
    QAction* actionCollapseAll;
    QAction* actionFindItem;
    #ifndef QT_NO_DEBUG
    QAction* actionDebugObject;
    #endif

    //! The navigation stack of this widget, used only in TableView mode.
    QStack<int> navigation_stack;
    //! The navigation bar.
    ObjectHierarchyNavigator* navigation_bar;
    //! The action toolbars list. Contains toolbars created for each category in the action provider.
    /*!
      We use QObjects instead of QToolBars since we can then do a direct contains() call in the event filter on this list.
      */
    QList<QObject*> action_toolbars;
    //! The start position point used during drag & drop operations.
    QPoint startPos;

    Qtilities::DisplayMode display_mode;
    QPointer<QTableView> table_view;
    ObserverTableModel* table_model;
    QAbstractProxyModel* custom_table_proxy_model;
    QPointer<QTreeView> tree_view;
    ObserverTreeModel* tree_model;
    QAbstractProxyModel* custom_tree_proxy_model;
    QAbstractProxyModel* tree_proxy_model;
    QAbstractProxyModel* table_proxy_model;
    NamingPolicyDelegate* table_name_column_delegate;
    NamingPolicyDelegate* tree_name_column_delegate;
    ActivityPolicyFilter* activity_filter;

    //! Used to identify the top level observer. d_observer is current selection parent observer.
    QPointer<Observer> top_level_observer;

    #ifdef QTILITIES_PROPERTY_BROWSER
    QPointer<QDockWidget> property_browser_dock;
    QPointer<ObjectPropertyBrowser> property_browser_widget;
    Qt::DockWidgetArea property_editor_dock_area;
    ObjectPropertyBrowser::BrowserType property_editor_type;
    QStringList property_filter;
    bool property_filter_inversed;

    QPointer<QDockWidget> dynamic_property_browser_dock;
    QPointer<ObjectDynamicPropertyBrowser> dynamic_property_browser_widget;
    Qt::DockWidgetArea dynamic_property_editor_dock_area;
    ObjectDynamicPropertyBrowser::BrowserType dynamic_property_editor_type;
//    QStringList dynamic_property_filter;
//    bool dynamic_property_filter_inversed;
    #endif

    //! Indicates if the widget is in an initialized state. Thus initialization was successful. \sa initialize()
    bool initialized;
    //! Indicates if the widget is read only
    bool read_only;
    //! Used to disable selection activiy updates in FollowSelection cases. This is only used internally to avoid continuous loops.
    bool update_selection_activity;
    //! Indicates if the expand/collapse all actions should be available when in TreeView mode.
    bool is_expand_collapse_visible;

    //! Used to store default observer hints to be used with this widget.
    QPointer<ObserverHints> hints_default;
    //! Used to store observer hints for the current selection parent observer.
    QPointer<ObserverHints> hints_selection_parent;
    //! Indicates if the default observer hints, or the current selection parent observer hints are used.
    bool use_observer_hints;

    //! Indicates if this widget updates global active objects.
    bool update_global_active_objects;
    //! The global meta type of this widget.
    QString global_meta_type;
    //! The shared global meta type of this widget. See setSharedGlobalMetaType().
    QString shared_global_meta_type;
    //! A list of appended contexts which have been appended when the selection changes in this widget.
    QStringList appended_contexts;
    //! Indicates if appending of contexts is enabled.
    bool append_selected_contexts;

    //! The current selection in this widget. Set in the selectedObjects() function.
    QList<QPointer<QObject> > current_selection;
    //! The current selection in this widget in terms of ObserverTreeItems. Set in the selectedObjects() function.
    QList<QPointer<ObserverTreeItem> > current_tree_item_selection;
    //! The IActionProvider interface implementation.
    ActionProvider* action_provider;
    //! The default row height used in TableView mode.
    int default_row_height;
    //! Indicates if the user should confirm delete operations.
    bool confirm_deletes;

    // Search related stuff:
    //! The search box widget.
    SearchBoxWidget* searchBoxWidget;
    QAction* actionFilterNodes;
    QAction* actionFilterItems;
    QAction* actionFilterCategories;
    QAction* actionFilterTypeSeperator;

    //! This hint keeps track of the previously used activeHints()->displayFlagsHint(). If it changed, the toolbars will be reconstructed in the refreshActionToolBar() function.
    ObserverHints::DisplayFlags last_display_flags;

    //! Stores if automatic column resizing must be done. See enableAutoColumnResizing()
    bool do_column_resizing;

    //! The task ID of the tree model rebuilding task.
    SingleTaskWidget* task_widget;
    //! Remembers if the search box widget was visible when starting a refresh operation.
    bool search_box_visible_before_refresh;

    //! The mouse button to react to when doing drag and drop moves.
    Qt::MouseButton button_move;
    //! The mouse button to react to when doing drag and drop copies.
    Qt::MouseButton button_copy;

    QStringList last_expanded_items_result;

    bool disable_proxy_models;

    //! Stores if lazy initialization has been enabled on this widget.
    bool lazy_init;

    ObserverTreeItem::TreeItemTypeFlags search_item_filter_flags;
};

Qtilities::CoreGui::ObserverWidget::ObserverWidget(DisplayMode display_mode, QWidget * parent, Qt::WindowFlags f) :
    QMainWindow(parent, f),
    ui(new Ui::ObserverWidget)
{
    ui->setupUi(this);
    d = new ObserverWidgetData;
    d->action_provider = new ActionProvider(this);

    hideProgressInfo();

    // **** IMPORTANT *****
    // Changes here should also happen in the other constructor!

    #ifdef QTILITIES_PROPERTY_BROWSER
    d->property_editor_dock_area = Qt::RightDockWidgetArea;
    d->property_editor_type = ObjectPropertyBrowser::TreeBrowser;
    d->property_filter_inversed = false;
    d->property_filter = QStringList();
    d->dynamic_property_editor_dock_area = Qt::RightDockWidgetArea;
    d->dynamic_property_editor_type = ObjectDynamicPropertyBrowser::TreeBrowser;
//    d->dynamic_property_filter_inversed = false;
//    d->dynamic_property_filter = QStringList();
    #endif

    ui->widgetProgressInfo->setVisible(false);

    d->display_mode = display_mode;
    d->hints_default = new ObserverHints(this);

    setWindowIcon(QIcon(qti_icon_QTILITIES_SYMBOL_WHITE_16x16));
    ui->widgetSearchBox->hide();
    ui->navigationBarWidget->setVisible(false);

    // Assign a default meta type for this widget:
    // We construct each action and then register it
    QString context_string = "ObserverWidget";
    int count = 0;
    context_string.append(QString("%1").arg(count));
    while (CONTEXT_MANAGER->hasContext(context_string)) {
        QString count_string = QString("%1").arg(count);
        context_string.chop(count_string.length());
        ++count;
        context_string.append(QString("%1").arg(count));
    }
    CONTEXT_MANAGER->registerContext(context_string);
    d->global_meta_type = context_string;
    d->shared_global_meta_type = QString();
    setObjectName(context_string);

    setDockNestingEnabled(true);
}

Qtilities::CoreGui::ObserverWidget::ObserverWidget(Observer* observer_context, DisplayMode display_mode, QWidget * parent, Qt::WindowFlags f) :
    QMainWindow(parent, f),
    ui(new Ui::ObserverWidget)
{
    ui->setupUi(this);
    d = new ObserverWidgetData;
    d->action_provider = new ActionProvider(this);

    #ifdef QTILITIES_PROPERTY_BROWSER
    d->property_editor_dock_area = Qt::RightDockWidgetArea;
    d->property_editor_type = ObjectPropertyBrowser::TreeBrowser;
    d->property_filter_inversed = false;
    d->property_filter = QStringList();
    d->dynamic_property_editor_dock_area = Qt::RightDockWidgetArea;
    d->dynamic_property_editor_type = ObjectDynamicPropertyBrowser::TreeBrowser;
//    d->dynamic_property_filter_inversed = false;
//    d->dynamic_property_filter = QStringList();
    #endif

    ui->widgetProgressInfo->setVisible(false);
    ui->navigationBarWidget->setVisible(false);

    d->display_mode = display_mode;
    d->hints_default = new ObserverHints(this);

    setWindowIcon(QIcon(qti_icon_QTILITIES_SYMBOL_WHITE_16x16));
    ui->widgetSearchBox->hide();

    // Assign a default meta type for this widget:
    // We construct each action and then register it
    QString context_string = "ObserverWidget";
    int count = 0;
    context_string.append(QString("%1").arg(count));
    while (CONTEXT_MANAGER->hasContext(context_string)) {
        QString count_string = QString("%1").arg(count);
        context_string.chop(count_string.length());
        ++count;
        context_string.append(QString("%1").arg(count));
    }
    CONTEXT_MANAGER->registerContext(context_string);
    d->global_meta_type = context_string;
    d->shared_global_meta_type = QString();
    setObjectName(context_string);

    if (observer_context) {
        setObserverContext(observer_context);
        initialize();
    }
}

Qtilities::CoreGui::ObserverWidget::~ObserverWidget() {
    delete ui;
    delete d;           
}

Qtilities::Core::ObserverHints* Qtilities::CoreGui::ObserverWidget::activeHints() const {
    if (d->use_observer_hints && d->hints_selection_parent)
        return d->hints_selection_parent;
    else
        return d->hints_default;
}

void Qtilities::CoreGui::ObserverWidget::setTreeExpandCollapseVisible(bool is_visible) {
    if (d->is_expand_collapse_visible != is_visible) {
        d->is_expand_collapse_visible = is_visible;
        refreshActions();
    }
}

bool Qtilities::CoreGui::ObserverWidget::treeExpandCollapseVisible() const {
    return d->is_expand_collapse_visible;
}

bool Qtilities::CoreGui::ObserverWidget::setObserverContext(Observer* observer) {
    if (d_observer == observer)
        return false;

    if (!observer) {
        if (d->display_mode == TableView && d->table_model) {
            d->table_model->setObserverContext(0);
        } else if (d->display_mode == TreeView && d->tree_model) {
            d->tree_model->setObserverContext(0);
        }
        setEnabled(false);
        return false;
    } else
        setEnabled(true);

    if (d_observer) {
        d_observer->disconnect(this);
        d->last_display_flags = ObserverHints::NoDisplayFlagsHint;
    }

    if (d->top_level_observer) {
        if (d->display_mode == TableView) {
            // It was set in the navigation stack, don't change it.
        } else if (d->display_mode == TreeView) {
            // Check if the top level observer is in the parent hierarchy of the new observer.
            // If so we leave it, otherwise we set the new observer as the top level observer:
            if (!Observer::isParentInHierarchy(d->top_level_observer,observer))
                d->top_level_observer = observer;
        }
    } else {
        // Top level observer not yet set, we set it to observer.
        d->top_level_observer = observer;
    }

    if (!ObserverAwareBase::setObserverContext(observer))
        return false;

    connect(d_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));

    // Update the observer context of the delegates
    if (d->display_mode == TableView && d->table_name_column_delegate)
        d->table_name_column_delegate->setObserverContext(observer);
    else if (d->display_mode == TreeView && d->tree_name_column_delegate)
        d->tree_name_column_delegate->setObserverContext(observer); 

    emit observerContextChanged(d_observer);
    emit selectedObjectsChanged(QList<QObject*>());
    setEnabled(true);
    return true;
}

void Qtilities::CoreGui::ObserverWidget::toggleLazyInit(bool enabled) {
    d->lazy_init = enabled;
    if (d->tree_model)
        d->tree_model->toggleLazyInit(enabled);
    if (d->table_model)
        d->table_model->toggleLazyInit(enabled);
}

bool Qtilities::CoreGui::ObserverWidget::lazyInitEnabled() const {
    return d->lazy_init;
}

int Qtilities::CoreGui::ObserverWidget::topLevelObserverID() {
    if (d->top_level_observer)
        return d->top_level_observer->observerID();
    else
        return -1;
}

bool Qtilities::CoreGui::ObserverWidget::setCustomTableModel(ObserverTableModel* table_model) {
    if (d->initialized)
        return false;

    if (d->table_model)
        return false;

    if (!table_model)
        return false;

    d->table_model = table_model;
    d->table_model->setParent(this);
    d->table_model->toggleLazyInit(d->lazy_init);
    return true;
}

bool Qtilities::CoreGui::ObserverWidget::setCustomTreeModel(ObserverTreeModel* tree_model) {
    if (d->initialized)
        return false;

    if (d->tree_model)
        return false;

    if (!tree_model)
        return false;

    d->tree_model = tree_model;
    d->tree_model->setParent(this);
    d->tree_model->toggleLazyInit(d->lazy_init);

    connect(d->tree_model,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex& )),this,SLOT(adaptColumns(const QModelIndex &, const QModelIndex&)));
    connect(d->tree_model,SIGNAL(treeModelBuildStarted(int)),SLOT(showProgressInfo(int)));
    connect(d->tree_model,SIGNAL(treeModelBuildEnded()),SLOT(hideProgressInfo()));
    connect(d->tree_model,SIGNAL(expandItemsRequest(QModelIndexList)),SLOT(expandNodes(QModelIndexList)));
    connect(d->tree_model,SIGNAL(treeModelBuildAboutToStart()),SLOT(handleTreeModelBuildAboutToStart()));
    return true;
}

bool Qtilities::CoreGui::ObserverWidget::setCustomTableProxyModel(QAbstractProxyModel* proxy_model) {
    if (d->initialized)
        return false;

    if (d->custom_table_proxy_model)
        return false;

    if (!proxy_model)
        return false;

    d->custom_table_proxy_model = proxy_model;
    d->custom_table_proxy_model->setParent(this);
    return true;
}

bool Qtilities::CoreGui::ObserverWidget::setCustomTreeProxyModel(QAbstractProxyModel* proxy_model) {
    if (d->initialized)
        return false;

    if (d->custom_tree_proxy_model)
        return false;

    if (!proxy_model)
        return false;

    d->custom_tree_proxy_model = proxy_model;
    d->custom_tree_proxy_model->setParent(this);
    return true;
}

Qtilities::CoreGui::ObserverTableModel* Qtilities::CoreGui::ObserverWidget::tableModel() const {
   return d->table_model;
}

Qtilities::CoreGui::ObserverTreeModel* Qtilities::CoreGui::ObserverWidget::treeModel() const {
    return d->tree_model;
}

QAbstractProxyModel* Qtilities::CoreGui::ObserverWidget::proxyModel() const {
    if (d->display_mode == Qtilities::TreeView)
        return d->tree_proxy_model;
    else if (d->display_mode == Qtilities::TableView)
        return d->table_proxy_model;

    return 0;
}

void Qtilities::CoreGui::ObserverWidget::disableProxyModels() const {
    d->disable_proxy_models = true;
}

void Qtilities::CoreGui::ObserverWidget::setDisplayMode(DisplayMode display_mode) {
    if (d->initialized) {
        if (d->display_mode != display_mode) {
            toggleDisplayMode();
        }
    } else
        d->display_mode = display_mode;
}

Qtilities::DisplayMode Qtilities::CoreGui::ObserverWidget::displayMode() const {
    return d->display_mode;
}

void Qtilities::CoreGui::ObserverWidget::setReadOnly(bool read_only) {
    if (d->read_only == read_only)
        return;

    d->read_only = read_only;

    // Make needed things read only:
    if (d->tree_model)
        d->tree_model->setReadOnly(read_only);
    if (d->table_model)
        d->table_model->setReadOnly(read_only);

    if (d->read_only) {
        setEnabled(true);
        if (d->tree_view)
            d->tree_view->setEnabled(true);
        if (d->table_view)
            d->table_view->setEnabled(true);
    }

    refreshActions();

    #ifdef QTILITIES_PROPERTY_BROWSER
    // TODO: This can be improved. Add that property editors can be made read only.
    if (d->property_browser_widget)
        d->property_browser_widget->setEnabled(!read_only);
    if (d->dynamic_property_browser_widget)
        d->dynamic_property_browser_widget->setEnabled(!read_only);
    #endif

    emit readOnlyStateChanged(read_only);
}

bool Qtilities::CoreGui::ObserverWidget::readOnly() const {
    return d->read_only;
}

QStringList Qtilities::CoreGui::ObserverWidget::findExpandedItems() const {
    if (d->display_mode == Qtilities::TreeView && d->tree_model && d->tree_view && d->tree_proxy_model) {
        QStringList expanded_items;

        QModelIndexList all_indexes = d->tree_model->getAllIndexes();
        foreach (QModelIndex index, all_indexes) {           
            ObserverTreeItem* item = d->tree_model->getItem(index);
            if (!item)
                continue;

            if (item->itemType() == ObserverTreeItem::CategoryItem || item->itemType() == ObserverTreeItem::TreeNode) {
                QModelIndex mapped_index;
                if (d->tree_proxy_model)
                    mapped_index = d->tree_proxy_model->mapFromSource(index);
                else
                    mapped_index = index;

                // For categories we check if the parent observer is already in the list of expanded items before we add it:
                Observer* obs = d->tree_model->parentOfIndex(index);
                if (obs) {
                    if (!expanded_items.contains(obs->observerName())) {
                        //qDebug() << "Not adding expanded item" << item->objectName() << "to list of expanded items. Its parent parent is not expanded. Parent: " << obs->observerName();
                        continue;
                    } /*else {
                        qDebug() << "Failed observer parent in list test" << item->objectName();
                    }*/
                }/* else {
                    qDebug() << "Failed observer parent test" << item->objectName();
                }*/

                if (d->tree_view->isExpanded(mapped_index)) {
                    QString item_text = d->tree_model->data(index,Qt::DisplayRole).toString();
                    if (item_text.endsWith("*"))
                        item_text.chop(1);
                    expanded_items << item_text;
                }
            }
        }
        d->last_expanded_items_result = expanded_items;
        return expanded_items;
    }

    return QStringList();
}

QStringList Qtilities::CoreGui::ObserverWidget::lastExpandedItemsResults() const {
    return d->last_expanded_items_result;
}

void Qtilities::CoreGui::ObserverWidget::initialize(bool hints_only) {
    // Check it this widget was initialized previously
    if (!d->initialized) {
        // Setup some flags and attributes for this widget the first time it is constructed.
        setAttribute(Qt::WA_DeleteOnClose, true);
        // Register contextString in the context manager.
        OBJECT_MANAGER->registerObject(this,QtilitiesCategory("GUI::Observer Widgets","::"));
        constructActions();
    }


    d->initialized = false;

    if (!d_observer) {
        LOG_FATAL(tr("You are attempting to initialize an ObserverWidget without an observer context."));
        d->action_provider->disableAllActions();
        return;
    }

    if (d->display_mode == Qtilities::TableView)
        QApplication::setOverrideCursor(Qt::WaitCursor);

    // Set the title and name of the observer widget.
    // Here we need to check if we must use d_observer inside a specific context
    setWindowTitle(d_observer->observerName());
    setObjectName("ObserverWidget: " + d_observer->observerName());
    ui->navigationBarWidget->setVisible(false);

    // Get hints from d_observer:
    if (d->use_observer_hints) {
        // Check if this observer provides hints for this model
        d->hints_selection_parent = d_observer->displayHints();

        // Check if we must connect to the paste action for the new hints:
        Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
        if (command) {
            if (command->action()) {
                if (activeHints()->actionHints() & ObserverHints::ActionPasteItem)
                    connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionPaste_triggered()));
                else
                    command->action()->disconnect();
            }
        }
    }

//    time_t start,end;
//    time(&start);

//    double diff = difftime(end,start);
//    qDebug() << QString("ObserverWidget init 1: " + QString::number(diff) + " seconds.");

    if (!hints_only) {
        // Delete the current layout on itemParentWidget
        if (ui->itemParentWidget->layout())
            delete ui->itemParentWidget->layout();

        // Set the title of this widget
        setWindowTitle(d_observer->observerName());

        // Check and setup the item display mode
        if (d->display_mode == TreeView) {
            disconnect(d_observer,SIGNAL(destroyed()),this,SLOT(contextDeleted()));
            connect(d_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));
            disconnect(d->top_level_observer,SIGNAL(destroyed()),this,SLOT(contextDeleted()));
            connect(d->top_level_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));

            if (d->table_view)
                d->table_view->hide();

            // Check if there is already a model.
            if (!d->tree_view) {
                d->tree_view = new QTreeView(ui->itemParentWidget);
                connect(d->tree_view,SIGNAL(expanded(QModelIndex)),SLOT(handleExpanded(QModelIndex)));
                connect(d->tree_view,SIGNAL(collapsed(QModelIndex)),SLOT(handleCollapsed(QModelIndex)));
                d->tree_view->setFocusPolicy(Qt::StrongFocus);
                d->tree_view->setRootIsDecorated(true);
                d->tree_view->setContextMenuPolicy(Qt::CustomContextMenu);
                d->tree_view->setAcceptDrops(true);
                d->tree_view->setAutoExpandDelay(500);
                d->tree_view->setDropIndicatorShown(true);
                d->tree_view->setDragEnabled(true);
                if (!d->tree_model) {
                    d->tree_model = new ObserverTreeModel(d->tree_view);
                    d->tree_model->toggleLazyInit(d->lazy_init);

                    connect(d->tree_model,SIGNAL(treeModelBuildAboutToStart()),SLOT(handleTreeModelBuildAboutToStart()));
                    connect(d->tree_model,SIGNAL(treeModelBuildStarted(int)),SLOT(showProgressInfo(int)));
                    connect(d->tree_model,SIGNAL(treeModelBuildEnded()),SLOT(hideProgressInfo()));
                    connect(d->tree_model,SIGNAL(dataChanged(const QModelIndex &, const QModelIndex&)),this,SLOT(adaptColumns(const QModelIndex &, const QModelIndex&)));
                    connect(d->tree_model,SIGNAL(expandItemsRequest(QModelIndexList)),SLOT(expandNodes(QModelIndexList)));
                }
                d->tree_view->setSortingEnabled(true);
                d->tree_view->sortByColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName),Qt::AscendingOrder);
                connect(d->tree_model,SIGNAL(selectionParentChanged(Observer*)),SLOT(setTreeSelectionParent(Observer*)),Qt::UniqueConnection);
                connect(d->tree_model,SIGNAL(selectObjects(QList<QPointer<QObject> >)),SLOT(selectObjects(QList<QPointer<QObject> >)),Qt::UniqueConnection);
                connect(d->tree_model,SIGNAL(selectObjects(QList<QObject*>)),SLOT(selectObjects(QList<QObject*>)),Qt::UniqueConnection);
                connect(d->tree_model,SIGNAL(selectCategories(QList<QtilitiesCategory>)),SLOT(selectCategories(QList<QtilitiesCategory>)),Qt::UniqueConnection);

                d->tree_view->viewport()->installEventFilter(this);
                d->tree_view->installEventFilter(this);

                // Setup tree selection:
                d->tree_view->setSelectionBehavior(QAbstractItemView::SelectItems);
                d->tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
            }

            if (ui->itemParentWidget->layout())
                delete ui->itemParentWidget->layout();

            QHBoxLayout* layout = new QHBoxLayout(ui->itemParentWidget);
            layout->setMargin(0);
            layout->addWidget(d->tree_view);

            d->tree_model->setObjectName(d->top_level_observer->observerName());

            // Initialize naming control delegate
            if (!d->tree_name_column_delegate) {
                d->tree_name_column_delegate = new NamingPolicyDelegate(this);
                connect(this,SIGNAL(selectedObjectsChanged(QList<QObject*>)),d->tree_name_column_delegate,SLOT(handleCurrentObjectChanged(QList<QObject*>)));
                d->tree_view->setItemDelegateForColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName),d->tree_name_column_delegate);
            }

            d->tree_name_column_delegate->setObserverContext(d_observer);

            // Setup proxy model:
            if (!d->disable_proxy_models) {
                if (!d->custom_tree_proxy_model) {
                    if (!d->tree_proxy_model) {
                        QSortFilterProxyModel* new_model = new ObserverTreeModelProxyFilter(this);
                        new_model->setDynamicSortFilter(true);
                        new_model->setFilterKeyColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
                        d->tree_proxy_model = new_model;
                    }
                } else {
                    d->tree_proxy_model = d->custom_tree_proxy_model;

                    QSortFilterProxyModel* custom_sort_proxy = qobject_cast<QSortFilterProxyModel*> (d->custom_tree_proxy_model);
                    if (custom_sort_proxy) {
                        custom_sort_proxy->setDynamicSortFilter(true);
                        custom_sort_proxy->setFilterKeyColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
                    }
                }

                if (d->tree_proxy_model != d->tree_view->model()) {
                    d->tree_proxy_model->setSourceModel(d->tree_model);
                    d->tree_view->setModel(d->tree_proxy_model);
                }
            } else {
                if (d->tree_model != d->tree_view->model())
                    d->tree_view->setModel(d->tree_model);
            }

            d->tree_model->setObserverContext(d->top_level_observer);

            if (d->tree_view->selectionModel())
                connect(d->tree_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()),Qt::UniqueConnection);

            // The item view must always be visible.
            d->tree_view->setVisible(true);
        } else if (d->display_mode == TableView) {
            // Connect to the current parent observer, in the tree view the model will monitor this for you.
            disconnect(d_observer,SIGNAL(destroyed()),this,SLOT(contextDeleted()));
            connect(d_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));

            if (d->tree_view)
                d->tree_view->hide();

            // Check if there is already a model.
            if (!d->table_view) {
                d->table_view = new QTableView();
                d->table_view->setFocusPolicy(Qt::StrongFocus);
                d->table_view->setShowGrid(false);
                d->table_view->setAcceptDrops(true);
                d->table_view->setDragEnabled(true);
                d->table_view->setContextMenuPolicy(Qt::CustomContextMenu);
                if (!d->table_model) {
                    d->table_model = new ObserverTableModel(d->table_view);
                    d->table_model->toggleLazyInit(d->lazy_init);
                }

                d->table_view->setSortingEnabled(true);
                connect(d->table_view->verticalHeader(),SIGNAL(sectionCountChanged(int,int)),SLOT(resizeTableViewRows()));
                connect(d->table_view->verticalHeader(),SIGNAL(sectionCountChanged(int,int)),SLOT(resizeColumns()));

                connect(d->table_model,SIGNAL(layoutChangeCompleted()),SLOT(handleLayoutChangeCompleted()));
                connect(d->table_model,SIGNAL(selectObjects(QList<QPointer<QObject> >)),SLOT(selectObjects(QList<QPointer<QObject> >)),Qt::UniqueConnection);

                d->table_view->viewport()->installEventFilter(this);
                d->table_view->installEventFilter(this);

                // Setup the table view to look nice
                d->table_view->setSelectionBehavior(QAbstractItemView::SelectItems);
                d->table_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
                d->table_view->verticalHeader()->setVisible(false);
            }

            // TODO: We don't have to do this everytime... Only when mode changes and first init... (also in tree view side)
            if (ui->itemParentWidget->layout())
                delete ui->itemParentWidget->layout();

            QHBoxLayout* layout = new QHBoxLayout(ui->itemParentWidget);
            layout->setMargin(0);
            layout->addWidget(d->table_view);

//            time(&end);
//            diff = difftime(end,start);
//            qDebug() << QString("ObserverWidget init 2: " + QString::number(diff) + " seconds.");

            d->table_model->setObjectName(d_observer->observerName());
            d->table_model->setObserverContext(d_observer);

            // Initialize naming control delegate
            if (!d->table_name_column_delegate) {
                d->table_name_column_delegate = new NamingPolicyDelegate(this);
                connect(this,SIGNAL(selectedObjectsChanged(QList<QObject*>)),d->table_name_column_delegate,SLOT(handleCurrentObjectChanged(QList<QObject*>)));
                d->table_view->setItemDelegateForColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName),d->table_name_column_delegate);
            }

            d->table_name_column_delegate->setObserverContext(d_observer);

            // Setup proxy model
            if (!d->disable_proxy_models) {
                if (!d->custom_table_proxy_model) {
                    if (!d->table_proxy_model) {
                        QSortFilterProxyModel* new_model = new ObserverTableModelProxyFilter(this);
                        new_model->setDynamicSortFilter(true);
                        new_model->setFilterKeyColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName));
                        d->table_proxy_model = new_model;
                    }
                } else {
                    QSortFilterProxyModel* custom_sort_proxy = qobject_cast<QSortFilterProxyModel*> (d->custom_table_proxy_model);
                    if (custom_sort_proxy) {
                        custom_sort_proxy->setDynamicSortFilter(true);
                        custom_sort_proxy->setFilterKeyColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName));
                    }
                    d->table_proxy_model = d->custom_table_proxy_model;
                }

                if (d->table_proxy_model != d->table_view->model()) {
                    d->table_proxy_model->setSourceModel(d->table_model);
                    d->table_view->setModel(d->table_proxy_model);
                }
            } else {
                if (d->table_model != d->table_view->model())
                    d->table_view->setModel(d->table_model);
            }

//            time(&end);
//            diff = difftime(end,start);
//            qDebug() << QString("ObserverWidget init 3: " + QString::number(diff) + " seconds.");

            if (d->table_view->selectionModel()) {
                d->table_view->selectionModel()->clear();
                connect(d->table_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()),Qt::UniqueConnection);
            }

            // The view must always be visible.
            d->table_view->setVisible(true);
        }
    }

    // Toggle use observer hints on custom models and set custom hints everytime initialize() is called.
    toggleUseObserverHints(d->use_observer_hints);
    setCustomHints(d->hints_default);

    // Check if the hierarchy navigation bar should be visible:
    if (activeHints()->displayFlagsHint() & ObserverHints::NavigationBar) {
        if (!d->navigation_bar) {
            d->navigation_bar = new ObjectHierarchyNavigator();
            QGridLayout* layout = new QGridLayout();
            layout->addWidget(d->navigation_bar);
            layout->setMargin(0);
            if (ui->navigationBarWidget->layout())
                delete ui->navigationBarWidget->layout();
            ui->navigationBarWidget->setLayout(layout);
            ui->navigationBarWidget->resize(d->navigation_bar->width(),d->navigation_bar->width());
        }

        if (d->display_mode == TableView) {
            d->navigation_bar->setVisible(true);
            d->navigation_bar->setCurrentObject(d_observer);
            d->navigation_bar->setNavigationStack(d->navigation_stack);
            d->navigation_bar->setVisible(true);
            ui->navigationBarWidget->setVisible(true);
        } else if (d->display_mode == TreeView) {
            d->navigation_bar->setVisible(false);
            ui->navigationBarWidget->setVisible(false);
        }
    } else {
        if (d->navigation_bar) {
            delete d->navigation_bar;
            d->navigation_bar = 0;
        }
        ui->navigationBarWidget->setVisible(false);
    }

    // Refreshes the visibility of columns:
    // We only do this for table views here, for tree views this happens after the tree
    // build is complete in hideProgressInfo() connected to treeModelBuildEnded() on the tree:
    if (displayMode() == Qtilities::TableView)
        refreshColumnVisibility();

    d->initialized = true;
    if (!hints_only) {
        // Construct the property browser if neccesarry:
        #ifdef QTILITIES_PROPERTY_BROWSER
        refreshPropertyBrowser();
        refreshDynamicPropertyBrowser();
        #endif
        installEventFilter(this);
    }

    bool create_default_selection = true;
    if (activeHints()) {
        // Ok since the new observer provides hints, we need to see if we must select its' active objects:
        // Check if the observer has a FollowSelection actity policy
        // In that case the observer widget, in table mode must select objects which are active and adapt to changes in the activity filter.
        if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
            // Check if the observer has an activity filter, which it should have with this hint:
            ActivityPolicyFilter* filter = 0;
            for (int i = 0; i < d_observer->subjectFilters().count(); i++) {
                filter = qobject_cast<ActivityPolicyFilter*> (d_observer->subjectFilters().at(i));
                if (filter) {
                    d->activity_filter = filter;

                    // Connect to the activity change signal (to update activity on observer widget side):
                    connect(d->activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(selectObjects(QList<QObject*>)),Qt::UniqueConnection);
                    QList<QObject*> active_subjects = d->activity_filter->activeSubjects();
                    selectObjects(active_subjects);
                    create_default_selection = false;
                    continue;
                }
            }
        } else {
            if (d->activity_filter)
                d->activity_filter->disconnect(this);
            d->activity_filter = 0;
        }

        if (create_default_selection && d->update_selection_activity) {
            selectObjects(QList<QObject*>());
        }
    }

    // Init all actions happens in here:
    // Don't update selection activity, already happened in the above code:
    d->update_selection_activity = false;
    handleSelectionModelChange();
    d->update_selection_activity = true;
    // If selection problems occur, uncomment the above again.
    //refreshActions();
    refreshActionToolBar();

    // Resize view:
    if (!hints_only)
        hideProgressInfo(false);

    resizeColumns();

    if (d->display_mode == Qtilities::TableView)
        QApplication::restoreOverrideCursor();
}

void Qtilities::CoreGui::ObserverWidget::refreshColumnVisibility() {
    // Update the columns shown
    if (d->display_mode == TableView && d->table_view && d->table_model) {
        // Show only the needed columns for the current observer.
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnNameHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName));
        else {
            d->table_view->showColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnIDHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnSubjectID));
        else {
            d->table_view->showColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnSubjectID));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnCategoryHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
        else {
//            if (activeHints()->hierarchicalDisplayHint() & ObserverHints::CategorizedHierarchy) {
                d->table_view->showColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
//            } else {
//                d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
//            }
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
        else {
            d->table_view->showColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
        else {
            d->table_view->showColumn(AbstractObserverItemModel::ColumnTypeInfo);
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
        else {
            d->table_view->showColumn(AbstractObserverItemModel::ColumnAccess);
        }
    } else if (d->display_mode == TreeView && d->tree_view && d->tree_model) {
        // Show only the needed columns for the current observer
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnNameHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
        }
    }
}

QStack<int> Qtilities::CoreGui::ObserverWidget::navigationStack() const {
    return d->navigation_stack;
}

void Qtilities::CoreGui::ObserverWidget::setNavigationStack(QStack<int> navigation_stack) {
    if (navigation_stack.size() == 0)
        d->top_level_observer = 0;

    d->navigation_stack = navigation_stack;

    // Check if the front item in the naviation stack is different from top_level_observer.
    // If so we set the top level to the first item.
    if (d->top_level_observer) {
        if (d->navigation_stack.front() != d->top_level_observer->observerID()) {
            d->top_level_observer = OBJECT_MANAGER->observerReference(d->navigation_stack.front());
        }
    } else {
        // setObserverContext not yet called, we set it to the front item.
        if (d->navigation_stack.count() > 0)
            d->top_level_observer = OBJECT_MANAGER->observerReference(d->navigation_stack.front());
    }

    // We need to check the subjectChange signal on all the navigation stack items.
    // For now, we just check if any of them detaches an observer, in that case we close this widget since
    // we cannot garuantee that the detached item doesn't appear in the stack.
    for (int i = 0; i < d->navigation_stack.count(); i++) {
        Observer* stack_observer = OBJECT_MANAGER->observerReference(d->navigation_stack.at(i));
        if (stack_observer)
            connect(stack_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication,QList<QPointer<QObject> >)),
                    SLOT(contextDetachHandler(Observer::SubjectChangeIndication,QList<QPointer<QObject> >)));
    }
}

void Qtilities::CoreGui::ObserverWidget::toggleUseObserverHints(bool toggle) {
    d->use_observer_hints = toggle;
    // Important: We need to change the models of this observer widget as well:
    if (d->tree_model)
        d->tree_model->toggleUseObserverHints(toggle);
    if (d->table_model)
        d->table_model->toggleUseObserverHints(toggle);
    // Important: We need to change the table proxy filters as well.
    // Note that we don't change the tree proxy filters since the tree model builder
    // does not build the tree for filtered categories.
    if (d->table_proxy_model) {
        ObserverTableModelProxyFilter* obs_proxy_model = qobject_cast<ObserverTableModelProxyFilter*> (d->table_proxy_model);
        if (obs_proxy_model)
            obs_proxy_model->toggleUseObserverHints(toggle);
    }
    if (d->custom_table_proxy_model) {
        ObserverTableModelProxyFilter* obs_proxy_model = qobject_cast<ObserverTableModelProxyFilter*> (d->custom_table_proxy_model);
        if (obs_proxy_model)
            obs_proxy_model->toggleUseObserverHints(toggle);
    }
}

bool Qtilities::CoreGui::ObserverWidget::usesObserverHints() const {
    return d->use_observer_hints;
}

bool Qtilities::CoreGui::ObserverWidget::setCustomHints(ObserverHints* custom_hints) {
    if (!custom_hints)
        return false;

    if (!d->hints_default)
        return false;

    *d->hints_default = *custom_hints;
    // Important: We need to change the models of this observer widget as well:
    if (d->tree_model)
        d->tree_model->setCustomHints(custom_hints);
    if (d->table_model)
        d->table_model->setCustomHints(custom_hints);
    // Important: We need to change the table proxy filters as well.
    // Note that we don't change the tree proxy filters since the tree model builder
    // does not build the tree for filtered categories.
    if (d->table_proxy_model) {
        ObserverTableModelProxyFilter* obs_proxy_model = qobject_cast<ObserverTableModelProxyFilter*> (d->table_proxy_model);
        if (obs_proxy_model)
            obs_proxy_model->setCustomHints(custom_hints);
    }
    if (d->custom_table_proxy_model) {
        ObserverTableModelProxyFilter* obs_proxy_model = qobject_cast<ObserverTableModelProxyFilter*> (d->custom_table_proxy_model);
        if (obs_proxy_model)
            obs_proxy_model->setCustomHints(custom_hints);
    }
    return true;
}

QList<QObject*> Qtilities::CoreGui::ObserverWidget::selectedObjects() const {
    QList<QObject*> selected_objects;
    QList<QPointer<QObject> > smart_selected_objects;
    QList<QPointer<ObserverTreeItem> > smart_tree_item_selection;

    if (d->display_mode == TableView) {
        if (!d->table_view || !d->table_model)
            return selected_objects;

        if (d->table_view->selectionModel()) {          
            QModelIndexList selected_indexes = d->table_view->selectionModel()->selectedIndexes();
            for (int i = 0; i < selected_indexes.count(); i++) {
                QModelIndex index = selected_indexes.at(i);
                if (index.column() == 1) {
                    QModelIndex mapped_idx = index;
                    if (d->table_proxy_model)
                        mapped_idx = d->table_proxy_model->mapToSource(index);
                    QObject* obj = d->table_model->getObject(mapped_idx);
                    smart_selected_objects << obj;
                    selected_objects << obj;
                }
            }
        }

        d->table_model->setSelectedObjects(smart_selected_objects);
    } else if (d->display_mode == TreeView) {
        if (!d->tree_view || !d->tree_model)
            return selected_objects;

        QList<QtilitiesCategory> selected_categories;

        if (d->tree_view->selectionModel()) {
            QModelIndexList selected_indexes = d->tree_view->selectionModel()->selectedIndexes();
            for (int i = 0; i < selected_indexes.count(); i++) {
                QModelIndex index = selected_indexes.at(i);
                if (index.column() == 0) {
                    QModelIndex mapped_idx = index;
                    if (d->tree_proxy_model)
                        mapped_idx = d->tree_proxy_model->mapToSource(index);
                    QObject* obj = d->tree_model->getObject(mapped_idx);
                    ObserverTreeItem* tree_item = d->tree_model->getItem(mapped_idx);
                    if (tree_item->itemType() == ObserverTreeItem::CategoryItem)
                        selected_categories << tree_item->category();
                    else {
                        smart_selected_objects << obj;
                        smart_tree_item_selection << tree_item;
                        selected_objects << obj;
                    }
                }
            }
        }

        d->tree_model->setSelectedObjects(smart_selected_objects);
        d->tree_model->setSelectedCategories(selected_categories);
    }
    d->current_selection = smart_selected_objects;   
    d->current_tree_item_selection = smart_tree_item_selection;
    selectedObjectsContextMatch();
    selectedObjectsHintsMatch();
    return selected_objects;
}

bool Qtilities::CoreGui::ObserverWidget::selectedObjectsContextMatch() const {
    QModelIndexList indexes = selectedIndexes();
    Observer* parent = 0;
    bool match = true;

    foreach (QModelIndex index, indexes) {
        Observer* obs = d->tree_model->parentOfIndex(index);
        if (parent == 0)
            parent = obs;
        else if (obs != parent) {
            parent = obs;
            match = false;
            break;
        }
    }

    return match;
}

bool Qtilities::CoreGui::ObserverWidget::selectedObjectsHintsMatch() const {
    if (!usesObserverHints())
        return true;

    QModelIndexList indexes = selectedIndexes();
    Observer* parent = 0;
    bool match = true;

    foreach (QModelIndex index, indexes) {
        Observer* obs = d->tree_model->parentOfIndex(index);
        if (parent == 0) {
            parent = obs;
        } else {
            ObserverHints* hintsA = 0;
            ObserverHints* hintsB = 0;
            if (!obs)
                hintsA = d_observer->displayHints();
            else
                hintsA = obs->displayHints();

            if (parent)
                hintsB = parent->displayHints();

            if (hintsA != 0 && hintsB != 0) {
                if (*hintsA != *hintsB) {
                    parent = obs;
                    match = false;
                    break;
                }
            }
        }
    }

    return match;
}

Qtilities::Core::Observer* Qtilities::CoreGui::ObserverWidget::selectionParent() const {
    if (!d->initialized)
        return 0;

    if (d->current_selection.count() == 0)
        return 0;

    if (d->display_mode == TreeView && d->tree_model) {
        return d->tree_model->selectionParent();
    } else if (d->display_mode == TableView ) {
        return d_observer;
    }

    return 0;
}

QModelIndexList Qtilities::CoreGui::ObserverWidget::selectedIndexes() const {
    QModelIndexList selected_indexes;

    if (d->display_mode == TableView) {
        if (!d->table_view || !d->table_model || !d->table_proxy_model)
            return selected_indexes;

        if (d->table_view->selectionModel()) {
            QModelIndexList selected_indexes_tmp = d->table_view->selectionModel()->selectedIndexes();
            for (int i = 0; i < selected_indexes_tmp.count(); i++) {
                QModelIndex index = selected_indexes_tmp.at(i);
                if (index.column() == 1)
                    selected_indexes << d->table_proxy_model->mapToSource(index);
            }
        }
    } else if (d->display_mode == TreeView) {
        if (!d->tree_view || !d->tree_model || !d->tree_proxy_model)
            return selected_indexes;

        if (d->tree_view->selectionModel()) {
            QModelIndexList selected_indexes_tmp = d->tree_view->selectionModel()->selectedIndexes();
            for (int i = 0; i < selected_indexes_tmp.count(); i++) {
                QModelIndex index = selected_indexes_tmp.at(i);
                if (index.column() == 0)
                    selected_indexes << d->tree_proxy_model->mapToSource(index);
            }
        }
    }
    return selected_indexes;
}

void Qtilities::CoreGui::ObserverWidget::setDefaultRowHeight(int height) {
    d->default_row_height = height;
    resizeTableViewRows();
}

int Qtilities::CoreGui::ObserverWidget::defaultRowHeight() const {
    return d->default_row_height;
}

QTableView* Qtilities::CoreGui::ObserverWidget::tableView() {
    if (d->display_mode == TableView)
        return d->table_view;
    else
        return 0;
}

QTreeView* Qtilities::CoreGui::ObserverWidget::treeView() {
    if (d->display_mode == TreeView)
        return d->tree_view;
    else
        return 0;
}

QAbstractItemView *Qtilities::CoreGui::ObserverWidget::view() {
    if (d->display_mode == TreeView)
        return d->tree_view;
    else if (d->display_mode == TableView)
        return d->table_view;
    else
        return 0;
}

void Qtilities::CoreGui::ObserverWidget::enableAutoColumnResizing() {
    d->do_column_resizing = true;
}

void Qtilities::CoreGui::ObserverWidget::disableAutoColumnResizing() {
    d->do_column_resizing = false;
}

void Qtilities::CoreGui::ObserverWidget::writeSettings() {
    if (!d->initialized)
        return;

    if (!QtilitiesCoreApplication::qtilitiesSettingsPathEnabled())
        return;

    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup(d->global_meta_type);
    settings.setValue("display_mode", (int) d->display_mode);
    settings.setValue("state", saveState());
    settings.setValue("default_row_heigth", d->default_row_height);
    settings.setValue("confirm_deletes", d->confirm_deletes);
    settings.setValue("do_column_resizing", d->do_column_resizing);
    if (d->table_view)
        settings.setValue("table_view_show_grid", d->table_view->showGrid());

    if (d->display_mode == Qtilities::TableView && d->table_model && d->table_view) {
        QHeaderView* table_header = d->table_view->horizontalHeader();
        settings.setValue("table_header_state", table_header->saveState());
    } else if (d->tree_view && d->tree_model && d->display_mode == Qtilities::TreeView) {
        QHeaderView* tree_header = d->tree_view->header();
        settings.setValue("tree_header_state", tree_header->saveState());
    }

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::ObserverWidget::readSettings() {
    if (!d->initialized) {
        LOG_ERROR(QString(tr("readSettings() can only be called on observer widget \"%1\" after the widget has been initialized.")).arg(d->global_meta_type));
        return;
    }

    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup(d->global_meta_type);

    // Display mode
    if (d->display_mode != settings.value("display_mode", TableView).toInt()) {
        if (d->display_mode == TableView) {
            d->display_mode = TreeView;
            setObserverContext(d->top_level_observer);
            initialize();
        } else if (d->display_mode == TreeView) {
            d->display_mode = TableView;
            initialize();
        }
    }

    // Default row height
    d->default_row_height = settings.value("default_row_heigth", 17).toInt();
    resizeTableViewRows();

    // MainWindow state
    restoreState(settings.value("state").toByteArray());

    // Display grid
    if (d->table_view)
        d->table_view->setShowGrid(settings.value("table_view_show_grid", false).toBool());

    // Confirm deletes
    d->confirm_deletes = settings.value("confirm_deletes", true).toBool();

    // Automatic column resizing
    d->do_column_resizing = settings.value("do_column_resizing", true).toBool();

    // Header settings:
    if (d->display_mode == Qtilities::TableView && d->table_model && d->table_view) {
        QHeaderView* table_header = d->table_view->horizontalHeader();
        table_header->restoreState(settings.value("table_header_state").toByteArray());
    } else if (d->tree_view && d->tree_model && d->display_mode == Qtilities::TreeView) {
        QHeaderView* tree_header = d->tree_view->header();
        tree_header->restoreState(settings.value("tree_header").toByteArray());
    }

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();

    // Connect in order for settings to be written when application quits
    connect(QCoreApplication::instance(),SIGNAL(aboutToQuit()),SLOT(writeSettings()));
    if (QtilitiesApplication::instance(true))
        connect(QtilitiesApplication::instance(),SIGNAL(settingsUpdateRequest(QString)),SLOT(handleSettingsUpdateRequest(QString)));
}

void Qtilities::CoreGui::ObserverWidget::setSharedGlobalMetaType(const QString& shared_meta_type) {
    d->shared_global_meta_type = shared_meta_type;
}

QString Qtilities::CoreGui::ObserverWidget::sharedGlobalMetaType() const {
    return d->shared_global_meta_type;
}

bool Qtilities::CoreGui::ObserverWidget::setGlobalMetaType(const QString& meta_type) {
    // Check if this global meta type is allowed.
    if (CONTEXT_MANAGER->hasContext(meta_type))
        return false;

    d->global_meta_type = meta_type;
    return true;
}

QString Qtilities::CoreGui::ObserverWidget::globalMetaType() const {
    return d->global_meta_type;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::ObserverWidget::actionProvider() {
    return d->action_provider;
}

void Qtilities::CoreGui::ObserverWidget::handleSettingsUpdateRequest(const QString& request_id) {
    if (request_id == d->global_meta_type)
        readSettings();
}

#ifdef QTILITIES_PROPERTY_BROWSER
Qtilities::CoreGui::ObjectPropertyBrowser* Qtilities::CoreGui::ObserverWidget::propertyBrowser() {
    return d->property_browser_widget;
}

QDockWidget* Qtilities::CoreGui::ObserverWidget::propertyBrowserDock() {
    return d->property_browser_dock;
}

void Qtilities::CoreGui::ObserverWidget::setPreferredPropertyEditorDockArea(Qt::DockWidgetArea property_editor_dock_area) {
    d->property_editor_dock_area = property_editor_dock_area;
}

void Qtilities::CoreGui::ObserverWidget::setPreferredPropertyEditorType(ObjectPropertyBrowser::BrowserType property_editor_type) {
    d->property_editor_type = property_editor_type;
}

void Qtilities::CoreGui::ObserverWidget::setPreferredPropertyFilter(QStringList filter_list, bool inversed_filter) {
    d->property_filter = filter_list;
    d->property_filter_inversed = inversed_filter;
}

Qtilities::CoreGui::ObjectDynamicPropertyBrowser* Qtilities::CoreGui::ObserverWidget::dynamicPropertyBrowser() {
    return d->dynamic_property_browser_widget;
}

QDockWidget* Qtilities::CoreGui::ObserverWidget::dynamicPropertyBrowserDock() {
    return d->dynamic_property_browser_dock;
}

void Qtilities::CoreGui::ObserverWidget::setPreferredDynamicPropertyEditorDockArea(Qt::DockWidgetArea property_editor_dock_area) {
    d->dynamic_property_editor_dock_area = property_editor_dock_area;
}

void Qtilities::CoreGui::ObserverWidget::setPreferredDynamicPropertyEditorType(ObjectDynamicPropertyBrowser::BrowserType property_editor_type) {
    d->dynamic_property_editor_type = property_editor_type;
}

//void Qtilities::CoreGui::ObserverWidget::setPreferredDynamicPropertyFilter(QStringList filter_list, bool inversed_filter) {
//    d->dynamic_property_filter = filter_list;
//    d->dynamic_property_filter_inversed = inversed_filter;
//}

#endif

Qtilities::CoreGui::SearchBoxWidget* Qtilities::CoreGui::ObserverWidget::searchBoxWidget() {
    return d->searchBoxWidget;
}

void Qtilities::CoreGui::ObserverWidget::resizeTableViewRows(int height) {
    if (height == -1)
        height = d->default_row_height;

    if (d->display_mode == TableView && d->table_view && d->table_model) {       
        for (int i = 0; i < d->table_model->rowCount(); i++)
            d->table_view->setRowHeight(i,height);
    }
}

void Qtilities::CoreGui::ObserverWidget::constructActions() {
    if (d->actionNewItem)
        return;

    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(d->global_meta_type));

    ACTION_MANAGER->commandObserver()->startProcessingCycle();

    // ---------------------------
    // New Item
    // ---------------------------
    d->actionNewItem = new QAction(QIcon(qti_icon_NEW_16x16),tr("New Item"),this);
    d->actionNewItem->setShortcut(QKeySequence("+"));
    connect(d->actionNewItem,SIGNAL(triggered()),SLOT(handle_actionNewItem_triggered()));
    Command* command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_NEW_ITEM,d->actionNewItem,context);
    d->action_provider->addAction(d->actionNewItem,QtilitiesCategory(tr("Items")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Remove Item
    // ---------------------------
    d->actionRemoveItem = new QAction(QIcon(qti_icon_REMOVE_ONE_16x16),tr("Detach Selection"),this);
    connect(d->actionRemoveItem,SIGNAL(triggered()),SLOT(selectionDetach()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_REMOVE_ITEM,d->actionRemoveItem,context);
    d->action_provider->addAction(d->actionRemoveItem,QtilitiesCategory(tr("Items")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Delete Item
    // ---------------------------
    d->actionDeleteItem = new QAction(QIcon(qti_icon_DELETE_ONE_16x16),tr("Delete Selection"),this);
    d->actionDeleteItem->setShortcut(QKeySequence(QKeySequence::Delete));
    connect(d->actionDeleteItem,SIGNAL(triggered()),SLOT(selectionDelete()));
    command = ACTION_MANAGER->registerAction(qti_action_SELECTION_DELETE,d->actionDeleteItem,context);
    d->action_provider->addAction(d->actionDeleteItem,QtilitiesCategory(tr("Items")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Remove All
    // ---------------------------    
    d->actionRemoveAll = new QAction(QIcon(qti_icon_REMOVE_ALL_16x16),tr("Deatch All Children"),this);
    connect(d->actionRemoveAll,SIGNAL(triggered()),SLOT(selectionDetachAll()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_REMOVE_ALL,d->actionRemoveAll,context);
    d->action_provider->addAction(d->actionRemoveAll,QtilitiesCategory(tr("Items")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Delete All
    // ---------------------------
    d->actionDeleteAll = new QAction(QIcon(qti_icon_DELETE_ALL_16x16),tr("Delete All Children"),this);
    connect(d->actionDeleteAll,SIGNAL(triggered()),SLOT(selectionDeleteAll()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_DELETE_ALL,d->actionDeleteAll,context);
    d->action_provider->addAction(d->actionDeleteAll,QtilitiesCategory(tr("Items")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Switch View
    // ---------------------------
    d->actionSwitchView = new QAction(QIcon(),tr("Switch View"),this);
    d->actionSwitchView->setShortcut(QKeySequence("F4"));
    connect(d->actionSwitchView,SIGNAL(triggered()),SLOT(toggleDisplayMode()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_SWITCH_VIEW,d->actionSwitchView,context);
    d->action_provider->addAction(d->actionSwitchView,QtilitiesCategory(tr("View")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Refresh View
    // ---------------------------
    d->actionRefreshView = new QAction(QIcon(qti_icon_REFRESH_16x16),tr("Refresh View"),this);
    d->actionSwitchView->setShortcut(QKeySequence(QKeySequence::Refresh));
    connect(d->actionRefreshView,SIGNAL(triggered()),SLOT(refresh()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_REFRESH_VIEW,d->actionRefreshView,context);
    d->action_provider->addAction(d->actionRefreshView,QtilitiesCategory(tr("View")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Find Item
    // ---------------------------
    d->actionFindItem = new QAction(QIcon(qti_icon_FIND_16x16),tr("Find"),this);
    d->actionFindItem->setShortcut(QKeySequence(QKeySequence::Find));
    connect(d->actionFindItem,SIGNAL(triggered()),SLOT(toggleSearchBox()));
    command = ACTION_MANAGER->registerAction(qti_action_EDIT_FIND,d->actionFindItem,context);
    d->action_provider->addAction(d->actionFindItem,QtilitiesCategory(tr("View")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Go To Parent
    // ---------------------------
    d->actionPushUp = new QAction(QIcon(qti_icon_PUSH_UP_CURRENT_16x16),tr("Go To Parent"),this);
    d->actionPushUp->setShortcut(QKeySequence("Left"));
    connect(d->actionPushUp,SIGNAL(triggered()),SLOT(selectionPushUp()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_HIERARCHY_UP,d->actionPushUp,context);
    d->action_provider->addAction(d->actionPushUp,QtilitiesCategory(tr("Hierarchy")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Go To Parent In New Window
    // ---------------------------
    d->actionPushUpNew = new QAction(QIcon(qti_icon_PUSH_UP_NEW_16x16),tr("Go To Parent (New Window)"),this);
    connect(d->actionPushUpNew,SIGNAL(triggered()),SLOT(selectionPushUpNew()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_HIERARCHY_UP_NEW,d->actionPushUpNew,context);
    d->action_provider->addAction(d->actionPushUpNew,QtilitiesCategory(tr("Hierarchy")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Push Down
    // ---------------------------
    d->actionPushDown = new QAction(QIcon(qti_icon_PUSH_DOWN_CURRENT_16x16),tr("Push Down"),this);
    d->actionPushDown->setShortcut(QKeySequence("Right"));
    connect(d->actionPushDown,SIGNAL(triggered()),SLOT(selectionPushDown()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_HIERARCHY_DOWN,d->actionPushDown,context);
    d->action_provider->addAction(d->actionPushDown,QtilitiesCategory(tr("Hierarchy")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Push Down In New Window
    // ---------------------------
    d->actionPushDownNew = new QAction(QIcon(qti_icon_PUSH_DOWN_NEW_16x16),tr("Push Down (New Window)"),this);
    connect(d->actionPushDownNew,SIGNAL(triggered()),SLOT(selectionPushDownNew()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_HIERARCHY_DOWN_NEW,d->actionPushDownNew,context);
    d->action_provider->addAction(d->actionPushDownNew,QtilitiesCategory(tr("Hierarchy")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Expand All
    // ---------------------------
    d->actionExpandAll = new QAction(QIcon(qti_icon_MAGNIFY_PLUS_16x16),tr("Expand All"),this);
    d->actionExpandAll->setShortcut(QKeySequence("Ctrl+>"));
    connect(d->actionExpandAll,SIGNAL(triggered()),SLOT(viewExpandAll()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_HIERARCHY_EXPAND,d->actionExpandAll,context);
    d->action_provider->addAction(d->actionExpandAll,QtilitiesCategory(tr("Hierarchy")));
    command->setCategory(QtilitiesCategory("Item Views"));
    // ---------------------------
    // Collapse All
    // ---------------------------
    d->actionCollapseAll = new QAction(QIcon(qti_icon_MAGNIFY_MINUS_16x16),tr("Collapse All"),this);
    d->actionCollapseAll->setShortcut(QKeySequence("Ctrl+<"));
    connect(d->actionCollapseAll,SIGNAL(triggered()),SLOT(viewCollapseAll()));
    command = ACTION_MANAGER->registerAction(qti_action_CONTEXT_HIERARCHY_COLLAPSE,d->actionCollapseAll,context);
    d->action_provider->addAction(d->actionCollapseAll,QtilitiesCategory(tr("Hierarchy")));
    command->setCategory(QtilitiesCategory("Item Views"));

    #ifndef QT_NO_DEBUG
    // ---------------------------
    // Object Debug
    // ---------------------------
    d->actionDebugObject = new QAction(QIcon(qti_icon_DEBUG_16x16),tr("Debug Object<br><br><span style=\"color: gray;\">Adds the selected object to your global object pool. If the debug plugin is loaded, you can inspect the object there.</span>"),this);
    connect(d->actionDebugObject,SIGNAL(triggered()),SLOT(selectionDebug()));
    d->action_provider->addAction(d->actionDebugObject,QtilitiesCategory(tr("Items")));
    #endif

    ACTION_MANAGER->commandObserver()->endProcessingCycle(false);
}

#ifndef QT_NO_DEBUG
void Qtilities::CoreGui::ObserverWidget::selectionDebug() const {
    bool change_mode = false;
    foreach (QObject* obj, d->current_selection) {
        OBJECT_MANAGER->registerObject(obj);
        change_mode = true;
    }

    if (change_mode) {
        QtilitiesMainWindow* main_window = qobject_cast<QtilitiesMainWindow*> (QtilitiesApplication::mainWindow());
        if (main_window) {
            if (main_window->modeManager()) {
                main_window->modeManager()->setActiveMode("Debugging");
            }
        }
    }
}
#endif

void Qtilities::CoreGui::ObserverWidget::refreshActions() {
    if (!d->initialized && !d->actionRemoveItem) {
        d->action_provider->disableAllActions();
        deleteActionToolBars();
        return;
    } else {
        d->action_provider->enableAllActions();
    }

    // Ok, first we set only actions specified by the observer's action hints to be visible
    if (activeHints()->actionHints() & ObserverHints::ActionRemoveItem)
        d->actionRemoveItem->setVisible(true);
    else
        d->actionRemoveItem->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionRemoveAll)
        d->actionRemoveAll->setVisible(true);
    else
        d->actionRemoveAll->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionDeleteItem)
        d->actionDeleteItem->setVisible(true);
    else
        d->actionDeleteItem->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionDeleteAll)
        d->actionDeleteAll->setVisible(true);
    else
        d->actionDeleteAll->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionPushDown) {
        d->actionPushDown->setVisible(true);
        connect(this,SIGNAL(doubleClickRequest(QObject*)),SLOT(selectionPushDown()),Qt::UniqueConnection);
    } else {
        d->actionPushDown->setVisible(false);
        disconnect(this,SIGNAL(doubleClickRequest(QObject*)),this,SLOT(selectionPushDown()));
    }

    if (activeHints()->actionHints() & ObserverHints::ActionPushDownNew)
        d->actionPushDownNew->setVisible(true);
    else
        d->actionPushDownNew->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionPushUp)
        d->actionPushUp->setVisible(true);
    else
        d->actionPushUp->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionPushUpNew)
        d->actionPushUpNew->setVisible(true);
    else
        d->actionPushUpNew->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionNewItem)
        d->actionNewItem->setVisible(true);
    else
        d->actionNewItem->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionRefreshView)
        d->actionRefreshView->setVisible(true);
    else
        d->actionRefreshView->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionSwitchView)
        d->actionSwitchView->setVisible(true);
    else
        d->actionSwitchView->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionFindItem)
        d->actionFindItem->setVisible(true);
    else
        d->actionFindItem->setVisible(false);

    // Remove & Delete All Actions
    if (d_observer) {
        if (d_observer->subjectCount() > 0) {
            d->actionRemoveAll->setEnabled(true);
            d->actionDeleteAll->setEnabled(true);
        } else {
            d->actionRemoveAll->setEnabled(false);
            d->actionDeleteAll->setEnabled(false);
        }
    }

    // New Item
    d->actionNewItem->setEnabled(true);

    // Navigating Up/Down Actions
    if (d->display_mode == TableView) {
        d->actionSwitchView->setIcon(QIcon(qti_icon_TREE_16x16));
        if (d->navigation_stack.count() == 0) {
            d->actionPushUp->setEnabled(false);
            d->actionPushUpNew->setEnabled(false);
        } else {
            d->actionPushUp->setEnabled(true);
            d->actionPushUpNew->setEnabled(true);
        }
        d->actionCollapseAll->setEnabled(false);
        d->actionExpandAll->setEnabled(false);
        d->actionCollapseAll->setVisible(false);
        d->actionExpandAll->setVisible(false);
        if (d->actionFilterCategories) {
            d->actionFilterCategories->setVisible(false);
            d->actionFilterNodes->setVisible(false);
            d->actionFilterItems->setVisible(false);
            d->actionFilterTypeSeperator->setVisible(false);
        }
        // Remove & Delete All Actions
        if (d_observer) {
            if (d_observer->subjectCount() > 0) {
                d->actionRemoveAll->setEnabled(true);
                d->actionDeleteAll->setEnabled(true);
            } else {
                d->actionRemoveAll->setEnabled(false);
                d->actionDeleteAll->setEnabled(false);
            }
        }
    } else {
        d->actionCollapseAll->setVisible(d->is_expand_collapse_visible);
        d->actionExpandAll->setVisible(d->is_expand_collapse_visible);
        d->actionCollapseAll->setEnabled(d->is_expand_collapse_visible);
        d->actionExpandAll->setEnabled(d->is_expand_collapse_visible);

        d->actionSwitchView->setIcon(QIcon(qti_icon_TABLE_16x16));
        d->actionPushDown->setEnabled(false);
        d->actionPushUp->setEnabled(false);
        d->actionPushDownNew->setEnabled(false);
        d->actionPushUpNew->setEnabled(false);
        d->actionPushDown->setVisible(false);
        d->actionPushUp->setVisible(false);
        d->actionPushDownNew->setVisible(false);
        d->actionPushUpNew->setVisible(false);
        if (d->actionFilterCategories) {
            d->actionFilterCategories->setVisible(true);
            d->actionFilterNodes->setVisible(true);
            d->actionFilterItems->setVisible(true);
            d->actionFilterTypeSeperator->setVisible(true);
        }
    }

    // Remove & Delete Items + Navigating Up & Down
    if (d->current_selection.count() == 0) {
        d->actionDeleteItem->setEnabled(false);
        d->actionRemoveItem->setEnabled(false);
        if (d->display_mode == TableView) {
            d->actionPushDown->setEnabled(false);
            d->actionPushDownNew->setEnabled(false);            
        } else if (d->display_mode == TreeView) {
            d->actionRemoveAll->setEnabled(false);
            d->actionDeleteAll->setEnabled(false);
        }
        #ifndef QT_NO_DEBUG
        d->actionDebugObject->setVisible(false);
        #endif
    } else {       
        #ifndef QT_NO_DEBUG
        d->actionDebugObject->setVisible(true);
        #endif
        if (d->display_mode == TableView) {
            d->actionDeleteItem->setEnabled(true);
            d->actionRemoveItem->setEnabled(true);
            d->actionPushDown->setEnabled(false);
            d->actionPushDownNew->setEnabled(false);

            // Inspect property to see if push up, or push down related actions should be enabled.
            if (d->current_selection.count() == 1) {
                // Check if the selected object is an observer.
                QObject* obj = d->current_selection.front();
                if (obj) {
                    if (d_observer) {
                        d->actionDeleteItem->setText("Delete \"" + d_observer->subjectNameInContext(obj) + "\"");
                        d->actionRemoveItem->setText("Detach \"" + d_observer->subjectNameInContext(obj) + "\"");
                    } else {
                        d->actionDeleteItem->setText("Delete \"" + obj->objectName() + "\"");
                        d->actionRemoveItem->setText("Detach \"" + obj->objectName() + "\"");
                    }

                    Observer* observer = qobject_cast<Observer*> (obj);
                    if (observer) {
                        d->actionPushDown->setEnabled(true);
                        d->actionPushDownNew->setEnabled(true);

                        // Delete and Remove All depends on the selected context:
                        if (observer->displayHints()->observerSelectionContextHint() == ObserverHints::SelectionUseSelectedContext) {
                            if (observer->subjectCount() > 0) {
                                d->actionRemoveAll->setText(tr("Detach All Under \"") + observer->observerName() + "\"");
                                d->actionRemoveAll->setEnabled(true);
                                d->actionDeleteAll->setText(tr("Delete All Under: ") + observer->observerName() + "\"");
                                d->actionDeleteAll->setEnabled(true);
                            } else {
                                d->actionRemoveAll->setEnabled(false);
                                d->actionDeleteAll->setEnabled(false);
                            }
                        } else if (observer->displayHints()->observerSelectionContextHint() == ObserverHints::SelectionUseParentContext && selectionParent()) {
                            if (selectionParent()->subjectCount() > 0) {
                                d->actionRemoveAll->setText(tr("Detach All Under \"") + observer->observerName() + "\"");
                                d->actionRemoveAll->setEnabled(true);
                                d->actionDeleteAll->setText(tr("Delete All Under: ") + observer->observerName() + "\"");
                                d->actionDeleteAll->setEnabled(true);
                            } else {
                                d->actionRemoveAll->setEnabled(false);
                                d->actionDeleteAll->setEnabled(false);
                            }
                        }
                    }

                    // Can only delete or remove an item if its category is not const:
                    QtilitiesCategory category = d_observer->getMultiContextPropertyValue(d->current_selection.at(0),qti_prop_CATEGORY_MAP).value<QtilitiesCategory>();
                    Observer::AccessMode access_mode = d_observer->accessMode(category);
                    if (access_mode == Observer::ReadOnlyAccess || access_mode == Observer::LockedAccess) {
                        d->actionDeleteItem->setEnabled(false);
                        d->actionRemoveItem->setEnabled(false);
                    }
                }
            } else if (d->current_selection.count() > 1) {
                // Can only delete or remove items if categories of all selected objects are not const:
                bool has_const_category = false;
                if (!d_observer) {
                    foreach (QObject* obj, d->current_selection) {
                        if (obj) {
                            QtilitiesCategory category = d_observer->getMultiContextPropertyValue(obj,qti_prop_CATEGORY_MAP).value<QtilitiesCategory>();
                            Observer::AccessMode access_mode = d_observer->accessMode(category);
                            if (access_mode == Observer::ReadOnlyAccess || access_mode == Observer::LockedAccess) {
                                has_const_category = true;
                                break;
                            }
                        }
                    }
                }

                // We don't need to check if the selected objects share the same parent in TableView mode, it will always be true:
                Observer* obs = selectionParent();
                if (obs) {
                    d->actionRemoveAll->setEnabled(!has_const_category);
                    d->actionDeleteAll->setEnabled(!has_const_category);
                    d->actionSwitchView->setEnabled(true);
                } else {
                    d->actionRemoveAll->setEnabled(false);
                    d->actionDeleteAll->setEnabled(false);
                    d->actionSwitchView->setEnabled(false);
                }

                d->actionDeleteItem->setEnabled(!has_const_category);
                d->actionRemoveItem->setEnabled(!has_const_category);
                d->actionDeleteItem->setText("Delete Current Selection");
                d->actionRemoveItem->setText("Detach Current Selection");
            }
        } else if (d->display_mode == TreeView) {
            if (d->current_selection.count() > 1) {
                // Can only delete or remove items if categories of all selected objects are not const:
                bool has_const_category = false;
                if (!d_observer) {
                    foreach (QObject* obj, d->current_selection) {
                        QtilitiesCategory category = d_observer->getMultiContextPropertyValue(obj,qti_prop_CATEGORY_MAP).value<QtilitiesCategory>();
                        Observer::AccessMode access_mode = d_observer->accessMode(category);
                        if (access_mode == Observer::ReadOnlyAccess || access_mode == Observer::LockedAccess) {
                            has_const_category = true;
                            break;
                        }
                    }
                }

                // We check if the selected objects share the same parent, if so we enable delete and detach all actions:
                if (selectedObjectsContextMatch()) {
                    Observer* obs = selectionParent();
                    if (obs) {
                        d->actionRemoveAll->setEnabled(!has_const_category);
                        d->actionDeleteAll->setEnabled(!has_const_category);
                        d->actionRemoveAll->setText(tr("Detach All Under \"") + obs->observerName() + "\"");
                        d->actionDeleteAll->setText(tr("Delete All Under \"") + obs->observerName() + "\"");
                        d->actionSwitchView->setEnabled(true);
                    } else {
                        d->actionRemoveAll->setEnabled(false);
                        d->actionDeleteAll->setEnabled(false);
                        d->actionSwitchView->setEnabled(false);
                    }
                } else {
                    d->actionRemoveAll->setEnabled(false);
                    d->actionDeleteAll->setEnabled(false);
                    d->actionRemoveAll->setText(tr("Multiple Contexts Selected, Cannot Detach All"));
                    d->actionDeleteAll->setText(tr("Multiple Contexts Selected, Cannot Delete All"));
                    d->actionSwitchView->setEnabled(false);
                }             

                d->actionDeleteItem->setEnabled(!has_const_category);
                d->actionDeleteItem->setText("Delete Current Selection");

                if (selectedObjectsContextMatch()) {
                    d->actionRemoveItem->setEnabled(true);
                    d->actionRemoveItem->setText("Detach Current Selection");
                } else {
                    d->actionRemoveItem->setEnabled(false);
                    d->actionRemoveItem->setText("Detach Current Selection Not Allowed For Multiple Parents");
                }

                // Next, check if the root observer is part of the selection, in that case we can't remove or delete it:
                foreach (QObject* obj, d->current_selection) {
                    if (obj == d->top_level_observer) {
                        d->actionRemoveAll->setEnabled(false);
                        d->actionDeleteAll->setEnabled(false);
                        d->actionDeleteItem->setEnabled(false);
                        d->actionRemoveItem->setEnabled(false);
                    }
                }
            } else if (d->current_selection.count() == 1) {
                // Give the remove and delete item actions better text:
                if (d->current_selection.front()) {
                    Observer* parent_obs = selectionParent();
                    if (parent_obs) {
                        d->actionDeleteItem->setText("Delete \"" + parent_obs->subjectNameInContext(d->current_selection.front()) + "\"");
                        d->actionRemoveItem->setText("Detach \"" + parent_obs->subjectNameInContext(d->current_selection.front()) + "\"");
                    } else {
                        d->actionDeleteItem->setText("Delete \"" + d->current_selection.front()->objectName() + "\"");
                        d->actionRemoveItem->setText("Detach \"" + d->current_selection.front()->objectName() + "\"");
                    }
                }

                // We can't delete the top level observer in a tree:
                Observer* selected = qobject_cast<Observer*> (d->current_selection.front());
                if (selected == d->top_level_observer) {
                    d->actionDeleteItem->setEnabled(false);
                    d->actionRemoveItem->setEnabled(false);
                } else {
                    d->actionDeleteItem->setEnabled(true);
                    d->actionRemoveItem->setEnabled(true);
                }

                // Can only delete or remove an item if it's category is not const:
                if (selectionParent() && d->current_selection.at(0)) {
                    QtilitiesCategory category = selectionParent()->getMultiContextPropertyValue(d->current_selection.at(0),qti_prop_CATEGORY_MAP).value<QtilitiesCategory>();
                    Observer::AccessMode access_mode = selectionParent()->accessMode(category);
                    if (access_mode == Observer::ReadOnlyAccess || access_mode == Observer::LockedAccess) {
                        d->actionDeleteItem->setEnabled(false);
                        d->actionRemoveItem->setEnabled(false);
                    } else {
                        d->actionDeleteItem->setEnabled(true);
                        d->actionRemoveItem->setEnabled(true);
                    }
                }

                // Delete All and Remove All depends on the selection context if it is an observer:
                if (selected) {
                    // We need to check a few things things:
                    // 1. Do we use the observer hints?
                    // 2. If not, does the selection have hints?
                    ObserverHints* hints_to_use_for_selection = 0;
                    if (d->use_observer_hints && selected->displayHints())
                        hints_to_use_for_selection = selected->displayHints();
                    else if (!d->use_observer_hints)
                        hints_to_use_for_selection = activeHints();

                    // We should not end up with not getting any hints.
                    if (!hints_to_use_for_selection) {
                        d->actionRemoveAll->setText(tr("Detach All In Current Context"));
                        d->actionDeleteAll->setText(tr("Delete All In Current Context"));
                        d->actionRemoveAll->setEnabled(true);
                        d->actionDeleteAll->setEnabled(true);
                    } else {
                        if (hints_to_use_for_selection->observerSelectionContextHint() == ObserverHints::SelectionUseSelectedContext) {
                            if (selected->subjectCount() > 0) {
                                d->actionRemoveAll->setText(tr("Detach All Under \"") + selected->observerName() + "\"");
                                d->actionRemoveAll->setEnabled(true);
                                d->actionDeleteAll->setText(tr("Delete All Under \"") + selected->observerName() + "\"");
                                d->actionDeleteAll->setEnabled(true);
                            } else {
                                d->actionRemoveAll->setEnabled(false);
                                d->actionDeleteAll->setEnabled(false);
                            }
                        } else if (hints_to_use_for_selection->observerSelectionContextHint() == ObserverHints::SelectionUseParentContext && selectionParent()) {
                            d->actionRemoveAll->setText(tr("Detach All Under \"") + selectionParent()->observerName() + "\"");
                            d->actionRemoveAll->setEnabled(true);
                            d->actionDeleteAll->setText(tr("Delete All Under  \"") + selectionParent()->observerName() + "\"");
                            d->actionDeleteAll->setEnabled(true);
                        }
                    }
                } else {
                    if (selectionParent()) {
                        d->actionRemoveAll->setText(tr("Detach All Under \"") + selectionParent()->observerName() + "\"");
                        d->actionDeleteAll->setText(tr("Delete All Under \"") + selectionParent()->observerName() + "\"");
                    } else {
                        d->actionRemoveAll->setText(tr("Detach All In Current Context"));
                        d->actionDeleteAll->setText(tr("Delete All In Current Context"));
                    }
                    d->actionRemoveAll->setEnabled(true);
                    d->actionDeleteAll->setEnabled(true);
                }

                // Check if any categories are selected:
                for (int i = 0; i < d->current_tree_item_selection.count(); i++) {
                    if (d->current_tree_item_selection.at(i)) {
                        if (d->current_tree_item_selection.at(i)) {
                            if (d->current_tree_item_selection.at(i)->itemType() == ObserverTreeItem::CategoryItem) {
                                d->actionDeleteItem->setEnabled(false);
                                d->actionRemoveItem->setEnabled(false);
                                d->actionDeleteAll->setEnabled(false);
                                d->actionRemoveAll->setEnabled(false);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // For now just make stuff read only here at the end. This can probably be optimized in the future:
    if (d->read_only) {
        d->actionRemoveAll->setEnabled(false);
        d->actionDeleteAll->setEnabled(false);
        d->actionDeleteItem->setEnabled(false);
        d->actionRemoveItem->setEnabled(false);
        d->actionNewItem->setEnabled(false);
        d->actionSwitchView->setEnabled(false);
        d->actionRefreshView->setEnabled(false);
    }

    refreshActionToolBar();
}

void Qtilities::CoreGui::ObserverWidget::setTreeSelectionParent(Observer* observer) {
    if (d->display_mode == Qtilities::TreeView && d->tree_view) {              
        // This function will only be entered in TreeView mode.
        // It is a slot connected to the selection parent changed signal in the tree model.
        bool valid_selection_parent_hints = true;
        //qDebug() << "Selection parent received:" << observer;

        // We will typically get an invalid observer when:
        // - The selection does not have a parent (root)
        // - The selection has multiple observer parents.
        if (!observer) {
            // Handle cases where multiple items are selected.
            if (d->current_selection.count() > 1 && usesObserverHints()) {
                if (!selectedObjectsHintsMatch()) {
                    valid_selection_parent_hints = false;
                    qWarning() << "Current selection has multiple parents with different observer hints. ObserverWidget cannot determine the hints to use in this situation. Reverting to observer hints set on the observer widget itself. Widget name:" << contextString() << ", Context:" << d_observer;
                }
            }

            observer = d->top_level_observer;
        }

        // Do some hints debugging (note this debugging code does not cater for multiple selections yet:
    //    if (observer) {
    //        qDebug() << "New selection parent: " << observer << " with display hints. Valid selection parent:" << valid_selection_parent_hints;
    //        if (observer->displayHints()) {
    //            ObserverHints::DisplayFlags flags = observer->displayHints()->displayFlagsHint();
    //            qDebug() << flags;
    //        }
    //    } else {
    //        qDebug() << "New selection parent: " << observer << " with NO display hints.";
    //    }

        setObserverContext(observer);
        // We set d->update_selection_activity to false in here since we don't want an initial selection
        // to be created in initialize()
        d->update_selection_activity = false;

        // If we did not get a valid parent, we revert to the hints set on the widget and log an error message.
        //qDebug() << "Disabling observer hints" << !valid_selection_parent_hints;
        bool change_obs_hints = false;
        bool current_uses_observer_hints;
        if (usesObserverHints()) {
            change_obs_hints = true;
            current_uses_observer_hints = usesObserverHints();
            toggleUseObserverHints(valid_selection_parent_hints);
        }

        initialize(true);

        if (change_obs_hints) {
            if (!valid_selection_parent_hints)
                toggleUseObserverHints(current_uses_observer_hints);
        }

        d->update_selection_activity = true;

        // We need to look at the current selection parent if in tree mode, otherwise in table mode we use d_observer:
        if (d->update_selection_activity && observer && valid_selection_parent_hints) {
            // Check if the observer has a FollowSelection activity policy
            if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
                // Check if the observer has a activity filter, which it should have with this hint
                ActivityPolicyFilter* filter = 0;

                QList<QPointer<QObject> > active_subjects = d->current_selection;
                active_subjects.removeOne(observer);

                for (int i = 0; i < observer->subjectFilters().count(); i++) {
                    filter = qobject_cast<ActivityPolicyFilter*> (observer->subjectFilters().at(i));
                    if (filter) {  
                        // We set d->update_selection_activity to false in here since we don't want selectObjects()
                        // to select the objects again. We will get in this slot when the user already made a new
                        // selection and we do not want to go into an endless loop.
                        d->update_selection_activity = false;
                        filter->setActiveSubjects(active_subjects);
                        d->update_selection_activity = true;

                        break;
                    }
                }
            }
        }

        d->tree_name_column_delegate->setObserverContext(observer);
        #ifdef QTILITIES_PROPERTY_BROWSER
        refreshPropertyBrowser();
        refreshDynamicPropertyBrowser();
        #endif
    }
}

void Qtilities::CoreGui::ObserverWidget::selectionDetach() {
    selectionRemoveItems(false);
}

void Qtilities::CoreGui::ObserverWidget::selectionDetachAll() {
    selectionRemoveAll(false);
}

void Qtilities::CoreGui::ObserverWidget::selectionRemoveItems(bool delete_items) {
    if (!d->initialized)
        return;

    // Detach selected objects:
    int first_delete_position = -1;
    if (d->display_mode == TableView && d->table_model) {
        d_observer->startProcessingCycle();
        for (int i = 0; i < d->current_selection.count(); i++) {
            if (i == 0)
                first_delete_position = d_observer->subjectReferences().indexOf(d->current_selection.at(i));
            if (delete_items)
                delete d->current_selection.at(i);
            else
                d_observer->detachSubject(d->current_selection.at(i));
        }

        if (first_delete_position <= d_observer->subjectCount() && first_delete_position > 0) {
            QList<QObject*> object_list;
            object_list << d_observer->subjectAt(first_delete_position-1);
            selectObjects(object_list);
        } else {
            clearSelection();
        }
        d_observer->endProcessingCycle();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (d->current_selection.count() == 0)
            return;
        else if (d->current_selection.count() == 1) {
            // Make sure the selected object is not the top level observer (might happen in a tree view)
            Observer* observer = qobject_cast<Observer*> (d->current_selection.front());
            if (observer == d->top_level_observer) {
                return;
            }

            Observer* selection_parent = d->tree_model->selectionParent();
            if (selection_parent) {
                first_delete_position = selection_parent->subjectReferences().indexOf(d->current_selection.front());
                if (delete_items)
                    delete d->current_selection.front();
                else
                    selection_parent->detachSubject(d->current_selection.front());
            } else if (!selection_parent && d_observer) {
                first_delete_position = d_observer->subjectReferences().indexOf(d->current_selection.front());
                if (delete_items)
                    delete d->current_selection.front();
                else
                    d_observer->detachSubject(d->current_selection.front());
            }

            // We must check if there is a selection parent, else use d_observer:
            QList<QObject*> object_list;
            if (selection_parent) {
                if (first_delete_position <= selection_parent->subjectCount() && first_delete_position > 0) {
                    object_list << selection_parent->subjectAt(first_delete_position-1);
                } else {
                    if (selection_parent->subjectCount() > 0)
                        object_list << selection_parent->subjectAt(0);
                    else
                        object_list << selection_parent;
                }
                selectObjects(object_list);
            } else {
                if (first_delete_position <= d_observer->subjectCount() && first_delete_position > 0) {
                    object_list << d_observer->subjectAt(first_delete_position-1);
                } else {
                    if (d_observer->subjectCount() > 0)
                        object_list << d_observer->subjectAt(0);
                    else
                        object_list << d_observer;
                }
                selectObjects(object_list);
            }

            object_list.removeOne(selection_parent);

            // Check if the new selection must become active:
            if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
                d->update_selection_activity = false;
                if (selection_parent)
                    selection_parent->refreshViewsLayout();
                if (d->activity_filter)
                    d->activity_filter->setActiveSubjects(object_list);
                d->update_selection_activity = true;
            }
        } else if (d->current_selection.count() > 1) {
            if (selectedObjectsContextMatch()) {
                Observer* selection_parent = d->tree_model->selectionParent();
                if (selection_parent) {
                    selection_parent->startProcessingCycle();
                    first_delete_position = selection_parent->subjectReferences().indexOf(d->current_selection.front());
                    foreach (QObject* obj, d->current_selection)  {
                        if (delete_items) {
                            if (obj)
                                delete obj;
                        } else
                            selection_parent->detachSubject(obj);
                    }
                    selection_parent->endProcessingCycle();
                } else if (!selectionParent() && d_observer) {
                    d_observer->startProcessingCycle();
                    first_delete_position = d_observer->subjectReferences().indexOf(d->current_selection.front());
                    foreach (QObject* obj, d->current_selection)  {
                        if (delete_items) {
                            if (obj)
                                delete obj;
                        } else
                            d_observer->detachSubject(obj);
                    }
                    d_observer->endProcessingCycle();
                }

                // We must check if there is a selection parent, else use d_observer:
                QList<QObject*> object_list;
                if (selection_parent) {
                    if (first_delete_position <= selection_parent->subjectCount() && first_delete_position > 0) {
                        object_list << selection_parent->subjectAt(first_delete_position-1);
                    } else {
                        if (selection_parent->subjectCount() > 0)
                            object_list << selection_parent->subjectAt(0);
                        else
                            object_list << selection_parent;
                    }
                    selectObjects(object_list);
                } else {
                    if (first_delete_position <= d_observer->subjectCount() && first_delete_position > 0) {
                        object_list << d_observer->subjectAt(first_delete_position-1);
                    } else {
                        if (d_observer->subjectCount() > 0)
                            object_list << d_observer->subjectAt(0);
                        else
                            object_list << d_observer;
                    }
                    selectObjects(object_list);
                }
            } else {
                // We don't support detachment of multiple items if they don't have the same parent:
                if (delete_items) {
                    QList<QObject*> object_list;
                    // In this case multiple parents are selected, thus we start the processing cycle on the top level item.
                    d_observer->startTreeProcessingCycle();
                    int count = 0;
                    foreach (QObject* obj, d->current_selection)  {
                        if (count == 0) {
                            // Try to make a clever selection after deletion:
                            QModelIndex index_first = d->tree_model->findObject(obj);
                            if (index_first.isValid()) {
                                Observer* obs = d->tree_model->parentOfIndex(index_first);
                                if (obs) {
                                    first_delete_position = obs->subjectReferences().indexOf(obj);
                                    if (first_delete_position <= obs->subjectCount() && first_delete_position > 0) {
                                        object_list << obs->subjectAt(first_delete_position-1);
                                    } else {
                                        if (obs->subjectCount() > 0)
                                            object_list << obs->subjectAt(0);
                                        else
                                            object_list << obs;
                                    }
                                }
                            }
                        }

                        ++count;
                        if (obj)
                            delete obj;
                    }

                    d_observer->endTreeProcessingCycle(false);
                    d_observer->refreshViewsLayout();
                    selectObjects(object_list);
                }
            }
        }
    }

    refreshActions();
}

void Qtilities::CoreGui::ObserverWidget::selectionRemoveAll(bool delete_all) {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        if (delete_all)
            d_observer->deleteAll();
        else
            d_observer->detachAll();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (d->current_selection.count() == 0)
            return;
        else if (d->current_selection.count() == 1) {
            // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed:
            Observer* obs = qobject_cast<Observer*> (d->current_selection.front());
            if (obs) {
                if (obs->displayHints()) {
                    if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                        // Do nothing, obs will be the selected object here.
                    } else if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseParentContext) {
                        obs = d->tree_model->selectionParent();
                        if (!obs)
                            obs = d_observer;
                    }
                }
            } else
                obs = d->tree_model->selectionParent();

            if (obs) {
                if (delete_all)
                    obs->deleteAll();
                else
                    obs->detachAll();
                QList<QObject*> object_list;
                object_list << obs;
                selectObjects(object_list);
            }
        } else if (d->current_selection.count() > 1) {
            // ObserverSelectionContext is not respected for multiple selections:
            // Only do something when multiple selections are present:
            if (selectedObjectsContextMatch()) {
                // We must check if there is an selection parent, else use d_observer
                Observer* obs = d->tree_model->selectionParent();
                if (!obs)
                    obs = d_observer;

                if (delete_all)
                    obs->deleteAll();
                else
                    obs->detachAll();
                QList<QObject*> object_list;
                object_list << obs;
                selectObjects(object_list);
            }
        }
    }

    refreshActions();
}

void Qtilities::CoreGui::ObserverWidget::handleLayoutChangeCompleted() {
    if (d->display_mode == Qtilities::TableView) {
        resizeColumns();
        initialize(true);
    }
}

void Qtilities::CoreGui::ObserverWidget::selectionDelete() {
    if (!d->initialized)
        return;

    int selected_count = selectedObjects().count();
    if (selected_count == 0)
        return;

    // Check if any categories were selected:
    /*if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        for (int i = 0; i < d->current_selection.count(); i++) {
            if (d->current_selection.at(i)) {
                ObserverTreeItem* tree_item = qobject_cast<ObserverTreeItem*> (d->current_selection.at(i));
                if (tree_item) {
                    if (tree_item->itemType() == ObserverTreeItem::CategoryItem) {
                        return;
                    }
                }
            }
        }
    }*/

    if (d->confirm_deletes) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle(tr("Confirm Deletion"));
        if (selected_count == 1)
            msgBox.setText(tr("Are you sure you want to delete the selected object?<br><br>This operation cannot be undone."));
        else
            msgBox.setText(tr("Are you sure you want to delete the selected objects?<br><br>This operation cannot be undone."));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Cancel:
              return;
              break;
          default:
              // should never be reached
              break;
        }
    }

    QApplication::processEvents();
    selectionRemoveItems(true);
}

void Qtilities::CoreGui::ObserverWidget::selectionDeleteAll() {
    if (!d->initialized)
        return;

    if (d->confirm_deletes) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Confirm Deletion"));
        msgBox.setText(tr("Are you sure you want to delete the selected object(s)?<br><br>This operation cannot be undone."));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Cancel:
              return;
              break;
          default:
              // should never be reached
              break;
        }
    }

    QApplication::processEvents();
    selectionRemoveAll(true);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionNewItem_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        if (d->current_selection.count() == 1) {
            // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed:
            bool use_parent_context = true;
            Observer* obs = qobject_cast<Observer*> (d->current_selection.front());
            if (obs) {
                if (obs->displayHints()) {
                    if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                        emit addActionNewItem_triggered(0, obs);
                        use_parent_context = false;
                    }
                }
            }

            if (use_parent_context)
                emit addActionNewItem_triggered(d->current_selection.front(), d_observer);
        } else if (d->current_selection.count() > 1) {
            emit addActionNewItem_triggered(0, d_observer);
        } else if (d->current_selection.count() == 0) {
            // Check if the stack contains a parent for the current d_observer
            if (!d->navigation_stack.isEmpty()) {
                // Get observer from stack
                Observer* selection_parent = OBJECT_MANAGER->observerReference(navigationStack().last());
                emit addActionNewItem_triggered(d_observer, selection_parent);
            } else
                emit addActionNewItem_triggered(d_observer, 0);
        }
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (d->current_selection.count() == 1 && d->tree_view->selectionModel()->selectedIndexes().count() > 0) {
            // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
            bool use_parent_context = true;
            Observer* obs = qobject_cast<Observer*> (d->current_selection.front());
            if (obs) {
                if (obs->displayHints()) {
                    if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                        emit addActionNewItem_triggered(0, obs);
                        use_parent_context = false;
                    }
                }
            }

            if (use_parent_context)
                emit addActionNewItem_triggered(d->current_selection.front(), d->tree_model->selectionParent());
        } else if (d->current_selection.count() > 1)
            emit addActionNewItem_triggered(0, d->tree_model->selectionParent());
        else if (d->current_selection.count() == 0)
            emit addActionNewItem_triggered(0, d_observer);
        else
            emit addActionNewItem_triggered(0, d_observer);
    }
}

void Qtilities::CoreGui::ObserverWidget::refresh() {
    if (!d->initialized || !d_observer)
        return;

    // Call selectObjects() in order to update internal d->current_selection.
    QList<QObject*> previous_selection = selectedObjects();

    // Call refresh on the applicable model:
    if (displayMode() == Qtilities::TreeView && d->tree_model)
        d->tree_model->refresh();
    else if (displayMode() == Qtilities::TableView && d->table_model)
        d->table_model->refresh();
    //d_observer->refreshViewsLayout(QList<QPointer<QObject> >(),true);

    // Handle observer's follow selection stuff
    if (activeHints()->activityControlHint() == ObserverHints::FollowSelection && d->activity_filter) {
        selectObjects(d->activity_filter->activeSubjects());
    } else {
        if (displayMode() == Qtilities::TreeView) {
            // We don't need to do anything here. refreshViewsLayout already selected what was needed above in tree view mode.
        } else if (displayMode() == Qtilities::TableView)
            selectObjects(previous_selection);
    }

    setWindowTitle(d_observer->observerName());
    if (d->navigation_bar && d->display_mode == Qtilities::TableView)
        d->navigation_bar->refreshHierarchy();

    resizeColumns();
}

void Qtilities::CoreGui::ObserverWidget::selectionPushUp() {
    if (!d->initialized)
        return;

    // Get the parent observer, this will be stored in the navigation stack
    if (d->navigation_stack.count() == 0)
        return;

    // First disconnet all current observer connections
    d_observer->disconnect(this);
    d_observer->disconnect(d->navigation_bar);

    // Get the current observer in order to select it after we pushed up:
    QObject* new_selection = d_observer;

    // Setup new observer
    Observer* observer = OBJECT_MANAGER->observerReference(d->navigation_stack.pop());
    Q_ASSERT(observer);

    setObserverContext(observer);
    initialize();
    selectObject(new_selection);

    resizeColumns();
}

void Qtilities::CoreGui::ObserverWidget::selectionPushUpNew() {
    if (!d->initialized)
        return;

    // Get the parent observer, this will be stored in the navigation stack
    if (d->navigation_stack.count() == 0)
        return;

    Observer* observer = OBJECT_MANAGER->observerReference(d->navigation_stack.front());
    if (!observer)
        return;

    ObserverWidget* new_child_widget = new ObserverWidget(d->display_mode);
    QStack<int> tmp_stack = d->navigation_stack;
    tmp_stack.pop();
    new_child_widget->setNavigationStack(tmp_stack);
    new_child_widget->setObserverContext(observer);
    new_child_widget->initialize();
    if (d->display_mode == TableView && d->table_view) {
        new_child_widget->tableView()->setShowGrid(d->table_view->showGrid());
        new_child_widget->tableView()->setAlternatingRowColors(d->table_view->alternatingRowColors());
    } else if (d->display_mode == TreeView && d->tree_view) {
        new_child_widget->treeView()->setAlternatingRowColors(d->tree_view->alternatingRowColors());
    }
    new_child_widget->setSizePolicy(sizePolicy());
    new_child_widget->show();
    new_child_widget->selectObject(this);

    newObserverWidgetCreated(new_child_widget);
}

void Qtilities::CoreGui::ObserverWidget::selectionPushDown() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView) {
        if (d->current_selection.count() == 0)
            return;

        // Set up new observer
        QObject* obj = d->current_selection.front();
        Observer* observer = qobject_cast<Observer*> (obj);
        if (!observer)
            return;

        // Check the observer's access
        if (observer->accessMode() == Observer::LockedAccess) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Context Access Locked"));
            msgBox.setText(tr("The context you are trying to access is locked."));
            msgBox.exec();
            return;
        }

        time_t start,end;
        time(&start);
        time(&end);

        // First disconnet all current observer connections
        d_observer->disconnect(this);
        d_observer->disconnect(d->navigation_bar);

        // Set up widget to use new observer
        d->navigation_stack.push(d_observer->observerID());
        connect(d_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication,QList<QPointer<QObject> >)),SLOT(contextDetachHandler(Observer::SubjectChangeIndication,QList<QPointer<QObject> >)));

        #ifdef QTILITIES_BENCHMARKING
        double diff = difftime(end,start);
        qDebug() << QString("Before observerwidget init: " + QString::number(diff) + " seconds.");
        #endif

        setObserverContext(observer);
        initialize();

        #ifdef QTILITIES_BENCHMARKING
        diff = difftime(end,start);
        qDebug() << QString("After observerwidget init: " + QString::number(diff) + " seconds.");
        #endif

        if (observer->subjectCount() > 0)
            selectObject(observer->subjectAt(0));

        resizeColumns();
    } /*else if (d->display_mode == TreeView) {
        if (d->current_selection.count() == 1) {
            Observer* observer = qobject_cast<Observer*> (d->current_selection.front());
            if (observer) {
                // Check the observer's access
                if (observer->accessMode() == Observer::LockedAccess) {
                    QMessageBox msgBox;
                    msgBox.setText("The context you are trying to access is locked.");
                    msgBox.exec();
                    return;
                }

                // In this case we must set up the navigation stack properly
                // Don't use the setNavigationStack() function, it only works pre-initialization.
                d->navigation_stack = d->tree_model->getParentHierarchy(selectedIndexes().front());
                setObserverContext(observer);
                d->display_mode = TableView;
                initialize();
            }
        }
    }*/
}

void Qtilities::CoreGui::ObserverWidget::selectionPushDownNew() {
    if (!d->initialized)
        return;

    Observer* observer = 0;
    ObserverWidget* new_child_widget = 0;

    if (d->display_mode == TableView) {
        if (d->current_selection.count() == 0)
            return;

        // Set up new observer
        QObject* obj = d->current_selection.front();
        observer = qobject_cast<Observer*> (obj);
        if (!observer)
            return;

        // Check the observer's access
        if (observer->accessMode() == Observer::LockedAccess) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Context Access Locked"));
            msgBox.setText(tr("The context you are trying to access is locked."));
            msgBox.exec();
            return;
        }

        d->navigation_stack.push(d_observer->observerID());
        new_child_widget = new ObserverWidget(d->display_mode);
        new_child_widget->setNavigationStack(d->navigation_stack);

        // Pop the new ID here since this window stays the same
        d->navigation_stack.pop();  
    } /*else if (d->display_mode == TreeView) {
        if (d->current_selection.count() == 1) {
            observer = qobject_cast<Observer*> (d->current_selection.front());
            if (observer) {
                // Check the observer's access
                if (observer->accessMode() == Observer::LockedAccess) {
                    QMessageBox msgBox;
                    msgBox.setText("The context you are trying to access is locked.");
                    msgBox.exec();
                    return;
                }

                new_child_widget = new ObserverWidget(TableView);
                // In this case we must set up the navigation stack properly
                // Don't use the setNavigationStack() function, it only works pre-initialization.
                new_child_widget->setNavigationStack(d->tree_model->getParentHierarchy(selectedIndexes().front()));
            } else {
                //delete new_child_widget;
                return;
            }
            new_child_widget->toggleAlternatingRowColors(d->tree_view->alternatingRowColors());
        } else
            return;
    }*/

    new_child_widget->setObserverContext(observer);
    new_child_widget->initialize();
    new_child_widget->tableView()->setShowGrid(d->table_view->showGrid());
    new_child_widget->tableView()->setAlternatingRowColors(d->table_view->alternatingRowColors());
    new_child_widget->setSizePolicy(sizePolicy());
    new_child_widget->show();
    new_child_widget->selectObjects(selectedObjects());
    newObserverWidgetCreated(new_child_widget);   
}

void Qtilities::CoreGui::ObserverWidget::toggleDisplayMode() {
    if (!d->initialized)
        return;

    QList<QObject*> selected_objects = selectedObjects();
    if (d->display_mode == TableView) {
        if (d->table_model)
            d->table_model->setRespondToObserverChanges(false);
        if (d->tree_model)
            d->tree_model->setRespondToObserverChanges(true);

        d->display_mode = TreeView;
        // We need to clear the navigation stack if we move to tree view. The tree view will set the stack
        // automatically depending on what is selected when switching back to table view.
        // setNavigationStack(QStack<int>());
        setObserverContext(d->top_level_observer);
        d->update_selection_activity = false;
        initialize();

        d->update_selection_activity = true;
        selectObjects(selected_objects);

        // Update the selection parent:
        // calculateSelectionParent() will update the parent and set the hints according to the parent.
        if (!selectedIndexes().isEmpty()) {
            if (d->tree_model)
                d->tree_model->calculateSelectionParent(selectedIndexes());
        }

        if (d->tree_view)
            d->tree_view->setFocus();
        if (d->searchBoxWidget)
            handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
    } else if (d->display_mode == TreeView && d->tree_model) {
        if (d->table_model)
            d->table_model->setRespondToObserverChanges(true);
        if (d->tree_model)
            d->tree_model->setRespondToObserverChanges(false);

        QModelIndexList selected_indexes = selectedIndexes();
        d->tree_model->calculateSelectionParent(selected_indexes);
        if (selected_indexes.count() > 0) {
            // Check if the navigation stack is the same for all items. If not, we switch to the root observer:
            QStack<int> stack;
            for (int i = 0; i < selected_indexes.count(); i++) {
                if (stack.isEmpty()) {
                    stack = d->tree_model->getParentHierarchy(selected_indexes.at(i));
                } else {
                    QStack<int> tmp_stack = d->tree_model->getParentHierarchy(selected_indexes.at(i));
                    if (stack != tmp_stack) {
                        // We need to clear the stack and show the root observer:
                        qDebug() << "Selection has different parents, will load the root observer context after switched.";
                        d->navigation_stack.clear();
                    } else {
                        stack = tmp_stack;
                    }
                }
            }

            d->navigation_stack = stack;
            if (!d->navigation_stack.isEmpty()) {
                bool pop_back = true;

                // We only check ObserverHints::SelectionUseSelectedContext when there is a single selection:
                if (selected_indexes.count() == 1){
                    Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selected_indexes.front()));
                    if (obs) {
                        // We need to check a few things things:
                        // 1. Do we use the observer hints?
                        // 2. If not, does the selection have hints?
                        ObserverHints* hints_to_use_for_selection = 0;
                        if (d->use_observer_hints && obs->displayHints())
                            hints_to_use_for_selection = obs->displayHints();
                        else if (!d->use_observer_hints)
                            hints_to_use_for_selection = activeHints();

                        if (hints_to_use_for_selection) {
                            // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
                            if (hints_to_use_for_selection->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                                setObserverContext(obs);
                                pop_back = false;
                            }
                        }
                    }
                }

                if (d->navigation_stack.count() > 0 && pop_back)
                    d->navigation_stack.pop_back();
            }
        } else {
            d->navigation_stack.clear();
        }

        ui->widgetProgressInfo->setVisible(false);
        d->display_mode = TableView;
//        qDebug() << "Obs context before:" << observerContext() << "Stack" << d->navigation_stack;
//        foreach (int id, d->navigation_stack) {
//            Observer* tmp = OBJECT_MANAGER->observerReference(id);
//            if (tmp)
//                qDebug() << tmp->observerName();
//        }
        // Initialize takes care of selecting the correct object according to the navigation stack.
        initialize();
        //qDebug() << "Obs context after:" << observerContext() << "Stack" << d->navigation_stack;
        if (d->table_view)
            d->table_view->setFocus();

        if (d->searchBoxWidget)
            handleSearchStringChanged(d->searchBoxWidget->currentSearchString());

        selectObjects(selected_objects);
    }

    emit displayModeChanged(d->display_mode);
}

void Qtilities::CoreGui::ObserverWidget::viewCollapseAll() {
    if (d->tree_view && d->display_mode == TreeView) {
        d->tree_view->expandToDepth(0);
        //qDebug() << "Collapsing all";
        resizeColumns();
    }
}

void Qtilities::CoreGui::ObserverWidget::viewExpandAll() {
    if (d->tree_view && d->display_mode == TreeView) {
        d->tree_view->expandAll();
        //qDebug() << "Expanding all";
        resizeColumns();
    }
}

void Qtilities::CoreGui::ObserverWidget::selectionCopy() {
    ObserverMimeData *mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID(),Qt::CopyAction);
    QApplication::clipboard()->setMimeData(mimeData);
    CLIPBOARD_MANAGER->setClipboardOrigin(IClipboard::CopyAction);
}

void Qtilities::CoreGui::ObserverWidget::selectionCut() {
    if (d->read_only)
        return;

    ObserverMimeData *mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID(),Qt::MoveAction);
    QApplication::clipboard()->setMimeData(mimeData);
    CLIPBOARD_MANAGER->setClipboardOrigin(IClipboard::CutAction);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionPaste_triggered() {
    if (d->read_only)
        return;

    if (activeHints()->actionHints() & ObserverHints::ActionPasteItem) {
        // Check if the subjects being dropped are of the same type as the destination observer.
        // If this is not the case, we do not allow the drop.
        const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (QApplication::clipboard()->mimeData());
        QString error_msg;
        if (observer_mime_data) {
            if (d_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data),&error_msg) == Observer::Allowed) {
                // Now check the proposed action of the event.
                if (CLIPBOARD_MANAGER->clipboardOrigin() == IClipboard::CutAction) {
                    OBJECT_MANAGER->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),d_observer->observerID());
                    CLIPBOARD_MANAGER->acceptMimeData();
                } else if (CLIPBOARD_MANAGER->clipboardOrigin() == IClipboard::CopyAction) {
                    // Attempt to copy the objects:
                    // For now we discard objects that cause problems during attachment and detachment:
                    for (int i = 0; i < observer_mime_data->subjectList().count(); i++) {
                        // Attach to destination
                        d_observer->attachSubject(observer_mime_data->subjectList().at(i));
                    }
                    CLIPBOARD_MANAGER->acceptMimeData();
                }
            } else {
                QMessageBox msgBox;
                msgBox.setText(tr("Paste Operation Failed."));
                msgBox.setInformativeText(tr("The paste operation could not be completed. The destination observer could not accept all the objects in your selection.\n\nError message: ") + error_msg + tr("\n\nDo you want to keep the data in the clipboard for later usage?"));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int ret = msgBox.exec();

                switch (ret) {
                  case QMessageBox::No:
                      CLIPBOARD_MANAGER->acceptMimeData();
                      break;
                  case QMessageBox::Yes:
                      break;
                  default:
                      break;
                }
            }
        } else {
            QMessageBox msgBox;
            msgBox.setText(tr("Paste Operation Failed."));
            msgBox.setInformativeText(tr("The paste operation could not be completed. Unsupported data type for this context.\n\nDo you want to keep the data in the clipboard for later usage?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();

            switch (ret) {
              case QMessageBox::No:
                  CLIPBOARD_MANAGER->acceptMimeData();
                  break;
              case QMessageBox::Yes:
                  break;
              default:
                  break;
            }
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText(tr("Paste Operation Failed."));
        msgBox.setInformativeText(tr("The destination context cannot accept paste operations.\n\nDo you want to keep the data in the clipboard for later usage?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        switch (ret) {
          case QMessageBox::No:
              CLIPBOARD_MANAGER->acceptMimeData();
              break;
          case QMessageBox::Yes:
              break;
          default:
              break;
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::toggleSearchBox() {
    if (!d->initialized)
        return;

    if (d->disable_proxy_models)
        return;

    if (!d->searchBoxWidget) {
        SearchBoxWidget::SearchOptions search_options = 0;
        search_options |= SearchBoxWidget::CaseSensitive;
        search_options |= SearchBoxWidget::RegEx;
        SearchBoxWidget::ButtonFlags button_flags = 0;
        button_flags |= SearchBoxWidget::HideButtonDown;
        d->searchBoxWidget = new SearchBoxWidget(search_options,SearchBoxWidget::SearchOnly,button_flags);
        d->searchBoxWidget->setObjectName("Search Box: Observer Widget (" + objectName() + ")");
        if (ui->widgetSearchBox->layout())
            delete ui->widgetSearchBox->layout();

        QHBoxLayout* layout = new QHBoxLayout(ui->widgetSearchBox);
        layout->addWidget(d->searchBoxWidget);
        layout->setMargin(0);

        connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),SLOT(toggleSearchBox()));

        QMenu* search_options_menu = d->searchBoxWidget->searchOptionsMenu();
        if (search_options_menu) {
            d->actionFilterTypeSeperator = search_options_menu->addSeparator();
            d->actionFilterNodes = new QAction(tr("Filter Nodes"),this);
            d->actionFilterNodes->setCheckable(true);
            connect(d->actionFilterNodes,SIGNAL(triggered()),SLOT(handleSearchItemTypesChanged()));
            search_options_menu->addAction(d->actionFilterNodes);
            d->actionFilterItems = new QAction(tr("Filter Items"),this);
            d->actionFilterItems->setCheckable(true);
            connect(d->actionFilterItems,SIGNAL(triggered()),SLOT(handleSearchItemTypesChanged()));
            search_options_menu->addAction(d->actionFilterItems);
            d->actionFilterCategories = new QAction(tr("Filter Categories"),this);
            d->actionFilterCategories->setCheckable(true);
            connect(d->actionFilterCategories,SIGNAL(triggered()),SLOT(handleSearchItemTypesChanged()));
            search_options_menu->addAction(d->actionFilterCategories);
            if (d->display_mode == TableView) {
                d->actionFilterCategories->setVisible(false);
                d->actionFilterNodes->setVisible(false);
                d->actionFilterItems->setVisible(false);
                d->actionFilterTypeSeperator->setVisible(false);
            }

            setSearchBoxCheckedItemFilters(d->search_item_filter_flags);
        }
        connect(d->searchBoxWidget,SIGNAL(searchOptionsChanged()),SLOT(handleSearchOptionsChanged()));
        connect(d->searchBoxWidget,SIGNAL(searchStringChanged(QString)),SLOT(handleSearchStringChanged(QString)));
    }

    if (!ui->widgetSearchBox->isVisible()) {
        ui->widgetSearchBox->show();
        d->searchBoxWidget->setEditorFocus();
        if (!d->searchBoxWidget->currentSearchString().isEmpty())
            handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
    } else {
        ui->widgetSearchBox->hide();
        if (!d->searchBoxWidget->currentSearchString().isEmpty())
            resetProxyModel();
        if (d->table_view && d->display_mode == TableView) {
            d->table_view->setFocus();
        } else if (d->tree_view && d->display_mode == TreeView) {
            d->tree_view->setFocus();
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::setSearchBoxCheckedItemFilters(Qtilities::CoreGui::ObserverTreeItem::TreeItemTypeFlags search_item_filter_flags) {
    d->search_item_filter_flags = search_item_filter_flags;

    if (d->actionFilterNodes)
        d->actionFilterNodes->setChecked(d->search_item_filter_flags & ObserverTreeItem::TreeNode);
    if (d->actionFilterItems)
        d->actionFilterItems->setChecked(d->search_item_filter_flags & ObserverTreeItem::TreeItem);
    if (d->actionFilterCategories)
        d->actionFilterCategories->setChecked(d->search_item_filter_flags & ObserverTreeItem::CategoryItem);

    if (d->searchBoxWidget)
        handleSearchItemTypesChanged();
}

Qtilities::CoreGui::ObserverTreeItem::TreeItemTypeFlags Qtilities::CoreGui::ObserverWidget::searchBoxCheckedItemFilters() const {
    return d->search_item_filter_flags;
}

void Qtilities::CoreGui::ObserverWidget::handleSearchItemTypesChanged() {
    // Only do something in tree view
    if (d->display_mode == TreeView && d->tree_proxy_model) {
        ObserverTreeModelProxyFilter* proxy = dynamic_cast<ObserverTreeModelProxyFilter*> (d->tree_proxy_model);
        if (proxy) {
            ObserverTreeItem::TreeItemTypeFlags flags = 0;
            if (d->actionFilterNodes->isChecked())
                flags |= ObserverTreeItem::TreeNode;
            if (d->actionFilterItems->isChecked())
                flags |= ObserverTreeItem::TreeItem;
            if (d->actionFilterCategories->isChecked())
                flags |= ObserverTreeItem::CategoryItem;
            proxy->setRowFilterTypes(flags);
            proxy->invalidate();
            resizeColumns();
        }
    }
}

bool Qtilities::CoreGui::ObserverWidget::appendSelectedContexts() const {
    return d->append_selected_contexts;
}

void Qtilities::CoreGui::ObserverWidget::setAppendSelectedContexts(bool enable) {
    d->append_selected_contexts = enable;
}

void Qtilities::CoreGui::ObserverWidget::handleSelectionModelChange() {
    if (!d->initialized)
        return;

    // Set the context backend objects:
    // The selectedObjects() function will set d->current_selection. Thus use this
    // member of the d pointer inside functions in this class since it is much faster
    // than to calculate the selected objects every time we need them.
    QList<QObject*> object_list = selectedObjects();

    // Remove contexts from previous selection
    if (d->append_selected_contexts) {
        int count = d->appended_contexts.count()-1;
        for (int i = count; i >= 0; i--) {
            CONTEXT_MANAGER->removeContext(d->appended_contexts.at(i),false);
            d->appended_contexts.removeAt(i);
        }

        QStringList contexts_in_current_selection;
        // Check if any of the objects implements IContext, if so we append the given context.
        for (int i = 0; i < object_list.count(); i++) {
            IContext* context = qobject_cast<IContext*> (object_list.at(i));
            if (context) {
                if (!d->appended_contexts.contains(context->contextString())) {
                    CONTEXT_MANAGER->appendContext(context->contextString(),false);
                    contexts_in_current_selection << context->contextString();
                    d->appended_contexts << context->contextString();
                }
            }
        }
    }

    if (d->update_selection_activity)
        CONTEXT_MANAGER->broadcastState();

    // Update the global object list
    #ifdef QTILITIES_PROPERTY_BROWSER
    if (!d->property_browser_widget) {
        refreshPropertyBrowser();
        refreshDynamicPropertyBrowser();
    #endif
        updateGlobalActiveSubjects();
    #ifdef QTILITIES_PROPERTY_BROWSER
    }
    #endif

    // If selected objects > 0 and the observer context supports copy & cut, we enable copy/cut
    if (d->current_selection.count() > 0) {
        if (activeHints()->actionHints() & ObserverHints::ActionCopyItem) {
            // ---------------------------
            // Copy
            // ---------------------------
            // The copy action is not a proxy action. It's enabled/disabled depending on the state of the application
            // clipboard. Thus we just connect to the paste action here and check the observer flag in the paste slot.
            Command* command = ACTION_MANAGER->command(qti_action_EDIT_COPY);
            if (command) {
                if (command->action()) {
                    command->action()->setEnabled(true);
                    connect(command->action(),SIGNAL(triggered()),SLOT(selectionCopy()));
                }
            }
        }

        if (activeHints()->actionHints() & ObserverHints::ActionCutItem) {
            // ---------------------------
            // Cut
            // ---------------------------
            // The cut action is not a proxy action. It's enabled/disabled depending on the state of the application
            // clipboard. Thus we just connect to the paste action here and check the observer flag in the paste slot.
            Command* command = ACTION_MANAGER->command(qti_action_EDIT_CUT);
            if (command) {
                if (command->action()) {
                    command->action()->setEnabled(true);
                    connect(command->action(),SIGNAL(triggered()),SLOT(selectionCut()));
                }
            }
        }
    } else {
        disconnectClipboard();
    }

    // Only refresh the property browser in table view mode here. In tree view mode it is refreshed in
    // the setTreeSelectionParent slot.
    Observer* selection_parent = 0;
    if (d->display_mode == TableView) {
        #ifdef QTILITIES_PROPERTY_BROWSER
        refreshPropertyBrowser();
        refreshDynamicPropertyBrowser();
        #endif
        selection_parent = d_observer;

        if (d->activity_filter && d->update_selection_activity) {
            // Check if the observer has a FollowSelection activity policy:
            // We only follow selection in table view mode for now.
            if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
                d->update_selection_activity = false;
                if (!d->activity_filter->setActiveSubjects(object_list)) {
                    // We need to do this selection manually:
                    //qDebug() << "Selection rejected by activity policy filter. Rejecting selection change.";
                    d->update_selection_activity = true;
                    selectObjects(d->activity_filter->activeSubjects());
                    d->update_selection_activity = false;
                }
                d->update_selection_activity = true;
            }
        }

        // Refresh actions
        refreshActions();
        emit selectedObjectsChanged(object_list, d_observer);
    } else if (d->display_mode == TreeView) {
        refreshActions();

        if (d->update_selection_activity)
            selection_parent = d->tree_model->calculateSelectionParent(selectedIndexes());

         // In this case the FollowSelection hint check will happen in the setTreeSelectionParent slot.
         // Refresh actions will happen in the return slot as well.

        emit selectedObjectsChanged(object_list, selection_parent);
    }
}

void Qtilities::CoreGui::ObserverWidget::disconnectClipboard() {
    Command* command = ACTION_MANAGER->command(qti_action_EDIT_COPY);
    if (command) {
        if (command->action()) {
            command->action()->setEnabled(false);
            command->action()->disconnect(this);
        }
    }

    command = ACTION_MANAGER->command(qti_action_EDIT_CUT);
    if (command) {
        if (command->action()) {
            command->action()->setEnabled(false);
            command->action()->disconnect(this);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::updateGlobalActiveSubjects() {
    if (d->update_global_active_objects) {
        QString global_activity_meta_type;
        if (d->shared_global_meta_type.isEmpty())
            global_activity_meta_type = d->global_meta_type;
        else
            global_activity_meta_type = d->shared_global_meta_type;

        // Update the global active object type
        if (selectedObjects().count() == 0) {
            QList<QObject*> this_list;
             this_list << d_observer;
            OBJECT_MANAGER->setMetaTypeActiveObjects(this_list, global_activity_meta_type);
        } else
            OBJECT_MANAGER->setMetaTypeActiveObjects(d->current_selection, global_activity_meta_type);
    }
}

void Qtilities::CoreGui::ObserverWidget::toggleUseGlobalActiveObjects(bool toggle) {
    bool do_update = false;
    if ((toggle != d->update_global_active_objects) && (toggle == true))
        do_update = true;

    d->update_global_active_objects = toggle;

    if (do_update)
        updateGlobalActiveSubjects();
}

bool Qtilities::CoreGui::ObserverWidget::useGlobalActiveObjects() const {
    return d->update_global_active_objects;
}

void Qtilities::CoreGui::ObserverWidget::setConfirmDeletes(bool confirm_deletes) {
    d->confirm_deletes = confirm_deletes;
}

bool Qtilities::CoreGui::ObserverWidget::confirmDeletes() const {
    return d->confirm_deletes;
}

void Qtilities::CoreGui::ObserverWidget::contextDeleted() {
    if (d->display_mode == TreeView){
        if (sender() == d->top_level_observer) {
            d->initialized = false;
            clearSelection();
            deleteActionToolBars();
            d->initialized = true;
            setEnabled(false);
        } else {
            if (!d->top_level_observer) {
                d->initialized = false;
                clearSelection();
                deleteActionToolBars();
                d->initialized = true;
                setEnabled(false);
            } else {
                if (!d->top_level_observer->isProcessingCycleActive()) {
                    setObserverContext(d->top_level_observer);
                    initialize();
                } else {
                    d->initialized = false;
                    clearSelection();
                    deleteActionToolBars();
                    d->initialized = true;
                    setEnabled(false);
                }
            }

            /*d->update_selection_activity = false;
            QItemSelectionModel *selection_model = d->tree_view->selectionModel();
            if (selection_model) {
                QModelIndex root_index = d->tree_model->index(0,0);
                selection_model->clearSelection();
                selection_model->select(root_index,QItemSelectionModel::Select);
            }
            d->update_selection_activity = true;
            */
        }
    } else if (d->display_mode == TableView) {
        d->initialized = false;
        clearSelection();
        deleteActionToolBars();
        d->initialized = true;
        setEnabled(false);
    }
    d->last_display_flags = ObserverHints::NoDisplayFlagsHint;
}

void Qtilities::CoreGui::ObserverWidget::contextDetachHandler(Observer::SubjectChangeIndication indication,QList<QPointer<QObject> > objects) {
    if (indication == Observer::SubjectRemoved) {
        for (int i = 0; i < objects.count(); i++) {
            Observer* observer = qobject_cast<Observer*> (objects.at(i));
            if (observer) {
                // Check the observer ID against all the stack items.
                for (int i = 0; i < d->navigation_stack.count(); i++) {
                    if ((observer->observerID() == d->navigation_stack.at(i)) || (observer->observerID() == d_observer->observerID()))
                        contextDeleted();
                }
            } else if (objects.at(i) == 0 && !d_observer) {
                contextDeleted();
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::selectObject(QObject* object) {
    if (!object)
        return;

    QList<QObject*> objects;
    objects << object;
    selectObjects(objects);
}

void Qtilities::CoreGui::ObserverWidget::selectCategories(QList<QtilitiesCategory> categories) {
    if (!d->update_selection_activity)
        return;

    if (categories.count() > 0) {
        // Handle for the table view
        if (d->tree_view && d->tree_model && d->display_mode == TreeView && d->tree_proxy_model) {
            d->update_selection_activity = false;

            QModelIndexList mapped_indexes;
            QModelIndex index;
            for (int i = 0; i < categories.count(); i++) {
                index = d->tree_model->findCategory(categories.at(i));
                if (index.isValid()) {
                    QModelIndex mapped_index;
                    if (d->tree_proxy_model) {
                        mapped_index = d->tree_proxy_model->mapFromSource(index);
                    } else
                        mapped_index = index;

                    if (mapped_index.isValid()) {
                         mapped_indexes << mapped_index;
                        d->tree_view->scrollTo(mapped_index,QAbstractItemView::EnsureVisible);
                    }
                }
            }


            QItemSelectionModel *selection_model = d->tree_view->selectionModel();
            if (selection_model) {
                QItemSelection item_selection;
                for (int i = 0; i < mapped_indexes.count(); i++)
                    item_selection.select(mapped_indexes.at(i),mapped_indexes.at(i));

                selection_model->clearSelection();
                selection_model->select(item_selection,QItemSelectionModel::Select);

                if (mapped_indexes.count() > 0)
                    d->tree_view->setCurrentIndex(mapped_indexes.front());

                selectedObjects();
                updateGlobalActiveSubjects();
                QApplication::processEvents();
            }

            d->update_selection_activity = true;

            // Update the property browser:
            #ifdef QTILITIES_PROPERTY_BROWSER
                refreshPropertyBrowser();
                refreshDynamicPropertyBrowser();
            #endif
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::selectCategory(QtilitiesCategory category) {
    QList<QtilitiesCategory> categories;
    categories << category;
    selectCategories(categories);
}

void Qtilities::CoreGui::ObserverWidget::clearSelection() {
    QList<QObject*> objects;
    selectObjects(objects);
    d->current_selection.clear();
}

void Qtilities::CoreGui::ObserverWidget::selectObjects(QList<QPointer<QObject> > objects) {
    if (!d->update_selection_activity)
        return;

    if (objects.count() == 0) {
        if (d->table_view) {
            if (d->table_view->selectionModel())
                d->table_view->selectionModel()->clear();
            emit selectedObjectsChanged(QList<QObject*>());
        } else if (d->tree_view) {
            if (d->tree_view->selectionModel())
                d->tree_view->selectionModel()->clear();
            emit selectedObjectsChanged(QList<QObject*>());
        }
    } else {
        QList<QObject*> simple_list;
        for (int i = 0; i < objects.count(); i++) {
            if (objects.at(i))
                simple_list << objects.at(i);
        }
        selectObjects(simple_list);
    }
}

void Qtilities::CoreGui::ObserverWidget::selectObjects(QList<QObject*> objects) {
    if (!d->update_selection_activity)
        return;

    //qDebug() << "selectObjects" << objects;

    if (objects.count() > 0) {
        // Handle for the table view
        if (d->table_view && d->table_model && d->display_mode == TableView && d->table_proxy_model) {
            d->update_selection_activity = false;

            QModelIndexList mapped_indexes;
            QModelIndex index;
            for (int i = 0; i < objects.count(); i++) {
                index = d->table_model->getIndex(objects.at(i));
                if (index.isValid()) {
                    QModelIndex mapped_index;
                    if (d->table_proxy_model) {
                        mapped_index = d->table_proxy_model->mapFromSource(index);
                    } else
                        mapped_index = index;

                    if (mapped_index.isValid()) {
                        mapped_indexes << mapped_index;
                        if (i == 0)
                            d->table_view->scrollTo(mapped_index,QAbstractItemView::EnsureVisible);
                    }
                }
            }

            QItemSelectionModel *selection_model = d->table_view->selectionModel();
            if (selection_model) {
                QItemSelection item_selection;
                for (int i = 0; i < mapped_indexes.count(); i++)
                    item_selection.select(mapped_indexes.at(i),mapped_indexes.at(i));

                selection_model->clearSelection();
                if (d->table_view->selectionBehavior() == QTableView::SelectRows) {
                    selection_model->select(item_selection,QItemSelectionModel::Rows | QItemSelectionModel::Select);
                } else if (d->table_view->selectionBehavior() == QTableView::SelectItems) {
                    selection_model->select(item_selection,QItemSelectionModel::Select);
                }
                if (mapped_indexes.count() > 0)
                    d->table_view->selectionModel()->setCurrentIndex(mapped_indexes.front(),QItemSelectionModel::Current);
                selectedObjects();
                updateGlobalActiveSubjects();
            }

            d->update_selection_activity = true;

            // Update the property browser:
            #ifdef QTILITIES_PROPERTY_BROWSER
                refreshPropertyBrowser();
                refreshDynamicPropertyBrowser();
            #endif
        } else if (d->tree_view && d->tree_model && d->display_mode == TreeView && d->tree_proxy_model) {
            d->update_selection_activity = false;

            QModelIndexList mapped_indexes;
            QModelIndex index;
            for (int i = 0; i < objects.count(); i++) {
                index = d->tree_model->findObject(objects.at(i));
                if (index.isValid()) {
                    QModelIndex mapped_index;
                    if (d->tree_proxy_model) {
                        mapped_index = d->tree_proxy_model->mapFromSource(index);
                    } else
                        mapped_index = index;

                    if (mapped_index.isValid()) {
                        mapped_indexes << mapped_index;
                        if (i == 0)
                            d->tree_view->scrollTo(mapped_index,QAbstractItemView::EnsureVisible);
                    }
                }
            }

            QItemSelectionModel *selection_model = d->tree_view->selectionModel();
            if (selection_model) {
                QItemSelection item_selection;
                for (int i = 0; i < mapped_indexes.count(); i++)
                    item_selection.select(mapped_indexes.at(i),mapped_indexes.at(i));

                selection_model->clearSelection();
                if (d->tree_view->selectionBehavior() == QTableView::SelectRows) {
                    selection_model->select(item_selection,QItemSelectionModel::Rows | QItemSelectionModel::Select);
                } else if (d->tree_view->selectionBehavior() == QTableView::SelectItems) {
                    selection_model->select(item_selection,QItemSelectionModel::Select);
                }
                if (mapped_indexes.count() > 0)
                    d->tree_view->selectionModel()->setCurrentIndex(mapped_indexes.front(),QItemSelectionModel::Current);

                selectedObjects();
                updateGlobalActiveSubjects();
            }

            d->update_selection_activity = true;

            // Update the property browser:
            #ifdef QTILITIES_PROPERTY_BROWSER
                refreshPropertyBrowser();
                refreshDynamicPropertyBrowser();
            #endif
        }
    }

    //qDebug() << "Current selection" << selectedObjects();
}

void Qtilities::CoreGui::ObserverWidget::handleSearchOptionsChanged() {
    handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
}

void Qtilities::CoreGui::ObserverWidget::handleSearchStringChanged(const QString& filter_string) {
    //qDebug() << observerContext() << "filter_string" << filter_string;
    QSortFilterProxyModel* model = 0;

    if (d->table_view && d->table_model && d->display_mode == TableView && d->table_proxy_model)
         model = qobject_cast<QSortFilterProxyModel*> (d->table_proxy_model);
    else if (d->tree_view && d->tree_model && d->display_mode == TreeView && d->tree_proxy_model)
         model = qobject_cast<QSortFilterProxyModel*> (d->tree_proxy_model);

    // Check if the installed proxy model is a QSortFilterProxyModel:
    if (model) {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        Qt::CaseSensitivity caseSensitivity = d->searchBoxWidget->caseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive;
        model->setFilterCaseSensitivity(caseSensitivity);

        QRegExp::PatternSyntax syntax = d->searchBoxWidget->patternSyntax();
        if (syntax == QRegExp::RegExp)
            model->setFilterRegExp(filter_string);
        else if (syntax == QRegExp::FixedString)
            model->setFilterFixedString(filter_string);
        else if (syntax == QRegExp::Wildcard)
            model->setFilterWildcard(filter_string);

        model->invalidate();
        resizeColumns();

        QApplication::restoreOverrideCursor();
    }
}

void Qtilities::CoreGui::ObserverWidget::resetProxyModel() {
    handleSearchStringChanged("");
}

void Qtilities::CoreGui::ObserverWidget::showProgressInfo(int task_id) {
    if (displayMode() == Qtilities::TreeView) {
        if (!d->task_widget) {
            d->task_widget = TaskManagerGui::instance()->singleTaskWidget(task_id);
            d->task_widget->setMinimumWidth(700);
            d->task_widget->progressBar()->setMinimumWidth(300);

            if (ui->widgetProgressBarHolder->layout())
                delete ui->widgetProgressBarHolder->layout();

            QHBoxLayout* layout = new QHBoxLayout(ui->widgetProgressBarHolder);
            layout->setMargin(0);
            layout->addWidget(d->task_widget);
        }

        d->task_widget->show();
        ui->itemParentWidget->hide();
        ui->navigationBarWidget->hide();

        if (d->searchBoxWidget) {
            d->search_box_visible_before_refresh = ui->widgetSearchBox->isVisible();
            ui->widgetSearchBox->hide();
        }

        ui->widgetProgressInfo->show();

        emit treeModelBuildStarted(task_id);
        QApplication::processEvents();
    }
}

void Qtilities::CoreGui::ObserverWidget::hideProgressInfo(bool emit_tree_build_completed) {
    if (displayMode() == Qtilities::TreeView) {
        ui->widgetProgressInfo->hide();
        ui->itemParentWidget->show();

        if (activeHints()) {
            if (activeHints()->displayFlagsHint() & ObserverHints::NavigationBar && d->display_mode == Qtilities::TableView)
                ui->navigationBarWidget->show();
        } else
            ui->navigationBarWidget->hide();

        if (d->searchBoxWidget && d->search_box_visible_before_refresh)
            ui->widgetSearchBox->show();

        refreshColumnVisibility();
        resizeColumns();

        if (emit_tree_build_completed)
            emit treeModelBuildEnded();

        QApplication::processEvents();
    }
}

void Qtilities::CoreGui::ObserverWidget::resizeColumns() {
    if (d->do_column_resizing) {
        if (d->display_mode == Qtilities::TableView && d->table_model && d->table_view) {
            resizeTableViewRows();

            QHeaderView* table_header = d->table_view->horizontalHeader();
            if (table_header) {
                d->table_view->resizeColumnsToContents();
                table_header->setStretchLastSection(true);
            }

            // Implementation if we want to stretch the name column, however the above implementation proved to be more practical:
//            if (table_header->visualIndex(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName)) != -1)
//                table_header->setResizeMode(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName),QHeaderView::Stretch);
        } else if (d->tree_view && d->tree_model && d->display_mode == Qtilities::TreeView) {
            QHeaderView* tree_header = d->tree_view->header();
            if (tree_header) {
                for (int i = 0; i < tree_header->count(); i++) {
                    if (!tree_header->isSectionHidden(i)) {
                        int logical_index = tree_header->logicalIndex(i);
                        d->tree_view->resizeColumnToContents(logical_index);
                    }
                }
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::adaptColumns(const QModelIndex & topleft, const QModelIndex& bottomRight) {
    if (d->do_column_resizing) {
        if (d->tree_view && d->tree_model && d->display_mode == Qtilities::TreeView) {
            int firstColumn= topleft.column();
            int lastColumn = bottomRight.column();
            do {
                d->tree_view->resizeColumnToContents(firstColumn);
                firstColumn++;
            } while (firstColumn < lastColumn);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handleTreeModelBuildAboutToStart() {
    if (displayMode() == Qtilities::TreeView) {
        QStringList expanded_items = lastExpandedItemsResults();
        d->tree_model->setExpandedItems(expanded_items);
    }
}

void Qtilities::CoreGui::ObserverWidget::handleExpanded(const QModelIndex &index) {
    if (!index.isValid())
        return;

    resizeColumns();
    emit expandedNodesChanged(findExpandedItems());
}

void Qtilities::CoreGui::ObserverWidget::handleCollapsed(const QModelIndex &index) {
    if (!index.isValid())
        return;

    resizeColumns();
    emit expandedNodesChanged(findExpandedItems());
}

void Qtilities::CoreGui::ObserverWidget::expandNodes(const QStringList &node_names) {
    if (d->tree_view && d->tree_model && d->display_mode == Qtilities::TreeView) {
        QModelIndexList indexes = d->tree_model->findExpandedNodeIndexes(node_names);
        expandNodes(indexes);
        //qDebug() << "expandNodes" << node_names;

        if (!node_names.isEmpty())
            d->last_expanded_items_result = node_names;
    }
}

void Qtilities::CoreGui::ObserverWidget::expandNodes(QModelIndexList indexes) {
    if (d->tree_view && d->tree_proxy_model && d->display_mode == Qtilities::TreeView) {
        disconnect(d->tree_view,SIGNAL(expanded(QModelIndex)),this,SLOT(handleExpanded(QModelIndex)));

        if (indexes.isEmpty()) {
            viewExpandAll();
            return;
        } else {
            viewCollapseAll();
        }

        foreach (QModelIndex index, indexes) {
            if (d->tree_proxy_model)
                d->tree_view->setExpanded(d->tree_proxy_model->mapFromSource(index),true);
            else
                d->tree_view->setExpanded(index,true);
        }
        connect(d->tree_view,SIGNAL(expanded(QModelIndex)),SLOT(handleExpanded(QModelIndex)),Qt::UniqueConnection);
        resizeColumns();
    }
}

#ifdef QTILITIES_PROPERTY_BROWSER
void Qtilities::CoreGui::ObserverWidget::refreshPropertyBrowser() {
    // Update the property editor visibility and object
    if (activeHints()->displayFlagsHint() & ObserverHints::PropertyBrowser) {
        constructPropertyBrowser();
        if (selectedObjects().count() == 1)
            d->property_browser_widget->setObject(d->current_selection.front());
        else if (selectedObjects().count() > 1)
            d->property_browser_widget->setObject(0);
        else
            d->property_browser_widget->setObject(d_observer);
        addDockWidget(d->property_editor_dock_area, d->property_browser_dock);

        // Resize the doc depending on where it is:
        //if (d->property_editor_dock_area == Qt::LeftDockWidgetArea || d->property_editor_dock_area == Qt::RightDockWidgetArea)
        //    d->property_browser_widget->setFixedWidth(d->property_browser_widget->sizeHint().width());
        if (d->property_editor_dock_area == Qt::TopDockWidgetArea || d->property_editor_dock_area == Qt::BottomDockWidgetArea)
            d->property_browser_widget->setMaximumHeight(d->property_browser_widget->sizeHint().height());
        d->property_browser_dock->show();

        if (d->property_browser_dock && d->dynamic_property_browser_dock)
            tabifyDockWidget(d->property_browser_dock,d->dynamic_property_browser_dock);
    } else {
        removeDockWidget(d->property_browser_dock);
    }
}

void Qtilities::CoreGui::ObserverWidget::constructPropertyBrowser() {
    if (!d->property_browser_dock) {
        d->property_browser_dock = new QDockWidget("Property Browser",0);
        connect(d->property_browser_dock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),SLOT(setPreferredPropertyEditorDockArea(Qt::DockWidgetArea)));
    }

    if (!d->property_browser_widget) {
        d->property_browser_widget = new ObjectPropertyBrowser(d->property_editor_type);
        if (!d->property_filter.isEmpty())
            d->property_browser_widget->setFilterList(d->property_filter,d->property_filter_inversed);
        d->property_browser_widget->setObject(d_observer);
    }

    d->property_browser_dock->setWidget(d->property_browser_widget);
}

void Qtilities::CoreGui::ObserverWidget::refreshDynamicPropertyBrowser() {
    // Update the property editor visibility and object
    if (activeHints()->displayFlagsHint() & ObserverHints::DynamicPropertyBrowser) {
        constructDynamicPropertyBrowser();
        if (selectedObjects().count() == 1)
            d->dynamic_property_browser_widget->setObject(d->current_selection.front());
        else if (selectedObjects().count() > 1)
            d->dynamic_property_browser_widget->setObject(0);
        else
            d->dynamic_property_browser_widget->setObject(d_observer);
        addDockWidget(d->dynamic_property_editor_dock_area, d->dynamic_property_browser_dock);

        // Resize the doc depending on where it is:
        //if (d->property_editor_dock_area == Qt::LeftDockWidgetArea || d->property_editor_dock_area == Qt::RightDockWidgetArea)
        //    d->property_browser_widget->setFixedWidth(d->property_browser_widget->sizeHint().width());
        if (d->dynamic_property_editor_dock_area == Qt::TopDockWidgetArea || d->dynamic_property_editor_dock_area == Qt::BottomDockWidgetArea)
            d->dynamic_property_browser_widget->setMaximumHeight(d->dynamic_property_browser_widget->sizeHint().height());
        d->dynamic_property_browser_dock->show();

        if (d->property_browser_dock && d->dynamic_property_browser_dock)
            tabifyDockWidget(d->property_browser_dock,d->dynamic_property_browser_dock);
    } else {
        removeDockWidget(d->dynamic_property_browser_dock);
    }
}

void Qtilities::CoreGui::ObserverWidget::constructDynamicPropertyBrowser() {
    if (!d->dynamic_property_browser_dock) {
        d->dynamic_property_browser_dock = new QDockWidget("Dynamic Property Browser",0);
        connect(d->dynamic_property_browser_dock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),SLOT(setPreferredDynamicPropertyEditorDockArea(Qt::DockWidgetArea)));
    }

    if (!d->dynamic_property_browser_widget) {
        d->dynamic_property_browser_widget = new ObjectDynamicPropertyBrowser(d->dynamic_property_editor_type,false);
//        if (!d->dynamic_property_filter.isEmpty())
//            d->dynamic_property_browser_widget->setFilterList(d->dynamic_property_filter,d->dynamic_property_filter_inversed);
        d->dynamic_property_browser_widget->setObject(d_observer);
    }

    d->dynamic_property_browser_dock->setWidget(d->dynamic_property_browser_widget);
}

#endif

void Qtilities::CoreGui::ObserverWidget::refreshActionToolBar(bool force_full_refresh) {
    // Check if an action toolbar should be created:
    if ((activeHints()->displayFlagsHint() & ObserverHints::ActionToolBar) && d->action_provider) {
        if (!force_full_refresh) {
            if (d->last_display_flags != activeHints()->displayFlagsHint() || !d->initialized)
                d->last_display_flags = activeHints()->displayFlagsHint();
            else {
                // Here we need to hide all toolbars that does not contain any actions:
                for (int i = 0; i < d->action_toolbars.count(); i++) {
                    QToolBar* toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(i));
                    bool has_visible_action = false;
                    foreach (QAction* action, toolbar->actions()) {
                        if (action->isVisible()) {
                            has_visible_action = true;
                            break;
                        }
                    }
                    if (!has_visible_action)
                        toolbar->hide();
                    else
                        toolbar->show();
                }
                return;
            }
        }

        // First delete all toolbars added by the observer widget itself:
        deleteActionToolBars();

        // Now create all toolbars:
        QList<QtilitiesCategory> categories = d->action_provider->actionCategories();
        for (int i = 0; i < categories.count(); i++) {
            QList<QAction*> action_list = d->action_provider->actions(IActionProvider::NoFilter,categories.at(i));
            if (action_list.count() > 0) {
                QToolBar* new_toolbar = 0;

                QList<QToolBar *> toolbars = findChildren<QToolBar *>();
                foreach (QToolBar* toolbar, toolbars) {
                    if (toolbar->objectName() == categories.at(i).toString()) {
                        new_toolbar = toolbar;
                        break;
                    }
                }

                if (!new_toolbar) {
                    new_toolbar = addToolBar(categories.at(i).toString());
                    d->action_toolbars << new_toolbar;
                }
                new_toolbar->setObjectName(categories.at(i).toString());
                new_toolbar->addActions(action_list);
            }
        }

        // Here we need to hide all toolbars that does not contain any actions:
        // This implementation will only hide empty toolbars, they will still be there when right clicking on the toolbar.
        // However, this is the best solution I believe since the user can still see which toolbars are available, and
        // then realize that no actions are available for the current selection in hidden toolbars.
        for (int i = 0; i < d->action_toolbars.count(); i++) {
            QToolBar* toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(i));
            bool has_visible_action = false;
            foreach (QAction* action, toolbar->actions()) {
                if (action->isVisible()) {
                    has_visible_action = true;
                    break;
                }
            }
            if (!has_visible_action)
                toolbar->hide();
            else
                toolbar->show();
        }
    } else {
        d->last_display_flags = ObserverHints::NoDisplayFlagsHint;
        deleteActionToolBars();
    }
}

void Qtilities::CoreGui::ObserverWidget::deleteActionToolBars() {
    int toolbar_count = d->action_toolbars.count();
    if (toolbar_count > 0) {
        for (int i = 0; i < toolbar_count; i++) {
            QPointer<QToolBar> toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(0));
            removeToolBar(toolbar);
            if (toolbar) {
                d->action_toolbars.removeOne(toolbar);
                if (toolbar)
                    delete toolbar;
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void Qtilities::CoreGui::ObserverWidget::setDragDropMoveButton(Qt::MouseButton mouse_button) {
    d->button_move = mouse_button;
}

Qt::MouseButton Qtilities::CoreGui::ObserverWidget::dragDropMoveButton() const {
    return d->button_move;
}

void Qtilities::CoreGui::ObserverWidget::setDragDropCopyButton(Qt::MouseButton mouse_button) {
    d->button_copy = mouse_button;
}

Qt::MouseButton Qtilities::CoreGui::ObserverWidget::dragDropCopyButton() const {
    return d->button_copy;
}

bool Qtilities::CoreGui::ObserverWidget::eventFilter(QObject *object, QEvent *event) {
    if (!d->initialized)
        return QMainWindow::eventFilter(object,event);

    // ----------------------------------------------
    // Double Click Signal Emitters
    // ----------------------------------------------
    // -> TreeView Mode:
    // ----------------------------------------------
    if (d->tree_view && d->tree_model && d->display_mode == TreeView) {
        if (object == d->tree_view->viewport() && event->type() == QEvent::MouseButtonDblClick) {
            if (selectedIndexes().count() > 0) {
                if (d->current_selection.count() == 1 && selectedIndexes().front().column() == d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName)) {
                    // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
                    bool use_parent_context = true;
                    Observer* obs = qobject_cast<Observer*> (d->current_selection.front());
                    if (obs) {
                        if (obs->displayHints()) {
                            if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                                emit doubleClickRequest(0, obs);
                                use_parent_context = false;
                            }
                        }
                    }

                    if (use_parent_context)
                        emit doubleClickRequest(d->current_selection.front(), d_observer);
                }
            }
            return false;
        }
    }
    // ----------------------------------------------
    // -> TableView Mode:
    // ----------------------------------------------
    if (d->table_view && d->table_model && d->display_mode == TableView) {
        if (object == d->table_view->viewport() && event->type() == QEvent::MouseButtonDblClick) {
           if (selectedIndexes().count() == 1) {
               // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
               bool use_parent_context = true;
               Observer* obs = qobject_cast<Observer*> (d->table_model->getObject(selectedIndexes().front()));
               if (obs) {
                   if (obs->displayHints()) {
                       if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                           emit doubleClickRequest(0, obs);
                           use_parent_context = false;
                       }
                   }
               }

               if (use_parent_context)
                   emit doubleClickRequest(d->current_selection.front(), d_observer);
           }
           return false;
        }
    }

    // ----------------------------------------------
    // FocusIn and FocusOut Related Event Filtering
    // ----------------------------------------------
    // -> TableView Mode:
    // ----------------------------------------------
    if (d->table_view && d->table_model && d->display_mode == TableView) {
        if (object == d->table_view && event->type() == QEvent::FocusIn) {
            // Connect to the paste action
            if (activeHints()->actionHints() & ObserverHints::ActionPasteItem) {
                Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
                if (command) {
                    if (command->action())
                        connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionPaste_triggered()));
                }
            }

            if (d_observer->subjectCount() == 0) {
                CONTEXT_MANAGER->setNewContext(contextString(),true);
                updateGlobalActiveSubjects();
                refreshActions();
            } else {
                CONTEXT_MANAGER->setNewContext(contextString(),true);
            }

            return false;
        } else if (object == d->table_view && event->type() == QEvent::FocusOut) {
            // Disconnect the paste action from the this widget.
            if (activeHints()->actionHints() & ObserverHints::ActionPasteItem) {
                Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
                if (command) {
                    if (command->action())
                        command->action()->disconnect(this);
                }
            }
            return false;
        }
    }
    // ----------------------------------------------
    // -> TreeView Mode:
    // ----------------------------------------------
    if (d->tree_view && d->tree_model && d->display_mode == TreeView) {
        if (object == d->tree_view && event->type() == QEvent::FocusIn) {
            if (!d_observer)
                return false;

            // Connect to the paste action
            if (activeHints()->actionHints() & ObserverHints::ActionPasteItem) {
                Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
                if (command) {
                    if (command->action())
                        connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionPaste_triggered()));
                }
            }

            if (d_observer->subjectCount() == 0) {
                CONTEXT_MANAGER->setNewContext(contextString(),true);
                updateGlobalActiveSubjects();
                refreshActions();
            } else {
                CONTEXT_MANAGER->setNewContext(contextString(),true);
            }
            return false;
        } else if (object == d->tree_view && event->type() == QEvent::FocusOut) {
            if (activeHints()->actionHints() & ObserverHints::ActionPasteItem) {
                // Disconnect the paste action from the this widget.
                Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
                if (command) {
                    if (command->action())
                        command->action()->disconnect(this);
                }
            }
            return false;
        }
    }

    // ----------------------------------------------
    // Drag & Drop Related Event Filtering
    // ----------------------------------------------
    // -> TableView Mode:
    // ----------------------------------------------
    if (d->table_view && d->table_model && d->display_mode == TableView) {
        if (object == d->table_view && event->type() == QEvent::DragMove && !d->read_only) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AcceptDrops))
                return false;

            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent *>(event);
            dragMoveEvent->accept();
            return false;
        } else if (object == this && event->type() == QEvent::Drop && !d->read_only) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AcceptDrops))
                return false;

            QDropEvent *dropEvent = static_cast<QDropEvent *>(event);

            if (!d->initialized)
                return false;

            if (dropEvent->proposedAction() == Qt::MoveAction || dropEvent->proposedAction() == Qt::CopyAction) {
                const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (dropEvent->mimeData());
                if (observer_mime_data) {
                    if (observer_mime_data->sourceID() == d_observer->observerID()) {
                        LOG_ERROR_P(QString(tr("The drop operation could not be completed. The destination and source is the same.")));
                        return false;
                    }

                    QString error_msg;
                    if (d_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data),&error_msg) == Observer::Allowed) {
                        // Now check the proposed action of the event.
                        if (dropEvent->proposedAction() == Qt::MoveAction) {
                            dropEvent->accept();
                            OBJECT_MANAGER->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),d_observer->observerID());
                        } else if (dropEvent->proposedAction() == Qt::CopyAction) {
                            dropEvent->accept();
                            // Attempt to copy the dragged objects:
                            QList<QPointer<QObject> > dropped_list = d_observer->attachSubjects(const_cast<ObserverMimeData*> (observer_mime_data));
                            if (dropped_list.count() != observer_mime_data->subjectList().count()) {
                                LOG_WARNING_P(QString(tr("The drop operation completed partially. %1/%2 objects were drop successfully.").arg(dropped_list.count()).arg(observer_mime_data->subjectList().count())));
                            } else {
                                LOG_INFO_P(QString(tr("The drop operation completed successfully on %1 objects.").arg(dropped_list.count())));
                            }
                        }
                    } else
                        LOG_ERROR_P(QString(tr("The drop operation could not be completed. The destination observer cannot accept all the objects in your selection. Error message: ") + error_msg));
                }
            }
            return false;
        } else if (object == d->table_view && event->type() == QEvent::DragEnter && !d->read_only) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AcceptDrops))
                return false;

            QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent *>(event);
            dragEnterEvent->accept();
            return false;
        } else if (object == d->table_view->viewport() && event->type() == QEvent::MouseButtonPress) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AllowDrags))
                return false;

            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->buttons() == d->button_copy || mouseEvent->buttons() == d->button_move)
               d->startPos = mouseEvent->pos();
            return false;
        } else if (object == d->table_view->viewport() && event->type() == QEvent::MouseMove) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AllowDrags))
                return false;

            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

            int distance = (mouseEvent->pos() - d->startPos).manhattanLength();
            if (distance >= QApplication::startDragDistance()) {
                if (mouseEvent->buttons() == d->button_copy || mouseEvent->buttons() == d->button_move) {
                    if (d->current_selection.size() > 0 && d->initialized && d->table_model) {
                        ObserverMimeData* mimeData;
                        QDrag *drag = new QDrag(this);
                        if (mouseEvent->buttons() == d->button_copy) {
                            mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID(),Qt::CopyAction);
                            drag->setMimeData(mimeData);
                            drag->exec(Qt::CopyAction);
                        } else if (mouseEvent->buttons() == d->button_move) {
                            mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID(),Qt::MoveAction);
                            drag->setMimeData(mimeData);
                            drag->exec(Qt::MoveAction);
                        }
                    }
                }
            }
        }
    }

    // ----------------------------------------------
    // -> TreeView Mode:
    // ----------------------------------------------
    if (d->tree_view && d->tree_model && d->display_mode == TreeView) {
        if (object == d->tree_view && event->type() == QEvent::DragMove && !d->read_only) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AcceptDrops || activeHints()->categoryEditingFlags() & ObserverHints::CategoriesAcceptSubjectDrops))
                return false;

            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent *>(event);
            if (!(dragMoveEvent->mouseButtons() & d->button_copy || dragMoveEvent->mouseButtons() & d->button_move) || (dragMoveEvent->mouseButtons() == Qt::NoButton))
                return false;

            dragMoveEvent->accept();
            return false;
        } else if (object == d->tree_view && event->type() == QEvent::DragEnter && !d->read_only) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AcceptDrops || activeHints()->categoryEditingFlags() & ObserverHints::CategoriesAcceptSubjectDrops))
                return false;

            QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent *>(event);
            if (!(dragEnterEvent->mouseButtons() & d->button_copy || dragEnterEvent->mouseButtons() & d->button_move) || (dragEnterEvent->mouseButtons() == Qt::NoButton))
                return false;

            dragEnterEvent->accept();
            return false;
        } else if (object == d->tree_view->viewport() && event->type() == QEvent::MouseButtonPress) {
            if (!(activeHints()->dragDropHint() & ObserverHints::AllowDrags))
                return false;

            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (!(mouseEvent->button() == d->button_copy || mouseEvent->button() == d->button_move) || (mouseEvent->button() == Qt::NoButton)) {
                d->startPos = QPoint();
                return false;
            }

            d->startPos = mouseEvent->pos();
            return false;
        } else if (object == d->tree_view->viewport() && event->type() == QEvent::MouseMove) {
            if (!d->initialized)
                return false;

            if (!(activeHints()->dragDropHint() & ObserverHints::AllowDrags))
                return false;

            if (d->startPos.isNull())
                return false;

            QMouseEvent* mouseEvent = static_cast<QMouseEvent *>(event);
            int distance = (mouseEvent->pos() - d->startPos).manhattanLength();
            if (distance >= QApplication::startDragDistance()) {
                if (mouseEvent->buttons() == d->button_copy || mouseEvent->buttons() == d->button_move) {
                    Observer* obs = selectionParent();
                    if (!obs)
                        return false;

                    // Check if all the objects in the current selection supports drag flags,
                    // otherwise we do not start the drag:
                    if (d->tree_view->selectionModel()) {
                        QModelIndexList index_list = selectedIndexes();
                        for (int i = 0; i < index_list.count(); i++) {
                            if (!(d->tree_model->flags(index_list.at(i)) & Qt::ItemIsDragEnabled)) {
                                return false;
                            }
                        }
                    }

                    // We don't start the drag here, we just populate the Qtilities clipboard manager
                    // with our own mime data object.
                    QList<QObject*> simple_objects = selectedObjects();
                    QList<QPointer<QObject> > smart_objects;
                    for (int i = 0; i < simple_objects.count(); i++)
                        smart_objects << simple_objects.at(i);
                    ObserverMimeData* mimeData = 0;
                    if (mouseEvent->buttons() == d->button_copy)
                        mimeData = new ObserverMimeData(smart_objects,obs->observerID(),Qt::CopyAction);
                    else if (mouseEvent->buttons() == d->button_move)
                        mimeData = new ObserverMimeData(smart_objects,obs->observerID(),Qt::MoveAction);
                    CLIPBOARD_MANAGER->setMimeData(mimeData);
                    d->tree_view->setDropIndicatorShown(true);
                }
            }

            return false;
        }
    }

    // ----------------------------------------------
    // Check if d_observer context is deleted
    // ----------------------------------------------
    if (object == this && event->type() == QEvent::User) {
        // We check if the event is a QtilitiesPropertyChangeEvent.
        QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
        if (qtilities_event) {
            if (!strcmp(qtilities_event->propertyName().data(),qti_prop_OBSERVER_MAP)) {
                // This implementation can be improved in the future. For now we don't care if the change affects this view.
                // We just close this window.
                contextDeleted();
            }
        }
        return false;
     }

    return false;
}
