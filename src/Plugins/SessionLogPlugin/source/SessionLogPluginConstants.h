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

#ifndef SESSION_LOG_PLUGIN_CONSTANTS_H
#define SESSION_LOG_PLUGIN_CONSTANTS_H

namespace Qtilities {
    //! Namespace containing all available plugins.
    namespace Plugins {
        //! Namespace containing all classes which is part of the SessionLogPlugin.
        /*!
        The session log plugin can be used to add a ready to use session log mode to your application when you use the %Qtilities main window architecture and the extension system module.

        The plugin will do the following when loaded:
        - Add a logging mode to your application if you use the Qtilities::CoreGui::QtilitiesMainWindow widget as your main window.
        - Create three log dock widgets (see Qtilities::CoreGui::LoggerGui) for warnings, errors and all messages. These docks will be accessible through the logging mode in your application.
        - Add the logger configuration page to the global object pool.
        - Register the context of the session log mode.
          */
        namespace SessionLog {
            //! Namespace containing constants defined in the SessionLogPlugin.
            namespace Constants {
                // Icons
                const char * const qti_icon_SESSION_LOG_MODE_48x48    = ":/qtilities/plugins/sessionlog/icons/session_log_48x48.png";
            }
        }
    }
}

#endif // SESSION_LOG_PLUGIN_CONSTANTS_H
