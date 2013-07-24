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

#include "TaskSummaryWidget.h"
#include "SingleTaskWidget.h"
#include "ui_TaskSummaryWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "TaskManagerGui.h"

#include <ITask>
#include <QtilitiesCoreApplication>

using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui;
using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::CoreGui::TaskSummaryWidgetPrivateData {
    TaskSummaryWidgetPrivateData() : task_summary_enabled(true),
        last_number_of_displayed_tasks(-1) {}

    QMap<int,QPointer<SingleTaskWidget> >           id_widget_map;
    TaskSummaryWidget::TaskDisplayOptions           display_options;
    TaskSummaryWidget::TaskRemoveOptionFlags        remove_options;
    TaskSummaryWidget::NoActiveTaskHandling         no_active_task_handling;
    QVBoxLayout*                                    layout;
    bool                                            task_summary_enabled;
    int                                             last_number_of_displayed_tasks;
    QList<int>                                      displayed_filter_task_ids;
};

Qtilities::CoreGui::TaskSummaryWidget::TaskSummaryWidget(TaskRemoveOption remove_options, TaskDisplayOptions display_options, QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::TaskSummaryWidget)
{
    ui->setupUi(this);
    d = new TaskSummaryWidgetPrivateData;
    d->display_options = display_options;
    d->remove_options = remove_options;
    d->no_active_task_handling = HideSummaryWidget;

    if (ui->widgetSingleWidgetItemsHolder->layout())
        delete ui->widgetSingleWidgetItemsHolder->layout();

    d->layout = new QVBoxLayout(ui->widgetSingleWidgetItemsHolder);
    d->layout->setMargin(0);
    d->layout->setSpacing(0);
    d->layout->setAlignment(Qt::AlignBottom);

    QObject::connect(OBJECT_MANAGER,SIGNAL(newObjectAdded(QObject*)),SLOT(addTask(QObject*)),Qt::UniqueConnection);
}

Qtilities::CoreGui::TaskSummaryWidget::~TaskSummaryWidget() {
    delete ui;
    delete d;
}

TaskSummaryWidget::TaskDisplayOptions Qtilities::CoreGui::TaskSummaryWidget::taskDisplayOptions() const {
    return d->display_options;
}

void Qtilities::CoreGui::TaskSummaryWidget::setTaskDisplayOptions(TaskDisplayOptions task_display_options) {
    if (d->display_options != task_display_options) {
        d->display_options = task_display_options;
        clear();
        findCurrentTasks();
    }
}

TaskSummaryWidget::NoActiveTaskHandling Qtilities::CoreGui::TaskSummaryWidget::noActiveTaskHandling() const {
    return d->no_active_task_handling;
}

void Qtilities::CoreGui::TaskSummaryWidget::setNoActiveTaskHandling(NoActiveTaskHandling no_active_task_handling) {
     if (d->no_active_task_handling != no_active_task_handling) {
         d->no_active_task_handling = no_active_task_handling;
         clear();
         findCurrentTasks();
     }
}

TaskSummaryWidget::TaskRemoveOptionFlags Qtilities::CoreGui::TaskSummaryWidget::taskRemoveOptionFlags() const {
    return d->remove_options;
}

void Qtilities::CoreGui::TaskSummaryWidget::setTaskRemoveOptionFlags(TaskRemoveOptionFlags task_remove_option_flags) {
    if (d->remove_options != task_remove_option_flags) {
        d->remove_options = task_remove_option_flags;
        clear();
        findCurrentTasks();
    }
}

bool Qtilities::CoreGui::TaskSummaryWidget::taskSummaryEnabled() const {
    return d->task_summary_enabled;
}

void Qtilities::CoreGui::TaskSummaryWidget::setTaskSummaryEnabled(bool enable) {
    d->task_summary_enabled = enable;
}

void Qtilities::CoreGui::TaskSummaryWidget::findCurrentTasks() {
    if (!d->task_summary_enabled)
        return;

    QList<QObject*> iface_list = OBJECT_MANAGER->registeredInterfaces("com.Qtilities.Core.ITask/1.0");
    foreach (QObject* obj, iface_list)
        addTask(obj);

    hideIfNeeded();
}

