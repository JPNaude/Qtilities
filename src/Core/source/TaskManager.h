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

#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <QObject>
#include <QPointer>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            class ITask;
        }
    }
}
using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace Core {
        /*!
        \struct TaskManagerPrivateData
        \brief A structure storing private data in the TaskManager class.
          */
        struct TaskManagerPrivateData;

        /*!
        \class TaskManager
        \brief A class which represents a task manager.

        The task manager is responsible to monitor the global object pool for tasks registered in it and to provide information about these tasks.

        See the \ref page_tasking article for more information on tasking.

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT TaskManager : public QObject
        {
            Q_OBJECT

        public:
            TaskManager(QObject* parent = 0);
            ~TaskManager();

            //! Returns the IDs of all tasks.
            QList<int> allTaskIDs() const;
            //! Returns all the tasks managed by the task manager.
            QList<ITask*> allTasks() const;
            //! Returns the names of all tasks.
            QStringList taskNames() const;
            //! Checks if a specific task exists and returns a reference to it when found, 0 otherwise.
            ITask* hasTask(const int task_id) const;
            //! Checks if a task with the specified task name exists.
            /*!
              If multiple tasks with task_name exists this function will returns the ID of the first one found.
              */
            ITask* hasTask(const QString& task_name) const;
            //! Maps a task name to the corresponding task ID.
            /*!
              If multiple tasks with task_name exists this function will returns the ID of the first one found.

              If no task with \p task_name exists, -1 will be returned.
              */
            int taskID(const QString& task_name);
            //! Maps a task ID to the corresponding task string.
            /*!
              \param ok Will return true if a task existed with the specified task_id, false otherwise.
              */
            QString taskName(const int task_id, bool* ok = 0) const;

            //! Sets if tasks should forward messages logged to the Qtilities::Logging::QtMsgLoggerEngine.
            /*!
             * Disabled by default.
             *
             * \note Each task also has the ability to disable/enable message forwarding to Qtilities::Logging::QtMsgLoggerEngine which
             * is enabled by default. Thus, this function can be used to disable/enable application wide forwarding to this engine. When
             * enabled, individual engines can be enabled/disables for fine grained control.
             *
             * \note The qt message logger engine must be enabled for messages to appear on the console. See Qtilities::Logging::Logger::toggleQtMsgEngine().
             *
             * \sa forwardTaskMessagesToQtMsgEngine(), Qtilities::Core::Interfaces::ITask::loggingToQtMsgEngineEnabled()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void setForwardTaskMessagesToQtMsgEngine(bool is_enabled);
            //! Gets if tasks should forward messages logged to the Qtilities::Logging::QtMsgLoggerEngine.
            /*!
             * Disabled by default.
             *
             * \note Each task also has the ability to disable/enable message forwarding to Qtilities::Logging::QtMsgLoggerEngine which
             * is enabled by default. Thus, this function can be used to disable/enable application wide forwarding to this engine. When
             * enabled, individual engines can be enabled/disables for fine grained control.
             *
             * \note The qt message logger engine must be enabled for messages to appear on the console. See Qtilities::Logging::Logger::toggleQtMsgEngine().
             *
             * \sa setForwardTaskMessagesToQtMsgEngine(), Qtilities::Core::Interfaces::ITask::setLoggingToQtMsgEngineEnabled()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            bool forwardTaskMessagesToQtMsgEngine() const;

            //! Sets if tasks should forward messages logged to the Qtilities::Logging::ConsoleLoggerEngine.
            /*!
             * Disabled by default.
             *
             * \note Each task also has the ability to disable/enable message forwarding to Qtilities::Logging::ConsoleLoggerEngine which
             * is enabled by default. Thus, this function can be used to disable/enable application wide forwarding to this engine. When
             * enabled, individual engines can be enabled/disables for fine grained control.
             *
             * \sa forwardTaskMessagesToConsole(), Qtilities::Core::Interfaces::ITask::loggingToConsoleEnabled()
             *
             * \note The console logger engine must be enabled for messages to appear on the console. See Qtilities::Logging::Logger::toggleConsoleEngine().
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void setForwardTaskMessagesToConsole(bool is_enabled);
            //! Gets if tasks should forward messages logged to the Qtilities::Logging::ConsoleLoggerEngine.
            /*!
             * Disabled by default.
             *
             * \note Each task also has the ability to disable/enable message forwarding to Qtilities::Logging::ConsoleLoggerEngine which
             * is enabled by default. Thus, this function can be used to disable/enable application wide forwarding to this engine. When
             * enabled, individual engines can be enabled/disables for fine grained control.
             *
             * \note The console logger engine must be enabled for messages to appear on the console. See Qtilities::Logging::Logger::toggleConsoleEngine().
             *
             * \sa setForwardTaskMessagesToConsole(), Qtilities::Core::Interfaces::ITask::setloggingToConsoleEnabled()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            bool forwardTaskMessagesToConsole() const;
            //! Function which can be used to assign an ID to a new task.
            /*!
             * New tasks does not have a unique ID by default. The standard way to assign tasks IDs are to register them
             * in the global object pool (using OBJECT_MANAGER->registerObject()).
             *
             * However, in cases where the task is local and you don't want to register it in the global object pool, but
             * you still need an ID, you can assign an ID to the task manually using this function.
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             *
             * \return True if successfull, false otherwise. If a task already has an ID assigned (thus, taskID() != -1), this function will fail.
             */
            bool assignIdToTask(ITask* task);

        public slots:
            //! Removes the task specified by task_id if it exists.
            void removeTask(const int task_id);
            //! Slot which will check if obj is a task and remove it if needed.
            void removeTask(QObject* obj);    
            //! Slot which will check if obj is a task and register it if needed.
            /*!
              \returns True if the task was successfully added, false otherwise.
              */
            bool addTask(QObject* obj);                

        signals:
            //! Called when a new task is registered in the global object pool.
            void newTaskAdded(ITask* new_task);
            //! Called when a task if removed from the global object pool.
            void taskRemoved(ITask* task_removed);

        private:
            QString contextName(int id) const;
            TaskManagerPrivateData* d;
        };
    }
}

#endif // TASK_MANAGER_H
