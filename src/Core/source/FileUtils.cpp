/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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
#include "QtilitiesFileInfo.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHash>
#include <QtDebug>

bool Qtilities::Core::FileUtils::removeDir(const QString& dirName) {
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
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
    QString new_file_name = file_path + "/" + name_only + "_" + append_string + "." + extension;
    return new_file_name;
}

QString Qtilities::Core::FileUtils::removeFromFileName(const QString &fullFileName, int len) {
    QtilitiesFileInfo file_info(fullFileName);
    QString file_path = file_info.path();
    QString extension = file_info.completeSuffix();
    QString name_only = file_info.baseName();
    QString new_file_name = file_path + "/" + name_only.remove(name_only.length()-len,len) + "." + extension;
    return new_file_name;
}

int Qtilities::Core::FileUtils::textFileHashCode(const QString& file_name) {
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
        return -1;
    QString file_contents = file.readAll();
    return qHash(file_contents);
}

bool Qtilities::Core::FileUtils::compareTextFiles(const QString& file1, const QString& file2) {
    int original_xml = FileUtils::textFileHashCode(file1);
    int readback_xml = FileUtils::textFileHashCode(file2);
    if (original_xml == -1 || readback_xml == -1)
        return false;
    return (original_xml == readback_xml);
}

bool Qtilities::Core::FileUtils::compareBinaryFiles(const QString& file1, const QString& file2) {
    QFile fileA(file1);
    if (!fileA.open(QIODevice::ReadOnly))
        return false;
    QFile fileB(file2);
    if (!fileB.open(QIODevice::ReadOnly))
        return false;

    while (!fileA.atEnd() || !fileB.atEnd()) {
        QByteArray lineA = fileA.readLine();
        QByteArray lineB = fileB.readLine();
        if (lineA != lineB)
            return false;
    }

    return true;
}
