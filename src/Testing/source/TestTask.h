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

#ifndef TEST_TASK_H
#define TEST_TASK_H

#include "Testing_global.h"
#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::Task.
        class TESTING_SHARED_EXPORT TestTask: public QObject, public ITestable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Testing::Interfaces::ITestable)

        public:
            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // ITestable Implementation
            // --------------------------------
            int execTest(int argc = 0, char ** argv = 0);
            QString testName() const { return tr("Task"); }

        private slots:
            //! Tests related to the busy state of the task.
            void testBusyState();
        };
    }
}

#endif // TEST_TASK_H
