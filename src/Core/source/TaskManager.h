/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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
