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

#include "AbstractLoggerEngine.h"

Qtilities::Logging::AbstractLoggerEngine::AbstractLoggerEngine() : QObject()
{
    abstractLoggerEngineData = new AbstractLoggerEngineData();
    abstractLoggerEngineData->formatting_engine = 0;
    abstractLoggerEngineData->is_enabled = true;

    connect(this,SIGNAL(destroyed()),SLOT(finalize()));

    installEventFilter(this);
    enableAllMessageTypes();
}

Qtilities::Logging::AbstractLoggerEngine::~AbstractLoggerEngine() {
    Log->detachLoggerEngine(this,false);
    delete abstractLoggerEngineData;
}

bool Qtilities::Logging::AbstractLoggerEngine::isInitialized() const {
    return abstractLoggerEngineData->is_initialized;
}

bool Qtilities::Logging::AbstractLoggerEngine::isActive() const {
    return abstractLoggerEngineData->is_enabled;
}

void Qtilities::Logging::AbstractLoggerEngine::setActive(bool is_active) {
    if (is_active == abstractLoggerEngineData->is_enabled)
        return;

    abstractLoggerEngineData->is_enabled = is_active;
}

void Qtilities::Logging::AbstractLoggerEngine::setName(const QString& name) {
    setObjectName(name);
    abstractLoggerEngineData->engine_name = name;
}

void Qtilities::Logging::AbstractLoggerEngine::setEnabledMessageTypes(Logger::MessageTypeFlags message_types) {
    abstractLoggerEngineData->enabled_message_types = message_types;
}

Qtilities::Logging::Logger::MessageTypeFlags Qtilities::Logging::AbstractLoggerEngine::getEnabledMessageTypes() const {
    return abstractLoggerEngineData->enabled_message_types;
}

void Qtilities::Logging::AbstractLoggerEngine::enableAllMessageTypes() {
    abstractLoggerEngineData->enabled_message_types = 0;
    abstractLoggerEngineData->enabled_message_types |= Logger::Info;
    abstractLoggerEngineData->enabled_message_types |= Logger::Warning;
    abstractLoggerEngineData->enabled_message_types |= Logger::Error;
    abstractLoggerEngineData->enabled_message_types |= Logger::Fatal;
    abstractLoggerEngineData->enabled_message_types |= Logger::Debug;
    abstractLoggerEngineData->enabled_message_types |= Logger::Trace;
}

void Qtilities::Logging::AbstractLoggerEngine::installFormattingEngine(AbstractFormattingEngine* engine) {
    if (engine == abstractLoggerEngineData->formatting_engine)
        return;

    if (!isFormattingEngineConstant() && abstractLoggerEngineData->formatting_engine && engine) {
        abstractLoggerEngineData->formatting_engine = engine;
        #ifndef QT_NO_DEBUG
        if (abstractLoggerEngineData->is_enabled) {
            logMessage("Formatting engine change detected.",Logger::Trace);
            logMessage(QString("This engine now logs messages using the following formatting engine: %1").arg(abstractLoggerEngineData->formatting_engine->name()),Logger::Trace);
        }
        #endif
    } else if (!abstractLoggerEngineData->formatting_engine && engine)
        abstractLoggerEngineData->formatting_engine = engine;
}

Qtilities::Logging::AbstractFormattingEngine* Qtilities::Logging::AbstractLoggerEngine::getInstalledFormattingEngine() {
    return abstractLoggerEngineData->formatting_engine;
}

QString Qtilities::Logging::AbstractLoggerEngine::formattingEngineName() {
    if (abstractLoggerEngineData->formatting_engine)
        return abstractLoggerEngineData->formatting_engine->objectName();
    else
        return "None";
}

void Qtilities::Logging::AbstractLoggerEngine::newMessages(const QString& engine_name, Logger::MessageType message_type, Logger::MessageContextFlags message_context, const QList<QVariant>& messages) {
    if ((!engine_name.isEmpty()) && (engine_name != name()))
        return;

    // Check the message context:
    if (!(abstractLoggerEngineData->message_contexts & message_context))
        return;

    static QMutex mutex;
    mutex.lock();

    // Check if active
    if (abstractLoggerEngineData->is_enabled) {
        // Check if there is a formatting engine present
        if (abstractLoggerEngineData->formatting_engine) {
            //Check if this message type is allowed
            if (abstractLoggerEngineData->enabled_message_types & message_type)
                logMessage(abstractLoggerEngineData->formatting_engine->formatMessage(message_type,messages),message_type);
        }
    }

    mutex.unlock();
}

bool Qtilities::Logging::AbstractLoggerEngine::removable() const {
    return abstractLoggerEngineData->is_removable;
}

void Qtilities::Logging::AbstractLoggerEngine::setRemovable(bool is_removable) {
    abstractLoggerEngineData->is_removable = is_removable;
}
