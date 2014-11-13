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

#ifndef ITASK_H
#define ITASK_H

#include "QtilitiesCore_global.h"
#include "IObjectBase.h"
#include "TaskManager.h"
#include <AbstractLoggerEngine>

#include <QString>
#include <QObject>
#include <QMap>

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            using namespace Qtilities::Logging;

            /*!
            \class ITask
            \brief An interface through which you can communicate with a task. Qtilities::Core::Task is a ready to use implementation of this interface.

            See the \ref page_tasking article for more information on tasking.

            <i>This class was added in %Qtilities v1.0.</i>
              */
            class QTILIITES_CORE_SHARED_EXPORT ITask : virtual public IObjectBase {
                friend class Qtilities::Core::TaskManager;

            public:
                //! The possible results of a completed task.
                enum TaskResult {
                    TaskNoResult                            = 0,  /*!< The task does not have any result information at present. Typically the task have not been started yet. */
                    TaskSuccessful                          = 1,  /*!< The task completed successfully without any warnings and errors. */
                    TaskSuccessfulWithWarnings              = 2,  /*!< The task completed successfully but there was some warnings. */
                    TaskSuccessfulWithErrors                = 3,  /*!< The task completed successfully but there was some errors (and possibly warnings as well). */
                    TaskResultFromBusyStateFailOnError      = 4,  /*!< The task completed and the result must be obtained from the task's busy state. If any errors were logged, the task will fail, thus the result will be TaskFailed.*/
                    TaskResultFromBusyStateSuccessOnError   = 5,  /*!< The task completed and the result must be obtained from the task's busy state. If any errors were logged, the task will be successfull with the TaskSuccessfulWithErrors result.*/
                    TaskFailed                              = 6   /*!< The task failed. */
                };

                //! The possible busy states of the task.
                enum TaskBusyState {
                    TaskBusyClean                   = 0,  /*!< No error or warning messages have been logged since the task was started. */
                    TaskBusyWithWarnings            = 1,  /*!< Some warning messages have been logged since the task was started. */
                    TaskBusyWithErrors              = 2   /*!< Some error messages (and possibly warnings as well) have been logged since the task was started. */
                };

                //! The possible states in which a task might find itself.
                enum TaskState {
                    TaskNotStarted                  = 1,  /*!< The task has not been started. */
                    TaskBusy                        = 2,  /*!< The task is busy. */
                    TaskPaused                      = 4,  /*!< The task is paused. */
                    TaskStopped                     = 8,  /*!< The task has been stopped. */
                    TaskCompleted                   = 16, /*!< The task has been completed. */
                    TaskIdle                        = TaskNotStarted |  TaskCompleted | TaskStopped  /*!< The task is idle, thus it can be started. */
                };

                //! The possible types of tasks.
                enum TaskType {
                    TaskLocal                       = 0,  /*!< A local task, thus its progress should not be shown in Qtilities::CoreGui::TaskSummaryWidget. */
                    TaskGlobal                      = 1   /*!< A global task, thus its progress should be shown in Qtilities::CoreGui::TaskSummaryWidget. */
                };

                //! Indicates what should happen to the task when the user stops it in a Qtilities::CoreGui::SingleTaskWidget.
                enum TaskStopAction {
                    TaskDeleteWhenStopped           = 0,  /*!< Delete the task when it is stopped while it was running. */
                    TaskHideWhenStopped             = 1,  /*!< Hide the task's Qtilities::CoreGui::SingleTaskWidget when the task when it is stopped while it was running. */
                    TaskDoNothingWhenStopped        = 2   /*!< Do nothing when it is stopped. This will give the user the chance to review the task log. */
                };

                //! Indicates if an confirmation message is required when an user stops the task in a Qtilities::CoreGui::SingleTaskWidget.
                enum TaskStopConfirmation {
                    TaskStopConfirmationNone        = 0,  /*!< Stop the task immediately without any confirmation. */
                    TaskStopConfirmationMsgBox      = 1   /*!< Display a message box asking the user if they are sure that they want to stop the task. */
                };

                //! Indicates what should happen to the task when the user click's on the "Remove Task" button in a Qtilities::CoreGui::SingleTaskWidget. The "Stop Task" becomes the "Remove Task" button when the task was stopped or completed.
                enum TaskRemoveAction {
                    TaskDeleteWhenRemoved            = 0,  /*!< Delete the task when the "Remove Task" button is pressed. */
                    TaskHideWhenRemoved              = 1   /*!< Hide the task's Qtilities::CoreGui::SingleTaskWidget when the "Remove Task" button is pressed. */
                };

                //! Indicates how sub tasks are timed.
                enum SubTaskPerformanceIndication {
                    SubTaskNoPerformanceIndication   = 0,  /*!< Sub tasks will not be timed, usefull for tasks with many small, quick sub tasks. */
                    SubTaskTimeFromTaskStart         = 1   /*!< Sub tasks are timed from the start of the task and execution times are printed in the task log when task logging is enabled. */
                };

                ITask() {
                    qRegisterMetaType<ITask::TaskResult>("ITask::TaskResult");
                    d_elapsed_time_notifications_enabled = false;
                    d_widget_logger_engine_message_display_flags = 0;
                    d_task_id = -1;
                }
                virtual ~ITask() {}

                // --------------------------------------------------
                // Task Information
                // --------------------------------------------------
                //! The name of the task.
                /*!
                  \note taskName() should be short, around 3 words.
                  */
                virtual QString taskName() const = 0;
                //! Sets the display name of the task.
                virtual void setDisplayName(const QString& display_name) = 0;
                //! Returns the display name for the task. The display name will typically be set just before emitting taskStarted() and contain a little bit more information than taskName().
                /*!
                  The default implementation will just return the task name for the task. However in many cases its possible to provide
                  a more detailed name to display in any task widget.
                  */
                virtual QString displayName() const  {
                    return taskName();
                }
                //! Returns the number of subtasks associated with the task.
                /*!
                  Default implementation returns -1, thus no sub tasks.
                  */
                virtual int numberOfSubTasks() const {
                    return -1;
                }
                //! Returns the number of subtasks completed.
                /*!
                  Default implementation returns 0, thus no sub tasks.
                  */
                virtual int currentProgress() const {
                    return 0;
                }
                //! The ID of this task.
                int taskID() const {
                    return d_task_id;
                }
                //! Returns the elapsed time in miliseconds of the last time the task was run.
                /*!
                  When a task was paused, the timer will continue to time for the duration that the task was paused.

                  When the task has not been run 0 is returned. When busy the function will return the elapsed time when
                  the function is called. When the task is completed, the function will return the total time the task
                  took to complete.

                  \sa elapsedTimeString(), taskElapsedTimeChanged(), toggleElapsedTimeChangedNotifications()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual int elapsedTime() const = 0;
                //! Returns the elapsed time of the task as a string.
                /*!
                  \param msec The time in miliseconds to format. When -1 (default) the current elapsed time of the task is used.
                  \param format The format to use, see QTime::toString(const QString& format) for details.

                  \sa elapsedTime(), taskElapsedTimeChanged(), toggleElapsedTimeChangedNotifications(), toggleElapsedTimeChangedNotifications()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual QString elapsedTimeString(int msec = -1, const QString & format = "hh:mm:ss.zzz") const {
                    int ref_msecs;
                    if (msec == -1)
                        ref_msecs = elapsedTime();
                    else
                        ref_msecs = msec;

                    QTime ref_time(0,0);
                    QTime ref_time_elapsed = ref_time.addMSecs(ref_msecs);
                    QString time_string;

                    if (format.startsWith("hh:") || format.startsWith("h:")) {
                        uint msecs_24_hrs = 86400000;
                        div_t divresult = div(ref_msecs,msecs_24_hrs);
                        if (divresult.quot > 0)
                            time_string = QString("%1:%2").arg(divresult.quot).arg(ref_time_elapsed.toString(format));
                        else
                            time_string = ref_time_elapsed.toString(format);
                    } else {
                        time_string = ref_time_elapsed.toString(format);
                    }

                    return time_string;
                }
                //! Signal emitted when the task's elapsed time changes.
                /*!
                  This function is only emitted while the task is busy, and when elapsed time change notifications are enabled. See toggleElapsedTimeChangedNotifications().
                  When enabled, it will emit every second while the task is busy.

                  \param The elapsed time in miliseconds of the task at the time that the signal was emitted. You can format the received \p msec value to a string using elapsedTimeString().

                  This function is only emitted while the task is busy.

                  \note This function must be a signal in your interface implementation.

                  \sa elapsedTime(), elapsedTimeString(), toggleElapsedTimeChangedNotifications(), toggleElapsedTimeChangedNotifications()
                  */
                virtual void taskElapsedTimeChanged(int msec) const = 0;
                //! Enables elapsed time changed notifications through taskElapsedTimeChanged().
                /*!
                  Disabled by default.

                  \sa elapsedTime(), elapsedTimeString(), taskElapsedTimeChanged(), elapsedTimeChangedNotificationsEnabled()
                  */
                void toggleElapsedTimeChangedNotifications(bool is_enabled) {
                    d_elapsed_time_notifications_enabled = is_enabled;
                }

                //! Indicates if elapsed time changed notifications through taskElapsedTimeChanged() are enabled.
                /*!
                  Disabled by default.

                  \sa elapsedTime(), elapsedTimeString(), taskElapsedTimeChanged(), toggleElapsedTimeChangedNotifications()
                  */
                bool elapsedTimeChangedNotificationsEnabled() const {
                    return d_elapsed_time_notifications_enabled;
                }

                //! This function allows you to overwrite the last run time of the task.
                /*!
                 * \brief setElapsedTime This is usefull when you need to restore the state of a task for example.
                 * \param msec The new elapsed time in msec.
                 *
                 *<i>This function was added in %Qtilities v1.2.</i>
                 */
                virtual void setLastRunTime(int msec) = 0;

                //! The state of the task.
                /*!
                    Default is TaskNotStarted.
                */
                virtual TaskState state() const = 0;
                //! The busy state of the task.
                /*!
                    Default is TaskBusyClean.
                */
                virtual TaskBusyState busyState() const = 0;
                //! The result of the task.
                /*!
                    Default is TaskNoResult.
                */
                virtual TaskResult result() const = 0;
                //! The type of this task.
                /*!
                    Default is TaskGlobal.
                */
                virtual TaskType taskType() const {
                    return TaskGlobal;
                }
                //! Indicates what should happen to the task when the user click's on the "Stop Task" button in a Qtilities::CoreGui::SingleTaskWidget
                /*!
                    Default is TaskDoNothingWhenStopped.
                */
                virtual TaskStopAction taskStopAction() const {
                    return TaskDoNothingWhenStopped;
                }
                //! Indicates what should happen to the task when the user click's on the "Remove Task" button in a Qtilities::CoreGui::SingleTaskWidget.
                /*!
                    Default is TaskHideWhenRemoved.
                */
                virtual TaskRemoveAction taskRemoveAction() const {
                    return TaskHideWhenRemoved;
                }
                //! Sets the task type.
                virtual void setTaskType(TaskType task_type) = 0;
                //! Sets the task's stop action.
                virtual void setTaskStopAction(TaskStopAction task_stop_action) = 0;
                //! Sets the task's remove action.
                virtual void setTaskRemoveAction(TaskRemoveAction task_remove_action) = 0;

                //! Indicates what should happen to the task when the user click's on the "Stop Task" button in a Qtilities::CoreGui::SingleTaskWidget.
                /*!
                 * Default is TaskStopConfirmationMsgBox.
                 *
                 * \sa  setTaskStopConfirmation()
                 *
                 * <i>This function was added in %Qtilities v1.3.</i>
                 */
                virtual TaskStopConfirmation taskStopConfirmation() const {
                    return TaskStopConfirmationMsgBox;
                }
                //! Sets the task's stop configuration action.
                /*!
                 * \sa taskStopConfirmation()
                 *
                 * <i>This function was added in %Qtilities v1.3.</i>
                 */
                virtual void setTaskStopConfirmation(TaskStopConfirmation task_stop_confirmation) = 0;

                //! Gets what performance measures are used for sub tasks.
                /*!
                    Default is SubTaskTimeFromTaskStart.

                    \sa setSubTaskPerformanceIndication()

                    <i>This function was added in %Qtilities v1.1.</i>
                */
                virtual SubTaskPerformanceIndication subTaskPerformanceIndication() const {
                    return SubTaskTimeFromTaskStart;
                }
                //! Sets what performance measures are used for sub tasks.
                /*!
                    \sa SubTaskPerformanceIndication()

                    <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual void setSubTaskPerformanceIndication(SubTaskPerformanceIndication performance_indication) = 0;

                //! The parent task of this task.
                /*!
                  \sa setParentTask(), removeParentTask()
                  */
                virtual ITask* parentTask() const = 0;
                //! Sets the parent task of this task.
                /*!
                  Allows you to make this task a subtask of another task. What this basically means is that messages from this task will be logged to
                  the parent task and the parent task's busy state will also track the busy state of this task.

                  If you don't want the busy state to be tracked (thus only want to get the messages of this task in the log of a different task, use setCustomLoggerEngine() instead.

                  \note It is recommended to set the parent task of this task before starting it since the parent task will not aquire the busy state of this task in this function.

                  \sa parentTask(), removeParentTask()
                  */
                virtual void setParentTask(ITask* parent_task) = 0;
                //! Removes the parent task of this task.
                /*!
                  \sa setParentTask(), parentTask()
                  */
                virtual void removeParentTask() = 0;

                // --------------------------------------------------
                // Logging Functionality
                // --------------------------------------------------
                //! Returns the last error logged in the task while it was busy.
                /*!
                 * \param count The number of messages to return. Note that the maximum returnable number
                 * of messages are defined by errorMessageStackSize(). Also, if the number of error
                 * messages that were logged at the time this function is called is less than the count
                 * specified, only the number of currently logged messages will be returned. When -1 (default)
                 * all messages in the stack are returned.
                 *
                 * \returns A QStringList with the most recent message logged being the first item in the list,
                 * and the last message being the first message logged.
                 *
                 * \note Only errors logged while this task is busy are taken into account.
                 * \note Logging must be enabled in order for this function to work. See loggingEnabled().
                 *
                 * <i>This function was added in %Qtilities v1.4.</i>
                 */
                virtual QStringList lastErrorMessages(int count = -1) const = 0;
                //! Sets the last error logged stack size.
                /*!
                 * \param size The new size of the last error messages stack.
                 *
                 * \sa lastErrorMessagesStackSize(), lastErrorMessages()
                 *
                 * <i>This function was added in %Qtilities v1.4.</i>
                 */
                virtual void setLastErrorMessagesStackSize(int size) = 0;
                //! Gets the last error logged stack size.
                /*!
                 * The default is 10.
                 *
                 * \returns The size of the last error messages stack.
                 *
                 * \sa lastErrorMessagesStackSize(), lastErrorMessages()
                 *
                 * <i>This function was added in %Qtilities v1.4.</i>
                 */
                virtual int lastErrorMessagesStackSize() const = 0;
                //! Sets the default message logging context flags of this task.
                /*!
                  Default is Logger::EngineSpecificMessages.
                  */
                virtual void setLogContext(Logger::MessageContextFlags message_context) = 0;
                //! Logs a message to this task.
                virtual void logMessage(const QString& message, Logger::MessageType type = Logger::Info) = 0;
                //! Convenience function to log an error in the task log.
                virtual void logError(const QString& message) = 0;
                //! Convenience function to log a warning in the task log.
                virtual void logWarning(const QString& message) = 0;
                //! Sets if the log must be cleared when the task is started.
                /*!
                  True by default.
                  */
                virtual void setClearLogOnStart(bool clear_log_on_start = true) const = 0;

                //! Indicates if the activities of this task is logged.
                /*!
                  False by default.
                  */
                virtual bool loggingEnabled() const {
                    return false;
                }
                //! Gets if the activities of this task is logged to Qtilities::Logging::QtMsgLoggerEngine in addition to loggerEngine().
                /*!
                 * True by default.
                 *
                 * \note Qtilities::Core::TaskManager::setForwardTaskMessagesToQtMsgEngine() must also be true, allowing
                 * the task manager to disable the forwarding of messages to the console for all tasks.
                 * \note The console logger engine must be enabled for messages to appear on the qt message engine. See Qtilities::Logging::Logger::toggleQtMsgEngine().
                 *
                 * \sa setLoggingToQtMsgEngineEnabled(), Qtilities::Core::TaskManager::setForwardTaskMessagesToQtMsgEngine()
                 *
                 * <i>This function was added in %Qtilities v1.5.</i>
                 */
                virtual bool loggingToQtMsgEngineEnabled() const {
                    return true;
                }
                //! Sets if the activities of this task is logged to Qtilities::Logging::QtMsgLoggerEngine in addition to loggerEngine().
                /*!
                 *
                 * \note Qtilities::Core::TaskManager::setForwardTaskMessagesToQtMsgEngine() must also be true, allowing
                 * the task manager to disable the forwarding of messages to the console for all tasks.
                 * \note The console logger engine must be enabled for messages to appear on the qt message engine. See Qtilities::Logging::Logger::toggleQtMsgEngine().
                 *
                 * \sa loggingToQtMsgEngineEnabled(), Qtilities::Core::TaskManager::setForwardTaskMessagesToQtMsgEngine()
                 *
                 * <i>This function was added in %Qtilities v1.5.</i>
                 */
                virtual void setLoggingToQtMsgEngineEnabled(bool enabled) {
                    Q_UNUSED(enabled);
                }
                //! Gets if the activities of this task is logged to Qtilities::Logging::ConsoleLoggerEngine in addition to loggerEngine().
                /*!
                 * True by default.
                 *
                 * \note Qtilities::Core::TaskManager::setForwardTaskMessagesToConsole() must also be true, allowing
                 * the task manager to disable the forwarding of messages to the console for all tasks.
                 * \note The console logger engine must be enabled for messages to appear on the console. See Qtilities::Logging::Logger::toggleConsoleEngine().
                 *
                 * \sa setloggingToConsoleEnabled(), Qtilities::Core::TaskManager::setForwardTaskMessagesToConsole()
                 *
                 * <i>This function was added in %Qtilities v1.5.</i>
                 */
                virtual bool loggingToConsoleEnabled() const {
                    return true;
                }
                //! Sets if the activities of this task is logged to Qtilities::Logging::ConsoleLoggerEngine in addition to loggerEngine().
                /*!
                 *
                 * \note Qtilities::Core::TaskManager::setForwardTaskMessagesToConsole() must also be true, allowing
                 * the task manager to disable the forwarding of messages to the console for all tasks.
                 * \note The console logger engine must be enabled for messages to appear on the console. See Qtilities::Logging::Logger::toggleConsoleEngine().
                 *
                 * \sa loggingToConsoleEnabled(), Qtilities::Core::TaskManager::setForwardTaskMessagesToConsole()
                 *
                 * <i>This function was added in %Qtilities v1.5.</i>
                 */
                virtual void setLoggingToConsoleEnabled(bool enabled) {
                    Q_UNUSED(enabled);
                }
                //! Returns a reference to the logger engine logging activities of this task. When loggingEnabled() is false, this function will always return 0.
                /*!
                  \sa setLoggerEngine()
                  */
                virtual AbstractLoggerEngine* loggerEngine() const {
                    return 0;
                }
                //! Sets the logger engine to be used by this task.
                /*!
                  When logging is enabled this function will be called automatically by Qtilities::CoreGui::TaskManagerGui when the task is registered in the global object pool.

                  If you want this task's messages to be logged in a different logger engine you should call setCustomLoggerEngine(). An example where this is usefull
                  is when you want to log the messages of a task in the log of another task. In that case, call setCustomLoggerEngine() with the loggerEngine() of the
                  task where the messages should be logged.

                  The task will take ownership of the logger engine. This function can only be called once. Thus, if the task already has a logger engine calling this function
                  will do nothing. To use a custom logger engine, see setCustomLoggerEngine().

                  \sa setCustomLoggerEngine()
                  */
                virtual void setLoggerEngine(AbstractLoggerEngine* engine) {
                    Q_UNUSED(engine)
                }
                //! Returns the custom logger engine used by this task.
                /*!
                  \sa setCustomLoggerEngine()
                  */
                virtual AbstractLoggerEngine* customLoggerEngine() const {
                    return 0;
                }
                //! Sets a custom logger engine to be used by this task.
                /*!
                  The task will not take ownership of the custom logger engine.

                  \param use_only_this_engine When true, the task's own logger engine will be deleted and only the custom engine will be used to log messages.

                  \sa setLoggerEngine()
                  */
                virtual void setCustomLoggerEngine(AbstractLoggerEngine* engine, bool use_only_this_engine = false) {
                    Q_UNUSED(engine)
                    Q_UNUSED(use_only_this_engine)
                }
                //! Removes the current custom logger engine from this task.
                /*!
                  \note This function does not delete the custom logger engine which was used, if any.

                  \sa setCustomLoggerEngine()
                  */
                virtual void removeCustomLoggerEngine() {}
                //! Clears the task's log.
                virtual void clearLog() {
                    // Do nothing by default.
                }
                //! When true, the task's log will be cleared everytime the task is started.
                virtual bool clearLogOnStart() const {
                    return false;
                }

                //! Gets the (int) WidgetLoggerEngine::MessageDisplaysFlag used when assigning logger engines to tasks.
                /*!
                  The default is WidgetLoggerEngine::DefaultDisplays.

                  \sa setWidgetLoggerEngineDisplaysFlag

                  <i>This function was added in %Qtilities v1.2.</i>
                  */
                int getWidgetLoggerEngineDisplaysFlag() const {
                    return d_widget_logger_engine_message_display_flags;
                }
                //! Sets the (int) WidgetLoggerEngine::MessageDisplaysFlag used when assigning logger engines to tasks.
                /*!
                  \sa getWidgetLoggerEngineDisplaysFlag

                  <i>This function was added in %Qtilities v1.2.</i>
                  */
                void setWidgetLoggerEngineDisplaysFlag(int message_display_flags) {
                    d_widget_logger_engine_message_display_flags = message_display_flags;
                }

                // --------------------------------------------------
                // Control Functionality
                // --------------------------------------------------
                //! Indicates if users can start the task.
                virtual bool canStart() const {
                    return false;
                }
                //! Starts the task from the user interface.
                /*!
                  The slot called when the user starts the task from it's Qtilities::CoreGui::SingleTaskWidget. Programmatically, you should use startTask() instead of start().
                  In Qtilities::Core::Task, the task will emit startTaskRequest() which must be handled by the task represented by Task. Qtilities::Core::QtilitiesProcess is a good example of this.

                  \note This function must be a slot in your interface implementation and will only do something when canStart() is true.

                  The default implementation of ITask does nothing.
                  */
                virtual void start() {}
                //! Indicates if users can stop the task.
                virtual bool canStop() const {
                    return false;
                }
                //! Stops the task from the user interface.
                /*!
                  In Qtilities::Core::Task, the task will emit stopTaskRequest() which must be handled by the process represented by Task. Qtilities::Core::QtilitiesProcess is a good example of this.

                  \note This function must be a slot in your interface implementation and will only do something when canStop() is true.

                  The default implementation of ITask does nothing.
                  */
                virtual void stop() {}              
                //! Indicates if users can pause the task.
                virtual bool canPause() const {
                    return false;
                }
                //! Pauses the task from the user interface.
                /*!
                  In Qtilities::Core::Task, the task will emit pauseTaskRequest() which must be handled by the process represented by Task. Qtilities::Core::QtilitiesProcess is a good example of this.

                  The task can be resumed through resume().

                  \note This function must be a slot in your interface implementation and will only do something when canPause() is true.

                  The default implementation of ITask does nothing.
                  */
                virtual void pause() {}
                //! Resumes the task after it has been paused.
                /*!
                  In Qtilities::Core::Task, the task will emit resumeTaskRequest() which must be handled by the process represented by Task. Qtilities::Core::QtilitiesProcess is a good example of this.

                  \note This function must be a slot in your interface implementation and will only do something if the task is in the paused state.

                  The default implementation of ITask does nothing.
                  */
                virtual void resume() {}              

                // --------------------------------------------------
                // Progress Information
                // --------------------------------------------------
                //! Signal emitted when the task started.
                /*!
                  \param expected_subtasks The number of subtasks which must be completed in order for this task to complete. If you don't provide the number of subtasks,
                  progress for the task will be shown as a progress bar as a busy indicator. See QProgressBar for more information on this. If you do provide the number of subtasks,
                  the progress be updated everytime subtaskCompleted() is emitted.

                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskStarted(int expected_subtasks = -1, const QString& message = QString(), Logger::MessageType type = Logger::Info) const = 0;
                //! Signal emitted when subtasks are completed in order to show the correct progress information.
                /*!
                  \param number_task_completed The number of subtasks completed, 1 by default.
                  \param subtask_message A message which describes the subtask(s) completed.

                  \note This function must be a signal in your interface implementation.
                  */
                virtual void subTaskCompleted(int number_task_completed = 1, const QString& message = QString(), Logger::MessageType type = Logger::Info) const = 0;
                //! Signal emitted when the task started.
                /*!
                  \param result The result of the task.
                  \param message A message which describes the completion of the task.

                  \note This function must be a signal in your interface implementation.
                  \note Do not delete the task in the implementation of this signal. Instead, set the task's lifetime flags using Task::setTaskLifeTimeFlags().
                  */
                virtual void taskCompleted(ITask::TaskResult result, const QString& message = QString(), Logger::MessageType type = Logger::Info) const = 0;
                //! Signal emitted when the task is paused.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskPaused() const = 0;
                //! Signal emitted when the task is resumed after it was paused.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskResumed() const = 0;
                //! Signal emitted when the task is stopped during execution.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskStopped() const = 0;

                //! Signal emitted when the task's state changes.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void stateChanged(TaskState new_state, TaskState old_state) const = 0;
                //! Signal emitted when the busy state of the task changes.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void busyStateChanged(TaskBusyState new_busy_state, TaskBusyState old_busy_state) const = 0;
                //! Signal emitted when the task's type changes.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskTypeChanged(TaskType new_task_type) const = 0;
                //! Signal emitted when the task's displayed name changes.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void displayedNameChanged(const QString& displayed_name) const = 0;
                //! Signal emitted when the new messages are logged in this task.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void newMessageLogged(const QString& message, Logger::MessageType) const = 0;

                //! Signal emitted when the task is about to be paused.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskAboutToPause() const = 0;
                //! Signal emitted when the task is about to be stopped.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskAboutToStop() const = 0;
                //! Signal emitted when the task is about to be started.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskAboutToStart() const = 0;
                //! Signal emitted when the task is about to be resumed from a paused state.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskAboutToResume() const = 0;
                //! Signal emitted when the task is about to be completed.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskAboutToComplete() const = 0;
                //! Signal emitted when a sub task is about to be completed.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void taskSubTaskAboutToComplete() const = 0;
                //! Signal emitted when a task's ability to be started changed.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void canStartChanged(bool new_value) const = 0;
                //! Signal emitted when a task's ability to be stopped changed.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void canStopChanged(bool new_value) const = 0;
                //! Signal emitted when a task's ability to be paused changed.
                /*!
                  \note This function must be a signal in your interface implementation.
                  */
                virtual void canPauseChanged(bool new_value) const = 0;

            public:
                //! Message logging function which directs messages aimed for a task to the task if the task exists, or to the logger as system wide messages if the task does not exist.
                /*!
                  In some cases a function gets a task to log its messages to as a parameter. There is off course the possibility that whatever is calling
                  the function does not pass a valid task. In those cases it is still desirable to log the messages of the function somewhere.

                  Thus function automatically directs messages to the correct place. For example:

\code
void MyObject::doSomething(ITask* task) {
    // Instead of having to do this:
    if (task)
        task->logMessage("Message",Logger::Info);
    else
        Log->logMessage(QString(),Logger::Info,"Message");

    // We can just do this:
    ITask::logMessageToTask("Message",task,Logger::Info);

    // Macros makes this even easier and allows us to do things like:
    LOG_TASK_INFO("Message",task);
}
\endcode

                    If no task was passed to the function, the messages will be logged as system wide messages to the logger. If a valid task was
                    received the message will be logged to the task.

                    <i>This function was added in %Qtilities v1.1.</i>

                    \sa logPriorityMessageToTask
                  */
                static void logMessageToTask(const QString& message, ITask* task = 0, Logger::MessageType type = Logger::Info) {
                    if (task)
                        task->logMessage(message,type);
                    else
                        Log->logMessage(QString(),type,message);
                }

                //! Message logging function which logs the message as a priority message and directs messages aimed for a task to the task if the task exists, or to the logger as system wide messages if the task does not exist.
                /*!
                  This function is the same as logMessage(), except that the message is also logged as a priority message. This is usefull in some scenarios, for example:
                  When loading a project and the loading fails, we want to log this message as a priority message in order to appear in the QtilitiesMainWindow priority
                  message area, but we also want to capture the message in the task log.

                  For example:

\code
void MyObject::doSomething(ITask* task) {
    // Instead of having to do this:
    ITask::logMessage("Message",task,Logger::Info);
    Log->logPriorityMessage(QString(),Logger::Info,"Message");

    // We can just do this:
    ITask::logPriorityMessageToTask("Message",task,Logger::Info);

    // Macros makes this even easier and allows us to do things like:
    LOG_TASK_INFO_P("Message",task);
}
\endcode

                    <i>This function was added in %Qtilities v1.1.</i>

                    \sa logMessageToTask()
                  */
                static void logPriorityMessageToTask(const QString& message, ITask* task = 0, Logger::MessageType type = Logger::Info) {
                    if (task)
                        task->logMessage(message,type);
                    else
                        Log->logMessage(QString(),type,message);

                    Log->logPriorityMessage(QString(),type,message);
                }

            private:
                //! Sets the unique task ID for the this task.
                void setTaskID(int task_id) {
                    d_task_id = task_id;
                }

                int d_task_id;
                bool d_elapsed_time_notifications_enabled;
                int d_widget_logger_engine_message_display_flags;
            };
        }
    }
}

