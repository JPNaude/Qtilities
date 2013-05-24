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

#include "ProjectManager.h"
#include "Project.h"
#include "ProjectManagementConfig.h"

#include <QtilitiesCore>

#include <QtilitiesApplication>
#include <QtilitiesCoreGuiConstants>

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
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::ProjectManagement::Constants;

struct Qtilities::ProjectManagement::ProjectManagerPrivateData  {
    ProjectManagerPrivateData() : current_project(0),
        current_project_busy_count(0),
        open_last_project(false),
        use_project_file_locks(true),
        default_custom_project_paths_category( QObject::tr("Default")),
        is_initialized(false),
        project_types(IExportable::Binary | IExportable::XML),
        default_project_type(IExportable::XML),
        project_changed_during_load(false),
        exec_style(ProjectManager::ExecNormal),
        saving_enabled(true),
        project_actions_menu_id(qti_action_FILE),
        project_actions_command_id(qti_action_FILE_SETTINGS),
        actionProjectNew(0),
        actionProjectOpen(0),
        actionProjectClose(0),
        actionProjectSave(0),
        actionProjectSaveAs(0),
        recent_projects_size(10) {}

    QPointer<Project>                       current_project;
    int                                     current_project_busy_count;
    QList<IProjectItem*>                    item_list;
    QPointer<ProjectManagementConfig>       config_widget;
    bool                                    open_last_project;
    bool                                    use_project_file_locks;
    bool                                    auto_create_new_project;
    bool                                    use_custom_projects_paths;
    // Keys = Categories, Values = Paths
    QMap<QString,QVariant>                  custom_projects_paths;
    QString                                 default_custom_project_paths_category;
    bool                                    check_modified_projects;
    ProjectManager::ModifiedProjectsHandlingPolicy  modified_projects_handling_policy;
    bool                                    is_initialized;
    IExportable::ExportModeFlags            project_types;
    IExportable::ExportMode                 default_project_type;
    QMap<IExportable::ExportMode,QString>   suffices;
    bool                                    project_changed_during_load;
    ProjectManager::ExecStyle               exec_style;
    bool                                    saving_enabled;
    QString                                 saving_info_message;

    FileLocker                              file_locker;

    QString                                 project_actions_menu_id;
    QString                                 project_actions_command_id;

    QAction*                                actionProjectNew;
    QAction*                                actionProjectOpen;
    QAction*                                actionProjectClose;
    QAction*                                actionProjectSave;
    QAction*                                actionProjectSaveAs;

