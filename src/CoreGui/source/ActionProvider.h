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
        \class ActionProviderData
        \brief The ActionProviderData struct stores private data used by the ActionProvider class.
          */
        class ActionProviderData;

        /*!
        \class ActionProvider
        \brief A ready to use implementation of the IActionProvider interface.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ActionProvider : public QObject, public IActionProvider {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IActionProvider)

        public:
            ActionProvider(QObject* parent = 0);
            virtual ~ActionProvider();

            QList<QAction*> actions(IActionProvider::ActionFilterFlags action_filter = NoFilter, const QtilitiesCategory& category_filter = QtilitiesCategory()) const;
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
            QActionGroup *addActionGroup(QActionGroup * action_group, const QtilitiesCategory& category_filter = QtilitiesCategory());
            void disableAllActions();
            void enableAllActions();


        private:
            ActionProviderData* d;
        };
    }
}

#endif // IACTIONPROVIDER
