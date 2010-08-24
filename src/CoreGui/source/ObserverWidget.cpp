/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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
#include "ActionProvider.h"

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
    ObserverWidgetData() : actionActionRemoveItem(0),
    actionActionRemoveAll(0),
    actionActionDeleteItem(0),
    actionActionDeleteAll(0),
    actionActionNewItem(0),
    actionActionRefreshView(0),
    actionActionPushUp(0),
    actionActionPushUpNew(0),
    actionActionPushDown(0),
    actionActionPushDownNew(0),
    actionActionSwitchView(0),
    actionExpandAll(0),
    actionCollapseAll(0),
    actionActionFindItem(0),
    proxy_model(0),
    activity_filter(0),
    searchBoxWidget(0),
    action_provider(0) { }

    QAction* actionActionRemoveItem;
    QAction* actionActionRemoveAll;
    QAction* actionActionDeleteItem;
    QAction* actionActionDeleteAll;
    QAction* actionActionNewItem;
    QAction* actionActionRefreshView;
    QAction* actionActionPushUp;
    QAction* actionActionPushUpNew;
    QAction* actionActionPushDown;
    QAction* actionActionPushDownNew;
    QAction* actionActionSwitchView;
    QAction* actionExpandAll;
    QAction* actionCollapseAll;
    QAction* actionActionFindItem;

    //! The navigation stack of this widget, used only in TableView mode.
    QStack<int> navigation_stack;
    //! The navigation bar.
    ObjectHierarchyNavigator* navigation_bar;
    //! The action toolbar.
    QToolBar* action_toolbar;
    //! The start position point used during drag & drop operations.
    QPoint startPos;

    Qtilities::CoreGui::ObserverWidget::DisplayMode display_mode;
    QPointer<QTableView> table_view;
    AbstractObserverTableModel* table_model;
    QPointer<QTreeView> tree_view;
    AbstractObserverTreeModel* tree_model;
    QSortFilterProxyModel *proxy_model;
    NamingPolicyDelegate* table_name_column_delegate;
    NamingPolicyDelegate* tree_name_column_delegate;
    ActivityPolicyFilter* activity_filter;

    //! Used to identify the top level observer. d_observer is current selection parent observer.
    QPointer<Observer> top_level_observer;

    QDockWidget* property_browser_dock;
    ObjectPropertyBrowser* property_browser_widget;

    //! Indicates if the widget is in an initialized state. Thus initialization was successful. \sa initialize()
    bool initialized;
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

    Qt::DockWidgetArea property_editor_dock_area;
    ObjectPropertyBrowser::BrowserType property_editor_type;
    QStringList appended_contexts;

    //! The current selection in this widget. Set in the selectedObjects() function.
    QList<QObject*> current_selection;
    //! The search box widget.
    SearchBoxWidget* searchBoxWidget;
    //! The IActionProvider interface implementation.
    ActionProvider* action_provider;
    //! The default row height used in TableView mode.
    int default_row_height;
};

