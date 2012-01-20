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

#include "Task.h"

#include <stdio.h>
#include <time.h>

using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::Core::TaskPrivateData {
    TaskPrivateData() : task_state(ITask::TaskNotStarted),
        task_busy_state(ITask::TaskBusyClean),
        task_result(ITask::TaskNoResult),
        task_type(ITask::TaskGlobal),
        task_lifetime_flags(Task::LifeTimeManual),
        task_stop_action(ITask::TaskDoNothingWhenStopped),
        task_remove_action(ITask::TaskHideWhenRemoved),
        number_of_sub_tasks(-1),
        current_progress(0),
        can_start(false),
        can_stop(false),
        can_pause(false),
        log_context(Logger::EngineSpecificMessages),
        logging_enabled(true),
        clear_log_on_start(true),
        parent_task(0) {}

    QString                         task_name;
    QString                         task_display_name;
    ITask::TaskState                task_state;
    ITask::TaskBusyState            task_busy_state;
    ITask::TaskResult               task_result;
    ITask::TaskType                 task_type;
    Task::TaskLifeTimeFlags         task_lifetime_flags;
    ITask::TaskStopAction           task_stop_action;
    ITask::TaskRemoveAction         task_remove_action;
    int                             number_of_sub_tasks;
    int                             current_progress;
    bool                            can_start;
    bool                            can_stop;
    bool                            can_pause;
    Logger::MessageContextFlags     log_context;
    QPointer<AbstractLoggerEngine>  log_engine;
    QPointer<AbstractLoggerEngine>  custom_log_engine;
    bool                            logging_enabled;
    bool                            clear_log_on_start;

    time_t                          timer_start;
    time_t                          timer_end;

    ITask*                          parent_task;
    QPointer<QObject>               parent_task_base;
};

Qtilities::Core::Task::Task(const QString& task_name, bool enable_logging, QObject* parent) : QObject(parent), ITask() {
    d = new TaskPrivateData;
    d->task_name = task_name;
    d->logging_enabled = enable_logging;
}

Qtilities::Core::Task::~Task() {
    delete d;
}

QString Qtilities::Core::Task::taskName() const {
    return d->task_name;
}

QString Qtilities::Core::Task::displayName() const {
    if (d->task_display_name.isEmpty())
        return d->task_name;
    else
        return d->task_display_name;
}

int Qtilities::Core::Task::numberOfSubTasks() const {
    return d->number_of_sub_tasks;
}

ITask::TaskState Qtilities::Core::Task::state() const {
    return d->task_state;
}

ITask::TaskBusyState Qtilities::Core::Task::busyState() const {
    return d->task_busy_state;
}

ITask::TaskResult Qtilities::Core::Task::result() const {
    return d->task_result;
}

ITask::TaskType Qtilities::Core::Task::taskType() const {
    return d->task_type;
}

ITask::TaskStopAction Qtilities::Core::Task::taskStopAction() const {
    return d->task_stop_action;
}

ITask::TaskRemoveAction Qtilities::Core::Task::taskRemoveAction() const {
    return d->task_remove_action;
}

void Qtilities::Core::Task::setDisplayName(const QString& display_name) {
    if (d->task_display_name != display_name) {
        d->task_display_name = display_name;
        emit displayedNameChanged(display_name);
    }
}

void Qtilities::Core::Task::setTaskType(TaskType task_type) {
    if (d->task_type != task_type) {
        d->task_type = task_type;
        emit taskTypeChanged(task_type);
    }
}

void Qtilities::Core::Task::setTaskStopAction(TaskStopAction task_stop_action) {
    d->task_stop_action = task_stop_action;
}

void Qtilities::Core::Task::setTaskRemoveAction(TaskRemoveAction task_remove_action) {
    d->task_remove_action = task_remove_action;
}

ITask* Qtilities::Core::Task::parentTask() const {
    if (d->parent_task_base && d->parent_task)
        return d->parent_task;
    else {
        d->parent_task = 0;
        return 0;
    }
}

void Qtilities::Core::Task::setParentTask(ITask* parent_task) {
    if (!parent_task)
        return;

    d->parent_task = parent_task;
    d->parent_task_base = parent_task->objectBase();
}

void Qtilities::Core::Task::removeParentTask() {
    d->parent_task = 0;
    d->parent_task_base = 0;
}

