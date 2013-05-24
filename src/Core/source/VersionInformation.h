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

        The VersionNumber class represents a version on three levels:
        - Major
        - Minor
        - Revision

        For example, to construct a version number for v1.0.3 you would do something like this:

\code
VersionNumber number(1,0,3);

// We can now print it like this: 1.0.3
number.toString();

// Or we can print it like this: 1.0.003
number.setFieldWidthRevision(3);
number.toString();

// If we want to use the version number in a file name, it might be usefull to print it like: 1_0_003
number.toString("_");
\endcode

        For Minor and Revision numbers you can specify the fieldWidth using setFieldWidthMinor() and setFieldWidthRevision() respectively.

        VersionNumber provides a range of functions to compare version numbers with each other. For example:
\code
VersionNumber ver0(4,4,4);
VersionNumber ver1(5,5,5);

bool is_bigger_or_equal = (ver0 >= ver1); // False
bool is_bigger = (ver0 > ver1); // False
bool is_equal = (ver0 == ver1); // False
bool is_not_equal = (ver0 != ver1); // True
bool is_smaller_and_equal = (ver0 <= ver1); // True
bool is_smaller = (ver0 < ver1); // True
\endcode

        \sa VersionInformation

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT VersionNumber
        {
        public:
            VersionNumber();
            VersionNumber(int major, int minor = 0, int revision = 0);
            VersionNumber(const QString& version, const QString& seperator = ".");
            VersionNumber(const VersionNumber& ref);
            virtual ~VersionNumber();

            //! Operator overload to compare two VersionNumber objects with each other.
            bool operator==(const VersionNumber& ref) const;
            //! Operator overload to compare two VersionNumber objects are not equal.
            bool operator!=(const VersionNumber& ref) const;
            //! Operator overload to assign one VersionNumber to another VersionNumber.
            VersionNumber& operator=(const VersionNumber& ref);
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

        The goal of VersionInformation is to design a lightweigth class which can be used to specify version related information easier.

        For example we can create information about a specific version, say v1.0.3 like this:

\code
VersionInformation version_info(1,0,3);

// We can now add versions that this version depend on. For exmaple if you have
// a plugin that depends on a specific version of an application, say v2.0.0, its specified like this:
version_info << VersionNumber(2,0,0);
\endcode

        A range of functions are provided to work with supported versions: hasSupportedVersion(), addSupportedVersion(), isSupportedVersion() etc. The Qtilities::ExtensionSystem::Intefaces::IPlugin interface is a practical example where VersionInformation is used in the way shown above.

        \sa VersionNumber

        <i>This class was added in %Qtilities v1.0.</i>
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
