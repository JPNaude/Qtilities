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
    qDebug() << itr.current()->objectName();
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
    qDebug() << itr.last()->objectName();
    QVERIFY(itr.last() == last);
    QVERIFY(itr.hasNext() == false);
    QVERIFY(itr.hasPrevious() == true);
    QVERIFY(testList.count() == 7);
    for (int i = 0; i < testList.count(); i++) {
        QVERIFY(testList.at(i).compare(QString::number(i+1)) == 0);
    }
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
//    testList << itr.previous()->objectName();
//    testList << itr.previous()->objectName();
//    testList << itr.previous()->objectName();
//    testList << itr.previous()->objectName();
//    testList << itr.previous()->objectName();
//    testList << itr.previous()->objectName();

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