Task::TaskLifeTimeFlags Qtilities::Core::Task::taskLifeTimeFlags() const {
    return d->task_lifetime_flags;
}

void Qtilities::Core::Task::setTaskLifeTimeFlags(TaskLifeTimeFlags task_lifetime_flags) {
    d->task_lifetime_flags = task_lifetime_flags;
}

// --------------------------------------------------
// Logging Functionality
// --------------------------------------------------
void Qtilities::Core::Task::setLogContext(Logger::MessageContextFlags log_context) {
    d->log_context = log_context;
}

bool Qtilities::Core::Task::loggingEnabled() const {
    return d->logging_enabled;
}

AbstractLoggerEngine* Qtilities::Core::Task::loggerEngine() const {
    return d->log_engine;
}

void Qtilities::Core::Task::setLoggerEngine(AbstractLoggerEngine* engine) {
    if (!d->log_engine && engine) {
        engine->setParent(this);
        d->log_engine = engine;
    }
}

void Qtilities::Core::Task::clearLog() {
    if (d->log_engine)
        d->log_engine->clearLog();
}

void Qtilities::Core::Task::setClearLogOnStart(bool clear_log_on_start) const {
    d->clear_log_on_start = clear_log_on_start;
}

bool Qtilities::Core::Task::clearLogOnStart() const {
    return d->clear_log_on_start;
}

void Qtilities::Core::Task::setCustomLoggerEngine(AbstractLoggerEngine* engine, bool use_only_this_engine) {
    if (use_only_this_engine && d->log_engine)
        delete d->log_engine;

    d->custom_log_engine = engine;
}

void Qtilities::Core::Task::removeCustomLoggerEngine() {
    d->custom_log_engine = 0;
}

AbstractLoggerEngine* Qtilities::Core::Task::customLoggerEngine() const {
    return d->custom_log_engine;
}

void Qtilities::Core::Task::logMessage(const QString& message, Logger::MessageType type) {
    if (!d->logging_enabled)
        return;

    emit newMessageLogged(message,type);

    if (d->task_state == ITask::TaskBusy || d->task_state == ITask::TaskPaused)
        updateBusyState(type);

    if (d->log_context & Logger::SystemWideMessages)
        Log->logMessage(QString(),type,message);
    if (d->log_context & Logger::PriorityMessages)
        Log->logPriorityMessage(QString(),type,message);
    if (d->log_context & Logger::EngineSpecificMessages) {
        if (d->log_engine)
            Log->logMessage(d->log_engine->name(),type,message);
        if (d->custom_log_engine)
            Log->logMessage(d->custom_log_engine->name(),type,message);
        if (parentTask())
            parentTask()->logMessage(message,type);
    }
}

void Qtilities::Core::Task::logError(const QString& message) {
    logMessage(message,Logger::Error);
}

void Qtilities::Core::Task::logWarning(const QString& message) {
    logMessage(message,Logger::Warning);
}

void Qtilities::Core::Task::updateBusyState(Logger::MessageType type) {
    if (type == Logger::Warning) {
        if (d->task_busy_state == ITask::TaskBusyClean) {
            d->task_busy_state = ITask::TaskBusyWithWarnings;
            emit busyStateChanged(ITask::TaskBusyWithWarnings,ITask::TaskBusyClean);
        }
    } else if (type == Logger::Error || type == Logger::Fatal) {
        if (d->task_busy_state == ITask::TaskBusyClean || d->task_busy_state == ITask::TaskBusyWithWarnings) {
            ITask::TaskBusyState old_busy_state = d->task_busy_state;
            d->task_busy_state = ITask::TaskBusyWithErrors;
            emit busyStateChanged(ITask::TaskBusyWithErrors,old_busy_state);
        }
    }
}

// --------------------------------
// Control Functionality
// --------------------------------
bool Qtilities::Core::Task::canStart() const {
    return d->can_start;
}

bool Qtilities::Core::Task::canStop() const {
    return d->can_stop;
}

bool Qtilities::Core::Task::canPause() const {
    return d->can_pause;
}

void Qtilities::Core::Task::setCanStop(bool can_stop) {
    d->can_stop = can_stop;
}

void Qtilities::Core::Task::setCanPause(bool can_pause) {
    d->can_pause = can_pause;
}

