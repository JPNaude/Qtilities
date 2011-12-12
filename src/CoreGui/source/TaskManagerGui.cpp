/****************************************************************************
**
** Copyright 2009, Jaco Naude
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
    TaskManagerGuiPrivateData() {}

    QList<QPointer<SingleTaskWidget> >  task_widgets;
};

Qtilities::CoreGui::TaskManagerGui* Qtilities::CoreGui::TaskManagerGui::m_Instance = 0;

Qtilities::CoreGui::TaskManagerGui* Qtilities::CoreGui::TaskManagerGui::instance() {
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

Qtilities::CoreGui::TaskManagerGui::TaskManagerGui() {
    d = new TaskManagerGuiPrivateData;
}

Qtilities::CoreGui::TaskManagerGui::~TaskManagerGui() {
    for (int i = 0; i < d->task_widgets.count(); i++) {
        if (d->task_widgets.at(i))
            delete d->task_widgets.at(i);
    }
    delete d;
}

SingleTaskWidget* Qtilities::CoreGui::TaskManagerGui::singleTaskWidget(int task_id) {
    //qDebug() << "Constructing single task widget for task: " << task_id << ", Name: " << TASK_MANAGER->hasTask(task_id)->taskName();
    SingleTaskWidget* single_task_widget = new SingleTaskWidget(task_id);
    d->task_widgets << single_task_widget;
    return single_task_widget;
}

AbstractLoggerEngine* Qtilities::CoreGui::TaskManagerGui::assignLoggerEngineToTask(QObject* obj) {
    ITask* task = qobject_cast<ITask*> (obj);
    if (!task)
        return 0;

    if (!task->loggingEnabled())
        return 0;

    if (task->loggerEngine())
        return task->loggerEngine();

    QString engine_name = "Task Log: " + task->displayName();
    LoggerGui::createLogWidget(&engine_name);
    //qDebug() << engine_name;
    AbstractLoggerEngine* log_engine = Log->loggerEngineReference(engine_name);
    Q_ASSERT(log_engine);
    log_engine->setMessageContexts(Logger::EngineSpecificMessages);
    task->setLoggerEngine(log_engine);
    LOG_DEBUG("Assigning logger engine to task: " + task->taskName());
    return log_engine;
}
