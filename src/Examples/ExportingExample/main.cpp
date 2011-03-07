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
#include <QtGui>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include "VersionDetails.h"
using namespace Qtilities::Examples::ExportingExample;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("YourOrganization");
    QtilitiesApplication::setOrganizationDomain("YourDomain");
    QtilitiesApplication::setApplicationName("My Application");
    QtilitiesApplication::setApplicationVersion("1.0");

    // Set the application export version:
    QtilitiesApplication::setApplicationExportVersion(0);

    // Register our VersionDetails class in the Qtilities factory:
    FactoryItemID version_info_id("Version Details");
    OBJECT_MANAGER->registerFactoryInterface(&VersionDetails::factory,version_info_id);

    // Next create a TreeNode with a couple of our classes attached to it:
    TreeNode* node = new TreeNode("TestNode");
    VersionDetails* ver1 = new VersionDetails;
    ver1->setDescriptionBrief("Version 1 Brief");
    ver1->setDescriptionDetailed("Version 1 Brief");
    ver1->setVersionMajor(0);
    ver1->setVersionMinor(0);
    VersionDetails* ver2 = new VersionDetails;
    ver2->setDescriptionBrief("Version 2 Brief");
    ver2->setDescriptionDetailed("Version 2 Brief");
    ver2->setVersionMajor(1);
    ver2->setVersionMinor(2);
    node->attachSubject(ver1);
    node->attachSubject(ver2);
    node->addNode("NewNode");

    // Next export the node to a file:
    node->saveToFile("Output_Version_0.xml");
    node->setApplicationExportVersion(1);
    node->saveToFile("Output_Version_1.xml");

    ObserverWidget* view = new ObserverWidget(node);
    view->show();
    return a.exec();
}
