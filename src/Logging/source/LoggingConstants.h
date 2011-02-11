/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#ifndef LOGGINGCONSTANTS_H
#define LOGGINGCONSTANTS_H

//! Namespace containing all the modules which forms part of the library set.
namespace Qtilities {
    //! Namespace containing all the classes which forms part of the Logging Module.
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
            const char * const qti_def_PATH_SESSION            = "/session";
            const char * const qti_def_PATH_LOGCONFIG_FILE          = "last_log_config.logconfig";
        }
    }
}

#endif // LOGGINGCONSTANTS_H
