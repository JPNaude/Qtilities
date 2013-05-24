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

#ifndef TEST_OBSERVER_RELATIONAL_TABLE_H
#define TEST_OBSERVER_RELATIONAL_TABLE_H

#include "Testing_global.h"

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::ObserverRelationalTable.
        class TESTING_SHARED_EXPORT TestObserverRelationalTable: public QObject, public ITestable
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
