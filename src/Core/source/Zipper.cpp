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
    ZipperPrivateData() : zip_process("zipper") {}

    QString                 ignore_list;
    QtilitiesProcess        zip_process;
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
}

Qtilities::Core::Zipper::~Zipper() {
    delete d;
}

void Qtilities::Core::Zipper::setIgnoreList(const QString& ignore_list) {
    d->ignore_list = ignore_list;
}

QString Zipper::ignoreList() const {
    return d->ignore_list;
}

QtilitiesProcess* Qtilities::Core::Zipper::zipProcess() {
    return &d->zip_process;
}

// -------------------------
// Zip Process Evoking Functions
// -------------------------
QString Qtilities::Core::Zipper::zipInfo(const QString& file_path, bool *ok, QStringList* errorMsgs) {
    if (file_path.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("Empty archive path specified.");
        if (ok)
            *ok = false;
        return QString();
    }

    QFileInfo fi(file_path);
    if (!fi.exists()) {
        if (errorMsgs)
            errorMsgs->append("Specified archive does not exist: " + file_path);
        if (ok)
            *ok = false;
        return QString();
    }

    QStringList arguments;
    arguments << "l";
    arguments << file_path;

    bool current_use_run_buffer = d->zip_process.lastRunBufferEnabled();
    d->zip_process.setLastRunBufferEnabled(true);
    bool result = executeCommand(arguments,errorMsgs);
    if (ok)
        *ok = result;
    QString buffer = d->zip_process.lastRunBuffer();
    d->zip_process.setLastRunBufferEnabled(current_use_run_buffer);
    return buffer;
}

bool Zipper::zipFiles(const QStringList &files, const QString &output_file, QStringList *errorMsgs) {
    if (files.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("No files specified.");
        return false;
    }

    if (output_file.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("Empty destination file specified.");
        return false;
    }

    QFile output_file_del(output_file);
    if (output_file_del.exists()) {
        if (!output_file_del.remove()) {
            if (errorMsgs)
                errorMsgs->append("Failed to remove existing destination path at: " + output_file);
            return false;
        }
    }

    QFileInfo file_info(output_file);
    QStringList arguments;
    arguments << "a";

    if (file_info.completeSuffix().endsWith("zip")) { // Do this to support .SOMETHING_ELSE.zip etc.
        arguments << "-tzip"; //
    } else {
        if (file_info.completeSuffix().isEmpty())
            arguments << "-tzip";
        else {
            if (isValidExtension(file_info.completeSuffix()))
                arguments << "-t" + file_info.completeSuffix();
            else
                arguments << "-tzip";
        }
    }

    arguments << output_file;

    // Build up the ignore list in 7zip format:
    QStringList ignore_list_items = d->ignore_list.split(" ",QString::SkipEmptyParts);
    foreach (const QString& ignore_token, ignore_list_items)
        arguments << "-xr!" + ignore_token;

    arguments << "-w" + d->temp_dir;

    // Create a filelist d->temp_dir and use that to archive:
    QString tmp_file_path = d->temp_dir + "/" + QString::number(QDateTime::currentDateTime().toTime_t());
    FileUtils::writeTextFile(tmp_file_path,files.join("\n"));
    arguments << "@" + tmp_file_path;

    bool success = executeCommand(arguments,errorMsgs);
    QFile::remove(tmp_file_path);
    return success;
}

bool Qtilities::Core::Zipper::zipFolder(const QString& source_path, const QString& output_file, ZipMode mode, QStringList* errorMsgs) {
    if (source_path.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("Empty source path specified.");
        return false;
    }

    if (output_file.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("Empty destination file specified.");
        return false;
    }

    QFile output_file_del(output_file);
    if (output_file_del.exists()) {
        if (!output_file_del.remove()) {
            if (errorMsgs)
                errorMsgs->append("Failed to remove existing destination path at: " + output_file);
            return false;
        }
    }

    QFileInfo file_info(output_file);
    QStringList arguments;
    arguments << "a";

    if (file_info.completeSuffix().endsWith("zip")) { // Do this to support .SOMETHING_ELSE.zip etc.
        arguments << "-tzip"; //
    } else {
        if (file_info.completeSuffix().isEmpty())
            arguments << "-tzip";
        else {
            if (isValidExtension(file_info.completeSuffix()))
                arguments << "-t" + file_info.completeSuffix();
            else
                arguments << "-tzip";
        }
    }

    arguments << output_file;
    arguments << source_path;
    // Build up the ignore list in 7zip format:
    QStringList ignore_list_items = d->ignore_list.split(" ",QString::SkipEmptyParts);
    foreach (const QString& ignore_token, ignore_list_items)
        arguments << "-xr!" + ignore_token;

    if (mode == CopyMode)
        arguments << "-mx0";
    arguments << "-w" + d->temp_dir;
    return executeCommand(arguments,errorMsgs);
}

