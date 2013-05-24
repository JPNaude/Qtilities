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

#ifndef ACTIONPROVIDER
#define ACTIONPROVIDER

#include "QtilitiesCoreGui_global.h"
#include "IActionProvider.h"

#include <QActionGroup>
#include <QList>
#include <QMap>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        /*!
        \struct ActionProviderPrivateData
        \brief The ActionProviderPrivateData struct stores private data used by the ActionProvider class.
          */
        struct ActionProviderPrivateData;

        /*!
        \class ActionProvider
        \brief A ready to use implementation of the IActionProvider interface.

In the development done using %Qtilities the problem was often faced where an object (which is not a QWidget) needed to provide some actions to another object. To solve the problem the concept of action providers was introducted. Any object can become an action provider and other objects (a toolbar for example) can query the object for actions that it provides, or it can add actions to the object, in the same way that you can add actions to QWidget.

Action providers allow actions to be grouped into categories and allows you to do specialized queries on the actions provided by an object, for example you can get only a list of actions which are enabled. The Qtilities::CoreGui::Interfaces::IActionProvider interface defines the interface that action providers use and the Qtilities::CoreGui::ActionProvider class provides a default implementation of this interface.

Many classes in %Qtilities are action providers and they provide actions through their \p actionProvider() function. An example is Qtilities::CoreGui::ObserverWidget, which provides all the actions that is created and allows you to get for example all the enabled actions for the current observer context. If you use an \p ObserverWidget that displays its actions in an action toolbar, you can easily add new actions to the toolbar where toolbars are created for each action category. For example:

\code
ObserverWidget widget;
widget.actionProvider()->addAction(my_action,QtilitiesCategory("My Action Category"));

// We can get a list of actions provided by any object like this:
QList<QAction*> actions = widget.actions();

// Only enabled actions:
QList<QAction*> enabled_actions = widget.actions(IActionProvider::FilterDisabled);

// Only actions for a specific category:
QList<QAction*> actions_per_category = widget.actions(IActionProvider::NoFilter,QtilitiesCategory("My Action Category"));
\endcode
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ActionProvider : public QObject, public IActionProvider {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IActionProvider)

        public:
            ActionProvider(QObject* parent = 0);
            virtual ~ActionProvider();

            // --------------------------------
            // IActionProvider Implementation
            // --------------------------------
            QList<QAction*> actions(IActionProvider::ActionFilterFlags action_filter = NoFilter, const QtilitiesCategory& category_filter = QtilitiesCategory("Hello World!")) const;
            QMap<QAction*, QtilitiesCategory> actionMap(IActionProvider::ActionFilterFlags action_filter = NoFilter, const QtilitiesCategory& category_filter = QtilitiesCategory()) const;
            /*!
              The ActionProvider implementation of the IActionProvider interface will always return
              a sorted category list in ascending order. That is: starts with 'AAA' and ends with 'ZZZ'.
              */
            QList<QtilitiesCategory> actionCategories() const;
            QList<QActionGroup*> actionGroups() const;
            QMap<QActionGroup*, QtilitiesCategory> actionGroupMap(QActionGroup *action_group) const;
            QList<QtilitiesCategory> actionGroupCategories() const;
            QAction *addAction(QAction * action, const QtilitiesCategory& category_filter = QtilitiesCategory());
            void addActions(QList<QAction*> actions, const QtilitiesCategory& category = QtilitiesCategory());
            QActionGroup *addActionGroup(QActionGroup * action_group, const QtilitiesCategory& category_filter = QtilitiesCategory());
            void disableAllActions();
            void enableAllActions();
            QList<QAction*> findActionsByText(const QString& match_string, Qt::MatchFlags match_flags = Qt::MatchFixedString);
            QList<QAction*> findActionsByObjectName(const QString& match_string, Qt::MatchFlags match_flags = Qt::MatchFixedString);
            void removeAction(QAction* action);
            void removeActionCategory(const QtilitiesCategory& category);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            ActionProviderPrivateData* d;
        };
    }
}

#endif // IACTIONPROVIDER
