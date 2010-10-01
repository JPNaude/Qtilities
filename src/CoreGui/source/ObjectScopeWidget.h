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

#ifndef OBJECTSCOPEWIDGET_H
#define OBJECTSCOPEWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "IActionProvider.h"

#include <ObserverProperty.h>
#include <Observer.h>

#include <QWidget>
#include <QObject>
#include <QVariant>
#include <QPointer>
#include <QTableWidgetItem>

namespace Ui {
    class ObjectScopeWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \struct ObjectScopeWidgetData
        \brief The ObjectScopeWidgetData stores data used by the ObjectScopeWidget class.
          */
        struct ObjectScopeWidgetData;

        /*!
        \class ObjectScopeWidget
        \brief The ObjectScopeWidget class provides details about the scope & contexts in which an object is present in the form of a widget.

        As explained in the \ref page_observers article, an object can be attached to multiple observers.
        In such cases it be useful to display the scope of an object within the application using the
        ObjectScopeWidget widget. An example object scope widget is shown below. Tooltips provide more information about
        specific contexts to which an object is attached and different columns are available to
        provide details about each context.

        \image html object_scope_widget_without_owner.jpg "Object Scope Widget Without Owner"

        When an object is attached to an observer context using Qtilities::Core::Observer::SpecificObserverOwnership
        a new column appear in the ObjectScopeWidget indicating object ownership as shown below.

        \image html object_scope_widget_with_owner.jpg "Object Scope Widget With Owner"

        \todo
        - Does not catch detachment if last scoped observer is detached under manual ownership.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObjectScopeWidget : public QWidget {
            Q_OBJECT
            Q_DISABLE_COPY(ObjectScopeWidget)
            Q_ENUMS(ColumnIDs)

        public:
            explicit ObjectScopeWidget(QWidget *parent = 0);
            virtual ~ObjectScopeWidget();

            enum ColumnIDs {
                NameColumn = 0,
                UsesInstanceNameColumn = 1,
                OwnerColumn = 2
            };

            //! When true, the object name will be shown above the object scope table.
            void setNameVisible(bool visible);
            bool eventFilter(QObject *object, QEvent *event);
            //! Action provider interface through which this widget provides its actions.
            IActionProvider* actionProvider();

            // --------------------------------
            // Factory Interface Implemenation
            // --------------------------------
            static FactoryItem<QWidget, ObjectScopeWidget> factory;

        protected:
            virtual void changeEvent(QEvent *e);
            void constructActions();
            void refreshActions();

        private slots:
            //! Sets the object for which the scope must be shown.
            /*!
              \param obj The object which must be used.
              */
            void setObject(QObject* obj);
            //! Sets the object for which the scope must be shown.
            /*!
              \param obj The object which must be used.
              */
            void setObject(QPointer<QObject> obj);
            //! Sets the object for which the scope must be shown.
            /*!
              Function which allows this widget to be connected to the Qtilities::Core::Interfaces::IObjectManager::metaTypeActiveObjectsChanged() signal.

              \param objects A list of objects. When the list contains 1 item, it will be used in this widget.
              */
            void setObject(QList<QObject*> objects);
            //! Sets the object by providing a list of smart pointers.
            void setObject(QList<QPointer<QObject> > objects);
            //! Refreshes the view.
            void updateContents();
            //! Handles the event where the current object is destroyed.
            void handleObjectDestroyed();

            void handle_observerWidgetSelectionChange(QList<QObject*> objects);
            void handle_actionDuplicateInScope_triggered();
            void handle_actionAddContext_triggered();
            void handle_actionRemoveContext_triggered();
            void handle_actionDetachToSelection_triggered();
            void handle_currentItemChanged(QTableWidgetItem * current);

        private:
            static QPointer<ObjectScopeWidget> currentWidget;
            static QPointer<ObjectScopeWidget> actionContainerWidget;

            ObserverProperty getObserverProperty(const char* property_name) const;
            SharedObserverProperty getSharedProperty(const char* property_name) const;
            Ui::ObjectScopeWidget *m_ui;
            ObjectScopeWidgetData* d;
        };
    }
}

#endif // OBJECTSCOPEWIDGET_H
