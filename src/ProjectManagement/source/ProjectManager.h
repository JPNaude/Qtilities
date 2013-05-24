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

#ifndef PROJECTMANAGERGLOBAL_H
#define PROJECTMANAGERGLOBAL_H

#include "ProjectManagement_global.h"
#include "ProjectManagementConstants.h"
#include "IProjectItem.h"
#include "IProject.h"

#include <QObject>
#include <QStringList>
#include <QAction>

#include <IModificationNotifier>
#include <ITaskContainer>
#include <ActionContainer>

using namespace Qtilities::ProjectManagement::Constants;
using namespace Qtilities::ProjectManagement::Interfaces;
using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::CoreGui;

namespace Qtilities {
    namespace ProjectManagement {
        /*!
        \struct ProjectManagerPrivateData
        \brief The struct used to store private data in the ProjectManager class.
          */
        struct ProjectManagerPrivateData;

        /*!
        \class ProjectManager
        \brief The ProjectManager class

        The ProjectManager class is a singleton which provides access to the project manager instance. The project manager can manage one "current" project at any time.

        For more information see the \ref working_with_projects section of the \ref page_project_management article.
          */
        class PROJECT_MANAGEMENT_SHARED_EXPORT ProjectManager : public QObject, public IModificationNotifier, public ITaskContainer
        {
            Q_OBJECT
            Q_ENUMS(ModifiedProjectsHandlingPolicy)
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
            //! Access names for tasks provided by ProjectManager.
            enum ContainedTasks {
                TaskSaveProject   = 0,  /*!< Save project task with progress information for saveProject(). Enabled by default. */
                TaskOpenProject   = 1,  /*!< Open project task with progress information for openProject(). Enabled by deafult. */
                TaskCloseProject   = 2  /*!< Close project task with progress information for closeProject(). Enabled by deafult. */
            };
            //! ContainedTasks to string conversion function.
            QString taskNameToString(ContainedTasks task_name) const {
                if (task_name == TaskSaveProject)
                    return "Saving Project";
                if (task_name == TaskOpenProject)
                    return "Opening Project";
                if (task_name == TaskCloseProject)
                    return "Closing Project";
                return "";
            }

            //! Defines the execution mode of the project manager.
            enum ExecStyle {
                ExecNormal   = 0,       /*!< Normal operation, prompt user when feedback or input action is required. */
                ExecSilent   = 1        /*!< Silent, do not prompt user when feedback or input action is required. Operate on defaults and if no defaults exists, fails. */
            };

            static ProjectManager* instance();
            ~ProjectManager();

            //! The possible ways to handle unsaved open projects when the application closes.
            /*!
              \sa checkModifiedOpenProjects()
              */
            enum ModifiedProjectsHandlingPolicy {
                PromptUser = 0,     /*!< All observer operations are available to the user (Attachment, Detachement etc.). */
                AutoSave   = 1      /*!< The observer is read only to the user. */
            };

