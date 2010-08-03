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
#include "QtilitiesCoreGui.h"
#include "ObjectScopeWidget.h"
#include "ObjectHierarchyNavigator.h"
#include "ObjectPropertyBrowser.h"
#include "QtilitiesPropertyChangeEvent.h"
#include "SearchBoxWidget.h"
#include "ObserverTableModelCategoryFilter.h"
#include "ActionProvider.h"

#include <ActivityPolicyFilter.h>
#include <Observer.h>
#include <ObserverMimeData.h>
#include <QtilitiesCore.h>
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
    actionMergeItems(0),
    actionSplitItems(0),
    actionFindItem(0),
    proxy_model(0),
    activity_filter(0),
    searchBoxWidget(0),
    action_provider(0) { }

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
    QAction* actionMergeItems;
    QAction* actionSplitItems;
    QAction* actionFindItem;

    QStack<int> navigation_stack;
    ObjectHierarchyNavigator* navigation_bar;
    QPoint startPos;

    Qtilities::CoreGui::ObserverWidget::DisplayMode display_mode;
    QPointer<QTableView> table_view;
    QPointer<ObserverTableModel> table_model;
    QPointer<QTreeView> tree_view;
    QPointer<ObserverTreeModel> tree_model;
    QSortFilterProxyModel *proxy_model;
    bool model_was_reset;
    NamingPolicyDelegate* table_name_column_delegate;
    NamingPolicyDelegate* tree_name_column_delegate;
    ActivityPolicyFilter* activity_filter;

    //! Used to identify the top level observer. d_observer is current selection parent observer.
    QPointer<Observer> top_level_observer;

    QDockWidget* property_browser_dock;
    ObjectPropertyBrowser* property_browser_widget;

    bool initialized;
    bool use_observer_hints;
    bool update_selection_activity;

    Qtilities::Core::Observer::DisplayFlags display_flags;
    Qtilities::Core::Observer::ActionHints action_hints;
    Qtilities::Core::Observer::DisplayFlags default_display_flags;
    Qtilities::Core::Observer::ActionHints default_action_hints;
    Qtilities::Core::Observer::ItemViewColumnFlags item_view_column_hints;
    Qtilities::Core::Observer::ItemViewColumnFlags default_item_view_column_flags;

    Qt::DockWidgetArea property_editor_dock_area;
    ObjectPropertyBrowser::BrowserType property_editor_type;
    QStringList appended_contexts;
    QList<QObject*> current_selection;

    SearchBoxWidget* searchBoxWidget;
    QString settings_string;
    QString global_meta_type;

    ActionProvider* action_provider;
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
    d->display_mode = display_mode;

    d->tree_view = 0;
    d->display_flags = 0;
    d->table_model = 0;
    d->table_view = 0;
    d->tree_model = 0;
    d->table_name_column_delegate = 0;
    d->tree_name_column_delegate = 0;
    d->top_level_observer = 0;

    // Init rest of parameters
    d->action_hints = Qtilities::Core::Observer::None;
    d->display_flags = Qtilities::Core::Observer::NoDisplayFlagsHint;
    d->item_view_column_hints = Qtilities::Core::Observer::NoItemViewColumnHint;
    d->default_action_hints = Qtilities::Core::Observer::None;
    d->default_display_flags = Qtilities::Core::Observer::NoDisplayFlagsHint;
    d->default_display_flags |= Qtilities::Core::Observer::ItemView;
    d->default_display_flags |= Qtilities::Core::Observer::NavigationBar;
    d->default_item_view_column_flags = Qtilities::Core::Observer::NoItemViewColumnHint;

    d->initialized = false;
    d->use_observer_hints = true;
    d->update_selection_activity = true;
    d->property_editor_dock_area = Qt::RightDockWidgetArea;
    d->property_editor_type = ObjectPropertyBrowser::TreeBrowser;
    ui->setupUi(this);
    d->global_meta_type = QString();

    setWindowIcon(QIcon(ICON_QTILITIES_SYMBOL_WHITE_16x16));
    ui->widgetSearchBox->hide();
}

Qtilities::CoreGui::ObserverWidget::~ObserverWidget()
{   
    delete ui;
    delete d;           
}

