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

#include "Project.h"
#include "ProjectManagementConstants.h"
#include "ProjectManager.h"

#include <Logger.h>
#include <Qtilities.h>

#include <QFileInfo>

using namespace Qtilities::ProjectManagement::Constants;

struct Qtilities::ProjectManagement::ProjectData {
    ProjectData(): project_file(QString()),
    project_name(QString(QObject::tr("New Project"))) {}

    QList<IProjectItem*>    project_items;
    QString                 project_file;
    QString                 project_name;
};

Qtilities::ProjectManagement::Project::Project(QObject* parent) : QObject(parent), IProject() {
    d = new ProjectData;
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

    if (file_name.endsWith(".xml")) {
    return true;
    } else if (file_name.endsWith(".prj")) {
        QTemporaryFile file;
        file.open();
        QDataStream stream(&file);   // we will serialize the data into the file
        stream.setVersion(QDataStream::Qt_4_6);
        stream << (quint32) QTILITIES_BINARY_EXPORT_FORMAT;
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
                    LOG_INFO(tr("Failed to replace the current project file at path: ") + d->project_file);
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
                LOG_INFO(tr("Successfully saved complete project to file: ") + d->project_file);
            if (success == IExportable::Incomplete)
                LOG_INFO(tr("Successfully saved incomplete project to file: ") + d->project_file);
        } else {
            LOG_ERROR(tr("Failed to save current project to file: ") + d->project_file);
            return false;
        }
        return true;
    }

    return false;
}

bool Qtilities::ProjectManagement::Project::loadProject(const QString& file_name, bool close_current_first) {
    if (close_current_first)
        closeProject();

    LOG_DEBUG(tr("Starting to load project from file: ") + file_name);

    if (file_name.endsWith(".xml")) {
        return true;
    } else if (file_name.endsWith(".prj")) {
        d->project_file = file_name;
        d->project_name = QFileInfo(file_name).fileName();

        QFile file(file_name);
        if (!file.exists()) {
            LOG_ERROR(QString(tr("Project file does not exist. Project will not be loaded.")));
            return false;
        }
        file.open(QIODevice::ReadOnly);

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_4_6);

        quint32 marker;
        stream >> marker;
        LOG_INFO(QString(tr("Inspecting project file format: Found binary export file format version: %1")).arg(marker));
        if (marker != (quint32) QTILITIES_BINARY_EXPORT_FORMAT) {
            LOG_ERROR(QString(tr("Project file format does not match the expected binary export file format (expected version: %1). Project will not be loaded.")).arg(QTILITIES_BINARY_EXPORT_FORMAT));
            return false;
        }
        stream >> marker;
        LOG_INFO(QString(tr("Inspecting project file format: Application binary export file format version: %1")).arg(marker));
        if (marker != PROJECT_MANAGER->projectFileVersion()) {
            LOG_ERROR(QString(tr("Project file format does not match the expected application binary export file format (expected version: %1). Project will not be loaded.")).arg(PROJECT_MANAGER->projectFileVersion()));
            return false;
        }
        stream >> marker;
        if (marker != MARKER_PROJECT_SECTION) {
            LOG_ERROR(tr("Project file does not contain valid project data. Project will not be loaded. Path: ") + file_name);
            return false;
        }
        stream >>d->project_name;
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
            LOG_ERROR(tr("Failed to load project from file %1. The number of project items does not match your current set of plugin's number of projec items.") + file_name);
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
                d->project_items.at(i)->exportBinary(stream);
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
                LOG_INFO(tr("Successfully loaded complete project from file: ") + file_name);
            if (success == IExportable::Incomplete)
                LOG_INFO(tr("Successfully loaded incomplete project from file: ") + file_name);
        } else {
            LOG_ERROR(tr("Failed to load project from file: ") + file_name);
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
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
    if (notification_targets & IModificationNotifier::NotifySubjects) {
        for (int i = 0; i < d->project_items.count(); i++)
            d->project_items.at(i)->setModificationState(new_state,notification_targets);
    }
}