// -----------------------------------
// Basic Task Logging Macros
// -----------------------------------
//! Logs a trace message directed at a task.
/*!
    \note Trace messages are not part of release mode builds.

    \sa Qtilities::Core::Interfaces::ITask::logMessageToTask()
  */
#define LOG_TASK_TRACE(Msg,task) Qtilities::Core::Interfaces::ITask::logMessageToTask(Msg,task,Qtilities::Logging::Logger::Trace)
//! Logs a debug message directed at a task.
/*!
    \note Trace messages are not part of release mode builds.

    \sa Qtilities::Core::Interfaces::ITask::logMessageToTask()
  */
#define LOG_TASK_DEBUG(Msg,task) Qtilities::Core::Interfaces::ITask::logMessageToTask(Msg,task,Qtilities::Logging::Logger::Debug)
//! Logs an error message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logMessageToTask()
  */
#define LOG_TASK_ERROR(Msg,task) Qtilities::Core::Interfaces::ITask::logMessageToTask(Msg,task,Qtilities::Logging::Logger::Error)
//! Logs a warning message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logMessageToTask()
  */
#define LOG_TASK_WARNING(Msg,task) Qtilities::Core::Interfaces::ITask::logMessageToTask(Msg,task,Qtilities::Logging::Logger::Warning)
//! Logs a fatal message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logMessageToTask()
  */
