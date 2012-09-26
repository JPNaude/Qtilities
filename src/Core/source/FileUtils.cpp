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
    static QFileInfoList list;
    if (first_run)
        list.clear();

    QDir dir(dirName);
    if (!dir.exists(dirName)) {
        return list;
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

    QDir::Filters final_filters;
    if (filters == QDir::NoFilter) {
        final_filters |= QDir::AllDirs;
        final_filters |= QDir::Files;
        final_filters |= QDir::NoDotAndDotDot;
    } else {
        final_filters = filters;
    }

    //qDebug() << "XXX" << dir.path() << file_filters << ignore_list;

    QStringList ignore_patterns = ignore_list.split(" ",QString::SkipEmptyParts);
    ignore_patterns.removeDuplicates();

    if (first_run) {
        if (task_ref) {
            // Calculate an estimate of the number of sub tasks:
            // Sub folders count as 10, files count as 1:
            int folder_count = 0;
            int file_count = 0;
            foreach (QFileInfo info, dir.entryInfoList(final_filters,sort)) {
                // Check if this entry must be ignored:
                bool not_ignored = true;
                if (!ignore_list.isEmpty()) {
                    foreach (QString ignore_pattern, ignore_patterns) {
                        // IMPORTANT: For paths, \\ separators does not do the trick. We need to use /
                        #ifdef Q_OS_WIN
                        QString path_to_match = QDir::fromNativeSeparators(info.filePath());
                        QString match_value_correct_sep = QDir::fromNativeSeparators(ignore_pattern);
                        #else
                        QString path_to_match = QDir::toNativeSeparators(info.filePath());
                        QString match_value_correct_sep = QDir::toNativeSeparators(ignore_pattern);
                        #endif

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
        list.clear();
    }

    foreach (QFileInfo info, dir.entryInfoList(final_filters,sort)) {
        QCoreApplication::processEvents();
        // Check if this entry must be ignored:
        bool not_ignored = true;
        if (!ignore_list.isEmpty()) {
            foreach (QString ignore_pattern, ignore_patterns) {
                // IMPORTANT: For paths, \\ separators does not do the trick. We need to use /
                #ifdef Q_OS_WIN
                QString path_to_match = QDir::fromNativeSeparators(info.filePath());
                QString match_value_correct_sep = QDir::fromNativeSeparators(ignore_pattern);
                #else
                QString path_to_match = QDir::toNativeSeparators(info.filePath());
                QString match_value_correct_sep = QDir::toNativeSeparators(ignore_pattern);
                #endif

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
                findFilesUnderDir(info.absoluteFilePath(),file_filters,ignore_list,filters,sort,false);

                // If its a directory and QDir::AllDirs was set and not QDir::Files, we assume that
                // the function is being used to return directory names and not files. In that
                // case we add this directory name to the results.
                if ((final_filters & QDir::AllDirs) && !(final_filters & QDir::Files)) {
                    LOG_TASK_INFO("Found directory: " + info.absoluteFilePath(),task_ref);
                    list.append(info);
                }

                if (task_ref) {
                    if (first_run)
                        task_ref->addCompletedSubTasks(10);
                }
            } else {
                list.append(info);
                if (task_ref) {
                    if (first_run) {
                        task_ref->logMessage("Found file: " + info.absoluteFilePath());
                        task_ref->addCompletedSubTasks(1);
                    }
                }
            }
        } else if (first_run) {
            if (task_ref) {
                if (first_run) {
                    if (info.isDir())
                        task_ref->logMessage("Ignoring directory: " + info.absoluteFilePath());
                    else
                        task_ref->logMessage("Ignoring file: " + info.absoluteFilePath());
                }
            }
        }
    }

    if (first_run) {
        last_files_under_dir = list;
        if (task_ref) {
            task_ref->setDisplayName("Found Files In: " + dir.dirName());
            task_ref->logMessage("Successfully searched for and found " + QString::number(list.count()) + " files under directory: " + dirName);
            task_ref->completeTask(ITask::TaskSuccessful);
        }
    }

    return list;
}

QFileInfoList Qtilities::Core::FileUtils::lastFilesUnderDir() {
    return last_files_under_dir;
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
        foreach (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir())
                result = removeDir(info.absoluteFilePath());
            else
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

bool FileUtils::comparePaths(const QString &path1, const QString &path2) {
    return (FileUtils::toNativeSeparators(QDir::cleanPath(path1)).compare(FileUtils::toNativeSeparators(QDir::cleanPath(path2)),Qt::CaseInsensitive) == 0);
}

QString FileUtils::toNativeSeparators(QString path) {
    #ifdef Q_OS_LINUX
    return path.replace("\\","/");
    #else
    return QDir::toNativeSeparators(path);
    #endif
}
