/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include <QApplication>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <QtilitiesUnitTests>
using namespace QtilitiesUnitTests;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Qtilities Tester");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

    TestFrontend testFrontend(argc,argv);
    QMainWindow mainWindow;
    QtilitiesApplication::setMainWindow(&mainWindow);
    LOG_INITIALIZE();
    Log->setGlobalLogLevel(Logger::Trace);
    Log->setIsQtMessageHandler(false);
    Log->toggleQtMsgEngine(false);
    Log->toggleConsoleEngine(false);

    // ---------------------------------------------
    // Create all the tests that we want to use:
    // ---------------------------------------------
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

    // ---------------------------------------------
    // Create the testing frontend:
    // ---------------------------------------------
    testFrontend.show();

    // Run Some Benchmarks:
//    if (do_benchmarks) {
//        // Delete the log window before doing benchmarks, we don't want the log widget to influence the benchmarks.
//        log_widget->close();
//        log_widget->hide();
//        BenchmarkTests benchmarkTests;
//        QTest::qExec(&benchmarkTests,argc,argv);
//    }

    return a.exec();
}
