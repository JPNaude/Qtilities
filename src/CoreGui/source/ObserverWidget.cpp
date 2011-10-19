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
#include "ObserverTableModelCategoryFilter.h"
#include "ObserverTreeModelProxyFilter.h"
#include "ActionProvider.h"
#include "ObserverTreeItem.h"
#include "QtilitiesMainWindow.h"

#include <ActivityPolicyFilter.h>
#include <ObserverHints.h>
#include <ObserverMimeData.h>
#include <QtilitiesCoreConstants.h>
#include <Logger.h>

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

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core::Properties;

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
        table_view(0),
        table_model(0),
        custom_table_proxy_model(0),
        tree_view(0),
        tree_model(0),
        custom_tree_proxy_model(0),
        proxy_model(0),
        table_name_column_delegate(0),
        tree_name_column_delegate(0),
        activity_filter(0),
        top_level_observer(0),
        initialized(false),
        read_only(false),
        update_selection_activity(true),
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
        do_column_resizing(true) { }

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
    QAbstractProxyModel* proxy_model;
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
    #endif

    //! Indicates if the widget is in an initialized state. Thus initialization was successful. \sa initialize()
    bool initialized;
    //! Indicates if the widget is read only
    bool read_only;
    //! Used to disable selection activiy updates in FollowSelection cases. This is only used internally to avoid continuous loops.
    bool update_selection_activity;

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
};

Qtilities::CoreGui::ObserverWidget::ObserverWidget(DisplayMode display_mode, QWidget * parent, Qt::WindowFlags f) :
    QMainWindow(parent, f),
    ui(new Ui::ObserverWidget)
{
    ui->setupUi(this);
    d = new ObserverWidgetData;
    d->action_provider = new ActionProvider(this);

    #ifdef QTILITIES_PROPERTY_BROWSER
    d->property_browser_dock = 0;
    d->property_browser_widget = 0;
    d->property_editor_dock_area = Qt::RightDockWidgetArea;
    d->property_editor_type = ObjectPropertyBrowser::TreeBrowser;
    d->property_filter_inversed = false;
    d->property_filter = QStringList();
    #endif

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
}

