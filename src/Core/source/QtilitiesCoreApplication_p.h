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

#ifndef QTILITIES_CORE_APPLICATION_P_H
#define QTILITIES_CORE_APPLICATION_P_H

#include "QtilitiesCore_global.h"
#include "ObjectManager.h"
#include "ContextManager.h"
#include "TaskManager.h"
#include "VersionInformation.h"

#include <QObject>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        /*!
          \class QtilitiesCoreApplicationPrivate
          \brief The QtilitiesCoreApplicationPrivate class stores private data used by the QtilitiesCoreApplication class.
         */
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesCoreApplicationPrivate : public QObject {
            Q_OBJECT

        public:
            static QtilitiesCoreApplicationPrivate* instance();
            ~QtilitiesCoreApplicationPrivate();

            //! Function to access object manager pointer.
            Qtilities::Core::Interfaces::IObjectManager* objectManager() const;
            //! Function to access context manager pointer.
            Qtilities::Core::Interfaces::IContextManager* contextManager() const;
            //! Function to access task manager pointer.
            Qtilities::Core::TaskManager* taskManager() const;          
            //! Returns the version string of %Qtilities as a QString.
            /*!
              \return The version of %Qtilities, for example: 0.1 Beta 1. Note that the v is not part of the returned string.
              */
            QString qtilitiesVersionString() const;
            //! Returns the version number of %Qtilities.
            VersionNumber qtilitiesVersion() const;
            //! Gets the session path used in your application.
            /*!
              By default this returns applicationSessionPathDefault() in a core application. In a GUI application it is overwritten by QtilitiesApplication to become:
\code
QDesktopServices::storageLocation(QDesktopServices::DataLocation)
\endcode
              */
            QString applicationSessionPath() const;
            //! The default session path.
            /*!
              By default this returns:
\code
QString("%1%2").arg(QCoreApplication::applicationDirPath()).arg(Qtilities::Logging::Constants::qti_def_PATH_SESSION);
\endcode

              \sa setApplicationSessionPath()
              */
            QString applicationSessionPathDefault() const;
            //! Sets the session path to be used in your application.
            /*!
              \sa applicationSessionPath()
              */
            void setApplicationSessionPath(const QString& path);

            //! Sets the application export format for your application.
            /*!
              \sa Qtilities::Core::IExportable::applicationExportVersion(), applicationExportVersion()
              */
            void setApplicationExportVersion(quint32 application_export_version);
            //! Gets the application export format for your application.
            /*!
              \sa Qtilities::Core::IExportable::applicationExportVersion(), setApplicationExportVersion()
              */
            quint32 applicationExportVersion() const;

            //! Gets the path of an ini file which is used by all %Qtilities classes to saved information between different sessions.
            QString qtilitiesSettingsPath();
            //! Enables/disables the saving of settings by %Qtilities classes.
            void setQtilitiesSettingsEnabled(bool is_enabled);
            //! Gets if the saving of settings by %Qtilities classes is enabled.
            bool qtilitiesSettingsEnabled() const;

            //! Sets if the application is busy, thus it cannot be closed.
            /*!
              This function uses a stacked approach, thus your setApplicationBusy(false) calls must match the number of setApplicationBusy(true) calls.

              For more information on this type of stacked approach, see Qtilities::Core::Observer::startProcessingCycle().
              */
            void setApplicationBusy(bool is_busy);
            //! Gets if the application is busy, thus it cannot be closed.
            bool applicationBusy() const;

        signals:
            //! Signal which broadcasts changes to the application's busy state.
            /*!
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void busyStateChanged(bool is_busy);

        private:
            QtilitiesCoreApplicationPrivate();
            static QtilitiesCoreApplicationPrivate* m_Instance;

            ObjectManager*      d_objectManager;
            IObjectManager*     d_objectManagerIFace;
            ContextManager*     d_contextManager;
            IContextManager*    d_contextManagerIFace;
            TaskManager*        d_taskManager;
            QString             d_application_session_path;
            VersionNumber       d_version_number;
            quint32             d_application_export_version;
            bool                d_settings_enabled;
            int                 d_application_busy_count;
        };
    }
}


#endif // QTILITIES_CORE_APPLICATION_P_H
