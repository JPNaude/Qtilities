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

struct Qtilities::Plugins::ProjectManagement::ProjectManagementPluginData {
    ProjectManagementPluginData() : actionProjectNew(0),
    actionProjectOpen(0),
    actionProjectClose(0),
    actionProjectSave(0),
    actionProjectSaveAs(0),
    appended_project_name(QString()) {}

    QAction*        actionProjectNew;
    QAction*        actionProjectOpen;
    QAction*        actionProjectClose;
    QAction*        actionProjectSave;
    QAction*        actionProjectSaveAs;
    QString         appended_project_name;
};

Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::ProjectManagementPlugin(QObject* parent) : IPlugin(parent)
{
    d = new ProjectManagementPluginData;
    setObjectName("Project Management Plugin");
    connect(PROJECT_MANAGER,SIGNAL(modificationStateChanged(bool)),SLOT(handle_projectStateChanged()));
}

Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::~ProjectManagementPlugin()
{
    delete d;
}

bool Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::initialize(const QStringList &arguments, QString *errorString) {
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    // Create and add project management actions:
    // Add project menu items
    bool existed;
    ActionContainer* file_menu = ACTION_MANAGER->createMenu(MENU_FILE,existed);
    // We construct each action and then register it
    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_STANDARD));

    // ---------------------------
    // New Project
    // ---------------------------
    d->actionProjectNew = new QAction(QIcon(),tr("New Project"),this);
    d->actionProjectNew->setShortcut(QKeySequence(tr("Alt+N")));
    connect(d->actionProjectNew,SIGNAL(triggered()),SLOT(handle_actionProjectNew()));
    Command* command = ACTION_MANAGER->registerAction(MENU_PROJECTS_NEW,d->actionProjectNew,context);
    file_menu->addAction(command,MENU_FILE_SETTINGS);
    // ---------------------------
    // Open Project
    // ---------------------------
    d->actionProjectOpen = new QAction(QIcon(),tr("Open Project"),this);
    d->actionProjectOpen->setShortcut(QKeySequence(tr("Alt+O")));
    connect(d->actionProjectOpen,SIGNAL(triggered()),SLOT(handle_actionProjectOpen()));
    command = ACTION_MANAGER->registerAction(MENU_PROJECTS_OPEN,d->actionProjectOpen,context);
    file_menu->addAction(command,MENU_FILE_SETTINGS);
    // ---------------------------
    // Close Project
    // ---------------------------
    d->actionProjectClose = new QAction(QIcon(),tr("Close Project"),this);
    d->actionProjectClose->setEnabled(false);
    d->actionProjectClose->setShortcut(QKeySequence(tr("Alt+C")));
    connect(d->actionProjectClose,SIGNAL(triggered()),SLOT(handle_actionProjectClose()));
    command = ACTION_MANAGER->registerAction(MENU_PROJECTS_CLOSE,d->actionProjectClose,context);
    file_menu->addAction(command,MENU_FILE_SETTINGS);
    // ---------------------------
    // Save Project
    // ---------------------------
    d->actionProjectSave = new QAction(QIcon(),tr("Save Project"),this);
    d->actionProjectSave->setEnabled(false);
    d->actionProjectSave->setShortcut(QKeySequence(tr("Alt+S")));
    connect(d->actionProjectSave,SIGNAL(triggered()),SLOT(handle_actionProjectSave()));
    command = ACTION_MANAGER->registerAction(MENU_PROJECTS_SAVE,d->actionProjectSave,context);
    file_menu->addAction(command,MENU_FILE_SETTINGS);
    // ---------------------------
    // Save Project As
    // ---------------------------
    d->actionProjectSaveAs = new QAction(QIcon(),tr("Save Project As"),this);
    d->actionProjectSaveAs->setEnabled(false);
    connect(d->actionProjectSaveAs,SIGNAL(triggered()),SLOT(handle_actionProjectSaveAs()));
    command = ACTION_MANAGER->registerAction(MENU_PROJECTS_SAVE_AS,d->actionProjectSaveAs,context);
    file_menu->addAction(command,MENU_FILE_SETTINGS);
    file_menu->addSeperator(MENU_FILE_SETTINGS);

    // Register project management config page.
    OBJECT_MANAGER->registerObject(PROJECT_MANAGER->configWidget());

    return true;
}

bool Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::initializeDependancies(QString *errorString) {
    Q_UNUSED(errorString)

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
    PROJECT_MANAGER->setProjectItemList(projectItems);

    return success;
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::finalize() {

}

double Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginVersion() {
    return (QString("%1.%2").arg(PROJECT_MANAGEMENT_PLUGIN_VERSION_MAJOR).arg(PROJECT_MANAGEMENT_PLUGIN_VERSION_MINOR)).toDouble();
}

QStringList Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginCompatibilityVersions() {
    QStringList compatible_versions;
    compatible_versions << QtilitiesCoreApplication::qtilitiesVersion();
    return compatible_versions;
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginPublisher() {
    return "Jaco Naude";
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginPublisherWebsite() {
    return "http://www.qtilities.org";
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginPublisherContact() {
    return "support@qtilities.org";
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginDescription() {
    return tr("A plugin which adds project management capabilities to the application.");
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginCopyright() {
    return QString(tr("Copyright") + " 2010, Jaco Naude");
}

QString Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::pluginLicense()  {
    return tr("See the Qtilities Libraries license");
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_actionProjectNew() {
    PROJECT_MANAGER->newProject();
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_actionProjectOpen() {
    QString filter = QString(tr("Project File (*%1)")).arg(FILE_EXT_PROJECT);
    QString project_path;
    if (PROJECT_MANAGER->useCustomProjectsPath())
        project_path = PROJECT_MANAGER->customProjectsPath();
    else
        project_path = QCoreApplication::applicationDirPath() + "/Projects";
    QString file_name = QFileDialog::getOpenFileName(0, tr("Open Existing Project"), project_path, filter);
    if (file_name.isEmpty())
        return;

    if (file_name.endsWith(QString("%1").arg(FILE_EXT_PROJECT))) {
        PROJECT_MANAGER->openProject(file_name);
    }
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

    QString filter = QString(tr("Project File (*%1)")).arg(FILE_EXT_PROJECT);
    QString project_path;
    if (PROJECT_MANAGER->useCustomProjectsPath())
        project_path = PROJECT_MANAGER->customProjectsPath();
    else
        project_path = QCoreApplication::applicationDirPath() + "/Projects";
    QString file_name = QFileDialog::getSaveFileName(0, tr("Save Project"),project_path, filter);
    if (file_name.isEmpty())
        return;

    if (file_name.endsWith(QString("%1").arg(FILE_EXT_PROJECT))) {
        PROJECT_MANAGER->saveProject(file_name);
    }
}

void Qtilities::Plugins::ProjectManagement::ProjectManagementPlugin::handle_projectStateChanged() {
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
                QString new_title = main_window->windowTitle();
                new_title.append(QString(" - %1").arg(project->projectName()));
                main_window->setWindowTitle(new_title);
                d->appended_project_name = project->projectName();
            } else {
                if (d->appended_project_name != project->projectName()) {
                    // The name of a previous project was appended.
                    QString new_title = main_window->windowTitle();
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