    // Recent projects:
    QList<QMenu*>                           recentProjectsMenus;
    QMap<QString, QVariant>                 recent_project_names; // Keys = Paths, Values = Names
    QStringList                             recent_project_stack;
    int                                     recent_projects_size;
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
    if (d->suffices.contains(project_type))
        return d->suffices[project_type];
    else
        return QString();
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

IExportable::ExportMode ProjectManagement::ProjectManager::projectTypeFromTypeFilter(const QString &project_type_filter_string) const {
    if (d->project_types & IExportable::Binary) {
        if (project_type_filter_string.contains(d->suffices[IExportable::Binary]))
            return IExportable::Binary;
    }
    if (d->project_types & IExportable::XML) {
        if (project_type_filter_string.contains(d->suffices[IExportable::XML]))
            return IExportable::XML;
    }
    return IExportable::None;
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
    emit projectCreationStarted();
    if (!d->current_project) {
        d->current_project = new Project;
        connect(d->current_project,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
        d->current_project->setProjectItems(d->item_list,true);
        emit projectCreationFinished();
        emit currentProjectChanged(d->current_project);
        return true;
    } else {
        if (closeProject()) {
            d->current_project = new Project;
            connect(d->current_project,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
            d->current_project->setProjectItems(d->item_list,true);
            emit projectCreationFinished();
            emit currentProjectChanged(d->current_project);
            return true;
        }
    }
    emit projectCreationFinished();
    return false;
}

bool Qtilities::ProjectManagement::ProjectManager::closeProject() {
    if (d->current_project) {
        if (activeProjectBusy()) {          
            QString msg = tr("You cannot save the current project while it is busy.<br>Wait for it to become idle and try again.");
            if (d->exec_style != ExecSilent) {
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Project Busy"));
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(msg);
                msgBox.exec();
            } else {
                LOG_ERROR_P(msg);
            }
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
        emit currentProjectChanged(d->current_project);

        emit modificationStateChanged(false);
        emit projectClosingFinished(success);

        if (task_ref) {
            task_ref->setDisplayName(tr("Closed Project"));
            task_ref->completeTask();
        }
    }
    return true;
}

bool Qtilities::ProjectManagement::ProjectManager::openProject(const QString& file_name) {
    QFileInfo fi(file_name);
    if (!fi.exists()) {
        LOG_ERROR_P(tr("The project you are trying to open does not exist at: ") + file_name + tr(". The open project operation will not continue."));
        return false;
    }

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

    // Check if the project is locked:
    if (d->use_project_file_locks) {
        bool is_file_locked = d->file_locker.isFileLocked(file_name);
        if (is_file_locked) {
            if (PROJECT_MANAGER->executionStyle() == ProjectManager::ExecNormal) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText(tr("The project you are trying to open is currently locked.<br><br>") + d->file_locker.lastLockSummary(file_name,"<br>"));
                msgBox.setInformativeText(tr("Do you want to break the lock in order to continue opening it?"));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Cancel);
                int ret = msgBox.exec();
                switch (ret) {
                    case QMessageBox::Yes:
                        {
                            QString errorMsg;
                            if (!d->file_locker.unlockFile(file_name,&errorMsg)) {
                                LOG_TASK_ERROR(errorMsg,task_ref);
                                if (task_ref)
                                    task_ref->completeTask(ITask::TaskFailed);
                                return false;
                            }
                        }
                        break;
                    case QMessageBox::Cancel:
                        // In this case we say the project was locked and abort.
                        LOG_TASK_ERROR(tr("Project file is locked, you decided not to release the lock: ") + file_name,task_ref);
                        if (task_ref)
                            task_ref->completeTask(ITask::TaskFailed);
                        return false;
                        break;
                    default:
                        // should never be reached
                        break;
                }
            } else if (PROJECT_MANAGER->executionStyle() == ProjectManager::ExecSilent) {
                // In this case we say the project was locked and abort.
                LOG_TASK_ERROR(tr("Project file is locked, cannot open it in silent execution mode: ") + file_name,task_ref);
                if (task_ref)
                    task_ref->completeTask(ITask::TaskFailed);
                return false;
            }
        }
    }

    markProjectAsChangedDuringLoad(false);
    emit projectLoadingStarted(file_name);

    d->current_project = new Project(this);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    connect(d->current_project,SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
    d->current_project->setProjectItems(d->item_list);
    if (!d->current_project->loadProject(file_name,false,task_ref)) {
        // Call close project on all project items in the project.
        // Remember that some of them might have been loaded successfully:
        d->current_project->closeProject(task_ref);

        delete d->current_project;
        QApplication::restoreOverrideCursor();
        emit projectLoadingFinished(file_name,false);
        markProjectAsChangedDuringLoad(false);

        if (task_ref)
            task_ref->completeTask(ITask::TaskFailed);

        return false;
    }

    addRecentProject(d->current_project);
    emit currentProjectChanged(d->current_project);
    QApplication::restoreOverrideCursor();
    markProjectAsChangedDuringLoad(false);

    emit projectLoadingFinished(file_name,true);

    if (task_ref) {
        QFileInfo fi(file_name);
        task_ref->setDisplayName(tr("Opened Project: ") + fi.fileName());
        task_ref->completeTask();
    }

    return true;
}

bool Qtilities::ProjectManagement::ProjectManager::saveProject(QString file_name, bool respect_project_busy) {
    if (!d->current_project) {
        emit projectSaveRequestedWithoutOpenProject();
        return false; 
    }

    if (activeProjectBusy() && respect_project_busy) {
        QString msg = tr("You cannot save the current project while it is busy.<br>Wait for it to become idle and try again.");
        if (d->exec_style != ExecSilent) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Project Busy"));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(msg);
            msgBox.exec();
        } else {
            LOG_ERROR_P(msg);
        }

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
                project_path = QCoreApplication::applicationDirPath() + QDir::separator() + "Projects";

            IExportable::ExportMode target_project_type = PROJECT_MANAGER->defaultProjectType();
            if (d->exec_style != ExecSilent) {
                QString selected_filter;
                file_name = QFileDialog::getSaveFileName(0, tr("Save Project"), project_path, filter, &selected_filter);
                target_project_type = PROJECT_MANAGER->projectTypeFromTypeFilter(selected_filter);
            }

            if (file_name.isEmpty()) {
                QApplication::restoreOverrideCursor();
                return false;
            } else {
                if (!isAllowedFileName(file_name))
                    file_name.append("." + projectTypeSuffix(target_project_type));
            }
        } else
            file_name = d->current_project->projectFile();
    }

    //qDebug() << "Starting project save to " << file_name;

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
            task_ref->completeTask();
        }