            // ---------------------------------------
            // Functions related to project types
            // ---------------------------------------
            //! Provides the allowed project types that can be used by the project manager to save projects.
            /*!
              When prompting to save projects, the allowed project types will be used as the possible file types that the user see. You can define the suffix used for a specific project type in your application using setProjectTypeSuffix().

              By default all available project types are allowed.

              \sa setAllowedProjectTypes()
              */
            IExportable::ExportModeFlags allowedProjectTypes() const;
            //! Sets the allowed project types that can be used by the project manager to save projects.
            /*!
              This functionality is usefull if you want to force the users of your application to use
              a specific format.

              \note Make sure to call this function before creating your ProjectManagementConfig page. If you are using the %Qtilities Project Management plugin, make sure you call this function before loading your plugins.

              \sa allowedProjectTypes()
              */
            void setAllowedProjectTypes(IExportable::ExportModeFlags project_types);
            //! Set the suffix used for a specific project type.
            /*!
              \param project_type The project type set the suffix for.
              \param suffix The suffix to use, for example "prj"

              \sa projectTypeSuffix()
              */
            void setProjectTypeSuffix(IExportable::ExportMode project_type, const QString& suffix);
            //! Get the suffix used for a specific project type.
            /*!
              The file naming conventions used by default are defined by the following constants:
              - Binary: qti_def_SUFFIX_PROJECT_BINARY
              - XML:    qti_def_SUFFIX_PROJECT_XML

              \sa setProjectTypeSuffix()
              */
            QString projectTypeSuffix(IExportable::ExportMode project_type);
            //! The default project type.
            /*!
              This default project type is used when the user saves a project without specifying a project file extension.

              \sa setDefaultProjectType()
              */
            IExportable::ExportMode defaultProjectType() const;
            //! Sets default project type.
            /*!
              If \p default_project_type is not an allowed type, this function does nothing.

              \sa defaultProjectType(), allowedProjectTypes()
              */
            void setDefaultProjectType(IExportable::ExportMode default_project_type);
            //! Function which returns a string which can be used in file dialog as a filter with the current allowed project types.
            /*!
              \sa allowedProjectTypes(), projectTypeFromTypeFilter()
              */
            QString allowedProjectTypesFilter() const;
            //! Gets the project type from a project types filter string provided by allowedProjectTypesFilter().
            /*!
              \sa allowedProjectTypesFilter(), allowedProjectTypes()

              <i>This function was added in %Qtilities v1.2.</i>
              */
            IExportable::ExportMode projectTypeFromTypeFilter(const QString& project_type_filter_string) const;
            //! Function which checks if a file name's suffix is one of the allowed project types' suffices.
            bool isAllowedFileName(const QString& file_name) const;

            // ---------------------------------------
            // Functions related to the active project
            // ---------------------------------------
            //! Creates a new project.
            /*!
              If no project is open when this function is called a new project will be created which does not have a project file associated with it. When the new project is saved, it will automatically prompt for a file to which the project must be saved.

              Alternatively if a project is already open when this function is called, the function will call closeProject() on the current project and then create a new project as if no project was opened.
              */
            bool newProject();
            //! Open a project.
            /*!
              If no project is open when this function is called, the specified project will be opened. If
              the specified file name does not exist, this function does nothing.

              Alternatively if a project is already open when this function is called, the function will call closeProject()
              on the current project and then open the new project as if no project was opened.

              If the current project file passed as \p file_name this function does nothing.
              */
            bool openProject(const QString& file_name);
            //! Close the current project.
            /*!
            If the current project is modified when calling this function, the saveProject() function will be called
            before closing the current project.

            This function deletes the current project instance. Thus always use a QPointer when storing a reference to
            an active project returned by currentProject().
            */
            bool closeProject();
            //! Save the current project to the specified file.
            /*!
              \param file_name When empty the current open project will be saved to its current file. If there is not
              current project this function will do nothing. If there is a current project which has not been saved yet
              this function will also do nothing.
              \param respect_project_busy When true the project won't save when its busy. It is however to ignore that the project is busy and
              still save it by settings this parameter to false.
              */
            bool saveProject(QString file_name = QString(), bool respect_project_busy = true);
            //! Returns the name of the current project.
            QString currentProjectName() const;
            //! Returns the file name of the current project.
            QString currentProjectFileName() const;
            //! Returns an interface to the current project.
            IProject* currentProject() const;
            //! Refreshes the project part list by scanning the global object pool.
            void refreshPartList();
            //! Sets if the current open project is busy, thus it cannot be closed, saved etc.
            /*!
              This function uses a stacked approach, thus your setActiveProjectBusy(false) calls must match the number of setActiveProjectBusy(true) calls.

              For more information on this type of stacked approach, see Qtilities::Core::Observer::startProcessingCycle().

              When settings a project as busy, the application will automatically be set as busy as well. For more information see Qtilities::Core::QtilitiesCoreApplication::setApplicationBusy().
              */
            void setActiveProjectBusy(bool is_busy);
            //! Gets if the active project is busy, thus it cannot be closed, saved etc.
            bool activeProjectBusy() const;
            //! Set project item list.
            void setProjectItemList(QList<IProjectItem*> item_list);

