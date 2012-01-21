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

#include "ObserverProjectItemWrapper.h"
#include "ProjectManager.h"

#include <IExportable>
#include <QtilitiesCoreApplication>
#include <Logger>
#include <IFactoryProvider>

#include <QApplication>
#include <QDomNodeList>

using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::Core;

struct Qtilities::ProjectManagement::ObserverProjectItemWrapperPrivateData {
    ObserverProjectItemWrapperPrivateData() : observer(0),
        export_flags(ObserverData::ExportData) {}

    QPointer<Observer> observer;
    ObserverData::ExportItemFlags export_flags;
};

Qtilities::ProjectManagement::ObserverProjectItemWrapper::ObserverProjectItemWrapper(Observer* observer, QObject *parent) :
    QObject(parent)
{
    d = new ObserverProjectItemWrapperPrivateData;
    if (observer)
        setObserverContext(observer);
}

void Qtilities::ProjectManagement::ObserverProjectItemWrapper::setObserverContext(Observer* observer) {
    if (d->observer)
        d->observer->disconnect(this);

    d->observer = observer;
    setObjectName(QString("Observer Project Item: \"%1\"").arg(observer->observerName()));
    IModificationNotifier* mod_iface = qobject_cast<IModificationNotifier*> (observer);
    if (mod_iface)
        connect(mod_iface->objectBase(),SIGNAL(modificationStateChanged(bool)),SLOT(setModificationState(bool)));
}

QString Qtilities::ProjectManagement::ObserverProjectItemWrapper::projectItemName() const {
    if (d->observer)
        return d->observer->observerName() + tr(" Project Item");
    else
        return tr("Uninitialized Observer Project Item Wrapper");
}

bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::newProjectItem() {
    if (!d->observer)
        return false;

    d->observer->deleteAll();
    d->observer->setModificationState(false);
    return true;
}

bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::closeProjectItem(ITask *task) {
    Q_UNUSED(task)

    if (!d->observer)
        return false;

    d->observer->startProcessingCycle();
    d->observer->deleteAll();
    d->observer->setModificationState(true);
    d->observer->endProcessingCycle();
    d->observer->refreshViewsLayout();
    return true;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::ProjectManagement::ObserverProjectItemWrapper::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::ProjectManagement::ObserverProjectItemWrapper::instanceFactoryInfo() const {
    return InstanceFactoryInfo();
}

void Qtilities::ProjectManagement::ObserverProjectItemWrapper::setExportVersion(Qtilities::ExportVersion version) {
    IExportable::setExportVersion(version);
    if (d->observer)
        d->observer->setExportVersion(version);
}

void Qtilities::ProjectManagement::ObserverProjectItemWrapper::setExportTask(ITask* task) {
    if (d->observer)
        d->observer->setExportTask(task);
    IExportable::setExportTask(task);
}

void Qtilities::ProjectManagement::ObserverProjectItemWrapper::clearExportTask() {
    if (d->observer)
        d->observer->clearExportTask();
    IExportable::clearExportTask();
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::ObserverProjectItemWrapper::exportBinary(QDataStream& stream) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (!d->observer)
        return IExportable::Incomplete;

    d->observer->setExportVersion(exportVersion());
    return d->observer->exportBinaryExt(stream,d->export_flags);
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::ObserverProjectItemWrapper::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {    
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (!d->observer)
        return IExportable::Incomplete;

    d->observer->setExportVersion(exportVersion());
    return d->observer->importBinary(stream,import_list);
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::ObserverProjectItemWrapper::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesExportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (d->observer) {
        // Add a new node for this observer. We don't want it to add its factory data
        // to the ProjectItem node.
        QDomElement wrapper_data = doc->createElement("ObserverProjectItemWrapper");
        object_node->appendChild(wrapper_data);
        return d->observer->exportXmlExt(doc,&wrapper_data,d->export_flags);
    } else
        return IExportable::Incomplete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags Qtilities::ProjectManagement::ObserverProjectItemWrapper::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)

    IExportable::ExportResultFlags version_check_result = IExportable::validateQtilitiesImportVersion(exportVersion(),exportTask());
    if (version_check_result != IExportable::VersionSupported)
        return version_check_result;

    if (d->observer) {
        QDomNodeList childNodes = object_node->childNodes();
        for(int i = 0; i < childNodes.count(); i++)
        {
            QDomNode childNode = childNodes.item(i);
            QDomElement child = childNode.toElement();

            if (child.isNull())
                continue;

            if (child.tagName() == "ObserverProjectItemWrapper") {
                d->observer->setExportVersion(exportVersion());
                doc->clear();
                return d->observer->importXml(doc,&child,import_list);
            }
        }
    }

    return IExportable::Incomplete;
}

void Qtilities::ProjectManagement::ObserverProjectItemWrapper::setExportItemFlags(ObserverData::ExportItemFlags flags) {
    d->export_flags = flags;
}

Qtilities::Core::ObserverData::ExportItemFlags Qtilities::ProjectManagement::ObserverProjectItemWrapper::exportItemFlags() const {
    return d->export_flags;
}

bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::isModified() const {
    if (d->observer)
        return d->observer->isModified();
    else
        return false;
}

void Qtilities::ProjectManagement::ObserverProjectItemWrapper::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    if (!d->observer)
        return;

    if (notification_targets & IModificationNotifier::NotifyListeners)
        emit modificationStateChanged(new_state);
    if (notification_targets & IModificationNotifier::NotifySubjects)
        d->observer->setModificationState(new_state,notification_targets);
}
