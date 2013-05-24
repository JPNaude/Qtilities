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

#ifndef EXTENSIONSYSTEMCONSTANTS_H
#define EXTENSIONSYSTEMCONSTANTS_H

namespace Qtilities {
    //! Namespace containing all the classes which forms part of the ExtensionSystem Module.
    /*!
    To use this module add, the following to your .pro %file:
    \code
    QTILITIES += extension_system;
    include(Qtilities_Path/src/Qtilities.pri)
    \endcode

    To include all files in this module:
    \code
    #include <QtilitiesExtensionSystem>
    using namespace QtilitiesExtensionSystem;
    \endcode

    For more information about the modules in %Qtilities, see \ref page_modules_overview.
    */
    namespace ExtensionSystem {
        //! Namespace containing constants used inside the ExtensionSystem Module.
        namespace Constants {
            // File Extensions
            //! The file extension used for plugin configuration set files.
            const char * const qti_def_SUFFIX_PLUGIN_CONFIG = ".pconfig";
        }
    }
}

#endif // EXTENSIONSYSTEMCONSTANTS_H