            // ----------------------------------------------
            // Custom project paths
            // ----------------------------------------------
            //! Sets a custom projects path to use for saving projects.
            /*!
              \param projects_path The path for projects of type category.
              \param projects_category The category to associated with the given path.

              \note When the directory does not exist, this function will create it for you.
              */
            void setCustomProjectsPath(const QString& projects_path, QString projects_category = QString());
            //! Returns the custom projects path.
            QString customProjectsPath(QString projects_category = QString()) const;
            //! Removes the custom projects path specified by the given category.
            void removeCustomProjectsPath(QString projects_category);
            //! Clears all custom projects paths except for the Default one.
            void clearCustomProjectsPaths();
            //! Sets the default project paths category.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setDefaultCustomProjectsCategory(const QString& projects_category);
            //! Gets the default project paths category.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            QString defaultCustomProjectsCategory() const;
            //! Returns a list of all registered project categories.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            QStringList customProjectCategories() const;
            //! Indicates if a custom projects path is used.
            bool useCustomProjectsPath() const;
            //! Toggles if a custom projects path is used.
            void setUseCustomProjectsPath(bool toggle);

            //! Sets the target menu where project menu items should be placed.
            /*!
             * These menu items are not added automatically by the project manager. Its done through the ProjectManagementPlugin.
             * Alternatively they can be added manually using the access functions on ProjectManager.
             * \sa actionNewProject(), actionCloseProject(), actionOpenProject(), actionSaveAsProject(), actionSaveProject(), recentProjectsMenuContainer()
             *
             * The default menu is Qtilities::CoreGui::Actions::qti_action_FILE.
             *
             * \sa setProjectMenuItemsBeforeCommand()
             *
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void setProjectMenuItemsTargetMenu(const QString& menu_id);
            //! Gets the target menu where project menu items should be placed.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            QString projectMenuItemsTargetMenu() const;
            //! Sets the command before which the project menus must be placed in the menu specified by projectMenuItemsTargetMenu().
            /*!
             * These menu items are not added automatically by the project manager. Its done through the ProjectManagementPlugin.
             * Alternatively they can be added manually using the access functions on ProjectManager.
             * \sa actionNewProject(), actionCloseProject(), actionOpenProject(), actionSaveAsProject(), actionSaveProject(), recentProjectsMenuContainer()
             *
             * The default menu is Qtilities::CoreGui::Actions::qti_action_FILE_SETTINGS.
             *
             * \sa setProjectMenuItemsTargetMenu()
             *
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void setProjectMenuItemsBeforeCommand(const QString& command_id);
            //! Gets the command before which the project menus must be placed in the menu specified by projectMenuItemsTargetMenu().
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            QString projectMenuItemsBeforeCommand() const;

            // ----------------------------------------------
            // Functions related to the project manager configuration
            // ----------------------------------------------
            //! Returns a widget with project management options.
            QWidget* configWidget();
            //! Sets the configuration option to open the last project from the previous session on application startup.
            void setOpenLastProjectOnStartup(bool toggle);
            //! Gets the configuration option to open the last project from the previous session on application startup.
            bool openLastProjectOnStartup() const;
            //! Sets if project file locking is enabled.
            /*!
             *<i>This function was added in %Qtilities v1.2.</i>
             *
             *\sa useProjectFileLocks()
             */
            void setUseProjectFileLocks(bool toggle);
            //! Gets if project file locking is enabled.
            /*!
             *Default is true.
             *
             *<i>This function was added in %Qtilities v1.2.</i>
             *
             *\sa setUseProjectFileLocks()
             */
            bool useProjectFileLocks() const;
            //! Sets the configuration option to create a new project when the no last open project is available.
            /*!
              This configuration setting has no effect if the openLastProjectOnStartup() is false.
              \sa
                - setOpenLastProjectOnStartup()
                - openLastProjectOnStartup()
              */
            void setCreateNewProjectOnStartup(bool toggle);
            //! Gets the configuration option to open the last project from the previous session on application startup.
            bool createNewProjectOnStartup() const;
            //! Indicates if the project manager will check for modified open projects when the application exits.
            bool checkModifiedOpenProjects() const;
            //! Toggles if the project manager will check for modified open projects when the application exists.
            void setCheckModifiedOpenProjects(bool toggle);
            //! Indicates how the project manager will handle modified open projects when set to check for them.
            /*!
              \sa checkModifiedOpenProjects(), setCheckModifiedOpenProjects()
              */
            ModifiedProjectsHandlingPolicy modifiedProjectsHandlingPolicy() const;
            //! Sets how the project manager should handle modified open projects when set to check for them.
            void setModifiedProjectsHandlingPolicy(ModifiedProjectsHandlingPolicy handling_policy);
            //! Saves the project manager settings using QSettings.
            /*!
              For more information about the saving of settings by %Qtilities classes, see \ref configuration_widget_storage_layout.
              */
            void writeSettings() const;
            //! Loads the project manager settings using QSettings.
            /*!
              For more information about the saving of settings by %Qtilities classes, see \ref configuration_widget_storage_layout.
              */
            void readSettings();
            //! Function which initializes the project manager on application startup.
            /*!
              The initialize function check the openLastProjectOnStartup() and createNewProjectOnStartup() settings
              and load or create the needed project on startup.

              \sa PROJECT_MANAGER_INITIALIZE
              */
            void initialize();
            //! Function which finalizes the project manager on application exit.
            /*!
              This function will save modified open projects if needed.

              \sa PROJECT_MANAGER_FINALIZE
              */
            void finalize();
            //! Sets that changes happened during the loading of the project. When loading is complete this information can be used by project item wrappers.
            /*!
              \note The project manager will reset this mark immediately after the projectLoadingFinished() signal was emitted.

              \sa projectChangedDuringLoad()
              */
            void markProjectAsChangedDuringLoad(bool changed = true);
            //! Gets if changes happened during the loading of the project. When loading is complete this information can be used by project item wrappers.
            /*!
              \sa markProjectAsChangedDuringLoad()
              */
            bool projectChangedDuringLoad();

