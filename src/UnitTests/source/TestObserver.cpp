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

#include "TestObserver.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

void Qtilities::UnitTests::TestObserver::testRecursiveAttachment() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* childNode = rootNode->addNode("Parent 1");
    QVERIFY(childNode->attachSubject(rootNode) == false);
}

void Qtilities::UnitTests::TestObserver::testRecursiveAttachmentContained() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* childNode = rootNode->addNode("Parent Node");
    TreeItem* item = childNode->addItem("Child Item");

    // Now create and add a contained observer:
    TreeNode* containedNode = new TreeNode("Contained Node");
    containedNode->setParent(item);
    QVERIFY(containedNode->attachSubject(rootNode) == false);
}

void Qtilities::UnitTests::TestObserver::testAttachWithObserverLimit() {
    QObject* obj = new QObject();
    SharedProperty observer_limit_property(qti_prop_OBSERVER_LIMIT,QVariant(1));
    Observer::setSharedProperty(obj, observer_limit_property);

    Observer observer1;
    Observer observer2;
    QVERIFY(observer1.attachSubject(obj) == true);
    QVERIFY(observer2.attachSubject(obj) == false);
}

void Qtilities::UnitTests::TestObserver::testSubjectLimit() {
    TreeNode node;
    node.setSubjectLimit(2);

    QVERIFY(node.addItem("Item 1") != 0);
    QVERIFY(node.addItem("Item 2") != 0);
    QVERIFY(node.addItem("Item 3") == 0);
    QVERIFY(node.setSubjectLimit(1) == false);
    QVERIFY(node.setSubjectLimit(3) == true);
    QVERIFY(node.addItem("Item 3") != 0);
}

void Qtilities::UnitTests::TestObserver::testOwnershipManual() {
    LOG_INFO("TestObserver::testOwnershipManual() start:");

    // Create the observer
    Observer* observerA = new Observer("Observer A");

    // Create the objects
    QPointer<QObject> object1 = new QObject();
    object1->setObjectName("Object 1");
    QPointer<QObject> object2 = new QObject();
    object2->setObjectName("Object 2");

    // Attach objects to observers
    observerA->attachSubject(object1,Observer::ManualOwnership);
    observerA->attachSubject(object2,Observer::ManualOwnership);

    // Now delete observers
    delete observerA;

    // Check the validity of the objects:
    QVERIFY(object1 != 0);
    QVERIFY(object2 != 0);

    delete object1;
    delete object2;

    LOG_INFO("TestObserver::testOwnershipManual() end.");
 }

void Qtilities::UnitTests::TestObserver::testOwnershipAuto() {
    LOG_INFO("TestObserver::testOwnershipManual() start:");

    // Create the observer
    Observer* observerA = new Observer("Observer A");

    // Create the objects
    QPointer<QObject> object1 = new QObject();
    object1->setObjectName("Object 1");
    QPointer<QObject> object2 = new QObject();
    object2->setObjectName("Object 2");
    QPointer<QObject> parentObject = new QObject();
    object2->setParent(parentObject);

    // Attach objects to observers
    observerA->attachSubject(object1,Observer::AutoOwnership);
    observerA->attachSubject(object2,Observer::AutoOwnership);

    // Now delete observers
    delete observerA;

    // Check the validity of the objects:
    QVERIFY(object1 == 0);
    QVERIFY(object2 != 0);

    delete object2;

    LOG_INFO("TestObserver::testOwnershipManual() end.");
}

void Qtilities::UnitTests::TestObserver::testOwnershipSpecificObserver() {
    LOG_INFO("TestObserver::testOwnershipSpecificObserver() start:");

    // Create the observer
    Observer* observerA = new Observer("Observer A");

    // Create the objects
    QPointer<QObject> object1 = new QObject();
    object1->setObjectName("Object 1");
    QPointer<QObject> object2 = new QObject();
    object2->setObjectName("Object 2");

    // Attach objects to observers
    observerA->attachSubject(object1,Observer::SpecificObserverOwnership);
    observerA->attachSubject(object2,Observer::SpecificObserverOwnership);

    // Now delete observer
    delete observerA;

    // Check the validity of the objects:
    QVERIFY(object1 == 0);
    QVERIFY(object2 == 0);

    LOG_INFO("TestObserver::testOwnershipSpecificObserver() end.");
}

