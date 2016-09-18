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
            //! Adds a separator to the action container. If the action container is a menu bar, this call does nothing.
            /*!
              \param before Inserts the separator before the action represented by before.
              \deprecated Use \ref addSeparator instead, that one's spelled properly.
              */
            Q_DECL_DEPRECATED virtual void addSeperator(const QString &before = QString()) = 0;

            virtual void addSeparator(const QString &before = QString()) { addSeperator(before); }

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
            Q_DECL_DEPRECATED void addSeperator(const QString &before = QString());
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
