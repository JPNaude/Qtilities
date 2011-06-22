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

#ifndef QTILITIES_H
#define QTILITIES_H

namespace Qtilities {
    //! The possible versions supported by Qtilities::Core::Intefaces::IExportable.
    /*!
      \note Versioning was introducted in %Qtilities v1.0, therefore versions before v1.0 are not supported any more. All future versions will be backward compatible.
      */
    enum ExportVersion {
        Qtilities_1_0           = 0,            /*!< %Qtilities version 0.3. See \ref page_serializing_overview_1_0 for an detailed overview. */
        Qtilities_Latest        = Qtilities_1_0 /*!< The latest export version in the current version of %Qtilities. */
    };

    //! The %Qtilities major version number.
    #define qti_def_VERSION_MAJOR 1
    //! The %Qtilities minor version number.
    #define qti_def_VERSION_MINOR 0
    //! The %Qtilities revision version number.
    #define qti_def_VERSION_REVISION 0
    //! The %Qtilities beta number.
    #define qti_def_VERSION_BETA 0
    //! The %Qtilities alpha number.
    #define qti_def_VERSION_ALPHA 0
}

#endif // QTILITIES_H
