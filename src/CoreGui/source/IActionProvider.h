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

#ifndef IACTIONPROVIDER
#define IACTIONPROVIDER

#include "QtilitiesCoreGui_global.h"

#include <QtilitiesCategory>
#include <IObjectBase>

#include <QActionGroup>
#include <QList>
#include <QMap>

using namespace Qtilities::Core;

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class IActionProvider
            \brief An interface through which objects can provide actions to other objects.

            For more information see ActionProvider.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IActionProvider : virtual public IObjectBase {
            public:
                IActionProvider() {}
                virtual ~IActionProvider() {}

                //! Action Filter Flag
                /*! The action filter flag is used to filter actions returned by the actions() function.
                  \sa actions()
                  */
                enum ActionFilter {
                    NoFilter = 0,           /*!< No filter. All actions in the action provider will be returned. */
                    FilterDisabled = 1,     /*!< Filter disabled actions, thus only enabled actions will be returned. */
                    FilterHidden = 2        /*!< Filter hidden actions, thus only visible actions will be returned. */
                };
                Q_DECLARE_FLAGS(ActionFilterFlags, ActionFilter);
                Q_FLAGS(ActionFilterFlags);

                //! A list of all the actions provided through the interface.
                /*!
                  \param action_filter The action filter flags to be used to filter returned actions.
                  \param category_filter Only actions which have a category which appears in the category filter will be part of the returned list.
                  */
                virtual QList<QAction *> actions(ActionFilterFlags action_filter = NoFilter, const QtilitiesCategory& category_filter = QtilitiesCategory("Hello World!")) const = 0;
                //! A map with the action and action category of all the actions provided through the interface.
                /*!
                  This function can be used to visualize the actions provided through this interface.

                  \param action_filter The action filter flags to be used to filter returned actions.
                  \param category_filter Only actions which have a category which appears in the category filter will be part of the returned list.
                  */
                virtual QMap<QAction*, QtilitiesCategory> actionMap(IActionProvider::ActionFilterFlags action_filter, const QtilitiesCategory& category_filter = QtilitiesCategory()) const = 0;
                //! A list of all action categories provided through this interface.
                virtual QList<QtilitiesCategory> actionCategories() const = 0;
                //! A list of all the action groups provided through the interface.
                virtual QList<QActionGroup*> actionGroups() const = 0;
                //! A map with the actions and action categories of all the action groups provided through the interface.
                /*!
                  This function can be used to visualize the actions provided through this interface.

                  \param action_group The action group for which the map is required. If no group is specified, all actions in all action groups are returned in the map.
                  */
                virtual QMap<QActionGroup*, QtilitiesCategory> actionGroupMap(QActionGroup* action_group = 0) const = 0;
                //! A list of all action group categories provided through this interface.
                virtual QList<QtilitiesCategory> actionGroupCategories() const = 0;
                //! Adds an action to the object implementing this interface and returns it if added successfully. Returns 0 otherwise.
                /*!
                  \param action The action to be added.
                  \param category The category under which the action should be added. By default no category is assigned. The first level of hierarchy in the category is defined by the first item in the list, and the last level of hierarchy by the last item in the list.
                  */
                virtual QAction* addAction(QAction * action, const QtilitiesCategory& category = QtilitiesCategory()) = 0;
                //! Adds an list of actions to the object implementing this interface.
                /*!
                  \param actions The actions to be added.
                  \param category The category under which the actions should be added. By default no category is assigned. The first level of hierarchy in the category is defined by the first item in the list, and the last level of hierarchy by the last item in the list.
                  */
                virtual void addActions(QList<QAction*> actions, const QtilitiesCategory& category = QtilitiesCategory()) = 0;
                //! Adds an action group to the object implementing this interface and returns it if added successfully. Returns 0 otherwise.
                /*!
                  \param action_group The action group to be added.
                  \param category The category under which the actions in the action group should be added. By default no category is assigned. The first level of hierarchy in the category is defined by the first item in the list, and the last level of hierarchy by the last item in the list.
                  */
                virtual QActionGroup* addActionGroup(QActionGroup* action_group, const QtilitiesCategory& category = QtilitiesCategory()) = 0;
                //! Disables all actions registered in this action provider.
                virtual void disableAllActions() = 0;
                //! Enables all actions registered in this action provider.
                virtual void enableAllActions() = 0;
                //! Finds an action that has the specified text and returns a reference to it, otherwise return 0 if it was not found.
                /*!
                  \param match_string The match string to match.
                  \param match_flags The match flags to use during matching. All match flags except Qt::MatchRecursive and Qt::MatchWrap can be used.
                  \returns All actions matching the match conditions. An empty list if no action matches the match conditions.
                  */
                virtual QList<QAction*> findActionsByText(const QString& match_string, Qt::MatchFlags match_flags = Qt::MatchFixedString) = 0;
                //! Finds an action with the specified objectName() and returns a reference to it, otherwise return 0 if it was not found.
                /*!
                  \param match_string The match string to match.
                  \param match_flags The match flags to use during matching. At present Qt::MatchExactly, Qt::MatchFixedString, Qt::MatchContains, Qt::MatchStartsWith, Qt::MatchEndsWith, Qt::MatchCaseSensitive is supported.
                  \returns All actions matching the match conditions. An empty list if no action matches the match conditions.
                  */
                virtual QList<QAction*> findActionsByObjectName(const QString& object_name, Qt::MatchFlags match_flags = Qt::MatchFixedString) = 0;
                //! Removes an action from the action provider. Does not delete the action.
                virtual void removeAction(QAction* action) = 0;
                //! Removes a complete category from the action provider. Does not delete the actions.
                virtual void removeActionCategory(const QtilitiesCategory& category) = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IActionProvider,"com.Qtilities.ObjManagement.IActionProvider/1.0")

#endif // IACTIONPROVIDER
