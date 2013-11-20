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
#include "FileLocker.h"
#include "QtilitiesCoreConstants.h"

#include <QCoreApplication>

using namespace Qtilities::Core;


FileLocker::FileLocker(const QString &lock_extension) {
    d_lock_extension = lock_extension;
}

bool FileLocker::isFileLocked(const QString &file_path) const {
    QFileInfo fi(file_path);
    if (!fi.exists())
        return false;

    // Check if a lock file exists:
    QFileInfo lock_fi(lockFilePathForFile(file_path));
    return lock_fi.exists();
}

bool FileLocker::lockFile(const QString &file_path, QString *errorMsg) {
    if (isFileLocked(file_path)) {
        if (errorMsg)
            *errorMsg = QString("Cannot lock file that is already locked: " + file_path);
        return false;
    }

    QString lock_file_path = lockFilePathForFile(file_path);
    QFile lock_file(lock_file_path);
    if(!lock_file.open(QFile::WriteOnly | QIODevice::Text)) {
        if (errorMsg)
            *errorMsg = QString("Cannot open lock file for writing at: " + file_path);
        return false;
    }

    QString lock_file_string;
    lock_file_string.append("Lock file created by " + QCoreApplication::applicationName() + "\n");

    QString host_name = "Unknown Host";
    // Get the host name:
    #ifdef Q_OS_WIN
    #ifndef Q_CC_MSVC
    char* host_name_char = getenv("COMPUTERNAME");
    #else
    char *host_name_char;
    size_t len;
    errno_t err = _dupenv_s( &host_name_char, &len, "COMPUTERNAME" );
    if ( err )
        qDebug() << "Can't find environment variable COMPUTERNAME";
    #endif
    #else
    char* host_name_char = getenv("HOSTNAME");
    #endif
    if (host_name_char)
        host_name = host_name_char;

    lock_file_string.append(host_name + "\n");
    lock_file_string.append(QDateTime::currentDateTime().toString() + "\n");

    if (!lock_file.write(lock_file_string.toUtf8())) {
        if (errorMsg)
            *errorMsg = QString("Cannot write lock contents to lock file: " + file_path);
        return false;
    }
    lock_file.close();

    return true;
}

bool FileLocker::unlockFile(const QString &file_path, QString *errorMsg) {
    if (!isFileLocked(file_path)) {
        if (errorMsg)
            *errorMsg = QString("Cannot unlock file that is not locked: " + file_path);
        return false;
    }

    // Check if a lock file exists:
    QString lock_file_pat = lockFilePathForFile(file_path);
    QFile lock_file(lock_file_pat);
    if (lock_file.remove())
        return true;
    else {
        if (errorMsg)
            *errorMsg = QString("Cannot unlock file. Failed to delete lock file at: " + lock_file_pat);
        return false;
    }
}

QString FileLocker::lastLockHostName(QString file_path, QString *errorMsg) const {
    if (!isFileLocked(file_path)) {
        if (errorMsg)
            *errorMsg = QString("Cannot unlock file that is not locked: " + file_path);
        return QString();
    }

    file_path = lockFilePathForFile(file_path);

    QFile lock_file(file_path);
    if (!lock_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg)
            *errorMsg = QString("Cannot open lock file for reading at: " + file_path);
        return QString();
    }

    int line_count = 0;
    QString host_name;
    while (!lock_file.atEnd()) {
        QString line = lock_file.readLine();
        if (line_count == 1) {
            host_name = line;
            break;
        }
        ++line_count;
    }

    return host_name;
}

QString FileLocker::lastLockDateTime(QString file_path, QString *errorMsg) const {
    if (!isFileLocked(file_path)) {
        if (errorMsg)
            *errorMsg = QString("Cannot unlock file that is not locked: " + file_path);
        return QString();
    }

    file_path = lockFilePathForFile(file_path);

    QFile lock_file(file_path);
    if (!lock_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg)
            *errorMsg = QString("Cannot open lock file for reading at: " + file_path);
        return QString();
    }

    int line_count = 0;
    QString date_time;
    while (!lock_file.atEnd()) {
        QString line = lock_file.readLine();
        if (line_count == 2) {
            date_time = line;
            break;
        }
        ++line_count;
    }

    return date_time;
}

QString FileLocker::lastLockSummary(QString file_path, const QString &line_break_char, QString *errorMsg) const {
    if (!isFileLocked(file_path)) {
        if (errorMsg)
            *errorMsg = "Cannot unlock file that is not locked: " + file_path;
        return QString();
    }

    file_path = lockFilePathForFile(file_path);

    QString summary;
    summary.append("Locked on host: " + lastLockHostName(file_path) + line_break_char);
    summary.append("Locked at: " + lastLockDateTime(file_path));
    return summary;
}

QString FileLocker::lockFilePathForFile(const QString &file_path) const {
    QFileInfo fi(file_path);
    if (!fi.exists())
        return QString();

    QString lock_file_path = fi.path();
    lock_file_path.append(QDir::separator());
    lock_file_path.append(fi.baseName());
    lock_file_path.append("." + QString(d_lock_extension));

    return lock_file_path;
}
