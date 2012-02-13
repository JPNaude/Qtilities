/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
#include "ITaskContainer.h"
#include "Task.h"

#include <QList>
#include <QUrl>
#include <QDir>
#include <QObject>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        /*!
        \struct FileUtilsPrivateData
        \brief Structure used by FileUtils to store private data.
          */
        struct FileUtilsPrivateData;

        /*!
        \class FileUtils
        \brief The FileUtils class provides commonly used file related functionality.

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT FileUtils : public QObject, public ITaskContainer {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::ITaskContainer)
            Q_ENUMS(ContainedTasks)

        public:
            //! Access names for tasks provided by FileUtils.
            enum ContainedTasks {
                TaskFindFilesUnderDir   = 0  /*!< The task which allows monitoring of the findFilesUnderDir() function's progress. */
            };
            //! ContainedTasks to string conversion function.
            QString taskNameToString(ContainedTasks task_name) const {
                if (task_name == TaskFindFilesUnderDir)
                    return "Finding Files Under Directory";
                return "";
            }

            FileUtils(QObject* parent = 0);
            virtual ~FileUtils();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // Functions for finding files under a directory.
            // --------------------------------
            //! Finds all files in the directory hierarhcy under a directory.
            /*!
               Finds all files under in the complete directory hierarhcy under dirName. The ITask implementation on this object is linked
               to this function's processing. Thus if you call this function, you can monitor the progress of findFilesUnderDir() through
               the ITask interface implementation provided by the TaskFindFilesUnderDir task.

\code
FileUtils fu;

// All non-system and non-hidden files:
QFileInfoList files = fu.findFilesUnderDir("c:/my_path");

// All system and hidden files as well:
QFileInfoList files = fu.findFilesUnderDir("c:/my_path",QDir::System | QDir::Hidden);
\endcode

               \param dirName Path of directory to search under.
               \param ignore_list Files which should be ignored. Must be in the format: *.svn *.bak *.tmp
               \param filters The QDir::Filters to apply when searching for files. All folder related filter options are ignored.
               \param sort The QDir::SortFlags to apply when searching for files.
               \param first_run Ignore this parameter, used in recursive operations.
               \return A list of QUrls containing the files. The QUrls are constructed using QUrl::fromLocalFile().
              */
            QFileInfoList findFilesUnderDir(const QString &dirName, const QString& ignore_list = QString(), QDir::Filters filters = QDir::Files, QDir::SortFlags sort = QDir::NoSort, bool first_run = true);
            //! Returns the last QFileInfoList produced by fileFilesUnderDir().
            QFileInfoList lastFilesUnderDir();
            //! Sets up the paramaters for future findFilesUnderDir() runs.
            void setFindFilesUnderDirParams(const QString &dirName, const QString& ignore_list = QString(), QDir::Filters filters = QDir::Files, QDir::SortFlags sort = QDir::NoSort);
        private slots:
            //! Finds all files in the directory hierarhcy under a directory.
            /*!
              This function uses parameters set through setFindFilesUnderDirParams().
              */
            QFileInfoList findFilesUnderDirLauncher();

        public:
            // --------------------------------
            // Static Convenience Functions
            // --------------------------------
            //! Removes the specified directory along with all of its contents.
            /*!
               \param dirName Path of directory to remove.
               \return true on success, false otherwise.
              */
            static bool removeDir(const QString &dirName);
            //! Appends a string to the file name part of the file, that is after the file's name and before the extension.
            static QString appendToFileName(const QString &fullFileName, const QString& append_string);
            //! Removes the specified number of characters from the end of the file name part of the file, that is after the file's name and before the extension.
            static QString removeFromFileName(const QString &fullFileName, int len);
            //! Calculates a hash code for a text file.
            /*!
              If something went wrong, -1 is returned as an error code.
              */
            static int textFileHashCode(const QString& file);
            //! Compares two text files and returns true if they are exactly the same, false otherwise.
            static bool compareTextFiles(const QString& file1, const QString& file2);
            //! Compares two binary files and returns true if they are exactly the same, false otherwise.
            static bool compareBinaryFiles(const QString& file1, const QString& file2);

        private:
            QFileInfoList                   last_files_under_dir;
            FileUtilsPrivateData*           d;
        };
    }
}

#endif // FILEUTILS_H
