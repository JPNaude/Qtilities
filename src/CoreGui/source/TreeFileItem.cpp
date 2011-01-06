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

Qtilities::CoreGui::TreeFileItem::TreeFileItem(const QString& file_name, const QString& relative_to_path, QObject* parent) : TreeItemBase(file_name, parent) {
    treeFileItemBase = new TreeFileItemData;
    setFile(file_name,relative_to_path);
    installEventFilter(this);
}

Qtilities::CoreGui::TreeFileItem::~TreeFileItem() {
    delete treeFileItemBase;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::CoreGui::TreeFileItem::instanceFactoryInfo() const {
    treeFileItemBase->instanceFactoryInfo.d_instance_name = objectName();
    return treeFileItemBase->instanceFactoryInfo;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::CoreGui::TreeFileItem::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::XML;
    return flags;
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
    file_data.setAttribute("Path",path());
    file_data.setAttribute("RelativeToPath",relativeToPath());

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
                        setFile(data.attribute("Path"));
                        result = IExportable::Complete;
                    }
                    if (data.hasAttribute("RelativeToPath")) {
                        setRelativeToPath(data.attribute("RelativeToPath"));
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

bool Qtilities::CoreGui::TreeFileItem::eventFilter(QObject *object, QEvent *event) {
    if (!treeFileItemBase->ignore_events) {
        if (object == this && event->type() == QEvent::DynamicPropertyChange) {
            QDynamicPropertyChangeEvent* propertyChangeEvent = static_cast<QDynamicPropertyChangeEvent *>(event);
            if (propertyChangeEvent) {
                QString property_name = QString(propertyChangeEvent->propertyName().data());
                if (property_name == QString(OBJECT_NAME)) {
                    QString new_name = Observer::getSharedProperty(this,OBJECT_NAME).value().toString();
                    if (treeFileItemBase->file_info.fileName() != new_name) {
                        treeFileItemBase->file_info.setFileName(new_name);
                        emit fileNameChanged(filePath());
                    }
                }
            }
        }
    }
    return false;
}


void Qtilities::CoreGui::TreeFileItem::setFile(const QString& file_name, const QString& relative_to_path,  bool broadcast) {
    bool modified = false;
    if (file_name != filePath() || (relative_to_path != relativeToPath() && !relative_to_path.isEmpty()))
        modified = true;

    treeFileItemBase->file_info.setFile(file_name);
    if (!relative_to_path.isEmpty())
        treeFileItemBase->file_info.setRelativeToPath(relative_to_path);

    // We need to check if an object name exists
    if (Observer::propertyExists(this,OBJECT_NAME)) {
        SharedObserverProperty new_subject_name_property(QVariant(treeFileItemBase->file_info.fileName()),OBJECT_NAME);
        new_subject_name_property.setIsExportable(false);

        Observer::setSharedProperty(this,new_subject_name_property);
    } else {
        setObjectName(treeFileItemBase->file_info.fileName());
    }

    if (broadcast)
        emit fileNameChanged(path());

    if (modified)
        setModificationState(true,IModificationNotifier::NotifyListeners);
}

bool Qtilities::CoreGui::TreeFileItem::isRelative() const {
    return treeFileItemBase->file_info.isRelative();
}

bool Qtilities::CoreGui::TreeFileItem::isAbsolute() const {
    return treeFileItemBase->file_info.isAbsolute();
}

bool Qtilities::CoreGui::TreeFileItem::hasRelativeToPath() const{
    return treeFileItemBase->file_info.hasRelativeToPath();
}

void Qtilities::CoreGui::TreeFileItem::setRelativeToPath(const QString& path) {
    treeFileItemBase->file_info.setRelativeToPath(path);
}

QString Qtilities::CoreGui::TreeFileItem::relativeToPath() const {
    return treeFileItemBase->file_info.relativeToPath();
}

QString Qtilities::CoreGui::TreeFileItem::path() const {
    return treeFileItemBase->file_info.path();
}

QString Qtilities::CoreGui::TreeFileItem::filePath() const {
    return treeFileItemBase->file_info.filePath();
}

QString Qtilities::CoreGui::TreeFileItem::absoluteToRelativePath() const {
    return treeFileItemBase->file_info.absoluteToRelativePath();
}

QString Qtilities::CoreGui::TreeFileItem::absoluteToRelativeFilePath() const {
    return treeFileItemBase->file_info.absoluteToRelativeFilePath();
}

QString Qtilities::CoreGui::TreeFileItem::fileName() const {
    return treeFileItemBase->file_info.fileName();
}

QString Qtilities::CoreGui::TreeFileItem::baseName() const {
    return treeFileItemBase->file_info.baseName();
}

QString Qtilities::CoreGui::TreeFileItem::completeBaseName() const {
    return treeFileItemBase->file_info.completeBaseName();
}

QString Qtilities::CoreGui::TreeFileItem::suffix() const {
    return treeFileItemBase->file_info.suffix();
}

QString Qtilities::CoreGui::TreeFileItem::completeSuffix() const {
    return treeFileItemBase->file_info.completeSuffix();
}

QString Qtilities::CoreGui::TreeFileItem::actualPath() const {
    return treeFileItemBase->file_info.actualPath();
}

QString Qtilities::CoreGui::TreeFileItem::actualFilePath() const {
    return treeFileItemBase->file_info.actualFilePath();
}

bool Qtilities::CoreGui::TreeFileItem::exists() const {
    return treeFileItemBase->file_info.exists();
}

