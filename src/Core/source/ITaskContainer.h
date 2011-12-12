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

#ifndef ITASK_CONTAINER_H
#define ITASK_CONTAINER_H

#include "QtilitiesCore_global.h"
#include "IObjectBase.h"
#include "ITask.h"

#include <QList>

namespace Qtilities {
    namespace Core {
        class Task;
        namespace Interfaces {
            /*!
            \struct ITaskContainerPrivateData
            \brief Structure used by ITaskContainer to store private data.
              */
            struct ITaskContainerPrivateData;

            /*!
            \class ITaskContainer
            \brief Generic interface which provides a way for objects to expose tasks that they can perform.

            This interface allows any object to expose tasks which can be performed by it.

            To find the ID of an exposed task:
\code
int task_id = d->file_utils.findTaskID(d->file_utils.taskNameToString(FileUtils::TaskFindFilesUnderDir));
\endcode

            <i>This class was added in %Qtilities v1.0.</i>
              */
            class QTILIITES_CORE_SHARED_EXPORT ITaskContainer : virtual public IObjectBase {
            public:
                ITaskContainer();
                virtual ~ITaskContainer();

                // ----------------------------------
                // Functions for the outside world to access tasks provided by an object.
                // ----------------------------------
                //! A list of tasks provided by the object.
                QList<ITask*> tasks() const;
                //! Returns the task specified by the given task_id.
                ITask* task(int task_id) const;

                //! Disables the creation and registering in the global object pool of a task object to monitor the progress of the task.
                void disableTask(int task_id);
                //! Enables the creation and registering in the global object pool of a task object to monitor the progress of the task.
                void enableTask(int task_id);
                //! Checks if a specific task is active.
                bool isTaskActive(int task_id) const;

                //! Sets a task a global. See Qtilities::Core::Interfaces::ITask::TaskType.
                void setTaskGlobal(int task_id);
                //! Sets a task a local. See Qtilities::Core::Interfaces::ITask::TaskType.
                void setTaskLocal(int task_id);
                //! Checks if a specific task is global or local. See Qtilities::Core::Interfaces::ITask::TaskType.
                bool isTaskGlobal(int task_id) const;

                //! Returns the task ID for the task specified by the given task_name.
                /*!
                  \returns The task ID linked to the given name. If a task linked to the given task_name does not exist, -1 is returned.
                  */
                int findTaskID(const QString& task_name) const;

            protected:
                // ----------------------------------
                // Functions to be used by the object containing the task to register tasks.
                // ----------------------------------
                //! Registers a task in this container.
                /*!
                  This function will automatically register the task in the global object pool.

                  \note Tasks registered within a container must have unique names.
                  */
                void registerTask(Task* task, const QString& task_name);
                //! Returns the Task object specified by the given task_name.
                Task* findTask(const QString& task_name) const;

            private:
                ITaskContainerPrivateData* container_data;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::ITaskContainer,"com.Qtilities.Core.ITaskContainer/1.0")

#endif // ITASK_CONTAINER_H
