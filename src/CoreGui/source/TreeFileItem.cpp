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
    installEventFilter(this);
}

Qtilities::CoreGui::TreeFileItem::~TreeFileItem() {
    delete treeFileItemBase;
}

void Qtilities::CoreGui::TreeFileItem::setFileName(const QString& file_name, bool broadcast) {
    if (file_name == fileName())
        return;

    // We need to check if an object name exists
    if (Observer::propertyExists(this,OBJECT_NAME)) {
        SharedObserverProperty new_subject_name_property(QVariant(strippedName(file_name)),OBJECT_NAME);
        new_subject_name_property.setIsExportable(false);
        Observer::setSharedProperty(this,new_subject_name_property);
        treeFileItemBase->file_path = strippedPath(file_name);
    } else {
        setObjectName(strippedName(file_name));
        treeFileItemBase->file_path = strippedPath(file_name);
    }

    emit fileNameChanged(fileName());

    if (broadcast)
        setModificationState(true,IModificationNotifier::NotifyListeners);
}

QString Qtilities::CoreGui::TreeFileItem::fileName() const {
    // The objectName() will be sync'ed with the OBJECT_NAME property by the name manager.
    if (!treeFileItemBase->file_path.isEmpty() && !objectName().isEmpty()) {
        return treeFileItemBase->file_path + "/" + objectName();
    } else
        return QString();
}

bool Qtilities::CoreGui::TreeFileItem::exists() const {
    QFileInfo file_info(fileName());
    return file_info.exists();
}

QString Qtilities::CoreGui::TreeFileItem::fileExtension() const {
    return strippedFileExtension(objectName());
}

Qtilities::Core::InstanceFactoryInfo Qtilities::CoreGui::TreeFileItem::instanceFactoryInfo() const {
    treeFileItemBase->instanceFactoryInfo.d_instance_name = objectName();
    return treeFileItemBase->instanceFactoryInfo;
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
    InstanceFactoryInfo factory_data = instanceFactoryInfo();
    factory_data.exportBinary(stream);

    // Export the file name:
    stream << fileName();

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(import_list)
    Q_UNUSED(params)

    // Import the file name:
    QString file_name;
    stream >> file_name;
    setFileName(file_name);

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    Q_UNUSED(params)

    // 1. Factory attributes is added to this item's node:
    instanceFactoryInfo().exportXML(doc,object_node);

    // 2. The data of this item is added to a new data node:
    QDomElement item_data = doc->createElement("Data");
    object_node->appendChild(item_data);

    // 2.1 Formatting:
    IExportable::Result result = saveFormattingToXML(doc,&item_data);

    // 2.2 File Information:
    QDomElement file_data = doc->createElement("File");
    item_data.appendChild(file_data);
    file_data.setAttribute("Path",fileName());

    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    Q_UNUSED(doc)
    Q_UNUSED(params)
    Q_UNUSED(import_list)

    IExportable::Result result = IExportable::Incomplete;

    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName() == "Data") {
            QDomNodeList dataNodes = child.childNodes();
            for(int i = 0; i < dataNodes.count(); i++) {
                QDomNode dataNode = dataNodes.item(i);
                QDomElement data = dataNode.toElement();

                if (data.isNull())
                    continue;

                if (data.tagName() == "File") {
                    // Restore the file path/name:
                    if (data.hasAttribute("Path")) {
                        setFileName(data.attribute("Path"));
                        result = IExportable::Complete;
                    }
                }
            }
        }
    }

    IExportable::Result formatting_result = loadFormattingFromXML(doc,object_node);
    if (formatting_result != IExportable::Complete)
        result = formatting_result;

    return result;
}

void Qtilities::CoreGui::TreeFileItem::setFactoryData(InstanceFactoryInfo instanceFactoryInfo) {
    treeFileItemBase->instanceFactoryInfo = instanceFactoryInfo;
}