void Qtilities::Core::Task::setCanStart(bool can_start) {
    d->can_start = can_start;
}

void Qtilities::Core::Task::start() {
    if (d->can_start) {
        //qDebug() << "In start(): " << taskName() << ", state: " << d->task_state;
        if (d->task_state & ITask::TaskIdle) {
            ITask::TaskState current_state = state();

            emit taskAboutToStart();
            emit startTaskRequest();

            // Check if the task was started:
            if (current_state != state()) {
                emit taskStarted();
                emit stateChanged(state(),current_state);

                if (clearLogOnStart())
                    clearLog();

                time(&d->timer_start);

                // The order here is important!
                d->task_busy_state = ITask::TaskBusyClean;
                d->current_progress = 0;
            }
        }
    }
}

void Qtilities::Core::Task::stop() {
    if (d->can_stop) {
        if (d->task_state == ITask::TaskBusy) {
            ITask::TaskState current_state = state();

            emit taskAboutToStop();
            emit stopTaskRequest();

            // Check if the task was stopped:
            if (current_state != state()) {
                time(&d->timer_end);
                double diff = difftime(d->timer_end,d->timer_start);
                logMessage(QString(tr("Task stopped after %1 second(s).")).arg(QString::number(diff)));

                d->task_busy_state = ITask::TaskBusyClean;
                emit taskStopped();
                emit stateChanged(state(),current_state);

                // Now we check if we must destroy the task:
                if (d->task_lifetime_flags & Task::LifeTimeDestroyWhenStopped)
                    deleteLater();
            }
        }
    }
}

void Qtilities::Core::Task::pause() {
    if (d->can_pause) {
        if (d->task_state == ITask::TaskBusy) {
            ITask::TaskState current_state = state();

            emit taskAboutToPause();
            emit pauseTaskRequest();

            // Check if the task was paused:
            if (current_state != state()) {
                time(&d->timer_end);
                double diff = difftime(d->timer_end,d->timer_start);
                logMessage(QString(tr("Task paused after %1 second(s).")).arg(QString::number(diff)));

                emit taskPaused();
                emit stateChanged(state(),current_state);
                //qDebug() << "In pause(): " << taskName() << ", state: " << d->task_state;
            }
        }
    }
}

void Qtilities::Core::Task::resume() {
    if (d->can_pause) {
        if (d->task_state == ITask::TaskPaused) {
            ITask::TaskState current_state = state();

            emit taskAboutToResume();
            emit resumeTaskRequest();

            // Check if the task was resumed:
            if (current_state != state()) {
                emit taskResumed();
                emit stateChanged(state(),current_state);
                //qDebug() << "In resume(): " << taskName() << ", state: " << d->task_state;
            }
        }
    }
}

bool Qtilities::Core::Task::startTask(int expected_subtasks, const QString& message, Logger::MessageType type) {  
    if (d->task_state == ITask::TaskBusy) {
        Log->toggleQtMsgEngine(true);
        LOG_DEBUG("Attempting to start task which is already busy. Task name: " + d->task_name + ", Task ID: " + taskID());
        Log->toggleQtMsgEngine(false);
        return false;
    }

    ITask::TaskState old_state = d->task_state;
    emit taskAboutToStart();

    //qDebug() << "Starting task " << taskName() << " with " << expected_subtasks << " subtasks.";
    d->number_of_sub_tasks = expected_subtasks;
    d->current_progress = 0;
    d->task_state = ITask::TaskBusy;

    //qDebug() << "In startTask(): " << taskName() << ", state: " << d->task_state;

    if (clearLogOnStart())
        clearLog();
    if (!message.isEmpty())
        logMessage(message,type);

    time(&d->timer_start);

    emit taskStarted(d->number_of_sub_tasks,message,type);
    emit stateChanged(ITask::TaskBusy,old_state);

    return true;
}