bool Qtilities::CoreGui::ObserverWidget::eventFilter(QObject *object, QEvent *event) {
    if (!d->initialized)
        return false;

    if (d->table_view && d->display_mode == TableView) {
        if (object == d->table_view->viewport() && event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->buttons() == Qt::LeftButton)
               d->startPos = mouseEvent->pos();
            return false;
        } else if (object == d->table_view->viewport() && event->type() == QEvent::MouseButtonDblClick) {
            if (d->current_selection.count() == 1)
                emit doubleClickRequest(d->current_selection.front(), d_observer);
            else
                emit doubleClickRequest(0, d_observer);

            return false;
        } else if (object == d->table_view->viewport() && event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

            int distance = (mouseEvent->pos() - d->startPos).manhattanLength();
            if (distance >= QApplication::startDragDistance()) {
                if (mouseEvent->buttons() == Qt::LeftButton || mouseEvent->buttons() == Qt::RightButton) {
                    if (selectedObjects().size() > 0 && d->initialized) {
                        // Check to see if the selection contains any observers. If so, we don't allow the drag to start.
                        bool do_drag = true;
                        /*for (int i = 0; i < selectedObjects().size(); i++) {
                            Observer* observer = qobject_cast<Observer*> (selectedObjects().at(i));
                            if (observer) {
                                do_drag = false;
                                break;
                            }
                        }*/

                        if (do_drag && d->table_model) {
                            ObserverMimeData *mimeData = new ObserverMimeData(selectedObjects(),d_observer->observerID());

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
    } else if (d->tree_view && d->tree_model && d->display_mode == TreeView) {
        if (object == d->tree_view->viewport() && event->type() == QEvent::MouseButtonDblClick) {
            if (d->current_selection.count() == 1)
                emit doubleClickRequest(d->current_selection.front(), d->tree_model->selectionParent());
            else
                emit doubleClickRequest(0, d->tree_model->selectionParent());
            return false;
        }
    }

    if (object == d->table_view && event->type() == QEvent::DragEnter) {
        QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent *>(event);
        dragEnterEvent->accept();
        return false;
    } else if (object == d->table_view && event->type() == QEvent::FocusIn) {
        // Connect to the paste action
        Command* command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_PASTE);
        if (command) {
            if (command->action())
                command->action()->disconnect(this);
        }

        if (d_observer->subjectCount() == 0) {
            QtilitiesCore::instance()->contextManager()->setNewContext(contextString(),true);
            setGlobalObjectSubjectType();
            refreshActions();
        } else {
            QtilitiesCore::instance()->contextManager()->setNewContext(contextString(),true);
        }

        return false;
    } else if (object == d->table_view && event->type() == QEvent::FocusOut) {
        // Disconnect the paste action from the this widget.
        Command* command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_PASTE);
        if (command) {
            if (command->action())
                command->action()->disconnect(this);
        }

        return false;
    } else if (object == d->tree_view && event->type() == QEvent::FocusIn) {
        if (!d_observer)
            return false;

        // Connect to the paste action
        Command* command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_PASTE);
        if (command) {
            if (command->action())
                connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionPaste_triggered()));
        }

        if (d_observer->subjectCount() == 0) {
            QtilitiesCore::instance()->contextManager()->setNewContext(contextString(),true);
            setGlobalObjectSubjectType();
            refreshActions();
        } else {
            QtilitiesCore::instance()->contextManager()->setNewContext(contextString(),true);
        }

        return false;
    } else if (object == d->tree_view && event->type() == QEvent::FocusOut) {
        // Disconnect the paste action from the this widget.
        Command* command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_PASTE);
        if (command) {
            if (command->action())
                command->action()->disconnect(this);
        }

        return false;
    } else if (object == d->table_view && event->type() == QEvent::DragMove) {
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
                    if (observer_mime_data->sourceID() == d_observer->observerID())
                        return false;

                    if (d_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data)) == Qtilities::Core::Observer::Allowed) {
                        // Now check the proposed action of the event.
                        if (dropEvent->proposedAction() == Qt::MoveAction) {
                            dropEvent->accept();
                            QtilitiesCore::instance()->objectManager()->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),d_observer->observerID());
                        } else if (dropEvent->proposedAction() == Qt::CopyAction) {
                            dropEvent->accept();

                            // Attempt to copy the dragged objects
                            // For now we discard objects that cause problems during attachment and detachment
                            for (int i = 0; i < observer_mime_data->subjectList().count(); i++) {
                                // Attach to destination
                                d_observer->attachSubject(observer_mime_data->subjectList().at(i));
                            }
                        }
                    } else {
                        QMessageBox msgBox;
                        msgBox.setWindowTitle(tr("Drop Operation Failed"));
                        msgBox.setWindowIcon(QIcon(ICON_OBSERVER));
                        msgBox.setText(QString(tr("The drop operation could not be completed. The destination observer manages cannot accept all the objects in your selection.")));
                        msgBox.exec();
                    }
                }
            }
        }
        return false;
    } else if (object == this && event->type() == QEvent::User) {
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

void Qtilities::CoreGui::ObserverWidget::setObserverContext(Observer* observer) {
    if (!observer)
        return;

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

    ObserverAwareBase::setObserverContext(observer);

    // Check if the observer has a FollowSelection actity policy
    if (d_observer->activityControlHint() == Qtilities::Core::Observer::FollowSelection) {
        // Check if the observer has a activity filter, which it should have with this hint
        ActivityPolicyFilter* filter = 0;
        for (int i = 0; i < d_observer->subjectFilters().count(); i++) {
            filter = qobject_cast<ActivityPolicyFilter*> (d_observer->subjectFilters().at(i));
            if (filter) {
                d->activity_filter = filter;

                // Connect to the activity change signal (to update activity on observer widget side)
                connect(d->activity_filter,SIGNAL(activeSubjectsChanged(QList<QObject*>,QList<QObject*>)),SLOT(selectSubjectsByRef(QList<QObject*>)));
                selectSubjectsByRef(d->activity_filter->activeSubjects());
            }
        }
    } else {
        if (d->activity_filter)
            d->activity_filter->disconnect(this);
        d->activity_filter = 0;
    }

    // Update the observer context of the delegates
    if (d->display_mode == TableView && d->table_name_column_delegate)
        d->table_name_column_delegate->setObserverContext(observer);
    else if (d->display_mode == TreeView && d->tree_name_column_delegate)
        d->tree_name_column_delegate->setObserverContext(observer); 

    emit observerContextChanged(d_observer);
}

int Qtilities::CoreGui::ObserverWidget::topLevelObserverID() {
    if (d->top_level_observer)
        return d->top_level_observer->observerID();
    else
        return -1;
}

void Qtilities::CoreGui::ObserverWidget::initialize(bool hints_only) {
    // Check it this widget was initialized previously
    if (!d->initialized) {
        // Setup some flags and attributes for this widget the first time it is constructed.
        setAttribute(Qt::WA_DeleteOnClose, true);
        // Register contextString in the context manager.
        QtilitiesCore::instance()->objectManager()->registerObject(this);
        constructActions();
    }

    d->initialized = false;

    if (!d_observer) {
        LOG_DEBUG("You are attempting to initialize an ObserverWidget without an observer context.");
        d->action_provider->disableAllActions();
        d->initialized = false;
        return;
    }

    // Set the title and name of the observer widget.
    // Here we need to check if we must use d_observer inside a specific context
    if (d->navigation_stack.count() > 0) {
        setWindowTitle(d_observer->observerName(d->navigation_stack.last()));
        setObjectName(d_observer->observerName(d->navigation_stack.last()));
    } else {
        setWindowTitle(d_observer->objectName());
        setObjectName(d_observer->objectName());
    }

    // Construct navigation toolbar and set up hints
    if (d->use_observer_hints) {
        // Check if this observer provides hints for this model
        d->display_flags = d_observer->displayFlagsHint();
        d->action_hints = d_observer->actionHints();
        d->item_view_column_hints = d_observer->itemViewColumnFlags();
    } else {
        d->action_hints = d->default_action_hints;
        d->item_view_column_hints = d->default_item_view_column_flags;
    }

    // Check if the observer does not specify flags, if so we use the defaults
    if (d->display_flags == Qtilities::Core::Observer::NoDisplayFlagsHint)
        d->display_flags = d->default_display_flags;

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
                d->tree_model = new ObserverTreeModel();
                d->tree_view->setSortingEnabled(true);
                d->tree_view->sortByColumn(ObserverTreeModel::NameColumn,Qt::AscendingOrder);
                d->model_was_reset = false;
                connect(d->tree_model,SIGNAL(selectionParentChanged(Observer*)),SLOT(setSelectionParent(Observer*)));
                connect(d->tree_model,SIGNAL(layoutChanged()),d->tree_view,SLOT(expandAll()));

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
            d->proxy_model->setFilterKeyColumn(ObserverTreeModel::NameColumn);
            d->tree_view->setModel(d->proxy_model);

            //d->tree_view->header()->hide();
            if (d->tree_view->selectionModel())
                connect(d->tree_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()));

            // Check if the item view should be vissible
            if (d->display_flags & Qtilities::Core::Observer::ItemView)
                d->tree_view->setVisible(true);
            else
                d->tree_view->setVisible(false);

            d->tree_view->expandAll();
        } else if (d->display_mode == TableView) {
            // Connect to the current parent observer, in the tree view the model will monitor this for you.
            connect(d_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication)),SLOT(handleSubjectCountChanged()));
            connect(d_observer,SIGNAL(modificationStateChanged(bool)),SLOT(handleSubjectCountChanged()));
            connect(d_observer,SIGNAL(destroyed()),SLOT(contextDeleted()));
            connect(d_observer,SIGNAL(nameChanged(QString)),SLOT(setWindowTitle(QString)));
            d->proxy_model = new ObserverTableModelCategoryFilter(this);
            d->proxy_model->setDynamicSortFilter(true);

            if (d->tree_view)
                d->tree_view->hide();

            // Check if there is already a model.
            if (!d->table_view) {
                d->table_view = new QTableView();
                d->table_view->setFocusPolicy(Qt::StrongFocus);
                d->table_model = new ObserverTableModel();
                d->table_view->setSortingEnabled(true);
                //connect(d->proxy_model,SIGNAL(layoutChanged()),SLOT(handleLayoutChange()));

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
            d->table_view->resizeRowsToContents();
            d->table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
            d->table_view->verticalHeader()->setVisible(false);

            // Setup proxy model
            d->proxy_model->setSourceModel(d->table_model);
            d->proxy_model->setFilterKeyColumn(ObserverTableModel::NameColumn);
            d->table_view->setModel(d->proxy_model);

            if (d->table_view->selectionModel()) {
                d->table_view->selectionModel()->clear();
                connect(d->table_view->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(handleSelectionModelChange()));
            }

            // Check if the item view should be vissible
            if (d->display_flags & Qtilities::Core::Observer::ItemView)
                d->table_view->setVisible(true);
            else
                d->table_view->setVisible(false);
        }
    }

    // Check if the hierarhcy navigation bar should be visible
    if (d->display_flags & Qtilities::Core::Observer::NavigationBar) {
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
    if (d->display_mode == TableView && d->table_view) {
        // Show only the needed columns for the current observer.
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::IdColumn))
            d->table_view->hideColumn(ObserverTableModel::IdColumn);
        else {
            d->table_view->showColumn(ObserverTableModel::IdColumn);
            d->table_view->setColumnWidth(ObserverTableModel::IdColumn,50);
        }
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::CategoryColumn))
            d->table_view->hideColumn(ObserverTableModel::CategoryColumn);
        else {
            if (d_observer->hierarchicalDisplayHint() & Observer::CategorizedHierarchy) {
                d->table_view->showColumn(ObserverTableModel::CategoryColumn);
                d->table_view->setColumnWidth(ObserverTableModel::CategoryColumn,50);
            } else {
                d->table_view->hideColumn(ObserverTableModel::CategoryColumn);
            }
        }
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::ChildCountColumn))
            d->table_view->hideColumn(ObserverTableModel::ChildCountColumn);
        else {
            d->table_view->showColumn(ObserverTableModel::ChildCountColumn);
            d->table_view->setColumnWidth(ObserverTableModel::ChildCountColumn,50);
        }
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::TypeInfoColumn))
            d->table_view->hideColumn(ObserverTableModel::TypeInfoColumn);
        else {
            d->table_view->showColumn(ObserverTableModel::TypeInfoColumn);
            d->table_view->setColumnWidth(ObserverTableModel::TypeInfoColumn,50);
        }
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::AccessColumn))
            d->table_view->hideColumn(ObserverTableModel::AccessColumn);
        else {
            d->table_view->showColumn(ObserverTableModel::AccessColumn);
            d->table_view->setColumnWidth(ObserverTableModel::AccessColumn,50);
        }

        // Resize columns
        QHeaderView* table_header = d->table_view->horizontalHeader();
        d->table_view->resizeColumnsToContents();
        table_header->setResizeMode(ObserverTableModel::NameColumn,QHeaderView::Stretch);
    } else if (d->display_mode == TreeView && d->tree_view) {
        // Show only the needed columns for the current observer
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::ChildCountColumn))
            d->tree_view->hideColumn(ObserverTreeModel::ChildCountColumn);
        else {
            d->tree_view->showColumn(ObserverTreeModel::ChildCountColumn);
            d->tree_view->setColumnWidth(ObserverTreeModel::ChildCountColumn,50);
        }
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::TypeInfoColumn))
            d->tree_view->hideColumn(ObserverTreeModel::TypeInfoColumn);
        else {
            d->tree_view->showColumn(ObserverTreeModel::TypeInfoColumn);
            d->tree_view->setColumnWidth(ObserverTreeModel::TypeInfoColumn,50);
        }
        if (!(d->item_view_column_hints & Qtilities::Core::Observer::AccessColumn))
            d->tree_view->hideColumn(ObserverTreeModel::AccessColumn);
        else {
            d->tree_view->showColumn(ObserverTreeModel::AccessColumn);
            d->tree_view->setColumnWidth(ObserverTreeModel::AccessColumn,50);
        }

        // Set name column width to something biggish for now. Will fix later.
        // Also fix in subjectCountChanged() slot.
        d->tree_view->setColumnWidth(0,300);

        //QHeaderView* tree_header = d->tree_view->header();
        //tree_header->setResizeMode(ObserverTreeModel::NameColumn,QHeaderView::Stretch);
    }

    d->initialized = true;
    if (!hints_only) {
        // Construct the property browser if neccesarry
        refreshPropertyBrowser();
        // Setup everything that is needed to handle drag & drop on the item view
        installEventFilter(this);
        handleSubjectCountChanged();
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
            d->top_level_observer = QtilitiesCore::instance()->objectManager()->observerReference(d->navigation_stack.front());
        }
    } else {
        // setObserverContext not yet called, we set it to the front item.
        if (d->navigation_stack.count() > 0)
            d->top_level_observer = QtilitiesCore::instance()->objectManager()->observerReference(d->navigation_stack.front());
    }

    // We need to check the subjectChange signal on all the navigation stack items.
    // For now, we just check if any of them detaches an observer, in that case we close this widget since
    // we cannot garuantee that the detached item doesn't appear in the stack.
    for (int i = 0; i < d->navigation_stack.count(); i++) {
        Observer* stack_observer = QtilitiesCore::instance()->objectManager()->observerReference(d->navigation_stack.at(i));
        if (stack_observer)
            connect(stack_observer,SIGNAL(numberOfSubjectsChanged(Observer::SubjectChangeIndication,QList<QObject*>)),SLOT(contextDetachHandler(Observer::SubjectChangeIndication,QList<QObject*>)));
    }

}

