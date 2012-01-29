/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
    QVERIFY(itr.current() == rootNode);
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QVERIFY(itr.first() == rootNode);
    QVERIFY(itr.last() == last);
    QVERIFY(itr.hasNext() == false);
    QVERIFY(itr.hasPrevious() == true);
    QVERIFY(testList.count() == 7);
    for (int i = 0; i < testList.count(); i++) {
        QVERIFY(testList.at(i).compare(QString::number(i+1)) == 0);
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
    QVERIFY(itr.current() == rootNode);
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        QObject* obj = itr.next();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QVERIFY(itr.first() == rootNode);
    QVERIFY(itr.last() == last);
    QVERIFY(itr.hasNext() == false);
    QVERIFY(itr.hasPrevious() == true);
    QVERIFY(testList.count() == 9);
    for (int i = 0; i < testList.count(); i++) {
        QVERIFY(testList.at(i).compare(QString::number(i+1)) == 0);
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
    QVERIFY(itr.current() == rootNode);
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

    QVERIFY(itr.first() == rootNode);
    QVERIFY(itr.last() == last);
    QVERIFY(itr.hasNext() == false);
    QVERIFY(itr.hasPrevious() == true);
    QVERIFY(testList.count() == 7);
    for (int i = 0; i < testList.count(); i++) {
        QVERIFY(testList.at(i).compare(QString::number(i+1)) == 0);
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
    QVERIFY(itr.current() == rootNodeA);
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

    QVERIFY(itr.first() == rootNodeA);
    QVERIFY(itr.last() == lastA);
    QVERIFY(itr.hasNext() == false);
    QVERIFY(itr.hasPrevious() == true);
    QVERIFY(testList.count() == 4);
    for (int i = 0; i < testList.count(); i++) {
        QVERIFY(testList.at(i).compare("A" + QString::number(i+1)) == 0);
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
    QVERIFY(itr.current() == rootNodeA);
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

    QVERIFY(itr.first() == rootNodeA);
    QVERIFY(itr.last() == last);
    QVERIFY(itr.hasNext() == false);
    QVERIFY(itr.hasPrevious() == true);
    QVERIFY(testList.count() == 20);
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
    QVERIFY(itr.last() == last);
    testList << itr.current()->objectName();
    while (itr.hasPrevious()) {
        QObject* obj = itr.previous();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QVERIFY(itr.first() == rootNodeA);
    QVERIFY(itr.hasNext() == true);
    QVERIFY(itr.hasPrevious() == false);
    QVERIFY(testList.count() == 20);
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
    QVERIFY(itr.last() == last);
    testList << itr.current()->objectName();
    while (itr.hasPrevious()) {
        QObject* obj = itr.previous();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QVERIFY(itr.first() == rootNode);
    QVERIFY(itr.hasNext() == true);
    QVERIFY(itr.hasPrevious() == false);
    QVERIFY(itr.last() == last);
    QVERIFY(testList.count() == 7);
    int compare_value = 0;
    for (int i = testList.count()-1; i >= 0; i--) {
        ++compare_value;
        QVERIFY(testList.at(i).compare(QString::number(compare_value)) == 0);
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
    QVERIFY(itr.last() == last);
    testList << itr.current()->objectName();
    while (itr.hasPrevious()) {
        QObject* obj = itr.previous();
        QVERIFY(obj);
        testList << obj->objectName();
    }

    QVERIFY(itr.first() == rootNode);
    QVERIFY(itr.hasNext() == true);
    QVERIFY(itr.hasPrevious() == false);
    QVERIFY(itr.last() == last);
    QVERIFY(testList.count() == 9);
    int compare_value = 0;
    for (int i = testList.count()-1; i >= 0; i--) {
        ++compare_value;
        QVERIFY(testList.at(i).compare(QString::number(compare_value)) == 0);
    }
}

