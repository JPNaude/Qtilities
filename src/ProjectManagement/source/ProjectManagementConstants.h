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

#ifndef PROJECT_MANAGEMENT_CONSTANTS_H
#define PROJECT_MANAGEMENT_CONSTANTS_H

namespace Qtilities {
    //! Namespace containing all the classes which forms part of the ProjectManagement Module.
    /*!
    To use this module, add the following to your .pro %file:
    \code
    QTILITIES += project_management;
    include(Qtilities_Path/src/Qtilities.pri)
    \endcode

    To include all files in this module:
    \code
    #include <QtilitiesProjectManagement>
    using namespace QtilitiesProjectManagement;
    \endcode

    For more information about the modules in %Qtilities, see \ref page_modules_overview.
    */
    namespace ProjectManagement {
        //! Namespace containing constants used inside the ProjectManagement Module.
        namespace Constants {
            // File Extensions
            //! The file extension used for binary project files.
            const char * const qti_def_SUFFIX_PROJECT_BINARY  = "prj";
            //! The file extension used for xml project files. By default xml and formatted in the %Qtilities Tree Format.
            const char * const qti_def_SUFFIX_PROJECT_XML     = "xml";
        }
    }
}

#endif // PROJECT_MANAGEMENT_CONSTANTS_H
