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

#ifndef BenchmarkTests_H
#define BenchmarkTests_H

#include "Testing_global.h"

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Contains some bencmarking code to benchmark parts of %Qtilities.
        class TESTING_SHARED_EXPORT BenchmarkTests: public QObject, public ITestable
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
             QString testName() const { return tr("Some expriments with benchmarking"); }
        private slots:
            void benchmarkObserverExport_1_0_1_0_data();
            //! Do a benchmark on a big observer export
            void benchmarkObserverExport_1_0_1_0();
            //! Do a benchmark on a big observer export
            void benchmarkObserverImport_1_0_1_0();
        };
    }
}

#endif // BenchmarkTests_H
