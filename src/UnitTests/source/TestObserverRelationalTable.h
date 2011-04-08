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

#ifndef TEST_OBSERVER_RELATIONAL_TABLE_H
#define TEST_OBSERVER_RELATIONAL_TABLE_H

#include "UnitTests_global.h"

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace UnitTests {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::TestObserverRelationalTable.
        class UNIT_TESTS_SHARED_EXPORT TestObserverRelationalTable: public QObject, public ITestable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::UnitTests::Interfaces::ITestable)

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
             QString testName() const { return tr("ObserverRelationalTable"); }

        private slots:
            // -----------------------------
            // Attachment/Detachment related tests
            // -----------------------------
            //! Tests stuff related to the VISITOR_ID property added to objects in the tree by the ObserverRelationalTable.
            void testVisitorIDs();
            //! Tests table comparison.
            void testCompare();
        };
    }
}

#endif // TEST_OBSERVER_RELATIONAL_TABLE_H
