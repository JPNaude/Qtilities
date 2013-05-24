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

#ifndef LOGGINGCONSTANTS_H
#define LOGGINGCONSTANTS_H

//! Namespace containing all the modules which forms part of the library set.
namespace Qtilities {
    //! Namespace containing all the classes which forms part of the Logging Module.
    /*!
    To use this module, add the following to your .pro %file:
    \code
    QTILITIES += logging;
    include(Qtilities_Path/src/Qtilities.pri)
    \endcode

    To include all files in this module:
    \code
    #include <QtilitiesLogging>
    using namespace QtilitiesLogging;
    \endcode

    For more information about the modules in %Qtilities, see \ref page_modules_overview.
    */
    namespace Logging {
        //! Namespace containing constants used inside the Logging Module.
        namespace Constants {
            // Default Formatting Engines
            const char * const qti_def_FORMATTING_ENGINE_DEFAULT    = "Default";
            const char * const qti_def_FORMATTING_ENGINE_RICH_TEXT  = "Rich Text";
            const char * const qti_def_FORMATTING_ENGINE_XML        = "XML Format";
            const char * const qti_def_FORMATTING_ENGINE_HTML       = "HTML Format";
            const char * const qti_def_FORMATTING_ENGINE_QT_MSG     = "Qt Messaging System Format";

            // Default Factory Tags
            const char * const qti_def_FACTORY_TAG_FILE_LOGGER_ENGINE = "qti.def.FactoryTag.File";

            // File Extensions
            const char * const qti_def_SUFFIX_LOGGER_CONFIG         = ".logconfig";

            // Default file paths (all subdirectories of the executable file)
            const char * const qti_def_PATH_SESSION                 = "Session";
            const char * const qti_def_PATH_LOGCONFIG_FILE          = "last_log_config.logconfig";
        }
    }
}

#endif // LOGGINGCONSTANTS_H
