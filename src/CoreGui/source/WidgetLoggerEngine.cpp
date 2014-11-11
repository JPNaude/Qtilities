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
    QPointer<WidgetLoggerEngineFrontend>            widget;
    WidgetLoggerEngine::MessageDisplaysFlag         message_displays_flag;
    Qt::ToolBarArea                                 toolbar_area;
};

Qtilities::CoreGui::WidgetLoggerEngine::WidgetLoggerEngine(MessageDisplaysFlag message_displays_flag,
                                                           Qt::ToolBarArea toolbar_area) : AbstractLoggerEngine()
{
    d = new WidgetLoggerEnginePrivateData;
    setName(QObject::tr("Widget Logger Engine"));
    d->message_displays_flag = message_displays_flag;
    d->toolbar_area = toolbar_area;
}

Qtilities::CoreGui::WidgetLoggerEngine::~WidgetLoggerEngine()
{
    if (d->widget)
        d->widget->deleteLater();
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
    d->widget = new WidgetLoggerEngineFrontend(d->message_displays_flag,
                                               d->toolbar_area);
    connect(d->widget,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));

    if (d->widget) {
        // Print startup info messages
        Q_ASSERT(abstractLoggerEngineData->formatting_engine);
        #ifndef QT_NO_DEBUG
        d->widget->appendMessage(objectName() + tr(" initialized successfully."));
        d->widget->appendMessage(tr("Log messages will be formatted using the following formatting engine: ") + abstractLoggerEngineData->formatting_engine->name());
        d->widget->appendMessage(" ");
        #endif
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

void Qtilities::CoreGui::WidgetLoggerEngine::logMessage(const QString& message, Logger::MessageType message_type) {
    if (d->widget)
        d->widget->appendMessage(message,message_type);
}

void Qtilities::CoreGui::WidgetLoggerEngine::clearLog() {
    if (d->widget)
        d->widget->clear();
}

void Qtilities::CoreGui::WidgetLoggerEngine::setLineWrapMode(QPlainTextEdit::LineWrapMode mode) {
    if (d->widget)
        d->widget->setLineWrapMode(mode);
}

QPlainTextEdit* Qtilities::CoreGui::WidgetLoggerEngine::plainTextEdit(MessageDisplaysFlag message_display) const {
    return d->widget->plainTextEdit(message_display);
}

