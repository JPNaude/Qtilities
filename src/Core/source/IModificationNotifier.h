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
            \brief Interface which allows objects to broadcast changes made to them.

            Objects can implement this interface if they want to broadcast information when they are modified.

            For example: The project manager will manage project items that implement this interface. When a project item's state changes, it
            will emit its modificationStateChanged() signal which notifies the user that the project changed.
            When the user saves the project, the project manager will call the setModificationState() slot on
            all its project items and all objects in the project tree will have their modification state set to false.

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
                    NotifySubjects  = 2   /*!< Notify all subjects about the new state. The new state will be set on all subjects as well. */
                };
                Q_DECLARE_FLAGS(NotificationTargets, NotificationTarget)
                Q_FLAGS(NotificationTargets)

                //! Indicates the modification state of the object.
                virtual bool isModified() const = 0;
                //! Sets the modification state of the object. Returns true if it was successfull.
                /*!
                   When implementing this interface, the setModificationState() function must be declared as a slot.

                  \param notification_targets Indicates which targets must be notified of the change in modification state. By default all listeners are notified.
                  \param force_notifications Forces the object to deliver modification state changes even if the new_state is the same as its current state.

                  \sa NotificationTarget
                  */
                virtual void setModificationState(bool new_state, NotificationTargets notification_targets = NotifyListeners, bool force_notifications = false) = 0;
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
