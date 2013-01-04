/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#include "QtilitiesApplication.h"
#include "QtilitiesApplication_p.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ActionManager.h"
#include "ClipboardManager.h"
#include "AboutWindow.h"
#include "TreeItem.h"
#include "TreeFileItem.h"
#include "TreeNode.h"
#include "TaskManagerGui.h"

#include <QtilitiesCoreApplication_p.h>
#include <Qtilities.h>
#include <Factory>

#include <QMutex>
#include <QMessageBox>
#include <QDesktopServices>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui::Constants;

Qtilities::CoreGui::QtilitiesApplication* Qtilities::CoreGui::QtilitiesApplication::m_Instance = 0;

Qtilities::CoreGui::QtilitiesApplication::QtilitiesApplication(int &argc, char ** argv) : QApplication(argc, argv) {
    if (!m_Instance) {
        m_Instance = this;

        // Register the naming policy filter in the object manager:
        FactoryItemID naming_policy_filter(qti_def_FACTORY_TAG_NAMING_FILTER,QtilitiesCategory(tr("Subject Filters")));
        QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&NamingPolicyFilter::factory,naming_policy_filter);
        // Register the tree item in the object manager:
        FactoryItemID tree_item_tag(qti_def_FACTORY_TAG_TREE_ITEM,QtilitiesCategory(tr("Tree Building Blocks")));
        QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&TreeItem::factory,tree_item_tag);
        // Register the tree node in the object manager:
        FactoryItemID tree_node_tag(qti_def_FACTORY_TAG_TREE_NODE,QtilitiesCategory(tr("Tree Building Blocks")));
        QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&TreeNode::factory,tree_node_tag);
        // Register the tree file item in the object manager:
        FactoryItemID tree_file_item_tag(qti_def_FACTORY_TAG_TREE_FILE_ITEM,QtilitiesCategory(tr("Tree Building Blocks")));
        QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&TreeFileItem::factory,tree_file_item_tag);

        // Register QList<QPointer<QObject> > in Meta Object System.
        qRegisterMetaType<QList<QPointer<QObject> > >("QList<QPointer<QObject> >");

        connect(OBJECT_MANAGER,SIGNAL(newObjectAdded(QObject*)),TaskManagerGui::instance(),SLOT(handleObjectPoolAddition(QObject*)));

        // Organization name not set here yet, thus we can't do this:
        // applicationSessionPath();
    } else {
        qWarning() << QString(tr("An instance was already created for QtilitiesApplication"));
    }
}

Qtilities::CoreGui::QtilitiesApplication::~QtilitiesApplication() {

}

Qtilities::Core::Interfaces::IObjectManager* Qtilities::CoreGui::QtilitiesApplication::objectManager() {
     return QtilitiesCoreApplicationPrivate::instance()->objectManager();
}

Qtilities::Core::Interfaces::IContextManager* Qtilities::CoreGui::QtilitiesApplication::contextManager() {
    return QtilitiesCoreApplicationPrivate::instance()->contextManager();
}

#ifndef QTILITIES_NO_HELP
Qtilities::CoreGui::HelpManager* Qtilities::CoreGui::QtilitiesApplication::helpManager() {
    return QtilitiesApplicationPrivate::instance()->helpManager();
}
#endif

Qtilities::CoreGui::Interfaces::IActionManager* Qtilities::CoreGui::QtilitiesApplication::actionManager() {
    return QtilitiesApplicationPrivate::instance()->actionManager();
}

Qtilities::CoreGui::Interfaces::IClipboard* Qtilities::CoreGui::QtilitiesApplication::clipboardManager() {
    return QtilitiesApplicationPrivate::instance()->clipboardManager();
}

void Qtilities::CoreGui::QtilitiesApplication::setMainWindow(QWidget* mainWindow) {
    QtilitiesApplicationPrivate::instance()->setMainWindow(mainWindow);
}

QWidget* Qtilities::CoreGui::QtilitiesApplication::mainWindow() {
    return QtilitiesApplicationPrivate::instance()->mainWindow();
}

QString Qtilities::CoreGui::QtilitiesApplication::qtilitiesVersionString() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersionString();
}

Qtilities::Core::VersionNumber Qtilities::CoreGui::QtilitiesApplication::qtilitiesVersion() {
    return QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersion();
}

void Qtilities::CoreGui::QtilitiesApplication::setConfigWidget(QWidget* configWidget) {
    QtilitiesApplicationPrivate::instance()->setConfigWidget(configWidget);
}

QWidget* Qtilities::CoreGui::QtilitiesApplication::configWidget() {
    return QtilitiesApplicationPrivate::instance()->configWidget();
}

