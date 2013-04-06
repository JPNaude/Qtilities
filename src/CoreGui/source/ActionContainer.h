/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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

#ifndef ACTIONCONTAINER_H
#define ACTIONCONTAINER_H

#include <QObject>
#include <QMenu>
#include <QMenuBar>

#include "QtilitiesCoreGui_global.h"
#include "Command.h"

namespace Qtilities {
    namespace CoreGui {
        /*!
        \class ActionContainer
        \brief A class which represents an action container.

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ActionContainer : public QObject
        {
            Q_OBJECT

        public:
            ActionContainer(QObject* parent = 0) : QObject(parent) {}
            virtual ~ActionContainer() {}

            //! The menu associated with this action container. If the action container is a menu, 0 is returned.
            virtual QMenu *menu() const = 0;
            //! Adds an action to the action container. If the action container is a menu bar, this call does nothing.
            /*!
              \param before Inserts the action before the action represented by before.
              */
            virtual void addAction(Command *action, const QString &before = QString()) = 0;
            //! Adds a seperator to the action container. If the action container is a menu bar, this call does nothing.
            /*!
              \param before Inserts the seperator before the action represented by before.
              */
            virtual void addSeperator(const QString &before = QString()) = 0;

            //! The menu bar associated with this action container. If the action container is a menu bar, 0 is returned.
            virtual QMenuBar *menuBar() const = 0;
            //! Adds a menu to the action container. If the action container is a menu, the menu is inserted as a sub menu in the current menu.
            /*!
              \param before Inserts the menu before the action/menu represented by before.
              */
            virtual void addMenu(ActionContainer *menu, const QString &before = QString()) = 0;
        };

        /*!
        \struct MenuContainerPrivateData
        \brief A structure storing private data in menu containers.
          */
        struct MenuContainerPrivateData;

        /*!
        \class MenuContainer
        \brief A class which represents an implementation of ActionContainer in the form of a menu.

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT MenuContainer : public ActionContainer
        {
            Q_OBJECT

        public:
            MenuContainer(const QString& name, QObject* parent = 0);
            virtual ~MenuContainer();

            // --------------------------------
            // ActionContainer Implementation
            // --------------------------------
            QMenu *menu() const;
            void addAction(Command *command, const QString &before = QString());
            void addSeperator(const QString &before = QString());
            QMenuBar *menuBar() const;
            void addMenu(ActionContainer *menu, const QString &before = QString());

        private:
            MenuContainerPrivateData* d;
        };

        /*!
        \struct MenuBarContainerPrivateData
        \brief A structure storing private data in menubar containers.
          */
        struct MenuBarContainerPrivateData;

        /*!
        \class MenuBarContainer
        \brief A class which represents an implementation of ActionContainer in the form of a menubar.

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT MenuBarContainer : public ActionContainer
        {
            Q_OBJECT

        public:
            MenuBarContainer(QObject* parent = 0);
            virtual ~MenuBarContainer();

            // --------------------------------
            // ActionContainer Implementation
            // --------------------------------
            QMenu *menu() const;
            void addAction(Command *command, const QString &before = QString());
            void addSeperator(const QString &before = QString());
            QMenuBar *menuBar() const;
            void addMenu(ActionContainer *menu, const QString &before = QString());

        private:
            MenuBarContainerPrivateData* d;
        };
    }
}

#endif // ACTIONCONTAINER_H
