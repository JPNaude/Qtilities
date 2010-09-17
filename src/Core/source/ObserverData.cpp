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

#include "ObserverData.h"
#include "ObserverHints.h"

#include <QDomElement>

using namespace Qtilities::Core::Interfaces;

Qtilities::Core::Interfaces::IFactoryData Qtilities::Core::ObserverData::factoryData() const {
    return IFactoryData();
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::ObserverData::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverData::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    Q_UNUSED(params)

    stream << (qint32) subject_limit;
    stream << (qint32) subject_id_counter;
    stream << observer_description;
    stream << (qint32) access_mode;
    stream << (qint32) access_mode_scope;

    // Stream categories
    stream << categories.count();
    for (int i = 0; i < categories.count(); i++) {
        categories.at(i).exportBinary(stream);
    }

    stream << deliver_qtilities_property_changed_events;

    if (display_hints) {
        // Indicates that this observer has hints.
        if (display_hints->isExportable()) {
            stream << (bool) true;
            return display_hints->exportBinary(stream);
        } else {
            stream << (bool) false;
        }
    } else {
        stream << (bool) false;
    }

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverData::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    quint32 ui32;
    stream >> ui32;
    subject_limit = ui32;
    stream >> ui32;
    subject_id_counter = ui32;
    stream >> observer_description;
    stream >> ui32;
    access_mode = ui32;
    stream >> ui32;
    access_mode_scope = ui32;

    // Stream categories
    stream >> ui32;
    int category_count = ui32;
    for (int i = 0; i < category_count; i++) {
        QtilitiesCategory category(stream);
        categories.push_back(category);
    }

    stream >> deliver_qtilities_property_changed_events;

    bool has_hints;
    stream >> has_hints;
    if (has_hints) {
        if (!display_hints)
            display_hints = new ObserverHints();
        return display_hints->importBinary(stream,import_list,params);
    } else
        return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverData::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    // Add parameters as attributes:
    if (subject_limit != -1)
        object_node->setAttribute("SubjectLimit",subject_limit);
    if (!observer_description.isEmpty())
        object_node->setAttribute("Description",observer_description);
    if (access_mode != Observer::FullAccess)
        object_node->setAttribute("AccessMode",Observer::accessModeToString((Observer::AccessMode) access_mode));
    if (access_mode != Observer::GlobalScope)
        object_node->setAttribute("AccessModeScope",Observer::accessModeScopeToString((Observer::AccessModeScope) access_mode_scope));

    // Categories:
    if (categories.count() > 0) {
        QDomElement categories_node = doc->createElement("Categories");
        object_node->appendChild(categories_node);
        for (int i = 0; i < categories.count(); i++) {
            QDomElement category = doc->createElement("Category");
            categories_node.appendChild(category);
            categories.at(i).exportXML(doc,&category);
        }
    }

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::ObserverData::importXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) {
    Q_UNUSED(params)

    if (object_node->hasAttribute("SubjectLimit"))
        subject_limit = object_node->attribute("SubjectLimit").toInt();
    if (object_node->hasAttribute("Description"))
        observer_description = object_node->attribute("Description");
    if (object_node->hasAttribute("AccessMode"))
        access_mode = Observer::stringToAccessMode(object_node->attribute("AccessMode"));
    if (object_node->hasAttribute("AccessModeScope"))
        access_mode_scope = Observer::stringToAccessModeScope(object_node->attribute("AccessModeScope"));

    return IExportable::Complete;
}

