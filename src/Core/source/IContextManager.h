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

#ifndef ICONTEXTMANAGER_H
#define ICONTEXTMANAGER_H

#include "QtilitiesCore_global.h"
#include "IObjectBase.h"

#include <QObject>
#include <QList>
#include <QStringList>
#include <QPointer>

namespace Qtilities {
    namespace Core {
        namespace Interfaces {
            /*!
            \class IContextManager
            \brief Interface used to communicate with the context manager.

            See the \ref page_action_management article for more information about the context manager.
              */
            class QTILIITES_CORE_SHARED_EXPORT IContextManager : public QObject, public IObjectBase
            {
                Q_OBJECT

            public:
                IContextManager(QObject* parent = 0) : QObject(parent) {}
                virtual ~IContextManager() {}

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                //! Registers a new context.
                /*!
                    The recommended way to do this is to let your object implement IContext and then register it with the global object pool which will automate the registration of your context.

                    \returns The unique ID assigned by the context manager for the new context. If the context already existed its existing ID is returned.
                    */
                virtual int registerContext(const QString& context, const QString& context_help_id = QString()) = 0;
                //! Unregisters an existing context.
                /*!
                 * \param context_id The ID of the context to be unregistered.
                 * \param notify Indicates if the context manager must notify when the active context(s) changed due to this unregisteration.
                 * \return True if successfull, false otherwise (for example when the context was not present in the context manager).
                 */
                virtual bool unregisterContext(int context_id, bool notify = true) = 0;
                //! Unregisters an existing context.
                /*!
                 * \param context_string The string representation of the context to be unregistered.
                 * \param notify Indicates if the context manager must notify when the active context(s) changed due to this unregisteration.
                 * \return True if successfull, false otherwise (for example when the context was not present in the context manager).
                 */
                virtual bool unregisterContext(const QString& context_string, bool notify = true) = 0;
                //! Returns a list of all the contexts currently registered.
                virtual QList<int> allContexts() const = 0;
                //! Returns the names of all registered contexts.
                virtual QStringList contextNames() const = 0;
                //! Returns the names of all active contexts.
                virtual QStringList activeContextNames() const = 0;
                //! Returns true if any of the contexts in the list were registered in the context manager.
                virtual bool hasContext(QList<int> contexts) const = 0;
                //! Returns true if the specified context is registered in the context manager.
                virtual bool hasContext(int context) const = 0;
                //! Returns true if the specified context string is registered in the context manager.
                virtual bool hasContext(const QString& context_string) const = 0;
                //! Sets a new context. Will clear all current active contexts.
                virtual void setNewContext(int context_id, bool notify = true) = 0;
                //! Appends a context to the set of active contexts.
                virtual void appendContext(int context_id, bool notify = true) = 0;
                //! Removes a context from the set of active contexts.
                virtual void removeContext(int context_id, bool notify = true) = 0;
                //! Sets a new context. Will clear all current active contexts. The context is identified using a string.
                virtual void setNewContext(const QString& context_string, bool notify = true) = 0;
                //! Appends a context to the set of active contexts. The context is identified using a string.
                virtual void appendContext(const QString& context_string, bool notify = true) = 0;
                //! Removes a context from the set of active contexts. The context is identified using a string.
                virtual void removeContext(const QString& context_string, bool notify = true) = 0;
                //! Gets the current context.
                virtual QList<int> activeContexts() const = 0;
                //! Returns an unique context ID for the given context string. Contexts are stored and identified using integer ID values throughout the library.
                virtual int contextID(const QString& context_string) = 0;
                //! Returns the string which was used to register a context id. If the context id does not exist, QString() is returned.
                virtual QString contextString(int context_id) const = 0;
                //! Returns the context help id for a given context id. QString() is returned if no help id was registered for the given context id.
                virtual QString contextHelpID(int context_id) const = 0;
                //! Returns the context help id for a given context string. QString() is returned if no help id was registered for the given context string.
                virtual QString contextHelpID(const QString& context_string) const = 0;

            public slots:
                //! Slot which will emit the current state of the context manager. Use this if you updated your contexts using notify = false.
                virtual void broadcastState() = 0;
                //! Inspects \p obj and if it implements Qtilities::Core::Interfaces::IContext, the context will be registered.
                virtual void addContexts(QObject* obj) = 0;

            signals:
                //! Notification that the context changes.
                void contextChanged(QList<int> new_contexts);
                //! Signal which is emitted when the setNewContext() is about to set the new context.
                void aboutToSetNewContext(int context_id);
                //! Signal which is emitted when the setNewContext() finished to set the new context.
                void finishedSetNewContext(int context_id);
                //! Signal which is emitted when the appendContext() is about to append the context.
                void aboutToAppendContext(int context_id);
                //! Signal which is emitted when the appendContext() function finished appending the context.
                void finishedAppendContext(int context_id);
                //! Signal which is emitted when the removeContext() is about to remove the context.
                void aboutToRemoveContext(int context_id);
                //! Signal which is emitted when the removeContext() is done removing the context.
                void finishedRemoveContext(int context_id);
                //! Signal which is emitted when the unregisterContext() is about to unregister the context.
                /*!
                 * \param context_id The applicable context.
                 *
                 * <i>This function was added in %Qtilities v1.2.</i>
                 */
                void aboutToUnregisterContext(int context_id);
                //! Signal which is emitted when the unregisterContext() is done unregistering the context.
                /*!
                 * \param context_id The applicable context.
                 *
                 * <i>This function was added in %Qtilities v1.2.</i>
                 */
                void finishedUnregisterContext(int context_id);
            };            
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IContextManager,"com.Qtilities.Core.IContextManager/1.0")

#endif // ICONTEXTMANAGER_H
