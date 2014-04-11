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

#include "FileUtils.h"
#include "ITask.h"
#include "Task.h"
#include "QtilitiesFileInfo.h"
#include "QtilitiesCoreApplication.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHash>
#include <QtDebug>
#include <QCoreApplication>

using namespace Qtilities::Core::Interfaces;

struct Qtilities::Core::FileUtilsPrivateData {
    FileUtilsPrivateData() { }

    // Storage for findFilesUnderDir() future calls:
    QString             dirName;
    QString             ignore_list;
    QString             file_filters;
    QDir::Filters       filters;
    QDir::SortFlags     sort;

    QFileInfoList       find_files_under_dir_list;
};

Qtilities::Core::FileUtils::FileUtils(bool enable_tasking, QObject* parent) : QObject(parent) {
    d = new FileUtilsPrivateData;

    if (enable_tasking) {
        // Register the tasks contained in this object:
        // Create TaskFindFilesUnderDir:
        Task* taskFindFileUnderDir = new Task(taskNameToString(TaskFindFilesUnderDir),true,this);
        taskFindFileUnderDir->setCanStart(true);
        connect(taskFindFileUnderDir,SIGNAL(startTaskRequest()),SLOT(findFilesUnderDirLauncher()));
        registerTask(taskFindFileUnderDir,taskNameToString(TaskFindFilesUnderDir));
    }
}

Qtilities::Core::FileUtils::~FileUtils() {
    delete d;
}

// --------------------------------
// Class Implementation
// --------------------------------

QFileInfoList Qtilities::Core::FileUtils::findFilesUnderDir(const QString &dirName,
                                                            const QString& file_filters,
                                                            const QString& ignore_list,
                                                            QDir::Filters filters,
                                                            QDir::SortFlags sort,
                                                            bool first_run) {
    if (first_run)
        d->find_files_under_dir_list.clear();

    QDir dir(dirName);
    if (!dir.exists(dirName)) {
        return d->find_files_under_dir_list;
    }

    int task_id = findTaskID(taskNameToString(TaskFindFilesUnderDir));
    Task* task_ref = 0;
    if (isTaskActive(task_id)) {
        task_ref = findTask(taskNameToString(TaskFindFilesUnderDir));
    }

    if (!file_filters.isEmpty()) {
        QStringList file_filters_list = file_filters.split(" ",QString::SkipEmptyParts);
        dir.setNameFilters(file_filters_list);
    }

    QDir::Filters final_filters = filters;
    // Always add QDir::NoDotAndDotDot otherwise we can go into an endless loop here.
    final_filters |= QDir::NoDotAndDotDot;
    //qDebug() << "XXX" << dir.path() << file_filters << ignore_list << final_filters;

    QStringList ignore_patterns = ignore_list.split(" ",QString::SkipEmptyParts);
    ignore_patterns.removeDuplicates();

    if (first_run) {
        if (task_ref) {
            // Calculate an estimate of the number of sub tasks:
            // Sub folders count as 10, files count as 1:
            int folder_count = 0;
            int file_count = 0;
            foreach (const QFileInfo& info, dir.entryInfoList(final_filters,sort)) {
                // Check if this entry must be ignored:
                bool not_ignored = true;
                if (!ignore_list.isEmpty()) {
                    foreach (const QString& ignore_pattern, ignore_patterns) {
                        // IMPORTANT: For paths, \\ separators does not do the trick. We need to use /
                        QString path_to_match = FileUtils::toUnixPath(info.filePath());
                        QString match_value_correct_sep = FileUtils::toUnixPath(ignore_pattern);

                        QRegExp regExp(match_value_correct_sep);
                        regExp.setPatternSyntax(QRegExp::Wildcard);
                        if (regExp.exactMatch(path_to_match)) {
                            not_ignored = false;
                            continue;
                        }
                    }
                }

                if (not_ignored) {
                    if (info.isDir())
                        ++folder_count;
                    else
                        ++file_count;
                }
            }

            task_ref->setDisplayName("Finding Files: " + dir.dirName());
            task_ref->startTask(folder_count*10 + file_count);
            task_ref->logMessage("Searching for files in directory: " + dirName);
        }
        d->find_files_under_dir_list.clear();
    } else {
        // If its not the first run, we are in a recursive call. In this case, remove QDir::AllDirs which is passed as an argument
        // for the recursive calls.
        final_filters &= ~QDir::AllDirs;
    }

    foreach (const QFileInfo& info, dir.entryInfoList(final_filters | QDir::AllDirs,sort)) {
        QCoreApplication::processEvents();
        // Check if this entry must be ignored:
        bool not_ignored = true;
        if (!ignore_list.isEmpty()) {
            foreach (const QString& ignore_pattern, ignore_patterns) {
                // IMPORTANT: For paths, \\ separators does not do the trick. We need to use /
                QString path_to_match = FileUtils::toUnixPath(info.filePath());
                QString match_value_correct_sep = FileUtils::toUnixPath(ignore_pattern);

                QRegExp regExp(match_value_correct_sep);
                regExp.setPatternSyntax(QRegExp::Wildcard);
                if (regExp.exactMatch(path_to_match)) {
                    not_ignored = false;
                    continue;
                }
            }
        }

        if (not_ignored) {
            if (info.isDir()) {
                if (task_ref) {
                    task_ref->logMessage("Searching for files in directory: " + info.absoluteFilePath());
                    task_ref->setDisplayName("Finding Files: " + info.dir().dirName());
                }
                findFilesUnderDir(info.absoluteFilePath(),file_filters,ignore_list,filters | QDir::AllDirs,sort,false);

                if (final_filters & QDir::Dirs) {
                    if (task_ref)
                        task_ref->logMessage("Found directory: " + info.absoluteFilePath());
                    d->find_files_under_dir_list.append(info);
                }

                if (task_ref && first_run)
                    task_ref->addCompletedSubTasks(10);
            } else {
                if (final_filters & QDir::Files) {
                    if (task_ref)
                        task_ref->logMessage("Found file: " + info.absoluteFilePath());
                    d->find_files_under_dir_list.append(info);
                }

                if (task_ref && first_run)
                    task_ref->addCompletedSubTasks(1);
            }
        } else if (first_run) {
            if (task_ref) {
                if (info.isDir())
                    task_ref->logMessage("Ignoring directory: " + info.absoluteFilePath());
                else
                    task_ref->logMessage("Ignoring file: " + info.absoluteFilePath());
            }
        }
    }

    if (first_run && task_ref) {
        task_ref->setDisplayName(tr("Found Files In: ") + dir.dirName());
        task_ref->logMessage("Successfully searched for and found " + QString::number(d->find_files_under_dir_list.count()) + " files under directory: " + dirName);
        task_ref->completeTask(ITask::TaskSuccessful);
    }

    return d->find_files_under_dir_list;
}

