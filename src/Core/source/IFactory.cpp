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

#include "IFactory.h"

#include <QtXml>

Qtilities::Core::Interfaces::IFactoryData::IFactoryData(QDomDocument* doc, QDomElement* object_node) {
    importXML(doc,object_node);
}

bool Qtilities::Core::Interfaces::IFactoryData::exportXML(QDomDocument* doc, QDomElement* object_node) const {
    // We store factory data as QDomCDATASection children:
    QDomCDATASection factoryTag = doc->createCDATASection(d_factory_tag);
    object_node->appendChild(factoryTag);
    QDomCDATASection instanceTag = doc->createCDATASection(d_instance_tag);
    object_node->appendChild(instanceTag);
    QDomCDATASection instanceName = doc->createCDATASection(d_instance_name);
    object_node->appendChild(instanceName);

    return true;
}

bool Qtilities::Core::Interfaces::IFactoryData::importXML(QDomDocument* doc, QDomElement* object_node) {
    Q_UNUSED(doc)

    QDomNodeList nodeList = object_node->childNodes();
    for(int i=0; i < nodeList.count(); i++)
    {
        QDomNode node = nodeList.item(i);
        QDomCDATASection cdata = node.toCDATASection();
        if(cdata.isNull())
            continue;

        if (i == 0)
            d_factory_tag = cdata.data();
        if (i == 1)
            d_instance_tag = cdata.data();
        if (i == 2)
            d_instance_name = cdata.data();
    }

    return true;
}
