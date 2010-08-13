/****************************************************************************
**
** Copyright 2009-2010, Jaco Naude
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library;  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "SubjectFilterTemplate.h"
#include "Observer.h"

struct Qtilities::Core::SubjectFilterTemplateData {
    SubjectFilterTemplateData() {}
};

Qtilities::Core::SubjectFilterTemplate::SubjectFilterTemplate(QObject* parent) : AbstractSubjectFilter(parent) {
	d = new SubjectFilterTemplateData;
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::SubjectFilterTemplate::evaluateAttachment(QObject* obj) const {
    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::SubjectFilterTemplate::initializeAttachment(QObject* obj, bool import_cycle) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    if (!observer) {
        LOG_TRACE("Cannot evaluate an attachment in a subject filter without an observer context.");
        return false;
    }

    return true;
}

void Qtilities::Core::SubjectFilterTemplate::finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle) {
    Q_UNUSED(obj);
    Q_UNUSED(attachment_successful);
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::SubjectFilterTemplate::evaluateDetachment(QObject* obj) const {
    Q_UNUSED(obj);
    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::SubjectFilterTemplate::initializeDetachment(QObject* obj, bool subject_deleted) {
    Q_UNUSED(obj);
    return true;
}

void Qtilities::Core::SubjectFilterTemplate::finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted) {
    Q_UNUSED(obj);
    Q_UNUSED(detachment_successful);
}

QStringList Qtilities::Core::SubjectFilterTemplate::monitoredProperties() {
    QStringList reserved_properties;
    return reserved_properties;
}

bool Qtilities::Core::SubjectFilterTemplate::monitoredPropertyChanged(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    Q_UNUSED(obj);
    Q_UNUSED(property_name);
    Q_UNUSED(propertyChangeEvent);
    return false;
}



