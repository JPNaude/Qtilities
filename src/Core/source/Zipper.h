/****************************************************************************
**
** Copyright 2010-2013, CSIR
** Author: JP Naude, jpnaude@csir.co.za
**
****************************************************************************/

#ifndef ZIPPER_H
#define ZIPPER_H

#include <QObject>
#include <QStringList>

#include <QtilitiesProcess>

namespace Qtilities {
namespace Core {

using namespace Qtilities::Core;

/*!
\struct ArchiveType
\brief A structure storing details about an archive type.
  */
struct ArchiveType{
    ArchiveType() {}
    ArchiveType(const ArchiveType& ref) {
        type = ref.type;
        description = ref.description;
        argument = ref.argument;
        extension = ref.extension;
    }

    QString type;
    QString description;
    QString argument;
    QString extension;
};

/*!
\struct ZipperPrivateData
\brief A structure storing private data in the Zipper class.
  */
struct ZipperPrivateData;

//! The zipper class is able to perform numerours archive and extraction operations.
/*!
This class is basically a wrapper around 7za, the command line utility from the 7-Zip project (see http://www.7-zip.org/).

Below is an example showing how to use Zipper to move the contents of a folder to a different folder:

\code
QString ignore_list = "*.svn *.txt";
Zipper zipper("/opt/7zip/7za",ignore_list);

// Now copy all the contents:
QString tmp_file = QtilitiesApplication::applicationSessionPath() + "/tmp.zip";
QString src_folder = "c:/source_folder";
QString dest_folder = "c:/destination_folder";
QString errorMsg;
if (zipper.moveFolderContents(src_folder,dest_folder,tmp_file,&errorMsg))
    LOG_INFO("Successfully moved folder contents from \"" + src_folder + "\" to destination folder \"" + dest_folder + "\".");
else
    LOG_ERROR(errorMsg);
\endcode

The zipper will by default register a global %Qtilities task in the global object pool in order to provide progress information.
If this is not desired, the task type can be changed to be Qtilities::Core::Interfaces::ITask::TaskLocal as follows:

\code
Zipper zipper("/opt/7zip/7za");
zipper->task()->setTaskType(ITask::TaskLocal);
\endcode

The following page provides details on switches uses in the source code of Zipper: http://www.dotnetperls.com/7-zip-examples
and http://sevenzip.sourceforge.jp/chm/cmdline/commands/
*/
class QTILIITES_CORE_SHARED_EXPORT Zipper : public QObject
{
    Q_OBJECT
    Q_ENUMS(ZipMode)

public:
    /*!
     * \param path_7za The path to the 7za executable.
     * \param ignore_list A space seperated ignore list to use when performing archiving operations.
     * \param temp_dir In some situations (for example, moving folders) the zipper creates temporary files in this path. If not specified, QtilitiesCoreApplication::applicationSessionPath() will be used.
     * \param parent The Zipper's parent object.
     */
    explicit Zipper(const QString& path_7za, const QString& ignore_list = QString(), const QString& temp_dir = QString(), QObject *parent = 0);
    ~Zipper();

    //! Sets the ignore list used by the zipper.
    void setIgnoreList(const QString& ignore_list);
    //! Gets the ignore list used by the zipper.
    QString ignoreList() const;
    //! Function to access the zipper process.
    QtilitiesProcess* zipProcess();

    // -------------------------
    // Archive Types
    // -------------------------
    //! Returns a list of valid ArchiveTypes
    static QList<ArchiveType> validArchiveTypes();
    //! Returns a list of valid extensions along with their types.
    static QMap<QString,QString> validExtensionTypeMap();
    //! Checks if an extension & type combination is valid.
    static bool isValidExtensionTypeCombination(const QString& extension, const QString& type);
    //! Checks if an extension is valid.
    static bool isValidExtension(const QString& extension);
    //! Returns an ArchiveType.
    static ArchiveType newArchiveType(const QString& type,const QString& description ,const QString& argument ,const QString& extension);

    // -------------------------
    // Zip Process Evoking Functions
    // -------------------------
    //! Convenience function to zip a list of files.
    /*!
     * \note This function will automatically prepend and append " characters to the path in order to support paths with spaces. Thus do not do this manually, the Zipper will do it for you.
     * \returns True if the operation completed successfully, false otherwise.
     */
    bool zipFiles(const QStringList& files, const QString& output_file, QStringList* errorMsgs = 0);

