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

#ifndef FILE_LOCKER_H
#define FILE_LOCKER_H

#include "QtilitiesCore_global.h"

#include <QDateTime>

namespace Qtilities {
    namespace Core {
        /*!
        \class FileLocker
        \brief The FileLocker class provides a simple mechanism to create and manage file locking using a .lck file with lock information.

        The .lck file created when locking a file will contain the date and time that the lock was created,
        as well as the hostname of the computer used to lock the file.

        <i>This class was added in %Qtilities v1.2.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT FileLocker {

        public:
            //! Default constructor.
            /*!
             * \param lock_extension The file extension to use for lock files.
             */
            FileLocker(const QString& lock_extension = "lck");
            virtual ~FileLocker() {}

            // -----------------------------------
            // Control Over Lock
            // -----------------------------------
            //! Checks if the given file is locked.
            virtual bool isFileLocked(const QString& file_path) const;
            //! Locks a file.
            /*!
             * This function will create a .lck file with the same name as the file name provided. If
             * the file is already locked, or the .lck file can't be created for some reason, it will return false
             * and set the errorMsg parameter to the reason why the lock failed.
             *
             * \param file_path The path of the file that must be locked.
             * \param errorMsg When valid, will be populated with error message if the function fails.
             * \returns True if successfull, false otherwise.
             *
             */
            virtual bool lockFile(const QString& file_path, QString* errorMsg = 0);
            //! Unlocks a file.
            /*!
             * This function will look for a .lck file with the same name as the file name provided. If
             * the file exists, it means that the file was locked and this function will attempt to lock it. If the \p .lck
             * file can't be removed for some reason, it will return false
             * and set the errorMsg parameter to the reason why the lock failed.
             *
             * \param file_path The path of the file that must be locked.
             * \param errorMsg When valid, will be populated with error message if the function fails.
             * \returns True if successfull, false otherwise.
             *
             */
            virtual bool unlockFile(const QString& file_path, QString* errorMsg = 0);

            // -----------------------------------
            // Details About A Lock
            // -----------------------------------
            //! Gets the host name that was used to lock a file.
            /*!
             * \param file_path The path of the file that is locked, not the lock file itself.
             * \param errorMsg When valid, will be populated with error message if the function fails.
             * \return The last lock host name.
             */
            virtual QString lastLockHostName(QString file_path, QString *errorMsg = 0) const;
            //! Gets the date and time when a lock was created.
            /*!
             * \param file_path The path of the file that is locked, not the lock file itself.
             * \param errorMsg When valid, will be populated with error message if the function fails.
             * \return The last lock date and time.
             */
            virtual QString lastLockDateTime(QString file_path, QString *errorMsg = 0) const;
            //! Provides a lock summary string for a lock on a file.
            /*!
             * \param file_path The path of the file that is locked, not the lock file itself.
             * \param line_break_char The line break characters to use in the summary text.
             * \param errorMsg When valid, will be populated with error message if the function fails.
             * \return The last lock summary.
             */
            virtual QString lastLockSummary(QString file_path, const QString& line_break_char = "\n", QString *errorMsg = 0) const;

        private:
            //! Returns the expected lock file path for a given file path.
            QString lockFilePathForFile(const QString& file_path) const;

            //! The extension used for the lock file.
            QString d_lock_extension;
        };
    }
}

#endif // FILE_LOCKER_H
