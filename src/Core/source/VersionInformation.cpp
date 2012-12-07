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

#include "VersionInformation.h"

#include <QStringList>

// ------------------------------------
// VersionNumber
// ------------------------------------

struct Qtilities::Core::VersionNumberPrivateData {
    VersionNumberPrivateData()
        : version_major(0),
          version_minor(0),
          version_revision(0),
          field_width_minor(-1),
          field_width_revision(-1),
          is_version_minor_used(true),
          is_version_revision_used(true) {}

    int         version_major;
    int         version_minor;
    int         version_revision;
    int         field_width_minor;
    int         field_width_revision;
    bool        is_version_minor_used;
    bool        is_version_revision_used;

};

Qtilities::Core::VersionNumber::VersionNumber() {
    d = new VersionNumberPrivateData;
    d->version_major = 0;
    d->version_minor = 0;
    d->version_revision = 0;
}

Qtilities::Core::VersionNumber::VersionNumber(int major, int minor, int revision) {
    d = new VersionNumberPrivateData;
    d->version_major = major;
    d->version_minor = minor;
    d->version_revision = revision;
}

Qtilities::Core::VersionNumber::VersionNumber(const QString& version, const QString& seperator) {
    d = new VersionNumberPrivateData;

    fromString(version, seperator);
}

Qtilities::Core::VersionNumber::VersionNumber(const VersionNumber& ref)  {
    d = new VersionNumberPrivateData;
    d->version_major = ref.versionMajor();
    d->version_minor = ref.versionMinor();
    d->version_revision = ref.versionRevision();
}

Qtilities::Core::VersionNumber::~VersionNumber() {
    delete d;
}

bool Qtilities::Core::VersionNumber::operator==(const VersionNumber& ref) const {
    if (d->version_major != ref.versionMajor())
        return false;
    if (d->is_version_minor_used) {
        if (d->version_minor != ref.versionMinor())
            return false;
    }
    if (d->is_version_revision_used) {
        if (d->version_revision != ref.versionRevision())
            return false;
    }

    return true;
}

bool Qtilities::Core::VersionNumber::operator!=(const VersionNumber& ref) const {
    return !(*this==ref);
}

Qtilities::Core::VersionNumber& Qtilities::Core::VersionNumber::operator=(const VersionNumber& ref) {
    if (this==&ref) return *this;

    d->version_major = ref.versionMajor();
    d->version_minor = ref.versionMinor();
    d->version_revision = ref.versionRevision();
    d->field_width_minor = ref.fieldWidthMinor();
    d->field_width_revision = ref.fieldWidthRevision();
    d->is_version_minor_used = ref.isVersionMinorUsed();
    d->is_version_revision_used = ref.isVersionRevisionUsed();

    return *this;
}

bool Qtilities::Core::VersionNumber::operator>(const VersionNumber& ref) {
    if (d->version_major > ref.versionMajor())
        return true;
    if (d->is_version_minor_used) {
        if (d->version_minor > ref.versionMinor())
            return true;
    }
    if (d->is_version_revision_used) {
        if (d->version_revision > ref.versionRevision())
            return true;
    }

    return false;
}

bool Qtilities::Core::VersionNumber::operator>=(const VersionNumber& ref) {
    if (*this==ref)
        return true;
    else
        return (*this > ref);
}

bool Qtilities::Core::VersionNumber::operator<(const VersionNumber& ref) {
    if (d->is_version_revision_used) {
        // Handle case: 2.5.3 < 1.5.3
        if (d->version_major < ref.versionMajor())
            return true;
        // Handle case: 1.4.3 < 1.5.3
        if (d->version_minor < ref.versionMinor())
            return true;
        // Handle case: 1.5.2 < 1.5.3
        if (d->version_revision < ref.versionRevision())
            return true;
        // All other cases
        return false;
    }

    if (d->is_version_minor_used) {
        // Handle case: 2.5.3 < 1.5.3
        if (d->version_major < ref.versionMajor())
            return true;
        // Handle case: 1.4.3 < 1.5.3
        if (d->version_minor < ref.versionMinor())
            return true;
        // All other cases
        return false;
    }

    if (d->version_major < ref.versionMajor())
        return true;

    return false;
}

bool Qtilities::Core::VersionNumber::operator<=(const VersionNumber& ref) {
    if (*this==ref)
        return true;
    else
        return (*this < ref);
}

int Qtilities::Core::VersionNumber::versionMajor() const {
    return d->version_major;
}

void Qtilities::Core::VersionNumber::setVersionMajor(int version) {
    d->version_major = version;
}

int Qtilities::Core::VersionNumber::versionMinor() const {
    return d->version_minor;
}

void Qtilities::Core::VersionNumber::setVersionMinor(int version) {
    d->version_minor = version;
}

int Qtilities::Core::VersionNumber::fieldWidthMinor() const {
    return d->field_width_minor;
}

void Qtilities::Core::VersionNumber::setFieldWidthMinor(int field_width) {
    d->field_width_minor = field_width;
}

bool Qtilities::Core::VersionNumber::isVersionMinorUsed() const {
    return d->is_version_minor_used;
}

