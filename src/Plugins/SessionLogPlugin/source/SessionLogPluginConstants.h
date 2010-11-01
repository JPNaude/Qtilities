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
                const char * const SESSION_LOG_MODE_ICON_48x48    = ":/icons/session_log_48x48.png";
            }
        }
    }
}

#endif // SESSION_LOG_PLUGIN_CONSTANTS_H
