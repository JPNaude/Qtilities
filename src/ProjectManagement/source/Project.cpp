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

#include "Project.h"
#include "ProjectManagementConstants.h"
#include "ProjectManager.h"

#include <Qtilities.h>

#include <Logger>
#include <QtilitiesApplication>
#include <FileUtils>

#include <QFileInfo>
#include <QDomElement>
#include <QApplication>
#include <QCursor>
#include <QMessageBox>

#include <FileLocker>

#include <stdio.h>
#include <time.h>

using namespace Qtilities::ProjectManagement::Constants;
using namespace Qtilities;
using namespace Qtilities::Core;

struct Qtilities::ProjectManagement::ProjectPrivateData {
    ProjectPrivateData(): project_file(QString()),
    project_name(QObject::tr("New Project")) {}

    QList<IProjectItem*>    project_items;
    QString                 project_file;
    QString                 project_name;
    QMutex                  modification_mutex;

    FileLocker              file_locker;
};

Qtilities::ProjectManagement::Project::Project(QObject* parent) : QObject(parent), IProject() {
    d = new ProjectPrivateData;
    setObjectName("Project");
}

Qtilities::ProjectManagement::Project::~Project() {
    closeProject();
    delete d;
}

bool Qtilities::ProjectManagement::Project::newProject() {
    d->project_file = QString();
    d->project_name = QObject::tr("New Project");
    for (int i = 0; i < d->project_items.count(); ++i) {
        d->project_items.at(i)->newProjectItem();
    }

    setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
    return true;
}

quint32 MARKER_PROJECT_SECTION = 0xBABEFACE;

