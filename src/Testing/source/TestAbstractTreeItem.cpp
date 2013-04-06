/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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
