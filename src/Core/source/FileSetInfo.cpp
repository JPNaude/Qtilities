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

#include "FileSetInfo.h"
#include "FileUtils.h"
#include "QtilitiesCoreConstants.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHash>
#include <QtDebug>
#include <QCoreApplication>
#include <QDomDocument>

using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::Core::Constants;

struct Qtilities::Core::FileSetInfoPrivateData {
    FileSetInfoPrivateData() : files_hash(-1),
        file_watching_enabled(true) { }

    QList<QtilitiesFileInfo>    files;
    int                         files_hash;
    QFileSystemWatcher          watcher;

    bool                        file_watching_enabled;
};

namespace Qtilities {
    namespace Core {
        FactoryItem<QObject, FileSetInfo> FileSetInfo::factory;
    }
}

using namespace Qtilities::Core;

Qtilities::Core::FileSetInfo::FileSetInfo(QObject* parent) : QObject(parent) {
    d = new FileSetInfoPrivateData;
    connect(&d->watcher,SIGNAL(fileChanged(QString)),SIGNAL(fileChanged(QString)));
    connect(&d->watcher,SIGNAL(fileChanged(QString)),SIGNAL(setChanged()));
}

FileSetInfo::FileSetInfo(const FileSetInfo &other) : QObject(other.parent()) {
    d = new FileSetInfoPrivateData;
    connect(&d->watcher,SIGNAL(fileChanged(QString)),SIGNAL(fileChanged(QString)));
    connect(&d->watcher,SIGNAL(fileChanged(QString)),SIGNAL(setChanged()));

    d->watcher.removePaths(d->watcher.files());
    d->files = other.files();
    if (d->files.count() > 0)
        d->watcher.addPaths(filePaths());

    emit setChanged();
}

FileSetInfo& Qtilities::Core::FileSetInfo::operator=(const FileSetInfo& other) {
    if (this==&other) return *this;

    d->watcher.removePaths(d->watcher.files());
    d->files = other.files();
    if (d->files.count() > 0)
        d->watcher.addPaths(filePaths());

    emit setChanged();

    return *this;
}

Qtilities::Core::FileSetInfo::~FileSetInfo() {
    delete d;
}

// -----------------------------------
// Control over the files in the set.
// -----------------------------------

bool FileSetInfo::addFile(const QString &file_path) {
    //qDebug() << "ADDING FILE TO FILESETINFO" << file_path;

    QtilitiesFileInfo fi(file_path);
    if (!d->files.contains(fi)) {
        if (d->file_watching_enabled) {
            if (fi.exists())
                d->watcher.addPath(fi.actualFilePath());
        }
        d->files << fi;
        emit setChanged();
        return true;
    }

    return false;
}

bool FileSetInfo::addFile(QtilitiesFileInfo file_info) {
    if (!d->files.contains(file_info)) {
        if (d->file_watching_enabled) {
            if (file_info.exists())
                d->watcher.addPath(file_info.actualFilePath());
        }
        d->files << file_info;
        emit setChanged();
        return true;
    }

    return false;
}

bool FileSetInfo::removeFile(const QString &file_path) {
    QtilitiesFileInfo fi(file_path);
    if (!d->files.contains(fi)) {
        d->watcher.removePath(fi.actualFilePath());
        d->files.removeOne(fi);
        emit setChanged();
        return true;
    }

    return false;
}

bool FileSetInfo::removeFile(QtilitiesFileInfo file_info) {
    if (!d->files.contains(file_info)) {
        d->watcher.removePath(file_info.actualFilePath());
        d->files.removeOne(file_info);
        emit setChanged();
        return true;
    }

    return false;
}

bool FileSetInfo::hasFile(const QString &file_path) const {
    QtilitiesFileInfo fi(file_path);
    return d->files.contains(fi);
}

QStringList FileSetInfo::filePaths() const {
    QStringList file_paths;
    foreach (QtilitiesFileInfo fi, d->files)
        file_paths << fi.actualFilePath();
    file_paths.sort();
    return file_paths;
}

QList<QtilitiesFileInfo> FileSetInfo::files() const {
    return d->files;
}

bool FileSetInfo::isEmpty() const {
    return d->files.isEmpty();
}

int FileSetInfo::count() const {
    return d->files.count();
}

void FileSetInfo::clear() {
    if (d->watcher.files().count() > 0)
        d->watcher.removePaths(d->watcher.files());
    d->files.clear();
    emit setChanged();
}

void FileSetInfo::updateRelativeToPaths(const QString &search_string, const QString &replace_string, ITask *task) {
    QList<QtilitiesFileInfo> local_list = files();
    for (int i = 0; i < local_list.count(); ++i) {
        QString actual_file_path = local_list.at(i).actualFilePath();

        bool do_replacement = false;
        if (FileUtils::pathStartsWith(replace_string,search_string)) {
            // Do check 1:
            do_replacement = (FileUtils::pathStartsWith(actual_file_path,search_string) && !FileUtils::pathStartsWith(actual_file_path,replace_string));
        } else {
            // Do check 2:
            do_replacement = (FileUtils::pathStartsWith(actual_file_path,search_string));
        }

        if (do_replacement) {
            actual_file_path.remove(0,search_string.length());
            QString new_actual_path = actual_file_path.prepend(replace_string);

            QtilitiesFileInfo updated_fi(new_actual_path) ;
            if (task)
                task->logMessage("Updating relative path of file in file set. Previous: \"" + local_list.at(i).actualFilePath() + "\", Updated: " + updated_fi.actualFilePath());
            addFile(updated_fi);
            removeFile(local_list.at(i));
        }
    }
}

