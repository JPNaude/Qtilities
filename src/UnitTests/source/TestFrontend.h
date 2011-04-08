/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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


#ifndef TESTFRONTEND_H
#define TESTFRONTEND_H

#include <QWidget>

#include <Observer>
#include <QtilitiesCategory>

#include "ITestable.h"

#include "UnitTests_global.h"

namespace Ui {
    class TestFrontend;
}

namespace Qtilities {
    namespace UnitTests {
        using namespace Qtilities::Core;
        using namespace Interfaces;
        /*!
          \struct TestFrontendPrivateData
          \brief The TestFrontendPrivateData class stores private data used by the TestFrontend class.
         */
        struct TestFrontendPrivateData;

        /*!
        \class TestFrontend
        \brief The test front-end for unit tests implementing Qtilities::UnitTests::Interfaces::ITestable.

        The Qtilities::UnitTests::TestFrontend class allows you to run any tests implementing Qtilities::UnitTests::Interfaces::ITestable.

        To construct a testing front-end is easy, for example:
\code
TestFrontend testFrontend(argc,argv);

// ---------------------------------------------
// Create and register the tests that we want to use:
// ---------------------------------------------
#ifndef QTILITIES_NO_UNIT_TESTS
TestVersionNumber* testVersionNumber = new TestVersionNumber;
testFrontend.addTest(testVersionNumber,QtilitiesCategory("Qtilities::Core","::"));

TestSubjectIterator* testSubjectIterator = new TestSubjectIterator;
testFrontend.addTest(testSubjectIterator,QtilitiesCategory("Qtilities::Core","::"));

TestTreeIterator* testTreeIterator = new TestTreeIterator;
testFrontend.addTest(testTreeIterator,QtilitiesCategory("Qtilities::Core","::"));

TestObserver* testObserver = new TestObserver;
testFrontend.addTest(testObserver,QtilitiesCategory("Qtilities::Core","::"));

TestObserverRelationalTable* testObserverRelationalTable = new TestObserverRelationalTable;
testFrontend.addTest(testObserverRelationalTable,QtilitiesCategory("Qtilities::Core","::"));

TestExporting* testExporting = new TestExporting;
testFrontend.addTest(testExporting,QtilitiesCategory("Qtilities::General","::"));

BenchmarkTests* benchmarkTests = new BenchmarkTests;
testFrontend.addTest(benchmarkTests,QtilitiesCategory("Qtilities::Benchmarking","::"));
#endif

// ---------------------------------------------
// Show the testing frontend:
// ---------------------------------------------
testFrontend.show();
\endcode

        The resulting test front-end looks like this:

        \image html class_testfrontend_screenshot.jpg "Test Frontend With Some Tests"

        <i>This class was added in %Qtilities v0.3.</i>
          */
        class UNIT_TESTS_SHARED_EXPORT TestFrontend : public QWidget
        {
            Q_OBJECT

        public:
            explicit TestFrontend(int argc = 0, char ** argv = 0, QWidget *parent = 0);
            ~TestFrontend();

            //! Adds a test to the list of tests.
            void addTest(ITestable* test, QtilitiesCategory category = QtilitiesCategory());

        private slots:
            void on_btnExecute_clicked();
            void on_btnShowLog_clicked();


        private:
            Ui::TestFrontend *ui;
            TestFrontendPrivateData* d;
        };
    }
}

#endif // TESTFRONTEND_H
