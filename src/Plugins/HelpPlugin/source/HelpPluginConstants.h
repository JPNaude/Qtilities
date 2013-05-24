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
