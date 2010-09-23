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

#include "TreeFileItem.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesCoreConstants.h"

#include <QDomElement>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Constants;

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QObject, TreeFileItem> TreeFileItem::factory;
    }
}

Qtilities::CoreGui::TreeFileItem::TreeFileItem(const QString& file_name, QObject* parent) : TreeItemBase(TreeFileItem::strippedName(file_name), parent) {
    treeFileItemBase = new TreeFileItemData;
    setFileName(file_name);
}

Qtilities::CoreGui::TreeFileItem::~TreeFileItem() {
    delete treeFileItemBase;
}

void Qtilities::CoreGui::TreeFileItem::setFileName(const QString& file_name, bool broadcast) {
    if (file_name == treeFileItemBase->file_name)
        return;

    treeFileItemBase->file_name = file_name;
    setObjectName(strippedName(file_name));

    if (broadcast)
        setModificationState(true,IModificationNotifier::NotifyListeners);

    emit fileNameChanged(file_name);
}

QString Qtilities::CoreGui::TreeFileItem::fileName() const {
    return treeFileItemBase->file_name;
}

bool Qtilities::CoreGui::TreeFileItem::exists() const {
    QFileInfo file_info(treeFileItemBase->file_name);
    return file_info.exists();
}

QString Qtilities::CoreGui::TreeFileItem::fileExtension() const {
    return strippedFileExtension(treeFileItemBase->file_name);
}

Qtilities::Core::IFactoryTag Qtilities::CoreGui::TreeFileItem::factoryData() const {
    treeFileItemBase->factoryData.d_instance_name = objectName();
    return treeFileItemBase->factoryData;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::CoreGui::TreeFileItem::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    Q_UNUSED(params)

    // First export the factory data of this item:
    IFactoryTag factory_data = factoryData();
    factory_data.exportBinary(stream);

    // Export the file name:
    stream << treeFileItemBase->file_name;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(import_list)
    Q_UNUSED(params)

    // Import the file name:
    stream >> treeFileItemBase->file_name;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    // 1. Factory attributes is added to this item's node:
    factoryData().exportXML(doc,object_node);

    // 2. The data of this item is added to a new data node:
    QDomElement item_data = doc->createElement("Data");
    object_node->appendChild(item_data);

    // 2.1 Formatting:
    IExportable::Result result = saveFormattingToXML(doc,&item_data);

    // 2.2 File Information:
    QDomElement file_data = doc->createElement("File");
    item_data.appendChild(file_data);
    file_data.setAttribute("FileName",treeFileItemBase->file_name);

    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(params)
    Q_UNUSED(import_list)

    return loadFormattingFromXML(doc,object_node);
}

void Qtilities::CoreGui::TreeFileItem::setFactoryData(IFactoryTag factoryData) {
    treeFileItemBase->factoryData = factoryData;
}
