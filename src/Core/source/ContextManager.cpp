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

#include "QtilitiesCoreConstants.h"
#include "ContextManager.h"
#include "IContext.h"

#include <Logger.h>

#include <QList>
#include <QtDebug>
#include <QPointer>
#include <QCoreApplication>

using namespace Qtilities::Core::Constants;

struct Qtilities::Core::ContextManagerData {
    ContextManagerData() : id_counter(-1) { }

    int id_counter;
    QMap<QString, int> string_id_map;

    QList<int> active_contexts;
    QList<int> contexts;
};

Qtilities::Core::ContextManager::ContextManager(QObject* parent) : IContextManager(parent)
{
    d = new ContextManagerData;
    setObjectName("Context Manager");

    // Add the standard context
    d->contexts.push_front(contextID(CONTEXT_STANDARD));
    setNewContext(CONTEXT_STANDARD);
}

int Qtilities::Core::ContextManager::registerContext(const QString& context) {
    int id = contextID(context);
    if (!d->contexts.contains(id)) {
        d->contexts.push_front(id);
        LOG_DEBUG("Context Manager: Registering new context: " + context);
        return id;
    } else
        return id;
}

QList<int> Qtilities::Core::ContextManager::allContexts() const {
    return d->contexts;
}

QStringList Qtilities::Core::ContextManager::contextNames() const {
    return d->string_id_map.keys();
}
QStringList Qtilities::Core::ContextManager::activeContextNames() const {
    QStringList active_names;
    for (int i = 0; i < d->active_contexts.count(); i++) {
        active_names << d->string_id_map.key(d->active_contexts.at(i));
    }
    return active_names;
}

bool Qtilities::Core::ContextManager::hasContext(QList<int> context) const {
    for (int i=0; i< d->contexts.count(); ++i) {
        if (context.contains(d->contexts.at(i)))
            return true;
    }
    return false;
}

bool Qtilities::Core::ContextManager::hasContext(int context) const {
    return d->contexts.contains(context);
}

bool Qtilities::Core::ContextManager::hasContext(const QString& context_string) const {
    return d->string_id_map.keys().contains(context_string);
}

void Qtilities::Core::ContextManager::setNewContext(int context, bool notify) {
    if (d->active_contexts.contains(context) && !notify) {
        LOG_TRACE("Context already active, the following contexts are currently active:");
        for (int i = 0; i < currentContexts().size(); i++) {
            QString debug_string = QString("- %1 - ID: %2, Name: %3").arg(i).arg(currentContexts().at(i)).arg(contextName(currentContexts().at(i)));
            LOG_TRACE(debug_string);
        }
        return;
    } else if (d->active_contexts.contains(context) && !notify) {
        emit aboutToSetNewContext(context);
        emit finishedSetNewContext(context);
        emit contextChanged(currentContexts());
        return;
    }

    if (notify)
        emit aboutToSetNewContext(context);

    // Clear contexts, and add standard.
    d->active_contexts.clear();
    if (context != 0)
        d->active_contexts.append(0);

    // If a valid context is not sent, we just set the context to the standard context.
    if (d->contexts.contains(context)) {
        d->active_contexts.append(context);
        LOG_TRACE("Context set to new, the following contexts are currently active:");
        for (int i = 0; i < currentContexts().size(); i++) {
            QString debug_string = QString("- %1 - ID: %2, Name: %3").arg(i).arg(currentContexts().at(i)).arg(contextName(currentContexts().at(i)));
            LOG_TRACE(debug_string);
        }
    } else
        LOG_WARNING(tr("Attempting to append unregistered context in function setNewContext with ID: ") + context);

    if (notify) {
        emit finishedSetNewContext(context);
        emit contextChanged(currentContexts());
    }
}

