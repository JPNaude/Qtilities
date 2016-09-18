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

#include "QtilitiesCategory.h"

#include <Logger.h>

#include <QDomElement>

// -----------------------------------------
// CategoryLevel
// -----------------------------------------
Qtilities::Core::IExportable::ExportModeFlags Qtilities::Core::CategoryLevel::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::IExportable::ExportResultFlags Qtilities::Core::CategoryLevel::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << d_name;
    return IExportable::Complete;
}

Qtilities::Core::IExportable::ExportResultFlags Qtilities::Core::CategoryLevel::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream >> d_name;
    return IExportable::Complete;
}

Qtilities::Core::IExportable::ExportResultFlags Qtilities::Core::CategoryLevel::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    Q_UNUSED(doc)

    object_node->setAttribute("Name",d_name);
    return IExportable::Complete;
}

Qtilities::Core::IExportable::ExportResultFlags Qtilities::Core::CategoryLevel::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (object_node->hasAttribute("Name")) {
        d_name = object_node->attribute("Name");
        return IExportable::Complete;
    }

    return IExportable::Failed;
}

// -----------------------------------------
// QtilitiesCategory
// -----------------------------------------

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QString& category_level_name) : IExportable()  {
    if (!category_level_name.isEmpty())
        addLevel(category_level_name);
    d_access_mode = 3;
    //d_category_icon = 0;
}

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QString& category_levels, const QString& separator) : IExportable() {
    QStringList category_name_list = category_levels.split(separator,QString::SkipEmptyParts);
    foreach(QString level,category_name_list) {
        if (level.trimmed().length() > 0)
            addLevel(level);
    }
    d_access_mode = 3;
    //d_category_icon = 0;
}

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QStringList& category_name_list) : IExportable() {
    foreach(QString level,category_name_list)
        addLevel(level);
    d_access_mode = 3;
    //d_category_icon = 0;
}

QtilitiesCategory& Qtilities::Core::QtilitiesCategory::operator=(const QtilitiesCategory& other) {
    if (this==&other) return *this;

    d_category_levels = other.categoryLevels();
    d_access_mode = other.accessMode();
//    if (!other.categoryIcon().isNull())
//        d_category_icon = new QIcon(other.categoryIcon());

    return *this;
}

bool Qtilities::Core::QtilitiesCategory::operator==(const QtilitiesCategory& ref) const {
    return d_category_levels == ref.categoryLevels();
}

bool Qtilities::Core::QtilitiesCategory::operator!=(const QtilitiesCategory& ref) const {
    return !(*this==ref);
}

QString Qtilities::Core::QtilitiesCategory::toString(const QString& join_string) const {
    QString category_string;
    int count = d_category_levels.count();
    for (int i = 0; i < count; ++i) {
        if (i > 0)
            category_string.append(join_string);
        category_string.append(d_category_levels.at(i).d_name);
    }
    return category_string;
}

QStringList Qtilities::Core::QtilitiesCategory::toStringList(int level) const {
    if (level == -1)
        level = d_category_levels.count();

    QStringList category_string_list;
    int level_counter = 0;
    for (int i = 0; i < d_category_levels.count(); ++i) {
        if (level_counter < level) {
            ++level_counter;
            category_string_list.push_back(d_category_levels.at(i).d_name);
        } else {
            break;
        }
    }
    return category_string_list;
}

void Qtilities::Core::QtilitiesCategory::addLevel(const QString& name) {
    CategoryLevel category_level(name);
    d_category_levels.push_back(category_level);
}

void Qtilities::Core::QtilitiesCategory::addLevel(CategoryLevel category_level) {
    d_category_levels.push_back(category_level);
}

void Qtilities::Core::QtilitiesCategory::setExportVersion(Qtilities::ExportVersion version) {
    IExportable::setExportVersion(version);

    // This is a bad way to do it... Fix sometime.
    QList<CategoryLevel> new_levels;
    for (int i = 0; i < d_category_levels.count(); ++i) {
        CategoryLevel level(d_category_levels.at(i));
        level.setExportVersion(exportVersion());
        new_levels << level;
    }

    d_category_levels.clear();
    d_category_levels.append(new_levels);
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::QtilitiesCategory::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesCategory::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << (quint32) accessMode();
    stream << (quint32) categoryDepth();
    bool all_successful = true;
    for (int i = 0; i < categoryDepth(); ++i) {
        if (categoryLevels().at(i).exportBinary(stream) != IExportable::Complete)
            all_successful = false;
    }
    if (all_successful)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesCategory::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    quint32 ui32;
    stream >> ui32;
    setAccessMode(ui32);
    stream >> ui32;
    int count_int = ui32;
    for (int i = 0; i < count_int; ++i) {
        Qtilities::Core::CategoryLevel category_level(stream,exportVersion());
        addLevel(category_level);
    }

    if (categoryDepth() == (int) count_int)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesCategory::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    object_node->setAttribute("AccessMode",d_access_mode);
    object_node->setAttribute("Depth",d_category_levels.count());
    bool all_successful = true;
    for (int i = 0; i < d_category_levels.count(); ++i) {
        QDomElement category_level = doc->createElement("CategoryLevel_" + QString::number(i));
        object_node->appendChild(category_level);
        if (d_category_levels.at(i).exportXml(doc,&category_level) != IExportable::Complete)
            all_successful = false;
    }

    if (all_successful)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::QtilitiesCategory::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    int depth_readback = 0;

    if (object_node->hasAttribute("AccessMode"))
        d_access_mode = object_node->attribute("AccessMode").toInt();
    if (object_node->hasAttribute("Depth"))
        depth_readback = object_node->attribute("Depth").toInt();

    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); ++i)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName().startsWith("CategoryLevel")) {
            CategoryLevel category_level;
            category_level.setExportVersion(exportVersion());
            category_level.importXml(doc,&child,import_list);
            addLevel(category_level);
            continue;
        }
    }

    if (categoryDepth() == depth_readback)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::CategoryLevel& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::CategoryLevel& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::QtilitiesCategory& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::QtilitiesCategory& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}