        return true;
    }

    QApplication::restoreOverrideCursor();
    emit projectSavingFinished(file_name,false);

    if (task_ref) {
        QFileInfo fi(file_name);
        task_ref->setDisplayName(tr("Saved Project: ") + fi.fileName());
        task_ref->completeTask();
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
    QList<QObject*> projectItemObjects = OBJECT_MANAGER->registeredInterfaces("com.Qtilities.ProjectManagement.IProjectItem/1.0");
    QList<IProjectItem*> projectItems;
    QStringList itemNames;
    // Check all items
    for (int i = 0; i < projectItemObjects.count(); ++i) {
        IProjectItem* part = qobject_cast<IProjectItem*> (projectItemObjects.at(i));
        if (part) {
            if (!itemNames.contains(part->projectItemName())) {
                projectItems.append(part);
                LOG_DEBUG(QString(tr("Project Manager: Found project item: %1")).arg(part->projectItemName()));
            } else
                LOG_ERROR(tr("The project manager found duplicate project items called: ") + part->projectItemName() + tr(", the second occurance is on object: ") + projectItemObjects.at(i)->objectName());
        } else
            LOG_ERROR(tr("The project manager found in invalid project item interface on object: ") + projectItemObjects.at(i)->objectName());
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
    foreach (const QString& stack_name, d->recent_project_stack) {
        recent_project_names << d->recent_project_names[FileUtils::toNativeSeparators(stack_name)].toString();
    }
    return recent_project_names;
}

QStringList Qtilities::ProjectManagement::ProjectManager::recentProjectPaths() const {
    return d->recent_project_stack;
}

QString Qtilities::ProjectManagement::ProjectManager::recentProjectPath(const QString& recent_project_name) const {
    for (int i = 0; i < d->recent_project_names.size(); ++i) {
        if (d->recent_project_names.values().at(i).toString() == recent_project_name)
            return d->recent_project_names.keys().at(i);
    }
    return QString();
}

void Qtilities::ProjectManagement::ProjectManager::clearRecentProjects() {
    d->recent_project_names.clear();
    d->recent_project_stack.clear();
    emit recentProjectsChanged(recentProjectNames(),recentProjectPaths());
    refreshRecentProjects();
    LOG_INFO(tr("Successfully cleared recent project list."));
}

void Qtilities::ProjectManagement::ProjectManager::setProjectItemList(QList<IProjectItem*> item_list) {
    d->item_list = item_list;
}

QStringList Qtilities::ProjectManagement::ProjectManager::registeredProjectItemNames() const {
    QStringList list;
    for (int i = 0; i < d->item_list.count(); ++i) {
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

void ProjectManagement::ProjectManager::setUseProjectFileLocks(bool toggle) {
    d->use_project_file_locks = toggle;
    writeSettings();
}

bool ProjectManagement::ProjectManager::useProjectFileLocks() const {
    return d->use_project_file_locks;
}

void Qtilities::ProjectManagement::ProjectManager::setCreateNewProjectOnStartup(bool toggle) {
    d->auto_create_new_project = toggle;
    writeSettings();
}

bool Qtilities::ProjectManagement::ProjectManager::createNewProjectOnStartup() const {
    return d->auto_create_new_project;
}

void Qtilities::ProjectManagement::ProjectManager::setCustomProjectsPath(const QString& projects_path, QString projects_category) {
    QMap<QString,QVariant> start_map = d->custom_projects_paths;

    QDir dir(projects_path);
    if (!dir.exists()) {
        if (dir.mkpath(projects_path)) {
            LOG_INFO(tr("Successfully created custom projects path at: ") + projects_path);
        }
    }

    if (projects_category.isEmpty())
        projects_category = "Default";
    d->custom_projects_paths[projects_category] = QVariant(FileUtils::toNativeSeparators(QDir::cleanPath(projects_path)));

    if (d->custom_projects_paths != start_map)
        emit customProjectPathsChanged();
}

QString Qtilities::ProjectManagement::ProjectManager::customProjectsPath(QString projects_category) const {
    if (projects_category.isEmpty())
        projects_category = "Default";

    if (!d->custom_projects_paths.contains(projects_category))
        d->custom_projects_paths[projects_category] = QtilitiesApplication::applicationSessionPath() + QDir::separator() + "Projects";

    return d->custom_projects_paths[projects_category].toString();
}

void ProjectManagement::ProjectManager::removeCustomProjectsPath(QString projects_category) {
    if (projects_category == QLatin1String("Default"))
        return;

    d->custom_projects_paths.remove(projects_category);
    emit customProjectPathsChanged();
}

void ProjectManagement::ProjectManager::setDefaultCustomProjectsCategory(const QString &projects_category) {
    d->default_custom_project_paths_category = projects_category;
}

QString ProjectManagement::ProjectManager::defaultCustomProjectsCategory() const {
    if (d->default_custom_project_paths_category.isEmpty())
        d->default_custom_project_paths_category = "Default";
    return d->default_custom_project_paths_category;
}

QStringList ProjectManagement::ProjectManager::customProjectCategories() const {
    return d->custom_projects_paths.keys();
}

bool Qtilities::ProjectManagement::ProjectManager::useCustomProjectsPath() const {
    return d->use_custom_projects_paths;
}

void Qtilities::ProjectManagement::ProjectManager::setUseCustomProjectsPath(bool toggle) {
    d->use_custom_projects_paths = toggle;
}

void ProjectManagement::ProjectManager::setProjectMenuItemsTargetMenu(const QString &menu_id) {
    d->project_actions_menu_id = menu_id;
}

QString ProjectManagement::ProjectManager::projectMenuItemsTargetMenu() const {
    return d->project_actions_menu_id;
}

void ProjectManagement::ProjectManager::setProjectMenuItemsBeforeCommand(const QString &command_id) {
    d->project_actions_command_id = command_id;
}

QString ProjectManagement::ProjectManager::projectMenuItemsBeforeCommand() const {
    return d->project_actions_command_id;
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
    if (!QtilitiesCoreApplication::qtilitiesSettingsEnabled())
        return;

    // Write the settings:
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Projects");
    settings.setValue("open_last_project", d->open_last_project);
    settings.setValue("auto_create_new_project", d->auto_create_new_project);
    settings.setValue("recent_project_map", d->recent_project_names);
    settings.setValue("recent_project_stack", d->recent_project_stack);
    settings.setValue("use_custom_projects_path", d->use_custom_projects_paths);
    settings.setValue("check_modified_projects", d->check_modified_projects);
    settings.setValue("modified_projects_handling_policy", QVariant((int) d->modified_projects_handling_policy));
    settings.setValue("custom_projects_paths", d->custom_projects_paths);
    settings.setValue("default_custom_project_paths_category",defaultCustomProjectsCategory());
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
    d->use_custom_projects_paths = settings.value("use_custom_projects_path", false).toBool();
    d->check_modified_projects = settings.value("check_modified_projects", true).toBool();
    d->modified_projects_handling_policy = (ProjectManager::ModifiedProjectsHandlingPolicy) settings.value("modified_projects_handling_policy", 0).toInt();
    d->recent_project_names = settings.value("recent_project_map", false).toMap();
    d->recent_project_stack = settings.value("recent_project_stack", QStringList()).toStringList();
    d->custom_projects_paths = settings.value("custom_projects_paths", false).toMap();
    d->default_custom_project_paths_category = settings.value("default_custom_project_paths_category","Default").toString();

    // This is for backward compatibility with Qtilities v1.0:
    // If there was a custom project path saved in Qtilities v1.0, we load it and add it to the list of current custom project paths as the default:
    if (settings.contains("custom_projects_path")) {
        QString old_custom_projects_path = settings.value("custom_projects_path",QtilitiesApplication::applicationSessionPath() + QDir::separator() + "Projects").toString();
        setCustomProjectsPath(old_custom_projects_path,"Default");
        // Now clear it:
        //settings.remove("custom_projects_path");
    }

    settings.endGroup();
    settings.endGroup();
}

void Qtilities::ProjectManagement::ProjectManager::initialize() {
    if (d->is_initialized)
        return;

    LOG_DEBUG(tr("Qtilities Project Management Framework, initialization started..."));
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

    connect(this,SIGNAL(modificationStateChanged(bool)),SLOT(handle_projectStateChanged()));
    connect(this,SIGNAL(projectClosingFinished(bool)),SLOT(handle_projectStateChanged()));
    connect(this,SIGNAL(projectLoadingFinished(QString,bool)),SLOT(handle_projectStateChanged()));
    connect(this,SIGNAL(projectCreationFinished()),SLOT(handle_projectStateChanged()));

    //connect(QtilitiesCoreApplication::instance(),SIGNAL(busyStateChanged(bool)),SLOT(handleApplicationBusyStateChanged()));

    d->is_initialized = true;
    if (success)
        LOG_DEBUG(tr("Qtilities Project Management Framework, initialization finished successfully..."));
    else
        LOG_DEBUG(tr("Qtilities Project Management Framework, initialization finished with some errors..."));
}

void Qtilities::ProjectManagement::ProjectManager::finalize() {
    if (!d->is_initialized)
        LOG_WARNING(tr("ProjectManager is finalized without being initialized first."));

    if (d->current_project && d->saving_enabled) {
        if (d->check_modified_projects && d->current_project->isModified()) {
            if (d->modified_projects_handling_policy == PromptUser) {
                if (d->exec_style != ExecSilent) {
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
                } else {
                    LOG_INFO(tr("Project manager is in silent execution mode. Auto-saving instead of prompting user."));
                    saveProject();
                }
            } else if (d->modified_projects_handling_policy == AutoSave) {
                saveProject();
            }
        }
    }

    if (d->current_project)
        delete d->current_project;
    writeSettings();
}

void Qtilities::ProjectManagement::ProjectManager::markProjectAsChangedDuringLoad(bool changed) {
    d->project_changed_during_load = changed;
}

bool Qtilities::ProjectManagement::ProjectManager::projectChangedDuringLoad() {
    return d->project_changed_during_load;
}

ProjectManagement::ProjectManager::ExecStyle ProjectManagement::ProjectManager::executionStyle() const {
    return d->exec_style;
}

void ProjectManagement::ProjectManager::setExecutionStyle(ProjectManagement::ProjectManager::ExecStyle exec_style) {
    d->exec_style = exec_style;
}

void ProjectManagement::ProjectManager::removeRecentProject(const QString &path) {
    // Remove it from the stack if its in there:
    QString clean_path = FileUtils::toNativeSeparators(QDir::cleanPath(path));
    if (d->recent_project_stack.contains(clean_path)) {
        d->recent_project_stack.removeOne(clean_path);
        d->recent_project_names.remove(clean_path);
    }
}

QStringList ProjectManagement::ProjectManager::removeNonExistingRecentProjects() {
    QStringList removed_paths;
    // Remove it from the stack if its in there:
    foreach (const QString& path, d->recent_project_stack) {
        QFileInfo fi(path);
        if (!fi.exists()) {
            removed_paths << path;
        }
    }

    foreach (const QString& path, removed_paths)
        removeRecentProject(path);

    emit recentProjectsChanged(recentProjectNames(),recentProjectPaths());
    return removed_paths;
}

void ProjectManagement::ProjectManager::toggleProjectSaving(bool is_enabled, const QString &info_message) {
    d->saving_enabled = is_enabled;
    d->saving_info_message = info_message;
}

bool ProjectManagement::ProjectManager::projectSavingEnabled() const {
    return d->saving_enabled;
}

QString ProjectManagement::ProjectManager::projectSavingInfoMessage() const {
    return d->saving_info_message;
}

QAction *ProjectManagement::ProjectManager::actionNewProject() {
    if (!d->actionProjectNew) {
        d->actionProjectNew = new QAction(QIcon(),tr("New Project"),this);
        d->actionProjectNew->setShortcut(QKeySequence(QKeySequence::New));
        connect(d->actionProjectNew,SIGNAL(triggered()),SLOT(handle_actionProjectNew()));
        Command* command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_NEW,d->actionProjectNew);
        command->setCategory(QtilitiesCategory(tr("Projects")));
    }

    return d->actionProjectNew;
}

QAction *ProjectManagement::ProjectManager::actionOpenProject() {
    if (!d->actionProjectOpen) {
        d->actionProjectOpen = new QAction(QIcon(),tr("Open Project"),this);
        d->actionProjectOpen->setShortcut(QKeySequence(QKeySequence::Open));
        connect(d->actionProjectOpen,SIGNAL(triggered()),SLOT(handle_actionProjectOpen()));
        Command* command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_OPEN,d->actionProjectOpen);
        command->setCategory(QtilitiesCategory(tr("Projects")));
    }

    return d->actionProjectOpen;
}

QAction *ProjectManagement::ProjectManager::actionCloseProject() {
    if (!d->actionProjectClose) {
        d->actionProjectClose = new QAction(QIcon(),tr("Close Project"),this);
        d->actionProjectClose->setEnabled(false);
        connect(d->actionProjectClose,SIGNAL(triggered()),SLOT(handle_actionProjectClose()));
        Command* command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_CLOSE,d->actionProjectClose);
        command->setCategory(QtilitiesCategory(tr("Projects")));
    }

    return d->actionProjectClose;
}

