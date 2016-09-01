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

#include "TestTreeIterator.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <TreeIterator>

int Qtilities::Testing::TestTreeIterator::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestTreeIterator::testIterationForwardSimple() {
    TreeNode* rootNode = new TreeNode("1");
    TreeNode* nodeA = rootNode->addNode("2");
    nodeA->addItem("3");
    nodeA->addItem("4");
    TreeNode* nodeB = rootNode->addNode("5");
    nodeB->addItem("6");
    TreeItem* last = nodeB->addItem("7");

    TreeIterator itr(rootNode);
    QStringList testList;
    QCOMPARE(itr.current(), rootNode);
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        QVERIFY(obj != 0);
        testList << obj->objectName();
    }

    QCOMPARE(itr.first(), rootNode);
    QCOMPARE(itr.last(), last);
    QVERIFY(!itr.hasNext());
    QVERIFY(itr.hasPrevious());
    QCOMPARE(testList.count(), 7);
    for (int i = 0; i < testList.count(); ++i) {
        QCOMPARE(testList.at(i), QString::number(i+1));
    }
}

void Qtilities::Testing::TestTreeIterator::testIterationForwardComplexA() {
    TreeNode* rootNode = new TreeNode("1");
    TreeNode* nodeA = rootNode->addNode("2");
    nodeA->addItem("3");
    nodeA->addItem("4");
    rootNode->addNode("5");
    rootNode->addNode("6");
    TreeNode* nodeD = rootNode->addNode("7");
    nodeD->addItem("8");
    TreeItem* last = nodeD->addItem("9");

    TreeIterator itr(rootNode);
    QStringList testList;
    QCOMPARE(itr.current(), rootNode);
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        QVERIFY(obj != 0);
        testList << obj->objectName();
    }

    QCOMPARE(itr.first(), rootNode);
    QCOMPARE(itr.last(), last);
    QVERIFY(!itr.hasNext());
    QVERIFY(itr.hasPrevious());
    QCOMPARE(testList.count(), 9);
    for (int i = 0; i < testList.count(); ++i) {
        QCOMPARE(testList.at(i), QString::number(i+1));
    }
}

void Qtilities::Testing::TestTreeIterator::testIterationForwardComplexB() {
    TreeNode* rootNode = new TreeNode("1");
    rootNode->addNode("2");
    rootNode->addNode("3");
    TreeNode* node4 = rootNode->addNode("4");
    TreeNode* node5 = node4->addNode("5");
    node5->addNode("6");
    TreeNode* last = rootNode->addNode("7");

    TreeIterator itr(rootNode);
    QStringList testList;
    QCOMPARE(itr.current(), rootNode);
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        QVERIFY(obj);
        testList << obj->objectName();
        if (itr.hasNext()) {
            itr.next();
            itr.previous();
        }
    }

    QCOMPARE(itr.first(), rootNode);
    QCOMPARE(itr.last(), last);
    QVERIFY(!itr.hasNext());
    QVERIFY(itr.hasPrevious());
    QCOMPARE(testList.count(), 7);
    for (int i = 0; i < testList.count(); ++i) {
        QCOMPARE(testList.at(i), QString::number(i+1));
    }
}

void Qtilities::Testing::TestTreeIterator::testIterationForwardMultipleParentsA() {
    TreeNode* rootTop = new TreeNode("Root Node");

    // This is the first tree (A):
    TreeNode* rootNodeA = new TreeNode("A1");
    rootNodeA->addItem("A2");
    TreeItem* shared_item = rootNodeA->addItem("A3"); // This is the shared item.
//    qDebug() << "Shared item ownership with one parent: " << (Observer::ObjectOwnership) ObjectManager::getSharedProperty(shared_item,qti_prop_OWNERSHIP).value().toInt();
//    qDebug() << "Shared item parent() with just one parent: " << shared_item->parent();
    QObject* lastA = rootNodeA->addItem("A4");

    // This is the second tree (B):
    TreeNode* rootNodeB = new TreeNode("B1");
    rootNodeB->addItem("B2");
    rootNodeB->attachSubject(shared_item); // Here we attach the shared item to another tree's node.
    // Because we use ManualOwnership above, shared_item's ownership will stay set to SpecificObserverOwnership where
    // nodeA5 is the parent() of the item. Therefore, this iterator will work: When it gets to shared_item and
    // sees there are two parents, it automatically takes the path of the parent() which will be nodeA5.
//    qDebug() << "Shared item ownership with second parent added: " << (Observer::ObjectOwnership) ObjectManager::getSharedProperty(shared_item,qti_prop_OWNERSHIP).value().toInt();
//    qDebug() << "Shared item parent() with just two parents: " << shared_item->parent();
    rootNodeB->addItem("B3");

    rootTop->attachSubject(rootNodeA);
    rootTop->attachSubject(rootNodeB);

    ObserverDotWriter writer(rootTop);
    writer.addNodeAttribute(shared_item,"color","red");
    writer.saveToFile(QtilitiesApplication::applicationSessionPath() + "/testIterationForwardMultipleParents.gv");

    // Now try to iterate through tree A:
    TreeIterator itr(rootNodeA);
    QStringList testList;
    QCOMPARE(itr.current(), rootNodeA);
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        QVERIFY(obj);
        testList << obj->objectName();
        if (itr.hasNext()) {
            obj = itr.next();
            obj = itr.previous();
        }
    }

    QCOMPARE(itr.first(), rootNodeA);
    QCOMPARE(itr.last(), lastA);
    QVERIFY(!itr.hasNext());
    QVERIFY(itr.hasPrevious());
    QCOMPARE(testList.count(), 4);
    for (int i = 0; i < testList.count(); ++i) {
        QCOMPARE(testList.at(i), QString("A" + QString::number(i+1)));
    }
}

