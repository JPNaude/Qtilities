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

#include "TestSubjectIterator.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <SubjectIterator>

#include <QString>

int Qtilities::Testing::TestSubjectIterator::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestSubjectIterator::testIterationSimpleFromStart() {
    TreeNode node;
    TreeItem* item1 = node.addItem("1");
    node.addItem("2");
    node.addItem("3");
    node.addItem("4");

    SubjectIterator<Qtilities::CoreGui::TreeItem> itr(item1);

    QStringList testList;
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        testList << itr.next()->objectName();
    }

    QVERIFY(testList.count() == 4);
    QVERIFY(testList.at(0).compare("1") == 0);
    QVERIFY(testList.at(1).compare("2") == 0);
    QVERIFY(testList.at(2).compare("3") == 0);
    QVERIFY(testList.at(3).compare("4") == 0);
}

void Qtilities::Testing::TestSubjectIterator::testIterationSimpleFromMiddle() {
    TreeNode node;
    node.addItem("1");
    TreeItem* item2 = node.addItem("2");
    node.addItem("3");

    SubjectIterator<QObject> itr(item2);

    QStringList testList;
    testList << itr.current()->objectName();
    while (itr.hasNext()) {
        testList << itr.next()->objectName();
    }

    QVERIFY(testList.count() == 2);
    QVERIFY(testList.at(0).compare("2") == 0);
    QVERIFY(testList.at(1).compare("3") == 0);
}

void Qtilities::Testing::TestSubjectIterator::testIterationComplex() {
    TreeNode node;
    TreeNode* nodeA = node.addNode("A");
    TreeNode* nodeB = node.addNode("B");
    nodeA->addItem("1");
    TreeItem* shared_item = nodeA->addItem("2");
    nodeA->addItem("3");
    nodeB->addItem("4");
    nodeB->addItem("5");
    nodeB->attachSubject(shared_item);
    nodeB->addItem("6");

    // If we want to iterate through the subjects in nodeA:
    SubjectIterator<QObject> itrA(nodeA,SubjectIterator<QObject>::IterateChildren);
    QVERIFY(itrA.current() != 0);

    // In this case item1 will be skipped:
    QStringList testListA;
    testListA << itrA.current()->objectName();
    while (itrA.hasNext()) {
        testListA << itrA.next()->objectName();
    }

    QVERIFY(testListA.count() == 3);
    QVERIFY(testListA.at(0).compare("1") == 0);
    QVERIFY(testListA.at(1).compare("2") == 0);
    QVERIFY(testListA.at(2).compare("3") == 0);

    // If we want to iterate through the subjects in nodeB:
    SubjectIterator<QObject> itrB(nodeB,SubjectIterator<QObject>::IterateChildren);

    // In this case item1 will be skipped:
    QStringList testListB;
    testListB << itrB.current()->objectName();
    while (itrB.hasNext()) {
        testListB << itrB.next()->objectName();
    }

    QVERIFY(testListB.count() == 4);
    QVERIFY(testListB.at(0).compare("4") == 0);
    QVERIFY(testListB.at(1).compare("5") == 0);
    QVERIFY(testListB.at(2).compare("2") == 0);
    QVERIFY(testListB.at(3).compare("6") == 0);

    // If we want to iterate through the subjects in nodeB:
    SubjectIterator<QObject> itrC(shared_item,nodeB);

    // In this case item1 will be skipped:
    QStringList testListC;
    testListC << itrC.current()->objectName();
    while (itrC.hasNext()) {
        testListC << itrC.next()->objectName();
    }

    QVERIFY(testListC.count() == 2);
    QVERIFY(testListC.at(0).compare("2") == 0);
    QVERIFY(testListC.at(1).compare("6") == 0);

    // If we want to iterate through the subjects in nodeB:
    SubjectIterator<QObject> itrD(shared_item,nodeA);

    // In this case item1 will be skipped:
    QStringList testListD;
    testListD << itrD.current()->objectName();
    while (itrD.hasNext()) {
        testListD << itrD.next()->objectName();
    }

    QVERIFY(testListD.count() == 2);
    QVERIFY(testListD.at(0).compare("2") == 0);
    QVERIFY(testListD.at(1).compare("3") == 0);
}

void Qtilities::Testing::TestSubjectIterator::testIterationObserverWithoutChildren() {
    TreeNode* node = new TreeNode;
    SubjectIterator<QObject> itr(node,SubjectIterator<QObject>::IterateChildren);
    QVERIFY(itr.hasNext() == false);
    QVERIFY(itr.hasPrevious() == false);
    QVERIFY(itr.first() == 0);
    QVERIFY(itr.last() == 0);
    QVERIFY(itr.current() == 0);
}

void Qtilities::Testing::TestSubjectIterator::testIterationConst() {
    TreeNode* node = new TreeNode;
    node->addItem("1");
    node->addItem("2");
    node->addItem("3");
    node->addItem("4");

    SubjectIterator<QObject> itr(node);
    itr.current()->setObjectName("Test");
    // The following should not compile:
    //ConstSubjectIterator<QObject> constItr(node);
    //constItr.current()->setObjectName("Test");
}