QAction *ProjectManagement::ProjectManager::actionSaveProject() {
    if (!d->actionProjectSave) {
        d->actionProjectSave = new QAction(QIcon(),tr("Save Project"),this);
        d->actionProjectSave->setEnabled(false);
        d->actionProjectSave->setShortcut(QKeySequence(QKeySequence::Save));
        connect(d->actionProjectSave,SIGNAL(triggered()),SLOT(handle_actionProjectSave()));
        Command* command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_SAVE,d->actionProjectSave);
        command->setCategory(QtilitiesCategory(tr("Projects")));
    }

    return d->actionProjectSave;
}

QAction *ProjectManagement::ProjectManager::actionSaveAsProject() {
    if (!d->actionProjectSaveAs) {
        d->actionProjectSaveAs = new QAction(QIcon(),tr("Save Project As"),this);
        d->actionProjectSaveAs->setEnabled(false);
        d->actionProjectSaveAs->setShortcut(QKeySequence(QKeySequence::SaveAs));
        connect(d->actionProjectSaveAs,SIGNAL(triggered()),SLOT(handle_actionProjectSaveAs()));
        Command* command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_SAVE_AS,d->actionProjectSaveAs);
        command->setCategory(QtilitiesCategory(tr("Projects")));
    }

    return d->actionProjectSaveAs;
}

