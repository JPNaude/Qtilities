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
    TaskSummaryWidgetPrivateData() : task_summary_enabled(true)  {}

    QMap<int,QPointer<SingleTaskWidget> >           id_widget_map;
    TaskSummaryWidget::TaskDisplayOptions           display_options;
    TaskSummaryWidget::TaskRemoveOptionFlags        remove_options;
    TaskSummaryWidget::NoActiveTaskHandling         no_active_task_handling;
    QVBoxLayout*                                    layout;
    bool                                            task_summary_enabled;
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
    foreach (QObject* obj, iface_list) {
        addTask(obj);
    }

    hideIfNeeded();
}

void Qtilities::CoreGui::TaskSummaryWidget::clear() {
    for (int i = 0; i < d->id_widget_map.count(); i++) {
        ITask* task = TASK_MANAGER->hasTask(d->id_widget_map.keys().at(i));
        if (task)
            task->objectBase()->disconnect(this);
        ui->widgetSingleWidgetItemsHolder->layout()->removeWidget(d->id_widget_map.values().at(i));
    }

    d->id_widget_map.clear();
    hideIfNeeded();
}

void Qtilities::CoreGui::TaskSummaryWidget::addTask(QObject* obj) {
    if (!d->task_summary_enabled)
        return;

    ITask* task = qobject_cast<ITask*> (obj);
    if (task) {
        if (d->id_widget_map.keys().contains(task->taskID()))
            return;

        connect(task->objectBase(),SIGNAL(taskStarted(int,QString,Logger::MessageType)),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(subTaskCompleted(int,QString,Logger::MessageType)),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskCompleted(ITask::TaskResult,QString,Logger::MessageType)),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskPaused()),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskResumed()),SLOT(handleTaskStateChanged()));
        connect(task->objectBase(),SIGNAL(taskStopped()),SLOT(handleTaskStateChanged()));

        if (task->taskType() == ITask::TaskGlobal) {
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
        updateTaskWidget(single_task_widget->task());
        connect(single_task_widget->task()->objectBase(),SIGNAL(taskTypeChanged(ITask::TaskType)),SLOT(handleTaskTypeChanged()),Qt::UniqueConnection);
        show();
    }
}

void Qtilities::CoreGui::TaskSummaryWidget::handleSingleTaskWidgetDestroyed() {
    QObject* sender_task = sender();
    for (int i = 0; i < d->id_widget_map.count(); i++) {
        if (d->id_widget_map.values().at(i) == sender_task) {
            // Disconnect from task:
            ITask* task = TASK_MANAGER->hasTask(d->id_widget_map.keys().at(i));
            if (task)
                task->objectBase()->disconnect(this);
            d->id_widget_map.remove(task->taskID());
            //qDebug() << "TaskSummaryWidget detected destruction of single task widget, disconnecting from and removing task from this widget";
        }
    }

    hideIfNeeded();
}

void Qtilities::CoreGui::TaskSummaryWidget::handleTaskTypeChanged() {
    ITask* sender_task = qobject_cast<ITask*> (sender());
    if (!sender_task)
        return;

    for (int i = 0; i < d->id_widget_map.count(); i++) {
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

        if (task->taskType() == ITask::TaskLocal) {
            task_widget->hide();
        } else if (task->taskType() == ITask::TaskGlobal) {
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
    if (d->no_active_task_handling == HideSummaryWidget) {
        for (int i = 0; i < d->id_widget_map.count(); i++) {
            if (d->id_widget_map.values().at(i)) {
                if (d->id_widget_map.values().at(i)->isVisible()) {
                    show();
                    return;
                }
            }
        }

        hide();
    }
}
