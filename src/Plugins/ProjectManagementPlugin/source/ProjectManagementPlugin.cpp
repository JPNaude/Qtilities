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

#include "ProjectManagementPlugin.h"
#include "ProjectManagementPluginConstants.h"

#include <Qtilities.h>
#include <QtilitiesApplication.h>
#include <ProjectManager.h>
#include <ActionContainer.h>
#include <Command.h>
#include <QtilitiesCoreConstants.h>
#include <QtilitiesCoreGuiConstants.h>
#include <Logger.h>

#include <QtPlugin>
#include <QIcon>
#include <QLabel>
#include <QFileDialog>

using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::ProjectManagement;
using namespace Qtilities::Plugins::ProjectManagement::Constants;

struct Qtilities::Plugins::ProjectManagement::ProjectManagementPluginPrivateData {
    ProjectManagementPluginPrivateData() : actionProjectNew(0),
    actionProjectOpen(0),
    actionProjectClose(0),
    actionProjectSave(0),
    actionProjectSaveAs(0),
    appended_project_name(QString()),
    is_initialized(false) {}

    QAction*        actionProjectNew;
    QAction*        actionProjectOpen;
    QAction*        actionProjectClose;
    QAction*        actionProjectSave;
    QAction*        actionProjectSaveAs;
    QString         appended_project_name;
    bool            is_initialized;
};

Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::ProjectManagementPlugin(QObject* parent) : QObject(parent) {
    d = new ProjectManagementPluginPrivateData;
    setObjectName(pluginName());
    connect(PROJECT_MANAGER,SIGNAL(modificationStateChanged(bool)),SLOT(handle_projectStateChanged()));
}

Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::~ProjectManagementPlugin() {
    delete d;
}

bool Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::initialize(const QStringList &arguments, QString *errorString) {
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Create and add project management actions:
    // Add project menu items
    bool existed;
    ActionContainer* file_menu = ACTION_MANAGER->createMenu(qti_action_FILE,existed);
    // We construct each action and then register it
    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

    // ---------------------------
    // New Project
    // ---------------------------
    d->actionProjectNew = new QAction(QIcon(),tr("New Project"),this);
    d->actionProjectNew->setShortcut(QKeySequence(QKeySequence::New));
    connect(d->actionProjectNew,SIGNAL(triggered()),SLOT(handle_actionProjectNew()));
    Command* command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_NEW,d->actionProjectNew,context);
    command->setCategory(QtilitiesCategory("Projects"));
    file_menu->addAction(command,qti_action_FILE_SETTINGS);
    // ---------------------------
    // Open Project
    // ---------------------------
    d->actionProjectOpen = new QAction(QIcon(),tr("Open Project"),this);
    d->actionProjectOpen->setShortcut(QKeySequence(QKeySequence::Open));
    connect(d->actionProjectOpen,SIGNAL(triggered()),SLOT(handle_actionProjectOpen()));
    command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_OPEN,d->actionProjectOpen,context);
    command->setCategory(QtilitiesCategory("Projects"));
    file_menu->addAction(command,qti_action_FILE_SETTINGS);
    // ---------------------------
    // Close Project
    // ---------------------------
    d->actionProjectClose = new QAction(QIcon(),tr("Close Project"),this);
    d->actionProjectClose->setEnabled(false);
    connect(d->actionProjectClose,SIGNAL(triggered()),SLOT(handle_actionProjectClose()));
    command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_CLOSE,d->actionProjectClose,context);
    command->setCategory(QtilitiesCategory("Projects"));
    file_menu->addAction(command,qti_action_FILE_SETTINGS);
    // ---------------------------
    // Save Project
    // ---------------------------
    d->actionProjectSave = new QAction(QIcon(),tr("Save Project"),this);
    d->actionProjectSave->setEnabled(false);
    d->actionProjectSave->setShortcut(QKeySequence(QKeySequence::Save));
    connect(d->actionProjectSave,SIGNAL(triggered()),SLOT(handle_actionProjectSave()));
    command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_SAVE,d->actionProjectSave,context);
    command->setCategory(QtilitiesCategory("Projects"));
    file_menu->addAction(command,qti_action_FILE_SETTINGS);
    // ---------------------------
    // Save Project As
    // ---------------------------
    d->actionProjectSaveAs = new QAction(QIcon(),tr("Save Project As"),this);
    d->actionProjectSaveAs->setEnabled(false);
    d->actionProjectSaveAs->setShortcut(QKeySequence(QKeySequence::SaveAs));
    connect(d->actionProjectSaveAs,SIGNAL(triggered()),SLOT(handle_actionProjectSaveAs()));
    command = ACTION_MANAGER->registerAction(qti_action_PROJECTS_SAVE_AS,d->actionProjectSaveAs,context);
    command->setCategory(QtilitiesCategory("Projects"));
    file_menu->addAction(command,qti_action_FILE_SETTINGS);
    file_menu->addSeperator(qti_action_FILE_SETTINGS);

    // Register project management config page.
    OBJECT_MANAGER->registerObject(PROJECT_MANAGER->configWidget(),QtilitiesCategory("GUI::Configuration Pages (IConfigPage)","::"));

    d->is_initialized = true;
    return true;
}

