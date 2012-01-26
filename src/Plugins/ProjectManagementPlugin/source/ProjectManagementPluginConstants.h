/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#ifndef PROJECT_MANAGEMENT_PLUGIN_CONSTANTS_H
#define PROJECT_MANAGEMENT_PLUGIN_CONSTANTS_H

namespace Qtilities {
    namespace Plugins {
        //! Namespace containing all classes which is part of the ProjectManagement plugin.
        /*!
        The project management plugin can be used to add project management functionality to any application which contains objects implementing the Qtilities::ProjectManagement::IProjectItem interface which are registered in the global object pool.

        The plugin will do the following when loaded:
        - Automatically register all project items found in the global object pool with the project manager.
        - The project management configuration page will be added to the global object pool.
        - Add project management actions to the application's \p File menu (see image below).
        - The name of the current project and its modification state will be added to your application's main window (see image below).

        \image html project_menu_items.jpg "Project Management Plugin Menu Items"
          */
        namespace ProjectManagement {
            //! Namespace containing constants defined in the ProjectManagement plugin.
            namespace Constants {
                // Actions & Action Containers
                const char * const qti_action_PROJECTS_NEW        = "Projects.New";
                const char * const qti_action_PROJECTS_OPEN       = "Projects.Open";
                const char * const qti_action_PROJECTS_CLOSE      = "Projects.Close";
                const char * const qti_action_PROJECTS_SAVE       = "Projects.Save";
                const char * const qti_action_PROJECTS_SAVE_AS    = "Projects.SaveAs";
            }
        }
    }
}

#endif // PROJECT_MANAGEMENT_PLUGIN_CONSTANTS_H