void Qtilities::UnitTests::TestObserver::testOwnershipObserverScope() {
    LOG_INFO("TestObserver::testOwnershipObserverScope() start:");

    // Create the observers
    Observer* observerA = new Observer("Observer A");
    Observer* observerB = new Observer("Observer B");

    // Create the objects
    QPointer<QObject> object1 = new QObject();
    object1->setObjectName("Object 1");
    QPointer<QObject> object2 = new QObject();
    object2->setObjectName("Object 2");

    // Attach objects to observers
    observerA->attachSubject(object1,Observer::ObserverScopeOwnership);
    observerA->attachSubject(object2,Observer::ObserverScopeOwnership);
    observerB->attachSubject(object1,Observer::ObserverScopeOwnership);
    observerB->attachSubject(object2,Observer::ObserverScopeOwnership);

    // Now delete observer A
    delete observerA;

    // Check the validity of the objects:
    QVERIFY(object1 != 0);
    QVERIFY(object2 != 0);

    // Now delete observer B
    delete observerB;

    // Check the validity of the objects:
    QVERIFY(object1 == 0);
    QVERIFY(object2 == 0);

    LOG_INFO("TestObserver::testOwnershipObserverScope() end.");
}

void Qtilities::UnitTests::TestObserver::testOwnershipOwnedByParent() {
    LOG_INFO("TestObserver::testOwnershipOwnedByParent() start:");

    // Create the observer
    QPointer<Observer> observerA = new Observer("Observer A");

    // Create the objects
    QPointer<QObject> object1 = new QObject();
    object1->setObjectName("Object 1");

    // Attach objects to observers
    observerA->attachSubject(object1,Observer::OwnedBySubjectOwnership);

    // Now delete the object
    delete object1;

    // Check the validity of the observer:
    QEXPECT_FAIL("", "Event loop is not running, thus deleteLater() will not be called to delete the object yet.", Continue);
    QVERIFY(observerA == 0);

    LOG_INFO("TestObserver::testOwnershipOwnedByParent() end.");
}

void Qtilities::UnitTests::TestObserver::testTreeCount() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    parentNode2->addItem("Child 5");

    QVERIFY(rootNode->treeCount() == 7);
}

void Qtilities::UnitTests::TestObserver::testTreeAt() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    TreeItem* item = parentNode2->addItem("Child 5");

    QVERIFY(rootNode->treeAt(6) == item);
}

void Qtilities::UnitTests::TestObserver::testTreeContains() {
    // Example tree using tree node classes to simplify test:
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

    foreach (QObject* obj, children)
        QVERIFY(rootNode->treeContains(obj) == true);
}

void Qtilities::UnitTests::TestObserver::testTreeChildren() {
    // Example tree using tree node classes to simplify test:
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

    QList<QObject*> children_verify = rootNode->treeChildren();
    foreach (QObject* obj, children)
        QVERIFY(children_verify.contains(obj) == true);
}

void Qtilities::UnitTests::TestObserver::testTreeCountContainment() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    TreeItem* item = parentNode2->addItem("Child 5");

    // Now create and add a contained observer:
    TreeNode* containedNode = new TreeNode("Contained Node");
    containedNode->addItem("Contained Item 1");
    containedNode->addItem("Contained Item 2");
    containedNode->addItem("Contained Item 3");
    containedNode->setParent(item);

    LOG_INFO(QString::number(rootNode->treeCount()));
    QVERIFY(rootNode->treeCount() == 11);
}

void Qtilities::UnitTests::TestObserver::testTreeAtContainment() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    TreeItem* item = parentNode2->addItem("Child 5");

    // Now create and add a contained observer:
    TreeNode* containedNode = new TreeNode("Contained Node");
    containedNode->addItem("Contained Item 1");
    containedNode->addItem("Contained Item 2");
    TreeItem* test_item = containedNode->addItem("Contained Item 3");
    containedNode->setParent(item);

    LOG_INFO(QString::number(rootNode->treeCount()));
    QVERIFY(rootNode->treeAt(10) == test_item);
}

void Qtilities::UnitTests::TestObserver::testTreeContainsContainment() {
    // Example tree using tree node classes to simplify test:
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
    TreeItem* item = parentNode2->addItem("Child 5");
    children << item;

    // Now create and add a contained observer:
    TreeNode* containedNode = new TreeNode("Contained Node");
    children << containedNode;
    children << containedNode->addItem("Contained Item 1");
    children << containedNode->addItem("Contained Item 2");
    children << containedNode->addItem("Contained Item 3");

    containedNode->setParent(item);

    foreach (QObject* obj, children)
        QVERIFY(rootNode->treeContains(obj) == true);
}

void Qtilities::UnitTests::TestObserver::testTreeChildrenContainment() {
    // Example tree using tree node classes to simplify test:
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
    TreeItem* item = parentNode2->addItem("Child 5");
    children << item;

    // Now create and add a contained observer:
    TreeNode* containedNode = new TreeNode("Contained Node");
    children << containedNode;
    children << containedNode->addItem("Contained Item 1");
    children << containedNode->addItem("Contained Item 2");
    children << containedNode->addItem("Contained Item 3");

    containedNode->setParent(item);

    QList<QObject*> children_verify = rootNode->treeChildren();
    foreach (QObject* obj, children)
        QVERIFY(children_verify.contains(obj) == true);
}