void Testing::TestTreeIterator::testIterationForwardMultipleParentsB() {
    TreeNode* rootNodeA = new TreeNode("Root");
    TreeNode* parentNode1 = rootNodeA->addNode("Parent 1");
    TreeNode* parentNode2 = rootNodeA->addNode("Parent 2");
    TreeNode* parentNode3 = rootNodeA->addNode("Parent 3");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode1->addItem("Child 3");
    parentNode1->addItem("Child 4");
    parentNode1->addItem("Child 5");
    parentNode2->addItem("Child 6");
    parentNode2->addItem("Child 7");
    parentNode2->addItem("Child 8");
    parentNode2->addItem("Child 9");
    parentNode2->addItem("Child 10");
    parentNode3->addItem("Child 11");
    parentNode3->addItem("Child 12");
    parentNode3->addItem("Child 13");
    TreeItem* shared_item = parentNode3->addItem("Shared");
    TreeItem* last = parentNode3->addItem("Child 15");

    // Add the shared item as the LAST item under parentNode1:
    parentNode1->attachSubject(shared_item);

    // Now try to iterate through tree A:
    TreeIterator itr(rootNodeA);
    QStringList testList;
    QCOMPARE(itr.current(), rootNodeA);
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        QVERIFY(obj);
        //qDebug() << obj << itr.hasNext();
        testList << obj->objectName();
        if (itr.hasNext()) {
            obj = itr.next();
            obj = itr.previous();
        }
    }

    QCOMPARE(itr.first(), rootNodeA);
    QCOMPARE(itr.last(), last);
    QVERIFY(!itr.hasNext());
    QVERIFY(itr.hasPrevious());
    QCOMPARE(testList.count(), 20);
}

