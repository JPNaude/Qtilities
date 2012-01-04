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