    //! The possible zip modes applicable to archiving folders.
    enum ZipMode {
        CopyMode = 0,       /*!< Only do a copy, uses the -mx0 switch. */
        CompressMode = 1    /*!< Does a compression, uses the default settings. */
    };
    //! Convenience function to zip a folder to a file.
    /*!
     * It is possible to control whether the filenames in the archive will contain the folder_path prefix. For example:
     *
     * \code
     * folder_path = subdir\  , output_file = archive1.zip
     * adds all files and subfolders from folder subdir to archive archive1.zip. The filenames in archive will contain subdir\ prefix.
     *
     * folder_path = .\subdir\*  , output_file = archive1.zip
     * adds all files and subfolders from folder subdir to archive archive2.zip. The filenames in archive will not contain subdir\ prefix.
     * \endcode
     *
     * \note This function will automatically prepend and append " characters to the path in order to support paths with spaces. Thus do not do this manually, the Zipper will do it for you.
     * \note This function zips the contents of folder_path, thus it automatically appends / and * to folder_path.
     *
     * \returns True if the operation completed successfully, false otherwise.
     */
    bool zipFolder(const QString& folder_path, const QString& output_file, ZipMode mode = CompressMode, QStringList* errorMsgs = 0);
    //! Convenience function to unzip a file to a folder.
    /*!
        \note This function will automatically prepend and append " characters to the path in order to support paths with spaces. Thus do not do this manually, the Zipper will do it for you.

        \returns True if the operation completed successfully, false otherwise.
      */
    bool unzipFolder(const QString& source_path, const QString& destination_path, const QStringList &additional_arguments = QStringList(), QStringList* errorMsgs = 0);
    //! Convenience function to move a folder from one location to another.
    /*!
        The complete source_path folder will be moved to destination_path. Thus destination_path will contain source_path, not its' contents. For that functionality see moveFolderContents().

        \note This function will automatically prepend and append " characters to the paths in order to support paths with spaces. Thus do not do this manually, the Zipper will do it for you.

        \returns True if the operation completed successfully, false otherwise.
      */
    bool moveFolder(const QString& source_path, const QString& destination_path, QStringList* errorMsgs = 0);
    //! Convenience function to move the contents of a folder from one location to another.
    /*!
        \note This function will automatically prepend and append " characters to the path in order to support paths with spaces. Thus do not do this manually, the Zipper will do it for you.

        \returns True if the operation completed successfully, false otherwise.
      */
    bool moveFolderContents(const QString& source_path, const QString& destination_path, QStringList* errorMsgs = 0);
    //! Convenience function to copy a folder from one location to another.
    /*!
        The complete source_path folder will be copied to destination_path. Thus destination_path will contain source_path, not its' contents. For that functionality see copyFolderContents().

        \note This function will automatically prepend and append " characters to the paths in order to support paths with spaces. Thus do not do this manually, the Zipper will do it for you.

        \returns True if the operation completed successfully, false otherwise.
      */
    bool copyFolder(const QString& source_path, const QString& destination_path, QStringList* errorMsgs = 0);
    //! Convenience function to copy the contents a folder from one location to another.
    /*!
        \note This function will automatically prepend and append " characters to the path in order to support paths with spaces. Thus do not do this manually, the Zipper will do it for you.

        \returns True if the operation completed successfully, false otherwise.
      */
    bool copyFolderContents(const QString& source_path, const QString& destination_path, QStringList* errorMsgs = 0);
    //! Function to list the information about an archive, using the \p l command.
    /*!
     * \param file_path The archive.
     * \param ok Indicates if the information returned is valid. When true, the error that occured can be obtained through lastProcessErrorMsg().
     * \returns The information of the archive. If the information could not be obtained, returns an empty string and sets ok to false.
     */
    QString zipInfo(const QString& file_path, bool *ok, QStringList* errorMsgs = 0);

protected:
    //! Executes a zip command with the given arguments.
    virtual bool executeCommand(QStringList arguments, QStringList *errorMsgs = 0);

private:
    ZipperPrivateData* d;
};

}
}

#endif // ZIPPER_H
