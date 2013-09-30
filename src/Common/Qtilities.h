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

#ifndef QTILITIES_H
#define QTILITIES_H

namespace Qtilities {
    //! The possible versions supported by Qtilities::Core::Intefaces::IExportable.
    /*!
      \note Versioning was introduced in %Qtilities v1.0, therefore versions before v1.0 are not supported any more. All future versions will be backward compatible.
      */
    enum ExportVersion {
        Qtilities_1_0           = 0,            /*!< %Qtilities v1.0. See \ref page_serializing_overview_1_0 for a detailed overview. */
        Qtilities_1_1           = 1,            /*!< %Qtilities v1.1. See \ref page_serializing_overview_1_1 for a detailed overview. */
        Qtilities_1_2           = 2,            /*!< %Qtilities v1.2. See \ref page_serializing_overview_1_2 for a detailed overview. */
        Qtilities_1_3           = 2,            /*!< %Qtilities v1.3. See \ref page_serializing_overview_1_2 for a detailed overview. */
        Qtilities_1_4           = 2,            /*!< %Qtilities v1.4. See \ref page_serializing_overview_1_2 for a detailed overview. */
        Qtilities_1_5           = 2,            /*!< %Qtilities v1.4. See \ref page_serializing_overview_1_2 for a detailed overview. */
        Qtilities_Latest        = Qtilities_1_5 /*!< The latest export version in the current version of %Qtilities. */
    };

    //! The %Qtilities major version number.
    #define qti_def_VERSION_MAJOR QTILITIES_VERSION_MAJOR
    //! The %Qtilities minor version number.
    #define qti_def_VERSION_MINOR QTILITIES_VERSION_MINOR
    //! The %Qtilities revision version number.
    #define qti_def_VERSION_REVISION QTILITIES_VERSION_REVISION
    //! The %Qtilities beta number.
    #define qti_def_VERSION_BETA 0
    //! The %Qtilities alpha number.
    #define qti_def_VERSION_ALPHA 0
}

#endif // QTILITIES_H
