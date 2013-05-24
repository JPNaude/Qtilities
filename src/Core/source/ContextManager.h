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

#ifndef CONTEXTMANAGER_H
#define CONTEXTMANAGER_H

#include <QObject>
#include <QPointer>

#include "QtilitiesCore_global.h"
#include "IContextManager.h"

class QMainWindow;

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        /*!
        \struct ContextManagerPrivateData
        \brief A structure storing private data in the ContextManager class.
          */
        struct ContextManagerPrivateData;

        /*!
        \class ContextManager
        \brief A class which represents a context manager.

        The context manager automatically registers the standard application context, qti_def_CONTEXT_STANDARD.
          */
        class QTILIITES_CORE_SHARED_EXPORT ContextManager : public IContextManager
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContextManager)

        public:
            ContextManager(QObject* parent = 0);

            // --------------------------------
            // IContextManager Implementation
            // --------------------------------
            int registerContext(const QString& context, const QString& context_help_id = QString());
            bool unregisterContext(int context_id, bool notify = true);
            bool unregisterContext(const QString& context_string, bool notify = true);
            QList<int> allContexts() const;
            QStringList contextNames() const;
            QStringList activeContextNames() const;
            bool hasContext(QList<int> contexts) const;
            bool hasContext(int context) const;
            bool hasContext(const QString& context_string) const;
            QList<int> activeContexts() const;
            int contextID(const QString& context_string);
            QString contextString(int context_id) const;
            QString contextHelpID(int context_id) const;
            QString contextHelpID(const QString& context_string) const;

        public slots:
            void setNewContext(int context_id, bool notify = true);
            void appendContext(int context_id, bool notify = true);
            void removeContext(int context_id, bool notify = true);
            void setNewContext(const QString& context_string, bool notify = true);
            void appendContext(const QString& context_string, bool notify = true);
            void removeContext(const QString& context_string, bool notify = true);
            void broadcastState();           
            void addContexts(QObject* obj);

        private:
            QString contextName(int id) const;
            ContextManagerPrivateData* d;
        };
    }
}

#endif // CONTEXTMANAGER_H
