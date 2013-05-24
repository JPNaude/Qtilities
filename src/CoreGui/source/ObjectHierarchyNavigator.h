/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
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