QFileInfoList Qtilities::Core::FileUtils::lastFilesUnderDir() {
    return d->find_files_under_dir_list;
}

void Qtilities::Core::FileUtils::setFindFilesUnderDirParams(const QString &dirName, const QString& file_filters, const QString& ignore_list, QDir::Filters filters, QDir::SortFlags sort) {
    d->dirName = dirName;
    d->file_filters = file_filters;
    d->ignore_list = ignore_list;
    d->filters = filters;
    d->sort = sort;
}

QFileInfoList Qtilities::Core::FileUtils::findFilesUnderDirLauncher() {
    int task_id = findTaskID(taskNameToString(TaskFindFilesUnderDir));
    Task* task_ref = 0;
    if (isTaskActive(task_id)) {
        task_ref = findTask(taskNameToString(TaskFindFilesUnderDir));
        Q_ASSERT(task_ref);

        if (task_ref->state() & ITask::TaskIdle)
            return findFilesUnderDir(d->dirName,d->file_filters,d->ignore_list,d->filters,d->sort);
    }

    return QFileInfoList();
}

bool Qtilities::Core::FileUtils::removeDir(const QString& dirName) {
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        foreach (const QFileInfo& info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                QCoreApplication::processEvents();
                result = removeDir(info.absoluteFilePath());
            } else
                result = QFile::remove(info.absoluteFilePath());

            if (!result)
                return result;
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

QString Qtilities::Core::FileUtils::appendToFileName(const QString &fullFileName, const QString& append_string) {
    QtilitiesFileInfo file_info(fullFileName);
    QString file_path = file_info.path();
    QString extension = file_info.completeSuffix();
    QString name_only = file_info.baseName();
    if (extension.isEmpty())
        return file_path + QDir::separator() + name_only + append_string;
    else
        return file_path + QDir::separator() + name_only + append_string + "." + extension;
}

QString Qtilities::Core::FileUtils::removeFromFileName(const QString &fullFileName, int len) {
    QtilitiesFileInfo file_info(fullFileName);
    QString file_path = file_info.path();
    QString extension = file_info.completeSuffix();
    QString name_only = file_info.baseName();
    if (extension.isEmpty())
        return file_path + QDir::separator() + name_only.remove(name_only.length()-len,len);
    else
        return file_path + QDir::separator() + name_only.remove(name_only.length()-len,len) + "." + extension;
}

int Qtilities::Core::FileUtils::fileHashCode(const QString& file_name) {
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
        return -1;
    QByteArray file_contents = file.readAll();
    //qDebug() << "textFileHashCode" << file_name << qHash(file_contents);
    file.close();
    return qHash(file_contents);
}

bool Qtilities::Core::FileUtils::compareFiles(const QString& file1, const QString& file2) {
    int original = FileUtils::fileHashCode(file1);
    int readback = FileUtils::fileHashCode(file2);
    if (original == -1 || readback == -1)
        return false;
    return (original == readback);
}

bool FileUtils::comparePaths(const QString &path1, const QString &path2, Qt::CaseSensitivity cs) {
    QFileInfo fi1(path1);
    QFileInfo fi2(path2);
    bool do_easy_compare = true;
    #ifdef Q_OS_WIN
    if (cs == Qt::CaseSensitive)
        do_easy_compare = false;
    #else
    Q_UNUSED(cs)
    #endif

    if (fi1.exists() && fi2.exists() && do_easy_compare)
        return fi1 == fi2;
    else {
        QString cleaned_1 = QDir::cleanPath(path1);
        QString cleaned_2 = QDir::cleanPath(path2);
        if (cleaned_1.size() == cleaned_2.size()) {
            #ifdef Q_OS_WIN
            return (FileUtils::toNativeSeparators(cleaned_1).compare(FileUtils::toNativeSeparators(cleaned_2),cs) == 0);
            #else
            return (FileUtils::toNativeSeparators(cleaned_1).compare(FileUtils::toNativeSeparators(cleaned_2),Qt::CaseSensitive) == 0);
            #endif
        } else
            return false;
    }
}

bool FileUtils::pathStartsWith(const QString &child_path, const QString &parent_path, Qt::CaseSensitivity cs) {
    #ifdef Q_OS_WIN
    return toNativeSeparators(QDir::cleanPath(child_path)).startsWith(toNativeSeparators(QDir::cleanPath(parent_path)),cs);
    #else
    Q_UNUSED(cs)
    return toNativeSeparators(QDir::cleanPath(child_path)).startsWith(toNativeSeparators(QDir::cleanPath(parent_path)),Qt::CaseSensitive);
    #endif
}

QString FileUtils::toNativeSeparators(QString path) {
    #ifdef Q_OS_LINUX
    QString new_path = path.replace("\\","/");
    return new_path;
    #else
    return QDir::toNativeSeparators(path);
    #endif
}

bool FileUtils::makeLocalCopyOfResource(const QString &resource_path, const QString &local_path, QString *errorMsg, QFile::Permissions local_permissions) {
    QFile resource_file(resource_path);
    if (!resource_file.exists()) {
        if (errorMsg)
            *errorMsg = QString("Resource file does not exist at path: %1. It will not be copied.").arg(resource_file.fileName());
        return false;
    }

    QFileInfo fi(local_path);
    QDir dir(fi.path());
    if (!dir.exists()) {
        if (!dir.mkpath(fi.path())) {
            if (errorMsg)
                *errorMsg = QString("Failed to create target directory: %1. Resource file will not be copied.").arg(fi.path());
            return false;
        }
    }

    QFile local_file(local_path);
    if (local_file.exists()) {
        if (!local_file.remove()) {
            if (errorMsg)
                *errorMsg = QString("Failed to remove existing local file at: %1. Resource file will not be copied.").arg(local_path);
            return false;
        }
    }
    if (!resource_file.copy(local_path)) {
        if (errorMsg)
            *errorMsg = QString("Failed to create a copy of resource file: %1 -> %2").arg(resource_path).arg(local_path);
        return false;
    } else {
        if (!QFile::setPermissions(local_path, local_permissions)) {
            if (errorMsg)
                *errorMsg = QString("Failed to set file permissions on copied resource file at path: %1.").arg(local_path);
            return false;
        }
    }

    return true;
}

QString FileUtils::toUnixPath(const QString &path) {
    #ifdef Q_OS_WIN
    return QDir::fromNativeSeparators(QDir::cleanPath(path));
    #else
    return QDir::toNativeSeparators(QDir::cleanPath(path));
    #endif
}

bool FileUtils::writeTextFile(const QString &file_path, const QString &file_contents, QString *errorMsg) {
    QFile file(file_path);
    if (!file.open(QFile::WriteOnly)) {
        if (errorMsg)
            *errorMsg = QObject::tr("Failed to open output file for writing at: %1").arg(file.fileName());
        return false;
    }

    if (file.write(file_contents.toUtf8()) == (qint64) -1) {
        if (errorMsg)
            *errorMsg = QObject::tr("Failed to write to file at: %1").arg(file.fileName());
        file.close();
        file.remove();
        return false;
    }
    file.close();
    return true;
}

QString FileUtils::readTextFile(const QString &file_path, bool *ok, QString *errorMsg) {
    QFileInfo fi(file_path);
    if (!fi.exists()) {
        if (errorMsg)
            *errorMsg = tr("File does not exist. Expected at: %1").arg(file_path);
        if (ok)
            *ok = false;
        return QString();
    }

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg)
            *errorMsg = tr("File cannot be opened as a text file. File path: %1").arg(file_path);
        if (ok)
            *ok = false;
        return QString();
    }

    QString contents = QString(file.readAll());
    file.close();

    if (ok)
        *ok = true;
    return contents;
}
