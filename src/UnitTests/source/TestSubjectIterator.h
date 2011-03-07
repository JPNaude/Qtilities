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

#ifndef TEST_SUBJECT_ITERATOR_H
#define TEST_SUBJECT_ITERATOR_H

#include "UnitTests_global.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace UnitTests {
        //! Allows testing of Qtilities::Core::SubjectIterator.
        class UNIT_TESTS_SHARED_EXPORT TestSubjectIterator: public QObject
        {
            Q_OBJECT
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
