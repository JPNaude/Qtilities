/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#include "TreeFileItem.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesCoreConstants.h"

#include <QDomElement>
#include <QApplication>

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
    setFileForce(file_path,relative_to_path);
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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeFileItem::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    // 1.1 Formatting:
    IExportable::ExportResultFlags result = saveFormattingToXML(doc,object_node,exportVersion());

    // 1.2 File Information:
    QDomElement file_data = doc->createElement("FileInfo");
    object_node->appendChild(file_data);
    file_data.setAttribute("Path",filePath());
    file_data.setAttribute("RelativeToPath",relativeToPath());
    file_data.setAttribute("PathDisplay",(int) pathDisplay());
    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::CoreGui::TreeFileItem::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    IExportable::ExportResultFlags result = IExportable::Incomplete;

    QDomNodeList dataNodes = object_node->childNodes();
    for(int i = 0; i < dataNodes.count(); ++i) {
        QDomNode dataNode = dataNodes.item(i);
        QDomElement data = dataNode.toElement();

        if (data.isNull())
            continue;

        if (data.tagName() == QLatin1String("FileInfo")) {
            // Restore the file path/name:
            if (data.hasAttribute("Path")) {
                setFileForce(data.attribute("Path"));
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

    IExportable::ExportResultFlags formatting_result = loadFormattingFromXML(doc,object_node,exportVersion());
    if (formatting_result != IExportable::Complete)
        result = formatting_result;

    return result;
}

void Qtilities::CoreGui::TreeFileItem::setFactoryData(InstanceFactoryInfo instanceFactoryInfo) {
    treeFileItemBase->instanceFactoryInfo = instanceFactoryInfo;
}

bool Qtilities::CoreGui::TreeFileItem::eventFilter(QObject *object, QEvent *event) {
    if (!treeFileItemBase->ignore_events) {
        if (object == this && event->type() == QEvent::User) {
            QtilitiesPropertyChangeEvent* qtilities_event = static_cast<QtilitiesPropertyChangeEvent *> (event);
            if (qtilities_event) {
                //if (d_queued_file_path.isEmpty())
                //    qDebug() << "Caught QtilitiesPropertyChangeEvent on design file. Queued file is empty, not updating internal QFileInfo";
                if (!qstrcmp(qtilities_event->propertyName().data(),qti_prop_NAME) && !d_queued_file_path.isEmpty()) {
                    //qDebug() << "Caught QtilitiesPropertyChangeEvent on design file. Updating internal QFileInfo. Queued file: " << d_queued_file_path;

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
                    for (int i = 0; i < parents.count(); ++i)
                        new_instance_names_property.addContext(QVariant(displayName()),parents.at(i)->observerID());
                    ObjectManager::setMultiContextProperty(this,new_instance_names_property);
                }
            }
        }
    //} else {
        //qDebug() << "Ignoring DynamicPropertyChange event on object:" << this;
    }
    return false;
}

void Qtilities::CoreGui::TreeFileItem::setFile(const QString& file_path, const QString& relative_to_path, bool broadcast) {
    QtilitiesFileInfo fi(file_path,relative_to_path);

    // We need to check if an object name exists first:
    if (ObjectManager::propertyExists(this,qti_prop_NAME)) {
        // The rest of the things that need to happen is done in eventFilter(), only when the name property was set correctly.
        // We just set paths to be used in the event filter here:
        d_queued_file_path = file_path.trimmed();
        d_queued_relative_to_path = relative_to_path.trimmed();

        //qDebug() << "Not updating internal QFileInfo in TreeFileItem::setFile(). This will happen in event filter and indicated using a debug message. Queued file: " << d_queued_file_path;
        SharedProperty new_subject_name_property(qti_prop_NAME,QVariant(fi.actualFilePath()));
        ObjectManager::setSharedProperty(this,new_subject_name_property);
        setObjectName(fi.actualFilePath());

        QApplication::processEvents();
    } else {
        // Handle cases where there is no naming policy filter:
        setObjectName(fi.actualFilePath());

        // In this case we do not need to check in eventFilter() since the object name was correctly updated:
        treeFileItemBase->file_info.setFile(file_path.trimmed());
        treeFileItemBase->file_info.setRelativeToPath(relative_to_path.trimmed());
    }

    setModificationState(true,IModificationNotifier::NotifyListeners);

    if (broadcast)
        emit filePathChanged(fi.actualFilePath());
}

void CoreGui::TreeFileItem::setFileForce(const QString &file_path, const QString &relative_to_path, bool broadcast) {
    if (ObjectManager::propertyExists(this,qti_prop_NAME)) {
        treeFileItemBase->file_info.setFile(file_path.trimmed());
        treeFileItemBase->file_info.setRelativeToPath(relative_to_path.trimmed());
        setFile(file_path,relative_to_path,broadcast);
        setObjectName(treeFileItemBase->file_info.actualFilePath());
    } else
        setFile(file_path,relative_to_path,broadcast);
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

void Qtilities::CoreGui::TreeFileItem::setFilePath(const QString& new_file_path) {
    if (!FileUtils::comparePaths(filePath(),new_file_path,Qt::CaseSensitive)) {
        QString tmp_string = new_file_path;
        if (tmp_string.startsWith("\\"))
            tmp_string = tmp_string.remove(0,1);
        treeFileItemBase->file_info.setFile(tmp_string);
        setModificationState(true,IModificationNotifier::NotifyListeners);
    }
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
    if (fileName() != new_file_name) {
        treeFileItemBase->file_info.setFileName(new_file_name);
        setModificationState(true,IModificationNotifier::NotifyListeners);
    }
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