ActionContainer *ProjectManagement::ProjectManager::recentProjectsMenuContainer() {
    bool existed;
    ActionContainer* action_container = ACTION_MANAGER->createMenu(qti_action_PROJECTS_RECENT,existed);

    QMenu* menu = action_container->menu();
    menu->setTitle(tr("Recent Projects"));

    d->recentProjectsMenus.append(menu);
    refreshRecentProjects();

    return action_container;
}

void ProjectManagement::ProjectManager::handle_actionProjectNew() {
    newProject();
}

void ProjectManagement::ProjectManager::handle_actionProjectOpen() {
    QString filter = allowedProjectTypesFilter();
    QString project_path;
    if (useCustomProjectsPath())
        project_path = customProjectsPath(defaultCustomProjectsCategory());
    else
        project_path = QCoreApplication::applicationDirPath() + tr("/Projects");
    QString file_name = QFileDialog::getOpenFileName(0, tr("Open Existing Project"), project_path, filter);
    if (file_name.isEmpty())
        return;

    if (!isAllowedFileName(file_name))
        file_name.append("." + projectTypeSuffix(defaultProjectType()));

    openProject(file_name);
}

void ProjectManagement::ProjectManager::handle_actionProjectClose() {
    closeProject();
}

void ProjectManagement::ProjectManager::handle_actionProjectSave() {
    if (!currentProject())
        return;

    if (currentProject()->projectFile().isEmpty()) {
        return handle_actionProjectSaveAs();
    } else {
        saveProject(currentProject()->projectFile());
    }
}

