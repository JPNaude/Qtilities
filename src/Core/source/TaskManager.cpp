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

#include "QtilitiesCoreConstants.h"
#include "TaskManager.h"
#include "Observer.h"
#include "ITask.h"

#include <Logger>

#include <QList>
#include <QtDebug>
#include <QPointer>
#include <QCoreApplication>

using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core::Interfaces;

struct Qtilities::Core::TaskManagerPrivateData {
    TaskManagerPrivateData() : task_observer(qti_def_GLOBAL_OBJECT_POOL),
        id_counter(-1),
        forward_task_messages_to_qt_msg_engine(false),
        forward_task_messages_to_console_engine(false) { }

    Observer            task_observer;
    QMap<int,QString>   task_id_name_map;
    int                 id_counter;
    bool                forward_task_messages_to_qt_msg_engine;
    bool                forward_task_messages_to_console_engine;
};

Qtilities::Core::TaskManager::TaskManager(QObject* parent) : QObject(parent) {
    d = new TaskManagerPrivateData;
    d->task_observer.setObjectName("Tasks Observer");
    setObjectName("Task Manager");
}

Qtilities::Core::TaskManager::~TaskManager(){
    delete d;
}

QList<int> Qtilities::Core::TaskManager::allTaskIDs() const {
    return d->task_id_name_map.keys();
}

QList<ITask*> Qtilities::Core::TaskManager::allTasks() const {
    QList<ITask*> all_tasks;
    for (int i = 0; i < d->task_observer.subjectCount(); ++i) {
        ITask* task = qobject_cast<ITask*> (d->task_observer.subjectAt(i));
        if (task)
            all_tasks << task;
    }
    return all_tasks;
}

QStringList Qtilities::Core::TaskManager::taskNames() const {
    return d->task_id_name_map.values();
}

ITask* Qtilities::Core::TaskManager::hasTask(const int task_id) const {
    if (d->task_id_name_map.contains(task_id)) {
        // Loop through all tasks and return one that contains task_id:
        for (int i = 0; i < d->task_observer.subjectCount(); ++i) {
            ITask* task = qobject_cast<ITask*> (d->task_observer.subjectAt(i));
            if (task) {
                if (task->taskID() == task_id)
                    return task;
            }
        }
    }

    return 0;
}

ITask* Qtilities::Core::TaskManager::hasTask(const QString& task_name) const {
    if (d->task_id_name_map.values().contains(task_name)) {
        int id = d->task_id_name_map.key(task_name);
        return hasTask(id);
    } else
        return 0;
}

int Qtilities::Core::TaskManager::taskID(const QString& task_name) {
    if (d->task_id_name_map.values().contains(task_name)) {
        return d->task_id_name_map.key(task_name);
    } else
        return -1;
}

QString Qtilities::Core::TaskManager::taskName(const int task_id, bool* ok) const {
    ITask* task = hasTask(task_id);
    if (task) {
        if (ok)
            *ok = true;
        return task->objectBase()->objectName();
    } else {
        if (ok)
            *ok = false;
        return QString();
    }
}

void TaskManager::setForwardTaskMessagesToQtMsgEngine(bool is_enabled) {
    d->forward_task_messages_to_qt_msg_engine = is_enabled;
}

bool TaskManager::forwardTaskMessagesToQtMsgEngine() const {
    return d->forward_task_messages_to_qt_msg_engine;
}

void TaskManager::setForwardTaskMessagesToConsole(bool is_enabled) {
    d->forward_task_messages_to_console_engine = is_enabled;
}

bool TaskManager::forwardTaskMessagesToConsole() const {
    return d->forward_task_messages_to_console_engine;
}

bool TaskManager::assignIdToTask(ITask *task) {
    if (!task)
        return false;

    if (task->taskID() != -1)
        return false;

    ++d->id_counter;
    task->setTaskID(d->id_counter);
    d->task_id_name_map[d->id_counter] = task->taskName();
    return true;
}

void Qtilities::Core::TaskManager::removeTask(const int task_id) {
    ITask* task = hasTask(task_id);
    if (task) {
        LOG_DEBUG(QString("Task Manager: Removing task with ID \"%1\" and name \"%2\"").arg(task->taskID()).arg(task->taskName()));
        d->task_observer.detachSubject(task->objectBase());
    }
}

void Qtilities::Core::TaskManager::removeTask(QObject* obj) {
    ITask* task = qobject_cast<ITask*> (obj);
    if (task) {
        LOG_DEBUG(QString("Task Manager: Removing task with ID \"%1\" and name \"%2\"").arg(task->taskID()).arg(task->taskName()));
        d->task_observer.detachSubject(obj);
    }
}

bool Qtilities::Core::TaskManager::addTask(QObject* obj) {
    ITask* task = qobject_cast<ITask*> (obj);
    if (task) {
        if (task->taskID() == -1) {
            ++d->id_counter;
            if (obj->objectName().isEmpty())
                obj->setObjectName("Task with ID: " + QString::number(d->id_counter) + ", Name: " + task->taskName());
            if (d->task_observer.attachSubject(obj,Observer::ManualOwnership))  {
                task->setTaskID(d->id_counter);
                d->task_id_name_map[d->id_counter] = task->taskName();
                LOG_TRACE(QString("Task Manager: Registering new task with ID \"%1\" and name \"%2\"").arg(d->id_counter).arg(task->taskName()));
                return true;
            } else
                return false;
        } else {
            if (obj->objectName().isEmpty())
                obj->setObjectName("Task with ID: " + QString::number(task->taskID()) + ", Name: " + task->taskName());
            if (d->task_observer.attachSubject(obj,Observer::ManualOwnership))  {
                LOG_TRACE(QString("Task Manager: Registering new task with ID \"%1\" and name \"%2\"").arg(task->taskID()).arg(task->taskName()));
                return true;
            } else
                return false;
        }
    } else
        return false;
}

