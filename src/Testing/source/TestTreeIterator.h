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

#ifndef TEST_TREE_ITERATOR_H
#define TEST_TREE_ITERATOR_H

#include "Testing_global.h"

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::TreeIterator.
        class TESTING_SHARED_EXPORT TestTreeIterator: public QObject, public ITestable
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
             QString testName() const { return tr("TreeIterator"); }

        private slots:
            //! Tests forward interation through a simple tree.
            void testIterationForwardSimple();
            //! Tests forward interation through a more complex tree.
            void testIterationForwardComplexA();
            //! Tests forward interation through another more complex tree.
            void testIterationForwardComplexB();
            //! Tests forward interation through a tree with items that appear in more than once tree.
            void testIterationForwardMultipleParentsA();
            //! Tests forward interation through a tree with items that appear in more than once tree.
            void testIterationForwardMultipleParentsB();
            //! Tests backwards interation through a tree with items that appear in more than once tree.
            void testIterationBackwardsMultipleParentsB();
            //! Tests backward interation through a simple tree.
            void testIterationBackwardSimple();
            //! Tests backward interation through a more complex tree.
            void testIterationBackwardComplexA();
            //! Tests forward interation through a tree with items that appear in more than once tree.
            void testIterationForwardMultipleParentsC();
        };
    }
}

#endif // TEST_TREE_ITERATOR_H
