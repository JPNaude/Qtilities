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

#include "ObserverProjectItemWrapper.h"
#include "ProjectManager.h"

#include <IExportable.h>
#include <ObserverRelationalTable.h>
#include <QtilitiesCoreApplication.h>
#include <Logger.h>
#include <IFactory.h>

#include <QApplication>

struct Qtilities::ProjectManagement::ObserverProjectItemWrapperData {
    ObserverProjectItemWrapperData() : observer(0) {}

    QPointer<Observer> observer;
};

using namespace Qtilities::Core::Interfaces;
using namespace Qtilities::Core;

Qtilities::ProjectManagement::ObserverProjectItemWrapper::ObserverProjectItemWrapper(QObject *parent) :
    QObject(parent)
{
    d = new ObserverProjectItemWrapperData;
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

bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::closeProjectItem() {
    if (!d->observer)
        return false;

    d->observer->deleteAll();
    d->observer->setModificationState(true);
    return true;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::ProjectManagement::ObserverProjectItemWrapper::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

Qtilities::Core::Interfaces::IFactoryTag Qtilities::ProjectManagement::ObserverProjectItemWrapper::factoryData() const {
    return IFactoryTag();
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::ObserverProjectItemWrapper::exportBinary(QDataStream& stream, QList<QVariant> params) const {
    return OBJECT_MANAGER->exportObserverBinary(stream,d->observer,PROJECT_MANAGER->verboseLogging());
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::ObserverProjectItemWrapper::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    return OBJECT_MANAGER->importObserverBinary(stream,d->observer,PROJECT_MANAGER->verboseLogging());
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::ObserverProjectItemWrapper::exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params) const {
    if (d->observer)
        return d->observer->exportXML(doc,object_node,params);
    else
        return IExportable::Incomplete;
}

Qtilities::Core::Interfaces::IExportable::Result Qtilities::ProjectManagement::ObserverProjectItemWrapper::importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params) {
    if (d->observer)
        return d->observer->importXML(doc,object_node,import_list,params);
    else
        return IExportable::Incomplete;
}

bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::isModified() const {
    if (d->observer)
        return d->observer->isModified();
    else
        return false;
}

void Qtilities::ProjectManagement::ObserverProjectItemWrapper::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets) {
    if (notification_targets & IModificationNotifier::NotifyListeners)
        emit modificationStateChanged(new_state);
    if (notification_targets & IModificationNotifier::NotifySubjects)
        d->observer->setModificationState(new_state,notification_targets);
}
