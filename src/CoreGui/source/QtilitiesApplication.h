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

          The QtilitiesApplication class is an extension of QApplication and should be used when building
          %Qtilities applications. For example:

\code
QtilitiesApplication a(argc, argv);
QtilitiesApplication::setOrganizationName("Jaco Naude");
QtilitiesApplication::setOrganizationDomain("Qtilities");
QtilitiesApplication::setApplicationName("Example");
QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());
\endcode

         If you for some reason don't want to use the QtilitiesApplication as shown above, you must call the initialize()
         function instead in your application's main function.

         This class provides extra functionality which is not found in QApplication:
         - Access to an object manager through objectManager().
         - Access to a context manager through contextManager().
         - Access to an action manager through actionManager().
         - Access to a clipboard manager through clipboardManager().
         - The ability to define your top level main window in your application using setMainWindow() and mainWindow().
         - Settings update requests using newSettingsUpdateRequest() and settingsUpdateRequest().
         - Information about %Qtilities through qtilitiesVersion() and aboutQtilities().

          \sa Qtilities::Core::QtilitiesCoreApplication
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
            /*!
              If you are using the action manager, you must specify a main window using setMainWindow() for
              the action manager to work correctly.
              */
            static IActionManager* const actionManager();
            //! Returns a reference to the clipboard manager.
            static IClipboard* const clipboardManager();
            //! Sets the main window reference for the application.
            static void setMainWindow(QWidget* mainWindow);
            //! Gets the main window reference for the application.
            static QWidget* const mainWindow();
            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            static QString qtilitiesVersion();

            //! Initializes the QtilitiesApplication object.
            /*!
              Normally the QtilitiesApplication object will be initialized automatically in the application's main function
              as follows:

\code
QtilitiesApplication a(argc, argv);
QtilitiesApplication::setOrganizationName("Jaco Naude");
QtilitiesApplication::setOrganizationDomain("Qtilities");
QtilitiesApplication::setApplicationName("Example");
QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());
\endcode

            If it however not possible to use QtilitiesApplication as show above it should be initialized directly after the above
            set of statements, as follows:

\code
QApplication a(argc, argv);
QApplication::setOrganizationName("Jaco Naude");
QApplication::setOrganizationDomain("Qtilities");
QApplication::setApplicationName("Example");
QApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersion());
QtilitiesApplication::initialize();
\endcode

            Example scenarios where this might happen is when you are using a different class which inherits QApplication, for
            example the QtSingleApplication solution.
              */
            void initialize();
            //! Returns a reference to the QtilitiesApplication instance.
            /*!
              \param silent When true, a warning message will not be printed when the QtilitiesCoreApplication instance was not found.
              */
            static QtilitiesApplication* instance(bool silent = false);
            //! Checks if a valid instance has been created.
            /*!
              \param silent When true, a warning message will not be printed when the QtilitiesCoreApplication instance was not found.
              */
            static bool hasInstance(const char *function, bool silent = false);

            //! QCoreApplication::notify() overload. This allows exception handling in GUI applications.
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