Qtilities::CoreGui::ObserverWidget::ObserverWidget(DisplayMode display_mode, QWidget * parent, Qt::WindowFlags f) :
    QMainWindow(parent, f),
    ui(new Ui::ObserverWidget)
{
    d = new ObserverWidgetData;
    d->action_provider = new ActionProvider(this);

    d->property_browser_dock = 0;
    d->property_browser_widget = 0;
    d->navigation_bar = 0;
    d->action_toolbar = 0;
    d->display_mode = display_mode;

    d->table_model = 0;
    d->table_view = 0;
    d->table_name_column_delegate = 0;

    d->tree_model = 0;
    d->tree_view = 0;
    d->tree_name_column_delegate = 0;

    d->top_level_observer = 0;
    d->default_row_height = 17;

    // Init rest of parameters
    d->hints_default = new ObserverHints(this);
    d->hints_selection_parent = 0;
    d->use_observer_hints = true;

    d->update_global_active_objects = false;
    d->initialized = false;
    d->update_selection_activity = true;
    d->property_editor_dock_area = Qt::RightDockWidgetArea;
    d->property_editor_type = ObjectPropertyBrowser::TreeBrowser;
    ui->setupUi(this);

    setWindowIcon(QIcon(ICON_QTILITIES_SYMBOL_WHITE_16x16));
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
    d->global_meta_type = context_string;
    d->shared_global_meta_type = QString();
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

Qtilities::Core::ObserverHints* Qtilities::CoreGui::ObserverWidget::activeHints() {
    if (d->use_observer_hints && d->hints_selection_parent)
        return d->hints_selection_parent;
    else
        return d->hints_default;
}

bool Qtilities::CoreGui::ObserverWidget::setObserverContext(Observer* observer) {
    if (!observer)
        return false;

    if (d_observer)
        d_observer->disconnect(this);

    // Connect to the distroyed signal
    connect(observer,SIGNAL(destroyed()),SLOT(contextDeleted()));

    if (d->top_level_observer) {
        // It was set in the navigation stack, don't change it.
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
    return true;
}

int Qtilities::CoreGui::ObserverWidget::topLevelObserverID() {
    if (d->top_level_observer)
        return d->top_level_observer->observerID();
    else
        return -1;
}

bool Qtilities::CoreGui::ObserverWidget::setCustomTableModel(AbstractObserverTableModel* table_model) {
    if (d->initialized)
        return false;

    if (d->table_model)
        return false;

    if (!table_model)
        return false;

    d->table_model = table_model;
    return true;
}

bool Qtilities::CoreGui::ObserverWidget::setCustomTreeModel(AbstractObserverTreeModel* tree_model) {
    if (d->initialized)
        return false;

    if (d->tree_model)
        return false;

    if (!tree_model)
        return false;

    d->tree_model = tree_model;
    return true;
}

void Qtilities::CoreGui::ObserverWidget::initialize(bool hints_only) {
    // Check it this widget was initialized previously
    if (!d->initialized) {
        // Setup some flags and attributes for this widget the first time it is constructed.
        setAttribute(Qt::WA_DeleteOnClose, true);
        // Register contextString in the context manager.
        OBJECT_MANAGER->registerObject(this);
        constructActions();
    }

    d->initialized = false;

    if (!d_observer) {
        LOG_FATAL("You are attempting to initialize an ObserverWidget without an observer context.");
        d->action_provider->disableAllActions();
        d->initialized = false;
        return;
    }

    // Set the title and name of the observer widget.
    // Here we need to check if we must use d_observer inside a specific context
    setWindowTitle(d_observer->objectName());
    setObjectName(d_observer->objectName());

    // Get hints from d_observer:
    if (d->use_observer_hints) {
        // Check if this observer provides hints for this model
        d->hints_selection_parent = d_observer->displayHints();

        // Check if the observer does not specify flags, if so we use the defaults
        if (activeHints()->displayFlagsHint() == ObserverHints::NoDisplayFlagsHint)
            activeHints()->setDisplayFlagsHint(ObserverHints::ItemView);
    }

    // Hide the search bar if it is visible
    ui->widgetSearchBox->hide();
    if (d->searchBoxWidget) {
        delete d->searchBoxWidget;
        d->searchBoxWidget = 0;
    }

    if (!hints_only) {
        // Delete the current layout on itemParentWidget
        if (ui->itemParentWidget->layout())
            delete ui->itemParentWidget->layout();

        // Set the title of this widget
        setWindowTitle(QString("%1").arg(d_observer->objectName()));

        // Delete the proxy model if it exists already.
        if (d->proxy_model)
            delete d->proxy_model;

        // Check and setup the item display mode
        if (d->display_mode == TreeView) {
            connect(d_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));
            connect(d->top_level_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));
            d->proxy_model = new QSortFilterProxyModel(this);
            d->proxy_model->setDynamicSortFilter(true);

            if (d->table_view)
                d->table_view->hide();

            // Check if there is already a model.
            if (!d->tree_view) {
                d->tree_view = new QTreeView(ui->itemParentWidget);
                d->tree_view->setFocusPolicy(Qt::StrongFocus);
                d->tree_view->setRootIsDecorated(true);
                if (!d->tree_model)
                    d->tree_model = new ObserverTreeModel();
                d->tree_view->setSortingEnabled(true);
                d->tree_view->sortByColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName),Qt::AscendingOrder);
                connect(d->tree_model,SIGNAL(selectionParentChanged(Observer*)),SLOT(setTreeSelectionParent(Observer*)));

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

            d->tree_model->setObserverContext(d->top_level_observer);
            d->tree_view->setItemDelegate(d->tree_name_column_delegate);

            // Setup proxy model
            d->proxy_model->setSourceModel(d->tree_model);
            d->proxy_model->setFilterKeyColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnName));
            d->tree_view->setModel(d->proxy_model);

            //d->tree_view->header()->hide();
            if (d->tree_view->selectionModel())
                connect(d->tree_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()));

            // The view must always be visible.
            d->tree_view->setVisible(true);
            d->tree_view->expandAll();
        } else if (d->display_mode == TableView) {
            // Connect to the current parent observer, in the tree view the model will monitor this for you.
            connect(d_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));
            //connect(d_observer,SIGNAL(nameChanged(QString)),SLOT(setWindowTitle(QString)));
            d->proxy_model = new ObserverTableModelCategoryFilter(this);
            d->proxy_model->setDynamicSortFilter(true);

            if (d->tree_view)
                d->tree_view->hide();

            // Check if there is already a model.
            if (!d->table_view) {
                d->table_view = new QTableView();
                d->table_view->setFocusPolicy(Qt::StrongFocus);
                d->table_view->setAcceptDrops(true);
                d->table_view->setDragEnabled(true);
                if (!d->table_model)
                    d->table_model = new ObserverTableModel();
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

            d->table_model->setObserverContext(d_observer);
            d->table_name_column_delegate->setObserverContext(d_observer);

            // Setup the table view to look nice
            d->table_view->setShowGrid(false);
            d->table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
            d->table_view->verticalHeader()->setVisible(false);

            // Setup proxy model
            d->proxy_model->setSourceModel(d->table_model);
            d->proxy_model->setFilterKeyColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName));
            d->table_view->setModel(d->proxy_model);

            if (d->table_view->selectionModel()) {
                d->table_view->selectionModel()->clear();
                connect(d->table_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()));
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

    // Check if an action toolbar should be created:
    if (activeHints()->displayFlagsHint() & ObserverHints::ActionToolBar && d->action_provider) {
        if (!d->action_toolbar) {
            d->action_toolbar = addToolBar("Context Toolbar");
            d->action_toolbar->addActions(d->action_provider->actions());
        }
    } else {
        if (d->action_toolbar) {
            removeToolBar(d->action_toolbar);
            if (d->action_toolbar) {
                delete d->action_toolbar;
                d->action_toolbar = 0;
            }
        }
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
            //d->table_view->setColumnWidth(d->table_model->columnPosition(AbstractObserverItemModel::ColumnSubjectID),50);
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnCategoryHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
        else {
            if (activeHints()->hierarchicalDisplayHint() & ObserverHints::CategorizedHierarchy) {
                d->table_view->showColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
                //d->table_view->setColumnWidth(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory),50);
            } else {
                d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnCategory));
            }
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnChildCountHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
        else {
            d->table_view->showColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnChildCount));
            //d->table_view->setColumnWidth(d->table_model->columnPosition(AbstractObserverItemModel::ColumnChildCount),50);
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
        else {
            d->table_view->showColumn(AbstractObserverItemModel::ColumnTypeInfo);
            //d->table_view->setColumnWidth(d->table_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo),50);
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint))
            d->table_view->hideColumn(d->table_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
        else {
            d->table_view->showColumn(AbstractObserverItemModel::ColumnAccess);
            //d->table_view->setColumnWidth(d->table_model->columnPosition(AbstractObserverItemModel::ColumnAccess),50);
        }

        // Resize columns
        d->table_view->resizeColumnsToContents();
        QHeaderView* table_header = d->table_view->horizontalHeader();
        table_header->setResizeMode(d->table_model->columnPosition(AbstractObserverItemModel::ColumnName),QHeaderView::Stretch);
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
            d->tree_view->setColumnWidth(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnChildCount),50);
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnTypeInfoHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo));
            d->tree_view->setColumnWidth(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnTypeInfo),50);
        }

        if (!(activeHints()->itemViewColumnHint() & ObserverHints::ColumnAccessHint))
            d->tree_view->hideColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
        else {
            d->tree_view->showColumn(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess));
            d->tree_view->setColumnWidth(d->tree_model->columnPosition(AbstractObserverItemModel::ColumnAccess),50);
        }

        // Set name column width to something biggish for now. Will fix later.
        // Also fix in subjectCountChanged() slot.
        d->tree_view->setColumnWidth(0,300);

        //QHeaderView* tree_header = d->tree_view->header();
        //tree_header->setResizeMode(d->tree_model->columnPosition(AbstractObserverItemModel::NameColumn),QHeaderView::Stretch);
    }

    d->initialized = true;
    if (!hints_only) {
        // Construct the property browser if neccesarry
        refreshPropertyBrowser();
        installEventFilter(this);
    }

    // Handle initial selections:
    if (d->use_observer_hints) {
        // Check if this observer provides hints for this model
        d->hints_selection_parent = d_observer->displayHints();

        // Check if the observer does not specify flags, if so we use the defaults
        if (activeHints()->displayFlagsHint() == ObserverHints::NoDisplayFlagsHint)
            activeHints()->setDisplayFlagsHint(ObserverHints::ItemView);

        bool create_default_selection = true;
        if (activeHints() && (d->display_mode == TableView)) {
            // Ok since the new observer provides hints, we need to see if we must select its' active objects:
            // Check if the observer has a FollowSelection actity policy
            // In that case the observer widget, in table mode must select objects which are active and adapt to changes in the activity filter.
            if (d_observer->displayHints()->activityControlHint() == ObserverHints::FollowSelection) {
                // Check if the observer has a activity filter, which it should have with this hint
                ActivityPolicyFilter* filter = 0;
                for (int i = 0; i < d_observer->subjectFilters().count(); i++) {
                    filter = qobject_cast<ActivityPolicyFilter*> (d_observer->subjectFilters().at(i));
                    if (filter) {
                        d->activity_filter = filter;

                        // Connect to the activity change signal (to update activity on observer widget side)
                        connect(d->activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(selectSubjectsInTable(QList<QObject*>)));
                        selectSubjectsInTable(d->activity_filter->activeSubjects());
                        create_default_selection = false;
                    }
                }
            } else {
                if (d->activity_filter)
                    d->activity_filter->disconnect(this);
                d->activity_filter = 0;
            }

            if (create_default_selection) {
                // Create an initial selection.
                QList<QObject*> initial_selection;
                if (d_observer->subjectCount() > 0)
                    initial_selection << d_observer->subjectAt(0);
                selectSubjectsInTable(initial_selection);
            }
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
            connect(stack_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication,QList<QObject*>)),SLOT(contextDetachHandler(Observer::SubjectChangeIndication,QList<QObject*>)));
    }

}