Qtilities::CoreGui::ObserverWidget::ObserverWidget(Observer* observer_context, DisplayMode display_mode, QWidget * parent, Qt::WindowFlags f) :
    QMainWindow(parent, f),
    ui(new Ui::ObserverWidget)
{
    ui->setupUi(this);
    d = new ObserverWidgetData;
    d->action_provider = new ActionProvider(this);

    #ifdef QTILITIES_PROPERTY_BROWSER
    d->property_browser_dock = 0;
    d->property_browser_widget = 0;
    d->property_editor_dock_area = Qt::RightDockWidgetArea;
    d->property_editor_type = ObjectPropertyBrowser::TreeBrowser;
    d->property_filter_inversed = false;
    d->property_filter = QStringList();
    #endif

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

Qtilities::CoreGui::ObserverWidget::~ObserverWidget()
{   
    delete ui;
    delete d;           
}

Qtilities::Core::ObserverHints* Qtilities::CoreGui::ObserverWidget::activeHints() const {
    if (d->use_observer_hints && d->hints_selection_parent)
        return d->hints_selection_parent;
    else
        return d->hints_default;
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

    // Connect to the distroyed signal
    connect(observer,SIGNAL(destroyed()),SLOT(contextDeleted()));

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

    // Resize the rows in the table view
    if (d->display_mode == TableView && d->table_view)
        resizeTableViewRows();

    // Update the observer context of the delegates
    if (d->display_mode == TableView && d->table_name_column_delegate)
        d->table_name_column_delegate->setObserverContext(observer);
    else if (d->display_mode == TreeView && d->tree_name_column_delegate)
        d->tree_name_column_delegate->setObserverContext(observer); 

    emit observerContextChanged(d_observer);
    emit selectedObjectsChanged(QList<QObject*>());
    return true;
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
    refreshActions();

    #ifdef QTILITIES_PROPERTY_BROWSER
    // TODO: This can be improved. Add that both property editors can be made read only.
    if (d->property_browser_widget)
        d->property_browser_widget->setEnabled(!read_only);
    #endif

    emit readOnlyStateChanged(read_only);
}

bool Qtilities::CoreGui::ObserverWidget::readOnly() const {
    return d->read_only;
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

    // Set the title and name of the observer widget.
    // Here we need to check if we must use d_observer inside a specific context
    setWindowTitle(d_observer->observerName());
    setObjectName("ObserverWidget: " + d_observer->observerName());

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
                d->tree_view->setFocusPolicy(Qt::StrongFocus);
                d->tree_view->setRootIsDecorated(true);
                d->tree_view->setContextMenuPolicy(Qt::CustomContextMenu);
                d->tree_view->setAcceptDrops(true);
                d->tree_view->setAutoExpandDelay(500);
                d->tree_view->setDropIndicatorShown(true);
                d->tree_view->setDragEnabled(true);
                if (!d->tree_model) {
                    d->tree_model = new ObserverTreeModel(d->tree_view);
                    d->tree_model->toggleUseObserverHints(d->use_observer_hints);
                    d->tree_model->setCustomHints(d->hints_default);
                }
                d->tree_view->setSortingEnabled(true);
                d->tree_view->sortByColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName),Qt::AscendingOrder);
                connect(d->tree_model,SIGNAL(selectionParentChanged(Observer*)),SLOT(setTreeSelectionParent(Observer*)));
                connect(d->tree_model,SIGNAL(selectObjects(QList<QPointer<QObject> >)),SLOT(selectObjects(QList<QPointer<QObject> >)));
                connect(d->tree_model,SIGNAL(selectObjects(QList<QObject*>)),SLOT(selectObjects(QList<QObject*>)));

                d->tree_view->viewport()->installEventFilter(this);
                d->tree_view->installEventFilter(this);

                // Initialize naming control delegate
                if (!d->tree_name_column_delegate) {
                    d->tree_name_column_delegate = new NamingPolicyDelegate(this);
                }

                d->tree_name_column_delegate->setObserverContext(d_observer);
                connect(this,SIGNAL(selectedObjectsChanged(QList<QObject*>)),d->tree_name_column_delegate,SLOT(handleCurrentObjectChanged(QList<QObject*>)));
            }

            if (ui->itemParentWidget->layout())
                delete ui->itemParentWidget->layout();

            QHBoxLayout* layout = new QHBoxLayout(ui->itemParentWidget);
            layout->setMargin(0);
            layout->addWidget(d->tree_view);
            d->tree_view->setEnabled(true);

            d->tree_model->setObjectName(d->top_level_observer->observerName());
            d->tree_model->setObserverContext(d->top_level_observer);
            d->tree_view->setItemDelegate(d->tree_name_column_delegate);

            // Setup tree selection:
            d->tree_view->setSelectionMode(QAbstractItemView::SingleSelection);
            d->tree_view->setSelectionBehavior(QAbstractItemView::SelectItems);

            // Setup proxy model:
            if (!d->custom_tree_proxy_model) {
                if (!d->proxy_model) {
                    QSortFilterProxyModel* new_model = new ObserverTreeModelProxyFilter(this);
                    new_model->setDynamicSortFilter(true);
                    new_model->setFilterKeyColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
                    d->proxy_model = new_model;
                }
            } else
                d->proxy_model = d->custom_tree_proxy_model;

            d->proxy_model->setSourceModel(d->tree_model);
            d->tree_view->setModel(d->proxy_model);

            if (d->tree_view->selectionModel())
                connect(d->tree_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()),Qt::UniqueConnection);

            // The view must always be visible.
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
                    d->table_model->toggleUseObserverHints(d->use_observer_hints);
                    d->table_model->setCustomHints(d->hints_default);
                }
                d->table_view->setSortingEnabled(true);
                connect(d->table_view->verticalHeader(),SIGNAL(sectionCountChanged(int,int)),SLOT(resizeTableViewRows()));

                d->table_view->viewport()->installEventFilter(this);
                d->table_view->installEventFilter(this);

                // Initialize naming control delegate
                if (!d->table_name_column_delegate) {
                    d->table_name_column_delegate = new NamingPolicyDelegate(this);
                }

                connect(this,SIGNAL(selectedObjectsChanged(QList<QObject*>)),d->table_name_column_delegate,SLOT(handleCurrentObjectChanged(QList<QObject*>)));
                d->table_view->setItemDelegate(d->table_name_column_delegate);
            }

            if (ui->itemParentWidget->layout())
                delete ui->itemParentWidget->layout();

            QHBoxLayout* layout = new QHBoxLayout(ui->itemParentWidget);
            layout->setMargin(0);
            layout->addWidget(d->table_view);
            d->table_view->setEnabled(true);

            d->table_model->setObjectName(d_observer->observerName());
            d->table_model->setObserverContext(d_observer);
            d->table_name_column_delegate->setObserverContext(d_observer);

            // Setup the table view to look nice
            d->table_view->setSelectionBehavior(QAbstractItemView::SelectItems);
            d->table_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
            d->table_view->verticalHeader()->setVisible(false);

            // Setup proxy model
            if (!d->custom_table_proxy_model) {
                if (!d->proxy_model) {
                    QSortFilterProxyModel* new_model = new ObserverTableModelCategoryFilter(this);
                    new_model->setDynamicSortFilter(true);
                    new_model->setFilterKeyColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName));
                    d->proxy_model = new_model;
                }
            } else
                d->proxy_model = d->custom_table_proxy_model;

            d->proxy_model->setSourceModel(d->table_model);
            d->table_view->setModel(d->proxy_model);

            if (d->table_view->selectionModel()) {
                d->table_view->selectionModel()->clear();
                connect(d->table_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()),Qt::UniqueConnection);
            }

            // The view must always be visible.
            d->table_view->setVisible(true);
            resizeTableViewRows();
        }
    }

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
            d->navigation_bar->show();
        }

        if (d->display_mode == TableView) {
            d->navigation_bar->setVisible(true);
            d->navigation_bar->setCurrentObject(d_observer);
            d->navigation_bar->setNavigationStack(d->navigation_stack);
        } else if (d->display_mode == TreeView) {
            d->navigation_bar->setVisible(false);
        }
        ui->navigationBarWidget->setVisible(true);
    } else {
        if (d->navigation_bar) {
            delete d->navigation_bar;
            d->navigation_bar = 0;
        }
        ui->navigationBarWidget->setVisible(false);
    }

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
            if (activeHints()->hierarchicalDisplayHint() & ObserverHints::CategorizedHierarchy) {
                d->table_view->showColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
            } else {
                d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
            }
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

        // Resize columns
        if (d->do_column_resizing) {
            d->table_view->resizeColumnsToContents();
            QHeaderView* table_header = d->table_view->horizontalHeader();
            table_header->setResizeMode(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName),QHeaderView::Stretch);
        }
    } else if (d->display_mode == TreeView && d->tree_view && d->tree_model) {
        // Show only the needed columns for the current observer
        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnNameHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
            if (d->do_column_resizing)
                d->tree_view->resizeColumnToContents(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
            if (d->do_column_resizing)
                d->tree_view->resizeColumnToContents(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
            if (d->do_column_resizing)
                d->tree_view->resizeColumnToContents(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
            if (d->do_column_resizing)
                d->tree_view->resizeColumnToContents(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
        }

        // Resize columns:
        if (d->do_column_resizing) {
            QHeaderView* tree_header = d->tree_view->header();
            if (tree_header) {
                for (int i = 0; i < tree_header->count(); i++) {
                    if (i != d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName))
                        tree_header->setResizeMode(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName),QHeaderView::ResizeToContents);
                }
                if (tree_header->visualIndex(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName)) != -1)
                    tree_header->setResizeMode(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName),QHeaderView::Stretch);
            }
        }
    }

    d->initialized = true;
    if (!hints_only) {
        // Construct the property browser if neccesarry:
        #ifdef QTILITIES_PROPERTY_BROWSER
        refreshPropertyBrowser();
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

    // Init all actions
    refreshActions();
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
            for (int i = 0; i < d->table_view->selectionModel()->selectedIndexes().count(); i++) {
                QModelIndex index = d->table_view->selectionModel()->selectedIndexes().at(i);
                if (index.column() == 1) {
                    QObject* obj = d->table_model->getObject(d->proxy_model->mapToSource(index));
                    smart_selected_objects << obj;
                    selected_objects << obj;
                }
            }
        }
    } else if (d->display_mode == TreeView) {
        if (!d->tree_view || !d->tree_model)
            return selected_objects;

        if (d->tree_view->selectionModel()) {
            for (int i = 0; i < d->tree_view->selectionModel()->selectedIndexes().count(); i++) {
                QModelIndex index = d->tree_view->selectionModel()->selectedIndexes().at(i);
                if (index.column() == 0) {
                    QObject* obj = d->tree_model->getObject(d->proxy_model->mapToSource(index));
                    smart_selected_objects << obj;
                    smart_tree_item_selection << d->tree_model->getItem(d->proxy_model->mapToSource(index));
                    selected_objects << obj;
                }
            }
        }
        d->tree_model->setSelectedObjects(smart_selected_objects);
    }
    d->current_selection = smart_selected_objects;   
    d->current_tree_item_selection = smart_tree_item_selection;
    return selected_objects;
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
        if (!d->table_view || !d->table_model)
            return selected_indexes;

        if (d->table_view->selectionModel()) {
            for (int i = 0; i < d->table_view->selectionModel()->selectedIndexes().count(); i++) {
                QModelIndex index = d->table_view->selectionModel()->selectedIndexes().at(i);
                if (index.column() == 1)
                    selected_indexes << d->proxy_model->mapToSource(index);
            }
        }
    } else if (d->display_mode == TreeView) {
        if (!d->tree_view || !d->tree_model)
            return selected_indexes;

        if (d->tree_view->selectionModel()) {
            for (int i = 0; i < d->tree_view->selectionModel()->selectedIndexes().count(); i++) {
                QModelIndex index = d->tree_view->selectionModel()->selectedIndexes().at(i);
                if (index.column() == 0)
                    selected_indexes << d->proxy_model->mapToSource(index);
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

void Qtilities::CoreGui::ObserverWidget::enableAutoColumnResizing() {
    d->do_column_resizing = true;
}

void Qtilities::CoreGui::ObserverWidget::disableAutoColumnResizing() {
    d->do_column_resizing = false;
}

void Qtilities::CoreGui::ObserverWidget::writeSettings() {
    if (!d->initialized)
        return;

    QSettings settings;
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
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::ObserverWidget::readSettings() {
    if (!d->initialized) {
        LOG_ERROR(QString(tr("readSettings() can only be called on observer widget \"%1\" after the widget has been initialized.")).arg(d->global_meta_type));
        return;
    }

    QSettings settings;
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

#endif

Qtilities::CoreGui::SearchBoxWidget* Qtilities::CoreGui::ObserverWidget::searchBoxWidget() {
    return d->searchBoxWidget;
}

void Qtilities::CoreGui::ObserverWidget::resizeTableViewRows(int height) {
    if (height == -1)
        height = d->default_row_height;
    if (d->display_mode == TableView && d->table_view && d->table_model) {
        if (d->do_column_resizing)
            d->table_view->resizeColumnsToContents();
        for (int i = 0; i < d->table_model->rowCount(); i++) {
            d->table_view->setRowHeight(i,height);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::constructActions() {
    if (d->actionNewItem)
        return;

    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(d->global_meta_type));

    bool current_processing_cycle_active = ACTION_MANAGER->commandObserver()->isProcessingCycleActive();
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

    if (!current_processing_cycle_active)
        ACTION_MANAGER->commandObserver()->endProcessingCycle(false);
}

#ifndef QT_NO_DEBUG
void Qtilities::CoreGui::ObserverWidget::selectionDebug() const {
    if (d->current_selection.count() == 1) {
        QObject* selection = d->current_selection.front();
        OBJECT_MANAGER->registerObject(selection);

        QtilitiesMainWindow* main_window = qobject_cast<QtilitiesMainWindow*> (QtilitiesApplication::mainWindow());
        if (main_window) {
            if (main_window->modeManager()) {
                main_window->modeManager()->setActiveMode("Qtilities Debugging");
            }
        }
    }
}
#endif

void Qtilities::CoreGui::ObserverWidget::refreshActions() {
    if (!d->initialized || !d->actionRemoveItem) {
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

    if (activeHints()->actionHints() & ObserverHints::ActionPushDown)
        d->actionPushDown->setVisible(true);
    else
        d->actionPushDown->setVisible(false);

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
        d->actionSwitchView->setIcon(QIcon(qti_icon_TABLE_16x16));
        d->actionCollapseAll->setVisible(true);
        d->actionExpandAll->setVisible(true);
        d->actionCollapseAll->setEnabled(true);
        d->actionExpandAll->setEnabled(true);
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
    if (selectedObjects().count() == 0) {
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
                    Observer* observer = qobject_cast<Observer*> (obj);
                    if (!observer) {
                        // Handle the cases where the current object has an observer child
                        foreach (QObject* child, obj->children()) {
                            Observer* child_observer = qobject_cast<Observer*> (child);
                            if (child_observer) {
                                observer = child_observer;
                                // For now we break, when there is cases where an object has more than 1 observer child,
                                // a pop up must prompt the user to choose which one to push into. This should however
                                // not be neccesarry because the whole idea of allowing categorized observer subjects
                                // is to avoid having multiple observer children. However if the need for it arises, this
                                // code can be changed.
                                break;
                            }
                        }
                    }

                    if (observer) {
                        d->actionPushDown->setEnabled(true);
                        d->actionPushDownNew->setEnabled(true);

                        // Delete and Remove All depends on the selected context:
                        if (observer->displayHints()->observerSelectionContextHint() == ObserverHints::SelectionUseSelectedContext) {
                            if (observer->subjectCount() > 0) {
                                d->actionRemoveAll->setText(tr("Detach All Under Selection"));
                                d->actionRemoveAll->setEnabled(true);
                                d->actionDeleteAll->setText(tr("Delete All Under Selection"));
                                d->actionDeleteAll->setEnabled(true);
                            } else {
                                d->actionRemoveAll->setEnabled(false);
                                d->actionDeleteAll->setEnabled(false);
                            }
                        } else if (observer->displayHints()->observerSelectionContextHint() == ObserverHints::SelectionUseParentContext && selectionParent()) {
                            d->actionRemoveAll->setText(tr("Detach All"));
                            d->actionRemoveAll->setEnabled(true);
                            d->actionDeleteAll->setText(tr("Delete All"));
                            d->actionDeleteAll->setEnabled(true);
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
            }
        } else if (d->display_mode == TreeView) {
            if (selectedObjects().count() == 1) {
                // We can't delete the top level observer in a tree:
                Observer* selected = qobject_cast<Observer*> (selectedObjects().front());
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

                    if (!hints_to_use_for_selection) {
                        d->actionRemoveAll->setText(tr("Detach All In Current Context"));
                        d->actionRemoveAll->setEnabled(true);
                        d->actionDeleteAll->setText(tr("Delete All In Current Context"));
                        d->actionDeleteAll->setEnabled(true);
                    } else {
                        if (hints_to_use_for_selection->observerSelectionContextHint() == ObserverHints::SelectionUseSelectedContext) {
                            if (selected->subjectCount() > 0) {
                                d->actionRemoveAll->setText(tr("Detach All Under Selection"));
                                d->actionRemoveAll->setEnabled(true);
                                d->actionDeleteAll->setText(tr("Delete All Under Selection"));
                                d->actionDeleteAll->setEnabled(true);
                            } else {
                                d->actionRemoveAll->setEnabled(false);
                                d->actionDeleteAll->setEnabled(false);
                            }
                        } else if (hints_to_use_for_selection->observerSelectionContextHint() == ObserverHints::SelectionUseParentContext && selectionParent()) {
                            d->actionRemoveAll->setText(tr("Detach All In Current Context"));
                            d->actionRemoveAll->setEnabled(true);
                            d->actionDeleteAll->setText(tr("Delete All In Current Context"));
                            d->actionDeleteAll->setEnabled(true);
                        }
                    }
                } else {
                    d->actionRemoveAll->setText(tr("Detach All In Current Context"));
                    d->actionRemoveAll->setEnabled(true);
                    d->actionDeleteAll->setText(tr("Delete All In Current Context"));
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
            } else {
                // Disable remove and delete all actions. We need to check if the selection is in the same
                // context to be able to enable it.
                d->actionRemoveAll->setEnabled(false);
                d->actionDeleteAll->setEnabled(false);
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
    }

    refreshActionToolBar();
}

void Qtilities::CoreGui::ObserverWidget::setTreeSelectionParent(Observer* observer) {
    // This function will only be entered in TreeView mode.
    // It is a slot connected to the selection parent changed signal in the tree model.
    if (!observer)
        observer = d->top_level_observer;

    // Do some hints debugging:
    /*if (observer) {
        qDebug() << "New selection parent: " << observer << " with display hints.";
        if (observer->displayHints()) {
            ObserverHints::DisplayFlags display_flags_hint = observer->displayHints()->displayFlagsHint();
            if (observer->displayHints()->displayFlagsHint() & ObserverHints::ActionToolBar) {
                qDebug() << "-> Action toolbar enabled";
            }
        }
    } else {
        qDebug() << "New selection parent: " << observer << " with NO display hints.";
    }*/


    if (setObserverContext(observer)) {
        // We set d->update_selection_activity to false in here since we don't want an initial selection
        // to be created in initialize()
        d->update_selection_activity = false;
        initialize(true);
        d->update_selection_activity = true;
    }

    // We need to look at the current selection parent if in tree mode, otherwise in table mode we use d_observer:
    if (d->update_selection_activity && observer) {
        // Check if the observer has a FollowSelection activity policy
        if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
            // Check if the observer has a activity filter, which it should have with this hint
            ActivityPolicyFilter* filter = 0;
            for (int i = 0; i < observer->subjectFilters().count(); i++) {
                filter = qobject_cast<ActivityPolicyFilter*> (observer->subjectFilters().at(i));
                if (filter) {
                    // We set d->update_selection_activity to false in here since we don't want selectObjects()
                    // to select the objects again. We will get in this slot when the user already made a new
                    // selection and we do not want to go into an endless loop.
                    d->update_selection_activity = false;
                    filter->setActiveSubjects(d->current_selection);
                    d->update_selection_activity = true;
                }
            }
        }
    }

    d->tree_name_column_delegate->setObserverContext(observer);
    #ifdef QTILITIES_PROPERTY_BROWSER
    refreshPropertyBrowser();
    #endif
}

void Qtilities::CoreGui::ObserverWidget::selectionDetach() {
    if (!d->initialized)
        return;

    // Detach selected objects:
    int first_delete_position = -1;
    if (d->display_mode == TableView && d->table_model) {
        for (int i = 0; i < d->current_selection.count(); i++) {
            d_observer->detachSubject(d->current_selection.at(i));
            if (i == 0)
                first_delete_position = d_observer->subjectReferences().indexOf(d->current_selection.at(i));
        }
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (d->current_selection.count() != 1)
            return;
        
        // Make sure the selected object is not the top level observer (might happen in a tree view)
        Observer* observer = qobject_cast<Observer*> (d->current_selection.front());
        if (observer == d->top_level_observer)
            return;

        if (selectionParent())
            first_delete_position = selectionParent()->subjectReferences().indexOf(d->current_selection.front());
        else if (!selectionParent() && d_observer)
            first_delete_position = d_observer->subjectReferences().indexOf(d->current_selection.front());

        if (!d_observer->detachSubject(d->current_selection.front()))
            return;
    }

    // Select a different item in the same context:
    QList<QObject*> object_list;
    if (d->display_mode == TableView && d->table_model) {
        if (first_delete_position <= d_observer->subjectCount() && first_delete_position > 0) {
            object_list << d_observer->subjectAt(first_delete_position-1);
            selectObjects(object_list);
        } else {
            clearSelection();
        }
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        // We must check if there is a selection parent, else use d_observer:
        if (d->tree_model->selectionParent()) {
            Observer* selection_parent = d->tree_model->selectionParent();
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
    }

    // Check if the new selection must become active:
    if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
        d->update_selection_activity = false;
        Observer* selection_parent = selectionParent();
        if (selection_parent)
            selection_parent->refreshViewsLayout();
        if (d->activity_filter)
            d->activity_filter->setActiveSubjects(object_list);
        d->update_selection_activity = true;
    }

    refreshActions();
}

void Qtilities::CoreGui::ObserverWidget::selectionDetachAll() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        d_observer->detachAll();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (selectedIndexes().count() != 1)
            return;

        // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed:
        bool use_parent_context = true;
        Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
        if (obs) {
            if (obs->displayHints()) {
                if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                    obs->detachAll();
                    QList<QObject*> object_list;
                    object_list << obs;
                    selectObjects(object_list);
                    use_parent_context = false;
                }
            }
        }

        if (use_parent_context) {
            // We must check if there is an selection parent, else use d_observer
            if (d->tree_model->selectionParent()) {
                Observer* selection_parent = d->tree_model->selectionParent();
                selection_parent->detachAll();
                QList<QObject*> object_list;
                object_list << selection_parent;
                selectObjects(object_list);
            } else {
                d_observer->detachAll();
                QList<QObject*> object_list;
                object_list << d_observer;
                selectObjects(object_list);
            }
        }
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

    // Delete selected objects:
    int first_delete_position = -1;
    for (int i = 0; i < selected_count; i++) {       
        if (d->current_selection.at(i)) {
            // Make sure the selected object is not the top level observer (might happen in a tree view)
            Observer* observer = qobject_cast<Observer*> (d->current_selection.at(0));
            if (observer != d->top_level_observer) {
                if (i == 0 && selectionParent())
                    first_delete_position = selectionParent()->subjectReferences().indexOf(d->current_selection.at(i));
                else if (i == 0 && !selectionParent() && d_observer)
                    first_delete_position = d_observer->subjectReferences().indexOf(d->current_selection.at(i));

                delete d->current_selection.at(0);
            } else
                return;
        }
    }

    // Select a different item in the same context:
    QList<QObject*> object_list;
    if (d->display_mode == TableView && d->table_model) {
        if (first_delete_position <= d_observer->subjectCount() && first_delete_position > 0) {
            object_list << d_observer->subjectAt(first_delete_position-1);
            selectObjects(object_list);
        } else {
            clearSelection();
        }
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        // We must check if there is a selection parent, else use d_observer:
        if (d->tree_model->selectionParent()) {
            Observer* selection_parent = d->tree_model->selectionParent();
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
    }

    // Check if the new selection must become active:
    if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
        d->update_selection_activity = false;
        Observer* selection_parent = selectionParent();
        if (selection_parent)
            selection_parent->refreshViewsLayout();
        if (d->activity_filter)
            d->activity_filter->setActiveSubjects(object_list);
        d->update_selection_activity = true;
    }

    refreshActions();
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

    if (d->display_mode == TableView && d->table_model) {
        d_observer->deleteAll();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (selectedIndexes().count() != 1)
            return;

        // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed:
        bool use_parent_context = true;
        Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
        if (obs) {
            if (obs->displayHints()) {
                if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                    obs->deleteAll();
                    QList<QObject*> object_list;
                    object_list << obs;
                    selectObjects(object_list);
                    use_parent_context = false;
                }
            }
        }

        if (use_parent_context) {
            // We must check if there is an selection parent, else use d_observer
            if (d->tree_model->selectionParent()) {
                Observer* selection_parent = d->tree_model->selectionParent();
                selection_parent->deleteAll();
                QList<QObject*> object_list;
                object_list << selection_parent;
                selectObjects(object_list);
            } else {
                d_observer->deleteAll();
                QList<QObject*> object_list;
                object_list << d_observer;
                selectObjects(object_list);
            }
        }
    }    

    refreshActions();
}

void Qtilities::CoreGui::ObserverWidget::handle_actionNewItem_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        if (d->current_selection.count() == 1) {
            // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
            bool use_parent_context = true;
            Observer* obs = qobject_cast<Observer*> (d->table_model->getObject(selectedIndexes().front()));
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
            if (navigationStack().count() > 0) {
                // Get observer from stack
                Observer* selection_parent = OBJECT_MANAGER->observerReference(navigationStack().last());
                emit addActionNewItem_triggered(d_observer, selection_parent);
            } else
                emit addActionNewItem_triggered(d_observer, 0);
        }
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (d->current_selection.count() == 1) {
            // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
            bool use_parent_context = true;
            Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
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
    }
}

void Qtilities::CoreGui::ObserverWidget::refresh() {
    if (!d->initialized || !d_observer)
        return;

    QList<QObject*> current_selection = selectedObjects();
    if (d->display_mode == TableView && d->table_model) {
        bool cycle_active = false;
        if (d_observer->isProcessingCycleActive()) {
            d_observer->endProcessingCycle();
            cycle_active = true;
        }
        d_observer->refreshViewsLayout();
        if (cycle_active)
            d_observer->startProcessingCycle();
        resizeTableViewRows();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        bool cycle_active = false;
        if (d_observer->isProcessingCycleActive()) {
            d_observer->endProcessingCycle();
            cycle_active = true;
        }
        d_observer->refreshViewsLayout();
        if (cycle_active)
            d_observer->startProcessingCycle();
    }

    if (activeHints()->activityControlHint() == ObserverHints::FollowSelection && d->activity_filter) {
        selectObjects(d->activity_filter->activeSubjects());
    } else {
        selectObjects(current_selection);
    }

    setWindowTitle(d_observer->observerName());

    if (d->navigation_bar)
        d->navigation_bar->refreshHierarchy();
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

    // Setup new observer
    Observer* observer = OBJECT_MANAGER->observerReference(d->navigation_stack.pop());
    Q_ASSERT(observer);

    setObserverContext(observer);
    initialize();
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
    new_child_widget->selectObjects(selectedObjects());

    newObserverWidgetCreated(new_child_widget);
}

void Qtilities::CoreGui::ObserverWidget::selectionPushDown() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView) {
        // Set up new observer
        QObject* obj = d->current_selection.front();
        Observer* observer = qobject_cast<Observer*> (obj);
        if (!observer) {
            // Handle the cases where the current object has an observer child
            foreach (QObject* child, obj->children()) {
                Observer* child_observer = qobject_cast<Observer*> (child);
                if (child_observer) {
                    observer = child_observer;
                    // For now we break, when there is cases where an object has more than 1 observer child,
                    // a pop up must prompt the user to choose which one to push into. This should however
                    // not be neccesarry because the whole idea of allowing categorized observer subjects
                    // is to avoid having multiple observer children. However if the need for it arises, this
                    // code can be changed.
                    break;
                }
            }
        }

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

        // First disconnet all current observer connections
        d_observer->disconnect(this);
        d_observer->disconnect(d->navigation_bar);

        // Set up widget to use new observer
        d->navigation_stack.push(d_observer->observerID());
        connect(d_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication,QList<QPointer<QObject> >)),SLOT(contextDetachHandler(Observer::SubjectChangeIndication,QList<QPointer<QObject> >)));
        setObserverContext(observer);
        initialize();
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
        // Set up new observer
        if (d->current_selection.count() > 0) {
            QObject* obj = d->current_selection.front();
            observer = qobject_cast<Observer*> (obj);
            if (!observer) {
                // Handle the cases where the current object has an observer child
                foreach (QObject* child, obj->children()) {
                    Observer* child_observer = qobject_cast<Observer*> (child);
                    if (child_observer) {
                        observer = child_observer;
                        // For now we break, when there is cases where an object has more than 1 observer child,
                        // a pop up must prompt the user to choose which one to push into. This should however
                        // not be neccesarry because the whole idea of allowing categorized observer subjects
                        // is to avoid having multiple observer children. However if the need for it arises, this
                        // code can be changed.
                        break;
                    }
                }
            }
        }

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

    if (d->display_mode == TableView) {
        QList<QObject*> selected_objects = selectedObjects();
        d->display_mode = TreeView;
        // We need to clear the navigation stack if we move to tree view. The tree view will set the stack
        // automatically depending on what is selected when switching back to table view.
        // setNavigationStack(QStack<int>());
        setObserverContext(d->top_level_observer);
        d->update_selection_activity = false;
        initialize();
        d->update_selection_activity = true;
        selectObjects(selected_objects);
        d->tree_view->setFocus();
        if (d->searchBoxWidget)
            handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
    } else if (d->display_mode == TreeView && d->tree_model) {
        if (selectedIndexes().count() > 0) {
            d->navigation_stack = d->tree_model->getParentHierarchy(selectedIndexes().front());

            bool pop_back = true;
            Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
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

            if (d->navigation_stack.count() > 0 && pop_back)
                d->navigation_stack.pop_back();
        }
        d->display_mode = TableView;
        initialize();
        d->table_view->setFocus();
        if (d->searchBoxWidget)
            handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
    }

    refreshActions();
}

void Qtilities::CoreGui::ObserverWidget::viewCollapseAll() {
    if (d->tree_view && d->display_mode == TreeView) {
        d->tree_view->expandToDepth(0);
    }
}

void Qtilities::CoreGui::ObserverWidget::viewExpandAll() {
    if (d->tree_view && d->display_mode == TreeView) {
        d->tree_view->expandAll();
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

        connect(d->searchBoxWidget,SIGNAL(searchOptionsChanged()),SLOT(handleSearchOptionsChanged()));
        connect(d->searchBoxWidget,SIGNAL(searchStringChanged(QString)),SLOT(handleSearchStringChanged(QString)));
        connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),ui->widgetSearchBox,SLOT(hide()));
        connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),SLOT(resetProxyModel()));
        if (d->table_view && d->display_mode == TableView) {
            connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),d->table_view,SLOT(setFocus()));
        } else if (d->tree_view && d->display_mode == TreeView) {
            connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),d->tree_view,SLOT(setFocus()));
        }

        QMenu* search_options_menu = d->searchBoxWidget->searchOptionsMenu();
        if (search_options_menu) {
            d->actionFilterTypeSeperator = search_options_menu->addSeparator();
            d->actionFilterNodes = new QAction(tr("Filter Nodes"),this);
            d->actionFilterNodes->setCheckable(true);
            d->actionFilterNodes->setChecked(false);
            connect(d->actionFilterNodes,SIGNAL(triggered()),SLOT(handleSearchItemTypesChanged()));
            search_options_menu->addAction(d->actionFilterNodes);
            d->actionFilterItems = new QAction(tr("Filter Items"),this);
            d->actionFilterItems->setCheckable(true);
            d->actionFilterItems->setChecked(true);
            connect(d->actionFilterItems,SIGNAL(triggered()),SLOT(handleSearchItemTypesChanged()));
            search_options_menu->addAction(d->actionFilterItems);
            d->actionFilterCategories = new QAction(tr("Filter Categories"),this);
            d->actionFilterCategories->setCheckable(true);
            d->actionFilterCategories->setChecked(false);
            connect(d->actionFilterCategories,SIGNAL(triggered()),SLOT(handleSearchItemTypesChanged()));
            search_options_menu->addAction(d->actionFilterCategories);
            if (d->display_mode == TableView) {
                d->actionFilterCategories->setVisible(false);
                d->actionFilterNodes->setVisible(false);
                d->actionFilterItems->setVisible(false);
                d->actionFilterTypeSeperator->setVisible(false);
            }
        }
    }

    if (!ui->widgetSearchBox->isVisible()) {
        ui->widgetSearchBox->show();
        d->searchBoxWidget->setEditorFocus();
        handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
    } else {
        ui->widgetSearchBox->hide();
        resetProxyModel();
        if (d->table_view && d->display_mode == TableView) {
            d->table_view->setFocus();
        } else if (d->tree_view && d->display_mode == TreeView) {
            d->tree_view->setFocus();
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handleSearchItemTypesChanged() {
    // Only do something in tree view
    if (d->display_mode == TreeView && d->proxy_model) {
        ObserverTreeModelProxyFilter* proxy = dynamic_cast<ObserverTreeModelProxyFilter*> (d->proxy_model);
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

    // Refresh actions
    refreshActions();

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
        #endif
        selection_parent = d_observer;

        if (d->activity_filter && d->update_selection_activity) {
            // Check if the observer has a FollowSelection activity policy:
            // We only follow selection in table view mode for now.
            if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
                d->update_selection_activity = false;
                d->activity_filter->setActiveSubjects(object_list);
                d->update_selection_activity = true;
            }
        }

    } else if (d->display_mode == TreeView) {
        if (d->update_selection_activity)
            selection_parent = d->tree_model->calculateSelectionParent(selectedIndexes());
        // In this case the FollowSelection hint check will happen in the setTreeSelectionParent slot.
    }

    // Emit signals
    emit selectedObjectsChanged(object_list, selection_parent);
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
            refreshActions();
            d->initialized = true;
            if (d->tree_view)
                d->tree_view->setEnabled(false);
        } else {
            if (!d->top_level_observer) {
                d->initialized = false;
                refreshActions();
                d->initialized = true;
                if (d->tree_view)
                    d->tree_view->setEnabled(false);
            } else {
                setObserverContext(d->top_level_observer);
                initialize();
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
        refreshActions();
        d->initialized = true;
        if (d->table_view)
            d->table_view->setEnabled(false);
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

void Qtilities::CoreGui::ObserverWidget::clearSelection() {
    QList<QObject*> objects;
    selectObjects(objects);
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
            viewExpandAll();
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

    if (objects.count() == 0) {
        if (d->table_view) {
            if (d->table_view->selectionModel())
                d->table_view->selectionModel()->clear();
        } else if (d->tree_view) {
            if (d->tree_view->selectionModel())
                d->tree_view->selectionModel()->clear();
            viewExpandAll();
        }
        return;
    }

    // Handle for the table view
    if (d->table_view && d->table_model && d->display_mode == TableView && d->proxy_model) {
        d->update_selection_activity = false;

        QModelIndexList indexes;
        QModelIndex index;
        for (int i = 0; i < objects.count(); i++) {
            index = d->table_model->getIndex(objects.at(i));
            if (index.isValid())
                indexes << index;
        }

        QItemSelectionModel *selection_model = d->table_view->selectionModel();
        if (selection_model) {
            QItemSelection item_selection;
            for (int i = 0; i < indexes.count(); i++) {
                d->table_view->selectRow(d->proxy_model->mapFromSource(indexes.at(i)).row());
                item_selection.merge(d->table_view->selectionModel()->selection(),QItemSelectionModel::Select);
            }

            selection_model->clearSelection();
            selection_model->select(item_selection,QItemSelectionModel::Select);
            selectedObjects();
            updateGlobalActiveSubjects();
        }

        d->update_selection_activity = true;

        // Update the property browser:
        #ifdef QTILITIES_PROPERTY_BROWSER
            refreshPropertyBrowser();
        #endif
    } else if (d->tree_view && d->tree_model && d->display_mode == TreeView && d->proxy_model) {
        d->update_selection_activity = false;

        QModelIndexList indexes;
        QModelIndex index;
        for (int i = 0; i < objects.count(); i++) {
            index = d->tree_model->findObject(objects.at(i));
            if (index.isValid()) {
                d->tree_view->scrollTo(d->proxy_model->mapFromSource(index),QAbstractItemView::EnsureVisible);
                indexes << index;
            }
        }

        QItemSelectionModel *selection_model = d->tree_view->selectionModel();
        if (selection_model) {
            QItemSelection item_selection;
            for (int i = 0; i < indexes.count(); i++) {
                QModelIndex mapped_index = d->proxy_model->mapFromSource(indexes.at(i));
                item_selection.select(mapped_index,mapped_index);
            }

            selection_model->clearSelection();
            selection_model->select(item_selection,QItemSelectionModel::Select);
            selectedObjects();
            updateGlobalActiveSubjects();
        }

        d->update_selection_activity = true;

        // Update the property browser:
        #ifdef QTILITIES_PROPERTY_BROWSER
            refreshPropertyBrowser();
        #endif
    }
}

void Qtilities::CoreGui::ObserverWidget::handleSearchOptionsChanged() {
    handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
}

void Qtilities::CoreGui::ObserverWidget::handleSearchStringChanged(const QString& filter_string) {
    QRegExp::PatternSyntax syntax;
    if (d->searchBoxWidget->regExpression())
        syntax = QRegExp::PatternSyntax(QRegExp::RegExp);
    else
        syntax = QRegExp::PatternSyntax(QRegExp::FixedString);
    Qt::CaseSensitivity caseSensitivity = d->searchBoxWidget->caseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QRegExp regExp(filter_string, caseSensitivity, syntax);

    // Check if the installed proxy model is a QSortFilterProxyModel:
    QSortFilterProxyModel* model = qobject_cast<QSortFilterProxyModel*> (d->proxy_model);
    if (model)
        model->setFilterRegExp(regExp);
}

void Qtilities::CoreGui::ObserverWidget::resetProxyModel() {
    handleSearchStringChanged("");
}

#ifdef QTILITIES_PROPERTY_BROWSER
void Qtilities::CoreGui::ObserverWidget::refreshPropertyBrowser() {
    // Update the property editor visibility and object
    if (activeHints()->displayFlagsHint() & ObserverHints::PropertyBrowser) {
        constructPropertyBrowser();
        if (selectedObjects().count() == 1)
            d->property_browser_widget->setObject(d->current_selection.front());
        else
            d->property_browser_widget->setObject(d_observer);
        addDockWidget(d->property_editor_dock_area, d->property_browser_dock);

        // Resize the doc depending on where it is:
        //if (d->property_editor_dock_area == Qt::LeftDockWidgetArea || d->property_editor_dock_area == Qt::RightDockWidgetArea)
        //    d->property_browser_widget->setFixedWidth(d->property_browser_widget->sizeHint().width());
        if (d->property_editor_dock_area == Qt::TopDockWidgetArea || d->property_editor_dock_area == Qt::BottomDockWidgetArea)
            d->property_browser_widget->setMaximumHeight(d->property_browser_widget->sizeHint().height());
        d->property_browser_dock->show();
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
#endif

void Qtilities::CoreGui::ObserverWidget::refreshActionToolBar(bool force_full_refresh) {
    // Check if an action toolbar should be created:
    if ((activeHints()->displayFlagsHint() & ObserverHints::ActionToolBar) && d->action_provider) {
        if (!force_full_refresh) {
            if (d->last_display_flags != activeHints()->displayFlagsHint() || !d->initialized)
                d->last_display_flags = activeHints()->displayFlagsHint();
            else {
                // Here we need to hide all toolbars that does not contain any actions:
                // Now create all toolbars:
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

        // First delete all toolbars:
        int toolbar_count = d->action_toolbars.count();
        if (toolbar_count > 0) {
            for (int i = 0; i < toolbar_count; i++) {
                QToolBar* toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(0));
                removeToolBar(toolbar);
                if (toolbar) {
                    d->action_toolbars.removeOne(toolbar);
                    delete toolbar;
                }
            }
        }

        // Now create all toolbars:
        QList<QtilitiesCategory> categories = d->action_provider->actionCategories();
        for (int i = 0; i < categories.count(); i++) {
            QList<QAction*> action_list = d->action_provider->actions(IActionProvider::NoFilter,categories.at(i));
            if (action_list.count() > 0) {
                QToolBar* new_toolbar = addToolBar(categories.at(i).toString());
                new_toolbar->setObjectName(categories.at(i).toString());
                d->action_toolbars << new_toolbar;
                new_toolbar->addActions(action_list);
            }
        }

        if (force_full_refresh) {
            // Here we need to hide all toolbars that does not contain any actions:
            // Now create all toolbars:
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
                delete toolbar;
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::changeEvent(QEvent *e)
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

bool Qtilities::CoreGui::ObserverWidget::eventFilter(QObject *object, QEvent *event) {
    if (!d->initialized)
        return false;

    // ----------------------------------------------
    // Double Click Signal Emitters
    // ----------------------------------------------
    // -> TreeView Mode:
    // ----------------------------------------------
    if (d->tree_view && d->tree_model && d->display_mode == TreeView) {
        if (object == d->tree_view->viewport() && event->type() == QEvent::MouseButtonDblClick) {
            if (d->current_selection.count() == 1 && selectedIndexes().front().column() == d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName)) {
                // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
                bool use_parent_context = true;
                Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
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
    }
    // ----------------------------------------------
    // -> TableView Mode:
    // ----------------------------------------------
    if (d->table_view && d->table_model && d->display_mode == TableView) {
        if (object == d->table_view->viewport() && event->type() == QEvent::MouseButtonDblClick) {
           if (d->current_selection.count() == 1) {
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
            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent *>(event);
            dragMoveEvent->accept();
            return false;
        } else if (object == this && event->type() == QEvent::Drop && !d->read_only) {
            QDropEvent *dropEvent = static_cast<QDropEvent *>(event);

            if (!d->initialized)
                return false;

            if (dropEvent->proposedAction() == Qt::MoveAction || dropEvent->proposedAction() == Qt::CopyAction) {
                const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (dropEvent->mimeData());
                if (observer_mime_data) {
                    if (observer_mime_data->sourceID() == d_observer->observerID()) {
                        LOG_ERROR(QString(tr("The drop operation could not be completed. The destination and source is the same.")));
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
                                LOG_WARNING(QString(tr("The drop operation completed partially. %1/%2 objects were drop successfully.").arg(dropped_list.count()).arg(observer_mime_data->subjectList().count())));
                            } else {
                                LOG_INFO(QString(tr("The drop operation completed successfully on %1 objects.").arg(dropped_list.count())));
                            }
                        }
                    } else
                        LOG_ERROR(QString(tr("The drop operation could not be completed. The destination observer cannot accept all the objects in your selection. Error message: ") + error_msg));
                }
            }
            return false;
        } else if (object == d->table_view && event->type() == QEvent::DragEnter && !d->read_only) {
            QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent *>(event);
            dragEnterEvent->accept();
            return false;
        } else if (object == d->table_view->viewport() && event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->buttons() == Qt::LeftButton)
               d->startPos = mouseEvent->pos();
            return false;
        } else if (object == d->table_view->viewport() && event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

            int distance = (mouseEvent->pos() - d->startPos).manhattanLength();
            if (distance >= QApplication::startDragDistance()) {
                if (mouseEvent->buttons() == Qt::LeftButton || mouseEvent->buttons() == Qt::RightButton) {
                    if (d->current_selection.size() > 0 && d->initialized && d->table_model) {
                        ObserverMimeData* mimeData;
                        QDrag *drag = new QDrag(this);
                        if (mouseEvent->buttons() == Qt::LeftButton) {
                            mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID(),Qt::CopyAction);
                            drag->setMimeData(mimeData);
                            drag->exec(Qt::CopyAction);
                        } else if (mouseEvent->buttons() == Qt::RightButton) {
                            mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID(),Qt::MoveAction);
                            drag->setMimeData(mimeData);
                            drag->exec(Qt::MoveAction);
                        }
                    }
                }
            }
            return false;
        }
    }

    // ----------------------------------------------
    // -> TreeView Mode:
    // ----------------------------------------------
    if (d->tree_view && d->tree_model && d->display_mode == TreeView) {
        if (object == d->tree_view && event->type() == QEvent::DragMove && !d->read_only) {
            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent *>(event);
            dragMoveEvent->accept();
            return false;
        } else if (object == d->tree_view && event->type() == QEvent::DragEnter && !d->read_only) {
            QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent *>(event);
            dragEnterEvent->accept();
            return false;
        } else if (object == d->tree_view->viewport() && event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->buttons() == Qt::LeftButton)
               d->startPos = mouseEvent->pos();
            return false;
        } else if (object == d->tree_view->viewport() && event->type() == QEvent::MouseMove) {
            if (!d->initialized)
                return false;
            QMouseEvent* mouseEvent = static_cast<QMouseEvent *>(event);

            int distance = (mouseEvent->pos() - d->startPos).manhattanLength();
            if (distance >= QApplication::startDragDistance()) {
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
                if (mouseEvent->buttons() == Qt::LeftButton)
                    mimeData = new ObserverMimeData(smart_objects,obs->observerID(),Qt::CopyAction);
                else if (mouseEvent->buttons() == Qt::RightButton)
                    mimeData = new ObserverMimeData(smart_objects,obs->observerID(),Qt::MoveAction);
                CLIPBOARD_MANAGER->setMimeData(mimeData);
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
