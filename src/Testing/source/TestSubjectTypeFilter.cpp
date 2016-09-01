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

#include "TestSubjectTypeFilter.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <TreeIterator>

int Qtilities::Testing::TestSubjectTypeFilter::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestSubjectTypeFilter::testSubjectTypeFiltering() {
    TreeNode node("node");
    SubjectTypeFilter* subject_type_filter = node.setChildType("Test Type");
    QVERIFY(subject_type_filter->inverseFilteringEnabled());

    QCOMPARE(node.subjectCount(), 0);
    QObject* obj = new QObject;
    obj->setObjectName("Object1");
    node.attachSubject(obj);
    QCOMPARE(node.subjectCount(), 1);
    node.detachSubject(obj);
    QCOMPARE(node.subjectCount(), 0);

    TreeItem* tree_item = new TreeItem("TreeItem");
    node.attachSubject(tree_item);
    QCOMPARE(node.subjectCount(), 1);
    node.detachAll();
    QCOMPARE(node.subjectCount(), 0);

    subject_type_filter->addSubjectType(SubjectTypeInfo("Qtilities::CoreGui::TreeItem","Qtilities tree item"));
    node.attachSubject(tree_item);
    QCOMPARE(node.subjectCount(), 0);
    subject_type_filter->enableInverseFiltering(false);
    node.attachSubject(tree_item);
    QCOMPARE(node.subjectCount(), 1);
    node.detachAll();
    QCOMPARE(node.subjectCount(), 0);

    obj = new QObject;
    obj->setObjectName("Object2");
    node.attachSubject(obj);
    QCOMPARE(node.subjectCount(), 0);
    subject_type_filter->addSubjectType(SubjectTypeInfo("QObject","Normal QObject"));
    node.attachSubject(obj);
    QCOMPARE(node.subjectCount(), 1);
}
