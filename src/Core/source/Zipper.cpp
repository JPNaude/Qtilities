/****************************************************************************
**
** Copyright 2010-2013, CSIR
** Author: JP Naude, jpnaude@csir.co.za
**
****************************************************************************/

#include "Zipper.h"
#include "QtilitiesProcess.h"
#include "QtilitiesCoreApplication.h"
#include "FileUtils.h"
#include "QtilitiesFileInfo.h"

struct Qtilities::Core::ZipperPrivateData {
    ZipperPrivateData() : is_completed(true) {}

    QString                 recorded_message;
    QString                 ignore_list;
    QString                 last_error_message;
    bool                    is_completed;
    bool                    exit_code;
    QtilitiesProcess*       zip_process;
    QProcess::ExitStatus    last_status;
    QProcess::ProcessError  last_error;
    QString                 path_7za;
    QString                 temp_dir;
};

Qtilities::Core::Zipper::Zipper(const QString& path_7za, const QString& ignore_list, const QString &temp_dir, QObject *parent) :
    QObject(parent)
{
    d = new ZipperPrivateData;
    d->path_7za = path_7za;
    d->ignore_list = ignore_list;
    if (temp_dir.isEmpty())
        d->temp_dir = QtilitiesCoreApplication::applicationSessionPath();
    else
        d->temp_dir = temp_dir;

    d->zip_process = new QtilitiesProcess("zipper",true,this);
    d->zip_process->setTaskStopAction(ITask::TaskDoNothingWhenStopped);
    d->zip_process->setTaskRemoveAction(ITask::TaskHideWhenRemoved);
    connect(d->zip_process,SIGNAL(newMessageLogged(QString,Logger::MessageType)),SLOT(getNewMessageLogged(QString,Logger::MessageType)));
    OBJECT_MANAGER->registerObject(d->zip_process);

    connect(d->zip_process,SIGNAL(taskCompleted(ITask::TaskResult,QString,Logger::MessageType)),SLOT(processCompleted()));
    connect(d->zip_process,SIGNAL(taskStarted()),SLOT(processStarted()));
}

Qtilities::Core::Zipper::~Zipper() {
    if (d->zip_process)
        delete d->zip_process;
    delete d;
}

ITask* Qtilities::Core::Zipper::task() {
    return d->zip_process;
}

bool Qtilities::Core::Zipper::executeCommand(QStringList arguments) {
    bool result;
    #ifdef Q_OS_WIN
    QString error_msg = QString("Could not start %1. Make sure this application is located in your application's installation path. If it is not, a reinstallating of %2 or installing 7-zip manually will solve the problem.").arg(d->path_7za).arg(QCoreApplication::applicationName());
    result = d->zip_process->startProcess("7za",arguments);
    #else
    QString error_msg = QString("Could not start %1 which is needed to perform archiving operations. Installing 7za through your package manager will solve the problem.").arg(d->path_7za);
    result = d->zip_process->startProcess("7za",arguments);
    #endif

    d->last_error_message = error_msg;

    return result;
}

// -----------------------------------------------------------
// Convenience Functions To Perform Common Zip Operations
// -----------------------------------------------------------
QString Qtilities::Core::Zipper::zipInfo(const QString& file_path) {
    // List the contents of the package in the info widget.
    if (zipInfoPrivate(file_path)) {
        while (busy())
            QCoreApplication::processEvents();

        // Query the zipper process:
        if (lastExitStatus() == QProcess::NormalExit) {
            if (exitCode() == 0) {
                LOG_INFO("Successfully retreived information about archive at \"" + file_path + "\".");
                return recordedMessage();
            } else {
                LOG_ERROR("The zip process failed with error code " + QString::number(exitCode()) + ". Study the log for more information.");
                return "The zip process failed to get zip info with error code " + QString::number(exitCode()) + ". Study the log for more information.";
            }
        } else {
            LOG_ERROR(lastProcessErrorMsg());
            return lastProcessErrorMsg();
        }
    } else {
        LOG_ERROR(lastProcessErrorMsg());
        return lastProcessErrorMsg();
    }
}

void Qtilities::Core::Zipper::getNewMessageLogged(const QString& message, Logger::MessageType) {
    d->recorded_message.append(message);
}

