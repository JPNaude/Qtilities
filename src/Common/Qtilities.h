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
    #define QTILITIES_VERSION_MINOR 3
    //! The %Qtilities revision version number.
    #define QTILITIES_VERSION_REVISION 0
    //! The %Qtilities beta number.
    #define QTILITIES_VERSION_BETA 0
    //! The %Qtilities alpha number.
    #define QTILITIES_VERSION_ALPHA 0

    //! The %Qtilities binary export formats version
    /*!
    The binary export format defines the binary format used by the classes implementing Qtilities::Core::Interfaces::IExportable.

    Binary exports are not backwards compatible at this stage.

    History: <br>
    0 - %Qtilities v0.1 Beta 1 <br>
    1 - %Qtilities v0.1 <br>
    2 - %Qtilities v0.2 <br>
        - Observer hints are not exported by default anymore. They now provide a hint to observers which checks this hint during export.
        - Observers now export their memory of category access modes using the new QtilitiesCategory class.<br>

    3 - %Qtilities v0.3 onwards<br>
        - ActivityPolicyFilter::parentTrackingPolicy() now part of activity filter exports.
        - ObserverHints::modificationStateDisplayHint() now part of observer hints exports.
    */
    #define QTILITIES_BINARY_EXPORT_FORMAT 3

    //! The shortcut export format version.
    /*!
    History:<br>
    No version defined - %Qtilities v0.1 Beta 1<br>
    1 - %Qtililties v0.1 onwards
    */
    #define QTILITIES_SHORTCUT_EXPORT_FORMAT 1

    //! The %Qtilities logger's binary export format.
    /*!
    The logger binary export format defines the binary format used by the classes implementing Qtilities::Logging::Interfaces::ILoggerExportable.

    History:<br>
    No version defined - %Qtilities v0.1 Beta 1<br>
    1 - %Qtilities v0.1<br>
    2 - %Qtilities v0.2 onwards<br>
    */
    #define QTILITIES_LOGGER_BINARY_EXPORT_FORMAT 2

    //! The %Qtilities XML export format.
    /*!
    The XML format defines the format which is used to export XML trees.

    History:<br>
    XML exporting was introduced in %Qtilities v0.2<br>
    1 - %Qtililties v0.2 onwards<br>
    */
    #define QTILITIES_XML_EXPORT_FORMAT 1

    //! The %Qtilities Extension System plugin configuration export format.
    /*!
    The plugin configuration format defines the format which is used to export plugin configuration set files.

    History:<br>
    Plugin configuration sets exporting was introduced in %Qtilities v0.3<br>
    1 - %Qtililties v0.3 onwards<br>
    */
    #define QTILITIES_PLUGIN_CONFIG_FORMAT 1
}

#endif // QTILITIES_H
