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

#ifndef IACTIONPROVIDER
#define IACTIONPROVIDER

#include "QtilitiesCoreGui_global.h"

#include <QActionGroup>
#include <QList>
#include <QMap>

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {

            /*!
            \class IActionProvider
            \brief An interface through which objects can provide actions to other objects.

            \sa ActionProvider
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IActionProvider {
            public:
                IActionProvider() {}
                virtual ~IActionProvider() {}

                //! A list of all the actions provided through the interface.
                /*!
                  \param only_enabled When true, only the enabled actions will be returned.
                  \param category_filter Only actions which have a category which appears in the category filter will be part of the returned list.
                  */
                virtual QList<QAction *> actions(bool only_enabled = false, const QStringList& category_filter = QStringList()) const = 0;
                //! A map with the action and action category of all the actions provided through the interface.
                /*!
                  This function can be used to visualize the actions provided through this interface.

                  \param only_enabled When true, only the enabled actions will be returned.
                  \param category_filter Only actions which have a category which appears in the category filter will be part of the returned list.
                  */
                virtual QMap<QAction*, QStringList> actionMap(bool only_enabled = false, const QStringList& category_filter = QStringList()) const = 0;
                //! A list of all action categories provided through this interface.
                virtual QList<QStringList> actionCategories() const = 0;
                //! A list of all the action groups provided through the interface.
                virtual QList<QActionGroup*> actionGroups() const = 0;
                //! A map with the actions and action categories of all the action groups provided through the interface.
                /*!
                  This function can be used to visualize the actions provided through this interface.

                  \param action_group The action group for which the map is required. If no group is specified, all actions in all action groups are returned in the map.
                  */
                virtual QMap<QActionGroup*, QStringList> actionGroupMap(QActionGroup* action_group = 0) const = 0;
                //! A list of all action group categories provided through this interface.
                virtual QList<QStringList> actionGroupCategories() const = 0;
                //! Adds an action to the object implementing this interface and returns it if added succesfully. Returns 0 otherwise.
                /*!
                  \param action The action to be added.
                  \param category The category under which the action should be added. By default no category is assigned. The first level of hierarchy in the category is defined by the first item in the list, and the last level of hierarchy by the last item in the list.
                  */
                virtual QAction* addAction(QAction * action, const QStringList& category = QStringList()) = 0;
                //! Adds an action group to the object implementing this interface and returns it if added succesfully. Returns 0 otherwise.
                /*!
                  \param action_group The action group to be added.
                  \param category The category under which the actions in the action group should be added. By default no category is assigned. The first level of hierarchy in the category is defined by the first item in the list, and the last level of hierarchy by the last item in the list.
                  */
                virtual QActionGroup* addActionGroup(QActionGroup* action_group, const QStringList& category = QStringList()) = 0;
                //! Disables all actions registered in this action provider.
                virtual void disableAllActions() = 0;
                //! Enables all actions registered in this action provider.
                virtual void enableAllActions() = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IActionProvider,"com.Qtilities.ObjManagement.IActionProvider/1.0")

#endif // IACTIONPROVIDER
