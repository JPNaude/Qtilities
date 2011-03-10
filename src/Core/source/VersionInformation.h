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

#ifndef VERSION_INFORMATION_H
#define VERSION_INFORMATION_H

#include <QString>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        // ------------------------------------
        // VersionNumber
        // ------------------------------------
        /*!
        \struct VersionNumberPrivateData
        \brief Structure used by VersionNumber to store private data.
          */
        struct VersionNumberPrivateData;

        /*!
        \class VersionNumber
        \brief VersionNumber represents a single version number.

        VersionNumber represents a version on three levels:
        - Major
        - Minor
        - Revision

        For Minor and Revision numbers you can specify the fieldWidth using setFieldWidthMinor() and setFieldWidthRevision() respectively.

        <i>This class was added in %Qtilities v0.3.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT VersionNumber
        {
        public:
            VersionNumber();
            VersionNumber(int major = 0, int minor = 0, int revision = 0);
            VersionNumber(const QString& version, const QString& seperator = ".");
            VersionNumber(const VersionNumber& ref);
            virtual ~VersionNumber();

            //! Operator overload to compare two VersionNumber objects with each other.
            bool operator==(const VersionNumber& ref) const;
            //! Operator overload to compare two VersionNumber objects are not equal.
            bool operator!=(const VersionNumber& ref) const;
            //! Operator overload to assign one VersionNumber to another VersionNumber.
            void operator=(const VersionNumber& ref);
            //! Operator overload to check if one VersionNumber to bigger than another VersionNumber.
            bool operator>(const VersionNumber& ref);
            //! Operator overload to check if one VersionNumber to bigger than or equal to another VersionNumber.
            bool operator>=(const VersionNumber& ref);
            //! Operator overload to check if one VersionNumber to smaller than another VersionNumber.
            bool operator<(const VersionNumber& ref);
            //! Operator overload to check if one VersionNumber to smaller than or equal to another VersionNumber.
            bool operator<=(const VersionNumber& ref);

            //! Gets the major version.
            int versionMajor() const;
            //! Sets the major version.
            void setVersionMajor(int version);

            //! Gets the minor version.
            int versionMinor() const;
            //! Sets the minor version.
            void setVersionMinor(int version);
            //! Gets the minor version's field width.
            /*!
              By default this is not defined and not taken into account in string representations of this version number.
              */
            int fieldWidthMinor() const;
            //! Sets the minor version's field width.
            void setFieldWidthMinor(int field_width);
            //! Gets if the minor version is part of the version.
            bool isVersionMinorUsed() const;
            //! Sets if the minor version is part of the version.
            void setIsVersionMinorUsed(bool is_used);

            //! Gets the revision version.
            int versionRevision() const;
            //! Sets the revision version.
            void setVersionRevision(int version);
            //! Gets the revision version's field width.
            int fieldWidthRevision() const;
            //! Sets the revision version's field width.
            /*!
              By default this is not defined and not taken into account in string representations of this version number.
              */
            void setFieldWidthRevision(int field_width);
            //! Gets if the minor version is part of the version.
            bool isVersionRevisionUsed() const;
            //! Sets if the minor version is part of the version.
            void setIsVersionRevisionUsed(bool is_used);

            //! Returns a string represenation of the complete VersionNumber, thus the major, minor and revision parts of the version.
            /*!
              \param seperator By default this is a point, thus ".". In some cases it is desirable to use a custom field seperator. For example as an underscore "_" can be desirable when the version information must be appended to a file name.
              */
            virtual QString toString(const QString& seperator = ".") const;
            //! Gets the version information from a string represenation, thus the major, minor and revision parts of the version.
            /*!
              \param version The version string.
              \param seperator By default this is a point, thus ".". In some cases it is desirable to use a custom field seperator. For example as an underscore "_" can be desirable when the version information must be appended to a file name.

              This function keeps the used parts of the version, thus it does not change that.
              */
            virtual void fromString(const QString& version, const QString& seperator = ".");

        private:
            VersionNumberPrivateData* d;
        };

        // ------------------------------------
        // VersionInformation
        // ------------------------------------
        /*!
        \struct VersionInformationPrivateData
        \brief Structure used by VersionInformation to store private data.
          */
        struct VersionInformationPrivateData;

        /*!
        \class VersionInformation
        \brief VersionInformation provides version information about a specific instance, for example a plugin.

        The goal of VersionInformation is to design a lightweigth class which can be used to make specifying version related information easier.

        <i>This class was added in %Qtilities v0.3.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT VersionInformation
        {
        public:
            VersionInformation(const VersionNumber& version, QList<VersionNumber> supported_versions);
            VersionInformation(int major, int minor = 0, int revision = 0);
            VersionInformation(const VersionInformation& ref);
            virtual ~VersionInformation();

            //! Returns the current version.
            VersionNumber version() const;
            //! Returns the supported versions by this version.
            QList<VersionNumber> supportedVersions() const;
            //! Indicates if supported versions are available.
            bool hasSupportedVersions() const;
            //! Adds a version number to the list of supported version.
            void addSupportedVersion(const VersionNumber& version_number);
            //! Indicates if a specific version number is supported by this VersionInformation object.
            bool isSupportedVersion(const VersionNumber& version_number) const;
            //! Indicates if a specific version number is supported by this VersionInformation object by providing a version string.
            /*!
              This function will construct a VersionNumber object from the string and check if it is supported.
              */
            bool isSupportedVersion(const QString& version_string, const QString& seperator = ".") const;
            //! Returns a QStringList with the string representations of all supported versions.
            QStringList supportedVersionString() const;
            //! Overload << operator so more supported versions can be streamed easily.
            inline VersionInformation& operator<<(const VersionNumber& version_number)
            {
               addSupportedVersion(version_number);
               return *this;
            }

        private:
            VersionInformationPrivateData* d;
        };
    }
}

#endif // VERSION_INFORMATION_H
