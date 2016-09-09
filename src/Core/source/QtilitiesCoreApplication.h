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

#ifndef QTILITIES_CORE_H
#define QTILITIES_CORE_H

#include "QtilitiesCore_global.h"
#include "ObjectManager.h"
#include "ContextManager.h"
#include "VersionInformation.h"
#include "TaskManager.h"

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
         - Information about %Qtilities through qtilitiesVersionString().
         - Export version informatin for both %Qtilities as well as application specific versioning information. For more information see Qtilities::Core::Interfaces::IExportable.
         - Allows setting of a session path for application information, see applicationSessionPath().

          \note In GUI applications, the QtilitiesCoreApplication instance will not be created. It is still possible to access
          the objectManager(), contextManager() and qtilitiesVersionString() functions. For the rest of the functionality, like the
          settings update requests for example, you should use Qtilities::CoreGui::QtilitiesApplication.

         All functions on QtilitiesCoreApplication are thread safe.

          \sa Qtilities::CoreGui::QtilitiesApplication
         */

        class QTILIITES_CORE_SHARED_EXPORT QtilitiesCoreApplication : public QCoreApplication
        {
            Q_OBJECT

        public:
            QtilitiesCoreApplication(int &argc, char ** argv);
            ~QtilitiesCoreApplication();

            //! Returns a reference to the observer manager.
            /*!
              This function is thread-safe.
              */
            static IObjectManager* objectManager();
            //! Returns a reference to the context manager.
            /*!
              This function is thread-safe.
              */
            static IContextManager* contextManager();
            //! Returns a reference to the task manager.
            /*!
              This function is thread-safe.
              */
            static TaskManager* taskManager();

            //! Returns a reference to the QtilitiesCoreApplication instance.
            /*!
              \param silent When true, a warning message will not be printed when the QtilitiesCoreApplication instance was not found.
              */
            static QtilitiesCoreApplication* instance(bool silent = false);
            //! Checks if a valid instance has been created.
            /*!
              \param silent When true, a warning message will not be printed when the QtilitiesCoreApplication instance was not found.
              */
            static bool hasInstance(const char *function, bool silent = false);
            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            static QString qtilitiesVersionString();
            //! Returns the version number of %Qtilities.
            static VersionNumber qtilitiesVersion();

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

            //! QCoreApplication::notify() overload. This allows exception handling in non-GUI applications.
            bool notify(QObject * object, QEvent * event);

            //! Create a new settings update request.
            /*!
              This function allows settings update requests to be sent anywhere in an application. This
              allows objects which depend on the settings to update themselves when the settings change.

              This function will emit settingsUpdateRequest() with the given \p request_id.
              */
            static inline void newSettingsUpdateRequest(const QString& request_id) { emit m_Instance->settingsUpdateRequest(request_id); }

            //! Returns a session path for your application where you can store session related information (for example shortcut configurations etc.)
            /*!
              In non-GUI applications this path is QCoreApplication::applicationDirPath() + Qtilities::Logging::Constants::qti_def_PATH_SESSION by default. However in GUI
              applications the path is different as specified by Qtilities::CoreGui::QtilitiesApplication::applicationSessionPath().

              \sa setApplicationSessionPath(), Qtilities::CoreGui::QtilitiesApplication::applicationSessionPath()
              */
            static QString applicationSessionPath();
            //! Sets the session path to be used in your application.
            /*!
              \note This function will automatically update the session path used by the Logger as well by calling Qtilities::Logging::Logger::setLoggerSessionConfigPath().

              \sa applicationSessionPath(), Qtilities::CoreGui::QtilitiesApplication::setApplicationSessionPath()
              */
            static void setApplicationSessionPath(const QString& path);

            //! Gets the path of an ini file which is used by all %Qtilities classes to save information between different sessions.
            /*!
              The path used points to a file called \p qtilities.ini in the path returned by applicationSessionPath().

              Everywhere in %Qtilities this path is used as follows to save settings.

\code
if (!QtilitiesCoreApplication::qtilitiesSettingsEnabled())
    return;

QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
// Some settings related code...
\endcode

              By using this ini file, the %Qtilities settings are kept seperate from the rest of the settings used by your application. If you don't want
              %Qtilities to save any information during runtime you can disable is using setQtilitiesSettingsEnabled().

              \sa setQtilitiesSettingsEnabled(), qtilitiesSettingsEnabled()
              */
            static QString qtilitiesSettingsPath();
            //! Enables/disables the saving of settings by %Qtilities classes.
            /*!
              By disabling the saving of settings by %Qtilities classes, you can make sure %Qtilities does not save any settings information anywhere on the host machine where a
              %Qtilities application is run. When saving is disabled, all settings related functionality in %Qtilities will operate on default settings.

              Saving is enabled by default.

              \note This function will automatically enables/disables the saving of settings on the Logger as well by calling Qtilities::Logging::Logger::setLoggerSettingsEnabled().

              \sa qtilitiesSettingsPath(), qtilitiesSettingsEnabled()
              */
            static void setQtilitiesSettingsEnabled(bool is_enabled);
            //! Gets if the saving of settings by %Qtilities classes is enabled.
            /*!
              Saving is enabled by default.

              \sa setQtilitiesSettingsEnabled()
              */
            static bool qtilitiesSettingsEnabled();

            //! Sets if the application is busy, thus it cannot be closed.
            /*!
              This function uses a stacked approach, thus your setApplicationBusy(false) calls must match the number of setApplicationBusy(true) calls.

              For more information on this type of stacked approach, see Qtilities::Core::Observer::startProcessingCycle().

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

        private:
            Q_DISABLE_COPY(QtilitiesCoreApplication)

            static QtilitiesCoreApplication* m_Instance;
        };
    }
}

#define QtilitiesApp static_cast<Qtilities::Core::QtilitiesCoreApplication *>(QCoreApplication::instance())
#define OBJECT_MANAGER static_cast<Qtilities::Core::QtilitiesCoreApplication *>(QCoreApplication::instance())->objectManager()
#define CONTEXT_MANAGER static_cast<Qtilities::Core::QtilitiesCoreApplication *>(QCoreApplication::instance())->contextManager()
#define TASK_MANAGER static_cast<Qtilities::Core::QtilitiesCoreApplication *>(QCoreApplication::instance())->taskManager()

#endif // QTILITIES_CORE_H