void Qtilities::CoreGui::QtilitiesApplication::initialize() {
    // Make a call to application session path which will set the default path in QtilitiesCoreApplication to
    // use QDesktopServices. This might be different in Qt 5 with the new QStandardPaths class.
    // Note that we cannot do this in QtilitiesApplication's constructor since the application name and
    // organization has not been set at that stage. This call is also in QtilitiesMainWindow's constructor.
    QtilitiesApplication::applicationSessionPath();

    // Register the naming policy filter in the object manager:
    FactoryItemID naming_policy_filter(qti_def_FACTORY_TAG_NAMING_FILTER,QtilitiesCategory(tr("Subject Filters")));
    QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&NamingPolicyFilter::factory,naming_policy_filter);
    // Register the naming policy filter in the object manager:
    FactoryItemID tree_item_tag(qti_def_FACTORY_TAG_TREE_ITEM,QtilitiesCategory(tr("Tree Building Blocks")));
    QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&TreeItem::factory,tree_item_tag);
    // Register the naming policy filter in the object manager:
    FactoryItemID tree_node_tag(qti_def_FACTORY_TAG_TREE_NODE,QtilitiesCategory(tr("Tree Building Blocks")));
    QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&TreeNode::factory,tree_node_tag);
    // Register the tree file item in the object manager:
    FactoryItemID tree_file_item_tag(qti_def_FACTORY_TAG_TREE_FILE_ITEM,QtilitiesCategory(tr("Tree Building Blocks")));
    QtilitiesCoreApplicationPrivate::instance()->objectManager()->registerFactoryInterface(&TreeFileItem::factory,tree_file_item_tag);

    // Register QList<QPointer<QObject> > in Meta Object System.
    qRegisterMetaType<QList<QPointer<QObject> > >("QList<QPointer<QObject> >");

    connect(OBJECT_MANAGER,SIGNAL(newObjectAdded(QObject*)),TaskManagerGui::instance(),SLOT(handleObjectPoolAddition(QObject*)));
}

Qtilities::CoreGui::QtilitiesApplication* Qtilities::CoreGui::QtilitiesApplication::instance(bool silent) {
    if (!QtilitiesApplication::hasInstance("instance",silent))
        return 0;
    else
        return m_Instance;
}

bool Qtilities::CoreGui::QtilitiesApplication::hasInstance(const char *function, bool silent) {
    bool instance_exists = (QtilitiesApplication::m_Instance != 0);
    if (!instance_exists && !silent)
        qWarning("QtilitiesApplication::%s: Please instantiate the QtilitiesApplication object before attempting to use it.", function);
    return instance_exists;
}

QWidget* Qtilities::CoreGui::QtilitiesApplication::aboutQtilities(bool show) {
    qti_private_AboutWindow* about_window = new qti_private_AboutWindow();
    if (show)
        about_window->show();
    return about_window;
}

QString Qtilities::CoreGui::QtilitiesApplication::applicationSessionPath() {
    if (QApplication::organizationName().isEmpty() || QApplication::applicationName().isEmpty()) {
        qDebug() << Q_FUNC_INFO << "Organization name and application name must be set in order to properly use QtilitiesApplication::applicationSessionPath().";
    } else {
        // If it is still the default from QtilitieCoreApplicationPrivate, we set it to use AppData:
        if (FileUtils::comparePaths(QtilitiesCoreApplicationPrivate::instance()->applicationSessionPath(),QtilitiesCoreApplicationPrivate::instance()->applicationSessionPathDefault()))
            QtilitiesCoreApplicationPrivate::instance()->setApplicationSessionPath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    }

    return QtilitiesCoreApplicationPrivate::instance()->applicationSessionPath();
}

void Qtilities::CoreGui::QtilitiesApplication::setApplicationSessionPath(const QString& path) {
    Log->setLoggerSessionConfigPath(path);
    QtilitiesCoreApplicationPrivate::instance()->setApplicationSessionPath(path);
}

void Qtilities::CoreGui::QtilitiesApplication::setApplicationExportVersion(quint32 application_export_version) {
    QtilitiesCoreApplicationPrivate::instance()->setApplicationExportVersion(application_export_version);
}

quint32 Qtilities::CoreGui::QtilitiesApplication::applicationExportVersion() {
    return QtilitiesCoreApplicationPrivate::instance()->applicationExportVersion();
}

void Qtilities::CoreGui::QtilitiesApplication::setApplicationBusy(bool is_busy) {
    QtilitiesCoreApplicationPrivate::instance()->setApplicationBusy(is_busy);
}

bool Qtilities::CoreGui::QtilitiesApplication::applicationBusy() {
    return QtilitiesCoreApplicationPrivate::instance()->applicationBusy();
}

