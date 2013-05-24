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
#include "TestAbstractTreeItem.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

int Qtilities::Testing::TestAbstractTreeItem::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestAbstractTreeItem::testCategoryOperations() {
    TreeItem tree_item;
    TreeNode tree_node;
    TreeNode tree_node2;

    // Set the category without being attached to any observer:
    QVERIFY(tree_item.setCategory(QtilitiesCategory("Test Category")) == false);
    QVERIFY(tree_node.attachSubject(&tree_item) == true);

    // Set the category with one parent:
    QVERIFY(tree_item.setCategory(QtilitiesCategory("Test Category")) == true);

    // Get the category for tree_node context:
    QVERIFY(tree_item.getCategoryString() == "Test Category");
    QVERIFY(tree_item.getCategoryString("::",tree_node.observerID()) == "Test Category");

    // Remove the category:
    tree_item.removeCategory(tree_node.observerID());

    // Attach to multiple observers: (already attached to tree_node)
    QVERIFY(tree_node2.attachSubject(&tree_item) == true);

    // Set the category with one parent:
    QVERIFY(tree_item.setCategory(QtilitiesCategory("Test Category")) == false);
    QVERIFY(tree_item.setCategory(QtilitiesCategory("Test Category 1"),tree_node.observerID()) == true);
    QVERIFY(tree_item.setCategory(QtilitiesCategory("Test Category 2"),tree_node2.observerID()) == true);
    QVERIFY(tree_item.getCategoryString("::",tree_node.observerID()) == "Test Category 1");
    QVERIFY(tree_item.getCategoryString("::",tree_node2.observerID()) == "Test Category 2");
}