void ProjectManagement::ProjectManager::handle_actionProjectSaveAs() {
    if (!currentProject())
        return;

    if (activeProjectBusy()) {
        if (executionStyle() == ExecNormal) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Project Busy"));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("You cannot save the current project while it is busy.<br>Wait for it to become idle and try again."));
            msgBox.exec();
            return;
        } else
            LOG_WARNING_P(tr("You cannot save the current project while it is busy.<br>Wait for it to become idle and try again."));
    }

    QString filter = allowedProjectTypesFilter();
    QString project_path;
    if (useCustomProjectsPath())
        project_path = customProjectsPath(defaultCustomProjectsCategory());
    else
        project_path = QCoreApplication::applicationDirPath() + "/Projects";

    IExportable::ExportMode target_project_type = defaultProjectType();
    QString selected_filter;
    QString file_name = QFileDialog::getSaveFileName(0, tr("Save Project"),project_path, filter, &selected_filter);
    target_project_type = projectTypeFromTypeFilter(selected_filter);
    if (file_name.isEmpty())
        return;

    if (!isAllowedFileName(file_name))
        file_name.append("." + projectTypeSuffix(target_project_type));

    saveProject(file_name);
}

void ProjectManagement::ProjectManager::handle_projectStateChanged() {
    if (!d->is_initialized)
        return;

    IProject* project = currentProject();
    if (project) {
        // Update actions:
        if (d->actionProjectClose)
            d->actionProjectClose->setEnabled(true);
        if (d->actionProjectNew)
            d->actionProjectNew->setEnabled(true);
        if (d->actionProjectOpen)
            d->actionProjectOpen->setEnabled(true);
        if (d->actionProjectSaveAs)
            d->actionProjectSaveAs->setEnabled(true);
        if (d->actionProjectSave)
            d->actionProjectSave->setEnabled(true);

//        // Check if the project name currently in the main window must change:
//        QWidget* main_window = QtilitiesApplication::mainWindow();
//        if (main_window && project) {
//            // Name was never appended before.
//            if (d->appended_project_name.isEmpty()) {
//                QString new_title = main_window->windowTitle().trimmed();
//                new_title.append(QString(" - %1").arg(project->projectName()));
//                main_window->setWindowTitle(new_title);
//                d->appended_project_name = project->projectName();
//            } else {
//                if (d->appended_project_name != project->projectName()) {
//                    // The name of a previous project was appended.
//                    QString new_title = main_window->windowTitle().trimmed();
//                    new_title.chop(d->appended_project_name.length() + 3);
//                    new_title.append(QString(" - %1").arg(project->projectName()));
//                    main_window->setWindowTitle(new_title);
//                    d->appended_project_name = project->projectName();
//                } else {
//                    // The name of the current project is already appended.
//                }
//            }
//        }

        // Add the * character if neccesarry:
        if (project->isModified()) {
            if (d->actionProjectSave)
                d->actionProjectSave->setEnabled(true);
//            QWidget* main_window = QtilitiesApplication::mainWindow();
//            if (main_window) {
//                if (!main_window->windowTitle().endsWith("*"))
//                    main_window->setWindowTitle(main_window->windowTitle().append("*"));
//            }
        } else {
            if (d->actionProjectSave)
                d->actionProjectSave->setEnabled(false);
//            QWidget* main_window = QtilitiesApplication::mainWindow();
//            if (main_window) {
//                if (main_window->windowTitle().endsWith("*")) {
//                    QString new_title = main_window->windowTitle();
//                    new_title.chop(1);
//                    main_window->setWindowTitle(new_title);
//                }
//            }
        }
    } else {
        if (d->actionProjectClose)
            d->actionProjectClose->setEnabled(false);
        if (d->actionProjectNew)
            d->actionProjectNew->setEnabled(true);
        if (d->actionProjectOpen)
            d->actionProjectOpen->setEnabled(true);
        if (d->actionProjectSave)
            d->actionProjectSave->setEnabled(false);
        if (d->actionProjectSaveAs)
            d->actionProjectSaveAs->setEnabled(false);

//        QWidget* main_window = QtilitiesApplication::mainWindow();
//        if (main_window) {
//            // Remove possible *:
//            if (main_window->windowTitle().endsWith("*")) {
//                QString new_title = main_window->windowTitle();
//                new_title.chop(1);
//                main_window->setWindowTitle(new_title);
//            }

//            // Remove possible last appended name:
//            if (!d->appended_project_name.isEmpty()) {
//                QString new_title = main_window->windowTitle();
//                new_title.chop(d->appended_project_name.length() + 3);
//                main_window->setWindowTitle(new_title);
//                d->appended_project_name.clear();
//            }
//        }
    }

    refreshRecentProjects();
}

