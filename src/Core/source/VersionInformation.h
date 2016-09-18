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
        \brief VersionNumber makes it easy to work with version numbers.

        The VersionNumber class makes it easy to work with version numbers. It allows functions to compare versions
        with each other, functions to parse version strings and to control which parts of the version number should be used.

        The VersionNumber class represents a version on three levels:
        - Major
        - Minor
        - Revision
        - Service Packs
        - Release Candidates
        - Betas

        For example, to construct a version number for v1.0.3 you would do something like this:

\code
VersionNumber number(1,0,3);

// We can now print it like this: 1.0.3
number.toString();

// Or we can print it like this: 1.0.003
number.setFieldWidthRevision(3);
number.toString();

// If we want to use the version number in a file name, it might be usefull to print it like this: 1_0_003
number.toString("_");
\endcode

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

        For Minor and Revision numbers you can specify the fieldWidth of the QString::number() conversion used using setFieldWidthMinor() and setFieldWidthRevision() respectively. It is also
        possible to control which parts of a version number is used using setIsVersionMinorUsed() and setIsVersionRevisionUsed().

        \subsection Development Stages

        Since %Qtilities v1.5, VersionNumber allows development stages to be included as part of the version number. Note that any version number can only have one development stage associated with it.
        The development stages are defined by the DevelopmentStage enumeration, supporting the following development stages:

        - None
        - Alpha
        - Beta
        - Release Candidate
        - Service Pack

        When considering string representations of version numbers, VersionNumber will assume that the development stage is mentioned at the end of the version number. For
        example: v1.1.1-b2 is considered to be v1.1.1 beta 2, or v1.1-rc1 will be considered v1.1 release candidate 1 (note that the revision number is not used in this case using
        setIsVersionRevisionUsed() set to false).

        The following examples show how different VersionNumber object with indications of development stages can be constructed:
\code
# Example 1: To represent v1.5.1-rc2
VersionNumber example1(1,5,1,2,DevelopmentStageReleaseCandidate);
VersionNumber example1_from_string("1.5.1-rc2",".",DevelopmentStageReleaseCandidate); # Note VersionNumber will use the default string identifier '-rc' to determine the development stage version.

# Example 2: To represent v1.5sp2 (v1.5 Service Pack 1)
VersionNumber example2(1,5,0,2,DevelopmentStageServicePack);
example2.setIsVersionRevisionUsed(false);
example2.setDevelopmentStageIdentifier("sp"); # Note that we are not using the default '-sp' service pack idenfier, therefore we must specify 'sp'.

