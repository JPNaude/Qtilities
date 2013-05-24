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

#ifndef QTILITIESFILEINFO_H
#define QTILITIESFILEINFO_H

#include <QFileInfo>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        /*!
        \struct QtilitiesFileInfoPrivateData
        \brief Structure used by QtilitiesFileInfo to store private data.
          */
        struct QtilitiesFileInfoPrivateData;

        /*!
        \class QtilitiesFileInfo
        \brief QtilitiesFileInfo is a class which extends QFileInfo with additional functionality.

        QtilitiesFileInfo extends the way that QFileInfo handles relative files so that the path which relative to files are relative to can be specified. Normally QFileInfo::makeAbsolute() will make relative to paths relative to the application path. QtilitiesFileInfo allows you to specify the relative to path using setRelativeToPath() and provides a relativeToPath() access function. It is also possible to set the relativeToPath() as part of the constructor.

        The actualPath() and actualFilePath() functions allows you to get the actual path of a file without needing to know if it is relative or not (for relative files, a relativeToPath must be specified to get the actual path).

        Additional extended functions include:
        - The ability to set the file name part only using setFileName().
        - The ability to check if a file name or path is valid.
        - The ability to specifiy a custom relative to path to use for the file.

        The following example shows how the class works and you will notice that it is very close to QFileInfo, except for the additional functionality:

\code
// Test 1: c:/relative_to_path/relative_path/file_name.tar.gz
QtilitiesFileInfo file_info("c:/relative_to_path/relative_path/file_name.tar.gz");
isAbsolute:  true
isRelative:  false
path:  "c:/relative_to_path/relative_path"
filePath:  "c:/relative_to_path/relative_path/file_name.tar.gz"
absolutePath:  "C:/relative_to_path/relative_path"
canonicalPath:  ""
relativeToPath: ""
absoluteToRelativePath:  ""
actualPath: "c:/relative_to_path/relative_path"
baseName:  "file_name"
completeBaseName:  "file_name.tar"
suffix:  "gz"
completeSuffix:  "tar.gz"

// Test 2: relative_path/file_name.tar.gz
file_info.setFile("relative_path/file_name.tar.gz");
isAbsolute:  false
isRelative:  true
path:  "relative_path"
filePath:  "relative_path/file_name.tar.gz"
absolutePath:  "D:/projects/Qt/Qtilities/trunk/bin/Examples/ConsoleLogging/relative_path"
canonicalPath:  ""
relativeToPath: ""
absoluteToRelativePath:  "/relative_path"
actualPath: "/relative_path"
baseName:  "file_name"
completeBaseName:  "file_name.tar"
suffix:  "gz"
completeSuffix:  "tar.gz"

// Test 3: relative_path/file_name.tar.gz with relative to path: c:/relative_to_path
file_info.setFile("relative_path/file_name.tar.gz");
file_info.setRelativeToPath("c:/relative_to_path");
isAbsolute:  false
isRelative:  true
path:  "relative_path"
filePath:  "relative_path/file_name.tar.gz"
absolutePath:  "D:/projects/Qt/Qtilities/trunk/bin/Examples/ConsoleLogging/relative_path"
canonicalPath:  ""
relativeToPath: "c:/relative_to_path"
absoluteToRelativePath:  "c:/relative_to_path/relative_path"
actualPath: "c:/relative_to_path/relative_path"
baseName:  "file_name"
completeBaseName:  "file_name.tar"
suffix:  "gz"
completeSuffix:  "tar.gz"

// Test 4: ../relative_path/file_name.tar.gz with relative to path: c:/relative_to_path
file_info.setFile("../relative_path/file_name.tar.gz");
file_info.setRelativeToPath("c:/relative_to_path");
isAbsolute:  false
isRelative:  true
path:  "../relative_path"
filePath:  "../relative_path/file_name.tar.gz"
absolutePath:  "D:/projects/Qt/Qtilities/trunk/bin/Examples/relative_path"
canonicalPath:  ""
relativeToPath: "c:/relative_to_path"
absoluteToRelativePath:  "c:/relative_path"
actualPath: "c:/relative_path"
baseName:  "file_name"
completeBaseName:  "file_name.tar"
suffix:  "gz"
completeSuffix:  "tar.gz"

// Test 5: ./relative_path/file_name.tar.gz with relative to path: c:/relative_to_path
file_info.setFile("./relative_path/file_name.tar.gz");
file_info.setRelativeToPath("c:/relative_to_path");
isAbsolute:  false
isRelative:  true
path:  "./relative_path"
filePath:  "./relative_path/file_name.tar.gz"
absolutePath:  "D:/projects/Qt/Qtilities/trunk/bin/Examples/ConsoleLogging/relat
ive_path"
canonicalPath:  ""
relativeToPath: "c:/relative_to_path"
absoluteToRelativePath:  "c:/relative_to_path/relative_path"
actualPath: "c:/relative_to_path/relative_path"
baseName:  "file_name"
completeBaseName:  "file_name.tar"
suffix:  "gz"
completeSuffix:  "tar.gz"

// Test 6: .././relative_path/file_name.tar.gz with relative to path: c:/relative_to_path"
file_info.setFile(".././relative_path/file_name.tar.gz");
file_info.setRelativeToPath("c:/relative_to_path");
isAbsolute:  false
isRelative:  true
path:  ".././relative_path"
filePath:  ".././relative_path/file_name.tar.gz"
absolutePath:  "D:/projects/Qt/Qtilities/trunk/bin/Examples/relative_path"
canonicalPath:  ""
relativeToPath: "c:/relative_to_path"
absoluteToRelativePath:  "c:/relative_path"
actualPath: "c:/relative_path"
baseName:  "file_name"
completeBaseName:  "file_name.tar"
suffix:  "gz"
completeSuffix:  "tar.gz"
\endcode

        In the same way that QFileInfo handles Unix file names, the above example will always begin with the root, '/', directory.

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesFileInfo : public QFileInfo
        {
        public:
            //! Constructs a new QtilitiesFileInfo that gives information about the given file.
            /*!
              \param file The file for which information is required.
              \param relative_to_path A path that indicates to what \p file is relative to if it is a relative to path.
              */
            QtilitiesFileInfo(const QString& file = QString(), const QString& relative_to_path = QString());
            QtilitiesFileInfo(const QtilitiesFileInfo& ref);
            //! Operator overload to compare two QtilitiesFileInfo objects with each other.
            /*!
              \note This overload does a comparison using the actualFilePath() paths of the two objects being compared.

              <i>This function was added in %Qtilities v1.2.</i>
              */
            bool operator==(const QtilitiesFileInfo& ref) const;
            //! Operator overload to compare two QtilitiesFileInfo objects are not equal.
            /*!
              \note This overload does a comparison using the actualFilePath() paths of the two objects being compared.

              <i>This function was added in %Qtilities v1.2.</i>
              */
            bool operator!=(const QtilitiesFileInfo& ref) const;
            virtual ~QtilitiesFileInfo();

            //! Indicates if a relative to path has been set.
            /*!
              \sa setRelativeToPath(), relativeToPath()
              */
            bool hasRelativeToPath() const;
            //! Gets the relative to path of the file.
            /*!
              \sa setRelativeToPath(), hasRelativeToPath()
              */
            QString relativeToPath() const;
            //! Gets the relative to path of the file.
            /*!
              \sa relativeToPath(), hasRelativeToPath()
              */
            void setRelativeToPath(const QString& relative_to_path);
            //! Gives the path made absolute to relativeToPath().
            /*!
              This function only does something is the file is relative.

              \sa absoluteToRelativeFilePath()
              */
            QString absoluteToRelativePath() const;
            //! Gives the file path made absolute to relativeToPath().
            /*!
              This function only does something is the file is relative.

              \sa absoluteToRelativePath()
              */
            QString absoluteToRelativeFilePath() const;
            //! Sets the the file name part of the file.
            /*!
              \sa QFileInfofileName()
              */
            virtual void setFileName(const QString& new_file_name);
            //! Returns the actual path without you needing to know if the file is relative or absolute.
            /*!
              This function will return the actual path of the file in the following way:
              - If the file is absolute it will just return path().
              - If the file is relative with a relativeToPath() set, it will return absoluteToRelativePath()
              - If the file is relative without a relativeToPath() set, it will just return path() as well.

              \sa actualFilePath()
              */
            QString actualPath() const;
            //! Returns the actual file path without you needing to know if the file is relative or absolute.
            /*!
              This function will return the file actual path of the file in the following way:
              - If the file is absolute it will just return filePath().
              - If the file is relative with a relativeToPath() set, it will return absoluteToRelativeFilePath()
              - If the file is relative without a relativeToPath() set, it will just return filePath() as well.

              \sa actualPath()
              */
            QString actualFilePath() const;

            //! Compares the actual file paths of QtilitiesFileInfo objects and returns true if they are the same, false otherwise.
            bool compareActualFilePaths(const QtilitiesFileInfo& ref) const;

            //! Replaces the relative to path if it matches the old relative to path.
            /*!
             * \brief updateRelateiveToPath
             * \param old_relative_to_path The old relative to path that must be matched in order for the update operation to happen.
             * \param new_relative_to_path The replacement relative to path.
             * \returns True if the update was performed, false otherwise.
             *
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            bool updateRelativeToPath(const QString& old_relative_to_path, const QString& new_relative_to_path);

            //! Indicates if a file name is valid.
            /*!
              Checks if the file name is valid. Thus checks that it does not contain any of the following characters:
\code
\ / : * ? " < > |
\endcode
              */
            static bool isValidFileName(const QString file_name);
            //! Returns the invalid characters which are checked in isValidFileName() in a QString seperated by spaces.
            /*!
              \sa isValidFileisValidFileNamePath()
              */
            static QString invalidFileNameCharacters() {
                return QString("\\ / : * ? \" < > |");
            }
            //! Indicates if a file path is valid.
            /*!
              Checks if the file name is valid. Thus checks that it does not contain any of the following characters:
\code
: * ? " < > |
\endcode
              */
            static bool isValidFilePath(const QString file_path);
            //! Returns the invalid characters which are checked in isValidFilePath() in a QString seperated by spaces.
            /*!
              \sa isValidFilePath()
              */
            static QString invalidFilePathCharacters() {
                return QString(": * ? \" < > |");
            }

        private:
            QtilitiesFileInfoPrivateData* d;
        };
    }
}

#endif // QTILITIESFILEINFO_H
