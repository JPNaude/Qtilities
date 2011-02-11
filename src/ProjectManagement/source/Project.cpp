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

#include <Logger.h>
#include <Qtilities.h>

#include <QFileInfo>
#include <QDomElement>
#include <QApplication>
#include <QCursor>

using namespace Qtilities::ProjectManagement::Constants;

struct Qtilities::ProjectManagement::ProjectPrivateData {
    ProjectPrivateData(): project_file(QString()),
    project_name(QString(QObject::tr("New Project"))) {}

    QList<IProjectItem*>    project_items;
    QString                 project_file;
    QString                 project_name;
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

    if (file_name.endsWith(qti_def_SUFFIX_PROJECT_XML)) {
        QTemporaryFile file;
        file.open();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        // Create the QDomDocument:
        QDomDocument doc("QtilitiesXMLProject");
        QDomElement root = doc.createElement("QtilitiesXMLProject");
        root.setAttribute("DocumentVersion",qti_def_FORMAT_TREES_XML);
        root.setAttribute("ApplicationName",QApplication::applicationName());
        root.setAttribute("ApplicationVersion",QApplication::applicationVersion());
        doc.appendChild(root);

        // Group all the project items together:
        QDomElement itemsRoot = doc.createElement("ProjectItems");
        root.appendChild(itemsRoot);

        IExportable::Result success = IExportable::Complete;

        // Create children for each project part:
        for (int i = 0; i < d->project_items.count(); i++) {
            QString name = d->project_items.at(i)->projectItemName();
            QDomElement itemRoot = doc.createElement("ProjectItem");
            itemRoot.setAttribute("Name",name);
            itemsRoot.appendChild(itemRoot);
            IExportable::Result item_result = d->project_items.at(i)->exportXML(&doc,&itemRoot);
            if (item_result == IExportable::Failed) {
                success = item_result;
                break;
            }
            if (item_result == IExportable::Incomplete && success == IExportable::Complete)
                success = item_result;
        }

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
    } else if (file_name.endsWith(qti_def_SUFFIX_PROJECT_BINARY)) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QTemporaryFile file;
        file.open();
        QDataStream stream(&file);   // we will serialize the data into the file
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (quint32) qti_def_FORMAT_TREES_BINARY;
        stream << PROJECT_MANAGER->projectFileVersion();
        stream << MARKER_PROJECT_SECTION;
        stream << d->project_name;
        stream << (quint32) d->project_items.count();

        QStringList item_names;
        for (int i = 0; i < d->project_items.count(); i++) {
            item_names << d->project_items.at(i)->projectItemName();
        }
        stream << item_names;

        // Now stream each project part.
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
    }
    QApplication::restoreOverrideCursor();
    return false;
}