void Testing::TestTreeIterator::testIterationBackwardsMultipleParentsB() {
    TreeNode* rootNodeA = new TreeNode("Root");
    TreeNode* parentNode1 = rootNodeA->addNode("Parent 1");
    TreeNode* parentNode2 = rootNodeA->addNode("Parent 2");
    TreeNode* parentNode3 = rootNodeA->addNode("Parent 3");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode1->addItem("Child 3");
    parentNode1->addItem("Child 4");
    parentNode1->addItem("Child 5");
    parentNode2->addItem("Child 6");
    parentNode2->addItem("Child 7");
    parentNode2->addItem("Child 8");
    parentNode2->addItem("Child 9");
    parentNode2->addItem("Child 10");
    parentNode3->addItem("Child 11");
    parentNode3->addItem("Child 12");
    parentNode3->addItem("Child 13");
    TreeItem* shared_item = parentNode3->addItem("Shared");
    TreeItem* last = parentNode3->addItem("Child 15");

    // Add the shared item as the LAST item under parentNode1:
    parentNode1->attachSubject(shared_item);

    // Now try to iterate through tree A:
    TreeIterator itr(rootNodeA);
    QStringList testList;
    QCOMPARE(itr.last(), last);
    testList << itr.current()->objectName();
    while (itr.hasPrevious()) {
        QObject* obj = itr.previous();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QCOMPARE(itr.first(), rootNodeA);
    QVERIFY(itr.hasNext());
    QVERIFY(!itr.hasPrevious());
    QCOMPARE(testList.count(), 20);
}

void Qtilities::Testing::TestTreeIterator::testIterationBackwardSimple() {
    TreeNode* rootNode = new TreeNode("1");
    TreeNode* nodeA = rootNode->addNode("2");
    nodeA->addItem("3");
    nodeA->addItem("4");
    TreeNode* nodeB = rootNode->addNode("5");
    nodeB->addItem("6");
    TreeItem* last = nodeB->addItem("7");

    TreeIterator itr(rootNode);
    QStringList testList;
    QCOMPARE(itr.last(), last);
    testList << itr.current()->objectName();
    while (itr.hasPrevious()) {
        QObject* obj = itr.previous();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QCOMPARE(itr.first(), rootNode);
    QVERIFY(itr.hasNext());
    QVERIFY(!itr.hasPrevious());
    QCOMPARE(itr.last(), last);
    QCOMPARE(testList.count(), 7);
    int compare_value = 0;
    for (int i = testList.count()-1; i >= 0; i--) {
        ++compare_value;
        QCOMPARE(testList.at(i), QString::number(compare_value));
    }
}

void Qtilities::Testing::TestTreeIterator::testIterationBackwardComplexA() {
    TreeNode* rootNode = new TreeNode("1");
    TreeNode* nodeA = rootNode->addNode("2");
    nodeA->addItem("3");
    nodeA->addItem("4");
    rootNode->addNode("5");
    rootNode->addNode("6");
    TreeNode* nodeD = rootNode->addNode("7");
    nodeD->addItem("8");
    TreeItem* last = nodeD->addItem("9");

    TreeIterator itr(rootNode);
    QStringList testList;
    QCOMPARE(itr.last(), last);
    testList << itr.current()->objectName();
    while (itr.hasPrevious()) {
        QObject* obj = itr.previous();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QCOMPARE(itr.first(), rootNode);
    QVERIFY(itr.hasNext());
    QVERIFY(!itr.hasPrevious());
    QCOMPARE(itr.last(), last);
    QCOMPARE(testList.count(), 9);
    int compare_value = 0;
    for (int i = testList.count()-1; i >= 0; i--) {
        ++compare_value;
        QCOMPARE(testList.at(i), QString::number(compare_value));
    }
}

void Testing::TestTreeIterator::testIterationForwardMultipleParentsC() {
    TreeNode other_obs0("Other_0");
    TreeItem* item_other_1 = other_obs0.addItem("O_1");
    TreeItem* item_other_2 = other_obs0.addItem("O_2");
    TreeItem* item_other_3 = other_obs0.addItem("O_3");
    TreeItem* item_other_4 = other_obs0.addItem("O_4");

    TreeNode* rootNode = new TreeNode("Level0_0");
    TreeNode* nodeL1_0 = rootNode->addNode("Level1_0");
    nodeL1_0->attachSubject(item_other_1);
    TreeNode* nodeL1_1 = rootNode->addNode("Level1_1");
    TreeNode* nodeL1_2 = rootNode->addNode("Level1_2");
    TreeNode* nodeL2_0 = nodeL1_2->addNode("Level2_0");
    TreeNode* nodeL2_1 = nodeL1_2->addNode("Level2_1");
    nodeL2_1->attachSubject(item_other_2);
    nodeL2_1->attachSubject(item_other_3);
    nodeL2_1->attachSubject(item_other_4);
    TreeNode* nodeL1_3 = rootNode->addNode("Level1_3_Missing");

    Q_UNUSED(nodeL1_1);
    Q_UNUSED(nodeL2_0);

    TreeNode other_obs1("Other_1");
    other_obs1.attachSubject(nodeL1_0);
    other_obs1.attachSubject(nodeL1_1);
    other_obs1.attachSubject(nodeL1_2);
    other_obs1.attachSubject(nodeL2_0);
    other_obs1.attachSubject(nodeL2_1);
    other_obs1.attachSubject(nodeL1_3);

    TreeIterator itr(rootNode);
    QStringList testList;
    QCOMPARE(itr.last(), nodeL1_3);
    testList << itr.current()->objectName();
    while (itr.hasPrevious()) {
        QObject* obj = itr.previous();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QCOMPARE(itr.first(), rootNode);
    QVERIFY(itr.hasNext());
    QVERIFY(!itr.hasPrevious());
    QCOMPARE(itr.last(), nodeL1_3);
    QCOMPARE(testList.count(), 11);
//    int compare_value = 0;
//    for (int i = testList.count()-1; i >= 0; i--) {
//        ++compare_value;
//        QVERIFY(testList.at(i).compare(QString::number(compare_value)) == 0);
//    }
}

