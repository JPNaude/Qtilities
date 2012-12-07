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

#include "ITaskContainer.h"
#include "Task.h"
#include "QtilitiesCoreApplication.h"

using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::Core::Interfaces::ITaskContainerPrivateData {
    ITaskContainerPrivateData() { }

    //! Keys = Tasks, Values = Activity
    QMap<QPointer<Task>,bool>   task_id_activity_map;
    //! Keys = TaskName, Values = Task ID
    QMap<QString,int>           task_name_id_map;
};


Qtilities::Core::Interfaces::ITaskContainer::ITaskContainer() {
    container_data = new ITaskContainerPrivateData;
}

Qtilities::Core::Interfaces::ITaskContainer::~ITaskContainer() {
    delete container_data;
}

QList<ITask*> Qtilities::Core::Interfaces::ITaskContainer::tasks() const {
    QList<ITask*> tasks_ifaces;
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        ITask* iface_cast = qobject_cast<ITask*> (container_data->task_id_activity_map.keys().at(i));
        if (iface_cast)
            tasks_ifaces << iface_cast;
    }
    return tasks_ifaces;
}

ITask* Qtilities::Core::Interfaces::ITaskContainer::task(int task_id) const {
    if (task_id == -1)
        return 0;

    QList<QPointer<Task> > keys = container_data->task_id_activity_map.keys();
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        if (keys.at(i)->taskID() == task_id)
            return keys.at(i);
    }

    return 0;
}

void Qtilities::Core::Interfaces::ITaskContainer::disableTask(int task_id) {
    if (task_id == -1)
        return;

    QList<QPointer<Task> > keys = container_data->task_id_activity_map.keys();
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        if (keys.at(i)->taskID() == task_id)
            container_data->task_id_activity_map[keys.at(i)] = false;
    }

    return;
}

void Qtilities::Core::Interfaces::ITaskContainer::enableTask(int task_id) {
    if (task_id == -1)
        return;

    QList<QPointer<Task> > keys = container_data->task_id_activity_map.keys();
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        if (keys.at(i)->taskID() == task_id)
            container_data->task_id_activity_map[keys.at(i)] = true;
    }

    return;
}

bool Qtilities::Core::Interfaces::ITaskContainer::isTaskActive(int task_id) const {
    if (task_id == -1)
        return false;

    QList<QPointer<Task> > keys = container_data->task_id_activity_map.keys();
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        if (keys.at(i)->taskID() == task_id)
            return container_data->task_id_activity_map[keys.at(i)];
    }

    return false;
}

void Qtilities::Core::Interfaces::ITaskContainer::setTaskGlobal(int task_id) {
    if (task_id == -1)
        return;

    QList<QPointer<Task> > keys = container_data->task_id_activity_map.keys();
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        Task* task = keys.at(i);
        if (task->taskID() == task_id)
            task->setTaskType(ITask::TaskGlobal);
    }

    return;
}

void Qtilities::Core::Interfaces::ITaskContainer::setTaskLocal(int task_id) {
    if (task_id == -1)
        return;

    QList<QPointer<Task> > keys = container_data->task_id_activity_map.keys();
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        Task* task = keys.at(i);
        if (task->taskID() == task_id)
            task->setTaskType(ITask::TaskLocal);
    }

    return;
}

bool Qtilities::Core::Interfaces::ITaskContainer::isTaskGlobal(int task_id) const {
    if (task_id == -1)
        return false;

    QList<QPointer<Task> > keys = container_data->task_id_activity_map.keys();
    for (int i = 0; i < container_data->task_id_activity_map.count(); ++i) {
        Task* task = keys.at(i);
        if (task->taskID() == task_id)
            return (task->taskType() == ITask::TaskGlobal);
    }

    return false;
}

void Qtilities::Core::Interfaces::ITaskContainer::registerTask(Task* task, const QString& task_name) {
    if (!task || task_name.isEmpty())
        return;

    if (task->objectName().isEmpty())
        task->setObjectName(task->taskName());
    OBJECT_MANAGER->registerObject(task,QtilitiesCategory("Tasks"));
    container_data->task_id_activity_map[task] = true;
    container_data->task_name_id_map[task_name] = task->taskID();
}

Task* Qtilities::Core::Interfaces::ITaskContainer::findTask(const QString& task_name) const {
    if (container_data->task_name_id_map.contains(task_name)) {
        ITask* itask = task(container_data->task_name_id_map[task_name]);
        if (itask)
            return qobject_cast<Task*> (itask->objectBase());
    }

    return 0;
}

int Qtilities::Core::Interfaces::ITaskContainer::findTaskID(const QString& task_name) const {
    if (container_data->task_name_id_map.contains(task_name)) {
        ITask* itask = task(container_data->task_name_id_map[task_name]);
        if (itask)
            return itask->taskID();
    }

    return -1;
}
