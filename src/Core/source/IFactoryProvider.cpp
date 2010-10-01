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

#include "IFactoryProvider.h"
#include "QtilitiesCoreConstants.h"

#include <QtXml>

using namespace Qtilities::Core::Constants;

Qtilities::Core::InstanceFactoryInfo::InstanceFactoryInfo(QDomDocument* doc, QDomElement* object_node) {
    importXML(doc,object_node);
}

bool Qtilities::Core::InstanceFactoryInfo::exportXML(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)

    if (d_factory_tag != QString(FACTORY_QTILITIES))
        object_node->setAttribute("FactoryTag", d_factory_tag);
    object_node->setAttribute("InstanceFactoryInfo", d_instance_tag);
    if (d_instance_tag != d_instance_name)
        object_node->setAttribute("Name", d_instance_name);

    return true;
}

bool Qtilities::Core::InstanceFactoryInfo::importXML(QDomDocument* doc, QDomElement* object_node) {
    Q_UNUSED(doc)

    if (object_node->hasAttribute("FactoryTag"))
        d_factory_tag = object_node->attribute("FactoryTag");
    else
        d_factory_tag = QString(FACTORY_QTILITIES);

    d_instance_tag = object_node->attribute("InstanceFactoryInfo");

    if (!object_node->hasAttribute("Name"))
        d_instance_name = d_instance_tag;
    else
        d_instance_name = object_node->attribute("Name");

    return true;
}
