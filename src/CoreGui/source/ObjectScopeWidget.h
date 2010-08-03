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

            //! Sets the object for which the scope must be shown in the widget.
            void setObject(QObject* obj);
            //! When true, the object name will be shown above the object scope table.
            void setNameVisible(bool visible);
            bool eventFilter(QObject *object, QEvent *event);
            //! Action provider interface through which this widget provides its actions.
            IActionProvider* actionProvider();

        protected:
            virtual void changeEvent(QEvent *e);
            void constructActions();
            void refreshActions();

        private slots:
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
