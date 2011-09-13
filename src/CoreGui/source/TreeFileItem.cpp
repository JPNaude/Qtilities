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

Qtilities::CoreGui::TreeFileItem::TreeFileItem(const QString& file_path, const QString& relative_to_path, PathDisplay path_display, QObject* parent) : TreeItemBase(file_path, parent) {
    treeFileItemBase = new TreeFileItemPrivateData;
    d_path_display = path_display;
    setFile(file_path,relative_to_path);
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

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    // 1.1 Formatting:
    IExportable::Result result = saveFormattingToXML(doc,object_node,exportVersion());

    // 1.2 File Information:
    QDomElement file_data = doc->createElement("FileInfo");
    object_node->appendChild(file_data);
    file_data.setAttribute("Path",filePath());
    file_data.setAttribute("RelativeToPath",relativeToPath());
    file_data.setAttribute("PathDisplay",(int) pathDisplay());
    return result;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::CoreGui::TreeFileItem::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::Result result = IExportable::Incomplete;

    QDomNodeList dataNodes = object_node->childNodes();
    for(int i = 0; i < dataNodes.count(); i++) {
        QDomNode dataNode = dataNodes.item(i);
        QDomElement data = dataNode.toElement();

        if (data.isNull())
            continue;

        if (data.tagName() == "FileInfo") {
            // Restore the file path/name:
            if (data.hasAttribute("Path")) {
                setFile(data.attribute("Path"));
                result = IExportable::Complete;
            }
            if (data.hasAttribute("RelativeToPath")) {
                setRelativeToPath(data.attribute("RelativeToPath"));
                result = IExportable::Complete;
            }
            if (data.hasAttribute("PathDisplay")) {
                setPathDisplay((PathDisplay) data.attribute("PathDisplay").toInt());
                result = IExportable::Complete;
            }
        }
    }

    IExportable::Result formatting_result = loadFormattingFromXML(doc,object_node,exportVersion());
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
                if (property_name == QString(qti_prop_NAME) && !d_queued_file_path.isEmpty()) {
                    // Finish off what was started in setFile():
                    treeFileItemBase->file_info.setFile(d_queued_file_path);
                    if (!d_queued_relative_to_path.isEmpty())
                        treeFileItemBase->file_info.setRelativeToPath(d_queued_relative_to_path);

                     setModificationState(true,IModificationNotifier::NotifyListeners);

                    // Check if the displayed name matches the new name:
                    // We need to create the property and add it to the object.
                    // The displayed name is the same in all displayed contexts.
                    QList<Observer*> parents = Observer::parentReferences(this);
                    MultiContextProperty new_instance_names_property(qti_prop_DISPLAYED_ALIAS_MAP);
                    for (int i = 0; i < parents.count(); i++)
                        new_instance_names_property.addContext(QVariant(displayName()),parents.at(i)->observerID());
                    ObjectManager::setMultiContextProperty(this,new_instance_names_property);
                }
            }
        }
    }
    return false;
}

void Qtilities::CoreGui::TreeFileItem::setFile(const QString& file_path, const QString& relative_to_path, bool broadcast) {
    bool modified = false;
    if (file_path != filePath() || (relative_to_path != relativeToPath() && !relative_to_path.isEmpty()))
        modified = true;

    if (!modified)
        return;

    QtilitiesFileInfo fi(file_path,relative_to_path);

    // We need to check if an object name exists first:
    if (ObjectManager::propertyExists(this,qti_prop_NAME)) {
        // The rest of the things that need to happen is done in eventFilter(), only when the name property was set correctly.
        // We just set paths to be used in the event filter here:
        d_queued_file_path = file_path;
        d_queued_relative_to_path = relative_to_path;

        SharedProperty new_subject_name_property(qti_prop_NAME,QVariant(fi.actualFilePath()));
        ObjectManager::setSharedProperty(this,new_subject_name_property);
    } else {
        // Handle cases where there is no naming policy filter:
        setObjectName(fi.actualFilePath());

        // In this case we do not need to check in eventFilter() since the object name was correctly updated:
        treeFileItemBase->file_info.setFile(file_path);
        if (!relative_to_path.isEmpty())
            treeFileItemBase->file_info.setRelativeToPath(relative_to_path);
    }

    setModificationState(true,IModificationNotifier::NotifyListeners);

    if (broadcast)
        emit filePathChanged(fi.actualFilePath());
}

QString Qtilities::CoreGui::TreeFileItem::displayName(const QString& file_path) {
    if (file_path.isEmpty()) {
        if (d_path_display == DisplayFileName) {
            return treeFileItemBase->file_info.fileName();
        } else if (d_path_display == DisplayFilePath) {
            return treeFileItemBase->file_info.filePath();
        } else if (d_path_display == DisplayActualFilePath) {
            return treeFileItemBase->file_info.actualFilePath();
        }
    } else {
        QtilitiesFileInfo fi(file_path);
        if (d_path_display == DisplayFileName) {
            return fi.fileName();
        } else if (d_path_display == DisplayFilePath) {
            return fi.filePath();
        } else if (d_path_display == DisplayActualFilePath) {
            return fi.actualFilePath();
        }
    }

    return QString(tr("Invalid display name type. See Qtilities::CoreGui::TreeFileItem::displayName()"));
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
    bool modified = false;
    if (relativeToPath() != path)
        modified = true;

    treeFileItemBase->file_info.setRelativeToPath(path);

    if (modified)
        setModificationState(true,IModificationNotifier::NotifyListeners);
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

void Qtilities::CoreGui::TreeFileItem::setFileName(const QString& new_file_name) {
    bool modified = false;
    if (fileName() != new_file_name)
        modified = true;

    treeFileItemBase->file_info.setFileName(new_file_name);

    if (modified)
        setModificationState(true,IModificationNotifier::NotifyListeners);
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
    QFileInfo file_info(treeFileItemBase->file_info.actualFilePath());
    return file_info.exists();
}

Qtilities::Core::QtilitiesFileInfo Qtilities::CoreGui::TreeFileItem::fileInfo() const {
    return treeFileItemBase->file_info;
}
