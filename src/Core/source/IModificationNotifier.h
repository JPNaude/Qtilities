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

#ifndef IMODIFICATIONNOTIFIER_H
#define IMODIFICATIONNOTIFIER_H

#include "QtilitiesCore_global.h"
#include "IObjectBase.h"

#include <QList>
#include <QPointer>
#include <QString>
#include <QObject>

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            /*!
            \class IModificationNotifier
            \brief Objects can implement this interface if they want to broadcast information when they are modified/change.

            Objects can implement this interface if they want to broadcast information when they are modified. The use
            of this interface will be described using an example of a program supports project management.

            The project manager will manage some objects all implementing this interface. When the object changes it
            will emit the modificationStateChanged() signal. The user will then be notified that the project changed.
            When the user saves the project, the project manager will call the setModificationState() slot on
            all objects.

            When you want to connect to the modificationStateChange signal on this interface, connect to it on the object
            returned by objectBase().
              */
            class QTILIITES_CORE_SHARED_EXPORT IModificationNotifier : virtual public IObjectBase {
            public:
                IModificationNotifier() : d_isModified(false) {}
                virtual ~IModificationNotifier() {}

                //! The targets which should be notified when the state of the object implementing this interface changes.
                enum NotificationTarget {
                    NotifyNone      = 0,  /*!< No targets will be notified about the modification state change. */
                    NotifyListeners = 1,  /*!< Notify all listeners connected to the modification state change signals. \sa modificationStateChanged() */
                    NotifySubjects  = 2,  /*!< Notify all subjects about the new state. The new state will be set on all subjects as well. */
                };
                Q_DECLARE_FLAGS(NotificationTargets, NotificationTarget);
                Q_FLAGS(NotificationTargets);

                //! Indicates the modification state of the object.
                virtual bool isModified() const = 0;
                //! Sets the modification state of the object. Returns true if it was successfull.
                /*!
                   When implementing this interface, the setModificationState() function must be declared as a slot.
                  \param notification_targets Indicates which targets must be notified of the change in modification state. By default all listeners are notified.

                  \sa NotificationTarget
                  */
                virtual void setModificationState(bool new_state, NotificationTargets notification_targets = NotifyListeners) = 0;
                //! Implement this function as a signal when implementing the object.
                /*!
                  Because this interface does not use the Q_OBJECT macro, you cannot connect to this signal directly.
                  Instead you need to connect to the signal on the base object implementing this interface as follows:

                  \code
                  connect(iface->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(yourSlot(bool)));
                  \endcode
                  */
                virtual void modificationStateChanged(bool is_modified) const = 0;

            protected:
                bool d_isModified;
            };

            Q_DECLARE_OPERATORS_FOR_FLAGS(IModificationNotifier::NotificationTargets)
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IModificationNotifier,"com.Qtilities.Core.IModificationNotifier/1.0")

#endif // IMODIFICATIONNOTIFIER_H
