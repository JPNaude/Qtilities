/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

            FileUtils(bool enable_tasking = true, QObject* parent = 0);
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
\endcode

                It is possible to find just directories by specifying the filters to be QDir::AllDirs without including QDir::Files.

               \param dirName Path of directory to search under.
               \param file_filters The list of files which must be returned, when empty all files are returned. For example: *.bit *.log *.ngc, when empty all files added by default (*.*).
               \param ignore_list Files which should be ignored. Must be in the format: *.svn *.bak *.tmp
               \param filters The QDir::Filters to apply when searching for files and folders.
               \param sort The QDir::SortFlags to apply when searching for files and folders.
               \param first_run Ignore this parameter, used in recursive operations.
               \return A list of QFileInfos containing the information about found files.
              */
            QFileInfoList findFilesUnderDir(const QString &dirName,
                                            const QString& file_filters = QString(),
                                            const QString& ignore_list = QString(),
                                            QDir::Filters filters = QDir::Files | QDir::NoDotAndDotDot,
                                            QDir::SortFlags sort = QDir::NoSort,
                                            bool first_run = true);
            //! Returns the last QFileInfoList produced by fileFilesUnderDir().
            QFileInfoList lastFilesUnderDir();
            //! Sets up the paramaters for future findFilesUnderDir() runs.
            /*!
               \param dirName Path of directory to search under.
               \param file_filters The list of files which must be returned, when empty all files are returned. For example: *.bit *.log *.ngc , when empty all files added by default (*.*).
               \param ignore_list Files which should be ignored. Must be in the format: *.svn *.bak *.tmp
               \param filters The QDir::Filters to apply when searching for files and folders.
               \param sort The QDir::SortFlags to apply when searching for files and folders.
               \param first_run Ignore this parameter, used in recursive operations.
              */
            void setFindFilesUnderDirParams(const QString &dirName,
                                            const QString& file_filters = QString(),
                                            const QString& ignore_list = QString(),
                                            QDir::Filters filters = QDir::Files | QDir::NoDotAndDotDot,
                                            QDir::SortFlags sort = QDir::NoSort);
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
              If something went wrong (for example if the file does not exist), -1 is returned as an error code.
              */
            static int fileHashCode(const QString& file);
            //! Compares two files and returns true if they are exactly the same, false otherwise.
            static bool compareFiles(const QString& file1, const QString& file2);
            //! Compares two paths in a system independant way.
            /*!
              This function takes two paths and checks if they are the same. The function does the following:
              - Does a case insensitive check.
              - Removes any unwanted things in the path through QDir::cleanPath().
              - Does an environment independent check by converting both paths to the native characters of the OS on which this function is called.

              \return True when the paths are the same, false otherwise.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            static bool comparePaths(const QString& path1, const QString& path2);
            //! Converts a path to the native format of the underlying OS.
            /*!
              This function is similar to QDir::toNativeSeparators(), but it supports linux as well.

              <i>This function was added in %Qtilities v1.2.</i>
              */
            static QString toNativeSeparators(QString path);
            //! Makes a local copy of the specified resource file.
            /*!
              This function will create a local copy of a resource file.

              \param resource_path The resource to make a copy of.
              \param local_path The target local path for the resource.
              \param errorMsg When specified, will be populated by any error messages when the function returns false.
              \param local_permissions The permissions to set on the local file after the copy process has been completed.

              <i>This function was added in %Qtilities v1.3.</i>
              */
            static bool makeLocalCopyOfResource(const QString& resource_path,
                                                const QString& local_path,
                                                QString* errorMsg,
                                                QFile::Permissions local_permissions = QFile::ReadOwner | QFile::WriteOwner);

        private:
            FileUtilsPrivateData* d;
        };
    }
}

#endif // FILEUTILS_H
