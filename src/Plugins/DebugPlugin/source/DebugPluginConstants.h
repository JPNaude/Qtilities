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

#ifndef DEBUG_PLUGIN_CONSTANTS_H
#define DEBUG_PLUGIN_CONSTANTS_H

#include <Qtilities.h>

namespace Qtilities {
    namespace Plugins {
        //! Namespace containing all classes which is part of the debug plugin.
        /*!
        The debug plugin adds a debug mode to your application which provides the following information at runtime:
        - An overview of the global object pool during runtime.
        - An overview of all factories registered in the object manager and the tags registered in them.

        Below is a screenshot of this plugin in action visualizing the global object pool:
        \image html global_object_pool_view.jpg "Global Object Pool Visualization"

        Below is a screenshot of this plugin in action providing an overview of the factories in an application:
        \image html factory_debugging.jpg "Factory Debugging"
          */
        namespace Debug {
            //! Namespace containing constants defined in the debug plugin.
            namespace Constants {
                #define DEBUG_PLUGIN_VERSION_MAJOR QTILITIES_VERSION_MAJOR
                #define DEBUG_PLUGIN_VERSION_MINOR QTILITIES_VERSION_MINOR
            }
        }
    }
}

#endif // DEBUG_PLUGIN_CONSTANTS_H
