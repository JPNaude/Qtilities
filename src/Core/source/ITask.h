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
                //! The possible results of completed task.
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
                    TaskBusyWithErrors              = 2,  /*!< Some error messages (and possibly warnings as well) have been logged since the task was started. */
                };

                //! The possible states in which a task might find itself.
                enum TaskState {
                    TaskNotStarted                  = 1,  /*!< The task has not been started. */
                    TaskBusy                        = 2,  /*!< The task is busy. */
                    TaskPaused                      = 4,  /*!< The task is paused. */
                    TaskStopped                     = 8,  /*!< The task has been stopped. */
                    TaskCompleted                   = 16,  /*!< The task has been completed. */
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
                    TaskDoNothingWhenStopped        = 2   /*!< Do nothing when it is stopped. This will give the user the chance to review the task log. The fate of the task will then be determined by TaskReviewedAction. */
                };

                //! Indicates what should happen to the task when the user click's on the "Remove Task" button in a Qtilities::CoreGui::SingleTaskWidget. The "Stop Task" becomes the "Remove Task" button when the task was stopped or completed.
                enum TaskRemoveAction {
                    TaskDeleteWhenRemoved            = 0,  /*!< Delete the task when the "Remove Task" button is pressed. */
                    TaskHideWhenRemoved              = 1   /*!< Hide the task's Qtilities::CoreGui::SingleTaskWidget when the "Remove Task" button is pressed. */
                };

                ITask() {
                    qRegisterMetaType<ITask::TaskResult>("ITask::TaskResult");
                    qRegisterMetaType<Logger::MessageType>("Logger::MessageType");
                    qRegisterMetaType<Logger::MessageContextFlags>("Logger::MessageContextFlags");
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
                //! Indicates what should happen to the task when the user click's on the "Cancel Task" button in a Qtilities::CoreGui::SingleTaskWidget
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
                  \note This function does not set the busy state of the task to busy since the success of the execution of the task will depend on the task represented by Task.

                  The default implementation of ITask does nothing.
                  */
                virtual void start() {}
                //! Indicates if users can stop the task.
                virtual bool canStop() const {
                    return false;
                }
                //! Stops the task from the user interface.
                /*!
                  In Qtilities::Core::Task, the task will emit stopTaskRequest() which must be handled by the task represented by Task. Qtilities::Core::QtilitiesProcess is a good example of this.

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
                  In Qtilities::Core::Task, the task will emit pauseTaskRequest() which must be handled by the task represented by Task. Qtilities::Core::QtilitiesProcess is a good example of this.

                  The task can be resumed through resume().
                  \note This function must be a slot in your interface implementation and will only do something when canPause() is true.

                  The default implementation of ITask does nothing.
                  */
                virtual void pause() {}
                //! Resumes the task after it has been paused.
                /*!
                  In Qtilities::Core::Task, the task will emit resumeTaskRequest() which must be handled by the task represented by Task. Qtilities::Core::QtilitiesProcess is a good example of this.

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

            private:
                //! Sets the unique task ID for the this task.
                void setTaskID(int task_id) {
                    d_task_id = task_id;
                }

                int d_task_id;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::ITask,"com.Qtilities.Core.ITask/1.0")

#endif // ITASK_H