void Qtilities::Core::ContextManager::appendContext(int context, bool notify) {
    if (d->contexts.contains(context)) {
        if (d->active_contexts.contains(context)) {
            LOG_TRACE("Context already active, the following contexts are currently active:");
            for (int i = 0; i < currentContexts().size(); i++) {
                QString debug_string = QString("- %1 - ID: %2, Name: %3").arg(i).arg(currentContexts().at(i)).arg(contextName(currentContexts().at(i)));
                LOG_TRACE(debug_string);
            }
            return;
        }

        if (notify)
            emit aboutToAppendContext(context);
        d->active_contexts.append(context);

        if (notify)
            emit contextChanged(currentContexts());
        qDebug() << "Context appended, the following contexts are currently active:";
        for (int i = 0; i < currentContexts().size(); i++) {
            QString debug_string = QString("%1 - ID: %2, Name: %3").arg(i).arg(currentContexts().at(i)).arg(contextName(currentContexts().at(i)));
            qDebug() << debug_string;
        }

        if (notify)
            emit finishedAppendContext(context);
    } else {
        LOG_ERROR(tr("Attempting to append unregistered context in function appendContext with ID: ") + context);
    }
}

void Qtilities::Core::ContextManager::removeContext(int context, bool notify) {
    // Make sure the standard context is never removed
    if (context == 0)
        return;

    for (int i = 0; i < d->active_contexts.count(); i++) {
        if (d->active_contexts.at(i) == context) {
            emit aboutToRemoveContext(context);
            d->active_contexts.removeAt(i);
            LOG_TRACE("Context removed, the following contexts are currently active:");
            for (int i = 0; i < currentContexts().size(); i++) {
                QString debug_string = QString("- %1 - ID: %2, Name: %3").arg(i).arg(currentContexts().at(i)).arg(contextName(currentContexts().at(i)));
                LOG_TRACE(debug_string);
            }

            if (notify) {
                emit finishedRemoveContext(context);
                emit contextChanged(currentContexts());
            }
        }
    }
}

void Qtilities::Core::ContextManager::setNewContext(const QString& context_string, bool notify) {
    LOG_TRACE("Clearing all contexts. New active context: " + context_string);
    setNewContext(contextID(context_string),notify);
}

void Qtilities::Core::ContextManager::appendContext(const QString& context_string, bool notify) {
    LOG_DEBUG("Appending context: " + context_string);
    appendContext(contextID(context_string),notify);
}

void Qtilities::Core::ContextManager::removeContext(const QString& context_string, bool notify) {
    LOG_DEBUG("Removing context: " + context_string);
    removeContext(contextID(context_string),notify);
}

QList<int> Qtilities::Core::ContextManager::currentContexts() const {
    return d->active_contexts;
}

int Qtilities::Core::ContextManager::contextID(const QString& context_string) {
    if (context_string.isEmpty()) {
        LOG_ERROR(tr("Registering contexts without a descriptive name is not allowed. Context will not be registered."));
        return -1;
    }

    if (d->string_id_map.keys().contains(context_string))
        return d->string_id_map.value(context_string);
    else {
        ++d->id_counter;
        d->string_id_map[context_string] = d->id_counter;
        return d->id_counter;
    }
}

QString Qtilities::Core::ContextManager::contextString(int context_id) const {
    for (int i = 0; i < d->string_id_map.count(); i++) {
        if (d->string_id_map.values().at(i) == context_id)
            return d->string_id_map.keys().at(i);
    }

    return QString();
}

void Qtilities::Core::ContextManager::addContexts(QObject* obj) {
    IContext* new_context = qobject_cast<IContext*> (obj);
    if (new_context) {
        registerContext(new_context->contextString());
    }
}

QString Qtilities::Core::ContextManager::contextName(int id) const {
    if (d->contexts.contains(id)) {
        // Loop through contexts, can do this better if needed.
        for (int i = 0; i < d->string_id_map.values().count(); i++) {
            if (d->string_id_map.values().at(i) == id)
                return d->string_id_map.keys().at(i);
        }
    }
    return QString();
}

void Qtilities::Core::ContextManager::broadcastState() {
    emit contextChanged(currentContexts());
}
