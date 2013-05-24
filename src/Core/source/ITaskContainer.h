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

            See the \ref page_tasking article for more information on tasking.

            \note ITaskContainer will delete all tasks registered in it during destruction.

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
