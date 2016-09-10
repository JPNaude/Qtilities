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

#ifndef TEST_OBSERVER_H
#define TEST_OBSERVER_H

#include "Testing_global.h"
#include "ITestable.h"
#include "FunctionCallAnalyzer.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::Observer.
        class TESTING_SHARED_EXPORT TestObserver: public QObject, public ITestable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Testing::Interfaces::ITestable)

        public:
            explicit TestObserver() {}
            ~TestObserver() {}

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // ITestable Implementation
            // --------------------------------
            int execTest(int argc = 0, char ** argv = 0);
            QString testName() const { return tr("Observer"); }

        private slots:
            // -----------------------------
            // Attachment/Detachment related tests
            // -----------------------------
            //! Tests recursive attachment.
            void testRecursiveAttachment();
            //! Tests attachment where the subject has the qti_prop_OBSERVER_LIMIT property set.
            void testAttachWithObserverLimit();
            //! Tests the subject limit functionality Observer.
            void testSubjectLimit();

            // -----------------------------
            // Ownership related tests
            // -----------------------------
            //! A test which tests the different Observer::ManualOwnership ownership.
            void testOwnershipManual();
            //! A test which tests the different Observer::AutoOwnership ownership.
            void testOwnershipAuto();
            //! A test which tests the different Observer::SpecificObserverOwnership ownership.
            void testOwnershipSpecificObserver();
            //! A test which tests the different Observer::ObserverScopeOwnership ownership.
            void testOwnershipObserverScope();
            //! A test which tests the different Observer::OwnedBySubjectOwnership ownership.
            void testOwnershipOwnedByParent();

            // -----------------------------
            // Tests for access function for objects in complete tree under an observer.
            // -----------------------------
            //! A test which tests treeCount() function.
            void testTreeCount();
            //! A test which tests treeAt() function.
            void testTreeAt();
            //! A test which tests treeAt() function.
            void testTreeContains();
            //! A test which tests treeChildren() function.
            void testTreeChildren();

            // -----------------------------
            // Modification state tests.
            // -----------------------------
            //! A test which counts the number of modification state changed signal emissions for a specific test case.
            //void testCountModificationStateChanges();
        };
    }
}

#endif // TEST_OBSERVER_H
