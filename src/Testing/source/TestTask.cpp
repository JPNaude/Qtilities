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

#include "TestTask.h"

#include <QtilitiesCore>
using namespace QtilitiesCore;

int Qtilities::Testing::TestTask::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestTask::testBusyState() {
    Task task("Test Task");
    QVERIFY(task.state() == ITask::TaskNotStarted);
    task.start();
    QVERIFY(task.state() == ITask::TaskNotStarted);
    task.startTask();
    QVERIFY(task.state() == ITask::TaskBusy);
    task.logMessage("Info message");
    QVERIFY(task.busyState() == ITask::TaskBusyClean);
    task.logWarning("Warning message");
    QVERIFY(task.busyState() == ITask::TaskBusyWithWarnings);
    task.logError("Error message");
    QVERIFY(task.busyState() == ITask::TaskBusyWithErrors);
    task.logMessage("Info message");
    QVERIFY(task.busyState() == ITask::TaskBusyWithErrors);
    task.logWarning("Warning message");
    QVERIFY(task.busyState() == ITask::TaskBusyWithErrors);

    task.completeTask();
    QVERIFY(task.state() == ITask::TaskCompleted);
    QVERIFY(task.result() == ITask::TaskFailed);
}
