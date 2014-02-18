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

#include "LoggerEngines.h"
#include "LoggingConstants.h"

#include <QFile>
#include <QTextStream>
#include <QList>
#include <QString>
#include <QMutex>

#include <stdio.h>

using namespace Qtilities::Logging;
using namespace Qtilities::Logging::Constants;
using namespace Qtilities::Logging::Interfaces;

// ------------------------------------
// FileLoggerEngine implementation
// ------------------------------------
namespace Qtilities {
    namespace Logging {
        LoggerFactoryItem<AbstractLoggerEngine, FileLoggerEngine> FileLoggerEngine::factory;
    }
}

Qtilities::Logging::FileLoggerEngine::FileLoggerEngine() : AbstractLoggerEngine()
{
    file_name = QString();
    abstractLoggerEngineData->formatting_engine = 0;
    setName("File Logger Engine");
}

Qtilities::Logging::FileLoggerEngine::~FileLoggerEngine()
{
    finalize();
}

bool Qtilities::Logging::FileLoggerEngine::initialize() {
    if (file_name.isEmpty()) {
        LOG_ERROR(QString("Failed to initialize file logger engine (%1): File name is empty...").arg(objectName()));
        return false;
    }

    if (!abstractLoggerEngineData->formatting_engine) {
        // Attempt to get the formatting engine with the specified file format.
        QFileInfo fi(file_name);
        QString extension = fi.fileName().split(".").last();
        AbstractFormattingEngine* formatting_engine_inst = Log->formattingEngineReferenceFromExtension(extension);
        if (!formatting_engine_inst) {
            // We assign a default formatting engine:
            abstractLoggerEngineData->formatting_engine = Log->formattingEngineReference(qti_def_FORMATTING_ENGINE_DEFAULT);
            LOG_DEBUG(QString("Assigning default formatting engine to file logger engine (%1).").arg(objectName()));
        } else {
            abstractLoggerEngineData->formatting_engine = formatting_engine_inst;
        }
    }

    QFileInfo fi(file_name);
    QDir dir(fi.path());
    if (!dir.exists()) {
        dir.mkpath(fi.path());
    }

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to initialize file logger engine (%1): Can't open the specified file (%2) for writing...").arg(objectName()).arg(file_name));
        return false;
    }

    QTextStream out(&file);
    out << abstractLoggerEngineData->formatting_engine->initializeString() << "\n";
    file.close();

    abstractLoggerEngineData->is_initialized = true;
    return true;
}

void Qtilities::Logging::FileLoggerEngine::finalize() {
    if (abstractLoggerEngineData->is_initialized) {
        QFile file(file_name);
        if (!file.exists())
            return;

        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;

        QTextStream out(&file);
        if (abstractLoggerEngineData->formatting_engine)
            out << abstractLoggerEngineData->formatting_engine->finalizeString();
        out << "\n";
        file.close();
    }
}

QString Qtilities::Logging::FileLoggerEngine::description() const {
    return "Writes log messages to a file.";
}

QString Qtilities::Logging::FileLoggerEngine::status() const {
    if (abstractLoggerEngineData->is_initialized) {
        if (abstractLoggerEngineData->is_enabled)
            return QString("Logging in progress to output file: %1").arg(file_name);
        else
            return "Ready but inactive.";
    } else
        return "Not initialized.";
}

void Qtilities::Logging::FileLoggerEngine::clearLog() {
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to clear file logger engine:" << file_name;
        return;
    }
    file.close();
}

void Qtilities::Logging::FileLoggerEngine::logMessage(const QString& message, Logger::MessageType message_type) {
    Q_UNUSED(message_type)

    if (!abstractLoggerEngineData->is_initialized)
        return;

    QFile file(file_name);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << message << "\n";
    file.close();
}


Qtilities::Logging::Interfaces::ILoggerExportable::ExportModeFlags Qtilities::Logging::FileLoggerEngine::supportedFormats() const {
    ILoggerExportable::ExportModeFlags flags = 0;
    flags |= ILoggerExportable::Binary;
    return flags;
}

bool Qtilities::Logging::FileLoggerEngine::exportBinary(QDataStream& stream) const {
    stream << file_name;
    return true;
}

bool Qtilities::Logging::FileLoggerEngine::importBinary(QDataStream& stream) {
    stream >> file_name;
    return true;
}

void Qtilities::Logging::FileLoggerEngine::setFileName(const QString& fileName) {
    if (!abstractLoggerEngineData->is_initialized)
        file_name = fileName;
}

QString Qtilities::Logging::FileLoggerEngine::getFileName() {
    return file_name;
}

// ------------------------------------
// QtMsgLoggerEngine implementation
// ------------------------------------

Qtilities::Logging::QtMsgLoggerEngine* Qtilities::Logging::QtMsgLoggerEngine::m_Instance = 0;

Qtilities::Logging::QtMsgLoggerEngine* Qtilities::Logging::QtMsgLoggerEngine::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new QtMsgLoggerEngine;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::Logging::QtMsgLoggerEngine::QtMsgLoggerEngine() : AbstractLoggerEngine() {
    setName("Qt Message Logger Engine");
}

