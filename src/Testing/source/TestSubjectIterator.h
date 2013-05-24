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

#ifndef TEST_SUBJECT_ITERATOR_H
#define TEST_SUBJECT_ITERATOR_H

#include "Testing_global.h"

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::SubjectIterator.
        class TESTING_SHARED_EXPORT TestSubjectIterator: public QObject, public ITestable
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
             QString testName() const { return tr("SubjectIterator"); }

        private slots:
            //! Tests interation by iterating through a simple tree.
            void testIterationSimpleFromStart();
            //! Tests interation by iterating through a simple tree.
            void testIterationSimpleFromMiddle();
            //! Tests interation by iterating through a more complex tree where items exists with multiple parents.
            void testIterationComplex();
            //! Tests interation for observer with not children.
            void testIterationObserverWithoutChildren();
            //! Test const iterator.
            void testIterationConst();
        };
    }
}

#endif // TEST_SUBJECT_ITERATOR_H
