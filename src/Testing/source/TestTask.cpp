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
    QCOMPARE(task.state(), ITask::TaskNotStarted);
    task.start();
    QCOMPARE(task.state(), ITask::TaskNotStarted);
    task.startTask();
    QCOMPARE(task.state(), ITask::TaskBusy);
    task.logMessage("Info message");
    QCOMPARE(task.busyState(), ITask::TaskBusyClean);
    task.logWarning("Warning message");
    QCOMPARE(task.busyState(), ITask::TaskBusyWithWarnings);
    task.logError("Error message");
    QCOMPARE(task.busyState(), ITask::TaskBusyWithErrors);
    task.logMessage("Info message");
    QCOMPARE(task.busyState(), ITask::TaskBusyWithErrors);
    task.logWarning("Warning message");
    QCOMPARE(task.busyState(), ITask::TaskBusyWithErrors);

    task.completeTask();
    QCOMPARE(task.state(), ITask::TaskCompleted);
    QCOMPARE(task.result(), ITask::TaskFailed);
}
