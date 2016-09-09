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

#ifndef QTILITIES_APPLICATION_H
#define QTILITIES_APPLICATION_H

#include "QtilitiesCoreGui_global.h"
#include "ActionManager.h"
#include "ClipboardManager.h"

#ifndef QTILITIES_NO_HELP
#include "HelpManager.h"
#endif

#include <LoggingConstants>

#include <QtilitiesCoreApplication>

#include <QApplication>
#include <QMainWindow>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::Core;

        /*!
          \class QtilitiesApplication
          \brief The QtilitiesApplication class is the same as QApplication with added functionality.

          The QtilitiesApplication class is an extension of QApplication and should be used when building
          %Qtilities applications. For example:

\code
QtilitiesApplication a(argc, argv);
QtilitiesApplication::setOrganizationName("Jaco Naudé");
QtilitiesApplication::setOrganizationDomain("Qtilities");
QtilitiesApplication::setApplicationName("Example");
QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());
\endcode

         If you for some reason don't want to use the QtilitiesApplication as shown above, you must call the initialize()
         function instead in your application's main function.

         This class provides extra functionality which is not found in QApplication:
         - Access to an object manager through objectManager().
         - Access to a context manager through contextManager().
         - Access to an action manager through actionManager().
         - Access to a clipboard manager through clipboardManager().
         - The ability to define and access your top level main window in your application using setMainWindow() and mainWindow().
         - Settings update requests using newSettingsUpdateRequest() and settingsUpdateRequest().
         - Information about %Qtilities through qtilitiesVersionString() and aboutQtilities().
         - Access to the application's configuration page through configWidget().
         - Export version informatin for both %Qtilities as well as application specific versioning information. For more information see Qtilities::Core::Interfaces::IExportable.
         - Allows setting of a session path for application information, see applicationSessionPath().
         - Allows you to indicate that your application is busy and cannot be closed. See setApplicationBusy().

         All functions on QtilitiesApplication are thread safe.

          \sa Qtilities::Core::QtilitiesCoreApplication
         */
        class QTILITIES_CORE_GUI_SHARED_EXPORT QtilitiesApplication : public QApplication
        {
            Q_OBJECT

        public:
            QtilitiesApplication(int &argc, char ** argv);
            ~QtilitiesApplication();

            //! Returns a reference to the object manager.
            /*!
              This function is thread-safe.
              */
            static IObjectManager* objectManager();
            //! Returns a reference to the context manager.
            /*!
              This function is thread-safe.
              */
            static IContextManager* contextManager();
            #ifndef QTILITIES_NO_HELP
            //! Returns a reference to the help manager.
            /*!
              This function is thread-safe.
              */
            static HelpManager* helpManager();
            #endif
            //! Returns a reference to the action manager.
            /*!
              If you are using the action manager, you must specify a main window using setMainWindow() for
              the action manager to work correctly.

              This function is thread-safe.
              */
            static IActionManager* actionManager();
            //! Returns a reference to the clipboard manager.
            /*!
              This function is thread-safe.
              */
            static IClipboard* clipboardManager();
            //! Sets the main window reference for the application.
            static void setMainWindow(QWidget* mainWindow);
            //! Gets the main window reference for the application.
            static QWidget* mainWindow();
            //! Sets the configuration widget reference for the application.
            static void setConfigWidget(QWidget* configWidget);
            //! Gets the configuration widget reference for the application.
            static QWidget* configWidget();
            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            static QString qtilitiesVersionString();
            //! Returns the version number of %Qtilities.
            VersionNumber qtilitiesVersion();

            //! Initializes the QtilitiesApplication object.
            /*!
              Normally the QtilitiesApplication object will be initialized automatically in the application's main function
              as follows:

\code
QtilitiesApplication a(argc, argv);
\endcode

            If it however not possible to use QtilitiesApplication as show above it should be initialized directly after the above
            set of statements, as follows:

\code
QApplication a(argc, argv);
QApplication::setOrganizationName("My Organization");
QApplication::setApplicationName("My Application");
QtilitiesApplication::initialize();
\endcode

            Example scenarios where this might happen is when you are using a different class which inherits QApplication, for
            example the QtSingleApplication solution.

            It is important to note that the initialize() function does not create an QtilitiesApplication instance, thus instance() will return 0.
            The goal of the function used in this way is to register all needed %Qtilities classes in the %Qtilities factory.

            \note Make sure to set your application name and organization name before calling this function as shown in the second example above.
              */
            static void initialize();
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

            //! Create a new settings update request.
            /*!
                This function allows settings update requests to be sent anywhere in an application. This
                allows objects which depend on specific settings to update themselves when these settings change.

                This function will emit settingsUpdateRequest() with the given \p request_id.
              */
            static inline void newSettingsUpdateRequest(const QString& request_id) { if (m_Instance) { emit m_Instance->settingsUpdateRequest(request_id); } }

            //! Returns a session path for your application where you can store session related information.
            /*!
              Throughout %Qtilities session information is saved to this session path. The information that is saved includes:
              - Shortcut configurations. For more information see: Qtilities::CoreGui::Interfaces::IActionManager::saveShortcutMapping().
              - %Logging configurations. For more information see: Qtilities::Logging::Logger::saveSessionConfig().
              - Plugin configurations. For more information see: Qtilities::ExtensionSystem::ExtensionSystemCore::savePluginConfiguration().
              - Internal settings saved by %Qtilities classes to a QSettings based ini file. See Qtilities::Core::QtilitiesCoreApplication::qtilitiesSettingsPath() for more information.

              For more information about the way %Qtilities saves session information see \ref configuration_widget_storage_layout.

              By default this is QDesktopServices::storageLocation(DataLocation).

              \note In non-GUI applications, QDesktopServices is not available and the default session path is different for that reason. See Qtilities::Core::QtilitiesCoreApplication::applicationSessionPath()
              for more information.

              \sa setApplicationSessionPath()
              */
            static QString applicationSessionPath();
            //! Sets the session path to be used in your application.
            /*!
              \note This function will automatically update the session path used by the Logger as well by calling Qtilities::Logging::Logger::setLoggerSessionConfigPath().

              \sa applicationSessionPath()
              */
            static void setApplicationSessionPath(const QString& path);

            //! Sets the application export format for your application.
            /*!
              \sa Qtilities::Core::IExportable::applicationExportVersion(), applicationExportVersion()
              */
            static void setApplicationExportVersion(quint32 application_export_version);
            //! Gets the application export format for your application.
            /*!
              Default is 0.

              \sa Qtilities::Core::IExportable::applicationExportVersion(), setApplicationExportVersion()
              */
            static quint32 applicationExportVersion();

            //! Sets if the application is busy, thus it cannot be closed.
            /*!
              This function uses a stacked approach, thus your setApplicationBusy(false) calls must match the number of setApplicationBusy(true) calls.
              For more information on this type of stacked approach, see Qtilities::Core::Observer::startProcessingCycle().

              If you use Qtilities::CoreGui::QtilitiesMainWindow as your main window, it will automatically block close events when you application is busy and
              present the user with a message box saying that the application is busy.

              This function is thread-safe.

              \sa busyStateChanged()
              */
            static void setApplicationBusy(bool is_busy);
            //! Gets if the application is busy, thus it cannot be closed.
            /*!
             * \sa busyStateChanged()
             */
            static bool applicationBusy();

        signals:
            //! Signal which broadcasts that settings identified by the \p request_id changed and requires updating.
            void settingsUpdateRequest(const QString& request_id);
            //! Signal which broadcasts changes to the application's busy state.
            /*!
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void busyStateChanged(bool is_busy);

        public slots:
            //! Returns a Qtilities::CoreGui::AboutWindow with information about the Qtilities libraries.
            static QWidget* aboutQtilities(bool show = true);

        private:
            Q_DISABLE_COPY(QtilitiesApplication)

            static QtilitiesApplication* m_Instance;
        };
    }
}

#if defined(QtilitiesApp)
#undef QtilitiesApp
#endif
#define QtilitiesApp static_cast<Qtilities::CoreGui::QtilitiesApplication *>(QApplication::instance())
#define ACTION_MANAGER static_cast<Qtilities::CoreGui::QtilitiesApplication *>(QApplication::instance())->actionManager()
#define CLIPBOARD_MANAGER static_cast<Qtilities::CoreGui::QtilitiesApplication *>(QApplication::instance())->clipboardManager()
#define HELP_MANAGER static_cast<Qtilities::CoreGui::QtilitiesApplication *>(QApplication::instance())->helpManager()

#endif // QTILITIES_APPLICATION_H