void Qtilities::CoreGui::ObserverWidget::toggleGrid(bool toggle) {
    if (d->display_mode == TableView && d->table_view)
        d->table_view->setShowGrid(toggle);
}

void Qtilities::CoreGui::ObserverWidget::toggleAlternatingRowColors(bool toggle) {
    if (d->display_mode == TableView && d->table_view)
        d->table_view->setAlternatingRowColors(toggle);
    else if (d->display_mode == TreeView && d->tree_view)
        d->tree_view->setAlternatingRowColors(toggle);
}

void Qtilities::CoreGui::ObserverWidget::toggleUseObserverHints(bool toggle) {
    d->use_observer_hints = toggle;
}

QList<QObject*> Qtilities::CoreGui::ObserverWidget::selectedObjects() const {
    QList<QObject*> selected_objects;

    if (d->display_mode == TableView) {
        if (!d->table_view || !d->table_model)
            return selected_objects;

        if (d->table_view->selectionModel()) {          
            for (int i = 0; i < d->table_view->selectionModel()->selectedIndexes().count(); i++) {
                QModelIndex index = d->table_view->selectionModel()->selectedIndexes().at(i);
                if (index.column() == 1)
                    selected_objects << d->table_model->getObject(d->proxy_model->mapToSource(index));
            }
        }
    } else if (d->display_mode == TreeView) {
        if (!d->tree_view || !d->tree_model)
            return selected_objects;

        if (d->tree_view->selectionModel()) {
            for (int i = 0; i < d->tree_view->selectionModel()->selectedIndexes().count(); i++) {
                QModelIndex index = d->tree_view->selectionModel()->selectedIndexes().at(i);
                if (index.column() == 0)
                    selected_objects << d->tree_model->getObject(d->proxy_model->mapToSource(index));
            }
        }
    }
    d->current_selection = selected_objects;
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

void Qtilities::CoreGui::ObserverWidget::writeSettings() {
    if (!d->initialized)
        return;

    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup(d->global_meta_type);
    settings.setValue("display_mode", (int) d->display_mode);
    settings.setValue("state", saveState());
    settings.setValue("default_row_heigth", d->default_row_height);
    if (d->table_view)
        settings.setValue("table_view_show_grid", d->table_view->showGrid());
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::ObserverWidget::readSettings() {
    if (!d->initialized) {
        LOG_ERROR(QString(tr("readSettings() can only be called on observer widget \"%1\" after the widget has been initialized.")).arg(d->global_meta_type));
        return;
    }

    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup(d->global_meta_type);

    // Display mode
    if (d->display_mode != settings.value("display_mode", TableView).toInt()) {
        if (d->display_mode == TableView) {
            d->display_mode = TreeView;
            setObserverContext(d->top_level_observer);
            initialize();
            if (d->tree_view)
                d->tree_view->expandAll();
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

    settings.endGroup();
    settings.endGroup();

    // Connect in order for settings to be written when application quits
    connect(QCoreApplication::instance(),SIGNAL(aboutToQuit()),SLOT(writeSettings()));
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

void Qtilities::CoreGui::ObserverWidget::inheritDisplayHints(ObserverHints* display_hints) {
    d->hints_selection_parent = display_hints;
}

void Qtilities::CoreGui::ObserverWidget::handleSettingsUpdateRequest(const QString& request_id) {
    if (request_id == d->global_meta_type)
        readSettings();
}

Qtilities::CoreGui::ObjectPropertyBrowser* Qtilities::CoreGui::ObserverWidget::propertyBrowser() {
    return d->property_browser_widget;
}

void Qtilities::CoreGui::ObserverWidget::setPreferredPropertyEditorDockArea(Qt::DockWidgetArea property_editor_dock_area) {
    d->property_editor_dock_area = property_editor_dock_area;
}

void Qtilities::CoreGui::ObserverWidget::setPreferredPropertyEditorType(ObjectPropertyBrowser::BrowserType property_editor_type) {
    d->property_editor_type = property_editor_type;
}

void Qtilities::CoreGui::ObserverWidget::resizeTableViewRows(int height) {
    if (height == -1)
        height = d->default_row_height;
    if (d->display_mode == TableView && d->table_view && d->table_model) {
        for (int i = 0; i < d->table_model->rowCount(); i++) {
            d->table_view->setRowHeight(i,height);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::constructActions() {
    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(d->global_meta_type));

    // ---------------------------
    // Remove All
    // ---------------------------
    d->actionActionRemoveItem = new QAction(QIcon(ICON_REMOVE_ONE),tr("Remove Item"),this);
    d->action_provider->addAction(d->actionActionRemoveItem,QStringList(tr("Children")));
    connect(d->actionActionRemoveItem,SIGNAL(triggered()),SLOT(handle_actionActionRemoveItem_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_REMOVE_ITEM,d->actionActionRemoveItem,context);
    // ---------------------------
    // Remove All
    // ---------------------------    
    d->actionActionRemoveAll = new QAction(QIcon(ICON_REMOVE_ALL),tr("Remove All Children"),this);
    d->action_provider->addAction(d->actionActionRemoveAll,QStringList(tr("Children")));
    connect(d->actionActionRemoveAll,SIGNAL(triggered()),SLOT(handle_actionActionRemoveAll_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_REMOVE_ALL,d->actionActionRemoveAll,context);
    // ---------------------------
    // Delete All
    // ---------------------------
    d->actionActionDeleteAll = new QAction(QIcon(ICON_DELETE_ALL),tr("Delete All Children"),this);
    d->action_provider->addAction(d->actionActionDeleteAll,QStringList(tr("Children")));
    connect(d->actionActionDeleteAll,SIGNAL(triggered()),SLOT(handle_actionActionDeleteAll_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_DELETE_ALL,d->actionActionDeleteAll,context);
    // ---------------------------
    // Switch View
    // ---------------------------
    d->actionActionSwitchView = new QAction(QIcon(ICON_SWITCH_VIEW),tr("Switch View"),this);
    d->actionActionSwitchView->setShortcut(QKeySequence("F4"));
    d->action_provider->addAction(d->actionActionSwitchView,QStringList(tr("View")));
    connect(d->actionActionSwitchView,SIGNAL(triggered()),SLOT(handle_actionActionSwitchView_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_SWITCH_VIEW,d->actionActionSwitchView,context);
    // ---------------------------
    // Refresh View
    // ---------------------------
    d->actionActionRefreshView = new QAction(QIcon(ICON_REFRESH),tr("Refresh View"),this);
    d->action_provider->addAction(d->actionActionRefreshView,QStringList(tr("View")));
    connect(d->actionActionRefreshView,SIGNAL(triggered()),SLOT(handle_actionActionRefreshView_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_REFRESH_VIEW,d->actionActionRefreshView,context);
    // ---------------------------
    // New Item
    // ---------------------------
    d->actionActionNewItem = new QAction(QIcon(ICON_NEW),tr("New Item"),this);
    d->actionActionNewItem->setShortcut(QKeySequence("+"));
    d->action_provider->addAction(d->actionActionNewItem,QStringList(tr("Children")));
    connect(d->actionActionNewItem,SIGNAL(triggered()),SLOT(handle_actionActionNewItem_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_NEW_ITEM,d->actionActionNewItem,context);
    // ---------------------------
    // Go To Parent
    // ---------------------------
    d->actionActionPushUp = new QAction(QIcon(ICON_PUSH_UP_CURRENT),tr("Go To Parent"),this);
    d->actionActionPushUp->setShortcut(QKeySequence("Left"));
    d->action_provider->addAction(d->actionActionPushUp,QStringList(tr("Hierarchy")));
    connect(d->actionActionPushUp,SIGNAL(triggered()),SLOT(handle_actionActionPushUp_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_HIERARCHY_UP,d->actionActionPushUp,context);
    // ---------------------------
    // Go To Parent In New Window
    // ---------------------------
    d->actionActionPushUpNew = new QAction(QIcon(ICON_PUSH_UP_NEW),tr("Go To Parent (New Window)"),this);
    d->action_provider->addAction(d->actionActionPushUpNew,QStringList(tr("Hierarchy")));
    connect(d->actionActionPushUpNew,SIGNAL(triggered()),SLOT(handle_actionActionPushUpNew_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_HIERARCHY_UP_NEW,d->actionActionPushUpNew,context);
    // ---------------------------
    // Push Down
    // ---------------------------
    d->actionActionPushDown = new QAction(QIcon(ICON_PUSH_DOWN_CURRENT),tr("Push Down"),this);
    d->action_provider->addAction(d->actionActionPushDown,QStringList(tr("Hierarchy")));
    d->actionActionPushDown->setShortcut(QKeySequence("Right"));
    connect(d->actionActionPushDown,SIGNAL(triggered()),SLOT(handle_actionActionPushDown_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_HIERARCHY_DOWN,d->actionActionPushDown,context);
    // ---------------------------
    // Push Down In New Window
    // ---------------------------
    d->actionActionPushDownNew = new QAction(QIcon(ICON_PUSH_DOWN_NEW),tr("Push Down (New Window)"),this);
    d->action_provider->addAction(d->actionActionPushDownNew,QStringList(tr("Hierarchy")));
    connect(d->actionActionPushDownNew,SIGNAL(triggered()),SLOT(handle_actionActionPushDownNew_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_HIERARCHY_DOWN_NEW,d->actionActionPushDownNew,context);
    // ---------------------------
    // Expand All
    // ---------------------------
    d->actionExpandAll = new QAction(QIcon(ICON_MAGNIFY_PLUS),tr("Expand All"),this);
    d->action_provider->addAction(d->actionExpandAll,QStringList(tr("Hierarchy")));
    d->actionExpandAll->setShortcut(QKeySequence("Ctrl+>"));
    connect(d->actionExpandAll,SIGNAL(triggered()),SLOT(handle_actionExpandAll_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_HIERARCHY_EXPAND,d->actionExpandAll,context);
    // ---------------------------
    // Collapse All
    // ---------------------------
    d->actionCollapseAll = new QAction(QIcon(ICON_MAGNIFY_MINUS),tr("Collapse All"),this);
    d->action_provider->addAction(d->actionCollapseAll,QStringList(tr("Hierarchy")));
    d->actionCollapseAll->setShortcut(QKeySequence("Ctrl+<"));
    connect(d->actionCollapseAll,SIGNAL(triggered()),SLOT(handle_actionCollapseAll_triggered()));
    ACTION_MANAGER->registerAction(MENU_CONTEXT_HIERARCHY_COLLAPSE,d->actionCollapseAll,context);
    // ---------------------------
    // Delete Item
    // ---------------------------
    d->actionActionDeleteItem = new QAction(QIcon(ICON_DELETE_ONE),tr("Delete Item"),this);
    d->action_provider->addAction(d->actionActionDeleteItem,QStringList(tr("Item")));
    d->actionActionDeleteItem->setShortcut(QKeySequence(QKeySequence::Delete));
    connect(d->actionActionDeleteItem,SIGNAL(triggered()),SLOT(handle_actionActionDeleteItem_triggered()));
    ACTION_MANAGER->registerAction(MENU_SELECTION_DELETE,d->actionActionDeleteItem,context);
    // ---------------------------
    // Find Item
    // ---------------------------
    d->actionActionFindItem = new QAction(QIcon(ICON_MAGNIFY),tr("Find"),this);
    d->action_provider->addAction(d->actionActionFindItem,QStringList(tr("View")));
    d->actionActionFindItem->setShortcut(QKeySequence(QKeySequence::Find));
    connect(d->actionActionFindItem,SIGNAL(triggered()),SLOT(handle_actionActionFindItem_triggered()));
    ACTION_MANAGER->registerAction(MENU_EDIT_FIND,d->actionActionFindItem,context);
}

void Qtilities::CoreGui::ObserverWidget::refreshActions() {
    if (!d->initialized) {
        d->action_provider->disableAllActions();
        return;
    }

    // Ok, first we set only actions specified by the observer's action hints to be visible
    if (activeHints()->actionHints() & ObserverHints::ActionRemoveItem)
        d->actionActionRemoveItem->setVisible(true);
    else
        d->actionActionRemoveItem->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionRemoveAll)
        d->actionActionRemoveAll->setVisible(true);
    else
        d->actionActionRemoveAll->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionDeleteItem)
        d->actionActionDeleteItem->setVisible(true);
    else
        d->actionActionDeleteItem->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionDeleteAll)
        d->actionActionDeleteAll->setVisible(true);
    else
        d->actionActionDeleteAll->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionPushDown)
        d->actionActionPushDown->setVisible(true);
    else
        d->actionActionPushDown->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionPushDownNew)
        d->actionActionPushDownNew->setVisible(true);
    else
        d->actionActionPushDownNew->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionPushUp)
        d->actionActionPushUp->setVisible(true);
    else
        d->actionActionPushUp->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionPushUpNew)
        d->actionActionPushUpNew->setVisible(true);
    else
        d->actionActionPushUpNew->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionNewItem)
        d->actionActionNewItem->setVisible(true);
    else
        d->actionActionNewItem->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionRefreshView)
        d->actionActionRefreshView->setVisible(true);
    else
        d->actionActionRefreshView->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionSwitchView)
        d->actionActionSwitchView->setVisible(true);
    else
        d->actionActionSwitchView->setVisible(false);

    if (activeHints()->actionHints() & ObserverHints::ActionFindItem)
        d->actionActionFindItem->setVisible(true);
    else
        d->actionActionFindItem->setVisible(false);

    // Remove & Delete All Actions
    if (d_observer) {
        if (d_observer->subjectCount() > 0) {
            d->actionActionRemoveAll->setEnabled(true);
            d->actionActionDeleteAll->setEnabled(true);
        } else {
            d->actionActionRemoveAll->setEnabled(false);
            d->actionActionDeleteAll->setEnabled(false);
        }
    }

    // New Item
    d->actionActionNewItem->setEnabled(true);

    // Navigating Up/Down Actions
    if (d->display_mode == TableView) {
        if (d->navigation_stack.count() == 0) {
            d->actionActionPushUp->setEnabled(false);
            d->actionActionPushUpNew->setEnabled(false);
        } else {
            d->actionActionPushUp->setEnabled(true);
            d->actionActionPushUpNew->setEnabled(true);
        }
        d->actionCollapseAll->setEnabled(false);
        d->actionExpandAll->setEnabled(false);
        d->actionCollapseAll->setVisible(false);
        d->actionExpandAll->setVisible(false);
    } else {
        d->actionCollapseAll->setVisible(true);
        d->actionExpandAll->setVisible(true);
        d->actionCollapseAll->setEnabled(true);
        d->actionExpandAll->setEnabled(true);
        d->actionActionPushDown->setEnabled(false);
        d->actionActionPushUp->setEnabled(false);
        d->actionActionPushDownNew->setEnabled(false);
        d->actionActionPushUpNew->setEnabled(false);
        d->actionActionPushDown->setVisible(false);
        d->actionActionPushUp->setVisible(false);
        d->actionActionPushDownNew->setVisible(false);
        d->actionActionPushUpNew->setVisible(false);
    }

    // Remove & Delete Items + Navigating Down Actions
    if (selectedObjects().count() == 0) {
        d->actionActionDeleteItem->setEnabled(false);
        d->actionActionRemoveItem->setEnabled(false);
        if (d->display_mode == TableView) {
            d->actionActionPushDown->setEnabled(false);
            d->actionActionPushDownNew->setEnabled(false);
        }
    } else {
        if (d->display_mode == TableView) {
            d->actionActionDeleteItem->setEnabled(true);
            d->actionActionRemoveItem->setEnabled(true);
            d->actionActionPushDown->setEnabled(false);
            d->actionActionPushDownNew->setEnabled(false);

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
                        d->actionActionPushDown->setEnabled(true);
                        d->actionActionPushDownNew->setEnabled(true);
                    }
                }
            }
        } else if (d->display_mode == TreeView) {
            if (selectedObjects().count() == 1) {
                Observer* selected = qobject_cast<Observer*> (selectedObjects().front());
                if (selected == d->top_level_observer) {
                    d->actionActionDeleteItem->setEnabled(false);
                    d->actionActionRemoveItem->setEnabled(false);
                } else {
                    d->actionActionDeleteItem->setEnabled(true);
                    d->actionActionRemoveItem->setEnabled(true);
                }
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::setTreeSelectionParent(Observer* observer) {
    // This function will only be entered in TreeView mode.
    // It is a slot connected to the selection parent changed signal in the tree model.
    if (!observer)
        observer = d->top_level_observer;

    setObserverContext(observer);
    initialize(true);

    // We need to look at the current selection parent if in tree mode, otherwise in table mode we use d_observer:
    if (d->update_selection_activity && observer) {
        // Check if the observer has a FollowSelection activity policy
        if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
            // Check if the observer has a activity filter, which it should have with this hint
            ActivityPolicyFilter* filter = 0;
            for (int i = 0; i < observer->subjectFilters().count(); i++) {
                filter = qobject_cast<ActivityPolicyFilter*> (observer->subjectFilters().at(i));
                if (filter) {
                    filter->setActiveSubjects(d->current_selection);
                }
            }
        }
    }

    d->tree_name_column_delegate->setObserverContext(observer);
    refreshPropertyBrowser();
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionRemoveItem_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        for (int i = 0; i < d->current_selection.count(); i++)
            d_observer->detachSubject(d->current_selection.at(i));
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (d->current_selection.count() != 1)
            return;
        
        // Make sure the selected object is not the top level observer (might happen in a tree view)
        Observer* observer = qobject_cast<Observer*> (d->current_selection.front());
        if (observer == d->top_level_observer)
            return;

        d_observer->detachSubject(d->current_selection.front());
        handle_actionExpandAll_triggered();
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionRemoveAll_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        d_observer->detachAll();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (selectedIndexes().count() != 1)
            return;

        // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
        bool use_parent_context = true;
        Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
        if (obs) {
            if (obs->displayHints()) {
                if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                    obs->deleteAll();
                    handle_actionExpandAll_triggered();
                    use_parent_context = false;
                }
            }
        }

        if (use_parent_context) {
            // We must check if there is an selection parent, else use d_observer
            if (d->tree_model->selectionParent()) {
                d->tree_model->selectionParent()->detachAll();
                handle_actionExpandAll_triggered();
            } else {
                d_observer->detachAll();
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionDeleteItem_triggered() {
    if (!d->initialized)
        return;

    int selected_count = d->current_selection.count();

    for (int i = 0; i < selected_count; i++) {
        if (d->current_selection.at(i)) {
            // Make sure the selected object is not the top level observer (might happen in a tree view)
            Observer* observer = qobject_cast<Observer*> (d->current_selection.at(0));
            if (observer != d->top_level_observer)
                delete d->current_selection.at(0);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionDeleteAll_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        d_observer->deleteAll();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (selectedIndexes().count() != 1)
            return;

        // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
        bool use_parent_context = true;
        Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
        if (obs) {
            if (obs->displayHints()) {
                if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
                    obs->deleteAll();
                    handle_actionExpandAll_triggered();
                    use_parent_context = false;
                }
            }
        }

        if (use_parent_context) {
            // We must check if there is an selection parent, else use d_observer
            if (d->tree_model->selectionParent()) {
                d->tree_model->selectionParent()->deleteAll();
                handle_actionExpandAll_triggered();
            } else {
                d_observer->deleteAll();
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionNewItem_triggered() {
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

void Qtilities::CoreGui::ObserverWidget::handle_actionActionRefreshView_triggered() {
    if (!d->initialized || !d_observer)
        return;

    if (d->display_mode == TableView && d->table_model) {
        d_observer->refreshViewsLayout();
        resizeTableViewRows();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        d_observer->refreshViewsLayout();
        handle_actionExpandAll_triggered();
    }

    setWindowTitle(QString("%1").arg(d_observer->objectName()));

    if (d->navigation_bar)
        d->navigation_bar->refreshHierarchy();
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionPushUp_triggered() {
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

void Qtilities::CoreGui::ObserverWidget::handle_actionActionPushUpNew_triggered() {
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
        new_child_widget->toggleGrid(d->table_view->showGrid());
        new_child_widget->toggleAlternatingRowColors(d->table_view->alternatingRowColors());
    } else if (d->display_mode == TreeView && d->tree_view) {
        new_child_widget->toggleAlternatingRowColors(d->tree_view->alternatingRowColors());
    }
    new_child_widget->setSizePolicy(sizePolicy());
    new_child_widget->show();

    newObserverWidgetCreated(new_child_widget);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionPushDown_triggered() {
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
            msgBox.setText("The context you are trying to access is locked.");
            msgBox.exec();
            return;
        }

        // First disconnet all current observer connections
        d_observer->disconnect(this);
        d_observer->disconnect(d->navigation_bar);

        // Set up widget to use new observer
        d->navigation_stack.push(d_observer->observerID());
        connect(d_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication,QList<QObject*>)),SLOT(contextDetachHandler(Observer::SubjectChangeIndication,QList<QObject*>)));
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

void Qtilities::CoreGui::ObserverWidget::handle_actionActionPushDownNew_triggered() {
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
            msgBox.setText("The context you are trying to access is locked.");
            msgBox.exec();
            return;
        }

        d->navigation_stack.push(d_observer->observerID());
        new_child_widget = new ObserverWidget(d->display_mode);
        new_child_widget->setNavigationStack(d->navigation_stack);
        new_child_widget->toggleGrid(d->table_view->showGrid());
        new_child_widget->toggleAlternatingRowColors(d->table_view->alternatingRowColors());

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
    new_child_widget->setSizePolicy(sizePolicy());
    new_child_widget->show();
    newObserverWidgetCreated(new_child_widget);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionActionSwitchView_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView) {
        d->display_mode = TreeView;
        setObserverContext(d->top_level_observer);
        initialize();
        handle_actionExpandAll_triggered();
    } else if (d->display_mode == TreeView && d->tree_model) {
        if (selectedIndexes().count() > 0) {
            d->navigation_stack = d->tree_model->getParentHierarchy(selectedIndexes().front());

            bool pop_back = true;
            Observer* obs = qobject_cast<Observer*> (d->tree_model->getObject(selectedIndexes().front()));
            if (obs) {
                if (obs->displayHints()) {
                    // Respect ObserverSelectionContext hint by first checking if the selection is an observer if needed
                    if (obs->displayHints()->observerSelectionContextHint() & ObserverHints::SelectionUseSelectedContext) {
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
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionCollapseAll_triggered() {
    if (!d->initialized)
        return;

    if (d->tree_view && d->display_mode == TreeView) {
        d->tree_view->expandToDepth(0);
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionExpandAll_triggered() {
    if (!d->initialized)
        return;

    if (d->tree_view && d->display_mode == TreeView) {
        d->tree_view->expandAll();
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionCopy_triggered() {
    ObserverMimeData *mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID());
    QApplication::clipboard()->setMimeData(mimeData);
    CLIPBOARD_MANAGER->setClipboardOrigin(IClipboard::CopyAction);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionCut_triggered() {
    ObserverMimeData *mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID());
    QApplication::clipboard()->setMimeData(mimeData);
    CLIPBOARD_MANAGER->setClipboardOrigin(IClipboard::CutAction);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionPaste_triggered() {
    if (activeHints()->actionHints() & ObserverHints::ActionPasteItem){
        // Check if the subjects being dropped are of the same type as the destination observer.
        // If this is not the case, we do not allow the drop.
        const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (QApplication::clipboard()->mimeData());
        if (observer_mime_data) {
            if (d_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data)) == Observer::Allowed) {
                // Now check the proposed action of the event.
                if (CLIPBOARD_MANAGER->clipboardOrigin() == IClipboard::CutAction) {
                    OBJECT_MANAGER->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),d_observer->observerID());
                    CLIPBOARD_MANAGER->acceptMimeData();
                } else if (CLIPBOARD_MANAGER->clipboardOrigin() == IClipboard::CopyAction) {
                    // Attempt to copy the objects
                    // For now we discard objects that cause problems during attachment and detachment
                    for (int i = 0; i < observer_mime_data->subjectList().count(); i++) {
                        // Attach to destination
                        d_observer->attachSubject(observer_mime_data->subjectList().at(i));
                    }
                    CLIPBOARD_MANAGER->acceptMimeData();
                }
            } else {
                QMessageBox msgBox;
                msgBox.setText("Paste Operation Failed.");
                msgBox.setInformativeText(QString(tr("The paste operation could not be completed. The destination observer could not accept all the objects in your selection.\n\nDo you want to keep the data in the clipboard for later usage?")));
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
            msgBox.setText("Paste Operation Failed.");
            msgBox.setInformativeText("The paste operation could not be completed. Unsupported data type for this context.\n\nDo you want to keep the data in the clipboard for later usage?");
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
        msgBox.setText("Paste Operation Failed.");
        msgBox.setInformativeText("The destination context cannot accept paste operations.\n\nDo you want to keep the data in the clipboard for later usage?");
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

void Qtilities::CoreGui::ObserverWidget::handle_actionActionFindItem_triggered() {
    if (!d->initialized)
        return;

    if (!d->searchBoxWidget) {
        SearchBoxWidget::SearchOptions search_options = 0;
        search_options |= SearchBoxWidget::CaseSensitive;
        search_options |= SearchBoxWidget::RegEx;
        SearchBoxWidget::ButtonFlags button_flags = 0;
        button_flags |= SearchBoxWidget::HideButton;
        d->searchBoxWidget = new SearchBoxWidget(search_options,SearchBoxWidget::SearchOnly,button_flags);
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

    if (d->update_selection_activity)
        CONTEXT_MANAGER->broadcastState();

    // Update the global object list
    if (!d->property_browser_widget) {
        updateGlobalActiveSubjects();
    }

    // If selected objects > 0 and the observer context supports copy & cut, we enable copy/cut
    if (d->current_selection.count() > 0) {
        if (activeHints()->actionHints() & ObserverHints::ActionCopyItem) {
            // ---------------------------
            // Copy
            // ---------------------------
            // The copy action is not a multi context action. It's enabled/disabled depending on the state of the application
            // clipboard. Thus we just connect to the paste action here and check the observer flag in the paste slot.
            Command* command = ACTION_MANAGER->command(MENU_EDIT_COPY);
            if (command) {
                if (command->action()) {
                    command->action()->setEnabled(true);
                    connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionCopy_triggered()));
                }
            }
        }

        if (activeHints()->actionHints() & ObserverHints::ActionCutItem) {
            // ---------------------------
            // Cut
            // ---------------------------
            // The cut action is not a multi context action. It's enabled/disabled depending on the state of the application
            // clipboard. Thus we just connect to the paste action here and check the observer flag in the paste slot.
            Command* command = ACTION_MANAGER->command(MENU_EDIT_CUT);
            if (command) {
                if (command->action()) {
                    command->action()->setEnabled(true);
                    connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionCut_triggered()));
                }
            }
        }
    } else {
        disconnectClipboard();
    }

    // Only refresh the property browser in table view mode here. In tree view mode it is refreshed in
    // the setSelectionParent slot.
    Observer* selection_parent = 0;
    if (d->display_mode == TableView) {
        refreshPropertyBrowser();
        selection_parent = d_observer;

        if (d->activity_filter && d->update_selection_activity) {
            // Check if the observer has a FollowSelection activity policy
            if (activeHints()->activityControlHint() == ObserverHints::FollowSelection) {
                d->activity_filter->setActiveSubjects(object_list);
            }
        }

    } else if (d->display_mode == TreeView) {
        selection_parent = d->tree_model->calculateSelectionParent(selectedIndexes());
        // In this case the FollowSelection hint check will happen in the setTreeSelection slot.
    }

    // Emit signals
    emit selectedObjectsChanged(object_list, selection_parent);
}

void Qtilities::CoreGui::ObserverWidget::disconnectClipboard() {
    Command* command = ACTION_MANAGER->command(MENU_EDIT_COPY);
    if (command) {
        if (command->action()) {
            command->action()->setEnabled(false);
            command->action()->disconnect(this);
        }
    }

    command = ACTION_MANAGER->command(MENU_EDIT_CUT);
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
        if (d->current_selection.count() == 0) {
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

void Qtilities::CoreGui::ObserverWidget::contextDeleted() {
    if (d->display_mode == TreeView){
        if (sender() == d->top_level_observer) {
            d->initialized = false;
            refreshActions();
        } else {
            setObserverContext(d->top_level_observer);
            initialize();
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
    }
}

void Qtilities::CoreGui::ObserverWidget::contextDetachHandler(Observer::SubjectChangeIndication indication, QList<QObject*> objects) {
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

void Qtilities::CoreGui::ObserverWidget::selectSubjectsInTable(QList<QObject*> objects) {
    if (objects.count() == 0)
        return;

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
    d->proxy_model->setFilterRegExp(regExp);

    if (d->table_view && d->display_mode == TableView)
        d->table_view->resizeRowsToContents();
    else if (d->tree_view && d->display_mode == TreeView)
        handle_actionExpandAll_triggered();
}

void Qtilities::CoreGui::ObserverWidget::resetProxyModel() {
    handleSearchStringChanged("");
}

void Qtilities::CoreGui::ObserverWidget::refreshPropertyBrowser() {
    // Update the property editor visibility and object
    if (activeHints()->displayFlagsHint() & ObserverHints::PropertyBrowser) {
        constructPropertyBrowser();
        if (selectedObjects().count() == 1)
            d->property_browser_widget->setObject(d->current_selection.front());
        else
            d->property_browser_widget->setObject(d_observer);
        d->property_browser_dock->show();
        addDockWidget(d->property_editor_dock_area, d->property_browser_dock);
    } else {
        removeDockWidget(d->property_browser_dock);
    }
}

void Qtilities::CoreGui::ObserverWidget::constructPropertyBrowser() {
    if (!d->property_browser_dock) {
        d->property_browser_dock = new QDockWidget("Property Browser",0);
        d->property_browser_widget = new ObjectPropertyBrowser(d->property_editor_type);
        d->property_browser_widget->setObject(d_observer);
        d->property_browser_dock->setWidget(d->property_browser_widget);
        connect(d->property_browser_dock,SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),SLOT(setPreferredPropertyEditorDockArea(Qt::DockWidgetArea)));
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
            if (d->current_selection.count() == 1) {
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
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionPaste_triggered()));
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
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    command->action()->disconnect(this);
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
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionPaste_triggered()));
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
            // Disconnect the paste action from the this widget.
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    command->action()->disconnect(this);
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
        if (object == d->table_view && event->type() == QEvent::DragMove) {
            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent *>(event);
            dragMoveEvent->accept();
            return false;
        } else if (object == this && event->type() == QEvent::Drop) {
            QDropEvent *dropEvent = static_cast<QDropEvent *>(event);

            if (!d->initialized)
                return false;

            if (d->display_mode == TableView) {
                if (dropEvent->proposedAction() == Qt::MoveAction || dropEvent->proposedAction() == Qt::CopyAction) {
                    const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (dropEvent->mimeData());
                    if (observer_mime_data) {
                        if (observer_mime_data->sourceID() == d_observer->observerID()) {
                            QMessageBox msgBox;
                            msgBox.setWindowTitle(tr("Drop Operation Failed"));
                            msgBox.setText(QString(tr("The drop operation could not be completed. The destination and source is the same.")));
                            msgBox.exec();
                            return false;
                        }

                        if (d_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data)) == Observer::Allowed) {
                            // Now check the proposed action of the event.
                            if (dropEvent->proposedAction() == Qt::MoveAction) {
                                dropEvent->accept();
                                OBJECT_MANAGER->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),d_observer->observerID());
                            } else if (dropEvent->proposedAction() == Qt::CopyAction) {
                                dropEvent->accept();

                                // Attempt to copy the dragged objects
                                // For now we discard objects that cause problems during attachment and detachment
                                for (int i = 0; i < observer_mime_data->subjectList().count(); i++) {
                                    // Attach to destination
                                    if (!d_observer->attachSubject(observer_mime_data->subjectList().at(i))) {
                                        QMessageBox msgBox;
                                        msgBox.setWindowTitle(tr("Drop Operation Failed"));
                                        msgBox.setText(QString(tr("Attachment of your object(s) failed in the destination context.")));
                                        msgBox.exec();
                                    }
                                }
                            }
                        } else {
                            QMessageBox msgBox;
                            msgBox.setWindowTitle(tr("Drop Operation Failed"));
                            msgBox.setText(QString(tr("The drop operation could not be completed. The destination observer cannot accept all the objects in your selection.")));
                            msgBox.exec();
                        }
                    }
                }
            }
            return false;
        } else if (object == d->table_view && event->type() == QEvent::DragEnter) {
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
                    if (d->current_selection.size() > 0 && d->initialized) {
                        // Check to see if the selection contains any observers. If so, we don't allow the drag to start.
                        bool do_drag = true;
                        /*for (int i = 0; i < d->current_selection.size(); i++) {
                            Observer* observer = qobject_cast<Observer*> (d->current_selection.at(i));
                            if (observer) {
                                do_drag = false;
                                break;
                            }
                        }*/

                        if (do_drag && d->table_model) {
                            ObserverMimeData *mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID());

                            QDrag *drag = new QDrag(this);
                            drag->setMimeData(mimeData);
                            //drag->setPixmap(QPixmap(ICON_OBSERVER));

                            if (mouseEvent->buttons() == Qt::LeftButton)
                                drag->exec(Qt::CopyAction);
                            else if (mouseEvent->buttons() == Qt::RightButton)
                                drag->exec(Qt::MoveAction);
                        }
                    }
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
            if (!strcmp(qtilities_event->propertyName().data(),OBSERVER_SUBJECT_IDS)) {
                // This implementation can be improved in the future. For now we don't care if the change affects this view.
                // We just close this window.
                contextDeleted();
            }
        }
        return false;
     }
     return false;
}
