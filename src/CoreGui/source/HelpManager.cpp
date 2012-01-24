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

using namespace Qtilities::CoreGui;

struct Qtilities::CoreGui::HelpManagerPrivateData {
    HelpManagerPrivateData() { }

    QPointer<QHelpEngine>   helpEngine;
    QStringList             registered_files_session;
    QStringList             registered_files;
    QPointer<Task>          setup_task;
    QPointer<Task>          indexing_task;
    QMap<QString,QString>   file_namespace_map;
};

HelpManager::HelpManager(QObject* parent) : QObject(parent) {
    d = new HelpManagerPrivateData;

}

HelpManager::~HelpManager() {
    delete d;
}

QHelpEngine* HelpManager::helpEngine()  {
    if (!d->helpEngine) {
        qDebug() << "You must initialize the help manager before using the helpEngine()";
    }

    return d->helpEngine;
}

void HelpManager::initialize() {
    if (!d->helpEngine) {
        QTemporaryFile temp_file;
        temp_file.open();

        // Delete the current help collection file:
        d->helpEngine = new QHelpEngine(temp_file.fileName(),this);
        connect(d->helpEngine,SIGNAL(warning(QString)),SLOT(logMessage(QString)));

        readSettings(false);
    }
    Q_ASSERT(d->helpEngine);

    // Search engine setup:
    if (!d->setup_task) {
        d->setup_task = new Task("Setting Up Help Engine");
        OBJECT_MANAGER->registerObject(d->setup_task);
        connect(d->helpEngine,SIGNAL(setupStarted()),d->setup_task,SLOT(startTask()));
        connect(d->helpEngine,SIGNAL(setupFinished()),d->setup_task,SLOT(completeTask()));
    }

    if (!d->helpEngine->setupData())
        LOG_ERROR(tr("Failed to setup the help engine. Error: ") + d->helpEngine->error());

    // Unregister everything here. All files in d->registered_files will be registered again.
    unregisterAllNamespaces();

    foreach (QString registered_file, d->registered_files) {
        QString filename = registered_file;

        QFile file(filename);
        QTemporaryFile *temp_file =QTemporaryFile::createLocalFile(file);
        if (temp_file) {
            filename = temp_file->fileName();
            temp_file->setParent(this);
            temp_file->setAutoRemove(true);
        }

        if (!d->helpEngine->registerDocumentation(filename))
            LOG_ERROR(tr("Failed to register documentation from file: ") + registered_file + tr(". Error: ") + d->helpEngine->error());
        else {
            QString namespace_name = d->helpEngine->namespaceName(filename);
            d->file_namespace_map[filename] = namespace_name;
            LOG_INFO(tr("Successfully registered documentation from file: ") + registered_file + tr(" using namespace ") + namespace_name);
        }
    }

    // Search engine indexing:
    if (!d->indexing_task) {
        d->indexing_task = new Task("Indexing Documentation");
        OBJECT_MANAGER->registerObject(d->indexing_task);
        connect(d->helpEngine->searchEngine(),SIGNAL(indexingStarted()),d->indexing_task,SLOT(startTask()));
        connect(d->helpEngine->searchEngine(),SIGNAL(indexingFinished()),d->indexing_task,SLOT(completeTask()));
    }
    d->helpEngine->searchEngine()->reindexDocumentation();
}

void HelpManager::clearRegisteredFiles(bool initialize_after_change) {
    d->registered_files.clear();
    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

void HelpManager::unregisterAllNamespaces() {
    foreach (QString namespace_name, d->file_namespace_map.values()) {
        if (!d->helpEngine->unregisterDocumentation(namespace_name))
            LOG_ERROR(tr("Failed to unregister namespace from help engine: ") + namespace_name + tr(". Error: ") + d->helpEngine->error());
        else {
            LOG_INFO(tr("Successfully unregistered namespace from help engine: ") + namespace_name);
        }
    }

    d->file_namespace_map.clear();
}

void HelpManager::registerFiles(const QStringList &files, bool initialize_after_change) {
    foreach (QString file, files) {
        if (d->registered_files.contains(file))
            continue;
        d->registered_files << file;
    }

    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

void HelpManager::registerFile(const QString &file, bool initialize_after_change) {
    if (d->registered_files.contains(file))
        return;

    d->registered_files << file;
    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

QStringList HelpManager::registeredFiles() const {
    return d->registered_files;
}

void HelpManager::unregisterFiles(const QStringList &files, bool initialize_after_change) {
    QStringList old_files = d->registered_files;
    foreach (QString file, files)
        d->registered_files.removeAll(file);

    if (old_files == d->registered_files)
        return;

    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

void HelpManager::unregisterFile(const QString &file, bool initialize_after_change) {
    if (!d->registered_files.contains(file))
        return;

    d->registered_files << file;
    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

void Qtilities::CoreGui::HelpManager::logMessage(const QString& message) {
    LOG_WARNING(message);
}

void Qtilities::CoreGui::HelpManager::readSettings(bool initialize_after_change) {
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Help");
    registerFiles(settings.value("registered_files").toStringList(),initialize_after_change);
    settings.endGroup();
    settings.endGroup();
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
