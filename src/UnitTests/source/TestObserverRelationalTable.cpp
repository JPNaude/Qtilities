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

#include "TestObserverRelationalTable.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

int Qtilities::UnitTests::TestObserverRelationalTable::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::UnitTests::TestObserverRelationalTable::testVisitorIDs() {
    QList<QObject*> children;
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    children << parentNode1;
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    children << parentNode2;
    children << parentNode1->addItem("Child 1");
    children << parentNode1->addItem("Child 2");
    children << parentNode2->addItem("Child 3");
    children << parentNode2->addItem("Child 4");
    children << parentNode2->addItem("Child 5");

    // The VISITOR_ID property should not exist on any objects:
    foreach (QObject* obj, children)
        QVERIFY(Observer::propertyExists(obj,qti_prop_VISITOR_ID) == false);

    // Construct relational table:
    ObserverRelationalTable* table = new ObserverRelationalTable(rootNode);

    // Now check that all children got the VISITOR_ID property:
    foreach (QObject* obj, children)
        QVERIFY(Observer::propertyExists(obj,qti_prop_VISITOR_ID) == true);

    // Now delete the table:
    delete table;

    // Now check that the VISITOR_ID property was removed from all objects:
    foreach (QObject* obj, children)
        QVERIFY(Observer::propertyExists(obj,qti_prop_VISITOR_ID) == false);
}

void Qtilities::UnitTests::TestObserverRelationalTable::testCompare() {
    //LOG_INFO("TestObserverRelationalTable::testCompare() start:");
    // Remember for the same observers use compare(), for different observer use diff()
    TreeNode* observerA = new TreeNode("Observer A");
    TreeNode* parentNode1 = observerA->addNode("Parent 1");
    TreeNode* parentNode2 = observerA->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");

    ObserverRelationalTable tableA(observerA);
    ObserverRelationalTable tableB(observerA);
    QVERIFY(tableA.compare(tableB) == true);

    TreeItem* shared_item = parentNode2->addItem("Child 5");

    // The tables should not have changed yet:
    QVERIFY(tableA.compare(tableB) == true);
    tableA.refresh();
    // Now A should be up to date and B not:
    QVERIFY(tableA.compare(tableB) == false);
    tableB.refresh();
    // Now both should be up to date:
    QVERIFY(tableA.compare(tableB) == true);

    // Check difference in object relationships
    parentNode1->addItem(shared_item);
    tableA.refresh();
    QVERIFY(tableA.compare(tableB) == false);
    tableB.refresh();
    QVERIFY(tableA.compare(tableB) == true);
    //LOG_INFO("TestObserverRelationalTable::testCompare() end:");
}

