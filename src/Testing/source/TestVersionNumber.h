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

#ifndef TEST_VERSION_NUMBER_H
#define TEST_VERSION_NUMBER_H

#include "Testing_global.h"
#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::VersionNumber.
        class TESTING_SHARED_EXPORT TestVersionNumber: public QObject, public ITestable
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
            QString testName() const { return tr("VersionNumber"); }

        private slots:
            //! Tests operator overload: ==
            void testOperatorEqual();
            //! Tests operator overload: !=
            void testOperatorNotEqual();
            //! Tests operator overload: =
            void testOperatorAssign();
            //! Tests operator overload: >
            void testOperatorBigger();
            //! Tests operator overload: >=
            void testOperatorBiggerEqual();
            //! Tests operator overload: <
            void testOperatorSmaller();
            //! Tests operator overload: <=
            void testOperatorSmallerEqual();
            //! Tests toString()
            void testToString();
        };
    }
}

#endif // TEST_VERSION_NUMBER_H