void Qtilities::Core::Task::addCompletedSubTasks(int number_of_sub_tasks, const QString& message, Logger::MessageType type) {
    if (d->task_state != ITask::TaskBusy) {
        Log->toggleQtMsgEngine(true);
        LOG_DEBUG("Attempting to register completed sub-tasks in a task which has not been started. Task name: " + d->task_name + ", Task ID: " + taskID());
        Log->toggleQtMsgEngine(false);
        return;
    }

    emit taskSubTaskAboutToComplete();

    time(&d->timer_end);
    double diff = difftime(d->timer_end,d->timer_start);
    logMessage(QString(tr("Subtask completed after %1 second(s).")).arg(QString::number(diff)));

    //qDebug() << "addCompletedSubTasks() progress on task " << taskName() << " with " << number_of_sub_tasks << " new, " << d->current_progress << " current, " << d->number_of_sub_tasks << " total.";
    d->current_progress = d->current_progress + number_of_sub_tasks;
    if (!message.isEmpty())
        logMessage(message,type);

    emit subTaskCompleted(number_of_sub_tasks, message, type);
}

bool Qtilities::Core::Task::completeTask(ITask::TaskResult result, const QString& message, Logger::MessageType type) {
    if (d->task_state != ITask::TaskBusy) {
        Log->toggleQtMsgEngine(true);
        LOG_DEBUG("Attempting to complete task which is not busy. Task name: " + d->task_name + ", Task ID: " + taskID());
        Log->toggleQtMsgEngine(false);
        return false;
    }

    ITask::TaskState old_state = d->task_state;
    emit taskAboutToComplete();

    if (!message.isEmpty())
        logMessage(message,type);

    // Check if we must get the task result from the busy state:
    if (result == ITask::TaskResultFromBusyStateFailOnError) {
        if (d->task_busy_state == ITask::TaskBusyWithErrors)
            d->task_result = ITask::TaskFailed;
        else if (d->task_busy_state == ITask::TaskBusyWithWarnings)
            d->task_result = ITask::TaskSuccessfulWithWarnings;
        else
            d->task_result = ITask::TaskSuccessful;
    } else if (result == ITask::TaskResultFromBusyStateSuccessOnError) {
        if (d->task_busy_state == ITask::TaskBusyWithErrors)
            d->task_result = ITask::TaskSuccessfulWithErrors;
        else if (d->task_busy_state == ITask::TaskBusyWithWarnings)
            d->task_result = ITask::TaskSuccessfulWithWarnings;
        else
            d->task_result = ITask::TaskSuccessful;
    } else {
        d->task_result = result;
    }

    time(&d->timer_end);
    double diff = difftime(d->timer_end,d->timer_start);
    logMessage(QString(tr("Task completed after %1 second(s).")).arg(QString::number(diff)));

    d->task_state = ITask::TaskCompleted;
    d->task_busy_state = ITask::TaskBusyClean;

    // Log information about the result of the task:
    if (d->task_result == ITask::TaskSuccessful) {
        logMessage(QString(tr("Task \"%1\" completed successfully.")).arg(taskName()));
    } else if (d->task_result == ITask::TaskSuccessfulWithErrors) {
        logMessage(QString(tr("Task \"%1\" completed successfully but some warnings and/or errors were logged in while the task was busy. See the task log for more information.")).arg(taskName()),Logger::Warning);
    } else if (d->task_result == ITask::TaskSuccessfulWithWarnings) {
        logMessage(QString(tr("Task \"%1\" completed successfully but some warnings were logged in while the task was busy. See the task log for more information.")).arg(taskName()),Logger::Warning);
    } else if (d->task_result == ITask::TaskFailed) {
        logMessage(QString(tr("Task \"%1\" failed. See the task log for more information.")).arg(taskName()),Logger::Error);
    }

    emit taskCompleted(result,message,type);
    emit stateChanged(ITask::TaskCompleted,old_state);

    // Now we check if we must destroy the task:
    if (d->task_lifetime_flags & Task::LifeTimeDestroyWhenSuccessful && result == ITask::TaskSuccessful)
        deleteLater();
    else if (d->task_lifetime_flags & Task::LifeTimeDestroyWhenSuccessfullWithWarnings && result == ITask::TaskSuccessfulWithWarnings)
        deleteLater();
    else if (d->task_lifetime_flags & Task::LifeTimeDestroyWhenFailed && result == ITask::TaskFailed)
        deleteLater();

    //qDebug() << "In completeTask(): " << taskName() << ", state: " << d->task_state;

    return true;
}

// --------------------------------
// Progress Information
// --------------------------------
int Qtilities::Core::Task::currentProgress() const {
    return d->current_progress;
}
