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

#include "QtilitiesCoreGuiConstants.h"
#include "HelpManager.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include "QtilitiesCoreApplication_p.h"

#include <QHelpEngine>
#include <QHelpSearchEngine>

struct Qtilities::CoreGui::HelpManagerPrivateData {
    HelpManagerPrivateData() { }

    QPointer<QHelpEngine>   helpEngine;
    QStringList             registered_files_session;
    QStringList             registered_files;
};

Qtilities::CoreGui::HelpManager::HelpManager(QObject* parent) : QObject(parent) {
    d = new HelpManagerPrivateData;

    readSettings(false);
}

Qtilities::CoreGui::HelpManager::~HelpManager() {
    delete d;
}

QHelpEngine* Qtilities::CoreGui::HelpManager::helpEngine()  {
    if (!d->helpEngine) {
        // Delete the current help collection file:
        QFile::remove(QtilitiesCoreApplicationPrivate::instance()->applicationSessionPath() + "/help_collection.qhc");
        d->helpEngine = new QHelpEngine(QtilitiesCoreApplicationPrivate::instance()->applicationSessionPath() + "/help_collection.qhc",this);
        connect(d->helpEngine,SIGNAL(warning(QString)),SLOT(logMessage(QString)));
    }

    return d->helpEngine;
}

void HelpManager::initialize() {
    // Call helpEngine() in case it was not called before to construct d->helpEngine:
    helpEngine();

    // Search engine indexing:
    Task* indexing = new Task("Indexing Registered Documentation");
    indexing->setTaskLifeTimeFlags(Task::LifeTimeDestroyWhenSuccessful);
    OBJECT_MANAGER->registerObject(indexing);
    connect(d->helpEngine->searchEngine(),SIGNAL(indexingStarted()),indexing,SLOT(startTask()));
    connect(d->helpEngine->searchEngine(),SIGNAL(indexingFinished()),indexing,SLOT(completeTask()));

    if (!d->helpEngine->setupData())
        LOG_ERROR("Failed to setup help engine: " + d->helpEngine->error());

    foreach (QString file, d->registered_files) {
        if (!d->helpEngine->registerDocumentation(file))
            LOG_ERROR(tr("Failed to register documentation from file: ") + file);
        else
            LOG_INFO(tr("Successfully registered documentation from file: ") + file);
    }
}

void HelpManager::clearRegisterFiles(bool initialize_after_change) {
    d->registered_files.clear();
    if (initialize_after_change)
        initialize();
}

void HelpManager::registerFiles(const QStringList &files, bool initialize_after_change) {
    d->registered_files << files;
    if (initialize_after_change)
        initialize();
}

void HelpManager::registerFile(const QString &file, bool initialize_after_change) {
    d->registered_files << file;
    if (initialize_after_change)
        initialize();
}

QStringList HelpManager::registeredFiles() const {
    return d->registered_files;
}

void HelpManager::unregisterFiles(const QStringList &files, bool initialize_after_change) {
    foreach (QString file, files)
        d->registered_files.removeAll(file);
    if (initialize_after_change)
        initialize();
}

void HelpManager::unregisterFile(const QString &file, bool initialize_after_change) {
    d->registered_files << file;
    if (initialize_after_change)
        initialize();
}

void Qtilities::CoreGui::HelpManager::logMessage(const QString& message) {
    LOG_WARNING(message);
}

void Qtilities::CoreGui::HelpManager::readSettings(bool initialize_after_change) {
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Help");
    d->registered_files = settings.value("registered_files").toStringList();
    settings.endGroup();
    settings.endGroup();

    if (initialize_after_change)
        initialize();
}

void Qtilities::CoreGui::HelpManager::writeSettings() {
    if (!QtilitiesCoreApplication::qtilitiesSettingsPathEnabled())
        return;

    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Help");
    settings.setValue("registered_files",d->registered_files);
    settings.endGroup();
    settings.endGroup();
}
