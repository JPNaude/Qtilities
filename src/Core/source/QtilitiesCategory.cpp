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

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::exportBinary(QDataStream& stream) const {
    stream << d_name;
    return IExportable::Complete;
}

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    stream >> d_name;
    return IExportable::Complete;
}

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    object_node->setAttribute("Name",d_name);
    return IExportable::Complete;
}

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    if (object_node->hasAttribute("Name")) {
        d_name = object_node->attribute("Name");
        return IExportable::Complete;
    }

    return IExportable::Failed;
}

// -----------------------------------------
// QtilitiesCategory
// -----------------------------------------

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QString& category_level_name, int access_mode)  {
    if (!category_level_name.isEmpty())
        addLevel(category_level_name);
    d_access_mode = access_mode;
}

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QString& category_levels, const QString& seperator) {
    QStringList category_name_list = category_levels.split(seperator,QString::SkipEmptyParts);
    foreach(QString level,category_name_list) {
        if (level.trimmed().length() > 0)
            addLevel(level);
    }
    d_access_mode = 3;
}

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QStringList& category_name_list) {
    foreach(QString level,category_name_list)
        addLevel(level);
    d_access_mode = 3;
}

void Qtilities::Core::QtilitiesCategory::operator=(const QtilitiesCategory& other) {
    d_category_levels = other.categoryLevels();
    d_access_mode = other.accessMode();
}

bool Qtilities::Core::QtilitiesCategory::operator==(const QtilitiesCategory& ref) const {
    return d_category_levels == ref.categoryLevels();
}

bool Qtilities::Core::QtilitiesCategory::operator!=(const QtilitiesCategory& ref) const {
    return !(*this==ref);
}

QString Qtilities::Core::QtilitiesCategory::toString(const QString& join_string) const {
    QStringList category_string_list = toStringList();
    return category_string_list.join(join_string);
}

QStringList Qtilities::Core::QtilitiesCategory::toStringList(int level) const {
    if (level == -1)
        level = d_category_levels.count();

    QStringList category_string_list;
    int level_counter = 0;
    for (int i = 0; i < d_category_levels.count(); i++) {
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
    for (int i = 0; i < d_category_levels.count(); i++) {
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

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::exportBinary(QDataStream& stream) const {
    stream << (quint32) accessMode();
    stream << (quint32) categoryDepth();
    bool all_successfull = true;
    for (int i = 0; i < categoryDepth(); i++) {
        if (categoryLevels().at(i).exportBinary(stream) != IExportable::Complete)
            all_successfull = false;
    }
    if (all_successfull)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)
     
    quint32 ui32;
    stream >> ui32;
    setAccessMode(ui32);
    stream >> ui32;
    int count_int = ui32;
    for (int i = 0; i < count_int; i++) {
        Qtilities::Core::CategoryLevel category_level(stream,exportVersion());
        addLevel(category_level);
    }

    if (categoryDepth() == (int) count_int)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    object_node->setAttribute("AccessMode",d_access_mode);
    object_node->setAttribute("Depth",d_category_levels.count());
    bool all_successfull = true;
    for (int i = 0; i < d_category_levels.count(); i++) {
        QDomElement category_level = doc->createElement("CategoryLevel_" + QString::number(i));
        object_node->appendChild(category_level);
        if (d_category_levels.at(i).exportXml(doc,&category_level) != IExportable::Complete)
                all_successfull = false;
    }

    if (all_successfull)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc) 
    Q_UNUSED(import_list)

    int depth_readback = 0;

    if (object_node->hasAttribute("AccessMode"))
        d_access_mode = object_node->attribute("AccessMode").toInt();
    if (object_node->hasAttribute("Depth"))
        depth_readback = object_node->attribute("Depth").toInt();

    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); i++)
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
