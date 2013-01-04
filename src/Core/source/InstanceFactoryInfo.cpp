/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#include "IFactoryProvider.h"
#include "QtilitiesCoreConstants.h"

#include <QtXml>

using namespace Qtilities::Core::Constants;

Qtilities::Core::InstanceFactoryInfo::InstanceFactoryInfo(QDomDocument* doc, QDomElement* object_node,Qtilities::ExportVersion version) {
    importXml(doc,object_node,version);
}

bool Qtilities::Core::InstanceFactoryInfo::isValid() {
    if (d_factory_tag.isEmpty())
        return false;
    if (d_instance_tag.isEmpty())
        return false;
    return true;
}

quint32 MARKER_IFI_CLASS_SECTION = 0xBAADF00D;

bool Qtilities::Core::InstanceFactoryInfo::exportBinary(QDataStream& stream, Qtilities::ExportVersion version) const {
    Q_UNUSED(version)

    stream << MARKER_IFI_CLASS_SECTION;
    stream << d_factory_tag;
    stream << d_instance_tag;
    stream << d_instance_name;
    stream << MARKER_IFI_CLASS_SECTION;
    return true;
}

bool Qtilities::Core::InstanceFactoryInfo::importBinary(QDataStream& stream, Qtilities::ExportVersion version) {
    Q_UNUSED(version)

    // We don't do a version check here. Observer will do it for us.

    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_IFI_CLASS_SECTION) {
        LOG_ERROR(QObject::tr("InstanceFactoryInfo binary import failed to detect start marker. Import will fail: ") + Q_FUNC_INFO);
        return false;
    }
    stream >> d_factory_tag;
    stream >> d_instance_tag;
    stream >> d_instance_name;
    stream >> ui32;
    if (ui32 != MARKER_IFI_CLASS_SECTION) {
        LOG_ERROR(QObject::tr("InstanceFactoryInfo binary import failed to detect end marker. Import will fail: ") + Q_FUNC_INFO);
        return false;
    }
    return true;
}

bool Qtilities::Core::InstanceFactoryInfo::exportXml(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) const {
    Q_UNUSED(version)
    Q_UNUSED(doc)

    if (d_factory_tag != QString(qti_def_FACTORY_QTILITIES))
        object_node->setAttribute("FactoryTag", d_factory_tag);
    object_node->setAttribute("InstanceFactoryInfo", d_instance_tag);
    if (d_instance_tag != d_instance_name)
        object_node->setAttribute("Name", d_instance_name);

    return true;
}

bool Qtilities::Core::InstanceFactoryInfo::importXml(QDomDocument* doc, QDomElement* object_node, Qtilities::ExportVersion version) {
    Q_UNUSED(version)
    Q_UNUSED(doc)

    // We don't do a version check here. Observer will do it for us.

    if (object_node->hasAttribute("FactoryTag"))
        d_factory_tag = object_node->attribute("FactoryTag");
    else
        d_factory_tag = QString(qti_def_FACTORY_QTILITIES);

    d_instance_tag = object_node->attribute("InstanceFactoryInfo");

    if (!object_node->hasAttribute("Name"))
        d_instance_name = d_instance_tag;
    else
        d_instance_name = object_node->attribute("Name");

    return true;
}
