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

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        /*!
        \class FileUtils
        \brief The FileUtils class provides commonly used file related functionality.
          */
        class QTILIITES_CORE_SHARED_EXPORT FileUtils
        {
        public:
            //! Removes the specified directory along with all of its contents.
            /*!
               \param dirName Path of directory to remove.
               \return true on success; false on error.
              */
            static bool removeDir(const QString &dirName);
            //! Appends a string to the file name part of the file, that is after the file's name and before the extension.
            static QString appendToFileName(const QString &fullFileName, const QString& append_string);
            //! Removes the specified number of characters from the end of the file name part of the file, that is after the file's name and before the extension.
            static QString removeFromFileName(const QString &fullFileName, int len);
        };
    }
}

#endif // FILEUTILS_H
