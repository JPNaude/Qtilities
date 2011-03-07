/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include <QApplication>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Building Tree Structures Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

    // Create the tree nodes:
    /*TreeNode* nodeA = new TreeNode("Node A");
    nodeA->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::FollowSelection);

    // Some TreeItem classes:
    TreeNode* nodeB = nodeA->addNode("Tree Items");
    nodeB->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::FollowSelection);
    nodeB->addItem("Item 1");
    nodeB->addItem("Item 2");
    nodeB->addItem("Item 3");
    nodeB->addItem("Item 4");
    nodeB->addItem("Item 5");
    nodeB->addItem("Item 6");
    nodeB->addItem("Item 7");

    // Some TreeFileItem classes:
    TreeNode* nodeC = nodeA->addNode("File Tree Items");
    TreeFileItem* file_item = new TreeFileItem("c:/test_dir/test.xml");
    nodeC->addItem(file_item);*/

    // Add some formatting to the nodes:
    /*nodeA->setForegroundRole(QBrush(Qt::darkRed));
    nodeA->setFont(QFont("Helvetica [Cronyx]",20));
    nodeA->setAlignment(Qt::AlignCenter);
    nodeA->setBackgroundRole(QBrush(Qt::gray));
    nodeB->setForegroundRole(QBrush(Qt::darkGreen));
    nodeB->setFont(QFont("Helvetica [Cronyx]",20));
    nodeB->setAlignment(Qt::AlignCenter);
    nodeB->setBackgroundRole(QBrush(Qt::gray));
    nodeC->setForegroundRole(QBrush(Qt::darkYellow));
    nodeC->setFont(QFont("Helvetica [Cronyx]",20));
    nodeC->setAlignment(Qt::AlignCenter);
    nodeC->setBackgroundRole(QBrush(Qt::gray));*/

    /*nodeC->startProcessingCycle();
    for (int i = 0; i < 100; i++) {
        nodeC->addItem(QString("Batch Item").arg(i));
    }
    nodeC->endProcessingCycle();*/

    // Test activity:
    /*QList<QObject*> active_subjects;
    active_subjects << active_item;
    nodeB->activityPolicyFilter()->setActiveSubjects(active_subjects);*/

    // Test XML tree streaming:
    /*QString path_formatted = QString("%1/test_formatted.xml").arg(QApplication::applicationDirPath());
    nodeA->saveToFile(path_formatted);
    nodeA->loadFromFile(path_formatted);

    QString path_test = QString("%1/working_tree.xml").arg(QApplication::applicationDirPath());
    nodeA->addNodeFromFile(path_test);

    // Create an observer widget wih the items:
    ObserverWidget* tree_widget = new ObserverWidget();
    QtilitiesApplication::setMainWindow(tree_widget);
    tree_widget->setObserverContext(nodeA);
    tree_widget->initialize();
    tree_widget->show();*/

    // ----------------------------------
    // Categorized Tree
    // ------------------------------------
    TreeNode* rootNode = new TreeNode("Root");
    rootNode->enableCategorizedDisplay();
    rootNode->addItem("Child 1",QtilitiesCategory("Category 1"));
    rootNode->addItem("Child 2",QtilitiesCategory("Category 1"));
    rootNode->addItem("Child 3");
    rootNode->addItem("Child 4",QtilitiesCategory("Category 2"));
    rootNode->addItem("Child 5",QtilitiesCategory("Category 2"));
    rootNode->displayHints()->setModificationStateDisplayHint(ObserverHints::CharacterModificationStateDisplay);

    // ----------------------------------
    // Uncategorized Tree
    // ------------------------------------
    /*TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    parentNode2->addItem("Child 5");*/

    ObserverWidget* uncategorized_widget = new ObserverWidget(rootNode);
    uncategorized_widget->show();

    // ----------------------------------
    // More Advanced Tree
    // ------------------------------------
    /*TreeNode* rootNode = new TreeNode("Root");
    TreeNode* parentNode1 = rootNode->addNode("Parent 1");
    parentNode1->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::CheckboxTriggered);
    TreeNode* parentNode2 = rootNode->addNode("Parent 2");
    parentNode2->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames);
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode2->addItem("Child 3");
    parentNode2->addItem("Child 4");
    parentNode2->addItem("Child 5");*/

    /*rootNode->saveToFile(QApplication::applicationDirPath() + "/example_tree.xml");
    ObserverWidget* uncategorized_widget = new ObserverWidget(rootNode);
    uncategorized_widget->show();*/

    /*TreeNode* node = new TreeNode("Root Node");
    TreeNode* nodeA = node->addNode("Node A");
    TreeNode* nodeB = node->addNode("Node B");
    nodeA->addItem("Item 1");
    nodeA->addItem("Item 2");
    TreeItem* sharedItem = nodeA->addItem("Shared Item");
    nodeB->attachSubject(sharedItem);
    nodeB->addItem("Item 3");
    nodeB->addItem("Item 4");

    // Create a dot script for this tree:
    ObserverDotWriter dotGraph(node);
    dotGraph.generateDotScript();
    dotGraph.saveToFile(QApplication::applicationDirPath() + "/output_file.gv");*/

    return a.exec();
}
