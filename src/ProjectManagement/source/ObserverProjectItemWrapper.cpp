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

#include <IExportable.h>
#include <ObserverRelationalTable.h>
#include <QtilitiesCoreApplication.h>
#include <Logger.h>
#include <IFactory.h>

#include <QApplication>

struct Qtilities::ProjectManagement::ObserverProjectItemWrapperData {
    ObserverProjectItemWrapperData() : observer(0) {}

    Observer* observer;
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


bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::saveProjectItem(QDataStream& stream) {
    if (OBJECT_MANAGER->exportObserverBinary(stream,d->observer,true) == IExportable::Failed)
        return false;
    else
        return true;
}

bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::loadProjectItem(QDataStream& stream) {
    if (OBJECT_MANAGER->importObserverBinary(stream,d->observer,true) == IExportable::Failed)
        return false;
    else
        return true;
}

bool Qtilities::ProjectManagement::ObserverProjectItemWrapper::closeProjectItem() {
    if (!d->observer)
        return false;

    d->observer->deleteAll();
    d->observer->setModificationState(true);
    return true;
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