void ProjectManagement::ProjectManager::handleApplicationBusyStateChanged() {
    if (QtilitiesApplication::applicationBusy()) {
        if (d->actionProjectClose)
            d->actionProjectClose->setEnabled(false);
        if (d->actionProjectNew)
            d->actionProjectNew->setEnabled(false);
        if (d->actionProjectOpen)
            d->actionProjectOpen->setEnabled(false);
        if (d->actionProjectSave)
            d->actionProjectSave->setEnabled(false);
        if (d->actionProjectSaveAs)
            d->actionProjectSaveAs->setEnabled(false);
        // Need to loop through all of them.
//        if (d->recentProjectsMenus)
//            d->recentProjectsMenus->setEnabled(false);
    } else{
        if (d->actionProjectClose)
            d->actionProjectClose->setEnabled(true);
        if (d->actionProjectNew)
            d->actionProjectNew->setEnabled(true);
        if (d->actionProjectOpen)
            d->actionProjectOpen->setEnabled(true);
        if (d->actionProjectSave)
            d->actionProjectSave->setEnabled(true);
        if (d->actionProjectSaveAs)
            d->actionProjectSaveAs->setEnabled(true);
//        if (d->recentProjectsMenus)
//            d->recentProjectsMenus->setEnabled(true);
    }
}