void Qtilities::CoreGui::TaskSummaryWidget::clear() {
    QList<int> keys = d->id_widget_map.keys();
    QList<QPointer<SingleTaskWidget> > values = d->id_widget_map.values();
    for (int i = 0; i < d->id_widget_map.count(); ++i) {
        ITask* task = TASK_MANAGER->hasTask(keys.at(i));
        if (task)
            task->objectBase()->disconnect(this);
        ui->widgetSingleWidgetItemsHolder->layout()->removeWidget(values.at(i));
    }

    d->id_widget_map.clear();
    hideIfNeeded();
}

void TaskSummaryWidget::setDisplayedTasksFilter(QList<int> displayed_task_ids) {
    if (d->displayed_filter_task_ids != displayed_task_ids) {
        d->displayed_filter_task_ids = displayed_task_ids;
        hideIfNeeded();
    }
}

QList<int> TaskSummaryWidget::displayedTasksFilter() const {
    return d->displayed_filter_task_ids;
}

void TaskSummaryWidget::displayedTasksFilterAddId(int id) {
    if (!d->displayed_filter_task_ids.contains(id)) {
        d->displayed_filter_task_ids.append(id);
        hideIfNeeded();
    }
}

void TaskSummaryWidget::displayedTasksFilterRemoveId(int id) {
    if (d->displayed_filter_task_ids.contains(id)) {
        d->displayed_filter_task_ids.removeOne(id);
        hideIfNeeded();
    }
}

QPointer<SingleTaskWidget> TaskSummaryWidget::getSingleTaskWidgetForTask(int task_id) {
    QPointer<SingleTaskWidget> widget;

    if (d->id_widget_map.contains(task_id))
        return d->id_widget_map[task_id];

    return widget;
}

void Qtilities::CoreGui::TaskSummaryWidget::addTask(QObject* obj) {
    if (!d->task_summary_enabled)
        return;

    ITask* task = qobject_cast<ITask*> (obj);
    if (task) {
        if (d->id_widget_map.contains(task->taskID()))
            return;

        connect(task->objectBase(),SIGNAL(taskStarted(int,QString,Logger::MessageType)),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(subTaskCompleted(int,QString,Logger::MessageType)),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskCompleted(ITask::TaskResult,QString,Logger::MessageType)),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskPaused()),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskResumed()),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskStopped()),SLOT(handleTaskStateChanged()));

        if (task->taskType() == ITask::TaskGlobal || (task->taskType() == ITask::TaskLocal && d->displayed_filter_task_ids.contains(task->taskID()))) {
            SingleTaskWidget* task_widget = TaskManagerGui::instance()->singleTaskWidget(task->taskID());
            task_widget->setPauseButtonVisible(false);
            task_widget->setStopButtonVisible(true);
            d->id_widget_map[task->taskID()] = task_widget;
            connect(task_widget,SIGNAL(destroyed()),SLOT(handleSingleTaskWidgetDestroyed()));
            addSingleTaskWidget(task_widget);
        }
    }
}

void Qtilities::CoreGui::TaskSummaryWidget::addSingleTaskWidget(SingleTaskWidget* single_task_widget) {
    if (single_task_widget) {
        d->layout->insertWidget(0,single_task_widget);
        connect(single_task_widget,SIGNAL(hiddenByStopButton()),SLOT(hideIfNeeded()),Qt::UniqueConnection);
        updateTaskWidget(single_task_widget->task()); // hideIfNeeded() called in here
        connect(single_task_widget->task()->objectBase(),SIGNAL(taskTypeChanged(ITask::TaskType)),SLOT(handleTaskTypeChanged()),Qt::UniqueConnection);
    }
}

void Qtilities::CoreGui::TaskSummaryWidget::handleSingleTaskWidgetDestroyed() {
    QObject* sender_task = sender();
    QList<QPointer<SingleTaskWidget> > map_values = d->id_widget_map.values();
    for (int i = 0; i < map_values.count(); ++i) {
        if (map_values.at(i) == sender_task) {
            // Disconnect from task:
            int task_id = d->id_widget_map.keys().at(i);
            ITask* task = TASK_MANAGER->hasTask(task_id);
            if (task)
                task->objectBase()->disconnect(this);

            d->id_widget_map.remove(task_id);
            break;
            //qDebug() << "TaskSummaryWidget detected destruction of single task widget, disconnecting from and removing task from this widget";
        }
    }

    hideIfNeeded();
}

