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

#include "TestObserver.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

int Qtilities::Testing::TestObserver::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestObserver::testRecursiveAttachment() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* childNode = rootNode->addNode("Parent 1");
    QVERIFY(!childNode->attachSubject(rootNode));
}

void Qtilities::Testing::TestObserver::testAttachWithObserverLimit() {
    QObject* obj = new QObject();
    SharedProperty observer_limit_property(qti_prop_OBSERVER_LIMIT,QVariant(1));
    ObjectManager::setSharedProperty(obj, observer_limit_property);

    Observer observer1;
    Observer observer2;
    QVERIFY(observer1.attachSubject(obj));
    QVERIFY(!observer2.attachSubject(obj));
}

void Qtilities::Testing::TestObserver::testSubjectLimit() {
    TreeNode node;
    node.setSubjectLimit(2);

    QVERIFY(node.addItem("Item 1") != 0);
    QVERIFY(node.addItem("Item 2") != 0);
    QVERIFY(node.addItem("Item 3") == 0);
    QVERIFY(!node.setSubjectLimit(1));
    QVERIFY(node.setSubjectLimit(3));
    QVERIFY(node.addItem("Item 3") != 0);
}

void Qtilities::Testing::TestObserver::testOwnershipManual() {
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

void Qtilities::Testing::TestObserver::testOwnershipAuto() {
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
    QApplication::processEvents();

    // Check the validity of the objects:
    QVERIFY(object1 == 0);
    QVERIFY(object2 != 0);

    delete object2;

    LOG_INFO("TestObserver::testOwnershipManual() end.");
}

void Qtilities::Testing::TestObserver::testOwnershipSpecificObserver() {
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
    QApplication::processEvents();

    // Check the validity of the objects:
    QVERIFY(object1 == 0);
    QVERIFY(object2 == 0);

    LOG_INFO("TestObserver::testOwnershipSpecificObserver() end.");
}

void Qtilities::Testing::TestObserver::testOwnershipObserverScope() {
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
    QApplication::processEvents();

    // Check the validity of the objects:
    QVERIFY(object1 != 0);
    QVERIFY(object2 != 0);

    // Now delete observer B
    delete observerB;
    QApplication::processEvents();

    // Check the validity of the objects:
    QVERIFY(object1 == 0);
    QVERIFY(object2 == 0);

    LOG_INFO("TestObserver::testOwnershipObserverScope() end.");
}

void Qtilities::Testing::TestObserver::testOwnershipOwnedByParent() {
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
    QApplication::processEvents();

    // Check the validity of the observer:
    QEXPECT_FAIL("", "Event loop is not running, thus deleteLater() will not be called to delete the object yet.", Continue);
    QVERIFY(observerA == 0);

    LOG_INFO("TestObserver::testOwnershipOwnedByParent() end.");
}

void Qtilities::Testing::TestObserver::testTreeCount() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    parentNode2->addItem("Child 5");

    QCOMPARE(rootNode->treeCount(), 7);
}

void Qtilities::Testing::TestObserver::testTreeAt() {
    // Example tree using tree node classes to simplify test:
    TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    TreeItem* item = parentNode2->addItem("Child 5");

    QCOMPARE(rootNode->treeAt(6), item);
}

void Qtilities::Testing::TestObserver::testTreeContains() {
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
        QVERIFY(rootNode->treeContains(obj));
}

void Qtilities::Testing::TestObserver::testTreeChildren() {
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
        QVERIFY(children_verify.contains(obj));

    QList<QObject*> nodes_verify = rootNode->treeChildren("Qtilities::CoreGui::TreeNode");
    QCOMPARE(nodes_verify.count(), 2);
    QList<QObject*> items_verify = rootNode->treeChildren("Qtilities::CoreGui::TreeItem");
    QCOMPARE(items_verify.count(), 5);
}

//void Qtilities::Testing::TestObserver::testCountModificationStateChanges() {
//    TreeNode node("testCountModificationStateChangesNode");
//    QSignalSpy spy(&node, SIGNAL(modificationStateChanged(bool)));

//    node.addItem("1");
//    node.setModificationState(false);
//    node.addItem("2");
//    node.addItem("3");
//    //LOG_INFO("testCountModificationStateChanges: Signal Spy: modificationStateChanged(bool) -> count: " + QString::number(spy.count()));
//    QCOMPARE(spy.count(), 3);

//    spy.clear();

//    node.startProcessingCycle();
//    node.addItem("4");
//    node.addItem("5");
//    node.addItem("6");
//    TreeNode* nodeA = node.addNode("A");
//    nodeA->addItem("7");
//    TreeNode* nodeB = node.addNode("B");
//    nodeB->addItem("8");

//    node.endProcessingCycle();
//    //LOG_INFO("testCountModificationStateChanges: Signal Spy: modificationStateChanged(bool) -> count: " + QString::number(spy.count()));
//    QCOMPARE(spy.count(), 1);

//    node.saveToFile("testCountModificationStateChanges.xml");
//    node.deleteAll();
//    spy.clear();
//    node.loadFromFile("testCountModificationStateChanges.xml");
//    //LOG_INFO("testCountModificationStateChanges: Signal Spy: modificationStateChanged(bool) -> count: " + QString::number(spy.count()));
//    QCOMPARE(spy.count(), 1);
//    spy.clear();

//    node.deleteAll();
//    spy.clear();
//    //node.startProcessingCycle();
//    node.loadFromFile("testCountModificationStateChanges.xml");
//    //node.endProcessingCycle();
//    //LOG_INFO("testCountModificationStateChanges: Signal Spy: modificationStateChanged(bool) -> count: " + QString::number(spy.count()));
//    QCOMPARE(spy.count(), 1);
//    spy.clear();
//}
