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

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QString& category_level_name, int access_mode)  {
    if (!category_level_name.isEmpty())
        addLevel(category_level_name);
    access_mode = access_mode;
}

Qtilities::Core::QtilitiesCategory::QtilitiesCategory(const QStringList& category_name_list) {
    foreach(QString level,category_name_list)
        addLevel(level);
    d_access_mode = 3;
}

bool Qtilities::Core::QtilitiesCategory::exportBinary(QDataStream& stream) const {
    stream << (quint32) d_access_mode;
    stream << (quint32) d_category_levels.count();
    for (int i = 0; i < d_category_levels.count(); i++) {
        if (!d_category_levels.at(i).exportBinary(stream))
            return false;
    }
    return true;
}

bool Qtilities::Core::QtilitiesCategory::importBinary(QDataStream& stream) {
    quint32 ui32;
    stream >> d_access_mode;
    stream >> ui32;
    int count_int = ui32;
    for (int i = 0; i < count_int; i++) {
        CategoryLevel category_level(stream);
        d_category_levels.push_back(category_level);
    }
    return true;
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
