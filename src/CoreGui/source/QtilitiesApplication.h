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

#ifndef QTILITIES_APPLICATION_H
#define QTILITIES_APPLICATION_H

#include "QtilitiesCoreGui_global.h"
#include "ActionManager.h"
#include "ClipboardManager.h"

#include <QtilitiesCoreApplication>

#include <QApplication>
#include <QMainWindow>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::Core;

        /*!
          \class QtilitiesApplication
          \brief The QtilitiesApplication is the same as QApplication with added functionality.

          \sa \sa Qtilities::Core::QtilitiesCoreApplication
         */
        class QTILITIES_CORE_GUI_SHARED_EXPORT QtilitiesApplication : public QApplication
        {
            Q_OBJECT

        public:
            QtilitiesApplication(int &argc, char ** argv);
            ~QtilitiesApplication();

            //! Returns a reference to the object manager.
            static IObjectManager* const objectManager();
            //! Returns a reference to the context manager.
            static IContextManager* const contextManager();
            //! Returns a reference to the action manager.
            static IActionManager* const actionManager();
            //! Returns a reference to the clipboard manager.
            static IClipboard* const clipboardManager();
            //! Sets the main window reference for the application.
            static void setMainWindow(QMainWindow* mainWindow);
            //! Gets the main window reference for the application.
            static QMainWindow* const mainWindow();
            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            static QString qtilitiesVersion();

            //! Returns a reference to the QtilitiesApplication instance.
            static QtilitiesApplication* instance();
            //! Checks if a valid instance has been created.
            static bool hasInstance(const char *function);

            //! QCoreApplication::notify() overload. This allows exception handling in GUI applications.
            bool notify(QObject * object, QEvent * event);

            //! Create a new settings update request.
            /*!
              This function will emit settingsUpdateRequest() with the given request ID.
              */
            static inline void newSettingsUpdateRequest(const QString& request_id) { emit m_Instance->settingsUpdateRequest(request_id); }

        signals:
            //! Signal which broadcasts that settings identified by the request_id changed and requires updating.
            void settingsUpdateRequest(const QString& request_id);

        public slots:
            //! Displays an Qtilities::CoreGui::AboutWindow with information about the Qtilities libraries.
            void aboutQtilities();

        private:
            Q_DISABLE_COPY(QtilitiesApplication)

            static QtilitiesApplication* m_Instance;
        };
    }
}

#if defined(QtilitiesApp)
#undef QtilitiesApp
#endif
#define QtilitiesApp ((QtilitiesApplication *) QApplication::instance())
#define ACTION_MANAGER ((QtilitiesApplication *) QApplication::instance())->actionManager()
#define CLIPBOARD_MANAGER ((QtilitiesApplication *) QApplication::instance())->clipboardManager()

#endif // QTILITIES_APPLICATION_H
