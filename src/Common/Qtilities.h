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

#ifndef QTILITIES_H
#define QTILITIES_H

namespace Qtilities {
    //! The %Qtilities major version number.
    #define QTILITIES_VERSION_MAJOR 0
    //! The %Qtilities minor version number.
    #define QTILITIES_VERSION_MINOR 2
    //! The %Qtilities beta number.
    #define QTILITIES_VERSION_BETA 0
    //! The %Qtilities alpha number.
    #define QTILITIES_VERSION_ALPHA 0

    //! The %Qtilities binary export formats version
    /*!
    The binary export format indicates how binary exports are done in %Qtilities. The export format
    defines the binary format used by the following functions and classes which implements
    Qtilities::Core::Interfaces::IExportable:
    <b>Classes:</b>
    - Qtilities::Core::Observer
    - Qtilities::Core::ObserverRelationalTable
    - Qtilities::Core::ObserverData

    <b>Functions (corresponding import functions not shown here):</b>
    - Qtilities::Core::ObjectManager::exportObserverBinary()
    - Qtilities::Core::ObjectManager::exportObjectProperties()
    - Qtilities::ProjectManagement::Project::saveProject()

    Binary exports is not backwards compatible at this stage.

    History:
    0 - %Qtilities v0.1 Beta 1
    1 - %Qtilities v0.1
    2 - %Qtilities v0.2
         The following changes were part of the new v2 format:
         - Observer hints are not exported by default anymore. They now provide a hint to observers which checks this hint during export.
         - Observers now export their memory of category access modes using the new QtilitiesCategory class.
    */
    #define QTILITIES_BINARY_EXPORT_FORMAT 2

    //! The shortcut export format version.
    /*!
    History:
    No version defined - %Qtilities v0.1 Beta 1
    1 - %Qtililties v0.1-v0.2 onwards
    */
    #define QTILITIES_SHORTCUT_EXPORT_FORMAT 1

    //! The %Qtilities binary export format.
    /*!
    The binary export format indicates how binary exports are done in the %Qtilities logging module.
    The export format defines the binary format used by the following functions and classes which implements
    Qtilities::Core::Interfaces::ILoggerExportable:
    <b>Classes:</b>
    - Qtilities::Logging::Logger

    History:
    No version defined - %Qtilities v0.1 Beta 1
    1 - %Qtilities v0.1
    2 - %Qtilities v0.2
    */
    #define QTILITIES_LOGGER_BINARY_EXPORT_FORMAT 2

    //! The %Qtilities XML export format.
    /*!
    The XML format defines the format which is used to export XML trees.

    History:
    XML exporting was introduced in %Qtilities v0.2
    1 - %Qtililties v0.2
    */
    #define QTILITIES_XML_EXPORT_FORMAT 1

}

#endif // QTILITIES_H
