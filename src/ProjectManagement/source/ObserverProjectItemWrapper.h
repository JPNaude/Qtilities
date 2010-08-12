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

#ifndef OBSERVER_PROJECT_ITEM_WRAPPER_H
#define OBSERVER_PROJECT_ITEM_WRAPPER_H

#include "IProjectItem.h"
#include "ProjectManagement_global.h"

#include <Observer.h>

#include <QObject>

using namespace Qtilities::Core;
using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::ProjectManagement::Interfaces;

namespace Qtilities {
    namespace ProjectManagement {
        /*!
          \struct ObserverProjectItemWrapperData
          \brief The ObserverProjectItemWrapperData struct stores private data used by the ObserverProjectItemWrapper class.
         */
        struct ObserverProjectItemWrapperData;

        /*!
        \class ObserverProjectItemWrapper
        \brief This class wraps an observer context as a project item.
          */
        class PROJECT_MANAGEMENT_SHARED_EXPORT ObserverProjectItemWrapper : public QObject, public IProjectItem
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::ProjectManagement::Interfaces::IProjectItem)

        public:
            explicit ObserverProjectItemWrapper(QObject *parent = 0);

            //! Sets the observer context for this project item
            void setObserverContext(Observer* observer);

            // --------------------------------------------
            // IProjectItem Implementation
            // --------------------------------------------
            QString projectItemName() const;
            bool newProjectItem();
            bool loadProjectItem(QDataStream& stream);
            bool saveProjectItem(QDataStream& stream);
            bool closeProjectItem();

            // --------------------------------
            // IModificationNotifier Implemenation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners);
        signals:
            void modificationStateChanged(bool is_modified) const;
            void partialStateChanged(const QString& part_name) const;

        public:
            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }

        private:
            ObserverProjectItemWrapperData* d;
        };
    }
}

#endif // OBSERVER_PROJECT_ITEM_WRAPPER_H
