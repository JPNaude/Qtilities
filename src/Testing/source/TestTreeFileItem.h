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

#ifndef TEST_TREE_FILE_ITEM_H
#define TEST_TREE_FILE_ITEM_H

#include "Testing_global.h"
#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::CoreGui::TreeFileItem.
        class TESTING_SHARED_EXPORT TestTreeFileItem: public QObject, public ITestable
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
            QString testName() const { return tr("TreeFileItem"); }

        private slots:
            //! Tests operator overload: ==
            void testMe();
        };
    }
}

#endif // TEST_TREE_FILE_ITEM_H
