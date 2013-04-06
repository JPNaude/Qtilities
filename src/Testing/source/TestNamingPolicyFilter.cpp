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

#include "TestNamingPolicyFilter.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <TreeIterator>

int Qtilities::Testing::TestNamingPolicyFilter::execTest(int argc, char ** argv) {
    return QTest::qExec(this,argc,argv);
}

void Qtilities::Testing::TestNamingPolicyFilter::testPropertyManagement() {
    TreeNode nodeA("nodeA");
    nodeA.enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::AllowDuplicateNames);
    TreeItem* item = nodeA.addItem("A");

    // -----------------------
    // Check the creation of the basic name properties:
    // -----------------------
    // Check qti_prop_NAME
    QVERIFY(ObjectManager::propertyExists(item,qti_prop_NAME) == true);
    QString property_name_value = ObjectManager::getSharedProperty(item,qti_prop_NAME).value().toString();
    QVERIFY(property_name_value == QString("A"));
    // Check qti_prop_NAME_MANAGER_ID
    QVERIFY(ObjectManager::propertyExists(item,qti_prop_NAME_MANAGER_ID) == true);
    int property_name_manager_id_value = ObjectManager::getSharedProperty(item,qti_prop_NAME_MANAGER_ID).value().toInt();
    QVERIFY(property_name_manager_id_value == nodeA.observerID());

    // -----------------------
    // Do some checking on instance names:
    // -----------------------
    TreeNode nodeB("nodeB");
    nodeB.enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames);
    nodeB.attachSubject(item);
    // Check qti_prop_ALIAS_MAP
    QVERIFY(ObjectManager::propertyExists(item,qti_prop_ALIAS_MAP) == true);
    QString property_alias_map_value = ObjectManager::getMultiContextProperty(item,qti_prop_ALIAS_MAP).value(nodeB.observerID()).toString();
    QVERIFY(property_alias_map_value == QString("A"));
    QVERIFY(ObjectManager::getMultiContextProperty(item,qti_prop_ALIAS_MAP).hasContext(nodeA.observerID()) == false);
    // Check qti_prop_NAME has not changed.
    QVERIFY(ObjectManager::propertyExists(item,qti_prop_NAME) == true);
    property_name_value = ObjectManager::getSharedProperty(item,qti_prop_NAME).value().toString();
    QVERIFY(property_name_value == QString("A"));
    // Check qti_prop_NAME_MANAGER_ID has not changed.
    QVERIFY(ObjectManager::propertyExists(item,qti_prop_NAME_MANAGER_ID) == true);
    property_name_manager_id_value = ObjectManager::getSharedProperty(item,qti_prop_NAME_MANAGER_ID).value().toInt();
    QVERIFY(property_name_manager_id_value == nodeA.observerID());

    // -----------------------
    // Check qti_prop_NAME sync'ing with objectName():
    // -----------------------
    nodeA.setMultiContextPropertyValue(item,qti_prop_NAME,QString("B"));
    property_name_value = ObjectManager::getSharedProperty(item,qti_prop_NAME).value().toString();
    QVERIFY(property_name_value == QString("B"));
    QVERIFY(item->objectName() == QString("B"));
    property_alias_map_value = ObjectManager::getMultiContextProperty(item,qti_prop_ALIAS_MAP).value(nodeB.observerID()).toString();
    QVERIFY(property_alias_map_value == QString("A"));
}

void Qtilities::Testing::TestNamingPolicyFilter::testSetUniquenessPolicies() {
    TreeNode node;
    NamingPolicyFilter* filter = node.enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::AllowDuplicateNames);

    // Check that policy can be changed only if no subjects are attached:
    NamingPolicyFilter::UniquenessPolicy policy = filter->uniquenessNamingPolicy();
    filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
    QVERIFY(policy != filter->uniquenessNamingPolicy());

    // Now attach some subjects and see that we can't change it anymore:
    node.addItem("A");
    node.addItem("B");
    policy = filter->uniquenessNamingPolicy();
    filter->setUniquenessPolicy(NamingPolicyFilter::AllowDuplicateNames);
    QVERIFY(policy == filter->uniquenessNamingPolicy());
}

void Qtilities::Testing::TestNamingPolicyFilter::testRejectUniquenessResolutionPolicy() {
    TreeNode node;
    node.enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames,NamingPolicyFilter::Reject);

    QVERIFY(node.subjectCount() == 0);
    node.addItem("A");
    QVERIFY(node.subjectCount() == 1);
    node.addItem("B");
    QVERIFY(node.subjectCount() == 2);
    node.addItem("B");
    QVERIFY(node.subjectCount() == 2);
}

void Qtilities::Testing::TestNamingPolicyFilter::testAutoRenameUniquenessResolutionPolicy() {
    TreeNode node;
    node.enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames,NamingPolicyFilter::AutoRename);

    QVERIFY(node.subjectCount() == 0);
    node.addItem("A");
    QVERIFY(node.subjectCount() == 1);
    node.addItem("B");
    QVERIFY(node.subjectCount() == 2);
    TreeItem* item = node.addItem("B");
    QVERIFY(node.subjectCount() == 3);
    QVERIFY(item->objectName() == "B_1");
}

void Qtilities::Testing::TestNamingPolicyFilter::testRejectValidityResolutionPolicy() {
    TreeNode node;
    node.enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames,NamingPolicyFilter::Reject,NamingPolicyFilter::Reject);

    QVERIFY(node.subjectCount() == 0);
    node.addItem("A");
    QVERIFY(node.subjectCount() == 1);
    node.addItem("B");
    QVERIFY(node.subjectCount() == 2);
    node.addItem("");
    QVERIFY(node.subjectCount() == 2);
}

void Qtilities::Testing::TestNamingPolicyFilter::testProcessingCycleValidationChecks() {
    TreeNode node;
    NamingPolicyFilter* filter = node.enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames,NamingPolicyFilter::Reject,NamingPolicyFilter::Reject);
    filter->setProcessingCycleValidationChecks(NamingPolicyFilter::NoChecks);

    node.startProcessingCycle();
    QVERIFY(node.subjectCount() == 0);
    node.addItem("A");
    QVERIFY(node.subjectCount() == 1);
    node.addItem("A");
    QVERIFY(node.subjectCount() == 2);
    TreeItem* item = node.addItem("");
    QVERIFY(node.subjectCount() == 3);
    node.detachSubject(item);
    QVERIFY(node.subjectCount() == 2);

    filter->setProcessingCycleValidationChecks(NamingPolicyFilter::Validity);
    node.addItem("");
    QVERIFY(node.subjectCount() == 2);
    node.addItem("A");
    QVERIFY(node.subjectCount() == 3);

    filter->setProcessingCycleValidationChecks(NamingPolicyFilter::Uniqueness);

    node.addItem("");
    QVERIFY(node.subjectCount() == 4);
    node.addItem("A");
    QVERIFY(node.subjectCount() == 4);

    node.endProcessingCycle();
}