bool Qtilities::Core::Zipper::unzipFolder(const QString& source_path, const QString& destination_path, const QStringList& additional_arguments, QStringList *errorMsgs) {
    if (source_path.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("Empty source path specified.");
        return false;
    }

    if (!destination_path.isEmpty()) {
        QDir dest_path(destination_path);
        if (!dest_path.mkpath(destination_path)) {
            if (errorMsgs)
                errorMsgs->append("Failed to create destination folder at: " + destination_path);
            return false;
        }
    }

    QStringList arguments;
    arguments << "x";
    arguments << source_path;
    if (!destination_path.isEmpty())
        arguments << "-o" + destination_path;
    arguments << "-w" + d->temp_dir;
    arguments << additional_arguments;
    return executeCommand(arguments,errorMsgs);
}

bool Qtilities::Core::Zipper::moveFolder(const QString& source_path, const QString& destination_path, QStringList *errorMsgs) {
    if (copyFolder(source_path,destination_path,errorMsgs)) {
        if (FileUtils::removeDir(source_path))
            return true;
        else {
            if (errorMsgs)
                errorMsgs->append(QString("Failed to remove source path at: %1.").arg(source_path));
        }
    }
    return false;
}

bool Qtilities::Core::Zipper::moveFolderContents(const QString& source_path, const QString& destination_path, QStringList *errorMsgs) {
    if (copyFolderContents(source_path,destination_path,errorMsgs)) {
        if (FileUtils::removeDir(source_path))
            return true;
        else {
            if (errorMsgs)
                errorMsgs->append(QString("Failed to remove source path at: %1.").arg(source_path));
        }
    }
    return false;
}

bool Qtilities::Core::Zipper::copyFolder(const QString& source_path, const QString& destination_path, QStringList *errorMsgs) {
    if (source_path.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("Empty source path specified.");
        return false;
    }

    if (destination_path.isEmpty()) {
        if (errorMsgs)
            errorMsgs->append("Empty destination path specified.");
        return false;
    }

    QString tmp_file = d->temp_dir + "/tmp.zip";
    QFile tmp_file_del(tmp_file);
    if (tmp_file_del.exists()) {
        if (!tmp_file_del.remove()) {
            if (errorMsgs)
                errorMsgs->append("Failed to remove old temporary file at path: " + tmp_file);
            return false;
        }
    }

    if (!zipFolder(source_path,tmp_file,CopyMode,errorMsgs))
        return false;

    if (!unzipFolder(tmp_file,destination_path,QStringList("-aoa"),errorMsgs))
        return false;

    if (tmp_file_del.exists()) {
        if (!tmp_file_del.remove()) {
            if (errorMsgs)
                errorMsgs->append("Failed to remove newly created temporary file at path: " + tmp_file);
            return false;
        }
    }

    return true;
}

bool Qtilities::Core::Zipper::copyFolderContents(const QString& source_path, const QString& destination_path, QStringList *errorMsgs) {
    return copyFolder(source_path + "/*",destination_path,errorMsgs);
}

// -------------------------
// Archive Types
// -------------------------
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

bool Qtilities::Core::Zipper::executeCommand(QStringList arguments, QStringList* errorMsgs) {
    int polling_interval = 10;
    if (d->zip_process.startProcess(d->path_7za,arguments)) {
        if (polling_interval != -1) {
            while (d->zip_process.state() == ITask::TaskBusy) {
                QEventLoop loop;
                QTimer t;

                connect(d->zip_process.process(),SIGNAL(finished(int)),&loop,SLOT(quit()));
                if (polling_interval > 0) {
                    t.start(polling_interval);
                    connect(&t,SIGNAL(timeout()),&loop, SLOT(quit()));
                }

                loop.exec();
                QCoreApplication::processEvents();
            }
        } else {
            while (d->zip_process.state() == ITask::TaskBusy)
                QCoreApplication::processEvents();
        }

        if (d->zip_process.result() == ITask::TaskFailed) {
            if (errorMsgs)
                *errorMsgs = d->zip_process.lastErrorMessages();
            return false;
        } else
            return true;
    } else {
        if (errorMsgs)
            errorMsgs->append(QString("Failed to start process for command \"%1\".").arg(d->path_7za));
        return false;
    }
}
