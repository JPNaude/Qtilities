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

#include "QtilitiesFileInfo.h"
#include "FileUtils.h"

#include <QDir>

struct Qtilities::Core::QtilitiesFileInfoPrivateData {
    QtilitiesFileInfoPrivateData() : relative_to_path(QString()) {}

    //! The relative to path of this file.
    QString relative_to_path;
};

Qtilities::Core::QtilitiesFileInfo::QtilitiesFileInfo(const QString& file, const QString& relative_to_path) : QFileInfo(file) {
    d = new QtilitiesFileInfoPrivateData;
    d->relative_to_path = relative_to_path;
}

Qtilities::Core::QtilitiesFileInfo::QtilitiesFileInfo(const QtilitiesFileInfo& ref) : QFileInfo(ref) {
    d = new QtilitiesFileInfoPrivateData;
    d->relative_to_path = ref.relativeToPath();
}

bool Qtilities::Core::QtilitiesFileInfo::operator==(const Qtilities::Core::QtilitiesFileInfo &ref) const {
    return actualFilePath() == ref.actualFilePath();
}

bool Qtilities::Core::QtilitiesFileInfo::operator!=(const Qtilities::Core::QtilitiesFileInfo &ref) const {
    return !(*this==ref);
}

Qtilities::Core::QtilitiesFileInfo::~QtilitiesFileInfo() {
    delete d;
}

bool Qtilities::Core::QtilitiesFileInfo::hasRelativeToPath() const {
    return !d->relative_to_path.isEmpty();
}

QString Qtilities::Core::QtilitiesFileInfo::relativeToPath() const {
    return d->relative_to_path;
}

void Qtilities::Core::QtilitiesFileInfo::setRelativeToPath(const QString& relative_to_path) {
    d->relative_to_path = relative_to_path;
}

QString Qtilities::Core::QtilitiesFileInfo::absoluteToRelativePath() const {
    if (isRelative()) {
        return QDir::toNativeSeparators(QDir::cleanPath(d->relative_to_path + "/" + path()));
    } else
        return QString();
}

QString Qtilities::Core::QtilitiesFileInfo::absoluteToRelativeFilePath() const {
    if (isRelative()) {
        return QDir::toNativeSeparators(QDir::cleanPath(d->relative_to_path + "/" + filePath()));
    } else
        return QString();
}

void Qtilities::Core::QtilitiesFileInfo::setFileName(const QString& new_file_name) {
    setFile(QDir::toNativeSeparators(path() + "/" + new_file_name));
}

QString Qtilities::Core::QtilitiesFileInfo::actualPath() const {
    if (isAbsolute()) {
        return path();
    } else if (isRelative()) {
        if (hasRelativeToPath())
            return absoluteToRelativePath();
    }

    return path();
}

QString Qtilities::Core::QtilitiesFileInfo::actualFilePath() const {
    if (isAbsolute()) {
        return filePath();
    } else if (isRelative()) {
        if (hasRelativeToPath())
            return absoluteToRelativeFilePath();
    }

    return filePath();
}

bool Qtilities::Core::QtilitiesFileInfo::compareActualFilePaths(const QtilitiesFileInfo& ref) const {
    QString path1 = QDir::toNativeSeparators(QDir::cleanPath(actualFilePath()));
    QString path2 = QDir::toNativeSeparators(QDir::cleanPath(ref.actualFilePath()));

    if (path1.compare(path2,Qt::CaseInsensitive) == 0)
        return true;
    else
        return false;
}

bool Qtilities::Core::QtilitiesFileInfo::updateRelativeToPath(const QString &old_relative_to_path, const QString &new_relative_to_path) {
    if (FileUtils::comparePaths(old_relative_to_path,d->relative_to_path)) {
        setRelativeToPath(new_relative_to_path);
        return true;
    }
    return false;
}

#ifdef Q_OS_WIN
bool Qtilities::Core::QtilitiesFileInfo::isValidFileName(const QString file_name) {
    bool valid = !file_name.isEmpty();

    if (valid)
        valid = !file_name.contains(":");

    if (valid)
        valid = !file_name.contains("*");

    if (valid)
        valid = !file_name.contains("?");

    if (valid)
        valid = !file_name.contains("\"");

    if (valid)
        valid = !file_name.contains("/");

    if (valid)
        valid = !file_name.contains("\\");

    if (valid)
        valid = !file_name.contains("<");

    if (valid)
        valid = !file_name.contains(">");

    if (valid)
        valid = !file_name.contains("|");

    return valid;
}

bool Qtilities::Core::QtilitiesFileInfo::isValidFilePath(const QString file_path) {   
    bool valid = !file_path.isEmpty();

    if (valid)
        valid = !file_path.contains("*");

    if (valid)
        valid = !file_path.contains("?");

    if (valid)
        valid = !file_path.contains("\"");

    if (valid)
        valid = !file_path.contains("<");

    if (valid)
        valid = !file_path.contains(">");

    if (valid)
        valid = !file_path.contains("|");

    return valid;
}
#endif
