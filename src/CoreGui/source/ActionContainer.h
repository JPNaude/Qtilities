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
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ActionContainer : public QObject
        {
            Q_OBJECT
            Q_ENUMS(EmptyPolicy)

        public:
            ActionContainer(const QString& name, QObject* parent = 0) : QObject(parent) {
                container_name = name;
            }
            virtual ~ActionContainer() {}

            enum EmptyPolicy { None, Hide, Disable };

            //! Sets the emtpy policy of the action.
            void setEmptyPolicy(EmptyPolicy policy) { empty_policy = policy; }
            //! Gets the emtpy policy.
            EmptyPolicy emptyPolicy() { return empty_policy; }

            //! The menu associated with this command. If the container is a menu, 0 is returned.
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

            //! The menu bar associated with this command. If the command is a menu bar, 0 is returned.
            virtual QMenuBar *menuBar() const = 0;
            //! Adds a menu to the action container. If the action container is a menu, the menu is inserted as a sub menu in the current menu.
            /*!
              \param before Inserts the menu before the action/menu represented by before.
              */
            virtual void addMenu(ActionContainer *menu, const QString &before = QString()) = 0;

        protected:
            EmptyPolicy empty_policy;
            QString container_name;
        };

        /*!
        \struct MenuContainerData
        \brief A structure storing private data in menu containers.
          */
        struct MenuContainerData;

        /*!
        \class MenuContainer
        \brief A class which represents an implementation of ActionContainer in the form of a menu.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT MenuContainer : public ActionContainer
        {
            Q_OBJECT

        public:
            MenuContainer(const QString& name, QObject* parent = 0);
            virtual ~MenuContainer();

            // --------------------------------
            // ActionContainer Implemenation
            // --------------------------------
            QMenu *menu() const;
            void addAction(Command *command, const QString &before = QString());
            void addSeperator(const QString &before = QString());
            QMenuBar *menuBar() const;
            void addMenu(ActionContainer *menu, const QString &before = QString());

        public slots:
            void evaluateMenuActions();

        private:
            MenuContainerData* d;
        };

        /*!
        \struct MenuBarContainerData
        \brief A structure storing private data in menubar containers.
          */
        struct MenuBarContainerData;

        /*!
        \class MenuBarContainer
        \brief A class which represents an implementation of ActionContainer in the form of a menubar.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT MenuBarContainer : public ActionContainer
        {
            Q_OBJECT

        public:
            MenuBarContainer(QObject* parent = 0);
            virtual ~MenuBarContainer();

            // --------------------------------
            // ActionContainer Implemenation
            // --------------------------------
            QMenu *menu() const;
            void addAction(Command *command, const QString &before = QString());
            void addSeperator(const QString &before = QString());
            QMenuBar *menuBar() const;
            void addMenu(ActionContainer *menu, const QString &before = QString());

        private:
            MenuBarContainerData* d;
        };
    }
}

#endif // ACTIONCONTAINER_H