bool Qtilities::Core::Zipper::zipFolder(const QString& folder_path, const QString& output_file, ZipMode mode) {
    if (zipFolderPrivate(folder_path,output_file,mode)) {
        while (busy())
            QCoreApplication::processEvents();

        // Now check the last exit status:
        if (lastExitStatus() == QProcess::NormalExit) {
            if (exitCode() == 0) {
                LOG_INFO("Successfully archived folder contents from \"" + folder_path + "/*" + "\" to destination file \"" + output_file + "\".");
                return true;
            } else {
                LOG_ERROR("The zip process failed with error code " + QString::number(exitCode()) + ". Study the log for more information.");
                return false;
            }
        } else {
            LOG_ERROR(lastProcessErrorMsg());
            return false;
        }
    } else {
        LOG_ERROR(lastProcessErrorMsg());
        return false;
    }
}

bool Qtilities::Core::Zipper::unzipFolder(const QString& input_file, const QString& destination_path) {
    if (unzipFolderPrivate(input_file,destination_path)) {
        while (busy())
            QCoreApplication::processEvents();

        // Now check the last exit status:
        if (lastExitStatus() == QProcess::NormalExit) {
            if (exitCode() == 0) {
                LOG_INFO("Successfully extracted archive \"" + input_file + "\" to destination folder \"" + destination_path + "\".");
                return true;
            } else {
                LOG_ERROR("The zip extraction process failed with error code " + QString::number(exitCode()) + ". Study the log for more information.");
                return false;
            }
        } else {
            LOG_ERROR(lastProcessErrorMsg());
            return false;
        }
    } else {
        LOG_ERROR(lastProcessErrorMsg());
        return false;
    }
}

bool Qtilities::Core::Zipper::moveFolder(const QString& source_path, const QString& destination_path) {
    if (copyFolder(source_path,destination_path)) {
        if (FileUtils::removeDir(source_path))
            return true;
        else {
            LOG_ERROR("Failed to remove source path during folder move operation at path: " + source_path);
            return false;
        }
    } else
        return false;
}

bool Qtilities::Core::Zipper::moveFolderContents(const QString& source_path, const QString& destination_path) {
    if (copyFolderContents(source_path,destination_path)) {
        if (FileUtils::removeDir(source_path))
            return true;
        else {
            LOG_ERROR("Failed to remove source path during folder contents move operation at path: " + source_path);
            return false;
        }
    } else
        return false;
}

bool Qtilities::Core::Zipper::copyFolder(const QString& source_path, const QString& destination_path) {
    QString tmp_file = d->temp_dir + "/tmp.zip";

    if (copyFolderPrivate(source_path,destination_path,tmp_file)) {
        while (busy())
            QCoreApplication::processEvents();

        // Now check the last exit status:
        if (lastExitStatus() == QProcess::NormalExit) {
            if (exitCode() == 0) {
                LOG_INFO("Successfully moved folder from \"" + source_path + "\" to destination folder \"" + destination_path + "\".");
                return true;
            } else {
                LOG_ERROR("The zip process failed to move folder with error code " + QString::number(exitCode()) + ". Study the log for more information.");
                return false;
            }
        } else {
            return false;
        }
    } else {
        LOG_ERROR(lastProcessErrorMsg());
        return false;
    }

    return false;
}

bool Qtilities::Core::Zipper::copyFolderContents(const QString& source_path, const QString& destination_path) {
    QString tmp_file = d->temp_dir + "/tmp.zip";
    if (copyFolderContentsPrivate(source_path,destination_path,tmp_file)) {
        while (busy())
            QCoreApplication::processEvents();

        // Now check the last exit status:
        if (lastExitStatus() == QProcess::NormalExit) {
            if (exitCode() == 0) {
                LOG_INFO("Successfully moved folder contents from \"" + source_path + "\" to destination folder \"" + destination_path + "\".");
                return true;
            } else {
                LOG_ERROR("The zip process failed to move folder contents with error code " + QString::number(exitCode()) + ". Study the log for more information.");
                return false;
            }
        } else {
            return false;
        }
    } else {
        LOG_ERROR(lastProcessErrorMsg());
        return false;
    }
}

