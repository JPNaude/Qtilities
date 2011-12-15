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

#ifndef TASK_H
#define TASK_H

#include "QtilitiesCore_global.h"
#include "ITask.h"

#include <Logger>

#include <QObject>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::Logging;

        /*!
        \struct TaskPrivateData
        \brief Structure used by Task to store private data.
          */
        struct TaskPrivateData;

        /*!
        \class Task
        \brief The Task class is a ready to use implementation of the Qtilities::Core::Interfaces::ITask interface.

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT Task : public QObject, public ITask
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::ITask)

//            Q_ENUMS(ITask::TaskState)
//            Q_ENUMS(ITask::TaskResult)
//            Q_ENUMS(ITask::TaskType)
//            Q_ENUMS(ITask::TaskStopAction)
//            Q_ENUMS(ITask::TaskRemoveAction)
//            Q_ENUMS(ITask::TaskState)

            // Add properties to task to simplify debugging in Qtilities Debug widget:
            Q_PROPERTY(QString Name READ taskName)
            Q_PROPERTY(QString DisplayName READ displayName)
            Q_PROPERTY(int SubTasks READ numberOfSubTasks)
            Q_PROPERTY(int CurrentProgress READ currentProgress)
//            Q_PROPERTY(ITask::TaskState State READ state)
//            Q_PROPERTY(ITask::TaskResult Result READ result)
//            Q_PROPERTY(ITask::TaskType Type READ taskType)
//            Q_PROPERTY(ITask::TaskStopAction StopAction READ taskStopAction)
//            Q_PROPERTY(ITask::TaskRemoveAction RemoveAction READ taskRemoveAction)
            //Q_PROPERTY(ITask::TaskLifeTimeFlags LifeTime READ taskLifeTimeFlags)
            Q_PROPERTY(bool ClearLogOnStart READ clearLogOnStart)
            Q_PROPERTY(bool CanStart READ canStart)
            Q_PROPERTY(bool CanPause READ canPause)
            Q_PROPERTY(bool CanStop READ canStop)

        public:
            //! The possible ways that the lifetime of this task can be managed.
            /*!
              The default is LifetimeManual.
              */
            enum TaskLifeTime {
                LifeTimeManual                              = 1,  /*!< The lifetime of this task is managed manually. */
                LifeTimeDestroyWhenSuccessful               = 2,  /*!< The task will destroy itself when it completed succesfully (without any warnings). */
                LifeTimeDestroyWhenSuccessfullWithWarnings  = 4,  /*!< The task will destroy itself when it completed succesfully (with warnings). */
                LifeTimeDestroyWhenStopped                  = 8,  /*!< The task will destroy itself when stopped. */
                LifeTimeDestroyWhenFailed                   = 16  /*!< The task will destroy itself when it failed. */
            };
            Q_DECLARE_FLAGS(TaskLifeTimeFlags, TaskLifeTime);
            Q_FLAGS(TaskLifeTimeFlags);
            Q_ENUMS(TaskLifeTime)

            //! Default constructor
            /*!
                \param task_name A name for the task.
                \param enable_logging When true, the activity of this task will be logged. Thus, logMessage() will route messages to a logger engine created for this task.
              */
            Task(const QString& task_name, bool enable_logging = true, QObject* parent = 0);
            virtual ~Task();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // -------------------------------------------------------------------------
            // ITask Implementation
            // -------------------------------------------------------------------------
            // Task Information
            // --------------------------------
            QString taskName() const;
            void setDisplayName(const QString& display_name);
            QString displayName() const;
            int numberOfSubTasks() const;
            TaskState state() const;
            TaskBusyState busyState() const;
            TaskResult result() const;
            TaskType taskType() const;
            TaskStopAction taskStopAction() const;
            TaskRemoveAction taskRemoveAction() const;
            void setTaskType(TaskType task_type);
            void setTaskStopAction(TaskStopAction task_stop_action);
            void setTaskRemoveAction(TaskRemoveAction task_remove_action);

            //! Gets the task lifetime.
            TaskLifeTimeFlags taskLifeTimeFlags() const;
            //! Sets the task lifetime.
            /*!
              Set this before starting your task.
              */
            void setTaskLifeTimeFlags(TaskLifeTimeFlags task_lifetime_flags);

            // --------------------------------------------------
            // Logging Functionality
            // --------------------------------------------------
            //! Sets the default message logging context flags of this task.
            /*!
              Default is Logger::EngineSpecificMessages.
              */
            void setLogContext(Logger::MessageContextFlags message_context);
            //! Logs a message to this task.
            void logMessage(const QString& message, Logger::MessageType type = Logger::Info);
            //! Convenience function to log an error in the task log.
            void logError(const QString& message);
            //! Convenience function to log a warning in the task log.
            void logWarning(const QString& message);
            //! Sets if the log must be cleared when the task is started.
            /*!
              True by default.
              */
            void setClearLogOnStart(bool clear_log_on_start = true) const;

            bool loggingEnabled() const;
            AbstractLoggerEngine* loggerEngine() const;
            void setLoggerEngine(AbstractLoggerEngine* engine);
            void clearLog();
            bool clearLogOnStart() const;
            void setCustomLoggerEngine(AbstractLoggerEngine* engine, bool use_only_this_engine = false);
            void removeCustomLoggerEngine();
            AbstractLoggerEngine* customLoggerEngine() const;

        signals:
            void newMessageLogged(const QString& message, Logger::MessageType) const;

        public:
            // --------------------------------
            // Control Functionality
            // --------------------------------
            /*!
              False by default in Qtilities::Core::Task.
              */
            bool canStart() const;
            void setCanStart(bool can_start);
            /*!
              False by default in Qtilities::Core::Task.
              */
            bool canStop() const;
            void setCanStop(bool can_stop);
            /*!
              False by default in Qtilities::Core::Task.
              */
            bool canPause() const;
            void setCanPause(bool can_pause);
        public slots:
            virtual void start();
            virtual void stop();
            virtual void pause();
            virtual void resume();

            //! Function which should be used to start tasks from the process's side.
            bool startTask(int expected_subtasks = -1, const QString& message = QString(), Logger::MessageType type = Logger::Info);
            //! Function which should be used to add completed tasks from the process's side.
            void addCompletedSubTasks(int number_of_sub_tasks = 1, const QString& message = QString(), Logger::MessageType type = Logger::Info);
            //! Function which should be used to complete the task from the process's side.
            bool completeTask(ITask::TaskResult result = ITask::TaskResultFromBusyStateFailOnError, const QString& message = QString(), Logger::MessageType type = Logger::Info);

            // --------------------------------
            // Progress Information
            // --------------------------------
        public:
            int currentProgress() const;

        signals:
            void taskStarted(int expected_subtasks = -1, const QString& message = QString(), Logger::MessageType type = Logger::Info) const;
            void subTaskCompleted(int number_task_completed = 1, const QString& message = QString(), Logger::MessageType type = Logger::Info) const;
            void taskCompleted(ITask::TaskResult result, const QString& message = QString(), Logger::MessageType type = Logger::Info) const;
            void taskPaused() const;
            void taskResumed() const;
            void taskStopped() const;

            void stateChanged(ITask::TaskState new_state, ITask::TaskState old_state) const;
            void busyStateChanged(ITask::TaskBusyState new_busy_state, ITask::TaskBusyState old_busy_state) const;
            void taskTypeChanged(ITask::TaskType new_task_type) const;
            void displayedNameChanged(const QString& displayed_name) const;

            void startTaskRequest();
            void stopTaskRequest();
            void pauseTaskRequest();
            void resumeTaskRequest();

        private:
            //! Updates the busy state of the task. Called when messages are logged while the task is busy.
            void updateBusyState(Logger::MessageType type);

            TaskPrivateData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(Task::TaskLifeTimeFlags);
    }
}

#endif // TASK_H
