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

#include "ObserverStringSubject.h"
#include <Observer.h>
#include <IFactory.h>

using namespace Qtilities::Core::Interfaces;

FactoryItem<QObject, ObserverStringSubject> ObserverStringSubject::factory;

struct ObserverStringSubjectData {
    ObserverStringSubjectData() : is_modified(false) {}

    QString subject_string;
    bool is_modified;
};

ObserverStringSubject::ObserverStringSubject(const QString& string, QObject *parent) :
    QObject(parent)
{
    d = new ObserverStringSubjectData;
    d->subject_string = string;
    setObjectName(string);
}

ObserverStringSubject::~ObserverStringSubject() {
    delete d;
}

void ObserverStringSubject::setString(const QString& string) {
    d->subject_string = string;
}

QString ObserverStringSubject::string() const {
    return d->subject_string;
}

Qtilities::Core::Observer::ExportModeFlags ObserverStringSubject::supportedFormats() const {
    return IExportable::Binary;
}

Qtilities::Core::Interfaces::IFactoryData ObserverStringSubject::factoryData() const {
    IFactoryData factoryData("Example IFactory","Observer String Subject",objectName());
    return factoryData;
}

Qtilities::Core::Interfaces::IExportable::Result ObserverStringSubject::exportBinary(QDataStream& stream) const {
    // Create IFactoryData
    IFactoryData factory_data = factoryData();
    factory_data.exportBinary(stream);
    stream << d->subject_string;
    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::Result ObserverStringSubject::importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list) {
    stream >> d->subject_string;
    return IExportable::Complete;
}

bool ObserverStringSubject::isModified() const {
    return d->is_modified;
}

void ObserverStringSubject::setModificationState(bool new_state, bool notify_listeners, bool notify_subjects) {
    if (d->is_modified != new_state) {
        d->is_modified = new_state;
        if (notify_listeners)
            emit modificationStateChanged(new_state);
    }
}
