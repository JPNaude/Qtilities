/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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
            //! Tests recursive attachment where the tree was constructed using the containment approach.
            void testRecursiveAttachmentContained();
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
            //! A test which tests treeCount() function where the tree was constructed using the containment approach.
            void testTreeCountContainment();
            //! A test which tests treeAt() function where the tree was constructed using the containment approach.
            void testTreeAtContainment();
            //! A test which tests treeAt() function where the tree was constructed using the containment approach.
            void testTreeContainsContainment();
            //! A test which tests treeChildren() function where the tree was constructed using the containment approach.
            void testTreeChildrenContainment();

            // -----------------------------
            // Modification state tests.
            // -----------------------------
            //! A test which counts the number of modification state changed signal emissions for a specific test case..
            void testCountModificationStateChanges();
        };
    }
}

#endif // TEST_OBSERVER_H