#define LOG_TASK_FATAL(Msg,task) Qtilities::Core::Interfaces::ITask::logMessageToTask(QMsg,task,Qtilities::Logging::Logger::Fatal)
//! Logs an information message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logMessageToTask()
  */
#define LOG_TASK_INFO(Msg,task) Qtilities::Core::Interfaces::ITask::logMessageToTask(Msg,task,Qtilities::Logging::Logger::Info)

// -----------------------------------
// Priority Task Logging Macros
// -----------------------------------
//! Logs a priority trace message directed at a task.
/*!
    \note Trace messages are not part of release mode builds.

    \sa Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask()
  */
#define LOG_TASK_TRACE_P(Msg,task) Qtilities::Core::Interfaces::ITask::logPriorityMessageToTaskToTask(Msg,task,Qtilities::Logging::Logger::Trace)
//! Logs a priority debug message directed at a task.
/*!
    \note Trace messages are not part of release mode builds.

    \sa Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask()
  */
#define LOG_TASK_DEBUG_P(Msg,task) Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask(Msg,task,Qtilities::Logging::Logger::Debug)
//! Logs a priority error message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask()
  */
#define LOG_TASK_ERROR_P(Msg,task) Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask(Msg,task,Qtilities::Logging::Logger::Error)
//! Logs a priority warning message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask()
  */
#define LOG_TASK_WARNING_P(Msg,task) Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask(Msg,task,Qtilities::Logging::Logger::Warning)
//! Logs a priority fatal message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask()
  */
#define LOG_TASK_FATAL_P(Msg,task) Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask(Msg,task,Qtilities::Logging::Logger::Fatal)
//! Logs a priority information message directed at a task.
/*!
    \sa Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask()
  */
#define LOG_TASK_INFO_P(Msg,task) Qtilities::Core::Interfaces::ITask::logPriorityMessageToTask(Msg,task,Qtilities::Logging::Logger::Info)


Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::ITask,"com.Qtilities.Core.ITask/1.0")

#endif // ITASK_H