void Qtilities::Core::VersionNumber::setIsVersionMinorUsed(bool is_used) {
    d->is_version_minor_used = is_used;
}

int Qtilities::Core::VersionNumber::versionRevision() const {
    return d->version_revision;
}

void Qtilities::Core::VersionNumber::setVersionRevision(int version) {
    d->version_revision = version;
}

int Qtilities::Core::VersionNumber::fieldWidthRevision() const {
    return d->field_width_revision;
}

void Qtilities::Core::VersionNumber::setFieldWidthRevision(int field_width) {
    d->field_width_revision = field_width;
}

bool Qtilities::Core::VersionNumber::isVersionRevisionUsed() const {
    return d->is_version_revision_used;
}

void Qtilities::Core::VersionNumber::setIsVersionRevisionUsed(bool is_used) {
    d->is_version_revision_used = is_used;
}

QString Qtilities::Core::VersionNumber::toString(const QString& seperator) const {
    QString version;
    if (d->is_version_minor_used && d->is_version_revision_used) {
        if (d->field_width_minor == -1 && d->field_width_revision == -1)
            version = QString::number(d->version_major) + seperator + QString("%1").arg(d->version_minor) + seperator + QString("%1").arg(d->version_revision);
        else if (d->field_width_minor == -1 && !(d->field_width_revision == -1))
            version = QString::number(d->version_major) + seperator + QString("%1").arg(d->version_minor) + seperator + QString("%1").arg(d->version_revision,d->field_width_revision,10,QChar('0'));
        else if (d->field_width_minor != -1 && d->field_width_revision == -1)
            version = QString::number(d->version_major) + seperator + QString("%1").arg(d->version_minor,d->field_width_minor,10,QChar('0')) + seperator + QString("%1").arg(d->version_revision);
        else if (d->field_width_minor != -1 && d->field_width_revision != -1)
            version = QString::number(d->version_major) + seperator + QString("%1").arg(d->version_minor,d->field_width_minor,10,QChar('0')) + seperator + QString("%1").arg(d->version_revision,d->field_width_revision,10,QChar('0'));
    } else if (d->is_version_minor_used && !d->is_version_revision_used) {
        if (d->field_width_minor == -1)
            version = QString::number(d->version_major) + seperator + QString("%1").arg(d->version_minor);
        else if (d->field_width_minor != -1)
            version = QString::number(d->version_major) + seperator + QString("%1").arg(d->version_minor,d->field_width_minor,10,QChar('0'));
    } else if (!d->is_version_minor_used && !d->is_version_revision_used)
        version = QString::number(d->version_major);

    return version;
}

void Qtilities::Core::VersionNumber::fromString(const QString& version, const QString& seperator)
{
    QStringList list = version.split(seperator);
    if (list.count() >= 1)
        d->version_major = list.at(0).toInt();
    if (list.count() >= 2)
        d->version_minor = list.at(1).toInt();
    if (list.count() >= 3)
        d->version_revision = list.at(2).toInt();
}

// ------------------------------------
// VersionInformation
// ------------------------------------

struct Qtilities::Core::VersionInformationPrivateData {
    VersionInformationPrivateData() {}

    VersionNumber version;
    QList<VersionNumber> supported_versions;
};

Qtilities::Core::VersionInformation::VersionInformation(const VersionNumber& version, QList<VersionNumber> supported_versions) {
    d = new VersionInformationPrivateData;
    d->version = version;
    d->supported_versions = supported_versions;
}

Qtilities::Core::VersionInformation::VersionInformation(int major, int minor, int revision) {
    d = new VersionInformationPrivateData;
    VersionNumber version(major,minor,revision);
    d->version = version;
}

Qtilities::Core::VersionInformation::VersionInformation(const VersionInformation& ref)  {
    d = new VersionInformationPrivateData;
    d->version = ref.version();
    d->supported_versions = ref.supportedVersions();
}

Qtilities::Core::VersionInformation::~VersionInformation() {
    delete d;
}

Qtilities::Core::VersionNumber Qtilities::Core::VersionInformation::version() const {
    return d->version;
}

bool Qtilities::Core::VersionInformation::hasSupportedVersions() const {
    return !d->supported_versions.isEmpty();
}

QList<Qtilities::Core::VersionNumber> Qtilities::Core::VersionInformation::supportedVersions() const {
    return d->supported_versions;
}

void Qtilities::Core::VersionInformation::addSupportedVersion(const VersionNumber& version_number) {
    d->supported_versions.append(version_number);
}

bool Qtilities::Core::VersionInformation::isSupportedVersion(const VersionNumber& version_number) const {
    return d->supported_versions.contains(version_number);
}

bool Qtilities::Core::VersionInformation::isSupportedVersion(const QString& version_string, const QString& seperator) const {
    VersionNumber version_number(version_string,seperator);
    return d->supported_versions.contains(version_number);
}

QStringList Qtilities::Core::VersionInformation::supportedVersionString() const {
    QStringList version_strings;
    foreach (const VersionNumber& number, d->supported_versions)
        version_strings << number.toString();
    return version_strings;
}

