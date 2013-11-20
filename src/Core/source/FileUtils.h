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

// All text files under /my_path:
QFileInfoList files = fu.findFilesUnderDir("/my_path","*.txt");
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
              - If both paths exists, QFileInfo's == operator overload is used.

              If both files does not exist, the following check is done:
              - Does a case insensitive check.
              - Removes any unwanted things in the path through QDir::cleanPath().
              - Does an environment independent check by converting both paths to the native characters of the OS on which this function is called.

              \param path1 The first path to check.
              \param path2 The second path to check against path1.
              \param cs The case sensitivity of the check. This only applies to Windows. Only Unix based systems Qt::CaseSensitive is always used.

              \return True when the paths are the same, false otherwise.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            static bool comparePaths(const QString& path1, const QString& path2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
            //! Check if one path starts with another path (does it checks if the one path is a parent of another path).
            /*!
              \param child_path The child path.
              \param parent_path The parent path.
              \param cs The case sensitivity of the check. This only applies to Windows. Only Unix based systems Qt::CaseSensitive is always used.

              \return True when child_path starts with parent_path.

              <i>This function was added in %Qtilities v1.2.</i>
              */
            static bool pathStartsWith(const QString& child_path, const QString& parent_path, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
            //! Converts a path to the native format of the underlying OS.
            /*!
              This function is similar to QDir::toNativeSeparators(), but it supports linux as well.

              <i>This function was added in %Qtilities v1.2.</i>
              */
            static QString toNativeSeparators(QString path);
            //! Makes a local copy of the specified resource file.
            /*!
              This function will create a local copy of a resource file.

              \note From %Qtilities v1.5 onwards, This function will also create the local_path directory using QDir::mkpath() if it does not exist.

              \param resource_path The resource to make a copy of.
              \param local_path The target local path for the resource.
              \param errorMsg When specified, will be populated by any error messages when the function returns false.
              \param local_permissions The permissions to set on the local file after the copy process has been completed.

              <i>This function was added in %Qtilities v1.3.</i>
              */
            static bool makeLocalCopyOfResource(const QString& resource_path,
                                                const QString& local_path,
                                                QString* errorMsg = 0,
                                                QFile::Permissions local_permissions = QFile::ReadOwner | QFile::WriteOwner);
            //! Convenience function that returns the given path as an Unix formatted path.
            /*!
              Returns the give path as an Unix formatted path, that is, the path will be cleaned and all separators will be in Linux format.

              \param path The path to convert.
              \returns The unix version of the given path.

              <i>This function was added in %Qtilities v1.5.</i>
              */
            static QString toUnixPath(const QString& path);
            //! Convenience function that writes the given string to the file at the given file path.
            /*!
              This function is a convenience function to write file_contents to the file at file_path. If
              the file does not exist it will be created. If it does exist, it will be overwritten.

              If for any reason the operation cannot be completed, it will return false and set errorMsg with
              the reason why it failed.

              \param file_path The path of the file.
              \param file_contents The contents that should be written to the file.
              \param errorMsg When the function fails, errorMsg will contain a reason why it failed.
              \returns True when successfull, false otherwise.

              \sa readTextFile()

              <i>This function was added in %Qtilities v1.5.</i>
              */
            static bool writeTextFile(const QString& file_path,
                                      const QString &file_contents,
                                      QString* errorMsg = 0);
            //! Convenience function that reads the contents of a text file.
            /*!
              This function is a convenience function to read the contents of a text file at file_path. If
              the file cannot be opened as a text file, the function will fail and errorMsg will be set
              with an appropriate error message.

              \param file_path The path of the file.
              \param ok Will be set to false if the function failed.
              \param errorMsg When the function fails, errorMsg will contain a reason why it failed.
              \returns The contents of the file when the function completed successfully, an empty string otherwise.

              \sa writeTextFile()

              <i>This function was added in %Qtilities v1.5.</i>
              */
            static QString readTextFile(const QString& file_path,
                                        bool *ok = 0,
                                        QString* errorMsg = 0);

        private:
            FileUtilsPrivateData* d;
        };
    }
}

#endif // FILEUTILS_H
