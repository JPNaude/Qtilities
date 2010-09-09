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
using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Icons;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Building Tree Structures Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());

    // Create the tree nodes:
    TreeNode* nodeA = new TreeNode("Node A");
    TreeNode* nodeB = nodeA->addNode("Node B");
    TreeNode* nodeC = nodeA->addNode("Node C");
    ObserverHints::DisplayFlags display_flags = 0;
    display_flags |= ObserverHints::ItemView;
    display_flags |= ObserverHints::NavigationBar;
    display_flags |= ObserverHints::ActionToolBar;
    nodeA->displayHints()->setDisplayFlagsHint(display_flags);
    nodeB->displayHints()->setDisplayFlagsHint(display_flags);
    nodeC->displayHints()->setDisplayFlagsHint(display_flags);
    nodeC->enableNamingControl(ObserverHints::EditableNames,NamingPolicyFilter::ProhibitDuplicateNames);

    // Create the tree items:
    nodeA->addItem("Item 1");
    nodeB->addItem("Item 2");
    nodeB->addItem("Item 3");
    nodeB->addItem("Item 4");
    nodeB->addItem("Item 5");
    nodeB->addItem("Item 6");
    nodeB->addItem("Item 7");

    nodeC->startProcessingCycle();
    for (int i = 0; i < 100; i++) {
        nodeC->addItem(QString("Batch Item").arg(i));
    }
    nodeC->endProcessingCycle();

    // Test XML tree streaming:
    QString path = QString("%1/test.xml").arg(QApplication::applicationDirPath());
    nodeA->saveToFile(path);
    nodeA->loadFromFile(path);

    // Create an observer widget wih the items:
    ObserverWidget* tree_widget = new ObserverWidget();
    QtilitiesApplication::setMainWindow(tree_widget);
    tree_widget->setObserverContext(nodeA);
    tree_widget->initialize();
    tree_widget->show();

    return a.exec();
}