// -----------------------------------
// State Of Files In Set
// -----------------------------------
int FileSetInfo::fileSetHash(bool update_previous_hash_storage) const {
    if (d->files.isEmpty())
        return -1;

    // We store the file with its corresponding hash code in a map in order
    // to always sort the hash code order according to the file name:
    QMap<QString,int> sorted_hash_codes;

    QString hash_string = "";
    foreach (const QString& file, filePaths())
        sorted_hash_codes[file] = FileUtils::fileHashCode(file);

    for (int i = 0 ; i < sorted_hash_codes.count(); ++i) {
        //qDebug() << "TEST" << i << sorted_hash_codes.values().at(i) << sorted_hash_codes.keys().at(i);
        hash_string.append(QString::number(sorted_hash_codes.values().at(i)));
    }

    //qDebug() << "Combined hash string" << hash_string << qHash(hash_string);
    int new_hash = qHash(hash_string);
    if (update_previous_hash_storage)
        d->files_hash = new_hash;
    return new_hash;
}

void FileSetInfo::setFileSetHash(int hash) {
    d->files_hash = hash;
}

int FileSetInfo::fileSetHashPrevious() const {
    return d->files_hash;
}

bool FileSetInfo::allFilesExists() const {
    for (int i = 0; i < d->files.count(); ++i) {
        QFile file(d->files.at(i).actualFilePath());
        if (!file.exists())
            return false;
    }

    return true;
}

void FileSetInfo::disableFileWatching() {
    if (d->file_watching_enabled) {
        if (d->watcher.files().count() > 0)
            d->watcher.removePaths(d->watcher.files());
        d->file_watching_enabled = false;
    }
}

void FileSetInfo::enableFileWatching() {
    if (!d->file_watching_enabled) {
        if (d->files.count() > 0)
            d->watcher.addPaths(filePaths());
        d->file_watching_enabled = true;
    }
}

void FileSetInfo::setFileWatchingEnabled(bool enabled) {
    if (d->file_watching_enabled != enabled) {
        if (enabled)
            enableFileWatching();
        else
            disableFileWatching();
    }
}

bool FileSetInfo::fileWatchingEnabled() const {
    return d->file_watching_enabled;
}

// --------------------------------
// IExportable Implementation
// --------------------------------
Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::FileSetInfo::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

InstanceFactoryInfo FileSetInfo::staticInstanceFactoryInfo() {
    InstanceFactoryInfo factoryData(qti_def_FACTORY_QTILITIES,qti_def_FACTORY_TAG_FILE_SET_INFO,"");
    return factoryData;
}

InstanceFactoryInfo FileSetInfo::instanceFactoryInfo() const {
    InstanceFactoryInfo fi = FileSetInfo::staticInstanceFactoryInfo();
    fi.d_instance_name = objectName();
    return fi;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::FileSetInfo::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    stream << filePaths();
    stream << (quint32) fileSetHash();

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::FileSetInfo::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    clear();

    QStringList files;
    stream >> files;
    foreach (const QString& file, files)
        addFile(file);

    quint32 ui32;
    stream >> ui32;
    d->files_hash = ui32;

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::FileSetInfo::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    object_node->setAttribute("FileCount",d->files.count());
    for (int i = 0; i < d->files.count(); ++i) {
        QDomElement file = doc->createElement("File_" + QString::number(i));
        file.setAttribute("FilePath",d->files.at(i).filePath());
        if (!d->files.at(i).relativeToPath().isEmpty())
            file.setAttribute("RelativeToPath",d->files.at(i).relativeToPath());
        object_node->appendChild(file);
    }
    if (d->files.count() > 0)
        object_node->setAttribute("FileSetHash",QString::number(fileSetHash()));

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::Core::FileSetInfo::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    clear();
    int depth_readback = 0;

    if (object_node->hasAttribute("FileSetHash"))
        d->files_hash = object_node->attribute("FileSetHash").toInt();

    if (object_node->hasAttribute("FileCount"))
        depth_readback = object_node->attribute("FileCount").toInt();

    QDomNodeList childNodes = object_node->childNodes();
    for(int i = 0; i < childNodes.count(); ++i)
    {
        QDomNode childNode = childNodes.item(i);
        QDomElement child = childNode.toElement();

        if (child.isNull())
            continue;

        if (child.tagName().startsWith("File_")) {
            addFile(QtilitiesFileInfo(child.attribute("FilePath"),child.attribute("RelativeToPath")));
            continue;
        }
    }

    if (d->files.count() == depth_readback)
        return IExportable::Complete;
    else
        return IExportable::Failed;
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::FileSetInfo& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::FileSetInfo& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}

