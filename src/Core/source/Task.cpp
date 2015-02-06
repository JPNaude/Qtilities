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

#include "Task.h"
#include "QtilitiesCoreApplication.h"

#include <LoggerEngines>

using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::Core::TaskPrivateData {
    TaskPrivateData() : last_error_messages_count(10),
        task_state(ITask::TaskNotStarted),
        task_busy_state(ITask::TaskBusyClean),
        task_result(ITask::TaskNoResult),
        task_type(ITask::TaskGlobal),
        task_lifetime_flags(Task::LifeTimeManual),
        task_stop_action(ITask::TaskDoNothingWhenStopped),
        task_remove_action(ITask::TaskHideWhenRemoved),
        sub_task_performance_indication(ITask::SubTaskTimeFromTaskStart),
        task_stop_confirmation(ITask::TaskStopConfirmationMsgBox),
        number_of_sub_tasks(-1),
        current_progress(0),
        can_start(false),
        can_stop(false),
        can_pause(false),
        log_context(Logger::EngineSpecificMessages),
        logging_enabled(true),
        logging_enabled_to_qt_msg_engine(true),
        logging_enabled_to_console(true),
        clear_log_on_start(true),
        last_run_time(-1),
        parent_task(0) {}

    QString                         task_name;
    QString                         task_display_name;
    QVector<QString>                last_error_messages;
    int                             last_error_messages_count;
    ITask::TaskState                task_state;
    ITask::TaskBusyState            task_busy_state;
    ITask::TaskResult               task_result;
    ITask::TaskType                 task_type;
    Task::TaskLifeTimeFlags         task_lifetime_flags;
    ITask::TaskStopAction           task_stop_action;
    ITask::TaskRemoveAction         task_remove_action;
    ITask::SubTaskPerformanceIndication sub_task_performance_indication;
    ITask::TaskStopConfirmation     task_stop_confirmation;
    int                             number_of_sub_tasks;
    int                             current_progress;
    bool                            can_start;
    bool                            can_stop;
    bool                            can_pause;
    Logger::MessageContextFlags     log_context;
    QPointer<AbstractLoggerEngine>  log_engine;
    QPointer<AbstractLoggerEngine>  custom_log_engine;
    bool                            logging_enabled;
    bool                            logging_enabled_to_qt_msg_engine;
    bool                            logging_enabled_to_console;
    bool                            clear_log_on_start;

    QTime                           timer;
    int                             last_run_time;
    QTimer                          elapsed_time_notification_timer;

    ITask*                          parent_task;
    QPointer<QObject>               parent_task_base;
};

Qtilities::Core::Task::Task(const QString& task_name, bool enable_logging, QObject* parent) : QObject(parent), ITask() {
    d = new TaskPrivateData;
    d->task_name = task_name;
    d->logging_enabled = enable_logging;

    d->elapsed_time_notification_timer.setInterval(1000);
    connect(&d->elapsed_time_notification_timer,SIGNAL(timeout()),SLOT(broadcastElapsedTimeChanged()));

    QtilitiesCoreApplication::taskManager()->assignIdToTask(this);
}

Qtilities::Core::Task::~Task() {
    delete d;
}

