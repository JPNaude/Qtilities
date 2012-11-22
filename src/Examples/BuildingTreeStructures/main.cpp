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
    Log->setIsQtMessageHandler(false);

    // ----------------------------------
    // Uncategorized Tree
    // ------------------------------------
    TreeNode* rootNodeUncategorized = new TreeNode("Root");
    rootNodeUncategorized->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    rootNodeUncategorized->displayHints()->setActionHints(ObserverHints::ActionAllHints);
    rootNodeUncategorized->displayHints()->setDragDropHint(ObserverHints::AllDragDrop);
    rootNodeUncategorized->displayHints()->setRootIndexDisplayHint(ObserverHints::RootIndexHide);

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

    QWidget* combined_uncategorized_widget = new QWidget;
    if (combined_uncategorized_widget->layout())
        delete combined_uncategorized_widget->layout();
    QHBoxLayout* combined_uncategorized_widget_layout = new QHBoxLayout(combined_uncategorized_widget);
    combined_uncategorized_widget_layout->setMargin(0);

    TreeWidget* uncategorized_widget = new TreeWidget(rootNodeUncategorized);
    uncategorized_widget->treeView()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    uncategorized_widget->setDragDropCopyButton(Qt::LeftButton);
    uncategorized_widget->setDragDropMoveButton(Qt::RightButton);
    combined_uncategorized_widget_layout->addWidget(uncategorized_widget);
    uncategorized_widget->show();

    ObserverWidget* uncategorized_widget_level_view = new ObserverWidget(parentNode1,Qtilities::TableView);
    combined_uncategorized_widget_layout->addWidget(uncategorized_widget_level_view);
    uncategorized_widget_level_view->show();

    tab_widget->addTab(combined_uncategorized_widget,QIcon(),"Uncategorized Tree");

    // ------------------------------------
    // Demonstration of tree duplication
    // ------------------------------------
