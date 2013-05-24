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

#ifndef PROJECT_MANAGEMENT_PLUGIN_CONSTANTS_H
#define PROJECT_MANAGEMENT_PLUGIN_CONSTANTS_H

namespace Qtilities {
    namespace Plugins {
        //! Namespace containing all classes which is part of the ProjectManagement plugin.
        /*!
        The project management plugin can be used to add project management functionality to any application which contains objects implementing the Qtilities::ProjectManagement::IProjectItem interface which are registered in the global object pool.

        The plugin will do the following when loaded:
        - The project management configuration page will be added to the global object pool.
        - Add project management actions to the application's \p File menu (see image below).
        - The name of the current project and its modification state will be added to your application's main window (see image below).

        \image html project_menu_items.jpg "Project Management Plugin Menu Items"
          */
        namespace ProjectManagement {
            //! Namespace containing constants defined in the ProjectManagement plugin.
            namespace Constants {
            }
        }
    }
}

#endif // PROJECT_MANAGEMENT_PLUGIN_CONSTANTS_H