bool Qtilities::ProjectManagement::Project::saveProject(const QString& file_name, ITask* task) {
    if (!PROJECT_MANAGER->projectSavingEnabled()) {
        if (PROJECT_MANAGER->executionStyle() == ProjectManager::ExecSilent) {
            LOG_ERROR(tr("Saving of projects is currently disabled, project save can't continue: ") + PROJECT_MANAGER->projectSavingInfoMessage());
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Cannot Save Project"));
            msgBox.setText(tr("Saving of projects is currently disabled, project save can't continue."));
            msgBox.setInformativeText(PROJECT_MANAGER->projectSavingInfoMessage());
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }

        return false;
    }

    LOG_TASK_INFO(tr("Starting to save current project to file: ") + file_name,task);

    if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::XML))) {
        QTemporaryFile file;
        file.open();

        // Create the QDomDocument:
        QDomDocument doc("QtilitiesXMLProject");
        QDomElement root = doc.createElement("QtilitiesXMLProject");
        doc.appendChild(root);

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        IExportable::setExportTask(task);
        IExportable::ExportResultFlags success = exportXml(&doc,&root);
        IExportable::clearExportTask();
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_TASK_INFO("Project XML export completed in " + QString::number(diff) + " seconds.",task);
        #endif

        // Put the complete doc in a string and save it to the file:
        QString docStr = doc.toString(2);
        docStr.prepend("<!--Created by " + QApplication::applicationName() + " v" + QApplication::applicationVersion() + " on " + QDateTime::currentDateTime().toString() + "-->\n");
        docStr.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        file.write(docStr.toUtf8());
        file.close();

        if (success != IExportable::Failed) {
            // Copy the tmp file to the actual project file.
            QString old_project_file = d->project_file;
            d->project_file = file_name;
            QFile current_file(d->project_file);
            if (current_file.exists())  {
                if (!current_file.remove()) {
                    LOG_TASK_ERROR(tr("Failed to replace the current project file at path: ") + d->project_file,task);
                    return false;
                }
            }
            file.copy(d->project_file);

            // Only if successfull, check if the new file is different to the old file and handle locks accordingly:
            if (PROJECT_MANAGER->useProjectFileLocks()) {
                // Unlock the old file:
                if (d->file_locker.isFileLocked(old_project_file)) {
                    QString errorMsg;
                    if (!d->file_locker.unlockFile(old_project_file,&errorMsg))
                        LOG_TASK_WARNING(errorMsg,task);
                }
            }

            // We change the project name to the selected file name:
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.baseName();
            d->project_name = file_name_only;

            // Add a lock on the project file.
            if (PROJECT_MANAGER->useProjectFileLocks()) {
                if (!d->file_locker.isFileLocked(file_name)) {
                    QString errorMsg;
                    if (!d->file_locker.lockFile(file_name,&errorMsg))
                        LOG_TASK_WARNING(errorMsg,task);
                }
            }

            setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            if (success == IExportable::Complete)
                LOG_TASK_INFO_P(tr("Successfully saved complete project to file: ") + d->project_file,task);
            if (success == IExportable::Incomplete)
                LOG_TASK_INFO_P(tr("Successfully saved incomplete project to file: ") + d->project_file,task);
        } else {
            LOG_TASK_ERROR_P(tr("Failed to save current project to file: ") + d->project_file,task);
            return false;
        }

        return true;
    } else if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::Binary))) {
        QTemporaryFile file;
        file.open();
        QDataStream stream(&file);
        if (exportVersion() == Qtilities::Qtilities_1_0 || exportVersion() == Qtilities::Qtilities_1_1 || exportVersion() == Qtilities::Qtilities_1_2)
            stream.setVersion(QDataStream::Qt_4_7);

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        IExportable::setExportTask(task);
        IExportable::ExportResultFlags success = exportBinary(stream);
        IExportable::clearExportTask();
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_TASK_INFO("Project binary export completed in " + QString::number(diff) + " seconds.",task);
        #endif

        file.close();

        if (success != IExportable::Failed) {
            // Copy the tmp file to the actual project file.
            QString old_project_file = d->project_file;
            d->project_file = file_name;
            QFile current_file(d->project_file);
            if (current_file.exists())  {
                if (!current_file.remove()) {
                    LOG_TASK_ERROR_P(tr("Failed to replace the current project file at path: ") + d->project_file,task);
                    return false;
                }
            }
            file.copy(d->project_file);

            // Only if successfull, check if the new file is different to the old file and handle locks accordingly:
            if (PROJECT_MANAGER->useProjectFileLocks()) {
                // Unlock the old file:
                if (d->file_locker.isFileLocked(old_project_file)) {
                    QString errorMsg;
                    if (!d->file_locker.unlockFile(old_project_file,&errorMsg))
                        LOG_TASK_WARNING(errorMsg,task);
                }
            }

            // We change the project name to the selected file name
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.baseName();
            d->project_name = file_name_only;

            // Add a lock on the project file.
            if (PROJECT_MANAGER->useProjectFileLocks()) {
                if (!d->file_locker.isFileLocked(file_name)) {
                    QString errorMsg;
                    if (!d->file_locker.lockFile(file_name,&errorMsg))
                        LOG_TASK_WARNING(errorMsg,task);
                }
            }

            setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            if (success == IExportable::Complete)
                LOG_TASK_INFO_P(tr("Successfully saved complete project to file: ") + d->project_file,task);
            if (success == IExportable::Incomplete)
                LOG_TASK_INFO_P(tr("Successfully saved incomplete project to file: ") + d->project_file,task);
        } else {
            LOG_TASK_ERROR_P(tr("Failed to save current project to file: ") + d->project_file,task);
            return false;
        }
        return true;
    } else {
        LOG_TASK_ERROR_P(tr("Failed to save project. Unsupported project file suffix found on file: ") + file_name,task);
    }

    return false;
}