bool Qtilities::ProjectManagement::Project::loadProject(const QString& file_name, bool close_current_first) {
    if (close_current_first)
        closeProject();

    LOG_DEBUG(tr("Starting to load project from file: ") + file_name);
    QFile file(file_name);
    if (!file.exists()) {
        LOG_ERROR_P(QString(tr("Project file does not exist. Project will not be loaded.")));
        return false;
    }
    d->project_file = file_name;
    d->project_name = QFileInfo(file_name).fileName();
    file.open(QIODevice::ReadOnly);

    if (file_name.endsWith(qti_def_SUFFIX_PROJECT_XML)) {
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
            return IExportable::Failed;
        }
        file.close();

        // Interpret the loaded doc:
        QDomElement root = doc.documentElement();

        // Check the document version:
        if (root.hasAttribute("DocumentVersion")) {
            QString document_version = root.attribute("DocumentVersion");
            if (document_version.toInt() > qti_def_FORMAT_TREES_XML) {
                LOG_ERROR(QString(tr("The DocumentVersion of the input file is not supported by this version of your application. The document version of the input file is %1, while supported versions are versions up to %2. The document will not be parsed.")).arg(document_version.toInt()).arg(qti_def_FORMAT_TREES_XML));
                QApplication::restoreOverrideCursor();
                return IExportable::Failed;
            }
        } else {
            LOG_ERROR(QString(tr("The DocumentVersion of the input file could not be determined. This might indicate that the input file is in the wrong format. The document will not be parsed.")));
            QApplication::restoreOverrideCursor();
            return IExportable::Failed;
        }

        // Now check out all the children below the root node:
        IExportable::Result success = IExportable::Complete;
        QList<QPointer<QObject> > internal_import_list;
        QDomNodeList childNodes = root.childNodes();
        for(int i = 0; i < childNodes.count(); i++) {
            QDomNode childNode = childNodes.item(i);
            QDomElement child = childNode.toElement();

            if (child.isNull())
                continue;

            if (child.tagName() == "ProjectItems") {
                QDomNodeList itemNodes = child.childNodes();
                for(int i = 0; i < itemNodes.count(); i++) {
                    QDomNode itemNode = itemNodes.item(i);
                    QDomElement item = itemNode.toElement();

                    if (item.isNull())
                        continue;

                    if (item.tagName() == "ProjectItem") {
                        QString item_name;
                        if (item.hasAttribute("Name"))
                            item_name = item.attribute("Name");
                        else {
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
                            IExportable::Result item_result = item_iface->importXML(&doc,&item,internal_import_list);
                            if (item_result == IExportable::Failed) {
                                success = item_result;
                                break;
                            }
                            if (item_result == IExportable::Incomplete && success == IExportable::Complete)
                                success = item_result;
                        }
                        continue;
                    }
                }
                continue;
            }
        }

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
        } else {
            LOG_ERROR_P(tr("Failed to load project from file: ") + file_name);
            return false;
        }

        QApplication::restoreOverrideCursor();

        return true;
    } else if (file_name.endsWith(qti_def_SUFFIX_PROJECT_BINARY)) {
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_4_6);

        quint32 marker;
        stream >> marker;
        LOG_INFO(QString(tr("Inspecting project file format: Found binary export file format version: %1")).arg(marker));
        if (marker != (quint32) qti_def_FORMAT_TREES_BINARY) {
            LOG_ERROR(QString(tr("Project file format does not match the expected binary export file format (expected version: %1). Project will not be loaded.")).arg(qti_def_FORMAT_TREES_BINARY));
            file.close();
            return false;
        }
        stream >> marker;
        LOG_INFO(QString(tr("Inspecting project file format: Application binary export file format version: %1")).arg(marker));
        if (marker != PROJECT_MANAGER->projectFileVersion()) {
            LOG_ERROR(QString(tr("Project file format does not match the expected application binary export file format (expected version: %1). Project will not be loaded.")).arg(PROJECT_MANAGER->projectFileVersion()));
            file.close();
            return false;
        }
        stream >> marker;
        if (marker != MARKER_PROJECT_SECTION) {
            LOG_ERROR(tr("Project file does not contain valid project data. Project will not be loaded. Path: ") + file_name);
            file.close();
            return false;
        }
        stream >> d->project_name;
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
            LOG_ERROR(QString(tr("Failed to load project from file %1. The number of project items does not match your current set of plugin's number of project items, or they are not loaded in the same order.")).arg(file_name));
            file.close();
            return false;
        }

        // Now stream each project part.
        int int_count = project_item_count;
        IExportable::Result success = IExportable::Complete;
        QList<QPointer<QObject> > import_list;
        for (int i = 0; i < int_count; i++) {
            if (d->project_items.at(i)->supportedFormats() & IExportable::Binary) {
                LOG_DEBUG(QString(tr("Loading item %1: %2.")).arg(i).arg(d->project_items.at(i)->projectItemName()));
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
        } else {
            LOG_ERROR_P(tr("Failed to load project from file: ") + file_name);
            return false;
        }

        return true;
    }
    return false;
}

bool Qtilities::ProjectManagement::Project::closeProject() {
    for (int i = 0; i < d->project_items.count(); i++) {
        d->project_items.at(i)->closeProjectItem();
    }

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
    if (notification_targets & IModificationNotifier::NotifySubjects) {
        for (int i = 0; i < d->project_items.count(); i++)
            d->project_items.at(i)->setModificationState(new_state,IModificationNotifier::NotifySubjects);
    }
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}