// -----------------------------------------------------------
// Private Zip Functionality
// -----------------------------------------------------------
bool Qtilities::Core::Zipper::zipInfoPrivate(const QString& file_path) {
    if (file_path.isEmpty()) {
        d->last_error_message = "Zipper::zipInfo() got an empty file_path parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    QFileInfo fi(file_path);
    if (!fi.exists()) {
        d->last_error_message = "Zipper::zipInfo() cannot get information for input file, it does not exist: " + file_path;
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    QStringList arguments;
    arguments << "l";
    arguments << file_path;
    clearRecordedMessage();
    LOG_INFO("Executing Process: 7za " + arguments.join(" "));
    d->is_completed = false;
    if (!d->zip_process->startProcess(d->path_7za,arguments))
        return false;
    else {
        while (busy())
            QCoreApplication::processEvents();
        return true;
    }
}

bool Qtilities::Core::Zipper::zipFolderPrivate(const QString& folder_path, const QString& output_file, ZipMode mode) {
    if (folder_path.isEmpty()) {
        d->last_error_message = "Zipper::zipFolder() got an empty folder_path parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    if (output_file.isEmpty()) {
        d->last_error_message = "Zipper::zipFolder() got an empty output_file parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    d->is_completed = false;

    QFile output_file_del(output_file);
    if (output_file_del.exists()) {
        if (!output_file_del.remove()) {
            d->last_error_message = "Zipper::zipFolder() failed to remove existing output file at path: " + output_file;
            getNewMessageLogged(d->last_error_message,Logger::Error);
            return false;
        }
    }

    QFileInfo file_info(folder_path);

    QStringList arguments;
    arguments << "a";
    if (file_info.completeSuffix().isEmpty())
        arguments << "-tzip";
    else
        arguments << "-t" + file_info.completeSuffix();
    arguments << output_file;
    arguments << folder_path;
    // Build up the ignore list in 7zip format:
    QStringList ignore_list_items = d->ignore_list.split(" ",QString::SkipEmptyParts);
    foreach (const QString& ignore_token, ignore_list_items)
        arguments << "-xr!" + ignore_token;

    if (mode == CopyMode)
        arguments << "-mx0";
    arguments << "-w" + d->temp_dir;
    return executeCommand(arguments);
}

bool Qtilities::Core::Zipper::unzipFolderPrivate(const QString& input_file, const QString& destination_path) {
    if (input_file.isEmpty()) {
        d->last_error_message = "Zipper::unzipFolder() got an empty input_file parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    if (destination_path.isEmpty()) {
        d->last_error_message = "Zipper::unzipFolder() got an empty destination_path parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    QDir dest_path(destination_path);
    if (!dest_path.mkpath(destination_path)) {
        d->last_error_message = "Failed to create destination unzip folder at: " + destination_path;
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    d->is_completed = false;
    QStringList arguments;
    arguments << "x";
    arguments << input_file;
    arguments << "-o" + destination_path;
    arguments << "-aoa";
    arguments << "-w" + d->temp_dir;
    return executeCommand(arguments);
}

bool Qtilities::Core::Zipper::copyFolderPrivate(const QString& source_path, const QString& destination_path, const QString& tmp_file) {
    if (source_path.isEmpty()) {
        d->last_error_message = "Zipper::moveFolder() got an empty source_path parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    if (destination_path.isEmpty()) {
        d->last_error_message = "Zipper::moveFolder() got an empty destination_path parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    if (tmp_file.isEmpty()) {
        d->last_error_message = "Zipper::moveFolder() got an empty tmp_file parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    if (!QtilitiesFileInfo::isValidFilePath(destination_path)) {
        d->last_error_message = "Zipper::moveFolder() got a destination path which contains invalid characters: " + QtilitiesFileInfo::invalidFilePathCharacters();
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    QFile tmp_file_del(tmp_file);
    if (tmp_file_del.exists()) {
        if (!tmp_file_del.remove()) {
            d->last_error_message = "Failed to remove an old temporary file at path: " + tmp_file;
            getNewMessageLogged(d->last_error_message,Logger::Error);
            return false;
        }
    }

    LOG_DEBUG("Zipper is using the following temp file: " + tmp_file);

    if (!zipFolder(source_path,tmp_file,CopyMode)) {
        d->is_completed = true;
        return false;
    } 

    if (!unzipFolder(tmp_file,destination_path)) {
        d->is_completed = true;
        return false;
    }

    if (tmp_file_del.exists()) {
        if (!tmp_file_del.remove()) {
            d->last_error_message = "Failed to remove newly created temporary file at path: " + tmp_file;
            getNewMessageLogged(d->last_error_message,Logger::Error);
        }
    }

    // In this case, check lastExitStatus() for the result.
    return true;
}

bool Qtilities::Core::Zipper::copyFolderContentsPrivate(const QString& source_path, const QString& destination_path, const QString& tmp_file) {
    if (source_path.isEmpty()) {
        d->last_error_message = "Zipper::moveFolderContents() got an empty source_path parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    if (destination_path.isEmpty()) {
        d->last_error_message = "Zipper::moveFolderContents() got an empty destination_path parameter.";
        getNewMessageLogged(d->last_error_message,Logger::Error);
        return false;
    }

    return copyFolderPrivate(source_path + "/*",destination_path,tmp_file);
}

void Qtilities::Core::Zipper::processCompleted() {
    d->zip_process->process()->waitForFinished(3000);

    d->is_completed = true;
    d->last_status = d->zip_process->process()->exitStatus();
    if (d->zip_process->result() == ITask::TaskSuccessful || d->zip_process->result() == ITask::TaskSuccessfulWithWarnings)
        d->exit_code = 0;
    else if (d->zip_process->result() == ITask::TaskFailed)
        d->exit_code = d->zip_process->process()->exitCode();
}

void Qtilities::Core::Zipper::processStarted() {
    d->is_completed = false;
}

Qtilities::Core::ArchiveType Qtilities::Core::Zipper::newArchiveType(const QString& type,const QString& description ,const QString& argument ,const QString& extension){
    ArchiveType archiveType;
    archiveType.type = type;
    archiveType.description = description;
    archiveType.argument = argument;
    archiveType.extension = extension;

    return archiveType;
}

 QList<Qtilities::Core::ArchiveType> Qtilities::Core::Zipper::validArchiveTypes() {
    QList<ArchiveType> valid_archive_types;
    valid_archive_types.append(newArchiveType("7Z","http://en.wikipedia.org/wiki/7z","-t7z","7z"));
    valid_archive_types.append(newArchiveType("GZIP","http://en.wikipedia.org/wiki/Gzip","-tgzip","gzip"));
    valid_archive_types.append(newArchiveType("GZIP","http://en.wikipedia.org/wiki/Gzip","-tgzip","gz"));
    valid_archive_types.append(newArchiveType("ZIP","http://en.wikipedia.org/wiki/ZIP_(file_format)","-tzip","zip"));
    valid_archive_types.append(newArchiveType("BZIP2","http://en.wikipedia.org/wiki/Bzip2","-tbzip2","bzip2"));
    valid_archive_types.append(newArchiveType("TAR","http://en.wikipedia.org/wiki/Tar_(file_format)","-ttar","tar"));
    valid_archive_types.append(newArchiveType("ISO","http://en.wikipedia.org/wiki/ISO_image","-tiso","iso"));
    valid_archive_types.append(newArchiveType("UDF","http://en.wikipedia.org/wiki/Universal_Disk_Format","-tudf","udf"));
    return valid_archive_types;
 }

 QMap<QString,QString> Qtilities::Core::Zipper::validExtensionTypeMap() {
     QMap<QString,QString> map;
     QList<ArchiveType> archive_types = Zipper::validArchiveTypes();
     for (int i = 0; i < archive_types.count(); ++i) {
         map[archive_types.at(i).extension] = archive_types.at(i).type;
     }
     return map;
 }

 bool Qtilities::Core::Zipper::isValidExtensionTypeCombination(const QString& extension, const QString& type) {
      QMap<QString,QString> map = Zipper::validExtensionTypeMap();
      if (map.contains(extension)) {
          if (map.value(extension) == type)
              return true;
      }

      return false;
 }

 bool Qtilities::Core::Zipper::isValidExtension(const QString& extension) {
      QMap<QString,QString> map = Zipper::validExtensionTypeMap();
      if (map.contains(extension))
          return true;

      return false;
 }

bool Qtilities::Core::Zipper::busy() {
    return !d->is_completed;
}

QString Qtilities::Core::Zipper::recordedMessage() const {
    return d->recorded_message;
}

void Qtilities::Core::Zipper::clearRecordedMessage() {
    d->recorded_message.clear();
}

void Qtilities::Core::Zipper::setIgnoreList(const QString& ignore_list) {
    d->ignore_list = ignore_list;
}

int Qtilities::Core::Zipper::exitCode() const {
    return d->exit_code;
}

QProcess::ExitStatus Qtilities::Core::Zipper::lastExitStatus() const {
    return d->last_status;
}

QProcess::ProcessError Qtilities::Core::Zipper::lastProcessError() const {
    return d->last_error;
}

QString Qtilities::Core::Zipper::lastProcessErrorMsg() const {
    return d->last_error_message;
}
