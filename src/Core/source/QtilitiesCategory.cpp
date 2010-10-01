/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::CategoryLevel::instanceFactoryInfo() const {
    return InstanceFactoryInfo();
}

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    stream << d_name;
    return IExportable::Complete;
}

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    stream >> d_name;
    return IExportable::Complete;
}

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    QDomElement category_level_item = doc->createElement("CategoryLevel");
    object_node->appendChild(category_level_item);
    category_level_item.setAttribute("Name",d_name);
    return IExportable::Complete;
}

Qtilities::Core::IExportable::Result Qtilities::Core::CategoryLevel::importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(params)
    Q_UNUSED(import_list)

    QDomNodeList dataNodes = object_node->childNodes();
    for(int i = 0; i < dataNodes.count(); i++)
    {
        QDomNode dataChildNode = dataNodes.item(i);
        QDomElement dataChild = dataChildNode.toElement();

        if (dataChild.isNull())
            continue;

        if (dataChild.tagName() == "CategoryLevel") {
            d_name = dataChild.attribute("Name");
            continue;
        }
    }
    return IExportable::Complete;
}

// -----------------------------------------
// QtilitiesCategory
// -----------------------------------------

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QString& category_level_name, int access_mode)  {
    if (!category_level_name.isEmpty())
        addLevel(category_level_name);
    access_mode = access_mode;
}

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QString& category_levels, const QString& seperator) {
    QStringList category_name_list = category_levels.split(seperator);
    foreach(QString level,category_name_list)
        addLevel(level);
    d_access_mode = 3;
}

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QStringList& category_name_list) {
    foreach(QString level,category_name_list)
        addLevel(level);
    d_access_mode = 3;
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

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::QtilitiesCategory::instanceFactoryInfo() const {
    return instanceFactoryInfo();
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::QtilitiesCategory::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    stream << (quint32) d_access_mode;
    stream << (quint32) d_category_levels.count();
    IExportable::Result result = IExportable::Complete;
    for (int i = 0; i < d_category_levels.count(); i++) {
        IExportable::Result level_result = d_category_levels.at(i).exportBinary(stream);
        if (level_result == IExportable::Failed)
            return level_result;
        if (level_result == IExportable::Incomplete && result == IExportable::Complete)
            result = level_result;
    }
    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(import_list)
    Q_UNUSED(params)

    quint32 ui32;
    stream >> d_access_mode;
    stream >> ui32;
    int count_int = ui32;
    for (int i = 0; i < count_int; i++) {
        CategoryLevel category_level(stream);
        d_category_levels.push_back(category_level);
    }

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    QDomElement category_item = doc->createElement("Category");
    object_node->appendChild(category_item);
    category_item.setAttribute("AccessMode",d_access_mode);
    for (int i = 0; i < d_category_levels.count(); i++)
        d_category_levels.at(i).exportXML(doc,&category_item);

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::QtilitiesCategory::importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(params)
    Q_UNUSED(import_list)

    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); i++)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "Category") {
            d_access_mode = child.attribute("AccessMode").toInt();
            QDomNodeList levelNodes = child.childNodes();
            for(int i = 0; i < levelNodes.count(); i++)
            {
                QDomNode levelNode = levelNodes.item(i);
                QDomElement level = levelNode.toElement();

                if (level.isNull())
                    continue;

                if (level.tagName() == "Category") {
                    CategoryLevel category_level;
                    category_level.importXML(doc,&level,import_list);
                    continue;
                }
            }
            continue;
        }
    }
    return IExportable::Complete;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::QtilitiesCategory& category) {
    stream << (quint32) category.accessMode();
    stream << (quint32) category.categoryDepth();
    for (int i = 0; i < category.categoryDepth(); i++)
        category.categoryLevels().at(i).exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::QtilitiesCategory& category) {
    quint32 ui32;
    stream >> ui32;
    category.setAccessMode(ui32);
    stream >> ui32;
    int count_int = ui32;
    for (int i = 0; i < count_int; i++) {
        Qtilities::Core::CategoryLevel category_level(stream);
        category.addLevel(category_level);
    }

    return stream;
}
