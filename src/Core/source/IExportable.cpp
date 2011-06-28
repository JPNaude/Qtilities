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

#include "IExportable.h"
#include "QtilitiesCoreApplication.h"

Qtilities::Core::Interfaces::IExportable::IExportable() {
    d_export_version = Qtilities::Qtilities_Latest;
    d_application_export_version_set = false;
    d_is_exportable = true;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::Interfaces::IExportable::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Interfaces::IExportable::exportBinary(QDataStream& stream ) const {
    Q_UNUSED(stream)

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Interfaces::IExportable::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(stream)
    Q_UNUSED(import_list)

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Interfaces::IExportable::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::Core::Interfaces::IExportable::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(import_list)

    return IExportable::Complete;
}

void Qtilities::Core::Interfaces::IExportable::setApplicationExportVersion(quint32 version) {
    d_application_export_version_set = true;
    d_export_application_version = version;
}

quint32 Qtilities::Core::Interfaces::IExportable::applicationExportVersion() const {
    if (!d_application_export_version_set)
        return QtilitiesCoreApplication::applicationExportVersion();
    else
        return d_export_application_version;
}

QString Qtilities::Core::Interfaces::IExportable::exportModeToString(ExportMode export_mode) {
    if (export_mode == None) {
        return "None";
    } else if (export_mode == Binary) {
        return "Binary";
    } else if (export_mode == XML) {
        return "XML";
    }

    return QString();
}

Qtilities::Core::Interfaces::IExportable::ExportMode Qtilities::Core::Interfaces::IExportable::stringToExportMode(const QString& export_mode_string) {
    if (export_mode_string == "None") {
        return None;
    } else if (export_mode_string == "Binary") {
        return Binary;
    } else if (export_mode_string == "XML") {
        return XML;
    }

    Q_ASSERT(0);
    return None;
}
