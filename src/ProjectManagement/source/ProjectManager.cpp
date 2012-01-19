/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#include "ProjectManager.h"
#include "Project.h"
#include "ProjectManagementConfig.h"

#include <QtilitiesCore>

#include <QtilitiesApplication>

#include <QMutex>
#include <QCoreApplication>
#include <QMap>
#include <QPointer>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

using namespace QtilitiesCore;
using namespace Qtilities::CoreGui;
using namespace Qtilities::ProjectManagement::Constants;

struct Qtilities::ProjectManagement::ProjectManagerPrivateData  {
    ProjectManagerPrivateData() : current_project(0),
        current_project_busy_count(0),
        recent_projects_size(5),
        is_initialized(false),
        project_types(IExportable::Binary | IExportable::XML),
        default_project_type(IExportable::XML),
        project_changed_during_load(false) {}

    QPointer<Project>                       current_project;
    int                                     current_project_busy_count;
    QList<IProjectItem*>                    item_list;
    QMap<QString, QVariant>                 recent_project_names;
    QStringList                             recent_project_stack;
    int                                     recent_projects_size;
    QPointer<ProjectManagementConfig>       config_widget;
    bool                                    open_last_project;
    bool                                    auto_create_new_project;
    bool                                    use_custom_projects_path;
    QString                                 custom_projects_path;
    bool                                    check_modified_projects;
    ProjectManager::ModifiedProjectsHandlingPolicy  modified_projects_handling_policy;
    bool                                    is_initialized;
    IExportable::ExportModeFlags            project_types;
    IExportable::ExportMode                 default_project_type;
    QMap<IExportable::ExportMode,QString>   suffices;
    bool                                    project_changed_during_load;
};

Qtilities::ProjectManagement::ProjectManager* Qtilities::ProjectManagement::ProjectManager::m_Instance = 0;

Qtilities::ProjectManagement::ProjectManager* Qtilities::ProjectManagement::ProjectManager::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new ProjectManager;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::ProjectManagement::ProjectManager::ProjectManager(QObject* parent) : QObject(parent)
{    
    d = new ProjectManagerPrivateData;
    readSettings();

    d->suffices[IExportable::Binary] = qti_def_SUFFIX_PROJECT_BINARY;
    d->suffices[IExportable::XML] = qti_def_SUFFIX_PROJECT_XML;

    // Register the tasks contained in this object:
    // Create TaskSaveProject:
    Task* taskSaveProject = new Task(taskNameToString(TaskSaveProject),true,this);
    registerTask(taskSaveProject,taskNameToString(TaskSaveProject));

    // Create TaskOpenProject:
    Task* taskOpenProject = new Task(taskNameToString(TaskOpenProject),true,this);
    registerTask(taskOpenProject,taskNameToString(TaskOpenProject));

    // Create TaskCloseProject:
    Task* taskCloseProject = new Task(taskNameToString(TaskCloseProject),true,this);
    registerTask(taskCloseProject,taskNameToString(TaskCloseProject));
}

