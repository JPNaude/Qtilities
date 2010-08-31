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

using namespace Qtilities::CoreGui;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Building Tree Structures Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());

    // Create the observers
    Observer* observerA = new Observer("Observer A","Top level observer");
    observerA->useDisplayHints();
    Observer* observerB = new Observer("Observer B","Child observer");
    observerB->useDisplayHints();
    Observer* observerC = new Observer("Observer C","Child observer");
    observerC->useDisplayHints();

    // Create the objects
    QObject* object1 = new QObject();
    object1->setObjectName("Object 1");
    QObject* object2 = new QObject();
    object2->setObjectName("Object 2");
    QObject* object3 = new QObject();
    object3->setObjectName("Object 3");
    QObject* object4 = new QObject();
    object4->setObjectName("Object 4");
    QObject* object5 = new QObject();
    object5->setObjectName("Object 5");

    // Create the structure of the tree
    observerA->attachSubject(observerB);
    observerA->attachSubject(observerC);
    observerA->attachSubject(object1);
    observerA->attachSubject(object4);

    observerB->attachSubject(object2);
    observerB->attachSubject(object3);

    observerC->attachSubject(object5);
    observerC->attachSubject(object3);

    ObserverWidget* tree_widget = new ObserverWidget();
    QtilitiesApplication::setMainWindow(tree_widget);
    tree_widget->setObserverContext(observerA);
    tree_widget->initialize();
    tree_widget->show();

    return a.exec();
}
