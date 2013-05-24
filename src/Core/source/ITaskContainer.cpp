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
    // We must delete all tasks:
    QMapIterator<QPointer<Task>, bool> task_itr(container_data->task_id_activity_map);
    while (task_itr.hasNext()) {
        task_itr.next();
        if (task_itr.key())
            delete task_itr.key();
    }
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
