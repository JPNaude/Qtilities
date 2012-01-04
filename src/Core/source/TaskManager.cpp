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
        id_counter(-1) { }

    Observer            task_observer;
    QMap<int,QString>   task_id_name_map;
    int                 id_counter;
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
    for (int i = 0; i < d->task_observer.subjectCount(); i++) {
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
    if (d->task_id_name_map.keys().contains(task_id)) {
        // Loop through all tasks and return one that contains task_id:
        for (int i = 0; i < d->task_observer.subjectCount(); i++) {
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
        ++d->id_counter;
        obj->setObjectName("Task with ID: " + QString::number(d->id_counter) + ", Name: " + task->taskName());
        if (d->task_observer.attachSubject(obj,Observer::ManualOwnership))  {
            task->setTaskID(d->id_counter);
            d->task_id_name_map[d->id_counter] = task->taskName();
            LOG_DEBUG(QString("Task Manager: Registering new task with ID \"%1\" and name \"%2\"").arg(d->id_counter).arg(task->taskName()));
            return true;
        } else
            return false;
    } else
        return false;
}

