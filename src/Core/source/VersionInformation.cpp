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
          is_version_revision_used(true),
          development_stage(VersionNumber::DevelopmentStageNone),
          version_development_stage(0) {}

    int         version_major;
    int         version_minor;
    int         version_revision;
    int         field_width_minor;
    int         field_width_revision;
    bool        is_version_minor_used;
    bool        is_version_revision_used;

    VersionNumber::DevelopmentStage development_stage;
    QString                         development_stage_identifier;
    int                             version_development_stage;
};

Qtilities::Core::VersionNumber::VersionNumber() {
    d = new VersionNumberPrivateData;
    d->version_major = 0;
    d->version_minor = 0;
    d->version_revision = 0;
}

Qtilities::Core::VersionNumber::VersionNumber(int major, int minor, int revision, int stage_version, DevelopmentStage development_stage) {
    d = new VersionNumberPrivateData;
    d->version_major = major;
    d->version_minor = minor;
    d->version_revision = revision;
    d->version_development_stage = stage_version;
    d->development_stage = development_stage;
    d->development_stage_identifier = defaultDevelopmentStageIdentifer(d->development_stage);
}

Qtilities::Core::VersionNumber::VersionNumber(const QString& version, const QString& separator, DevelopmentStage development_stage, const QString& stage_identifier) {
    d = new VersionNumberPrivateData;
    d->development_stage = development_stage;
    if (stage_identifier.isEmpty())
        d->development_stage_identifier = defaultDevelopmentStageIdentifer(d->development_stage);
    else
        d->development_stage_identifier = stage_identifier;
    fromString(version,separator,d->development_stage_identifier,d->development_stage);
}

Qtilities::Core::VersionNumber::VersionNumber(const VersionNumber& ref)  {
    d = new VersionNumberPrivateData;
    d->version_major = ref.versionMajor();
    d->version_minor = ref.versionMinor();
    d->version_revision = ref.versionRevision();

    d->field_width_minor = ref.fieldWidthMinor();
    d->field_width_revision = ref.fieldWidthRevision();
    d->is_version_minor_used = ref.isVersionMinorUsed();
    d->is_version_revision_used = ref.isVersionRevisionUsed();

    d->development_stage = ref.developmentStage();
    d->development_stage_identifier = ref.developmentStageIdentifier();
    d->version_development_stage = ref.versionDevelopmentStage();
}

Qtilities::Core::VersionNumber::~VersionNumber() {
    delete d;
}

