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

#ifndef FILE_SET_INFO_H
#define FILE_SET_INFO_H

#include "QtilitiesCore_global.h"

#include "IExportable.h"
#include "Factory.h"
#include "QtilitiesFileInfo.h"
#include "ITask.h"

#include <QObject>
#include <QStringList>

using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace Core {
        /*!
        \struct FileSetInfoPrivateData
        \brief Structure used by FileSetInfo to store private data.
          */
        struct FileSetInfoPrivateData;

        /*!
        \class FileSetInfo
        \brief The FileSetInfo class provides information about a set of files.

        The FileSetInfo class is an easy to use class which provides information about a set of files. Files can be added through
        addFile() and removed through removeFile(). You can get a list of all files in the set
        through files(), and check for a specific file through hasFile().

        FileSetInfo provides the following funcationality on the files which are part of the set:
        - Ability to generate a hash code for all files contained in the file set. See fileSetHash().
        - Notifications when any files in the set was modified, renamed or removed from disk. See fileChanged() and setChanged().
        - The ability to save the state of the file set and compare it against the files in the set at a later time.

        <i>This class was added in %Qtilities v1.2.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT FileSetInfo : public QObject, public IExportable {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)

        public:
            FileSetInfo(QObject* parent = 0);
            FileSetInfo(const FileSetInfo& other);
            FileSetInfo& operator=(const FileSetInfo& other);
            virtual ~FileSetInfo();

            // -----------------------------------
            // Control over the files in the set.
            // -----------------------------------
            //! Adds a file to the set by providing a file path.
            /*!
             *This function will add the specified file to the set and return true if successfull. If the file
             *is already in the set it won't be added again and false will be returned.
             *
             *\returns True when added successfully, false otherwise.
             *
             *\sa removeFile(), hasFile(), files(), fileChanged(), setChanged()
             */
            bool addFile(const QString& file_path);
            //! Adds a file to the set by providing a QtilitiesFileInfo object describing the file.
            /*!
             *This function will add the specified file to the set and return true if successfull. If the file
             *is already in the set it won't be added again and false will be returned.
             *
             *\returns True when added successfully, false otherwise.
             *
             *\sa removeFile(), hasFile(), files(), fileChanged(), setChanged()
             */
            bool addFile(QtilitiesFileInfo file_info);
            //! Removes a file from the set by providing a file path.
            /*!
             *This function will remove the file from the set and return true if successfull.
             *
             *\returns True when removed successfully, false otherwise.
             *
             *\sa addFile(), hasFile(), files()
             */
            bool removeFile(const QString& file_path);
            //! Removes a file from the set by providing a QtilitiesFileInfo object describing the file.
            /*!
             *This function will remove the file from the set and return true if successfull.
             *
             *\returns True when removed successfully, false otherwise.
             *
             *\sa addFile(), hasFile(), files()
             */
            bool removeFile(QtilitiesFileInfo file_info);
            //! Checks if the specified file is part of this set.
            /*!
             *\returns True when the file is part of this set, false otherwise.
             *
             *\sa addFile(), removeFile(), files()
             */
            bool hasFile(const QString& file_path) const;
            //! Gets a list of all files in this set as a QStringList containing the actual paths of all files.
            /*!
             *\returns A QStringList file paths of all files in the list.
             *
             *\sa addFile(), removeFile(), hasFile(), filesLastSave()
             */
            QStringList filePaths() const;
            //! Gets a list of all files in this set as a list of QtilitiesFileInfo objects.
            /*!
             *\returns A QStringList file paths of all files in the list.
             *
             *\sa addFile(), removeFile(), hasFile(), filesLastSave()
             */
            QList<QtilitiesFileInfo> files() const;
            //! Indicates if this file set is empty (thus it contains not files).
            bool isEmpty() const;
            //! Returns the number of files in this file set.
            int count() const;
            //! Removes all files from this file set hash.
            void clear();
            //! Updates all paths starting with the search string to start with replace string.
            void updateRelativeToPaths(const QString& search_string, const QString& replace_string, ITask *task);

            // -----------------------------------
            // State Of Files In Set
            // -----------------------------------
            //! Returns a hash code for the contents of all files in the set.
            /*!
             *This function will generate an unique hash code generated from the contents of all files in the set.
             *
             *\param update_previous_hash_storage When true, the internal storage storing the last calculated hash which is accessable through
             *fileSetHashPrevious() will be updated. When false, the new hash will only be returned and the internal storage won't be modified.
             *\returns The file set hash for the files in this set. If no files are in the set, this function will return -1.
             *
             *\sa fileSetHashPrevious()
             */
            int fileSetHash(bool update_previous_hash_storage = true) const;
            //! Sets the hash code for the contents of all files in the set.
            void setFileSetHash(int hash);
            //! Returns the last hash code calculated for the contents of all files in the set.
            /*!
             *This function will return the last calculated hash code for the contents of all files in the set.
             *
             *\note If the hash has never been calculated, this function will return -1.
             *
             *\sa fileSetHash()
             */
            int fileSetHashPrevious() const;
            //! Function which checks if all files in this set exists.
            bool allFilesExists() const;

            //! Disables file watching on this file set.
            /*!
             * \brief disableFileWatching
             * Enabled by default.
             *
             *\sa enableFileWatching(), fileWatchingEnabled(), setFileWatchingEnabled()
             */
            void disableFileWatching();
            //! Enables file watching on this file file set.
            /*!
             * \brief enableFileWatching
             * Enabled by default.
             *
             *\sa disableFileWatching(), fileWatchingEnabled(), setFileWatchingEnabled()
             */
            void enableFileWatching();
            //! Function to enable/disable file watching.
            /*!
             * \brief enableFileWatching
             * Enabled by default.
             *
             *\sa disableFileWatching(), enableFileWatching(), fileWatchingEnabled()
             */
            void setFileWatchingEnabled(bool enabled);
            //! Indicates if file watching is enabled on this file set.
            /*!
             * \brief fileWatchingEnabled
             * Enabled by default.
             *
             *\sa disableFileWatching(), enableFileWatching(), setFileWatchingEnabled()
             */
            bool fileWatchingEnabled() const;

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, FileSetInfo> factory;
            static InstanceFactoryInfo staticInstanceFactoryInfo();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        signals:
            //! Emitted when the contents of a file changed.
            /*!
             * \brief fileChanged Emitted when the contents of any monitored file in the set is changed, renamed or removed.
             * \param path The path of the file that changed.
             *
             *Internally this class uses QFileSystemWatcher to monitor changes to file contents and this signal
             *is basically connected to the same signal on the internal file system watcher.
             */
            void fileChanged(const QString & path);

            //! Emitted when the file set changed.
            /*!
             * \brief fileChanged
             *
             *Emitted when the set changed (only monitored files are taken into account).
             *
             *This function will be emitted when:
             *- The contents of any of the files in the set is changed, renamed or removed.
             *- When files are added to or removed from the set.
             */
            void setChanged();

        private:
            FileSetInfoPrivateData* d;
        };
    }
}

Q_DECLARE_METATYPE(Qtilities::Core::FileSetInfo);

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::FileSetInfo& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::FileSetInfo& stream_obj);

#endif // FILE_SET_INFO_H
