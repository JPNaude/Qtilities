/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
#include <QPointer>

namespace Ui {
    class ObjectHierarchyNavigator;
}

namespace Qtilities {
    namespace CoreGui {
        /*!
        \class Qtilities::CoreGui::ObjectHierarchyNavigator
        \brief The ObjectHierarchyNavigator widget provides an indication of where you are in an observer-subject hierarchy.

        This widget is appended to the top of the Qtilities::CoreGui::ObserverWidget widget when the observer context shown by the observer widget provides the correct hints
        and the observer widget uses the TableView mode.

        Below is an example where the widget is added to the top of an observer widget.

        \image html observer_widget_doc_table_view_categorized.jpg "Observer Widget Using The Object Hierarhcy Navigator"
        */
        class ObjectHierarchyNavigator : public QWidget {
            Q_OBJECT
        public:
            ObjectHierarchyNavigator(QWidget *parent = 0);
            ~ObjectHierarchyNavigator();

        public slots:
            //! Function to set the current object which is the item to the right most part of the hierarchy display.
            void setCurrentObject(QObject* obj);
            //! Allows you to set the navigation stack of this widget.
            /*!
              The front item in the stack is handled as the top most observer in the hierarchy (left in the display)
              and the last item is the observer parent of the current object (see setCurrentObject()).

              Setting the stack will automatically refresh the hierarchy view using refreshHierarchy().
            */
            void setNavigationStack(QStack<int> navigation_stack);
            //! Refreshes the display.
            void refreshHierarchy();

        protected:
            void changeEvent(QEvent *e);

        private:
            QPointer<QObject> d_current_obj;
            QStack<int> d_navigation_stack;
            Ui::ObjectHierarchyNavigator *ui;
        };
    }
}

#endif // OBJECTHIERARCHYNAVIGATOR_H
