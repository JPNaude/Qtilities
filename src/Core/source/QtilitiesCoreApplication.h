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

#ifndef QTILITIES_CORE_H
#define QTILITIES_CORE_H

#include "QtilitiesCore_global.h"
#include "ObjectManager.h"
#include "ContextManager.h"

#include <Logger>
#include <QList>
#include <QString>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        /*!
          \class QtilitiesCoreApplication
          \brief The QtilitiesCoreApplication class is the same as QCoreApplication with added functionality.

         This class provides extra functionality which is not found in QCoreApplication:
         - Access to an object manager through objectManager().
         - Access to a context manager through contextManager().
         - Settings update requests using newSettingsUpdateRequest() and settingsUpdateRequest().
         - Information about %Qtilities through qtilitiesVersion().

          \note In GUI applications, the QtilitiesCoreApplication instance will not be created. It is still possible to access
          the objectManager(), contextManager() and qtilitiesVersion() functions. For the rest of the functionality, like the
          settings update request for example, you should use Qtilities::CoreGui::QtilitiesApplication.

          \sa Qtilities::CoreGui::QtilitiesApplication
         */

        class QTILIITES_CORE_SHARED_EXPORT QtilitiesCoreApplication : public QCoreApplication
        {
            Q_OBJECT

        public:
            QtilitiesCoreApplication(int &argc, char ** argv);
            ~QtilitiesCoreApplication();

            //! Returns a reference to the observer manager.
            static IObjectManager* const objectManager();
            //! Returns a reference to the context manager.
            static IContextManager* const contextManager();

            //! Returns a reference to the QtilitiesCoreApplication instance.
            static QtilitiesCoreApplication* instance();
            //! Checks if a valid instance has been created.
            static bool hasInstance(const char *function);
            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            static QString qtilitiesVersion();

            //! QCoreApplication::notify() overload. This allows exception handling in non-GUI applications.
            bool notify(QObject * object, QEvent * event);

            //! Create a new settings update request.
            /*!
            This function allows settings update requests to be sent anywhere in an application. This
            allows objects which depend on the settings to update themselves when the settings change.

              This function will emit settingsUpdateRequest() with the given \p request_id.
              */
            static inline void newSettingsUpdateRequest(const QString& request_id) { emit m_Instance->settingsUpdateRequest(request_id); }

        signals:
            //! Signal which broadcasts that settings identified by the \p request_id changed and requires updating.
            void settingsUpdateRequest(const QString& request_id);

        private:
            Q_DISABLE_COPY(QtilitiesCoreApplication)

            static QtilitiesCoreApplication* m_Instance;
        };
    }
}

#define QtilitiesApp ((QtilitiesCoreApplication *) QCoreApplication::instance())
#define OBJECT_MANAGER ((QtilitiesCoreApplication *) QCoreApplication::instance())->objectManager()
#define CONTEXT_MANAGER ((QtilitiesCoreApplication *) QCoreApplication::instance())->contextManager()

#endif // QTILITIES_CORE_H