bool Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::initializeDependancies(QString *errorString) {
    Q_UNUSED(errorString)
    return true;
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::finalize() {

}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginName() const {
    return "Project Management Plugin";
}

QtilitiesCategory Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginCategory() const {
    return QtilitiesCategory(tr("General"));
}

Qtilities::Core::VersionInformation Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginVersionInformation() const {
    VersionInformation version_info(qti_def_VERSION_MAJOR,qti_def_VERSION_MINOR,qti_def_VERSION_REVISION);
    return version_info;
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginPublisher() const {
    return "Jaco Naude";
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginPublisherWebsite() const {
    return "http://www.qtilities.org";
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginPublisherContact() const {
    return "support@qtilities.org";
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginDescription() const {
    return tr("A plugin which adds project management capabilities to the application.");
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginCopyright() const {
    return QString(tr("Copyright") + " 2010-2011, Jaco Naude");
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginLicense() const  {
    return tr("See the Qtilities Libraries license");
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_actionProjectNew() {
    PROJECT_MANAGER->newProject();
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_actionProjectOpen() {
    QString filter = PROJECT_MANAGER->allowedProjectTypesFilter();
    QString project_path;
    if (PROJECT_MANAGER->useCustomProjectsPath())
        project_path = PROJECT_MANAGER->customProjectsPath();
    else
        project_path = QCoreApplication::applicationDirPath() + "/Projects";
    QString file_name = QFileDialog::getOpenFileName(0, tr("Open Existing Project"), project_path, filter);
    if (file_name.isEmpty())
        return;

    if (!PROJECT_MANAGER->isAllowedFileName(file_name))
        file_name.append("." + PROJECT_MANAGER->projectTypeSuffix(PROJECT_MANAGER->defaultProjectType()));

    PROJECT_MANAGER->openProject(file_name);
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_actionProjectClose() {
    PROJECT_MANAGER->closeProject();
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_actionProjectSave() {
    if (!PROJECT_MANAGER->currentProject())
        return;

    if (PROJECT_MANAGER->currentProject()->projectFile().isEmpty()) {
        return handle_actionProjectSaveAs();
    } else {
        PROJECT_MANAGER->saveProject(PROJECT_MANAGER->currentProject()->projectFile());
    }
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_actionProjectSaveAs() {
    if (!PROJECT_MANAGER->currentProject())
        return;

    QString filter = PROJECT_MANAGER->allowedProjectTypesFilter();
    QString project_path;
    if (PROJECT_MANAGER->useCustomProjectsPath())
        project_path = PROJECT_MANAGER->customProjectsPath();
    else
        project_path = QCoreApplication::applicationDirPath() + "/Projects";
    QString file_name = QFileDialog::getSaveFileName(0, tr("Save Project"),project_path, filter);
    if (file_name.isEmpty())
        return;

    if (!PROJECT_MANAGER->isAllowedFileName(file_name))
        file_name.append(PROJECT_MANAGER->projectTypeSuffix(PROJECT_MANAGER->defaultProjectType()));

    PROJECT_MANAGER->saveProject(file_name);
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_projectStateChanged() {
    if (!d->is_initialized)
        return;

    IProject* project = PROJECT_MANAGER->currentProject();

    if (project) {
        // Update actions:
        d->actionProjectClose->setEnabled(true);
        d->actionProjectNew->setEnabled(true);
        d->actionProjectOpen->setEnabled(true);
        d->actionProjectSaveAs->setEnabled(true);

        // Check if the project name currenlty in the main window must change:
        QWidget* main_window = QtilitiesApplication::mainWindow();
        if (main_window && project) {
            // Name was never appended before.
            if (d->appended_project_name.isEmpty()) {
                QString new_title = main_window->windowTitle().trimmed();
                new_title.append(QString(" - %1").arg(project->projectName()));
                main_window->setWindowTitle(new_title);
                d->appended_project_name = project->projectName();
            } else {
                if (d->appended_project_name != project->projectName()) {
                    // The name of a previous project was appended.
                    QString new_title = main_window->windowTitle().trimmed();
                    new_title.chop(d->appended_project_name.length() + 3);
                    new_title.append(QString(" - %1").arg(project->projectName()));
                    main_window->setWindowTitle(new_title);
                    d->appended_project_name = project->projectName();
                } else {
                    // The name of the current project is already appended.
                }
            }
        }

        // Add the * character if neccesarry:
        if (project->isModified()) {
            d->actionProjectSave->setEnabled(true);
            QWidget* main_window = QtilitiesApplication::mainWindow();
            if (main_window) {
                if (!main_window->windowTitle().endsWith("*"))
                    main_window->setWindowTitle(main_window->windowTitle().append("*"));
            }
        } else {
            d->actionProjectSave->setEnabled(false);
            QWidget* main_window = QtilitiesApplication::mainWindow();
            if (main_window) {
                if (main_window->windowTitle().endsWith("*")) {
                    QString new_title = main_window->windowTitle();
                    new_title.chop(1);
                    main_window->setWindowTitle(new_title);
                }
            }
        }
    } else {
        d->actionProjectClose->setEnabled(false);
        d->actionProjectNew->setEnabled(true);
        d->actionProjectOpen->setEnabled(true);
        d->actionProjectSave->setEnabled(false);
        d->actionProjectSaveAs->setEnabled(false);

        QWidget* main_window = QtilitiesApplication::mainWindow();
        if (main_window) {
            // Remove possible *:
            if (main_window->windowTitle().endsWith("*")) {
                QString new_title = main_window->windowTitle();
                new_title.chop(1);
                main_window->setWindowTitle(new_title);
            }

            // Remove possible last appended name:
            if (!d->appended_project_name.isEmpty()) {
                QString new_title = main_window->windowTitle();
                new_title.chop(d->appended_project_name.length() + 3);
                main_window->setWindowTitle(new_title);
                d->appended_project_name.clear();
            }
        }
    }
}

using namespace Qtilities::Plugins::ProjectManagement;
Q_EXPORT_PLUGIN2(ProjectManagementPlugin, ProjectManagementPlugin);
