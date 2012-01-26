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
#include <QTabWidget>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Building Tree Structures Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

    // We need to set a QWidget as the application's main window in order for proxy actions to work:
    QTabWidget* tab_widget = new QTabWidget;
    QtilitiesMainWindow mainWindow;
    mainWindow.enablePriorityMessages();
    mainWindow.priorityMessageLabel()->setWordWrap(true);
    mainWindow.setCentralWidget(tab_widget);
    QtilitiesApplication::setMainWindow(&mainWindow);

    Log->setLoggerSessionConfigPath(QtilitiesApplication::applicationSessionPath());
    LOG_INITIALIZE();

    // ----------------------------------
    // Uncategorized Tree
    // ------------------------------------
    TreeNode* rootNodeUncategorized = new TreeNode("Root");
    //rootNodeUncategorized->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    rootNodeUncategorized->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    rootNodeUncategorized->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);

    TreeNode* parentNode1 = rootNodeUncategorized->addNode("Parent 1");
    parentNode1->copyHints(rootNodeUncategorized->displayHints());
    TreeNode* parentNode2 = rootNodeUncategorized->addNode("Parent 2");
    // Not copying the hints here allows demonstration of multiple selections with different parent hints:
    //parentNode2->copyHints(rootNodeUncategorized->displayHints());
    TreeNode* parentNode3 = rootNodeUncategorized->addNode("Parent 3");
    parentNode3->copyHints(rootNodeUncategorized->displayHints());
    parentNode1->addItem("Child 1");
    parentNode1->addItem("Child 2");
    parentNode1->addItem("Child 3");
    parentNode1->addItem("Child 4");
    parentNode1->addItem("Child 5");
    parentNode2->addItem("Child 6");
    parentNode2->addItem("Child 7");
    parentNode2->addItem("Child 8");
    parentNode2->addItem("Child 9");
    parentNode2->addItem("Child 10");
    parentNode3->addItem("Child 11");
    parentNode3->addItem("Child 12");
    parentNode3->addItem("Child 13");
    parentNode3->addItem("Child 14");
    parentNode3->addItem("Child 15");

    TreeWidget* uncategorized_widget = new TreeWidget(rootNodeUncategorized);
    tab_widget->addTab(uncategorized_widget,QIcon(),"Uncategorized Tree");
    uncategorized_widget->treeView()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    uncategorized_widget->setDragDropCopyButton(Qt::LeftButton);
    uncategorized_widget->setDragDropMoveButton(Qt::RightButton);
    uncategorized_widget->show();

    // ------------------------------------
    // Categorized Tree
    // ------------------------------------
    TreeNode* rootNodeCategorized = new TreeNode("Root");
    rootNodeCategorized->enableCategorizedDisplay();
    // TODO: This breaks the toolbar for some reason... Looks like a display issue since it only happens in QTabWidget:
    rootNodeCategorized->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    rootNodeCategorized->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    rootNodeCategorized->displayHints()->setCategoryEditingFlags(ObserverHints::CategoriesEditableAllLevels | ObserverHints::CategoriesAcceptSubjectDrops);
    rootNodeCategorized->displayHints()->setDragDropHint(ObserverHints::AllowDrags);
    rootNodeCategorized->addItem("Child 1",QtilitiesCategory("Category 1::A",QString("::")));
    rootNodeCategorized->addItem("Child 2",QtilitiesCategory("Category 1::B",QString("::")));
    rootNodeCategorized->addItem("Child 3");
    rootNodeCategorized->addItem("Child 4",QtilitiesCategory("Category 2"));
    TreeItem* modified_item = rootNodeCategorized->addItem("Child 5",QtilitiesCategory("Category 2"));
    rootNodeCategorized->displayHints()->setModificationStateDisplayHint(ObserverHints::CharacterModificationStateDisplay);
    modified_item->setModificationState(true);
    TreeWidget* categorized_widget = new TreeWidget(rootNodeCategorized);
    tab_widget->addTab(categorized_widget,QIcon(),"Categorized Tree");
    categorized_widget->show();

    // ----------------------------------
    // Tree With Subject Filters
    // ----------------------------------
    TreeNode* rootNodeWithSubjectFilters = new TreeNode("Root");
    TreeNode* parentNode1WithSubjectFilters = rootNodeWithSubjectFilters->addNode("Parent 1");
    parentNode1WithSubjectFilters->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::CheckboxTriggered);
    TreeNode* parentNode2WithSubjectFilters = rootNodeWithSubjectFilters->addNode("Parent 2");
    parentNode2WithSubjectFilters->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames);
    parentNode1WithSubjectFilters->addItem("Child 1");
    parentNode1WithSubjectFilters->addItem("Child 2");
    parentNode2WithSubjectFilters->addItem("Try to rename me to Child 3");
    parentNode2WithSubjectFilters->addItem("Child 3");
    parentNode2WithSubjectFilters->addItem("Child 4");

    TreeWidget* subject_filters_widget = new TreeWidget(rootNodeWithSubjectFilters);
    tab_widget->addTab(subject_filters_widget,QIcon(),"Tree With Subject Filters");
    subject_filters_widget->show();

    /*rootNode->saveToFile(QtilitiesApplication::applicationSessionPath() + "/example_tree.xml");
    TreeWidget* uncategorized_widget = new TreeWidget(rootNode);
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
    dotGraph.saveToFile(QtilitiesApplication::applicationSessionPath() + "/output_file.gv");*/

    // ------------------------------------
    // Forcing Selection Of At Least One Item
    // ------------------------------------
    TreeNode* rootNodeMinimumSelection = new TreeNode("Root");
    rootNodeMinimumSelection->enableActivityControl(ObserverHints::NoActivityDisplay,
                                                    ObserverHints::FollowSelection,
                                                    ActivityPolicyFilter::UniqueActivity,
                                                    ActivityPolicyFilter::ParentIgnoreActivity,
                                                    ActivityPolicyFilter::ProhibitNoneActive,
                                                    ActivityPolicyFilter::SetNewActive);

    // TODO: This breaks the toolbar for some reason... Looks like a display issue since it only happens in QTabWidget:
    //rootNodeMinimumSelection->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    rootNodeMinimumSelection->displayHints()->setActionHints(ObserverHints::ActionRefreshView);
    rootNodeMinimumSelection->addItem("Child 1");
    rootNodeMinimumSelection->addItem("Child 2");
    rootNodeMinimumSelection->addItem("Child 3");
    rootNodeMinimumSelection->addItem("Child 4");
    TreeWidget* minimum_selection_widget = new TreeWidget(Qtilities::TableView);
    minimum_selection_widget->setObserverContext(rootNodeMinimumSelection);
    minimum_selection_widget->initialize();
    tab_widget->addTab(minimum_selection_widget,QIcon(),"Forcing Minimum Selection In Table");
    minimum_selection_widget->show();

    // ----------------------------------
    // Tree With Formatting
    // ----------------------------------
    // Create the tree nodes:
    TreeNode* nodeAFormatting = new TreeNode("Node A");
    nodeAFormatting->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::FollowSelection);

    // Some TreeItem classes:
    TreeNode* nodeBFormatting = nodeAFormatting->addNode("Tree Items");
    nodeBFormatting->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::FollowSelection);
    nodeBFormatting->addItem("Item 1");
    nodeBFormatting->addItem("Item 2");
    nodeBFormatting->addItem("Item 3");
    nodeBFormatting->addItem("Item 4");
    nodeBFormatting->addItem("Item 5");
    nodeBFormatting->addItem("Item 6");
    nodeBFormatting->addItem("Item 7");

    // Some TreeFileItem classes:
    TreeNode* nodeCFormatting = nodeAFormatting->addNode("File Tree Items");
    TreeFileItem* file_item = new TreeFileItem("c:/test_dir/test.xml");
    nodeCFormatting->addItem(file_item);

    // Add some formatting to the nodes:
    nodeAFormatting->setForegroundRole(QBrush(Qt::darkRed));
    nodeAFormatting->setFont(QFont("Helvetica [Cronyx]",20));
    nodeAFormatting->setAlignment(Qt::AlignCenter);
    nodeAFormatting->setBackgroundRole(QBrush(Qt::gray));
    nodeBFormatting->setForegroundRole(QBrush(Qt::darkGreen));
    nodeBFormatting->setFont(QFont("Helvetica [Cronyx]",20));
    nodeBFormatting->setAlignment(Qt::AlignCenter);
    nodeBFormatting->setBackgroundRole(QBrush(Qt::gray));
    nodeCFormatting->setForegroundRole(QBrush(Qt::darkYellow));
    nodeCFormatting->setFont(QFont("Helvetica [Cronyx]",20));
    nodeCFormatting->setAlignment(Qt::AlignCenter);
    nodeCFormatting->setBackgroundRole(QBrush(Qt::gray));

    /*nodeCFormatting->startProcessingCycle();
    for (int i = 0; i < 100; i++) {
        nodeCFormatting->addItem(QString("Batch Item").arg(i));
    }
    nodeCFormatting->endProcessingCycle();*/

    // Test activity:
    /*QList<QObject*> active_subjects;
    active_subjects << active_item;
    nodeBFormatting->activityPolicyFilter()Formatting->setActiveSubjects(active_subjects);*/

    // Test XML tree streaming:
    /*QString path_formatted = QString("%1/test_formatted.xml").arg(QtilitiesApplication::applicationSessionPath());
    nodeAFormatting->saveToFile(path_formatted);
    nodeAFormatting->loadFromFile(path_formatted);

    QString path_test = QString("%1/working_tree.xml").arg(QtilitiesApplication::applicationSessionPath());
    nodeAFormatting->addNodeFromFile(path_test);*/

    TreeWidget* formatted_widget = new TreeWidget(nodeAFormatting);
    tab_widget->addTab(formatted_widget,QIcon(),"Tree With Formatting");
    formatted_widget->show();

    tab_widget->show();

    mainWindow.resize(600,500);
    mainWindow.show();
    return a.exec();
}
