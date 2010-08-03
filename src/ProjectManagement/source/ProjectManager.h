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

#ifndef PROJECTMANAGERGLOBAL_H
#define PROJECTMANAGERGLOBAL_H

#include <QObject>
#include <QStringList>

#include <IModificationNotifier.h>

#include "ProjectManagement_global.h"
#include "ProjectManagementConstants.h"
#include "IProjectItem.h"
#include "IProject.h"

namespace Qtilities {
    namespace ProjectManagement {
        using namespace Qtilities::ProjectManagement::Constants;
        using namespace Qtilities::ProjectManagement::Interfaces;
        using namespace Qtilities::Core::Interfaces;

        /*!
        \struct ProjectManagerData
        \brief The struct used to store private data in the ProjectManager class.
          */
        struct ProjectManagerData;

        /*!
        \class ProjectManager
        \brief The ProjectManager class

        The ProjectManager class is a singleton which provides access to the project manager instance. The project manager can manage one "current" project at any time.

        For more information see the \ref working_with_projects section of the \ref page_project_management article.
          */
        class PROJECT_MANAGEMENT_SHARED_EXPORT ProjectManager : public QObject, public IModificationNotifier
        {
            Q_OBJECT
            Q_ENUMS(ModifiedProjectsHandlingPolicy)
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
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
            // Functions related to the active project
            // ---------------------------------------
            //! Creates a new project.
            /*!
              If no project is open when this function is called a new project will be created which does not have
              a project file associated with it. When the new project is saved, it will automatically prompt for
              a file to which the project must be saved.

              Alternatively if a project is already open when this function is called, the function will call closeProject()
              on the current project and then create a new project as if no project was opened.
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
              */
            bool saveProject(QString file_name = QString());
            //! Returns the name of the current project.
            QString currentProjectName() const;
            //! Returns the file name of the current project.
            QString currentProjectFileName() const;
            //! Returns an interface to the current project.
            IProject* currentProject() const;

            // ----------------------------------------------
            // Functions related to the project manager state
            // ----------------------------------------------
            //! Returns a list of recent project names.
            QStringList recentProjectNames();
            //! Returns a list of recent project paths.
            QStringList recentProjectPaths();
            //! Returns the file for a recent project.
            QString recentProjectPath(const QString& recent_project_name);
            //! Clears the list of recent projects.
            void clearRecentProjects();
            //! Set project item list.
            void setProjectItemList(QList<IProjectItem*> item_list);

            // ----------------------------------------------
            // Functions related to the project file version
            // ----------------------------------------------
            //! This function sets the version of the project files generated by the project manager.
            /*!
              It is important to set the version of the project files generated whenever you change
              the structure of the exported file. Since the project files are binary the project
              manager must know exactly what the project file is going to look like.

              Each project file starts with the a quint32 value representing the Qtilities Libraries export version
              followed by a quint32 Project File version which is set using this function.

              When you attempt to open a project which was saved in a different version of the
              Qtilities Libraries (in which the export version was different) or a different project
              file version the open attempt will fail.
              */
            void setProjectFileVersion(quint32 project_file_version);
            //! Gets the project file version.
            quint32 projectFileVersion() const;
            //QList<quint32> supportedProjectFileVersions() const;

            // ----------------------------------------------
            // Functions related to the project manager configuration
            // ----------------------------------------------
            //! Returns a widget with project management options.
            QWidget* configWidget();
            //! Sets the configuration option to open the last project from the previous session on application startup.
            void setOpenLastProjectOnStartup(bool toggle);
            //! Gets the configuration option to open the last project from the previous session on application startup.
            bool openLastProjectOnStartup() const;
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
            //! Sets a custom projects path to use for saving projects.
            void setCustomProjectsPath(const QString& projects_path);
            //! Returns the custom projects path.
            QString customProjectsPath() const;
            //! Indicates if a custom projects path is used.
            bool useCustomProjectsPath() const;
            //! Toggles if a custom projects path is used.
            void setUseCustomProjectsPath(bool toggle);
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
            void writeSettings() const;
            //! Loads the project manager settings using QSettings.
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

            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }

            // --------------------------------
            // IModificationNotifier Implemenation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, bool notify_listeners = true, bool notify_subjects = false);
        signals:
            void modificationStateChanged(bool is_modified) const;
            void partialStateChanged(const QString& part_name) const;

        private:
            //! Add a project to the recent project list.
            void addRecentProject(IProject* project);

            ProjectManager(QObject* parent = 0);
            static ProjectManager* m_Instance;
            ProjectManagerData* d;
        };
    }
}

#define PROJECT_MANAGER Qtilities::ProjectManagement::ProjectManager::instance()
#define PROJECT_MANAGER_INITIALIZE() PROJECT_MANAGER->initialize();
#define PROJECT_MANAGER_FINALIZE() PROJECT_MANAGER->finalize();

#endif // PROJECTMANAGERGLOBAL_H