bool Qtilities::Core::VersionNumber::operator==(const VersionNumber& ref) const {
    if (d->version_major != ref.versionMajor())
        return false;
    if (d->is_version_minor_used && ref.isVersionMinorUsed()) {
        if (d->version_minor != ref.versionMinor())
            return false;
    }
    if (d->is_version_revision_used && ref.isVersionRevisionUsed()) {
        if (d->version_revision != ref.versionRevision())
            return false;
    }
    if (developmentStage(true) != ref.developmentStage(true))
        return false;
    // We can just check developmentStage(true) since we already know ref has the same stage:
    if (developmentStage(true) != DevelopmentStageNone) {
        if (d->version_development_stage != ref.versionDevelopmentStage())
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

bool Qtilities::Core::VersionNumber::operator>(const VersionNumber& ref) const {
    if (*this == ref)
        return false;
    else
        return !(*this < ref);
}

bool Qtilities::Core::VersionNumber::operator>=(const VersionNumber& ref) const {
    if (*this==ref)
        return true;
    else
        return (*this > ref);
}

bool Qtilities::Core::VersionNumber::operator<(const VersionNumber& ref) const {
    if (*this == ref)
        return false;

    if (developmentStage(true) != DevelopmentStageNone || ref.developmentStage(true) != DevelopmentStageNone) {
        // Handle case: 1.5.3 < 2.5.3
        if (d->version_major < ref.versionMajor())
            return true;
        // Handle case: 1.4.3 < 1.5.3 and 2.4.3 < 1.5.3
        if (d->version_minor < ref.versionMinor() && d->version_major == ref.versionMajor())
            return true;
        // Handle case: 1.5.2 < 1.5.3 and 2.5.2 < 1.5.3 and 2.6.2 < 1.5.3
        if (d->version_revision < ref.versionRevision() && d->version_minor == ref.versionMinor() && d->version_major == ref.versionMajor())
            return true;

        // If we get here, we know the major, minor and revision version is NOT smaller. Thus, we check if they are the same if and
        // they are we know we can check the development stage versions:
        if (d->version_revision == ref.versionRevision() && d->version_minor == ref.versionMinor() && d->version_major == ref.versionMajor()) {
            if (developmentStage(true) == ref.developmentStage(true)) {
                if (d->version_development_stage < ref.versionDevelopmentStage())
                    return true;
            } else {
                // In this case consider the development stage order:
                if ((int) developmentStage(true) < (int) ref.developmentStage(true))
                    return true;
            }
        }

        return false;
    }

    if (d->is_version_revision_used && ref.isVersionRevisionUsed()) {
        // Handle case: 1.5.3 < 2.5.3
        if (d->version_major < ref.versionMajor())
            return true;
        if (d->is_version_minor_used && ref.isVersionMinorUsed()) {
            // Handle case: 1.4.3 < 1.5.3 and 2.4.3 < 1.5.3
            if (d->version_minor < ref.versionMinor() && d->version_major == ref.versionMajor())
                return true;
        }
        // Handle case: 1.5.2 < 1.5.3 and 2.5.2 < 1.5.3 and 1.6.2 < 1.5.3
        if (d->version_revision < ref.versionRevision() && d->version_minor == ref.versionMinor() && d->version_major == ref.versionMajor())
            return true;
        // All other cases
        return false;
    }

    if (d->is_version_minor_used && ref.isVersionMinorUsed()) {
        // Handle case: 1.5 < 2.5
        if (d->version_major < ref.versionMajor())
            return true;
        // Handle case: 1.4 < 1.5 and 2.4 < 1.5
        if (d->version_minor < ref.versionMinor() && d->version_major == ref.versionMajor())
            return true;
        // All other cases
        return false;
    }

    if (d->version_major < ref.versionMajor())
        return true;

    return false;
}

bool Qtilities::Core::VersionNumber::operator<=(const VersionNumber& ref) const {
    if (*this==ref)
        return true;
    else
        return (*this < ref);
}

bool Qtilities::Core::VersionNumber::isNull() const {
    return (d->version_major == 0 && d->version_minor == 0 && d->version_revision == 0 && d->version_development_stage == 0);
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

QString Qtilities::Core::VersionNumber::developmentStageIdentifier() const {
    return d->development_stage_identifier;
}

void Qtilities::Core::VersionNumber::setDevelopmentStageIdentifier(const QString &identifier) {
    d->development_stage_identifier = identifier;
}

QString Qtilities::Core::VersionNumber::defaultDevelopmentStageIdentifer(VersionNumber::DevelopmentStage stage) {
    if (stage == DevelopmentStageAlpha)
        return "-a";
    else if (stage == DevelopmentStageBeta)
        return "-b";
    else if (stage == DevelopmentStageReleaseCandidate)
        return "-rc";
    else if (stage == DevelopmentStageServicePack)
        return "-sp";

    return QString();
}

Qtilities::Core::VersionNumber::DevelopmentStage Qtilities::Core::VersionNumber::developmentStage(bool check_if_valid) const {
    if (check_if_valid) {
        if (d->version_development_stage != 0)
            return d->development_stage;
        else
            return DevelopmentStageNone;
    } else
        return d->development_stage;
}

void Qtilities::Core::VersionNumber::setDevelopmentStage(Qtilities::Core::VersionNumber::DevelopmentStage stage) {
    if (d->development_stage != stage) {
        if (d->development_stage_identifier == defaultDevelopmentStageIdentifer(d->development_stage))
            d->development_stage_identifier = defaultDevelopmentStageIdentifer(stage);
        d->development_stage = stage;
    }
}

int Qtilities::Core::VersionNumber::versionDevelopmentStage() const {
    return d->version_development_stage;
}

void Qtilities::Core::VersionNumber::setVersionDevelopmentStage(int version) {
    d->version_development_stage = version;
}

QString Qtilities::Core::VersionNumber::toString(const QString& separator) const {
    QString version;
    if (d->is_version_minor_used && d->is_version_revision_used) {
        if (d->field_width_minor == -1 && d->field_width_revision == -1)
            version = QString::number(d->version_major) + separator + QString("%1").arg(d->version_minor) + separator + QString("%1").arg(d->version_revision);
        else if (d->field_width_minor == -1 && !(d->field_width_revision == -1))
            version = QString::number(d->version_major) + separator + QString("%1").arg(d->version_minor) + separator + QString("%1").arg(d->version_revision,d->field_width_revision,10,QChar('0'));
        else if (d->field_width_minor != -1 && d->field_width_revision == -1)
            version = QString::number(d->version_major) + separator + QString("%1").arg(d->version_minor,d->field_width_minor,10,QChar('0')) + separator + QString("%1").arg(d->version_revision);
        else if (d->field_width_minor != -1 && d->field_width_revision != -1)
            version = QString::number(d->version_major) + separator + QString("%1").arg(d->version_minor,d->field_width_minor,10,QChar('0')) + separator + QString("%1").arg(d->version_revision,d->field_width_revision,10,QChar('0'));
    } else if (d->is_version_minor_used && !d->is_version_revision_used) {
        if (d->field_width_minor == -1)
            version = QString::number(d->version_major) + separator + QString("%1").arg(d->version_minor);
        else if (d->field_width_minor != -1)
            version = QString::number(d->version_major) + separator + QString("%1").arg(d->version_minor,d->field_width_minor,10,QChar('0'));
    } else if (!d->is_version_minor_used && !d->is_version_revision_used)
        version = QString::number(d->version_major);

    if (d->development_stage != DevelopmentStageNone && d->version_development_stage != 0) {
        version.append(d->development_stage_identifier);
        version.append(QString::number(d->version_development_stage));
    }

    return version;
}

void Qtilities::Core::VersionNumber::fromString(const QString& version, const QString& separator, const QString& stage_identifier, DevelopmentStage stage) {
    QString cleaned_version = version.toLower();
    cleaned_version.replace(" ","");

    QStringList list = cleaned_version.split(separator,QString::SkipEmptyParts);
    if (list.isEmpty())
        return;

    QString local_stage_identifier = stage_identifier.toLower();
    if (local_stage_identifier.isEmpty())
        local_stage_identifier = d->development_stage_identifier.toLower();
    DevelopmentStage local_stage = stage;
    if (local_stage == DevelopmentStageNone)
        local_stage = d->development_stage;

    bool conv_ok;
    if (list.count() >= 1) {
        // We need to handle for example: 11sp1
        QString major_string = list.at(0);
        if (major_string.contains(local_stage_identifier) && !local_stage_identifier.isEmpty() && local_stage != DevelopmentStageNone) {
            QStringList revision_split = major_string.split(local_stage_identifier,QString::SkipEmptyParts);
            if (revision_split.count() > 0)
                major_string = revision_split.front();
        }
        int tmp_int = major_string.toInt(&conv_ok);
        if (conv_ok)
            d->version_major = tmp_int;
    }
    if (list.count() >= 2) {
        // We need to handle for example: 11.0sp1
        QString minor_string = list.at(1);
        if (minor_string.contains(local_stage_identifier) && !local_stage_identifier.isEmpty() && local_stage != DevelopmentStageNone) {
            QStringList revision_split = minor_string.split(local_stage_identifier,QString::SkipEmptyParts);
            if (revision_split.count() > 0)
                minor_string = revision_split.front();
        }
        int tmp_int = minor_string.toInt(&conv_ok);
        if (conv_ok)
            d->version_minor = tmp_int;
    }
    if (list.count() >= 3) {
        // We need to handle for example: 11.0.0sp1
        QString revision_string = list.at(2);
        if (revision_string.contains(local_stage_identifier) && !local_stage_identifier.isEmpty() && local_stage != DevelopmentStageNone) {
            QStringList revision_split = revision_string.split(local_stage_identifier,QString::SkipEmptyParts);
            if (revision_split.count() > 0)
                revision_string = revision_split.front();
        }
        int tmp_int = revision_string.toInt(&conv_ok);
        if (conv_ok)
            d->version_revision = tmp_int;
    }

    // Next, see if a stage type was specified:
    if (local_stage != DevelopmentStageNone) {
        QStringList stage_split_list = list.last().split(local_stage_identifier,QString::SkipEmptyParts);
        if (stage_split_list.count() == 2) {
            int tmp_int = stage_split_list.last().toInt(&conv_ok);
            if (conv_ok)
                d->version_development_stage = tmp_int;
        }
    }
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

bool Qtilities::Core::VersionInformation::isSupportedVersion(const QString& version_string, const QString& separator) const {
    VersionNumber version_number(version_string,separator);
    return d->supported_versions.contains(version_number);
}

QStringList Qtilities::Core::VersionInformation::supportedVersionString() const {
    QStringList version_strings;
    foreach (const VersionNumber& number, d->supported_versions)
        version_strings << number.toString();
    return version_strings;
}