Qtilities::Logging::QtMsgLoggerEngine::~QtMsgLoggerEngine() {}

bool Qtilities::Logging::QtMsgLoggerEngine::initialize() {
    abstractLoggerEngineData->is_initialized = true;
    return true;
}

void Qtilities::Logging::QtMsgLoggerEngine::finalize() {
    abstractLoggerEngineData->is_initialized = false;
}

QString Qtilities::Logging::QtMsgLoggerEngine::description() const {
    return "Writes log messages to the Qt message system.";
}

QString Qtilities::Logging::QtMsgLoggerEngine::status() const {
    if (abstractLoggerEngineData->is_initialized) {
        if (abstractLoggerEngineData->is_enabled)
            return "Logging in progress...";
        else
            return "Ready but inactive.";
    } else {
        return "Not initialized.";
    }
}

void Qtilities::Logging::QtMsgLoggerEngine::logMessage(const QString& message, Logger::MessageType message_type) {
    Q_UNUSED(message_type)
    qDebug() << message;
}

// ------------------------------------
// ConsoleLoggerEngine implementation
// ------------------------------------

Qtilities::Logging::ConsoleLoggerEngine* Qtilities::Logging::ConsoleLoggerEngine::m_Instance = 0;

Qtilities::Logging::ConsoleLoggerEngine* Qtilities::Logging::ConsoleLoggerEngine::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new ConsoleLoggerEngine;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::Logging::ConsoleLoggerEngine::ConsoleLoggerEngine() : AbstractLoggerEngine() {
    setName("Console Logger Engine");
    color_formatting_enabled = true;

    message_colors[Logger::Warning] = QString(CONSOLE_BLUE);
    message_colors[Logger::Error] = QString(CONSOLE_RED);
    message_colors[Logger::Fatal] = QString(CONSOLE_RED);
}

void ConsoleLoggerEngine::setConsoleFormattingEnabled(bool is_enabled) {
    color_formatting_enabled = is_enabled;
}

bool ConsoleLoggerEngine::consoleFormattingEnabled() const {
    return color_formatting_enabled;
}

void ConsoleLoggerEngine::setConsoleFormattingHint(Logger::MessageType message_type, QString hint_color) {
    message_colors[message_type] = hint_color;
}

Qtilities::Logging::ConsoleLoggerEngine::~ConsoleLoggerEngine() {

}

bool Qtilities::Logging::ConsoleLoggerEngine::initialize() {
    abstractLoggerEngineData->is_initialized = true;
    return true;
}

void Qtilities::Logging::ConsoleLoggerEngine::finalize() {
    abstractLoggerEngineData->is_initialized = false;
}

QString Qtilities::Logging::ConsoleLoggerEngine::description() const {
    return "Writes log messages to the console.";
}

QString Qtilities::Logging::ConsoleLoggerEngine::status() const {
    if (abstractLoggerEngineData->is_initialized) {
        if (abstractLoggerEngineData->is_enabled)
            return "Logging in progress...";
        else
            return "Ready but inactive.";
    } else
        return "Not initialized.";
}

void ConsoleLoggerEngine::resetConsoleEscapeCodes() {
    #ifndef Q_OS_WIN
    fprintf(stdout, CONSOLE_RESET);
    #endif
}

void Qtilities::Logging::ConsoleLoggerEngine::logMessage(const QString& message, Logger::MessageType message_type) {
#ifdef Q_OS_WIN
    if (message_type == Logger::Error || message_type == Logger::Fatal)
        fprintf(stderr, "%s\n", qPrintable(message));
    else
        fprintf(stdout, "%s\n", qPrintable(message));
#else
    if (color_formatting_enabled) {
        // For more info see: //http://en.wikipedia.org/wiki/ANSI_escape_code
        if (message_type == Logger::Info) {
            if (message_colors.contains(Logger::Info))
                fprintf(stdout, qPrintable(QString("%1%s\n%2").arg(message_colors[Logger::Info]).arg(CONSOLE_RESET)), qPrintable(message));
            else
                fprintf(stdout, qPrintable(QString("%1%s\n").arg(CONSOLE_RESET)), qPrintable(message));
        } else if (message_type == Logger::Warning) {
            fprintf(stdout, qPrintable(QString("%1%s\n%2").arg(message_colors[Logger::Warning]).arg(CONSOLE_RESET)), qPrintable(message));
        } else if (message_type == Logger::Error) {
            fprintf(stderr, qPrintable(QString("%1%s\n%2").arg(message_colors[Logger::Error]).arg(CONSOLE_RESET)), qPrintable(message));
        } else if (message_type == Logger::Fatal) {
            fprintf(stderr, qPrintable(QString("%1%s\n%2").arg(message_colors[Logger::Fatal]).arg(CONSOLE_RESET)), qPrintable(message));
        } else
            fprintf(stdout, "%s\n", qPrintable(message));
    } else
        fprintf(stdout, "%s\n", qPrintable(message));
#endif
}