//    QString error_msg;
//    TreeNode* duplicated_node = IExportable::duplicateInstance<TreeNode>(rootNodeUncategorized,&error_msg);
//    TreeWidget* test_duplicated_widget = new TreeWidget(duplicated_node);
//    test_duplicated_widget->show();

    // ------------------------------------
    // Categorized Tree
    // ------------------------------------
    TreeNode* rootNodeCategorized = new TreeNode("Root");
    rootNodeCategorized->enableCategorizedDisplay();
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
    categorized_widget->show();
    tab_widget->addTab(categorized_widget,QIcon(),"Categorized Tree");

    // ----------------------------------
    // Parent Tracking Activity
    // ----------------------------------
    TreeNode* rootNodeParentTracking = new TreeNode("Root");
    rootNodeParentTracking->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    rootNodeParentTracking->displayHints()->setActionHints(ObserverHints::ActionSwitchView
                                                           | ObserverHints::ActionPushDown
                                                           | ObserverHints::ActionPushUp
                                                           | ObserverHints::ActionDeleteItem);
    rootNodeParentTracking->toggleQtilitiesPropertyChangeEvents(true);
    rootNodeParentTracking->enableActivityControl(ObserverHints::CheckboxActivityDisplay,
                                                  ObserverHints::CheckboxTriggered);

    TreeNode* nodeTracking1 = rootNodeParentTracking->addNode("Node 1");
    nodeTracking1->toggleQtilitiesPropertyChangeEvents(true);
    nodeTracking1->enableActivityControl(ObserverHints::CheckboxActivityDisplay,
                                               ObserverHints::CheckboxTriggered,
                                               ActivityPolicyFilter::MultipleActivity,
                                               ActivityPolicyFilter::ParentFollowActivity);
    nodeTracking1->copyHints(rootNodeParentTracking->displayHints());
    QStringList test_items;
    test_items << "Item 1";
    test_items << "Item 2";
    test_items << "Item 3";
    test_items << "Item 4";
    nodeTracking1->addItems(test_items);

    TreeNode* nodeTrackingSub1 = nodeTracking1->addNode("Sub Node 1");
    nodeTrackingSub1->enableActivityControl(ObserverHints::CheckboxActivityDisplay,
                                               ObserverHints::CheckboxTriggered,
                                               ActivityPolicyFilter::MultipleActivity,
                                               ActivityPolicyFilter::ParentFollowActivity);
    nodeTrackingSub1->copyHints(rootNodeParentTracking->displayHints());
    nodeTrackingSub1->addItems(test_items);

    TreeNode* nodeTracking2 = rootNodeParentTracking->addNode("Node 2");
    nodeTracking2->enableActivityControl(ObserverHints::CheckboxActivityDisplay,
                                               ObserverHints::CheckboxTriggered,
                                               ActivityPolicyFilter::MultipleActivity,
                                               ActivityPolicyFilter::ParentIgnoreActivity);
    nodeTracking2->copyHints(rootNodeParentTracking->displayHints());
    nodeTracking2->addItems(test_items);

    TreeWidget* parent_tracking_widget = new TreeWidget(rootNodeParentTracking);
    parent_tracking_widget->show();
    tab_widget->addTab(parent_tracking_widget,QIcon(),"Parent Tracking Activity");

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
    subject_filters_widget->show();
    tab_widget->addTab(subject_filters_widget,QIcon(),"Tree With Subject Filters");

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
                                                    ActivityPolicyFilter::SetNewInactive);

    // TODO: This breaks the toolbar for some reason... Looks like a display issue since it only happens in QTabWidget:
    //rootNodeMinimumSelection->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    //rootNodeMinimumSelection->displayHints()->setActionHints(ObserverHints::ActionRefreshView | ObserverHints::ActionDeleteItem);
    rootNodeMinimumSelection->addItem("Child 1");
    rootNodeMinimumSelection->addItem("Child 2");
    rootNodeMinimumSelection->addItem("Child 3");
    rootNodeMinimumSelection->addItem("Child 4");
    ObserverWidget* minimum_selection_widget = new ObserverWidget(Qtilities::TableView);
    minimum_selection_widget->setObserverContext(rootNodeMinimumSelection);
    minimum_selection_widget->initialize();
    minimum_selection_widget->show();
    tab_widget->addTab(minimum_selection_widget,QIcon(),"Forcing Minimum Selection In Table");
    rootNodeMinimumSelection->addItem("Child 5");

    // ------------------------------------
    // Big Table Shown Using Fetch More Implementation
    // ------------------------------------
    TreeNode* bigTableObserver = new TreeNode("Big Table");
    for (int i = 0; i < 10000; i++) {
        bigTableObserver->addItem("Item " + QString::number(i));
    }

    ObserverWidget* big_table_widget = new ObserverWidget(Qtilities::TableView);
    big_table_widget->setObserverContext(bigTableObserver);
    big_table_widget->initialize();
    big_table_widget->show();
    tab_widget->addTab(big_table_widget,QIcon(),"Big Table");

    // ------------------------------------
    // Big With Activity Filter
    // ------------------------------------

    TreeNode* bigTableObserverWithActivity = new TreeNode("Big Table With Activity Filter");
    bigTableObserverWithActivity->enableActivityControl(ObserverHints::CheckboxActivityDisplay,ObserverHints::CheckboxTriggered);
    bigTableObserverWithActivity->enableCategorizedDisplay();
    bigTableObserverWithActivity->displayHints()->setItemViewColumnHint(ObserverHints::ColumnNameHint | ObserverHints::ColumnChildCountHint);
    bigTableObserverWithActivity->displayHints()->setDisplayFlagsHint(ObserverHints::ItemView | ObserverHints::ActionToolBar);
    bigTableObserverWithActivity->displayHints()->setActionHints(ObserverHints::ActionRefreshView | ObserverHints::ActionSwitchView | ObserverHints::ActionDeleteItem);

    QTime time;
    time.start();
    int msg_count = 0;
    int ms = time.elapsed();
    int s = ms / 1000; ms %= 1000;
    int m = s / 60; s %= 60;
    int h = m / 60; m %= 60;
    ++msg_count;
    qDebug() << "##### Construction" << msg_count << QString("%1:%2:%3:%4").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0')).arg(ms,4,10,QChar('0'));

    for (int i = 0; i < 2000; i++) {
        bigTableObserverWithActivity->addItem("Item " + QString::number(i),QtilitiesCategory(QString::number(i)));

//        if ((i % 10000) == 0) {
//            ms = time.elapsed();
//            s = ms / 1000; ms %= 1000;
//            m = s / 60; s %= 60;
//            h = m / 60; m %= 60;
//            qDebug() << "##### Add item" << i << QString("%1:%2:%3:%4").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0')).arg(ms,4,10,QChar('0'));
//        }
    }

    ms = time.elapsed();
    s = ms / 1000; ms %= 1000;
    m = s / 60; s %= 60;
    h = m / 60; m %= 60;
    ++msg_count;
    qDebug() << "##### Construction" << msg_count << QString("%1:%2:%3:%4").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0')).arg(ms,4,10,QChar('0'));

    bigTableObserverWithActivity->treeAt(0);

    ms = time.elapsed();
    s = ms / 1000; ms %= 1000;
    m = s / 60; s %= 60;
    h = m / 60; m %= 60;
    ++msg_count;
    qDebug() << "##### Construction" << msg_count << QString("%1:%2:%3:%4").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0')).arg(ms,4,10,QChar('0'));

    ObserverWidget* big_table_with_activity_widget = new ObserverWidget(Qtilities::TreeView);
    big_table_with_activity_widget->setObserverContext(bigTableObserverWithActivity);
    //big_table_with_activity_widget->toggleLazyInit(true);
    big_table_with_activity_widget->initialize();
    big_table_with_activity_widget->show();
    tab_widget->addTab(big_table_with_activity_widget,QIcon(),"Big Table (With Activity Filter)");

//    ObserverWidget* big_table_with_activity_widget2 = new ObserverWidget(Qtilities::TreeView);
//    big_table_with_activity_widget2->setObserverContext(bigTableObserverWithActivity);
//    big_table_with_activity_widget2->toggleLazyInit(true);
//    big_table_with_activity_widget2->initialize();
//    big_table_with_activity_widget2->show();

    ms = time.elapsed();
    s = ms / 1000; ms %= 1000;
    m = s / 60; s %= 60;
    h = m / 60; m %= 60;
    ++msg_count;
    qDebug() << "##### Construction" << msg_count << QString("%1:%2:%3:%4").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0')).arg(ms,4,10,QChar('0'));

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
    formatted_widget->show();
    tab_widget->addTab(formatted_widget,QIcon(),"Tree With Formatting");

    tab_widget->show();

    mainWindow.resize(900,500);
    mainWindow.show();
    return a.exec();
}
