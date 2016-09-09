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

#include "TestObjectManager.h"

#include <QtilitiesCore>
using namespace QtilitiesCore;

int Qtilities::Testing::TestObjectManager::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestObjectManager::testCloneAndRemoveDynamicProperties() {
    QObject* source_obj = new QObject;
    source_obj->setObjectName("Source Object");
    QObject* target_obj = new QObject;
    target_obj->setObjectName("Target Object");

    // Add some properties:
    // 1. Test a shared property:
    SharedProperty test_shared_property("Shared Property",QVariant(5));
    ObjectManager::setSharedProperty(source_obj,test_shared_property);
    QVERIFY(ObjectManager::propertyExists(source_obj,"Shared Property"));
    // 2. Test a multi context property:
    MultiContextProperty test_multi_context_property("Multi Context Property");
    test_multi_context_property.setValue(QVariant(1),1);
    test_multi_context_property.setValue(QVariant(2),2);
    ObjectManager::setMultiContextProperty(source_obj,test_multi_context_property);
    QVERIFY(ObjectManager::propertyExists(source_obj,"Multi Context Property"));
    // 3. Test a normal QVariant property
    QVariant test_normal_variant(10);
    source_obj->setProperty("Normal Property",test_normal_variant);
    QVERIFY(ObjectManager::propertyExists(source_obj,"Normal Property"));

    // Test before cloning:
    // 1. Get the shared property:
    SharedProperty check_shared_property = ObjectManager::getSharedProperty(source_obj,"Shared Property");
    QCOMPARE(test_shared_property, check_shared_property);
    // 2. Get the multi context property:
    MultiContextProperty check_multi_context_property = ObjectManager::getMultiContextProperty(source_obj,"Multi Context Property");
    QCOMPARE(test_multi_context_property, check_multi_context_property);
    // 3. Get the normal QVariant property
    QVariant check_normal_variant = source_obj->property("Normal Property");
    QCOMPARE(test_normal_variant, check_normal_variant);

    // Do the clone:
    ObjectManager::cloneObjectProperties(source_obj,target_obj);

    // Get the properties again and compare them:
    // 1. Get the shared property:
    QVERIFY(ObjectManager::propertyExists(target_obj,"Shared Property"));
    SharedProperty return_shared_property = ObjectManager::getSharedProperty(target_obj,"Shared Property");
    QCOMPARE(test_shared_property, return_shared_property);
    // 2. Get the multi context property:
    QVERIFY(ObjectManager::propertyExists(target_obj,"Multi Context Property"));
    MultiContextProperty return_multi_context_property = ObjectManager::getMultiContextProperty(target_obj,"Multi Context Property");
    QCOMPARE(test_multi_context_property, return_multi_context_property);
    // 3. Get the normal QVariant property
    QVERIFY(ObjectManager::propertyExists(target_obj,"Normal Property"));
    QVariant return_normal_variant = target_obj->property("Normal Property");
    QCOMPARE(test_normal_variant, return_normal_variant);

    // Test removing:
    ObjectManager::removeDynamicProperties(target_obj);
    QVERIFY(!ObjectManager::propertyExists(target_obj,"Shared Property"));
    QVERIFY(!ObjectManager::propertyExists(target_obj,"Multi Context Property"));
    QVERIFY(!ObjectManager::propertyExists(target_obj,"Normal Property"));
}

void Qtilities::Testing::TestObjectManager::testCompareDynamicProperties() {
    QObject* obj1 = new QObject;
    obj1->setObjectName("Object1");
    QObject* obj2 = new QObject;
    obj2->setObjectName("Object2");

    QVERIFY(ObjectManager::compareDynamicProperties(obj1,obj2));

    // Add some properties:
    // 1. Test a shared property:
    SharedProperty test_shared_property1("Shared Property",QVariant(5));
    ObjectManager::setSharedProperty(obj1,test_shared_property1);
    QVERIFY(ObjectManager::propertyExists(obj1,"Shared Property"));
    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2));

    SharedProperty test_shared_property2("Shared Property",QVariant(5));
    ObjectManager::setSharedProperty(obj2,test_shared_property2);
    QVERIFY(ObjectManager::propertyExists(obj2,"Shared Property"));
    QVERIFY(ObjectManager::compareDynamicProperties(obj1,obj2));

    // 2. Test a multi context property:
    MultiContextProperty test_multi_context_property1("Multi Context Property");
    test_multi_context_property1.setValue(QVariant(1),1);
    test_multi_context_property1.setValue(QVariant(2),2);
    ObjectManager::setMultiContextProperty(obj1,test_multi_context_property1);
    QVERIFY(ObjectManager::propertyExists(obj1,"Multi Context Property"));
    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2));

    MultiContextProperty test_multi_context_property2("Multi Context Property");
    test_multi_context_property2.setValue(QVariant(1),1);
    test_multi_context_property2.setValue(QVariant(2),2);
    ObjectManager::setMultiContextProperty(obj2,test_multi_context_property2);
    QVERIFY(ObjectManager::propertyExists(obj2,"Multi Context Property"));
    QVERIFY(ObjectManager::compareDynamicProperties(obj1,obj2));

    // 3. Test a normal QVariant property
    QVariant test_normal_variant1(10);
    obj1->setProperty("Normal Property",test_normal_variant1);
    QVERIFY(ObjectManager::propertyExists(obj1,"Normal Property"));
    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2));

    QVariant test_normal_variant2(10);
    obj2->setProperty("Normal Property",test_normal_variant2);
    QVERIFY(ObjectManager::propertyExists(obj2,"Normal Property"));
    QVERIFY(ObjectManager::compareDynamicProperties(obj1,obj2));
}

