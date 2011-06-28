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

#include "Project.h"
#include "ProjectManagementConstants.h"
#include "ProjectManager.h"

#include <Qtilities.h>

#include <Logger>
#include <QtilitiesApplication>

#include <QFileInfo>
#include <QDomElement>
#include <QApplication>
#include <QCursor>

#include <stdio.h>
#include <time.h>

using namespace Qtilities::ProjectManagement::Constants;
using namespace Qtilities;

struct Qtilities::ProjectManagement::ProjectPrivateData {
    ProjectPrivateData(): project_file(QString()),
    project_name(QString(QObject::tr("New Project"))) {}

    QList<IProjectItem*>    project_items;
    QString                 project_file;
    QString                 project_name;
    QMutex                  modification_mutex;
};

Qtilities::ProjectManagement::Project::Project(QObject* parent) : QObject(parent), IProject() {
    d = new ProjectPrivateData;
    setObjectName("Project");
}

Qtilities::ProjectManagement::Project::~Project() {
    delete d;
}

bool Qtilities::ProjectManagement::Project::newProject() {
    d->project_file = QString();
    d->project_name = QString(QObject::tr("New Project"));
    for (int i = 0; i < d->project_items.count(); i++) {
        d->project_items.at(i)->newProjectItem();
    }

    setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
    return true;
}

quint32 MARKER_PROJECT_SECTION = 0xBABEFACE;

bool Qtilities::ProjectManagement::Project::saveProject(const QString& file_name) {
    LOG_DEBUG(tr("Starting to save current project to file: ") + file_name);

    if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::XML))) {
        QTemporaryFile file;
        file.open();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        // Create the QDomDocument:
        QDomDocument doc("QtilitiesXMLProject");
        QDomElement root = doc.createElement("QtilitiesXMLProject");
        doc.appendChild(root);

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        IExportable::Result success = exportXml(&doc,&root);
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_WARNING("Project XML export completed in " + QString::number(diff) + " seconds.");
        #endif

        // Put the complete doc in a string and save it to the file:
        QString docStr = doc.toString(2);
        file.write(docStr.toAscii());
        file.close();

        if (success != IExportable::Failed) {
            // Copy the tmp file to the actual project file.
            d->project_file = file_name;
            QFile current_file(d->project_file);
            if (current_file.exists())  {
                if (!current_file.remove()) {
                    LOG_ERROR(tr("Failed to replace the current project file at path: ") + d->project_file);
                    QApplication::restoreOverrideCursor();
                    return false;
                }
            }
            file.copy(d->project_file);

            // We change the project name to the selected file name:
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.fileName().split(".").front();
            d->project_name = file_name_only;

            setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            if (success == IExportable::Complete)
                LOG_INFO_P(tr("Successfully saved complete project to file: ") + d->project_file);
            if (success == IExportable::Incomplete)
                LOG_INFO_P(tr("Successfully saved incomplete project to file: ") + d->project_file);
        } else {
            LOG_ERROR_P(tr("Failed to save current project to file: ") + d->project_file);
            QApplication::restoreOverrideCursor();
            return false;
        }

        QApplication::restoreOverrideCursor();
        return true;
    } else if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::Binary))) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QTemporaryFile file;
        file.open();
        QDataStream stream(&file);
        if (exportVersion() == Qtilities::Qtilities_1_0)
            stream.setVersion(QDataStream::Qt_4_7);

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        IExportable::Result success = exportBinary(stream);
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_WARNING("Project binary export completed in " + QString::number(diff) + " seconds.");
        #endif

        file.close();

        if (success != IExportable::Failed) {
            // Copy the tmp file to the actual project file.
            d->project_file = file_name;
            QFile current_file(d->project_file);
            if (current_file.exists())  {
                if (!current_file.remove()) {
                    LOG_ERROR(tr("Failed to replace the current project file at path: ") + d->project_file);
                    QApplication::restoreOverrideCursor();
                    return false;
                }
            }
            file.copy(d->project_file);

            // We change the project name to the selected file name
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.fileName().split(".").front();
            d->project_name = file_name_only;

            setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            if (success == IExportable::Complete)
                LOG_INFO_P(tr("Successfully saved complete project to file: ") + d->project_file);
            if (success == IExportable::Incomplete)
                LOG_INFO_P(tr("Successfully saved incomplete project to file: ") + d->project_file);
        } else {
            LOG_ERROR_P(tr("Failed to save current project to file: ") + d->project_file);
            QApplication::restoreOverrideCursor();
            return false;
        }
        QApplication::restoreOverrideCursor();
        return true;
    } else {
        LOG_ERROR_P(tr("Failed to save project. Unsupported project file suffix found on file: ") + file_name);
    }

    return false;
}

