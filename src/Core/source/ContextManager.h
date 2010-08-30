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
        \struct ContextManagerData
        \brief A structure storing private data in the ContextManager class.
          */
        struct ContextManagerData;

        /*!
        \class ContextManager
        \brief A class which represents a context manager.

        The context manager automatically registers the standard application context, CONTEXT_STANDARD.
          */
        class QTILIITES_CORE_SHARED_EXPORT ContextManager : public IContextManager
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContextManager)

        public:
            ContextManager(QObject* parent = 0);

            // --------------------------------
            // IContextManager Implemenation
            // --------------------------------
            int registerContext(const QString& context);
            QList<int> allContexts() const;
            QStringList contextNames() const;
            QStringList activeContextNames() const;
            bool hasContext(QList<int> contexts) const;
            bool hasContext(int context) const;
            bool hasContext(const QString& context_string) const;
            QList<int> currentContexts() const;
            int contextID(const QString& context_string);
            QString contextString(int context_id) const;

        public slots:
            void setNewContext(int context_id, bool notify = true);
            void appendContext(int context_id, bool notify = true);
            void removeContext(int context_id, bool notify = true);
            void setNewContext(const QString &context_string, bool notify = true);
            void appendContext(const QString& context_string, bool notify = true);
            void removeContext(const QString& context_string, bool notify = true);
            void broadcastState();           

        signals:
            //! Notification that the context changes.
            void contextChanged(QList<int> new_contexts);

        public slots:
            void addContexts(QObject* obj);

        private:
            QString contextName(int id) const;
            ContextManagerData* d;
        };
    }
}



#endif // CONTEXTMANAGER_H
