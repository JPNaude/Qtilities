/****************************************************************************
**
** Copyright 2009-2013, Jaco Naudé
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library;  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QtilitiesCore>

#include "TaskManagerGui.h"
#include "SingleTaskWidget.h"
#include "WidgetLoggerEngine.h"
#include "LoggerGui.h"

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::Logging;

struct Qtilities::CoreGui::TaskManagerGuiPrivateData {
    TaskManagerGuiPrivateData() : log_initialization(TaskManagerGui::TaskLogLazyInitialization),
        task_progress_updating_enabled(true),
        message_displays_flag(WidgetLoggerEngine::DefaultTaskDisplays) {}

    QList<QPointer<SingleTaskWidget> >      task_widgets;
    TaskManagerGui::TaskLogInitialization   log_initialization;
    bool                                    task_progress_updating_enabled;
    WidgetLoggerEngine::MessageDisplaysFlag message_displays_flag;
};

Qtilities::CoreGui::TaskManagerGui* TaskManagerGui::m_Instance = 0;

Qtilities::CoreGui::TaskManagerGui* TaskManagerGui::instance() {
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new TaskManagerGui;

      mutex.unlock();
    }

    return m_Instance;
}

TaskManagerGui::TaskManagerGui() {
    d = new TaskManagerGuiPrivateData;
}

void TaskManagerGui::handleObjectPoolAddition(QObject *obj) {
    ITask* task = qobject_cast<ITask*> (obj);
    if (!task)
        return;

    if (d->log_initialization == TaskLogActiveInitialization)
        assignLoggerEngineToTask(task);
    else if (d->log_initialization == TaskLogLazyInitialization)
        connect(task->objectBase(),SIGNAL(taskAboutToStart()),SLOT(assignLazyLoggerEngineToTask()));
    else if (d->log_initialization == TaskLogNoInitialization) {
        // Nothing to do here. The task will not get its own WidgetLoggerEngine.
    }
}

TaskManagerGui::~TaskManagerGui() {
    for (int i = 0; i < d->task_widgets.count(); ++i) {
        if (d->task_widgets.at(i))
            delete d->task_widgets.at(i);
    }
    delete d;
}

SingleTaskWidget* TaskManagerGui::singleTaskWidget(int task_id) {
    //qDebug() << "Constructing single task widget for task: " << task_id << ", Name: " << TASK_MANAGER->hasTask(task_id)->taskName();
    SingleTaskWidget* single_task_widget = new SingleTaskWidget(task_id);
    d->task_widgets << single_task_widget;
    return single_task_widget;
}

void TaskManagerGui::disableTaskProgressUpdating() {
    d->task_progress_updating_enabled = false;
}

void TaskManagerGui::enableTaskProgressUpdating() {
    d->task_progress_updating_enabled = true;
}

bool TaskManagerGui::taskProgressUpdatingEnabled() const {
    return d->task_progress_updating_enabled;
}

TaskManagerGui::TaskLogInitialization TaskManagerGui::getTaskLogInitializationMode() const {
    return d->log_initialization;
}

void TaskManagerGui::setTaskLogInitializationMode(Qtilities::CoreGui::TaskManagerGui::TaskLogInitialization log_initialization_mode) {
    d->log_initialization = log_initialization_mode;
}

WidgetLoggerEngine::MessageDisplaysFlag TaskManagerGui::getWidgetLoggerEngineDisplaysFlag() const {
    return d->message_displays_flag;
}

void TaskManagerGui::setWidgetLoggerEngineDisplaysFlag(WidgetLoggerEngine::MessageDisplaysFlag message_displays_flag) {
    d->message_displays_flag = message_displays_flag;
}

AbstractLoggerEngine* TaskManagerGui::assignLoggerEngineToTask(ITask* task, WidgetLoggerEngine::MessageDisplaysFlag message_displays_flag) {
    if (!task)
        return 0;

    if (!task->loggingEnabled())
        return 0;

    if (task->loggerEngine())
        return task->loggerEngine();

    if (message_displays_flag == WidgetLoggerEngine::NoMessageDisplays) {
        if (task->getWidgetLoggerEngineDisplaysFlag() != 0)
            message_displays_flag = (WidgetLoggerEngine::MessageDisplaysFlag) task->getWidgetLoggerEngineDisplaysFlag();
        else
            message_displays_flag = d->message_displays_flag;
    }

    QString engine_name = "Task Log: " + task->taskName();
    LoggerGui::createLogWidget(&engine_name,message_displays_flag);
    //qDebug() << engine_name;
    AbstractLoggerEngine* log_engine = Log->loggerEngineReference(engine_name);
    Q_ASSERT(log_engine);
    log_engine->setRemovable(false);
    log_engine->setMessageContexts(Logger::EngineSpecificMessages);
    task->setLoggerEngine(log_engine);
    LOG_TRACE("Assigning logger engine to task: " + task->taskName());
    return log_engine;
}

void TaskManagerGui::assignLazyLoggerEngineToTask() {
    ITask* task = qobject_cast<ITask*> (sender());
    if (task) {
        LOG_TRACE("Assigning lazy logger engine to task: " + task->taskName());
        assignLoggerEngineToTask(task);
    }
}
