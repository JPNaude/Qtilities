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
        return FileUtils::toNativeSeparators(QDir::cleanPath(d->relative_to_path + QDir::separator() + path()));
    } else
        return QString();
}

QString Qtilities::Core::QtilitiesFileInfo::absoluteToRelativeFilePath() const {
    if (isRelative()) {
        return FileUtils::toNativeSeparators(QDir::cleanPath(d->relative_to_path + QDir::separator() + filePath()));
    } else
        return QString();
}

void Qtilities::Core::QtilitiesFileInfo::setFileName(const QString& new_file_name) {
    setFile(FileUtils::toNativeSeparators(path() + QDir::separator() + new_file_name));
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
    return FileUtils::comparePaths(actualFilePath(),ref.absoluteFilePath());
}

bool Qtilities::Core::QtilitiesFileInfo::updateRelativeToPath(const QString &old_relative_to_path, const QString &new_relative_to_path) {
    if (FileUtils::comparePaths(old_relative_to_path,d->relative_to_path)) {
        setRelativeToPath(new_relative_to_path);
        return true;
    }
    return false;
}

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
        valid = !file_name.contains(QDir::separator());

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