VersionNumber example2_from_string("1.5sp2",".",DevelopmentStageServicePack,"sp"); # Note that we are not using the default '-sp' service pack idenfier, therefore we must specify 'sp'.
example2_from_string.setIsVersionRevisionUsed(false);
\endcode

        \sa VersionInformation

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT VersionNumber
        {
        public:
            /*!
             * \brief The DevelopmentStage enum defines the type of development stages which can be associated with a version number represented using a VersionNumber object.
             *
             * \note When comparing if a version number with a development stage is bigger than another version with a development stage, the order of the items in the enum
             * is used, where DevelopmentStageAlpha is considered the earliest release, and the DevelopmentStageServicePack the latest release. For example, v1.1-a1 is smaller
             * than v1.1-rc1, or v1.1 is bigger than v1.1-rc1. In addition, v1.1-sp1 is bigger then v1.1 etc.
             */
            enum DevelopmentStage {
                DevelopmentStageAlpha,              /*!< The version number represents an alpha release. Default string identifier is '-a'. */
                DevelopmentStageBeta,               /*!< The version number represents a beta release. Default string identifier is '-b'. */
                DevelopmentStageReleaseCandidate,   /*!< The version number represents a release candidate. Default string identifier is '-rc'. */
                DevelopmentStageNone,               /*!< No development stage is associated with the version number (the default). */
                DevelopmentStageServicePack         /*!< The version number represents a service pack candidate. Default string identifier is '-sp'. */
            };

            VersionNumber();
            /*!
             * Constructs a version number given a major, minor and revision number combination. In addition, a string identifier number and type can be specified.
             * \param major The major version number.
             * \param minor The minor version number.
             * \param revision The revision number.
             */
            VersionNumber(int major, int minor = 0, int revision = 0, int stage_version = 0, DevelopmentStage development_stage = DevelopmentStageNone);
            VersionNumber(const QString& version, const QString& separator = ".", DevelopmentStage develoment_stage = DevelopmentStageNone, const QString& stage_identifier = QString());
            VersionNumber(const VersionNumber& ref);
            virtual ~VersionNumber();

            //! Operator overload to compare two VersionNumber objects with each other.
            /*!
             * \note Minor and revision numbers are only considered if both VersionNumbers use them.
             */
            bool operator==(const VersionNumber& ref) const;
            //! Operator overload to compare two VersionNumber objects are not equal.
            /*!
             * \note Minor and revision numbers are only considered if both VersionNumbers use them.
             */
            bool operator!=(const VersionNumber& ref) const;
            //! Operator overload to assign one VersionNumber to another VersionNumber.
            VersionNumber& operator=(const VersionNumber& ref);
            //! Operator overload to check if one VersionNumber to bigger than another VersionNumber.
            /*!
             * \note Minor and revision numbers are only considered if both VersionNumbers use them.
             */
            bool operator>(const VersionNumber& ref) const;
            //! Operator overload to check if one VersionNumber to bigger than or equal to another VersionNumber.
            /*!
             * \note Minor and revision numbers are only considered if both VersionNumbers use them.
             */
            bool operator>=(const VersionNumber& ref) const;
            //! Operator overload to check if one VersionNumber to smaller than another VersionNumber.
            /*!
             * \note Minor and revision numbers are only considered if both VersionNumbers use them.
             */
            bool operator<(const VersionNumber& ref) const;
            //! Operator overload to check if one VersionNumber to smaller than or equal to another VersionNumber.
            /*!
             * \note Minor and revision numbers are only considered if both VersionNumbers use them.
             */
            bool operator<=(const VersionNumber& ref) const;

            //! Is null means the version is 0.0.0, this will be the case when object is constructed with default constructor.
            bool isNull() const;

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

            //! Gets the development stage string identifier used for the developmentStage() of this version number.
            /*!
             * \sa setDevelopmentStageIdentifer(), defaultDevelopmentStageIdentifer()
             */
            QString developmentStageIdentifier() const;
            //! Sets the development stage string identifier used for the developmentStage() of this version number.
            /*!
             * \sa developmentStageIdentifer(), defaultDevelopmentStageIdentifer()
             */
            void setDevelopmentStageIdentifier(const QString& identifier);
            //! Returns the default development stage identifier for specified development stage
            static QString defaultDevelopmentStageIdentifer(DevelopmentStage stage);

            //! Gets the development stage of this version number.
            /*!
             * \param check_if_valid When true, versionDevelopmentStage() needs to be valid in order for the associated development stage
             * to be returned. If versionDevelopmentStage() is not valid (version == 0) DevelopmentStageNone will be returned. When false
             * the development stage set using setDevelopmentStage() or set using one of the constructors will be returned even if no valid
             * development stage have been set. This parameter allows you to check the validity of the development stage associated with
             * the version number.
             * \return The development stage associated with this version number.
             */
            DevelopmentStage developmentStage(bool check_if_valid = false) const;
            //! Sets the development stage of this version number.
            void setDevelopmentStage(DevelopmentStage stage);
            //! Gets the development stage version.
            int versionDevelopmentStage() const;
            //! Sets the development stage version.
            void setVersionDevelopmentStage(int version);

            //! Returns a string represenation of the complete VersionNumber, thus the major, minor and revision parts of the version.
            /*!
              \param separator By default this is a point, thus ".". In some cases it is desirable to use a custom field separator. For example as an underscore "_" can be desirable when the version information must be appended to a file name.
              */
            virtual QString toString(const QString& separator = ".") const;
            //! Gets the version information from a string represenation, thus the major, minor and revision parts of the version.
            /*!
              \param version The version string.
              \param separator By default this is a point, thus ".". In some cases it is desirable to use a custom field separator. For example as an underscore "_" can be desirable when the version information must be appended to a file name.

              This function keeps the used parts of the version, thus it does not change that.
              */
            virtual void fromString(const QString& version, const QString& separator = ".", const QString& stage_identifier = QString(), DevelopmentStage stage_type = DevelopmentStageNone);

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
            bool isSupportedVersion(const QString& version_string, const QString& separator = ".") const;
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
