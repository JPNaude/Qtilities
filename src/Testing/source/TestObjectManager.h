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

#ifndef TEST_OBJECT_MANAGER_H
#define TEST_OBJECT_MANAGER_H

#include "Testing_global.h"
#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::Core::ObjectManager.
        class TESTING_SHARED_EXPORT TestObjectManager: public QObject, public ITestable
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
            QString testName() const { return tr("ObjectManager"); }

        private slots:
            //! Tests cloning and removing of dynamic properties.
            void testCloneAndRemoveDynamicProperties();
            //! Tests comparing of dynamic properties.
            void testCompareDynamicProperties();
            //! Tests comparing of dynamic properties diff calculation.
            void testCompareDynamicPropertiesDiff();
            //! Tests moving of subjects between observers using ObjectManager::moveSubjects().
            void testMoveSubjects();
        };
    }
}

#endif // TEST_OBJECT_MANAGER_H