bool Qtilities::ProjectManagement::Project::loadProject(const QString& file_name, bool close_current_first) {
    if (close_current_first)
        closeProject();

    LOG_INFO_P(tr("Loading project: ") + file_name);
    QFile file(file_name);
    if (!file.exists()) {
        LOG_ERROR_P(QString(tr("Project file does not exist at path \"") + file_name + tr("\". Project will not be loaded.")));
        return false;
    }
    d->project_file = file_name;
    d->project_name = QFileInfo(file_name).fileName();
    file.open(QIODevice::ReadOnly);

    if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::XML))) {
        // Load the file into doc:
        QDomDocument doc("QtilitiesXMLProject");
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QString docStr = file.readAll();
        QString error_string;
        int error_line;
        int error_column;
        if (!doc.setContent(docStr,&error_string,&error_line,&error_column)) {
            LOG_ERROR(QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string));
            file.close();
            QApplication::restoreOverrideCursor();
            return false;
        }
        file.close();
        QDomElement root = doc.documentElement();

        // Interpret the loaded doc:
        QList<QPointer<QObject> > import_list;

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        IExportable::Result success = importXml(&doc,&root,import_list);
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_WARNING("Project XML import completed in " + QString::number(diff) + " seconds.");
        #endif

        QApplication::restoreOverrideCursor();

        if (success != IExportable::Failed) {
            // We change the project name to the selected file name
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.fileName().split(".").front();
            d->project_name = file_name_only;

            // Process events here before we set the modification state. This would ensure that any
            // queued QtilitiesPropertyChangeEvents are processed. In some cases this can set the
            // modification state of observers and when these events are delivered later than the
            // setModificationState() call below, it might change the modification state again.
            QCoreApplication::processEvents();

            setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            if (success == IExportable::Complete)
                LOG_INFO_P(tr("Successfully loaded complete project from file: ") + file_name);
            if (success == IExportable::Incomplete)
                LOG_INFO_P(tr("Successfully loaded incomplete project from file: ") + file_name);
            return true;
        } else {
            LOG_ERROR_P(tr("Failed to load project from file: ") + file_name);
            return false;
        }
    } else if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::Binary))) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QDataStream stream(&file);
        if (exportVersion() == Qtilities::Qtilities_1_0)
            stream.setVersion(QDataStream::Qt_4_7);

        QList<QPointer<QObject> > import_list;

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        IExportable::Result success = importBinary(stream,import_list);
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_WARNING("Project binary import completed in " + QString::number(diff) + " seconds.");
        #endif

        QApplication::restoreOverrideCursor();

        file.close();

        if (success != IExportable::Failed) {
            // We change the project name to the selected file name
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.fileName().split(".").front();
            d->project_name = file_name_only;

            // Process events here before we set the modification state. This would ensure that any
            // queued QtilitiesPropertyChangeEvents are processed. In some cases this can set the
            // modification state of observers and when these events are delivered later than the
            // setModificationState() call below, it might change the modification state again.
            QCoreApplication::processEvents();

            setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            if (success == IExportable::Complete)
                LOG_INFO_P(tr("Successfully loaded complete project from file: ") + file_name);
            if (success == IExportable::Incomplete)
                LOG_INFO_P(tr("Successfully loaded incomplete project from file: ") + file_name);
            return true;
        } else {
            LOG_ERROR_P(tr("Failed to load project from file: ") + file_name);
            return false;
        }
    } else {
        LOG_ERROR_P(tr("Failed to load project. Unsupported project file suffix found on file: ") + file_name);
    }
    return false;
}

bool Qtilities::ProjectManagement::Project::closeProject() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    LOG_INFO_P(tr("Closing project: ") + d->project_file);
    for (int i = 0; i < d->project_items.count(); i++) {
        d->project_items.at(i)->closeProjectItem();
    }
    QApplication::restoreOverrideCursor();

    setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
    return true;
}

