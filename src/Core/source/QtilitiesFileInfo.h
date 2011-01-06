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

#ifndef QTILITIESFILEINFO_H
#define QTILITIESFILEINFO_H

#include <QFileInfo>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        /*!
        \struct QtilitiesFileInfoData
        \brief Structure used by QtilitiesFileInfo to store private data.
          */
        struct QtilitiesFileInfoData;

        /*!
        \class QtilitiesFileInfo
        \brief QtilitiesFileInfo is a class which extends QFileInfo with additional functionality.

        QtilitiesFileInfo extends the way that QFileInfo handles relative files so that the path which relative to files are
        relative to can be specified. Normally QFileInfo::makeAbsolute() will make relative to paths relative to the application path. QtilitiesFileInfo
        allows you to specify the relative to path using setRelativeToPath() and provides a relativeToPath() access function. It is also possible
        to set the relative to path as part of the constructor.

        Another extension is the ability to set the complete base name using setCompleteBaseName() function.

        <i>This class was added in %Qtilities v0.3.</i>
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
            //virtual ~QtilitiesFileInfo();

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
            void setFileName(const QString& new_file_name);
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

        private:
            QtilitiesFileInfoData* d;
        };
    }
}

#endif // QTILITIESFILEINFO_H
