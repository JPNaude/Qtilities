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

#ifndef OBJMANAGERCORE_H
#define OBJMANAGERCORE_H

#include "QtilitiesCore_global.h"
#include "IObjectManager.h"
#include "IContextManager.h"

#include <QList>
#include <QString>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        /*!
          \struct QtilitiesCoreData
          \brief The QtilitiesCoreData class stores private data used by the QtilitiesCore class.
         */
        struct QtilitiesCoreData;

        /*!
          \class QtilitiesCore
          \brief The QtilitiesCore is a singleton which provides accecss to the object and context managers among other things.

          The QtilitiesCore class is a singleton which provides access to the object and context managers. When using the Qtilities libraries in your application, this class allows provides the following
          features:
          - Notifications when settings changes in you application.
          - Can be used to indicate that your application is in the startup state. An example where this is usefull is in action management. See the \ref page_action_management article for more information. Note that you must explicitly set that your application is in startup.
         */

        class QTILIITES_CORE_SHARED_EXPORT QtilitiesCore : public QObject
        {
            Q_OBJECT

        public:
            static QtilitiesCore* instance();
            ~QtilitiesCore();

            //! Returns a reference to the observer manager. This reference will always be valid, thus there is no need to check the validity of the pointer.
            IObjectManager* objectManager();
            //! Returns a reference to the context manager. This reference will always be valid, thus there is no need to check the validity of the pointer.
            IContextManager* contextManager();

            //! Call this function when your application is finished loading.
            /*!
              In the startup state:
              - No context changes are emitted when using the action manager, thus no actions are updated when contexts change (which will happen as widgets are shown throughout startup).

              When going out of the startup state the standard context is set and broadcasted in the context manager.

              \sa startupFinished()
              */
            void setStartupState(bool in_startup);
            //! Function to check if the application is in the startup state.
            /*!
              True by default. You must
              \sa setStartupFinished()
              */
            bool startupFinished();
            //! Create a new settings update request.
            inline void newSettingsUpdateRequest(const QString& request_id) { emit settingsUpdateRequest(request_id); }

            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            QString version() const;

        signals:
            //! Signal which broadcasts that settings identified by the request_id changed and requires updating.
            void settingsUpdateRequest(const QString& request_id);

        private:
            QtilitiesCore(QObject* parent = 0);

            static QtilitiesCore* m_Instance;
            QtilitiesCoreData* d;
        };
    }
}

#define OBJECT_MANAGER Qtilities::Core::QtilitiesCore::instance()->objectManager()
#define CONTEXT_MANAGER Qtilities::Core::QtilitiesCore::instance()->contextManager()

#endif // OBJMANAGERCORE_H