QString Qtilities::ProjectManagement::Project::projectFile() const {
    return d->project_file;
}

QString Qtilities::ProjectManagement::Project::projectName() const {
    return d->project_name;
}

void Qtilities::ProjectManagement::Project::setProjectItems(QList<IProjectItem*> project_items, bool inherit_modification_state) {
    // Disconnect all previous project items:
    for (int i = 0; i < d->project_items.count(); i++) {
        d->project_items.at(i)->objectBase()->disconnect(this);
    }

    d->project_items = project_items;
    for (int i = 0; i < d->project_items.count(); i++) {
        connect(d->project_items.at(i)->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
    }

    if (inherit_modification_state)
        emit modificationStateChanged(isModified());
    else
        setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
}

void Qtilities::ProjectManagement::Project::addProjectItem(IProjectItem* project_item, bool inherit_modification_state) {
    if (!d->project_items.contains(project_item)) {
        bool is_modified = isModified();
        d->project_items.append(project_item);
        connect(project_item->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
        // Check if project item is modified. If so we need to update the project.
        if (inherit_modification_state)
            setModificationState(project_item->isModified());
        else
            setModificationState(is_modified,IModificationNotifier::NotifySubjects);
    }
}

void Qtilities::ProjectManagement::Project::removeProjectItem(IProjectItem* project_item) {
    d->project_items.removeOne(project_item);
    project_item->objectBase()->disconnect(this);
}

QStringList Qtilities::ProjectManagement::Project::projectItemNames() const {
    QStringList item_names;
    for (int i = 0; i < d->project_items.count(); i++)
        item_names << d->project_items.at(i)->projectItemName();
    return item_names;
}

int Qtilities::ProjectManagement::Project::projectItemCount() const {
    return d->project_items.count();
}

Qtilities::ProjectManagement::IProjectItem* Qtilities::ProjectManagement::Project::projectItem(int index) {
    if (index < 0 || index >= d->project_items.count())
        return 0;

    return d->project_items.at(index);
}

bool Qtilities::ProjectManagement::Project::isModified() const {
    for (int i = 0; i < d->project_items.count(); i++) {
        if (d->project_items.at(i)->isModified())
            return true;
    }
    return false;
}

void Qtilities::ProjectManagement::Project::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets) {
    if (!d->modification_mutex.tryLock())
        return;
    if (notification_targets & IModificationNotifier::NotifySubjects) {
        for (int i = 0; i < d->project_items.count(); i++)
            d->project_items.at(i)->setModificationState(new_state,notification_targets);
    }
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
    d->modification_mutex.unlock();
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::ProjectManagement::Project::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;

    return flags;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::Project::exportBinary(QDataStream& stream) const {
    // ---------------------------------------------------
    // Save file format information:
    // ---------------------------------------------------
    stream << MARKER_PROJECT_SECTION;
    stream << (quint32) exportVersion();
    stream << CoreGui::QtilitiesApplication::qtilitiesVersionString();
    stream << (quint32) applicationExportVersion();
    stream << QApplication::applicationVersion();
    stream << MARKER_PROJECT_SECTION;

    stream << (quint32) d->project_items.count();
    QStringList item_names;
    for (int i = 0; i < d->project_items.count(); i++) {
        item_names << d->project_items.at(i)->projectItemName();
    }
    stream << item_names;

    // ---------------------------------------------------
    // Do the actual export:
    // ---------------------------------------------------
    LOG_DEBUG(QString(tr("This project contains %1 project item(s).")).arg(d->project_items.count()));
    IExportable::Result success = IExportable::Complete;
    for (int i = 0; i < d->project_items.count(); i++) {
        if (d->project_items.at(i)->supportedFormats() & IExportable::Binary) {
            LOG_DEBUG(QString(tr("Saving item %1: %2.")).arg(i).arg(d->project_items.at(i)->projectItemName()));
            IExportable::Result item_result = d->project_items.at(i)->exportBinary(stream);
            if (item_result == IExportable::Failed) {
                success = item_result;
                break;
            }
            if (item_result == IExportable::Incomplete && success == IExportable::Complete)
                success = item_result;
        } else {
            success = IExportable::Incomplete;
            LOG_WARNING(QString(tr("Could not save project item %1: %2. This project item does not support binary exporting.")).arg(i).arg(d->project_items.at(i)->projectItemName()));
        }
    }

    stream << MARKER_PROJECT_SECTION;
    return success;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::Project::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    // ---------------------------------------------------
    // Inspect file format:
    // ---------------------------------------------------
    quint32 marker;
    stream >> marker;
    if (marker != MARKER_PROJECT_SECTION) {
        LOG_ERROR(QString(tr("Failed to load project from. Missing project marker at beginning of file.")));
        return IExportable::Failed;
    }

    stream >> marker;
    Qtilities::ExportVersion read_version = (Qtilities::ExportVersion) marker;
    LOG_INFO(QString(tr("Inspecting project file format: Qtilities export format version: %1")).arg(marker));
    QString qtilities_version;
    stream >> qtilities_version;
    LOG_INFO(QString(tr("Inspecting project file format: Qtilities version used to save the file: %1")).arg(qtilities_version));

    stream >> marker;
    quint32 application_read_version = marker;
    LOG_INFO(QString(tr("Inspecting project file format: Application export format version: %1")).arg(marker));
    QString application_version;
    stream >> application_version;
    LOG_INFO(QString(tr("Inspecting project file format: Application version used to save the file: %1")).arg(application_version));

    stream >> marker;
    if (marker != MARKER_PROJECT_SECTION) {
        LOG_ERROR(QString(tr("Failed to load project from. Missing project marker at beginning of file.")));
        return IExportable::Failed;
    }

    // ---------------------------------------------------
    // Check if input format is supported:
    // ---------------------------------------------------
    bool is_supported_format = false;
    if (read_version == Qtilities::Qtilities_1_0)
        is_supported_format = true;

    if (!is_supported_format) {
        LOG_ERROR(QString(tr("Unsupported project file found with export version: %1. The project file will not be parsed.")).arg(read_version));
        return IExportable::Failed;
    }

    // ---------------------------------------------------
    // Do the actual import:
    // ---------------------------------------------------
    quint32 project_item_count;
    stream >> project_item_count;

    QStringList item_names;
    QStringList item_names_readback;

    for (int i = 0; i < d->project_items.count(); i++) {
        item_names << d->project_items.at(i)->projectItemName();
    }
    stream >> item_names_readback;
    LOG_DEBUG(QString(tr("This project contains %1 project item(s).")).arg(item_names_readback.count()));
    if (item_names != item_names_readback) {
        LOG_ERROR(QString(tr("Failed to load project. The number of project items does not match your current set of plugin's number of project items, or they are not loaded in the same order.")));
        return IExportable::Failed;
    }

    // Now stream each project part.
    int int_count = project_item_count;
    IExportable::Result success = IExportable::Complete;
    for (int i = 0; i < int_count; i++) {
        if (d->project_items.at(i)->supportedFormats() & IExportable::Binary) {
            LOG_DEBUG(QString(tr("Loading item %1: %2.")).arg(i).arg(d->project_items.at(i)->projectItemName()));
            d->project_items.at(i)->setExportVersion(read_version);
            d->project_items.at(i)->setApplicationExportVersion(application_read_version);
            IExportable::Result item_result = d->project_items.at(i)->importBinary(stream, import_list);
            if (item_result == IExportable::Failed) {
                success = item_result;
                break;
            }
            if (item_result == IExportable::Incomplete && success == IExportable::Complete)
                success = item_result;
        } else {
            success = IExportable::Incomplete;
            LOG_WARNING(QString(tr("Could not load project item %1: %2. This project item does not support binary importing.")).arg(i).arg(d->project_items.at(i)->projectItemName()));
        }
    }

    if (success) {
        stream >> marker;
        if (marker != MARKER_PROJECT_SECTION)
            success = IExportable::Failed;
    }

    return success;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::Project::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    // ---------------------------------------------------
    // Save file format information:
    // ---------------------------------------------------
    object_node->setAttribute("ExportVersion",QString::number(exportVersion()));
    object_node->setAttribute("QtilitiesVersion",CoreGui::QtilitiesApplication::qtilitiesVersionString());
    object_node->setAttribute("ApplicationExportVersion",QString::number(applicationExportVersion()));
    object_node->setAttribute("ApplicationVersion",QApplication::applicationVersion());
    object_node->setAttribute("ApplicationName",QApplication::applicationName());

    // ---------------------------------------------------
    // Do the actual export:
    // ---------------------------------------------------
    IExportable::Result success = IExportable::Complete;
    for (int i = 0; i < d->project_items.count(); i++) {
        QString name = d->project_items.at(i)->projectItemName();
        QDomElement itemRoot = doc->createElement("ProjectItem_" + QString::number(i));
        itemRoot.setAttribute("Name",name);
        object_node->appendChild(itemRoot);
        IExportable::Result item_result = d->project_items.at(i)->exportXml(doc,&itemRoot);
        if (item_result == IExportable::Failed) {
            success = item_result;
            break;
        }
        if (item_result == IExportable::Incomplete && success == IExportable::Complete)
            success = item_result;
    }
    return success;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::Project::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    // ---------------------------------------------------
    // Inspect file format:
    // ---------------------------------------------------
    Qtilities::ExportVersion read_version;
    if (object_node->hasAttribute("ExportVersion")) {
        read_version = (Qtilities::ExportVersion) object_node->attribute("ExportVersion").toInt();
        LOG_INFO(QString(tr("Inspecting project file format: Qtilities export format version: %1")).arg(read_version));
    } else {
        LOG_ERROR(QString(tr("The export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The project file will not be parsed.")));
        QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    if (object_node->hasAttribute("QtilitiesVersion"))
        LOG_INFO(QString(tr("Inspecting project file format: Qtilities version used to save the file: %1")).arg(object_node->attribute("QtilitiesVersion")));
    quint32 application_read_version = 0;
    if (object_node->hasAttribute("ApplicationExportVersion")) {
        application_read_version = object_node->attribute("ApplicationExportVersion").toInt();
        LOG_INFO(QString(tr("Inspecting project file format: Application export format version: %1")).arg(application_read_version));
    } else {
        LOG_ERROR(QString(tr("The application export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The project file will not be parsed.")));
        QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    if (object_node->hasAttribute("ApplicationVersion"))
        LOG_INFO(QString(tr("Inspecting project file format: Application version used to save the file: %1")).arg(object_node->attribute("ApplicationVersion")));

    // ---------------------------------------------------
    // Check if input format is supported:
    // ---------------------------------------------------
    bool is_supported_format = false;
    if (read_version == Qtilities::Qtilities_1_0)
        is_supported_format = true;

    if (!is_supported_format) {
        LOG_ERROR(QString(tr("Unsupported project file found with export version: %1. The project file will not be parsed.")).arg(read_version));
        return IExportable::Failed;
    }

    bool found_project_item = false;

    // ---------------------------------------------------
    // Do the actual import:
    // ---------------------------------------------------
    IExportable::Result success = IExportable::Complete;
    QDomNodeList itemNodes = object_node->childNodes();
    for(int i = 0; i < itemNodes.count(); i++) {
        QDomNode itemNode = itemNodes.item(i);
        QDomElement item = itemNode.toElement();

        if (item.isNull())
            continue;

        if (item.tagName().startsWith("ProjectItem_")) {
            found_project_item = true;
            QString item_name;
            if (item.hasAttribute("Name")) {
                item_name = item.attribute("Name");
                LOG_TRACE("Found project item in import file with name: " + item_name);
            } else {
                LOG_WARNING(tr("Nameless project item found in input file. This item will be skipped."));
                continue;
            }

            // Now get the project item with name item_name:
            IProjectItem* item_iface = 0;
            for (int i = 0; i < d->project_items.count(); i++) {
                if (d->project_items.at(i)->projectItemName() == item_name) {
                    item_iface = d->project_items.at(i);
                    break;
                }
            }

            if (!item_iface) {
                LOG_WARNING(QString(tr("Input file contains a project item \"%1\" which does not exist in your application. Import will be incomplete.")).arg(item_name));
                if (success != IExportable::Failed) {
                    success = IExportable::Incomplete;
                }
                continue;
            } else {
                item_iface->setExportVersion(read_version);
                item_iface->setApplicationExportVersion(application_read_version);
                success = item_iface->importXml(doc,&item,import_list);
                if (success != IExportable::Complete && success != IExportable::Incomplete) {
                    LOG_ERROR(tr("Project item \"") + item_name + tr("\" failed during import."));
                    break;
                }
            }
            continue;
        }
    }

    if (!found_project_item)
        LOG_WARNING(tr("No project items found in project file."));

    return success;
}