void Qtilities::CoreGui::TaskSummaryWidget::handleTaskTypeChanged() {
    ITask* sender_task = qobject_cast<ITask*> (sender());
    if (!sender_task)
        return;

    for (int i = 0; i < d->id_widget_map.count(); ++i) {
        if (d->id_widget_map.keys().at(i) == sender_task->taskID()) {
            // Disconnect from task:
            ITask* task = TASK_MANAGER->hasTask(d->id_widget_map.keys().at(i));
            if (task)
                updateTaskWidget(task);
        }
    }

    hideIfNeeded();
}

void Qtilities::CoreGui::TaskSummaryWidget::handleTaskStateChanged() {
    QObject* sender_task = sender();
    if (sender_task) {
        ITask* task = qobject_cast<ITask*> (sender());
        if (task)
            updateTaskWidget(task);
    }
}

void Qtilities::CoreGui::TaskSummaryWidget::updateTaskWidget(ITask* task) {
    if (task) {
        QPointer<SingleTaskWidget> task_widget = d->id_widget_map[task->taskID()];
        if (!task_widget)
            return;

        if (task->taskType() == ITask::TaskLocal && !d->displayed_filter_task_ids.contains(task->taskID())) {
            task_widget->hide();
        } else if (task->taskType() == ITask::TaskGlobal || (d->displayed_filter_task_ids.contains(task->taskID()) && task->taskType() == ITask::TaskLocal)) {
            if (task->state() == ITask::TaskCompleted) {
                if (d->display_options == DisplayOnlyBusyTasks) {
                    if (task->result() == ITask::TaskSuccessful) {
                        if (d->remove_options & RemoveWhenCompletedSuccessfully)
                            task_widget->hide();
                    } else if (task->result() == ITask::TaskSuccessfulWithWarnings) {
                        if (d->remove_options & RemoveWhenCompletedSuccessfullyWithWarnings)
                            task_widget->hide();
                    } else if (task->result() == ITask::TaskFailed) {
                        if (d->remove_options & RemoveWhenFailed)
                            task_widget->hide();
                    } else if (task->result() == ITask::TaskNoResult) {
                        // Do nothing, we should never get in here.
                    }
                }
            } else if (task->state() == ITask::TaskNotStarted) {
                if (task->canStart()) {
                    task_widget->show();
                } else {
                    if (d->display_options == DisplayOnlyBusyTasks)
                        task_widget->hide();
                    else if (d->display_options == DisplayAllTasks)
                        task_widget->show();
                }
            } else if (task->state() == ITask::TaskPaused) {
                task_widget->show();
            } else if (task->state() == ITask::TaskStopped) {
                if (d->remove_options & RemoveWhenStopped)
                    task_widget->hide();
            } else if (task->state() == ITask::TaskBusy) {
                task_widget->show();
            }
        }

        hideIfNeeded();
    }
}

void Qtilities::CoreGui::TaskSummaryWidget::hideIfNeeded() {
    int num_displayed_tasks = 0;
    QList<QPointer<SingleTaskWidget> > id_widget_map_values = d->id_widget_map.values();
    for (int i = 0; i < d->id_widget_map.count(); ++i) {
        QPointer<SingleTaskWidget> task_widget = id_widget_map_values.at(i);
        if (task_widget) {
            if (task_widget->isVisibleTo(this)) {
                if (d->displayed_filter_task_ids.isEmpty()) {
                    task_widget->setMaximumWidth(this->width());
                    ++num_displayed_tasks;
                } else {
                    if (task_widget->task()) {
                        //qDebug() << Q_FUNC_INFO << d->displayed_filter_task_ids << task_widget->task()->taskID() << task_widget->task()->taskName();
                        if (d->displayed_filter_task_ids.contains(task_widget->task()->taskID())) {
                            task_widget->setMaximumWidth(this->width());
                            ++num_displayed_tasks;
                        } else
                            task_widget->hide();
                    } else
                        task_widget->hide();
                }
            }
        }
    }

    if (d->no_active_task_handling == HideSummaryWidget)
        setVisible(num_displayed_tasks > 0);

    if (d->last_number_of_displayed_tasks != num_displayed_tasks)
        emit numberOfDisplayedTasksChanged(num_displayed_tasks);

    d->last_number_of_displayed_tasks = num_displayed_tasks;
}