bool Qtilities::ProjectManagement::Project::loadProject(const QString& file_name, bool close_current_first, ITask* task) {
    if (close_current_first)
        closeProject();

    LOG_TASK_INFO_P(tr("Opening project: ") + file_name,task);
    QFile file(file_name);
    if (!file.exists()) {
        LOG_TASK_ERROR_P(QString(tr("Project file does not exist at path \"") + file_name + tr("\". Project will not be loaded.")),task);
        return false;
    }
    d->project_file = file_name;
    d->project_name = QFileInfo(file_name).fileName();
    file.open(QIODevice::ReadOnly);

    if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::XML))) {
        // Load the file into doc:
        QDomDocument doc("QtilitiesXMLProject");
        QString docStr = file.readAll();
        file.close();
        QString error_string;
        int error_line;
        int error_column;
        if (!doc.setContent(docStr,&error_string,&error_line,&error_column)) {
            LOG_TASK_ERROR_P(QString(tr("The tree input file could not be parsed by QDomDocument. Error on line %1 column %2: %3")).arg(error_line).arg(error_column).arg(error_string),task);
            return false;
        }
        QDomElement root = doc.documentElement();

        // Interpret the loaded doc:
        QList<QPointer<QObject> > import_list;

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        setExportTask(task);
        IExportable::ExportResultFlags success = importXml(&doc,&root,import_list);
        clearExportTask();
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_TASK_INFO("Project XML import completed in " + QString::number(diff) + " seconds.",task);
        #endif

        if (success & IExportable::SuccessResult || success == IExportable::Complete) {
            // We change the project name to the selected file name
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.baseName();
            d->project_name = file_name_only;

            // Add a lock on the project file.
            if (PROJECT_MANAGER->useProjectFileLocks()) {
                QString errorMsg;
                if (!d->file_locker.lockFile(file_name,&errorMsg))
                    LOG_TASK_WARNING(errorMsg,task);
                else
                    LOG_TASK_INFO(tr("Successfully locked project file."),task);
            } else
                LOG_TASK_INFO(tr("Project file locking is disabled, will not attempt to lock project file."),task);

            // Process events here before we set the modification state. This would ensure that any
            // queued QtilitiesPropertyChangeEvents are processed. In some cases this can set the
            // modification state of observers and when these events are delivered later than the
            // setModificationState() call below, it might change the modification state again.
            QCoreApplication::processEvents();

            if (!PROJECT_MANAGER->projectChangedDuringLoad())
                setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            else
                setModificationState(true,IModificationNotifier::NotifyListeners);

            if (success == IExportable::Complete)
                LOG_TASK_INFO_P(tr("Successfully loaded complete project from file: ") + file_name,task);
            if (success == IExportable::Incomplete) {
                QString backup_file_name = FileUtils::appendToFileName(file_name,"_complete");
                QFile backup_file(backup_file_name);
                if (backup_file.exists()) {
                    if (!backup_file.remove())
                        LOG_TASK_WARNING(tr("Failed to remove old project backup file at: ") + backup_file_name,task);
                }

                if (!file.copy(backup_file_name)) {
                    LOG_TASK_WARNING_P(tr("Successfully loaded incomplete project from file: ") + file_name + tr(". However, the project manager failed to make a backup of the complete project file at: ") + backup_file_name,task);
                } else {
                    LOG_TASK_INFO_P(tr("Successfully loaded incomplete project from file: ") + file_name + tr(". A backup of the complete project file was created at: ") + backup_file_name,task);
                }
            }
            return true;
        } else {
            LOG_TASK_ERROR_P(tr("Failed to load project from file: ") + file_name,task);
            return false;
        }
    } else if (file_name.endsWith(PROJECT_MANAGER->projectTypeSuffix(IExportable::Binary))) {
        QDataStream stream(&file);
        if (exportVersion() == Qtilities::Qtilities_1_0 || exportVersion() == Qtilities::Qtilities_1_1 || exportVersion() == Qtilities::Qtilities_1_2)
            stream.setVersion(QDataStream::Qt_4_7);

        QList<QPointer<QObject> > import_list;

        #ifdef QTILITIES_BENCHMARKING
        time_t start,end;
        time(&start);
        #endif
        setExportTask(task);
        IExportable::ExportResultFlags success = importBinary(stream,import_list);
        clearExportTask();
        #ifdef QTILITIES_BENCHMARKING
        time(&end);
        double diff = difftime(end,start);
        LOG_WARNING("Project binary import completed in " + QString::number(diff) + " seconds.");
        #endif

        file.close();

        if (success != IExportable::Failed) {
            // We change the project name to the selected file name
            QFileInfo fi(d->project_file);
            QString file_name_only = fi.baseName();
            d->project_name = file_name_only;

            // Add a lock on the project file.
            if (PROJECT_MANAGER->useProjectFileLocks()) {
                QString errorMsg;
                if (!d->file_locker.lockFile(file_name,&errorMsg))
                    LOG_TASK_WARNING(errorMsg,task);
                else
                    LOG_TASK_INFO(tr("Successfully locked project file."),task);
            } else
                LOG_TASK_INFO(tr("Project file locking is disabled, will not attempt to lock project file."),task);

            // Process events here before we set the modification state. This would ensure that any
            // queued QtilitiesPropertyChangeEvents are processed. In some cases this can set the
            // modification state of observers and when these events are delivered later than the
            // setModificationState() call below, it might change the modification state again.
            QCoreApplication::processEvents();

            if (!PROJECT_MANAGER->projectChangedDuringLoad())
                setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
            else
                setModificationState(true,IModificationNotifier::NotifyListeners);

            if (success == IExportable::Complete)
                LOG_TASK_INFO_P(tr("Successfully loaded complete project from file: ") + file_name,task);
            if (success == IExportable::Incomplete) {
                QString backup_file_name = FileUtils::appendToFileName(file_name,".complete");
                QFile backup_file(backup_file_name);
                if (backup_file.exists()) {
                    if (!backup_file.remove())
                        LOG_TASK_WARNING(tr("Failed to remove old project backup file at: ") + backup_file_name,task);
                }

                if (!file.copy(backup_file_name)) {
                    LOG_TASK_WARNING_P(tr("Successfully loaded incomplete project from file: ") + file_name + tr(". However, the project manager failed to make a backup of the complete project file at: ") + backup_file_name,task);
                } else {
                    LOG_TASK_INFO_P(tr("Successfully loaded incomplete project from file: ") + file_name + tr(". A backup of the complete project file was created at: ") + backup_file_name,task);
                }
            }
            return true;
        } else {
            LOG_TASK_ERROR_P(tr("Failed to load project from file: ") + file_name,task);
            return false;
        }
    } else {
        LOG_TASK_ERROR_P(tr("Failed to load project. Unsupported project file suffix found on file: ") + file_name,task);
    }
    return false;
}