Qtilities::ProjectManagement::ProjectManager::~ProjectManager()
{
    if (d->config_widget)
        delete d->config_widget;
    delete d;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::ProjectManagement::ProjectManager::allowedProjectTypes() const {
    return d->project_types;
}

void Qtilities::ProjectManagement::ProjectManager::setAllowedProjectTypes(IExportable::ExportModeFlags project_types) {
    d->project_types = project_types;

    if (!(d->project_types & d->default_project_type)) {
        if (d->project_types & IExportable::Binary)
            d->default_project_type = IExportable::Binary;
        if (d->project_types & IExportable::XML)
            d->default_project_type = IExportable::XML;
    }
}

void Qtilities::ProjectManagement::ProjectManager::setProjectTypeSuffix(IExportable::ExportMode project_type, const QString& suffix) {
    d->suffices[project_type] = suffix;
}

QString Qtilities::ProjectManagement::ProjectManager::projectTypeSuffix(IExportable::ExportMode project_type) {
    return d->suffices[project_type];
}

Qtilities::Core::Interfaces::IExportable::ExportMode Qtilities::ProjectManagement::ProjectManager::defaultProjectType() const {
    return d->default_project_type;
}

void Qtilities::ProjectManagement::ProjectManager::setDefaultProjectType(IExportable::ExportMode default_project_type) {
    if (d->project_types & default_project_type)
        d->default_project_type = default_project_type;
}

QString Qtilities::ProjectManagement::ProjectManager::allowedProjectTypesFilter() const {
    QStringList filter_list;
    if (d->project_types & IExportable::Binary)
        filter_list.append(QString(tr("Binary Project File (*.%1)")).arg(d->suffices[IExportable::Binary]));
    if (d->project_types & IExportable::XML)
        filter_list.append(QString(tr("XML Project File (*.%1)")).arg(d->suffices[IExportable::XML]));

    return filter_list.join(";;");
}

bool Qtilities::ProjectManagement::ProjectManager::isAllowedFileName(const QString& file_name) const {
    QFileInfo fi(file_name);
    if (d->suffices.values().contains(fi.completeSuffix()))
        return true;
    else
        return false;
}

bool Qtilities::ProjectManagement::ProjectManager::newProject() {
    // Check if a current project exist.
    // If not, create it.
    if (!d->current_project) {
        d->current_project = new Project;
        connect(d->current_project,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
        d->current_project->setProjectItems(d->item_list,true);
        return true;
    } else {
        if (closeProject()) {
            d->current_project = new Project;
            connect(d->current_project,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
            d->current_project->setProjectItems(d->item_list,true);
            return true;
        }
    }

    return false;
}

bool Qtilities::ProjectManagement::ProjectManager::closeProject(){
    if (d->current_project) {
        if (activeProjectBusy()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Project Busy");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText("You cannot close the current project while it is busy.<br>Wait for it to become idle and try again.");
            msgBox.exec();
            return false;
        }

        int task_id = findTaskID(taskNameToString(TaskCloseProject));
        Task* task_ref = 0;
        if (isTaskActive(task_id)) {
            task_ref = findTask(taskNameToString(TaskCloseProject));
            Q_ASSERT(task_ref);
            task_ref->startTask(-1,tr("Closing Project"));
        }

        emit projectClosingStarted(d->current_project->projectFile());
        bool success = true;

        if (d->current_project->isModified())
            success = saveProject(d->current_project->projectFile());

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        d->current_project->closeProject(task_ref);
        QApplication::restoreOverrideCursor();
        d->current_project->disconnect(this);
        delete d->current_project;
        d->current_project = 0;

        emit modificationStateChanged(false);
        emit projectClosingFinished(success);

        if (task_ref) {
            task_ref->setDisplayName(tr("Closed Project"));
            task_ref->completeTask(ITask::TaskSuccessful);
        }
    }
    return true;
}

bool Qtilities::ProjectManagement::ProjectManager::openProject(const QString& file_name) {
    // If the project is already open tell the user, don't open it again.
    if (d->current_project) {
        if (d->current_project->projectFile() == file_name) {
            LOG_INFO_P(QString(tr("The project you are trying to open is already open.")));
            return false;
        }
        if (!closeProject())
            return false;
        delete d->current_project;
    }

    int task_id = findTaskID(taskNameToString(TaskOpenProject));
    Task* task_ref = 0;
    if (isTaskActive(task_id)) {
        task_ref = findTask(taskNameToString(TaskOpenProject));
        Q_ASSERT(task_ref);
        QFileInfo fi(file_name);
        task_ref->setDisplayName(tr("Opening Project: ") + fi.fileName());
        task_ref->startTask(-1,tr("Opening Project: ") + fi.fileName(),Logger::Info);
    }

    markProjectAsChangedDuringLoad(false);
    emit projectLoadingStarted(file_name);

    d->current_project = new Project(this);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    connect(d->current_project,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
    d->current_project->setProjectItems(d->item_list);
    if (!d->current_project->loadProject(file_name,false,task_ref)) {
        delete d->current_project;
        QApplication::restoreOverrideCursor();
        emit projectLoadingFinished(file_name,false);
        markProjectAsChangedDuringLoad(false);

        if (task_ref) {
            QFileInfo fi(file_name);
            task_ref->setDisplayName(tr("Opened Project: ") + fi.fileName());
            task_ref->completeTask(ITask::TaskFailed);
        }

        return false;
    }

    addRecentProject(d->current_project);
    QApplication::restoreOverrideCursor();
    emit projectLoadingFinished(file_name,true);
    markProjectAsChangedDuringLoad(false);

    if (task_ref) {
        QFileInfo fi(file_name);
        task_ref->setDisplayName(tr("Opened Project: ") + fi.fileName());
        task_ref->completeTask(ITask::TaskSuccessful);
    }

    return true;
}

bool Qtilities::ProjectManagement::ProjectManager::saveProject(QString file_name, bool respect_project_busy) {
    if (!d->current_project)
        return false; 

    if (activeProjectBusy() && respect_project_busy) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Project Busy");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("You cannot save the current project while it is busy.<br>Wait for it to become idle and try again.");
        msgBox.exec();
        return false;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (file_name.isEmpty()) {
        if (d->current_project->projectFile().isEmpty()) {
            QString filter = allowedProjectTypesFilter();
            QString project_path;
            if (PROJECT_MANAGER->useCustomProjectsPath())
                project_path = PROJECT_MANAGER->customProjectsPath();
            else
                project_path = QCoreApplication::applicationDirPath() + "/Projects";
            file_name = QFileDialog::getSaveFileName(0, tr("Save Project"),project_path, filter);
            if (file_name.isEmpty()) {
                QApplication::restoreOverrideCursor();
                return false;
            } else {
                if (!isAllowedFileName(file_name))
                    file_name.append(projectTypeSuffix(defaultProjectType()));
            }
        } else
            file_name = d->current_project->projectFile();
    }

    int task_id = findTaskID(taskNameToString(TaskSaveProject));
    Task* task_ref = 0;
    if (isTaskActive(task_id)) {
        task_ref = findTask(taskNameToString(TaskSaveProject));
        Q_ASSERT(task_ref);
        QFileInfo fi(file_name);
        task_ref->setDisplayName(tr("Saving Project: ") + fi.fileName());
        task_ref->startTask(-1,tr("Saving Project: ") + fi.fileName(),Logger::Info);
    }

    emit projectSavingStarted(file_name);

    if (d->current_project->saveProject(file_name,task_ref)) {
        addRecentProject(d->current_project);
        QApplication::restoreOverrideCursor();

        emit projectSavingFinished(file_name,true);

        if (task_ref) {
            QFileInfo fi(file_name);
            task_ref->setDisplayName(tr("Saved Project: ") + fi.fileName());
            task_ref->completeTask(ITask::TaskSuccessful);
        }

        return true;
    }

    QApplication::restoreOverrideCursor();
    emit projectSavingFinished(file_name,false);

    if (task_ref) {
        QFileInfo fi(file_name);
        task_ref->setDisplayName(tr("Saved Project: ") + fi.fileName());
        task_ref->completeTask(ITask::TaskFailed);
    }

    return false;
}

QString Qtilities::ProjectManagement::ProjectManager::currentProjectName() const {
    if (d->current_project)
        return d->current_project->projectName();
    else
        return QString();
}

QString Qtilities::ProjectManagement::ProjectManager::currentProjectFileName() const {
    if (d->current_project)
        return d->current_project->projectFile();
    else
        return QString();
}

Qtilities::ProjectManagement::IProject* Qtilities::ProjectManagement::ProjectManager::currentProject() const {
    return d->current_project;
}

void Qtilities::ProjectManagement::ProjectManager::refreshPartList() {
    // Get a list of all the project items in the system.
    QList<QObject*> projectItemObjects = OBJECT_MANAGER->registeredInterfaces("IProjectItem");
    QList<IProjectItem*> projectItems;
    QStringList itemNames;
    bool success = true;
    // Check all items
    for (int i = 0; i < projectItemObjects.count(); i++) {
        IProjectItem* part = qobject_cast<IProjectItem*> (projectItemObjects.at(i));
        if (part) {
            if (!itemNames.contains(part->projectItemName())) {
                projectItems.append(part);
                LOG_INFO(QString(tr("Project Manager: Found project item: %1")).arg(part->projectItemName()));
            } else {
                LOG_ERROR(tr("The project manager found duplicate project items called: ") + part->projectItemName() + tr(", the second occurance is on object: ") + projectItemObjects.at(i)->objectName());
                success = false;
            }
        } else {
            LOG_ERROR(tr("The project manager found in invalid project item interface on object: ") + projectItemObjects.at(i)->objectName());
            success = false;
        }
    }
    setProjectItemList(projectItems);
}

void Qtilities::ProjectManagement::ProjectManager::setActiveProjectBusy(bool is_busy) {
    int previous_count = d->current_project_busy_count;

    if (is_busy) {
        ++d->current_project_busy_count;
    } else {
        if (d->current_project_busy_count > 0)
            --d->current_project_busy_count;
        else
            qWarning() << "setActiveProjectBusy(false) called too many times on QtilitiesCoreApplication";
    }

    #ifdef QTILITIES_BENCHMARKING
    qDebug() << "Settings current project busy: " << d->current_project_busy_count;
    #endif

    if (previous_count == 0 && d->current_project_busy_count == 1) {
        // Project becomes busy:
        QtilitiesCoreApplication::setApplicationBusy(true);
    } else if (previous_count == 1 && d->current_project_busy_count == 0) {
        // Project not busy anymore:
        QtilitiesCoreApplication::setApplicationBusy(false);
    }
}

bool Qtilities::ProjectManagement::ProjectManager::activeProjectBusy() const {
    return (d->current_project_busy_count > 0);
}

QStringList Qtilities::ProjectManagement::ProjectManager::recentProjectNames() const {
    QStringList recent_project_names;
    for (int i = 0; i < d->recent_project_names.values().count();i++)
        recent_project_names << d->recent_project_names.values().at(i).toString();
    return recent_project_names;
}

QStringList Qtilities::ProjectManagement::ProjectManager::recentProjectPaths() const {
    return d->recent_project_names.keys();
}

QString Qtilities::ProjectManagement::ProjectManager::recentProjectPath(const QString& recent_project_name) const {
    for (int i = 0; i < d->recent_project_names.size(); i++) {
        if (d->recent_project_names.values().at(i).toString() == recent_project_name)
            return d->recent_project_names.keys().at(i);
    }
    return QString();
}

void Qtilities::ProjectManagement::ProjectManager::clearRecentProjects() {
    d->recent_project_names.clear();
    d->recent_project_stack.clear();
    LOG_INFO(tr("Successfully cleared recent project list."));
}

void Qtilities::ProjectManagement::ProjectManager::setProjectItemList(QList<IProjectItem*> item_list) {
    d->item_list = item_list;
}

QStringList Qtilities::ProjectManagement::ProjectManager::registeredProjectItemNames() const {
    QStringList list;
    for (int i = 0; i < d->item_list.count(); i++) {
        if (d->item_list.at(i))
            list << d->item_list.at(i)->projectItemName();
    }
    return list;
}

QWidget* Qtilities::ProjectManagement::ProjectManager::configWidget() {
    if (!d->config_widget)
        d->config_widget = new ProjectManagementConfig();

    return d->config_widget;
}

void Qtilities::ProjectManagement::ProjectManager::setOpenLastProjectOnStartup(bool toggle) {
    d->open_last_project = toggle;
    writeSettings();
}

bool Qtilities::ProjectManagement::ProjectManager::openLastProjectOnStartup() const {
    return d->open_last_project;
}

void Qtilities::ProjectManagement::ProjectManager::setCreateNewProjectOnStartup(bool toggle) {
    d->auto_create_new_project = toggle;
    writeSettings();
}

bool Qtilities::ProjectManagement::ProjectManager::createNewProjectOnStartup() const {
    return d->auto_create_new_project;
}

void Qtilities::ProjectManagement::ProjectManager::setCustomProjectsPath(const QString& projects_path) {
    d->custom_projects_path = QDir::toNativeSeparators(QDir::cleanPath(projects_path));
}

QString Qtilities::ProjectManagement::ProjectManager::customProjectsPath() const {
    return d->custom_projects_path;
}

bool Qtilities::ProjectManagement::ProjectManager::useCustomProjectsPath() const {
    return d->use_custom_projects_path;
}

void Qtilities::ProjectManagement::ProjectManager::setUseCustomProjectsPath(bool toggle) {
    d->use_custom_projects_path = toggle;
}

bool Qtilities::ProjectManagement::ProjectManager::checkModifiedOpenProjects() const {
    return d->check_modified_projects;
}

void Qtilities::ProjectManagement::ProjectManager::setCheckModifiedOpenProjects(bool toggle) {
    d->check_modified_projects = toggle;
}

Qtilities::ProjectManagement::ProjectManager::ModifiedProjectsHandlingPolicy Qtilities::ProjectManagement::ProjectManager::modifiedProjectsHandlingPolicy() const {
    return d->modified_projects_handling_policy;
}

void Qtilities::ProjectManagement::ProjectManager::setModifiedProjectsHandlingPolicy(ModifiedProjectsHandlingPolicy handling_policy) {
    d->modified_projects_handling_policy = handling_policy;

}

void Qtilities::ProjectManagement::ProjectManager::writeSettings() const {
    if (!QtilitiesCoreApplication::qtilitiesSettingsPathEnabled())
        return;

    // Write the settings:
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Projects");
    settings.setValue("open_last_project", d->open_last_project);
    settings.setValue("auto_create_new_project", d->auto_create_new_project);
    settings.setValue("recent_project_map", d->recent_project_names);
    settings.setValue("recent_project_stack", d->recent_project_stack);
    settings.setValue("use_custom_projects_path", d->use_custom_projects_path);
    settings.setValue("check_modified_projects", d->check_modified_projects);
    settings.setValue("modified_projects_handling_policy", QVariant((int) d->modified_projects_handling_policy));
    settings.setValue("custom_projects_path", QVariant(d->custom_projects_path));
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::ProjectManagement::ProjectManager::readSettings() {
    // Load project management paramaters using QSettings()
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Projects");
    d->open_last_project = settings.value("open_last_project", false).toBool();
    d->auto_create_new_project = settings.value("auto_create_new_project", false).toBool();
    d->use_custom_projects_path = settings.value("use_custom_projects_path", false).toBool();
    d->check_modified_projects = settings.value("check_modified_projects", true).toBool();
    d->modified_projects_handling_policy = (ProjectManager::ModifiedProjectsHandlingPolicy) settings.value("modified_projects_handling_policy", 0).toInt();
    d->custom_projects_path = settings.value("custom_projects_path",QtilitiesApplication::applicationSessionPath() + QDir::separator() + "Projects").toString();
    d->recent_project_names = settings.value("recent_project_map", false).toMap();
    d->recent_project_stack = settings.value("recent_project_stack", QStringList()).toStringList();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::ProjectManagement::ProjectManager::initialize() {
    LOG_INFO(tr("Qtilities Project Management Framework, initialization started..."));
    readSettings();

    bool success = true;
    refreshPartList();

    // Settings was already read in the constructor.
    if (d->open_last_project) {
        QString last_project;
        if (d->recent_project_names.count() > 0 && d->recent_project_stack.count() > 0)
            last_project = d->recent_project_stack.front();

        if (!last_project.isEmpty()) {
            LOG_INFO_P(tr("Opening project from last session from path: ") + last_project);
            QApplication::processEvents();
            if (!openProject(last_project)) {
                // We create a empty project when the last project was not valid and auto create project is set.
                if (newProject())
                    LOG_INFO(tr("Successfully created new project in place of failed project."));
                else {
                    LOG_ERROR(tr("Failed to create a new project in place of failed project."));
                    success = false;
                }
            }
        } else if (d->auto_create_new_project) {
            if (newProject())
                LOG_INFO(tr("Successfully created new project on application startup."));
            else {
                LOG_ERROR(tr("Failed to create a new project on startup."));
                success = false;
            }
        }
    }

    d->is_initialized = true;
    if (success)
        LOG_INFO(tr("Qtilities Project Management Framework, initialization finished successfully..."));
    else
        LOG_WARNING(tr("Qtilities Project Management Framework, initialization finished with some errors..."));
}

void Qtilities::ProjectManagement::ProjectManager::finalize() {
    if (!d->is_initialized)
        LOG_WARNING(tr("ProjectManager is finalized without being initialized first."));

    if (d->current_project) {
        if (d->check_modified_projects && d->current_project->isModified()) {
            if (d->modified_projects_handling_policy == PromptUser) {
                QMessageBox msgBox;
                msgBox.setWindowTitle("Project Manager");
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText("Do you want to save your current project before you exit?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                if (ret == QMessageBox::Yes) {
                    saveProject();
                }
            } else if (d->modified_projects_handling_policy == AutoSave) {
                saveProject();
            }
        }
    }

    writeSettings();
}

void Qtilities::ProjectManagement::ProjectManager::markProjectAsChangedDuringLoad(bool changed) {
    d->project_changed_during_load = changed;
}

bool Qtilities::ProjectManagement::ProjectManager::projectChangedDuringLoad() {
    return d->project_changed_during_load;
}

void Qtilities::ProjectManagement::ProjectManager::addRecentProject(IProject* project) {
    if (!project)
        return;

    // First check if this project already exists in the list.
    // If so, move it to the first position and exit.
    if (d->recent_project_stack.contains(project->projectFile())) {
        // Remove current.
        d->recent_project_stack.removeOne(project->projectFile());
        // Add again in new position.
        d->recent_project_stack.push_front(project->projectFile());
        return;
    }

    // Check the required size of the recent projects list.
    if (d->recent_project_stack.count() == d->recent_projects_size) {
        d->recent_project_names.remove(d->recent_project_stack.last());
        d->recent_project_stack.removeLast();
    }

    // Now add the new project.
    d->recent_project_names[project->projectFile()] = project->projectName();
    d->recent_project_stack.push_front(project->projectFile());
}

bool Qtilities::ProjectManagement::ProjectManager::isModified() const {
    if (d->current_project) {
        return d->current_project->isModified();
    }

    return false;
}

void Qtilities::ProjectManagement::ProjectManager::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
    if (notification_targets & IModificationNotifier::NotifySubjects) {
        d->current_project->setModificationState(new_state,notification_targets);
    }
}

