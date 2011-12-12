/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#include "WidgetLoggerEngine.h"
#include "WidgetLoggerEngineFrontend.h"

#include <Logger.h>

#include <QWidget>
#include <QPointer>
#include <QList>
#include <QString>

// ------------------------------------
// WidgetLoggerEngine implementation
// ------------------------------------

struct Qtilities::CoreGui::WidgetLoggerEnginePrivateData { 
    QPointer<WidgetLoggerEngineFrontend> widget;
};

Qtilities::CoreGui::WidgetLoggerEngine::WidgetLoggerEngine() : AbstractLoggerEngine()
{
    d = new WidgetLoggerEnginePrivateData;
    setName(QObject::tr("Widget Logger Engine"));
}

Qtilities::CoreGui::WidgetLoggerEngine::~WidgetLoggerEngine()
{
    if (d->widget)
        d->widget->close();
    delete d;
}

void Qtilities::CoreGui::WidgetLoggerEngine::setWindowTitle(const QString& window_title) {
    if (d->widget)
        d->widget->setWindowTitle(window_title);
}

QString Qtilities::CoreGui::WidgetLoggerEngine::windowTitle() const {
    if (d->widget)
        return d->widget->windowTitle();
    else
        return QString();
}

bool Qtilities::CoreGui::WidgetLoggerEngine::initialize() {
    // During initialization we build a map with possible widgets
    abstractLoggerEngineData->is_initialized = true;
    d->widget = new WidgetLoggerEngineFrontend();
    connect(d->widget,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));

    if (d->widget) {
        // Print startup info messages
        Q_ASSERT(abstractLoggerEngineData->formatting_engine);
        d->widget->appendMessage(objectName() + tr(" initialized successfully."));
        d->widget->appendMessage(tr("Log messages will be formatted using the following formatting engine: ") + abstractLoggerEngineData->formatting_engine->name());
        d->widget->appendMessage(" ");
        return true;
    } else
        return false;
}

void Qtilities::CoreGui::WidgetLoggerEngine::finalize() {
    abstractLoggerEngineData->is_initialized = false;
}

QString Qtilities::CoreGui::WidgetLoggerEngine::description() const {
    return QObject::tr("An engine with a widget frontend used to display messages to the user.");
}

QString Qtilities::CoreGui::WidgetLoggerEngine::status() const {
    if (abstractLoggerEngineData->is_initialized) {
        if (abstractLoggerEngineData->is_enabled)
            return QObject::tr("Logging in progress...");
        else
            return QObject::tr("Ready but inactive.");
    } else {
        return QObject::tr("Not initialized.");
    }
}

QWidget* Qtilities::CoreGui::WidgetLoggerEngine::getWidget() {
    return d->widget;
}

void Qtilities::CoreGui::WidgetLoggerEngine::logMessage(const QString& message) {
    if (d->widget)
        d->widget->appendMessage(message);
}

void Qtilities::CoreGui::WidgetLoggerEngine::clearLog() {
    if (d->widget)
        d->widget->plainTextEdit()->clear();
}

QPlainTextEdit* Qtilities::CoreGui::WidgetLoggerEngine::plainTextEdit() const {
    return d->widget->plainTextEdit();
}