            //! Gets the execution style of the project manager.
            /*!
              Default is ExecNormal.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            ExecStyle executionStyle() const;
            //! Sets the execution style of the project manager.
            /*!
              Default is ExecNormal.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setExecutionStyle(ExecStyle exec_style);

            //! Disables/enables the ability to save projects.
            /*!
              Saving of projects are disabled by default.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void toggleProjectSaving(bool is_enabled, const QString& info_message = QString());
            //! Gets if project saving is enabled.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool projectSavingEnabled() const;
            //! Gets the info message about why project saving was disabled (if so).
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            QString projectSavingInfoMessage() const;

            // ---------------------------------------
            // Ready to use actions for common project management tasks.
            // ---------------------------------------
            //! Convenience function which returns an action which can be placed in a menu to allow switching back to the previous mode.
            /*!
             * This action is registered in the action manager under the following id: Qtilities::CoreGui::Constants::Actions::qti_action_PROJECTS_NEW.
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            QAction* actionNewProject();
            //! Convenience function which returns an action which can be placed in a menu to allow switching back to the previous mode.
            /*!
             * This action is registered in the action manager under the following id: Qtilities::CoreGui::Constants::Actions::qti_action_PROJECTS_OPEN.
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            QAction* actionOpenProject();
            //! Convenience function which returns an action which can be placed in a menu to allow switching back to the previous mode.
            /*!
             * This action is registered in the action manager under the following id: Qtilities::CoreGui::Constants::Actions::qti_action_PROJECTS_CLOSE.
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            QAction* actionCloseProject();
            //! Convenience function which returns an action which can be placed in a menu to allow switching back to the previous mode.
            /*!
             * This action is registered in the action manager under the following id: Qtilities::CoreGui::Constants::Actions::qti_action_PROJECTS_SAVE.
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            QAction* actionSaveProject();
            //! Convenience function which returns an action which can be placed in a menu to allow switching back to the previous mode.
            /*!
             * This action is registered in the action manager under the following id: Qtilities::CoreGui::Constants::Actions::qti_action_PROJECTS_SAVE_AS.
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            QAction* actionSaveAsProject();

            // ----------------------------------------------
            // Recent Projects
            // ----------------------------------------------
            //! Returns a list of recent project names.
            QStringList recentProjectNames() const;
            //! Returns a list of recent project paths.
            QStringList recentProjectPaths() const;
            //! Returns the file for a recent project.
            QString recentProjectPath(const QString& recent_project_name) const;
            //! Returns a list of recent project paths.
            QStringList registeredProjectItemNames() const;
            //! Convenience function which returns an action which can be placed in a menu to allow switching back to the previous mode.
            /*!
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            ActionContainer* recentProjectsMenuContainer();
            //! Removes a recent project path from the list of recent projects.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void removeRecentProject(const QString& path);

        public slots:
            //! Clears the list of recent projects.
            void clearRecentProjects();
            //! Removes all recent project paths which does not exist and return the files that were removed.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            QStringList removeNonExistingRecentProjects();

        private slots:
            void handle_actionProjectNew();
            void handle_actionProjectOpen();
            void handle_actionProjectClose();
            void handle_actionProjectSave();
            void handle_actionProjectSaveAs();
            void handle_projectStateChanged();
            void handleApplicationBusyStateChanged();
            void refreshRecentProjects();
            void handleRecentProjectActionTriggered();

        public:
            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

            //! A signal which is emitted when the active project changed.
            /*!
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void currentProjectChanged(IProject* project);
            //! A signal which is emitted when a project loading/opening process starts.
            /*!
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void projectCreationStarted();
            //! A signal which is emitted when a project loading/opening process completes.
            /*!
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void projectCreationFinished();
            //! A signal which is emitted when a project loading/opening process starts.
            /*!
              \param project_file The project file from which loading is done.
              */
            void projectLoadingStarted(const QString& project_file);
            //! A signal which is emitted when a project loading/opening process completes.
            /*!
              \param project_file The project file from which loading was done.
              \param success If the project was successfully loaded, success will be true. False otherwise.
              */
            void projectLoadingFinished(const QString& project_file, bool success);
            //! A signal which is emitted when a project save process starts.
            /*!
              \param project_file The project file to which the project is saved.
              */
            void projectSavingStarted(const QString& project_file);
            //! A signal which is emitted when a project save process completes.
            /*!
              \param project_file The project file to which the project is saved.
              \param success If the project was successfully saved, success will be true. False otherwise.
              */
            void projectSavingFinished(const QString& project_file, bool success);
            //! A signal which is emitted when a project closing process starts.
            /*!
              \param project_file The current project file which is being closed.
              */
            void projectClosingStarted(const QString& project_file);
            //! A signal which is emitted when a project closing process completes.
            /*!
              \param project_file The project file from which loading was done.
              \param success If the project was successfully closed, success will be true. False otherwise.
              */
            void projectClosingFinished(bool success);
            //! Signal which is emitted whenever the recent projects list changes.
            /*!
                This function was added in Qtilities v1.1.
              */
            void recentProjectsChanged(const QStringList& names, const QStringList& paths);
            //! Signal which is emitted whenever the custom project paths are changed.
            /*!
                This function signal was added in Qtilities v1.1.
              */
            void customProjectPathsChanged();
            //! Signal which is emitted when a project save operation was requested while no project was open.
            /*!
                This function signal was added in Qtilities v1.2.
              */
            void projectSaveRequestedWithoutOpenProject();

        private:
            //! Add a project to the recent project list.
            void addRecentProject(IProject* project);         

            ProjectManager(QObject* parent = 0);
            static ProjectManager* m_Instance;
            ProjectManagerPrivateData* d;
        };
    }
}

#define PROJECT_MANAGER Qtilities::ProjectManagement::ProjectManager::instance()
#define PROJECT_MANAGER_INITIALIZE() PROJECT_MANAGER->initialize();
#define PROJECT_MANAGER_FINALIZE() PROJECT_MANAGER->finalize();

#endif // PROJECTMANAGERGLOBAL_H
