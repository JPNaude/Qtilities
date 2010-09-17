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

#include "TreeItem.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesCoreConstants.h"

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Constants;

#include <QDomElement>

namespace Qtilities {
    namespace CoreGui {
        FactoryItem<QObject, TreeItem> TreeItem::factory;
    }
}

struct Qtilities::CoreGui::TreeItemData {
    TreeItemData() : is_modified(false) { }

    //! Stores if this tree item is modified.
    bool        is_modified;
};

Qtilities::CoreGui::TreeItem::TreeItem(const QString& name, QObject* parent) : QObject(parent), AbstractTreeItem() {
    itemData = new TreeItemData;
    setObjectName(name);
}

Qtilities::CoreGui::TreeItem::~TreeItem() {
    delete itemData;
}

Qtilities::Core::Interfaces::IFactoryData Qtilities::CoreGui::TreeItem::factoryData() const {
    IFactoryData factoryData(FACTORY_QTILITIES,FACTORY_TAG_TREE_ITEM,objectName());
    return factoryData;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::CoreGui::TreeItem::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeItem::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    Q_UNUSED(params)

    return IExportable::Incomplete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeItem::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(import_list)
    Q_UNUSED(params)


    return IExportable::Incomplete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeItem::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    // 1. Factory attributes is added to this item's node:
    factoryData().exportXML(doc,object_node);

    // 2. The data of this item is added to a new data node:
    QDomElement item_data = doc->createElement("Data");
    object_node->appendChild(item_data);

    // 2.1 Formatting
    if (isFormattingExportable())
        exportFormattingXML(doc,&item_data);

    return IExportable::Incomplete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeItem::importXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(object_node)
    Q_UNUSED(params)

    return IExportable::Incomplete;
}


bool Qtilities::CoreGui::TreeItem::isModified() const {
    return itemData->is_modified;
}

void Qtilities::CoreGui::TreeItem::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets) {
    itemData->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