void Qtilities::Core::Task::setTaskName(const QString &task_name) {
    if (d->task_name != task_name) {
        d->task_name = task_name;
        emit displayedNameChanged(task_name);
    }
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

int Task::elapsedTime() const {
    if (d->task_state == ITask::TaskNotStarted) {
        if (d->last_run_time == -1)
            return 0;
        else
            return d->last_run_time;
    } else if (d->task_state == ITask::TaskBusy || d->task_state == ITask::TaskPaused)
        return d->timer.elapsed();
    else if (d->task_state == ITask::TaskStopped || d->task_state == ITask::TaskCompleted)
        return d->last_run_time;

    return 0;
}

void Task::setLastRunTime(int msec) {
    d->last_run_time = msec;
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

ITask::SubTaskPerformanceIndication Task::subTaskPerformanceIndication() const {
    return d->sub_task_performance_indication;
}

void Task::setSubTaskPerformanceIndication(ITask::SubTaskPerformanceIndication performance_indication) {
    d->sub_task_performance_indication = performance_indication;
}

ITask::TaskStopConfirmation Task::taskStopConfirmation() const {
    return d->task_stop_confirmation;
}

void Task::setTaskStopConfirmation(ITask::TaskStopConfirmation task_stop_confirmation) {
    d->task_stop_confirmation = task_stop_confirmation;
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

    if (parent_task == this) {
        qDebug() << Q_FUNC_INFO << "Can't set a parent task to be the task itself";
        return;
    }

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
QStringList Task::lastErrorMessages(int count) const {
    QStringList returned_messages;
    if (count == -1)
        count = d->last_error_messages.count();

    for (int i = 0; i < count; i++) {
        if (d->last_error_messages.count() < i+1)
            break;

        if (!d->last_error_messages.at(i).isEmpty())
            returned_messages.append(d->last_error_messages.at(i));
    }

    return returned_messages;
}

void Task::setLastErrorMessagesStackSize(int size) {
    if (size < 0)
        return;

    if (d->last_error_messages_count != size) {
        d->last_error_messages_count = size;
        d->last_error_messages.resize(size);
    }
}

int Task::lastErrorMessagesStackSize() const {
    return d->last_error_messages_count;
}

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

bool Task::loggingToQtMsgEngineEnabled() const {
    return d->logging_enabled_to_qt_msg_engine;
}

void Task::setLoggingToQtMsgEngineEnabled(bool enabled) {
    d->logging_enabled_to_qt_msg_engine = enabled;
}

bool Task::loggingToConsoleEnabled() const {
    return d->logging_enabled_to_console;
}

void Task::setLoggingToConsoleEnabled(bool enabled) {
    d->logging_enabled_to_console = enabled;
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

    if (d->task_state == ITask::TaskBusy && d->last_error_messages_count > 0) {
        if (type == Logger::Error) {
            if (d->last_error_messages.count() == d->last_error_messages_count)
                d->last_error_messages.pop_back();
            d->last_error_messages.push_front(message);
        }
    }

    if (d->log_context & Logger::SystemWideMessages)
        Log->logMessage(QString(),type,message);
    if (d->log_context & Logger::PriorityMessages)
        Log->logPriorityMessage(QString(),type,message);
    if (d->log_context & Logger::EngineSpecificMessages) {
        bool do_console_output_once = false;

        if (d->log_engine) {
            Log->logMessage(d->log_engine->name(),type,message);
            do_console_output_once = true;
        }
        if (d->custom_log_engine) {
            Log->logMessage(d->custom_log_engine->name(),type,message);
            do_console_output_once = true;
        }
        if (parentTask()) {
            parentTask()->logMessage(message,type);
            do_console_output_once = false;
        }

        if (do_console_output_once) {
            if (Log->qtMsgEngineActive() && d->logging_enabled_to_qt_msg_engine && QtilitiesCoreApplication::taskManager()->forwardTaskMessagesToQtMsgEngine())
                QtMsgLoggerEngine::instance()->logMessage(message,type);
            if (Log->consoleEngineActive() && d->logging_enabled_to_console && QtilitiesCoreApplication::taskManager()->forwardTaskMessagesToConsole())
                ConsoleLoggerEngine::instance()->logMessage(message,type);
        }
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
    if (d->can_stop != can_stop) {
        d->can_stop = can_stop;
        emit canStopChanged(can_stop);
    }
}

void Qtilities::Core::Task::setCanPause(bool can_pause) {
    if (d->can_pause != can_pause) {
        d->can_pause = can_pause;
        emit canPauseChanged(can_pause);
    }
}

void Qtilities::Core::Task::setCanStart(bool can_start) {
    if (d->can_start != can_start) {
        d->can_start = can_start;
        emit canStartChanged(can_start);
    }
}

void Qtilities::Core::Task::start() {
    if (d->can_start) {
        //qDebug() << "In start(): " << taskName() << ", state: " << d->task_state;
        if (d->task_state & ITask::TaskIdle) {
            //ITask::TaskState current_state = state();

            //emit taskAboutToStart();
            emit startTaskRequest();

            // Check if the task was started:
//            if (current_state != state()) {
//                emit taskStarted();
//                emit stateChanged(state(),current_state);

//                if (clearLogOnStart())
//                    clearLog();

//                time(&d->timer_start);

//                // The order here is important!
//                d->task_busy_state = ITask::TaskBusyClean;
//                d->current_progress = 0;
//            }
        }
    }
}

void Qtilities::Core::Task::stop() {
    if (d->can_stop) {
        if (d->task_state == ITask::TaskBusy) {
            emit stopTaskRequest();
        }
    }
}

void Qtilities::Core::Task::pause() {
    if (d->can_pause) {
        if (d->task_state == ITask::TaskBusy) {
            emit pauseTaskRequest();
        }
    }
}

void Qtilities::Core::Task::resume() {
    if (d->can_pause) {
        if (d->task_state == ITask::TaskPaused) {
            emit resumeTaskRequest();
        }
    }
}

bool Qtilities::Core::Task::startTask(int expected_subtasks, const QString& message, Logger::MessageType type) {  
    if (d->task_state == ITask::TaskBusy) {
        LOG_DEBUG("Attempting to start task which is already busy. Task name: " + d->task_name + ", Task ID: " + QString::number(taskID()));
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

    if (d->last_run_time == -1)
        d->timer.start();
    else
        d->timer.restart();
    d->last_run_time = 0;

    if (elapsedTimeChangedNotificationsEnabled())
        d->elapsed_time_notification_timer.start();

    emit taskStarted(d->number_of_sub_tasks,message,type);
    emit stateChanged(ITask::TaskBusy,old_state);

    return true;
}

bool Task::stopTask(const QString &message, Logger::MessageType type) {
    emit taskAboutToStop();
    emit taskAboutToComplete();

    if (!message.isEmpty())
        logMessage(message,type);

    ITask::TaskState old_state = d->task_state;
    d->task_state = ITask::TaskStopped;
    blockSignals(true);
    completeTask(ITask::TaskFailed);
    blockSignals(false);    
    emit stateChanged(d->task_state,old_state);
    emit taskStopped();
    emit taskCompleted(ITask::TaskFailed);
    return true;
}

bool Task::pauseTask(const QString &message, Logger::MessageType type) {
    ITask::TaskState current_state = state();

    emit taskAboutToPause();

    if (!message.isEmpty())
        logMessage(message,type);

    d->task_state = Task::TaskPaused;

    logMessage(QString("Task paused (%1).").arg(elapsedTimeString()));

    emit taskPaused();
    emit stateChanged(state(),current_state);
    return true;
}

bool Task::resumeTask(const QString &message, Logger::MessageType type) {
    ITask::TaskState current_state = state();

    emit taskAboutToResume();

    if (!message.isEmpty())
        logMessage(message,type);

    d->task_state = Task::TaskBusy;

    emit taskResumed();
    emit stateChanged(state(),current_state);
    return true;
}

void Qtilities::Core::Task::addCompletedSubTasks(int number_of_sub_tasks, const QString& message, Logger::MessageType type) {
    if (d->task_state != ITask::TaskBusy) {
        LOG_DEBUG("Attempting to register completed sub-tasks in a task which has not been started. Task name: " + d->task_name + ", Task ID: " + QString::number(taskID()));
        return;
    }

    emit taskSubTaskAboutToComplete();

    if (d->sub_task_performance_indication == ITask::SubTaskTimeFromTaskStart)
        logMessage(QString("Subtask completed (%1).").arg(elapsedTimeString(d->timer.elapsed())));

    //qDebug() << "addCompletedSubTasks() progress on task " << taskName() << " with " << number_of_sub_tasks << " new, " << d->current_progress << " current, " << d->number_of_sub_tasks << " total.";
    d->current_progress = d->current_progress + number_of_sub_tasks;
    if (!message.isEmpty())
        logMessage(message,type);

    emit subTaskCompleted(number_of_sub_tasks, message, type);
}

bool Qtilities::Core::Task::completeTask(ITask::TaskResult result, const QString& message, Logger::MessageType type) {
    if (d->task_state != ITask::TaskBusy && d->task_state != ITask::TaskStopped) {
        LOG_DEBUG("Attempting to complete task which is not busy. Task name: " + d->task_name + ", Task ID: " + QString::number(taskID()));
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
    } else
        d->task_result = result; 

    if (d->task_state != ITask::TaskStopped)
        d->task_state = ITask::TaskCompleted;
    d->task_busy_state = ITask::TaskBusyClean;

    if (elapsedTimeChangedNotificationsEnabled())
        d->elapsed_time_notification_timer.stop();
    d->last_run_time = d->timer.elapsed();

    // Log information about the result of the task:
    QString task_name_to_log;
    if (!taskName().isEmpty())
        task_name_to_log = QString("\"%1\" ").arg(taskName());
    if (d->task_result == ITask::TaskSuccessful) {
        logMessage(QString("Task %1completed successfully (%2).").arg(task_name_to_log).arg(elapsedTimeString()));
    } else if (d->task_result == ITask::TaskSuccessfulWithErrors) {
        logMessage(QString("Task %1completed successfully but some warnings and/or errors were logged while the task was busy. See the task log for more information (%2).").arg(task_name_to_log).arg(elapsedTimeString()),Logger::Warning);
    } else if (d->task_result == ITask::TaskSuccessfulWithWarnings) {
        logMessage(QString("Task %1completed successfully but some warnings were logged while the task was busy. See the task log for more information (%2).").arg(task_name_to_log).arg(elapsedTimeString()),Logger::Warning);
    } else if (d->task_result == ITask::TaskFailed) {
        if (d->task_state == ITask::TaskStopped)
            logMessage(QString("Task %1was stopped (%2).").arg(task_name_to_log).arg(elapsedTimeString()),Logger::Info);
        else
            logMessage(QString("Task %1failed. See the task log for more information (%2).").arg(task_name_to_log).arg(elapsedTimeString()),Logger::Error);
    }

    // Note: this signal is blocked when task was stopped in Task::stopTask(). The stopTask() function emits it for us.
    emit stateChanged(d->task_state,old_state);

    // Now we check if we must destroy the task:
    if (d->task_lifetime_flags == Task::LifeTimeManual) {
        emit taskCompleted(d->task_result,message,type);
    } else {
        emit taskCompleted(d->task_result,message,type);
        if (d->task_lifetime_flags & Task::LifeTimeDestroyWhenSuccessful && result == ITask::TaskSuccessful)
            deleteLater();
        else if (d->task_lifetime_flags & Task::LifeTimeDestroyWhenSuccessfullWithWarnings && result == ITask::TaskSuccessfulWithWarnings)
            deleteLater();
        else if (d->task_lifetime_flags & Task::LifeTimeDestroyWhenFailed && result == ITask::TaskFailed)
            deleteLater();
    }

    //qDebug() << "In completeTask(): " << taskName() << ", state: " << d->task_state;
    return true;
}

// --------------------------------
// Progress Information
// --------------------------------
int Qtilities::Core::Task::currentProgress() const {
    return d->current_progress;
}

void Task::broadcastElapsedTimeChanged() {
    emit taskElapsedTimeChanged(elapsedTime());
}
