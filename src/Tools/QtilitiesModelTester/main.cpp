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

#include <QtilitiesTesting>
using namespace QtilitiesTesting;

#include "modeltest.h"

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Qtilities Tester");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

    // Create the observer widget in tree mode:
    ObserverWidget* observer_widget = new ObserverWidget;
    observer_widget->resize(600,850);
    QtilitiesApplication::setMainWindow(observer_widget);

    Log->setLoggerSessionConfigPath(QtilitiesApplication::applicationSessionPath());
    LOG_INITIALIZE();

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

    // Init and show the observer widget:
    observer_widget->setObserverContext(rootNodeCategorized);
    observer_widget->initialize();

    // Test the tree model:
    new ModelTest(observer_widget->proxyModel());
    // Test the table model:
    //new ModelTest(observer_widget->tableModel());

    observer_widget->show();

    QStringList items;
    items << "A" << "B" << "C";
    rootNodeCategorized->addItems(items);

    TreeNode* nodeA = rootNodeCategorized->addNode("Node A");
    nodeA->copyHints(rootNodeCategorized->displayHints());
    TreeNode* nodeB = rootNodeCategorized->addNode("Node B");
    nodeB->copyHints(rootNodeCategorized->displayHints());

    nodeA->addItem("Child 1",QtilitiesCategory("Category 3::A",QString("::")));
    nodeA->addItem("Child 2",QtilitiesCategory("Category 4::B",QString("::")));
    nodeA->addItem("Child 3");
    nodeA->addItem("Child 4",QtilitiesCategory("Category 5"));

    nodeB->addItem("Child 1",QtilitiesCategory("Category 6::A",QString("::")));
    nodeB->addItem("Child 2",QtilitiesCategory("Category 7::B",QString("::")));
    nodeB->addItem("Child 3");
    nodeB->addItem("Child 4",QtilitiesCategory("Category 8"));

    return a.exec();
}
