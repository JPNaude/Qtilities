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

#include "TestObserverRelationalTable.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

int Qtilities::Testing::TestObserverRelationalTable::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestObserverRelationalTable::testVisitorIDs() {
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
        QVERIFY(!ObjectManager::propertyExists(obj,qti_prop_VISITOR_ID));

    // Construct relational table:
    ObserverRelationalTable* table = new ObserverRelationalTable(rootNode);

    // Now check that all children got the VISITOR_ID property:
    foreach (QObject* obj, children)
        QVERIFY(ObjectManager::propertyExists(obj,qti_prop_VISITOR_ID));

    // Now delete the table:
    delete table;

    // Now check that the VISITOR_ID property was removed from all objects:
    foreach (QObject* obj, children)
        QVERIFY(!ObjectManager::propertyExists(obj,qti_prop_VISITOR_ID));
}

void Qtilities::Testing::TestObserverRelationalTable::testCompare() {
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
    QVERIFY(tableA.compare(tableB));

    TreeItem* shared_item = parentNode2->addItem("Child 5");

    // The tables should not have changed yet:
    QVERIFY(tableA.compare(tableB));
    tableA.refresh();
    // Now A should be up to date and B not:
    QVERIFY(!tableA.compare(tableB));
    tableB.refresh();
    // Now both should be up to date:
    QVERIFY(tableA.compare(tableB));

    // Check difference in object relationships
    parentNode1->addItem(shared_item);
    tableA.refresh();
    QVERIFY(!tableA.compare(tableB));
    tableB.refresh();
    QVERIFY(tableA.compare(tableB));
    //LOG_INFO("TestObserverRelationalTable::testCompare() end:");
}

