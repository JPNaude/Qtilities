/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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

#ifndef HELP_PLUGIN_CONSTANTS_H
#define HELP_PLUGIN_CONSTANTS_H

#include <Qtilities.h>

namespace Qtilities {
    namespace Plugins {
        //! Namespace containing all classes related to the Help Plugin.
        /*!
        This plugin provides a help mode in the %Qtilities main window which allows Qt Assistant help files to be shown.

        \image html plugins_help.jpg "Help Plugin Showing GUI Frontend For The Help Manager"

        In addition, it adds a configuration page for the help system to the user.

        \image html config_page_help.jpg "Help Configuration Page"

        The help plugin works through the help engine provided by the \p HELP_MANAGER (See Qtilities::CoreGui::QtilitiesApplication::helpManager() for more information).

        This plugin was introduced in %Qtilities v1.1.
        */
        namespace Help {
            //! Namespace containing all constants related to the Help Plugin.
            namespace Constants {
                // Icons
                const char * const HELP_MODE_ICON_48x48    = ":/qtilities/plugins/help/icons/help_48x48.png";
            }
        }
    }
}

#endif // HELP_PLUGIN_CONSTANTS_H