void Qtilities::Testing::TestObjectManager::testCompareDynamicPropertiesDiff() {
    QObject* obj1 = new QObject;
    obj1->setObjectName("Object1");
    QObject* obj2 = new QObject;
    obj2->setObjectName("Object2");

    QVERIFY(ObjectManager::compareDynamicProperties(obj1,obj2));

    // Test Added Property:
    // 1. Test a shared property:
    SharedProperty test_shared_property1("Shared Property",QVariant(5));
    ObjectManager::setSharedProperty(obj1,test_shared_property1);
    QVERIFY(ObjectManager::propertyExists(obj1,"Shared Property"));
    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2));

    PropertyDiffInfo diff;
    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2,ObjectManager::SharedProperties,&diff));
    QCOMPARE(diff.d_added_properties.count(), 1);
    QCOMPARE(diff.d_added_properties.values().at(0), QString("5"));
    QCOMPARE(diff.d_removed_properties.count(), 0);
    QCOMPARE(diff.d_changed_properties.count(), 0);

    SharedProperty test_shared_property2("Shared Property",QVariant(5));
    ObjectManager::setSharedProperty(obj2,test_shared_property2);
    QVERIFY(ObjectManager::propertyExists(obj2,"Shared Property"));
    QVERIFY(ObjectManager::compareDynamicProperties(obj1,obj2));

    // 2. Test a multi context property:
    MultiContextProperty test_multi_context_property1("Multi Context Property");
    test_multi_context_property1.setValue(QVariant(1),1);
    test_multi_context_property1.setValue(QVariant(2),2);
    ObjectManager::setMultiContextProperty(obj1,test_multi_context_property1);
    QVERIFY(ObjectManager::propertyExists(obj1,"Multi Context Property"));
    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2));

    QVERIFY(!ObjectManager::compareDynamicProperties(obj2,obj1,ObjectManager::MultiContextProperties,&diff));
    QCOMPARE(diff.d_added_properties.count(), 0);
    QCOMPARE(diff.d_removed_properties.count(), 1);
    QCOMPARE(diff.d_removed_properties.values().at(0), QString("(1,2)"));
    QCOMPARE(diff.d_changed_properties.count(), 0);

    MultiContextProperty test_multi_context_property2("Multi Context Property");
    test_multi_context_property2.setValue(QVariant(1),1);
    test_multi_context_property2.setValue(QVariant(2),2);
    ObjectManager::setMultiContextProperty(obj2,test_multi_context_property2);
    QVERIFY(ObjectManager::propertyExists(obj2,"Multi Context Property"));
    QVERIFY(ObjectManager::compareDynamicProperties(obj1,obj2));

    // 3. Test a normal QVariant property
    QVariant test_normal_variant1(10);
    obj1->setProperty("Normal Property",test_normal_variant1);
    QVERIFY(ObjectManager::propertyExists(obj1,"Normal Property"));
    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2));

    QVariant test_normal_variant2(5);
    obj2->setProperty("Normal Property",test_normal_variant2);
    QVERIFY(ObjectManager::propertyExists(obj2,"Normal Property"));

    QVERIFY(!ObjectManager::compareDynamicProperties(obj1,obj2,ObjectManager::NonQtilitiesProperties,&diff));
    QCOMPARE(diff.d_added_properties.count(), 0);
    QCOMPARE(diff.d_removed_properties.count(), 0);
    QCOMPARE(diff.d_changed_properties.count(), 1);
    QCOMPARE(diff.d_changed_properties.values().at(0), QString("5,10"));
}

void Qtilities::Testing::TestObjectManager::testMoveSubjects() {

}