void ProjectManagement::ProjectManager::refreshRecentProjects() {    
    if (d->recentProjectsMenus.isEmpty())
        return;

    QStringList names = recentProjectNames();
    QStringList paths = recentProjectPaths();

    if (names.count() != paths.count()) {
        qWarning() << "refreshRecentProjects::handleRecentProjectChanged received invalid parameters.";
        return;
    }

    foreach (QMenu* menu, d->recentProjectsMenus)
        menu->clear();

    int recent_count = 0;
    for (int i = 0; i < paths.count(); ++i) {
        QString name = names.at(i);
        // Skip the first project if it is the current open project:
        if (name == currentProjectName())
            continue;

        if (name.isEmpty())
            continue;

        QAction* prev_action = new QAction(paths.at(i),this);
        prev_action->setToolTip(paths.at(i));
        prev_action->setObjectName(paths.at(i));

        foreach (QMenu* menu, d->recentProjectsMenus)
            menu->addAction(prev_action);

        connect(prev_action,SIGNAL(triggered()),SLOT(handleRecentProjectActionTriggered()));
        ++recent_count;
        continue;
    }

    if (recent_count > 0) {
        QAction* clear_recent_action = new QAction(tr("Clear List"),0);
        connect(clear_recent_action,SIGNAL(triggered()),SLOT(clearRecentProjects()));
        QAction* remove_non_existing_recent_action = new QAction(tr("Remove Non-Existing"),0);
        connect(remove_non_existing_recent_action,SIGNAL(triggered()),SLOT(removeNonExistingRecentProjects()));

        foreach (QMenu* menu, d->recentProjectsMenus) {
            menu->addSeparator();
            menu->addAction(clear_recent_action);
            menu->addAction(remove_non_existing_recent_action);
            menu->setEnabled(true);
        }
    } else {
        foreach (QMenu* menu, d->recentProjectsMenus)
            menu->setEnabled(false);
    }
}

void ProjectManagement::ProjectManager::handleRecentProjectActionTriggered() {
    QAction* action = qobject_cast<QAction*> (sender());
    if (action) {
        // Check if this file exists:
        QFileInfo fi(action->objectName());
        if (!fi.exists()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Cannot Find Previous Project File"));
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(tr("The recent project you are trying to open does not exist."));
            msgBox.setInformativeText(QString(tr("%1<br><br>This project will be removed from your list of previous projects.")).arg(action->objectName()));
            msgBox.exec();
            removeRecentProject(action->objectName());
            refreshRecentProjects();
        } else {
            // The path to open is the object name:
            openProject(action->objectName());
        }
    }
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
    } else {
        // Check the required size of the recent projects list.
        if (d->recent_project_stack.count() == d->recent_projects_size) {
            d->recent_project_names.remove(d->recent_project_stack.last());
            d->recent_project_stack.removeLast();
        }

        // Now add the new project.
        d->recent_project_names[FileUtils::toNativeSeparators(project->projectFile())] = project->projectName();
        d->recent_project_stack.push_front(project->projectFile());
    }

    writeSettings();

    emit recentProjectsChanged(recentProjectNames(),recentProjectPaths());
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

void ProjectManagement::ProjectManager::clearCustomProjectsPaths() {
    foreach (const QString& key, d->custom_projects_paths.keys()) {
        if (key != "Default")
            removeCustomProjectsPath(key);
    }
}

