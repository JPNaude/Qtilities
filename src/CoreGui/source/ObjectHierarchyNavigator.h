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

#ifndef OBJECTHIERARCHYNAVIGATOR_H
#define OBJECTHIERARCHYNAVIGATOR_H

#include <QWidget>
#include <QStack>

namespace Ui {
    class ObjectHierarchyNavigator;
}

namespace Qtilities {
    namespace CoreGui {
        /*!
        \class Qtilities::CoreGui::ObjectHierarchyNavigator
        \brief The ObjectHierarchyNavigator widget provides an indication of where you are in an observer-subject hierarhcy.

        This widget is appended to the top of the Qtilities::CoreGui::ObserverWidget widget when the observer context shown by the observer widget provides the correct hints.

        Below is an example where the widget is added to the top of an observer widget.

        \image html observer_widget_table_with_actions.jpg "Observer Widget With Action Toolbar"
        \image latex observer_widget_table_with_actions.eps "Observer Widget With Action Toolbar" width=3in

        \todo
        - Wishlist: Make observer names clickable to be able to navigate using the navigation bar widget.
        */
        class ObjectHierarchyNavigator : public QWidget {
            Q_OBJECT
        public:
            ObjectHierarchyNavigator(QWidget *parent = 0);
            ~ObjectHierarchyNavigator();

        public slots:
            void setCurrentObject(QObject* obj);
            //! Allows you to set the navigation stack of this widget. Setting the stack will automatically refresh the hierarchy view.
            void setNavigationStack(QStack<int> navigation_stack);
            void refreshHierarchy();

        protected:
            void changeEvent(QEvent *e);

        private:
            QObject* d_current_obj;
            QStack<int> d_navigation_stack;
            Ui::ObjectHierarchyNavigator *ui;
        };
    }
}

#endif // OBJECTHIERARCHYNAVIGATOR_H