bool Qtilities::ProjectManagement::Project::closeProject(ITask *task) {
    LOG_TASK_INFO_P(tr("Closing project: ") + d->project_file,task);
    for (int i = 0; i < d->project_items.count(); ++i) {
        d->project_items.at(i)->closeProjectItem(task);
    }

    // Release existing lock on the project file.
    if (PROJECT_MANAGER->useProjectFileLocks() && !d->project_file.isEmpty()) {
        QString errorMsg;
        if (!d->file_locker.unlockFile(d->project_file,&errorMsg))
            LOG_TASK_WARNING(errorMsg,task);
    }

    setModificationState(false,IModificationNotifier::NotifyListeners | IModificationNotifier::NotifySubjects);
    return true;
}

QString Qtilities::ProjectManagement::Project::projectFile() const {
    return FileUtils::toNativeSeparators(QDir::cleanPath(d->project_file));
}

QString Qtilities::ProjectManagement::Project::projectName() const {
    return d->project_name;
}

void Qtilities::ProjectManagement::Project::setProjectItems(QList<IProjectItem*> project_items, bool inherit_modification_state) {
    // Disconnect all previous project items:
    for (int i = 0; i < d->project_items.count(); ++i) {
        d->project_items.at(i)->objectBase()->disconnect(this);
    }

    d->project_items = project_items;
    for (int i = 0; i < d->project_items.count(); ++i) {
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
    for (int i = 0; i < d->project_items.count(); ++i)
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
    for (int i = 0; i < d->project_items.count(); ++i) {
        if (d->project_items.at(i)->isModified())
            return true;
    }
    return false;
}

void Qtilities::ProjectManagement::Project::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    if (!d->modification_mutex.tryLock())
        return;

    if (notification_targets & IModificationNotifier::NotifySubjects) {
        for (int i = 0; i < d->project_items.count(); ++i)
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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::Project::exportBinary(QDataStream& stream) const {
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
    for (int i = 0; i < d->project_items.count(); ++i) {
        item_names << d->project_items.at(i)->projectItemName();
    }
    stream << item_names;

    // ---------------------------------------------------
    // Do the actual export:
    // ---------------------------------------------------
    LOG_DEBUG(QString(tr("This project contains %1 project item(s).")).arg(d->project_items.count()));
    IExportable::ExportResultFlags success = IExportable::Complete;
    for (int i = 0; i < d->project_items.count(); ++i) {
        if (d->project_items.at(i)->supportedFormats() & IExportable::Binary) {
            LOG_DEBUG(QString(tr("Saving item %1: %2.")).arg(i).arg(d->project_items.at(i)->projectItemName()));
            d->project_items.at(i)->setExportTask(exportTask());
            IExportable::ExportResultFlags item_result = d->project_items.at(i)->exportBinary(stream);
            d->project_items.at(i)->clearExportTask();

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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::Project::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {

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
    if (!(read_version < Qtilities::Qtilities_1_0 || read_version > Qtilities::Qtilities_Latest))
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

    for (int i = 0; i < d->project_items.count(); ++i) {
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
    IExportable::ExportResultFlags success = IExportable::Complete;
    for (int i = 0; i < int_count; ++i) {
        if (d->project_items.at(i)->supportedFormats() & IExportable::Binary) {
            LOG_DEBUG(QString(tr("Loading item %1: %2.")).arg(i).arg(d->project_items.at(i)->projectItemName()));
            d->project_items.at(i)->setExportVersion(read_version);
            d->project_items.at(i)->setApplicationExportVersion(application_read_version);

            d->project_items.at(i)->setExportTask(exportTask());
            IExportable::ExportResultFlags item_result = d->project_items.at(i)->importBinary(stream, import_list);
            d->project_items.at(i)->clearExportTask();

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

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::Project::exportXml(QDomDocument* doc, QDomElement* object_node) const {
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
    IExportable::ExportResultFlags success = IExportable::Complete;
    for (int i = 0; i < d->project_items.count(); ++i) {
        QString name = d->project_items.at(i)->projectItemName();
        QDomElement itemRoot = doc->createElement("ProjectItem_" + QString::number(i));
        itemRoot.setAttribute("Name",name);
        object_node->appendChild(itemRoot);
        d->project_items.at(i)->setExportTask(exportTask());
        IExportable::ExportResultFlags item_result = d->project_items.at(i)->exportXml(doc,&itemRoot);
        d->project_items.at(i)->clearExportTask();
        if (item_result == IExportable::Failed) {
            success = item_result;
            break;
        }
        if (item_result == IExportable::Incomplete && success == IExportable::Complete)
            success = item_result;
    }

    // TODO - Export dynamic properties here
    return success;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::Project::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    // ---------------------------------------------------
    // Inspect file format:
    // ---------------------------------------------------
    Qtilities::ExportVersion read_version;
    if (object_node->hasAttribute("ExportVersion")) {
        read_version = (Qtilities::ExportVersion) object_node->attribute("ExportVersion").toInt();
        LOG_TASK_INFO(QString(tr("Inspecting project file format: Qtilities export format version: %1")).arg(read_version),exportTask());
    } else {
        LOG_TASK_ERROR(QString(tr("The export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The project file will not be parsed.")),exportTask());
        QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    if (object_node->hasAttribute("QtilitiesVersion"))
        LOG_TASK_INFO(QString(tr("Inspecting project file format: Qtilities version used to save the file: %1")).arg(object_node->attribute("QtilitiesVersion")),exportTask());
    quint32 application_read_version = 0;
    if (object_node->hasAttribute("ApplicationExportVersion")) {
        application_read_version = object_node->attribute("ApplicationExportVersion").toInt();
        LOG_TASK_INFO(QString(tr("Inspecting project file format: Application export format version: %1")).arg(application_read_version),exportTask());
    } else {
        LOG_TASK_ERROR(QString(tr("The application export version of the input file could not be determined. This might indicate that the input file is in the wrong format. The project file will not be parsed.")),exportTask());
        QApplication::restoreOverrideCursor();
        return IExportable::Failed;
    }
    if (object_node->hasAttribute("ApplicationVersion"))
        LOG_TASK_INFO(QString(tr("Inspecting project file format: Application version used to save the file: %1")).arg(object_node->attribute("ApplicationVersion")),exportTask());

    // ---------------------------------------------------
    // Check if input format is supported:
    // ---------------------------------------------------
    bool is_supported_format = false;
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(read_version,exportTask());
    if (version_check_result == IExportable::VersionSupported)
        is_supported_format = true;

    if (!is_supported_format) {
        LOG_TASK_ERROR(QString(tr("Unsupported project file found with export version: %1. The project file will not be parsed.")).arg(read_version),exportTask());
        return IExportable::Failed;
    }

    bool found_project_item = false;

    // ---------------------------------------------------
    // Do the actual import:
    // ---------------------------------------------------
    IExportable::ExportResultFlags success = IExportable::Complete;
    QDomNodeList itemNodes = object_node->childNodes();
    for(int i = 0; i < itemNodes.count(); ++i) {
        QDomNode itemNode = itemNodes.item(i);
        QDomElement item = itemNode.toElement();

        if (item.isNull())
            continue;

        if (item.tagName().startsWith("ProjectItem_")) {
            found_project_item = true;
            QString item_name;
            if (item.hasAttribute("Name")) {
                item_name = item.attribute("Name");
                LOG_TASK_TRACE("Found project item in import file with name: " + item_name,exportTask());
            } else {
                LOG_TASK_WARNING(tr("Nameless project item found in input file. This item will be skipped."),exportTask());
                continue;
            }

            // Now get the project item with name item_name:
            IProjectItem* item_iface = 0;
            for (int i = 0; i < d->project_items.count(); ++i) {
                if (d->project_items.at(i)->projectItemName() == item_name) {
                    item_iface = d->project_items.at(i);
                    break;
                }
            }

            if (!item_iface) {
                LOG_TASK_WARNING(QString(tr("Input file contains a project item \"%1\" which does not exist in your application. Import will be incomplete.")).arg(item_name),exportTask());
                if (success != IExportable::Failed)
                    success = IExportable::Incomplete;
                continue;
            } else {
                item_iface->setExportVersion(read_version);
                item_iface->setApplicationExportVersion(application_read_version);
                item_iface->setExportTask(exportTask());
                success = item_iface->importXml(doc,&item,import_list);
                item_iface->clearExportTask();

                if (success & IExportable::FailedResult) {
                    LOG_TASK_ERROR(tr("Project item \"") + item_name + tr("\" failed during import."),exportTask());
                    success = IExportable::Incomplete;
                    break;
                }
            }
            continue;
        }
    }

    if (!found_project_item)
        LOG_TASK_WARNING(tr("No project items found in project file."),exportTask());

    return success;
}
