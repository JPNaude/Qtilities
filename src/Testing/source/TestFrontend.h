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

#ifndef TESTFRONTEND_H
#define TESTFRONTEND_H

#include <QWidget>

#include <Observer>
#include <QtilitiesCategory>

#include "ITestable.h"

#include "Testing_global.h"

namespace Ui {
    class TestFrontend;
}
using namespace Qtilities::Core;
using namespace Qtilities::Testing::Interfaces;

namespace Qtilities {
    namespace Testing {
        /*!
          \struct TestFrontendPrivateData
          \brief The TestFrontendPrivateData class stores private data used by the TestFrontend class.
         */
        struct TestFrontendPrivateData;

        /*!
        \class TestFrontend
        \brief The test front-end for unit tests implementing Qtilities::Testing::Interfaces::ITestable.

        The Qtilities::Testing::TestFrontend class allows you to run any tests implementing Qtilities::Testing::Interfaces::ITestable.

        To construct a testing front-end is easy, for example:
\code
TestFrontend testFrontend(argc,argv);

// ---------------------------------------------
// Create and register the tests that we want to use:
// ---------------------------------------------
#ifdef QTILITIES_TESTING
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

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class TESTING_SHARED_EXPORT TestFrontend : public QWidget
        {
            Q_OBJECT

        public:
            explicit TestFrontend(int argc = 0, char ** argv = 0, QWidget *parent = 0);
            ~TestFrontend();

            //! Adds a test to the list of tests.
            void addTest(ITestable* test, QtilitiesCategory category = QtilitiesCategory());

            //! Execute the active tests.
            void execute();

            //! Returns the number of tests with errors from the last on_btnExecute_clicked() call.
            int numberOfFailedTests() const;

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