void Qtilities::CoreGui::ObserverWidget::setDisplayFlags(Qtilities::Core::Observer::DisplayFlags display_flags) {
    d->default_display_flags = display_flags;
}

Qtilities::Core::Observer::DisplayFlags Qtilities::CoreGui::ObserverWidget::displayFlags() const {
    return d->default_display_flags;
}

Qtilities::CoreGui::ObserverWidget::DisplayMode Qtilities::CoreGui::ObserverWidget::displayMode() const {
    return d->display_mode;
}

void Qtilities::CoreGui::ObserverWidget::setActionHints(Qtilities::Core::Observer::ActionHints action_hints) {
    d->default_action_hints = action_hints;
}

Qtilities::Core::Observer::ActionHints Qtilities::CoreGui::ObserverWidget::actionHints() const {
    return d->default_action_hints;
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

void Qtilities::CoreGui::ObserverWidget::writeSettings() {
    if (d->settings_string.isEmpty())
        return;
    else
        writeSettings(d->settings_string);
}

void Qtilities::CoreGui::ObserverWidget::writeSettings(const QString& widget_name) {
    if (!d->initialized)
        return;

    if (d->settings_string.isEmpty())
        d->settings_string = widget_name;

    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup(widget_name);
    settings.setValue("display_mode", (int) d->display_mode);
    settings.setValue("state", saveState());
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::ObserverWidget::readSettings(const QString& widget_name) {
    if (!d->initialized)
        return;

    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup(widget_name);

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

    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
    settings.endGroup();

    d->settings_string = widget_name;
    // Connect in order for settings to be written when application quits
    connect(QCoreApplication::instance(),SIGNAL(aboutToQuit()),SLOT(writeSettings()));
    connect(QtilitiesCoreGui::instance(),SIGNAL(settingsUpdateRequest(QString)),SLOT(handleSettingsUpdateRequest(QString)));
}

void Qtilities::CoreGui::ObserverWidget::setGlobalMetaType(const QString& meta_type) {
    d->global_meta_type = meta_type;
}

QString Qtilities::CoreGui::ObserverWidget::globalMetaType() const {
    return d->global_meta_type;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::ObserverWidget::actionProvider() {
    return d->action_provider;
}

void Qtilities::CoreGui::ObserverWidget::handleSettingsUpdateRequest(const QString& request_id) {
    if (request_id == d->settings_string)
        readSettings(request_id);
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

void Qtilities::CoreGui::ObserverWidget::handleSubjectCountChanged() {
    if (!d->initialized || !isVisible())
        return;

    if (d->display_mode == TableView && d->table_view) {
        // Row height
        for (int i = 0; i < d->table_model->rowCount(); i++) {
            d->table_view->setRowHeight(i,17);
            QHeaderView* table_header = d->table_view->horizontalHeader();
            d->table_view->resizeColumnsToContents();
            table_header->setResizeMode(ObserverTableModel::NameColumn,QHeaderView::Stretch);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::constructActions() {
    // We construct each action and then register it
    QList<int> context;
    context.push_front(QtilitiesCore::instance()->contextManager()->contextID(contextString()));

    // ---------------------------
    // Remove All
    // ---------------------------
    d->actionRemoveItem = new QAction(QIcon(ICON_REMOVE_ONE),tr("Remove Item"),this);
    d->action_provider->addAction(d->actionRemoveItem,QStringList(tr("Children")));
    connect(d->actionRemoveItem,SIGNAL(triggered()),SLOT(handle_actionRemoveItem_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_REMOVE_ITEM,d->actionRemoveItem,context);
    // ---------------------------
    // Remove All
    // ---------------------------    
    d->actionRemoveAll = new QAction(QIcon(ICON_REMOVE_ALL),tr("Remove All Children"),this);
    d->action_provider->addAction(d->actionRemoveAll,QStringList(tr("Children")));
    connect(d->actionRemoveAll,SIGNAL(triggered()),SLOT(handle_actionRemoveAll_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_REMOVE_ALL,d->actionRemoveAll,context);
    // ---------------------------
    // Delete All
    // ---------------------------
    d->actionDeleteAll = new QAction(QIcon(ICON_DELETE_ALL),tr("Delete All Children"),this);
    d->action_provider->addAction(d->actionDeleteAll,QStringList(tr("Children")));
    connect(d->actionDeleteAll,SIGNAL(triggered()),SLOT(handle_actionDeleteAll_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_DELETE_ALL,d->actionDeleteAll,context);
    // ---------------------------
    // Switch View
    // ---------------------------
    d->actionSwitchView = new QAction(QIcon(ICON_SWITCH_VIEW),tr("Switch View"),this);
    d->actionSwitchView->setShortcut(QKeySequence("F4"));
    d->action_provider->addAction(d->actionSwitchView,QStringList(tr("View")));
    connect(d->actionSwitchView,SIGNAL(triggered()),SLOT(handle_actionSwitchView_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_SWITCH_VIEW,d->actionSwitchView,context);
    // ---------------------------
    // Refresh View
    // ---------------------------
    d->actionRefreshView = new QAction(QIcon(ICON_REFRESH),tr("Refresh View"),this);
    d->action_provider->addAction(d->actionRefreshView,QStringList(tr("View")));
    connect(d->actionRefreshView,SIGNAL(triggered()),SLOT(handle_actionRefreshView_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_REFRESH_VIEW,d->actionRefreshView,context);
    // ---------------------------
    // New Item
    // ---------------------------
    d->actionNewItem = new QAction(QIcon(ICON_NEW),tr("New Item"),this);
    d->actionNewItem->setShortcut(QKeySequence("+"));
    d->action_provider->addAction(d->actionNewItem,QStringList(tr("Children")));
    connect(d->actionNewItem,SIGNAL(triggered()),SLOT(handle_actionNewItem_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_NEW_ITEM,d->actionNewItem,context);
    // ---------------------------
    // Go To Parent
    // ---------------------------
    d->actionPushUp = new QAction(QIcon(ICON_PUSH_UP_CURRENT),tr("Go To Parent"),this);
    d->actionPushUp->setShortcut(QKeySequence("Left"));
    d->action_provider->addAction(d->actionPushUp,QStringList(tr("Hierarhcy")));
    connect(d->actionPushUp,SIGNAL(triggered()),SLOT(handle_actionPushUp_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_HIERARCHY_UP,d->actionPushUp,context);
    // ---------------------------
    // Go To Parent In New Window
    // ---------------------------
    d->actionPushUpNew = new QAction(QIcon(ICON_PUSH_UP_NEW),tr("Go To Parent (New Window)"),this);
    d->action_provider->addAction(d->actionPushUpNew,QStringList(tr("Hierarhcy")));
    connect(d->actionPushUpNew,SIGNAL(triggered()),SLOT(handle_actionPushUpNew_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_HIERARCHY_UP_NEW,d->actionPushUpNew,context);
    // ---------------------------
    // Push Down
    // ---------------------------
    d->actionPushDown = new QAction(QIcon(ICON_PUSH_DOWN_CURRENT),tr("Push Down"),this);
    d->action_provider->addAction(d->actionPushDown,QStringList(tr("Hierarhcy")));
    d->actionPushDown->setShortcut(QKeySequence("Right"));
    connect(d->actionPushDown,SIGNAL(triggered()),SLOT(handle_actionPushDown_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_HIERARCHY_DOWN,d->actionPushDown,context);
    // ---------------------------
    // Push Down In New Window
    // ---------------------------
    d->actionPushDownNew = new QAction(QIcon(ICON_PUSH_DOWN_NEW),tr("Push Down (New Window)"),this);
    d->action_provider->addAction(d->actionPushDownNew,QStringList(tr("Hierarhcy")));
    connect(d->actionPushDownNew,SIGNAL(triggered()),SLOT(handle_actionPushDownNew_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_HIERARCHY_DOWN_NEW,d->actionPushDownNew,context);
    // ---------------------------
    // Expand All
    // ---------------------------
    d->actionExpandAll = new QAction(QIcon(ICON_MAGNIFY_PLUS),tr("Expand All"),this);
    d->action_provider->addAction(d->actionExpandAll,QStringList(tr("Hierarhcy")));
    d->actionExpandAll->setShortcut(QKeySequence("Ctrl+>"));
    connect(d->actionExpandAll,SIGNAL(triggered()),SLOT(handle_actionExpandAll_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_HIERARCHY_EXPAND,d->actionExpandAll,context);
    // ---------------------------
    // Collapse All
    // ---------------------------
    d->actionCollapseAll = new QAction(QIcon(ICON_MAGNIFY_MINUS),tr("Collapse All"),this);
    d->action_provider->addAction(d->actionCollapseAll,QStringList(tr("Hierarhcy")));
    d->actionCollapseAll->setShortcut(QKeySequence("Ctrl+<"));
    connect(d->actionCollapseAll,SIGNAL(triggered()),SLOT(handle_actionCollapseAll_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_CONTEXT_HIERARCHY_COLLAPSE,d->actionCollapseAll,context);
    // ---------------------------
    // Delete Item
    // ---------------------------
    d->actionDeleteItem = new QAction(QIcon(ICON_DELETE_ONE),tr("Delete Item"),this);
    d->action_provider->addAction(d->actionDeleteItem,QStringList(tr("Item")));
    d->actionDeleteItem->setShortcut(QKeySequence(QKeySequence::Delete));
    connect(d->actionDeleteItem,SIGNAL(triggered()),SLOT(handle_actionDeleteItem_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_SELECTION_DELETE,d->actionDeleteItem,context);
    // ---------------------------
    // Merge Items
    // ---------------------------
    d->actionMergeItems = new QAction(QIcon(ICON_MERGE),tr("Merge Items"),this);
    d->action_provider->addAction(d->actionMergeItems,QStringList(tr("Items")));
    connect(d->actionMergeItems,SIGNAL(triggered()),SLOT(handle_actionMergeItems_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_SELECTION_MERGE_ITEMS,d->actionMergeItems,context);
    // ---------------------------
    // Split Items
    // ---------------------------
    d->actionSplitItems = new QAction(QIcon(ICON_SPLIT),tr("Split Item"),this);
    d->action_provider->addAction(d->actionSplitItems,QStringList(tr("Items")));
    connect(d->actionSplitItems,SIGNAL(triggered()),SLOT(handle_actionSplitItems_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_SELECTION_SPLIT_ITEMS,d->actionSplitItems,context);
    // ---------------------------
    // Find Item
    // ---------------------------
    d->actionFindItem = new QAction(QIcon(ICON_MAGNIFY),tr("Find"),this);
    d->action_provider->addAction(d->actionFindItem,QStringList(tr("View")));
    d->actionFindItem->setShortcut(QKeySequence(QKeySequence::Find));
    connect(d->actionFindItem,SIGNAL(triggered()),SLOT(handle_actionFindItem_triggered()));
    QtilitiesCoreGui::instance()->actionManager()->registerAction(MENU_EDIT_FIND,d->actionFindItem,context);
}

void Qtilities::CoreGui::ObserverWidget::refreshActions() {
    if (!d->initialized) {
        d->action_provider->disableAllActions();
        return;
    }

    // Ok, first we set only actions specified by the observer's action hints to be visible
    if (d->action_hints & Qtilities::Core::Observer::RemoveItem)
        d->actionRemoveItem->setVisible(true);
    else
        d->actionRemoveItem->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::RemoveAll)
        d->actionRemoveAll->setVisible(true);
    else
        d->actionRemoveAll->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::DeleteItem)
        d->actionDeleteItem->setVisible(true);
    else
        d->actionDeleteItem->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::DeleteAll)
        d->actionDeleteAll->setVisible(true);
    else
        d->actionDeleteAll->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::PushDown)
        d->actionPushDown->setVisible(true);
    else
        d->actionPushDown->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::PushDownNew)
        d->actionPushDownNew->setVisible(true);
    else
        d->actionPushDownNew->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::PushUp)
        d->actionPushUp->setVisible(true);
    else
        d->actionPushUp->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::PushUpNew)
        d->actionPushUpNew->setVisible(true);
    else
        d->actionPushUpNew->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::NewItem)
        d->actionNewItem->setVisible(true);
    else
        d->actionNewItem->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::RefreshView)
        d->actionRefreshView->setVisible(true);
    else
        d->actionRefreshView->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::SwitchView)
        d->actionSwitchView->setVisible(true);
    else
        d->actionSwitchView->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::MergeItems)
        d->actionMergeItems->setVisible(true);
    else
        d->actionMergeItems->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::SplitItems)
        d->actionSplitItems->setVisible(true);
    else
        d->actionSplitItems->setVisible(false);

    if (d->action_hints & Qtilities::Core::Observer::FindItem)
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
    } else {
        d->actionCollapseAll->setVisible(true);
        d->actionExpandAll->setVisible(true);
        d->actionCollapseAll->setEnabled(true);
        d->actionExpandAll->setEnabled(true);
        d->actionPushDown->setEnabled(false);
        d->actionPushUp->setEnabled(false);
    }

    // Remove & Delete Items + Navigating Down Actions
    if (selectedObjects().count() == 0) {
        d->actionDeleteItem->setEnabled(false);
        d->actionRemoveItem->setEnabled(false);
        d->actionSplitItems->setEnabled(false);
        d->actionMergeItems->setEnabled(false);

        if (d->display_mode == TableView) {
            d->actionPushDown->setEnabled(false);
            d->actionPushDownNew->setEnabled(false);
        }
    } else {
        d->actionSplitItems->setEnabled(true);

        if (selectedObjects().count() > 1) {
            d->actionMergeItems->setEnabled(true);
        } else {
            d->actionMergeItems->setEnabled(false);
        }

        if (d->display_mode == TableView) {
            d->actionDeleteItem->setEnabled(true);
            d->actionRemoveItem->setEnabled(true);
            d->actionPushDown->setEnabled(false);
            d->actionPushDownNew->setEnabled(false);

            // Inspect property to see if push up, or push down related actions should be enabled.
            if (selectedObjects().count() == 1) {
                // Check if the selected object is an observer.
                QObject* obj = selectedObjects().front();
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
                }
            }
        } else if (d->display_mode == TreeView) {
            if (selectedObjects().count() == 1) {
                Observer* selected = qobject_cast<Observer*> (selectedObjects().front());
                if (selected == d->top_level_observer) {
                    d->actionDeleteItem->setEnabled(false);
                    d->actionRemoveItem->setEnabled(false);
                } else {
                    d->actionDeleteItem->setEnabled(true);
                    d->actionRemoveItem->setEnabled(true);
                }
            }
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::setSelectionParent(Observer* observer) {
    // This function will only be entered in TreeView mode.
    // It is a slot connected to the selection parent changed signal in the tree model.
    if (observer) {
        // The case where the parent is a valid observer. 
        if (d->tree_model)
            d->tree_model->useObserverHints(observer);
        setObserverContext(observer);
        initialize(true);
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionRemoveItem_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        for (int i = 0; i < selectedObjects().count(); i++)
            d_observer->detachSubject(selectedObjects().at(i));
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (selectedObjects().count() != 1)
            return;
        
        // Make sure the selected object is not the top level observer (might happen in a tree view)
        Observer* observer = qobject_cast<Observer*> (selectedObjects().front());
        if (observer == d->top_level_observer)
            return;

        d_observer->detachSubject(selectedObjects().front());
        handle_actionExpandAll_triggered();
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionRemoveAll_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        d_observer->detachAll();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (selectedObjects().count() != 1)
            return;

        d_observer->detachAll();
        if (d_observer == d->top_level_observer)
            return;
        handle_actionExpandAll_triggered();
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionDeleteItem_triggered() {
    if (!d->initialized)
        return;

    int selected_count = selectedObjects().count();

    for (int i = 0; i < selected_count; i++) {
        if (selectedObjects().at(i)) {
            // Make sure the selected object is not the top level observer (might happen in a tree view)
            Observer* observer = qobject_cast<Observer*> (selectedObjects().at(0));
            if (observer != d->top_level_observer)
                delete selectedObjects().at(0);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionDeleteAll_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        d_observer->deleteAll();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        // We must check if there is an selection parent, else use d_observer
        if (d->tree_model->selectionParent()) {
            d->tree_model->selectionParent()->deleteAll();
            handle_actionExpandAll_triggered();
        } else {
            d_observer->deleteAll();
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionNewItem_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        if (selectedObjects().count() == 1)
            emit addNewItem_triggered(selectedObjects().front(), d_observer);
        else {
            // Check if the stack contains a parent for the current d_observer
            if (navigationStack().count() > 0) {
                // Get observer from stack
                Observer* selection_parent = QtilitiesCore::instance()->objectManager()->observerReference(navigationStack().last());
                emit addNewItem_triggered(d_observer, selection_parent);
            } else
                emit addNewItem_triggered(d_observer, 0);
        }
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        if (selectedObjects().count() == 1)
            emit addNewItem_triggered(selectedObjects().front(), d->tree_model->selectionParent());
        else
            emit addNewItem_triggered(0, d->tree_model->selectionParent());
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionRefreshView_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView && d->table_model) {
        d->table_model->handleDataChange();
        handleSubjectCountChanged();
    } else if (d->display_mode == TreeView && d->tree_model && d->tree_view) {
        d->tree_model->rebuildTreeStructure();
        handle_actionExpandAll_triggered();
    }

    setWindowTitle(QString("%1").arg(d_observer->objectName()));

    if (d->navigation_bar)
        d->navigation_bar->refreshHierarchy();
}

void Qtilities::CoreGui::ObserverWidget::handle_actionPushUp_triggered() {
    if (!d->initialized)
        return;

    // Get the parent observer, this will be stored in the navigation stack
    if (d->navigation_stack.count() == 0)
        return;

    // First disconnet all current observer connections
    d_observer->disconnect(this);
    d_observer->disconnect(d->navigation_bar);

    // Setup new observer
    Observer* observer = QtilitiesCore::instance()->objectManager()->observerReference(d->navigation_stack.pop());
    Q_ASSERT(observer);

    setObserverContext(observer);
    initialize();
}

void Qtilities::CoreGui::ObserverWidget::handle_actionPushUpNew_triggered() {
    if (!d->initialized)
        return;

    // Get the parent observer, this will be stored in the navigation stack
    if (d->navigation_stack.count() == 0)
        return;

    Observer* observer = QtilitiesCore::instance()->objectManager()->observerReference(d->navigation_stack.front());
    Q_ASSERT(observer);

    ObserverWidget* new_child_widget = new ObserverWidget(d->display_mode);
    new_child_widget->setDisplayFlags(displayFlags());
    new_child_widget->setActionHints(actionHints());

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

void Qtilities::CoreGui::ObserverWidget::handle_actionPushDown_triggered() {
    if (!d->initialized)
        return;

    if (d->display_mode == TableView) {
        // Set up new observer
        QObject* obj = selectedObjects().front();
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
        if (observer->accessMode() == Qtilities::Core::Observer::LockedAccess) {
            QMessageBox msgBox;
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
    } else if (d->display_mode == TreeView) {
        if (selectedObjects().count() == 1) {
            Observer* observer = qobject_cast<Observer*> (selectedObjects().front());
            if (observer) {
                // Check the observer's access
                if (observer->accessMode() == Qtilities::Core::Observer::LockedAccess) {
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
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionPushDownNew_triggered() {
    if (!d->initialized)
        return;

    Observer* observer = 0;
    ObserverWidget* new_child_widget = 0;

    if (d->display_mode == TableView) {
        // Set up new observer
        QObject* obj = selectedObjects().front();
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
        if (observer->accessMode() == Qtilities::Core::Observer::LockedAccess) {
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
    } else if (d->display_mode == TreeView) {
        if (selectedObjects().count() == 1) {
            observer = qobject_cast<Observer*> (selectedObjects().front());
            if (observer) {
                // Check the observer's access
                if (observer->accessMode() == Qtilities::Core::Observer::LockedAccess) {
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
    }

    new_child_widget->setDisplayFlags(displayFlags());
    new_child_widget->setActionHints(actionHints());
    new_child_widget->setObserverContext(observer);
    new_child_widget->initialize();
    new_child_widget->setSizePolicy(sizePolicy());
    new_child_widget->show();
    newObserverWidgetCreated(new_child_widget);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionSwitchView_triggered() {
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
            if (d->navigation_stack.count() > 0)
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
    QtilitiesCoreGui::instance()->clipboardManager()->setClipboardOrigin(IClipboard::CopyAction);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionCut_triggered() {
    ObserverMimeData *mimeData = new ObserverMimeData(d->current_selection,d_observer->observerID());
    QApplication::clipboard()->setMimeData(mimeData);
    QtilitiesCoreGui::instance()->clipboardManager()->setClipboardOrigin(IClipboard::CutAction);
}

void Qtilities::CoreGui::ObserverWidget::handle_actionPaste_triggered() {
    if (d->action_hints & Qtilities::Core::Observer::PasteItem){
        // Check if the subjects being dropped are of the same type as the destination observer.
        // If this is not the case, we do not allow the drop.
        const ObserverMimeData* observer_mime_data = qobject_cast<const ObserverMimeData*> (QApplication::clipboard()->mimeData());
        if (observer_mime_data) {
            if (d_observer->canAttach(const_cast<ObserverMimeData*> (observer_mime_data)) == Qtilities::Core::Observer::Allowed) {
                // Now check the proposed action of the event.
                if (QtilitiesCoreGui::instance()->clipboardManager()->clipboardOrigin() == IClipboard::CutAction) {
                    QtilitiesCore::instance()->objectManager()->moveSubjects(observer_mime_data->subjectList(),observer_mime_data->sourceID(),d_observer->observerID());
                    QtilitiesCoreGui::instance()->clipboardManager()->acceptMimeData();
                } else if (QtilitiesCoreGui::instance()->clipboardManager()->clipboardOrigin() == IClipboard::CopyAction) {
                    // Attempt to copy the objects
                    // For now we discard objects that cause problems during attachment and detachment
                    for (int i = 0; i < observer_mime_data->subjectList().count(); i++) {
                        // Attach to destination
                        d_observer->attachSubject(observer_mime_data->subjectList().at(i));
                    }
                    QtilitiesCoreGui::instance()->clipboardManager()->acceptMimeData();
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
                      QtilitiesCoreGui::instance()->clipboardManager()->acceptMimeData();
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
                  QtilitiesCoreGui::instance()->clipboardManager()->acceptMimeData();
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
              QtilitiesCoreGui::instance()->clipboardManager()->acceptMimeData();
              break;
          case QMessageBox::Yes:
              break;
          default:
              break;
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::handle_actionMergeItems_triggered() {
    if (!d->initialized)
        return;

    emit mergeItems_triggered();
}

void Qtilities::CoreGui::ObserverWidget::handle_actionSplitItems_triggered() {
    if (!d->initialized)
        return;

    emit splitItems_triggered();
}

void Qtilities::CoreGui::ObserverWidget::handle_actionFindItem_triggered() {
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

    refreshActions();

    // Set the context backend objects
    QList<QObject*> object_list = selectedObjects();

    // Remove contexts from previous selection
    int count = d->appended_contexts.count()-1;
    for (int i = count; i >= 0; i--) {
        QtilitiesCore::instance()->contextManager()->removeContext(d->appended_contexts.at(i),false);
        d->appended_contexts.removeAt(i);
    }

    QStringList contexts_in_current_selection;
    // Check if any of the objects implements IContext, if so we append the given context.
    for (int i = 0; i < object_list.count(); i++) {
        IContext* context = qobject_cast<IContext*> (object_list.at(i));
        if (context) {
            if (!d->appended_contexts.contains(context->contextString())) {
                QtilitiesCore::instance()->contextManager()->appendContext(context->contextString(),false);
                contexts_in_current_selection << context->contextString();
                d->appended_contexts << context->contextString();
            }
        }
    }

    if (d->update_selection_activity)
        QtilitiesCore::instance()->contextManager()->broadcastState();

    // Update the global object list
    if (!d->property_browser_widget) {
        // Set the active objects for the type of observer
        QString meta_type = d_observer->metaObject()->className();
        QtilitiesCore::instance()->objectManager()->setMetaTypeActiveObjects(meta_type,object_list);

        setGlobalObjectSubjectType();
    }

    // If selected objects > 0 and the observer context supports copy & cut, we enable copy/cut
    if (d->current_selection.count() > 0) {
        if (d->action_hints & Qtilities::Core::Observer::CopyItem) {
            // ---------------------------
            // Copy
            // ---------------------------
            // The copy action is not a multi context action. It's enabled/disabled depending on the state of the application
            // clipboard. Thus we just connect to the paste action here and check the observer flag in the paste slot.
            Command* command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_COPY);
            if (command) {
                if (command->action()) {
                    command->action()->setEnabled(true);
                    connect(command->action(),SIGNAL(triggered()),SLOT(handle_actionCopy_triggered()));
                }
            }
        }

        if (d->action_hints & Qtilities::Core::Observer::CutItem) {
            // ---------------------------
            // Cut
            // ---------------------------
            // The cut action is not a multi context action. It's enabled/disabled depending on the state of the application
            // clipboard. Thus we just connect to the paste action here and check the observer flag in the paste slot.
            Command* command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_CUT);
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

    if (d->update_selection_activity) {
        // Check if the observer has a FollowSelection actity policy
        if ((d_observer->activityControlHint() == Qtilities::Core::Observer::FollowSelection) && d->activity_filter) {
            d->activity_filter->setActiveSubjects(object_list);
        }
    }

    Observer* selection_parent = 0;
    // Calculate selection parent if necessary
    if (d->display_mode == TreeView && d->tree_model) {
        selection_parent = d->tree_model->calculateSelectionParent(selectedIndexes());
        d->tree_name_column_delegate->setObserverContext(selection_parent);
    } else if (d->display_mode == TableView) {
        selection_parent = d_observer;
    }

    // Refresh property browser
    refreshPropertyBrowser();

    // Emit signals
    emit selectedObjectsChanged(object_list, selection_parent);
}

void Qtilities::CoreGui::ObserverWidget::disconnectClipboard() {
    Command* command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_COPY);
    if (command) {
        if (command->action()) {
            command->action()->setEnabled(false);
            command->action()->disconnect(this);
        }
    }

    command = QtilitiesCoreGui::instance()->actionManager()->command(MENU_EDIT_CUT);
    if (command) {
        if (command->action()) {
            command->action()->setEnabled(false);
            command->action()->disconnect(this);
        }
    }
}

void Qtilities::CoreGui::ObserverWidget::setGlobalObjectSubjectType() {
    if (!d->global_meta_type.isEmpty()) {
        // Update the global active object type
        if (selectedObjects().count() == 0) {
            QList<QObject*> this_list;

            // Check if the observer has a parent which is not an observer, in that case
            // set the parent as the global object.
            // This is needed when observers are contained behind an interface.
            Observer* obs = qobject_cast<Observer*> (d_observer->parent());
            if (!obs)
                this_list << d_observer->parent();
            else
                this_list << d_observer;
            QtilitiesCore::instance()->objectManager()->setMetaTypeActiveObjects(d->global_meta_type,this_list);
        } else
            QtilitiesCore::instance()->objectManager()->setMetaTypeActiveObjects(d->global_meta_type,d->current_selection);
    }
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
    if (indication == Qtilities::Core::Observer::SubjectRemoved) {
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

void Qtilities::CoreGui::ObserverWidget::selectSubjectsByRef(QList<QObject*> objects) {  
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
        }
        d->update_selection_activity = true;
    } else if (d->tree_view && d->tree_model && d->display_mode == TreeView && d->proxy_model) {
        // We just select the top level item for now:
        // This is because this function will only be connected to the top level observer for now.
        // In the future this should be upgraded, because the function name suggests otherwise.
        /*d->update_selection_activity = false;
        QItemSelectionModel *selection_model = d->tree_view->selectionModel();
        if (selection_model) {
            QModelIndex root_index = d->tree_model->index(0,0);
            selection_model->clearSelection();
            selection_model->select(root_index,QItemSelectionModel::Select);
        }
        d->update_selection_activity = true;*/
        // The above only selects the top level. However this is not correct since the observerContext()
        // is set whenever the user selects an object in the tree. This function should really
        // not cater for a tree model.
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
    if (d->display_flags & Qtilities::Core::Observer::PropertyBrowser) {
        constructPropertyBrowser();
        if (selectedObjects().count() == 1)
            d->property_browser_widget->setObject(selectedObjects().front());
        else
            d->property_browser_widget->setObject(d_observer);
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
