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
#include "ObjManagementConstants.h"
#include "Observer.h"

#include <QtDebug>
#include <QMutex>
#include <QVariant>
#include <QRegExpValidator>

struct Qtilities::ObjManagement::SubjectFilterTemplateData {
    SubjectFilterTemplateData() {}
};

Qtilities::ObjManagement::AbstractSubjectFilter::SubjectFilterTemplate::SubjectFilterTemplate(QObject* parent) : AbstractSubjectFilter(parent) {
	d = new SubjectFilterTemplateData;
}

Qtilities::ObjManagement::AbstractSubjectFilter::EvaluationResult Qtilities::ObjManagement::SubjectFilterTemplate::evaluateAttachment(QObject* obj) const {
    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::ObjManagement::SubjectFilterTemplate::initializeAttachment(QObject* obj) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    if (!observer) {
        qDebug() << QString("Cannot evaluate an attachment in a subject filter without an observer context.");
        return false;
    } else
        return true;
}

void Qtilities::ObjManagement::SubjectFilterTemplate::finalizeAttachment(QObject* obj, bool attachment_successful) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return;
    #endif

    if (!observer) {
        qDebug() << QString("Cannot evaluate an attachment in a subject filter without an observer context.");
        return;
    }

    if (!attachment_successful)
        return;

 
}

Qtilities::ObjManagement::AbstractSubjectFilter::EvaluationResult Qtilities::ObjManagement::SubjectFilterTemplate::evaluateDetachment(QObject* obj) const {
    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::ObjManagement::SubjectFilterTemplate::initializeDetachment(QObject* obj) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    return true;
}

void Qtilities::ObjManagement::SubjectFilterTemplate::finalizeDetachment(QObject* obj, bool detachment_successful) {
    #ifndef QT_NO_DEBUG
        Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
        if (!obj)
            return false;
    #endif

    if (!detachment_successful)
        return;

    // Ensure that property changes are not handled by the QDynamicPropertyChangeEvent handler.
    filter_mutex.tryLock();



    // Unlock the filter mutex.
    filter_mutex.unlock();
}

QStringList Qtilities::ObjManagement::SubjectFilterTemplate::reservedProperties() {
    QStringList reserved_properties;
    return reserved_properties;
}

bool Qtilities::ObjManagement::SubjectFilterTemplate::reservedPropertyChanged(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    if(!filter_mutex.tryLock())
        return false;

    if (!observer)
        return false;


    filter_mutex.unlock();
    return false;
}


