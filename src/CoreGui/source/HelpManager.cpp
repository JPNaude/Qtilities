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

    // Settings
    QUrl                    home_page;
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

    foreach (const QString& registered_file, d->registered_files) {
        QString filename = registered_file;

        QFile file(filename);
        if (file.open(QFile::ReadOnly)) {
            QTemporaryFile *temp_file = QTemporaryFile::createLocalFile(file);
            if (temp_file) {
                filename = temp_file->fileName();
                temp_file->setParent(this);
                temp_file->setAutoRemove(true);
            }
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

void HelpManager::setHomePage(const QUrl& home_page) {
    if (d->home_page != home_page) {
        d->home_page = home_page;
        emit homePageChanged(home_page);
    }
}

QUrl HelpManager::homePage() const {
    return d->home_page;
}

void HelpManager::clearRegisteredFiles(bool initialize_after_change) {
    d->registered_files.clear();
    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

void HelpManager::unregisterAllNamespaces() {
    foreach (const QString& namespace_name, d->file_namespace_map.values()) {
        if (!d->helpEngine->unregisterDocumentation(namespace_name))
            LOG_ERROR(tr("Failed to unregister namespace from help engine: ") + namespace_name + tr(". Error: ") + d->helpEngine->error());
        else {
            LOG_INFO(tr("Successfully unregistered namespace from help engine: ") + namespace_name);
        }
    }

    d->file_namespace_map.clear();
}

void HelpManager::registerFiles(const QStringList &files, bool initialize_after_change) {
    foreach (const QString& file, files) {
        QString formatted_file = formatFileName(file);
        if (d->registered_files.contains(formatted_file,Qt::CaseInsensitive))
            continue;
        d->registered_files << formatted_file;
    }

    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

void HelpManager::registerFile(const QString &file, bool initialize_after_change) {
    QString formatted_file = formatFileName(file);
    if (d->registered_files.contains(formatted_file,Qt::CaseInsensitive))
        return;

    d->registered_files << formatted_file;
    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

QStringList HelpManager::registeredFiles() const {
    return d->registered_files;
}

void HelpManager::unregisterFiles(const QStringList &files, bool initialize_after_change) {
    QStringList old_files = d->registered_files;
    foreach (const QString& file, files)
        d->registered_files.removeAll(formatFileName(file));

    if (old_files == d->registered_files)
        return;

    if (initialize_after_change)
        initialize();

    emit registeredFilesChanged(d->registered_files);
}

void HelpManager::unregisterFile(const QString &file, bool initialize_after_change) {
    QString formatted_file = formatFileName(file);
    if (!d->registered_files.contains(formatted_file,Qt::CaseInsensitive))
        return;

    d->registered_files << formatted_file;
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
    if (!QtilitiesCoreApplication::qtilitiesSettingsEnabled())
        return;

    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Help");
    settings.setValue("registered_files",d->registered_files);
    settings.endGroup();
    settings.endGroup();
}

QString Qtilities::CoreGui::HelpManager::formatFileName(const QString &file_name) {
    QString formatted_name;
    #ifdef Q_OS_WIN
    formatted_name = QDir::fromNativeSeparators(QDir::cleanPath(file_name));
    #else
    formatted_name = FileUtils::toNativeSeparators(QDir::cleanPath(file_name));
    #endif
    return formatted_name;
}

void HelpManager::requestUrlDisplay(const QUrl &url, bool ensure_visible) {
    // Check if its an external URL, is so open in browser otherwise open in help plugin:
    if (url.scheme() == "qthelp")
        emit forwardRequestUrlDisplay(url,ensure_visible);
    else if (url.scheme() == "qthelp")
        QDesktopServices::openUrl(url);
    else if (url.scheme() == "http")
        QDesktopServices::openUrl(url);

}
