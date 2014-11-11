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

#include "Logger.h"
#include "LoggerFactory.h"
#include "AbstractFormattingEngine.h"
#include "AbstractLoggerEngine.h"
#include "FormattingEngines.h"
#include "LoggerEngines.h"
#include "LoggingConstants.h"

#include <Qtilities.h>

#include <QtDebug>
#include <QMutex>

using namespace Qtilities::Logging::Constants;

struct Qtilities::Logging::LoggerPrivateData {
    LoggerFactory<AbstractLoggerEngine>         logger_engine_factory;
    QList<QPointer<AbstractLoggerEngine> >      logger_engines;
    QList<QPointer<AbstractFormattingEngine> >  formatting_engines;
    QString                                     default_formatting_engine;
    Logger::MessageType                         global_log_level;
    bool                                        initialized;
    bool                                        is_qt_message_handler;
    bool                                        remember_session_config;
    QPointer<AbstractFormattingEngine>          priority_formatting_engine;
    QString                                     session_path;
    bool                                        settings_enabled;
};

Qtilities::Logging::Logger* Qtilities::Logging::Logger::m_Instance = 0;

Qtilities::Logging::Logger* Qtilities::Logging::Logger::instance() {
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new Logger;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::Logging::Logger::Logger(QObject* parent) : QObject(parent) {
    d = new LoggerPrivateData;

    d->default_formatting_engine = QString("Uninitialized");
    d->global_log_level = Logger::Debug;
    d->initialized = false;
    d->remember_session_config = false;
    d->priority_formatting_engine = 0;
    d->session_path = QCoreApplication::applicationDirPath() + qti_def_PATH_SESSION;
    d->settings_enabled = true;

    qRegisterMetaType<Logger::MessageType>("Logger::MessageType");
    qRegisterMetaType<Logger::MessageContextFlags>("Logger::MessageContextFlags");
}

Qtilities::Logging::Logger::~Logger() {
    clear();
    delete d;
}

void Qtilities::Logging::Logger::initialize(const QString& configuration_file_name) {
    if (d->initialized)
        return;

    // In the initialize function we use the Qt Debug logging system to log messages since no logger engines would be present at this stage.
    // qDebug() << tr("Qtilities Logging Framework, initialization started...");

    // Register the formatting engines that comes as part of the Qtilities Logging Framework
    d->formatting_engines << FormattingEngine_Default::instance();
    d->formatting_engines << FormattingEngine_Rich_Text::instance();
    d->formatting_engines << FormattingEngine_XML::instance();
    d->formatting_engines << FormattingEngine_HTML::instance();
    d->formatting_engines << FormattingEngine_QtMsgEngineFormat::instance();
    d->default_formatting_engine = QString(qti_def_FORMATTING_ENGINE_DEFAULT);

    // Register the logger enigines that comes as part of the Qtilities Logging Framework
    d->logger_engine_factory.registerFactoryInterface(qti_def_FACTORY_TAG_FILE_LOGGER_ENGINE, &FileLoggerEngine::factory);

    //qDebug() << tr("> Number of formatting engines available: ") << d->formatting_engines.count();
    //qDebug() << tr("> Number of logger engine factories available: ") << d->logger_engine_factory.tags().count();

    // Attach a QtMsgLoggerEngine and a ConsoleLoggerEngine and disable them both.
    AbstractLoggerEngine* tmp_engine_ptr = QtMsgLoggerEngine::instance();
    tmp_engine_ptr->installFormattingEngine(FormattingEngine_QtMsgEngineFormat::instance());
    attachLoggerEngine(tmp_engine_ptr, true);
    toggleQtMsgEngine(false);

    tmp_engine_ptr = qobject_cast<AbstractLoggerEngine*> (ConsoleLoggerEngine::instance());
    tmp_engine_ptr->installFormattingEngine(FormattingEngine_Default::instance());
    attachLoggerEngine(tmp_engine_ptr, true);
    toggleConsoleEngine(false);

    readSettings();

    // Now load the logger config if neccesarry.
    if (d->remember_session_config)
        loadSessionConfig(configuration_file_name);

    d->initialized = true;
    // qDebug() << tr("Qtilities Logging Framework, initialization finished successfully...");
}

void Qtilities::Logging::Logger::finalize(const QString &configuration_file_name) {
    if (d->remember_session_config) {
        saveSessionConfig(configuration_file_name);
    }

    clear();
}

void Qtilities::Logging::Logger::clear() {
    // Delete all logger engines
    //qDebug() << tr("Qtilities Logging Framework, clearing started...");
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i)) {
            if (d->logger_engines.at(i) != QtMsgLoggerEngine::instance() && d->logger_engines.at(i) != ConsoleLoggerEngine::instance()) {
                //qDebug() << tr("> Deleting logger engine: ") << d->logger_engines.at(i)->objectName();
                delete d->logger_engines.at(i);
            }
        }

    }
    d->logger_engines.clear();
    //qDebug() << tr("Qtilities Logging Framework, clearing finished successfully...");
}

void Qtilities::Logging::Logger::logMessage(const QString& engine_name, MessageType message_type, const QVariant& message, const QVariant &msg1, const QVariant &msg2, const QVariant &msg3, const QVariant &msg4, const QVariant &msg5, const QVariant &msg6, const QVariant &msg7, const QVariant &msg8 , const QVariant &msg9) {
    // In release mode we should not log debug and trace messages.
    #ifdef QT_NO_DEBUG
        if (message_type == Debug || message_type == Trace)
            return;
    #endif

    if (message_type == AllLogLevels || message_type == None)
        return;

    if (message_type > d->global_log_level)
        return;

    QList<QVariant> message_contents;
    message_contents.push_back(message);
    if (!msg1.isNull()) message_contents.push_back(msg1);
    if (!msg2.isNull()) message_contents.push_back(msg2);
    if (!msg3.isNull()) message_contents.push_back(msg3);
    if (!msg4.isNull()) message_contents.push_back(msg4);
    if (!msg5.isNull()) message_contents.push_back(msg5);
    if (!msg6.isNull()) message_contents.push_back(msg6);
    if (!msg7.isNull()) message_contents.push_back(msg7);
    if (!msg8.isNull()) message_contents.push_back(msg8);
    if (!msg9.isNull()) message_contents.push_back(msg9);

//    if (message.toString().contains("Argument missing"))
//        int i = 5;
//    if (message.toString() == "QFile::remove: Empty or null file name")
//        int i = 5;
//    else if (message.toString() == "QFile::seek: IODevice is not open")
//        int i = 5;
//    else if (message.toString().contains("&other != this"))
//        int i = 5;
//    else if (message.toString().contains("QFontDatabase"))
//        int i = 5;
//    else if (message.toString().contains("index out of range"))
//        int i = 5;
//    else if (message.toString().contains("Cannot send events to objects owned"))
//        int i = 5;
//    else if (message.toString().contains("abort()"))
//        int i = 5;
//    else if (message.toString().contains("Thread: Destroyed while thread is still running"))
//        int i = 5;
//    else if (message.toString().contains("QObject: Cannot create children for a parent that is in a different thread"))
//        int i = 5;
//    else if (message.toString().contains("QFSFileEngine::open: No file name specified"))
//        int i = 5;
//    else if (message.toString().contains("QFont::setPixelSize: Pixel size"))
//        int i = 5;

    // Create the correct message context:
    MessageContextFlags context = 0;
    if (engine_name.isEmpty())
        context |= SystemWideMessages;
    else
        context |= EngineSpecificMessages;

    emit newMessage(engine_name,message_type,context,message_contents);
}

void Qtilities::Logging::Logger::logPriorityMessage(const QString& engine_name, MessageType message_type, const QVariant& message, const QVariant &msg1, const QVariant &msg2, const QVariant &msg3, const QVariant &msg4, const QVariant &msg5, const QVariant &msg6, const QVariant &msg7, const QVariant &msg8 , const QVariant &msg9) {
    // In release mode we should not log debug and trace messages.
    #ifdef QT_NO_DEBUG
        if (message_type == Debug || message_type == Trace)
            return;
    #endif

    if (message_type == AllLogLevels || message_type == None)
        return;

    if (message_type > d->global_log_level)
        return;

    QList<QVariant> message_contents;
    message_contents.push_back(message);
    if (!msg1.isNull()) message_contents.push_back(msg1);
    if (!msg2.isNull()) message_contents.push_back(msg2);
    if (!msg3.isNull()) message_contents.push_back(msg3);
    if (!msg4.isNull()) message_contents.push_back(msg4);
    if (!msg5.isNull()) message_contents.push_back(msg5);
    if (!msg6.isNull()) message_contents.push_back(msg6);
    if (!msg7.isNull()) message_contents.push_back(msg7);
    if (!msg8.isNull()) message_contents.push_back(msg8);
    if (!msg9.isNull()) message_contents.push_back(msg9);

    // Create the correct message context:
    MessageContextFlags context = 0;
    context |= PriorityMessages;

    emit newMessage(engine_name,message_type,context,message_contents);

    QString formatted_message;
    if (d->priority_formatting_engine) {
        formatted_message = d->priority_formatting_engine->formatMessage(message_type,message_contents);
    } else
        formatted_message = message.toString();

    emit newPriorityMessage(message_type,formatted_message);
}

bool Qtilities::Logging::Logger::setPriorityFormattingEngine(const QString& name) {
    if (!availableLoggerEnginesInFactory().contains(name))
        return false;

    if (d->priority_formatting_engine)
        delete d->priority_formatting_engine;

    d->priority_formatting_engine = formattingEngineReference(name);
    return true;
}

void Qtilities::Logging::Logger::setPriorityFormattingEngine(AbstractFormattingEngine* engine) {
    if (engine)
        d->priority_formatting_engine = engine;
}

QStringList Qtilities::Logging::Logger::availableFormattingEnginesInFactory() const {
    QStringList names;
    for (int i = 0; i < d->formatting_engines.count(); ++i) {
        names << d->formatting_engines.at(i)->name();
    }
    return names;
}

Qtilities::Logging::AbstractFormattingEngine* Qtilities::Logging::Logger::formattingEngineReference(const QString& name) {
    for (int i = 0; i < d->formatting_engines.count(); ++i) {
        if (name == d->formatting_engines.at(i)->name())
            return d->formatting_engines.at(i);
    }
    return 0;
}

void Qtilities::Logging::Logger::registerFormattingEngine(Qtilities::Logging::AbstractFormattingEngine *formatting_engine) {
    if (formatting_engine)
        d->formatting_engines << formatting_engine;
}

Qtilities::Logging::AbstractFormattingEngine* Qtilities::Logging::Logger::formattingEngineReferenceFromExtension(const QString& file_extension) {
    AbstractFormattingEngine* engine = 0;
    uint highest_priority = 0;
    for (int i = 0; i < d->formatting_engines.count(); ++i) {
        if (file_extension == d->formatting_engines.at(i)->fileExtension()) {
            if (d->formatting_engines.at(i)->priority() > highest_priority) {
                highest_priority = d->formatting_engines.at(i)->priority();
                engine = d->formatting_engines.at(i);
            }
        }
    }

    return engine;
}

Qtilities::Logging::AbstractFormattingEngine* Qtilities::Logging::Logger::formattingEngineReferenceAt(int index) {
    if (index < 0 || index >= d->formatting_engines.count())
        return 0;

    return d->formatting_engines.at(index);
}

Qtilities::Logging::AbstractLoggerEngine* Qtilities::Logging::Logger::newLoggerEngine(QString tag, AbstractFormattingEngine* formatting_engine) {
    // Check that the name is unique:
    QString engine_name = tag;
    int count = 0;
    engine_name.append(QString("_%1").arg(count));
    while (attachedLoggerEngineNames().contains(engine_name)) {
        QString count_string = QString("%1").arg(count);
        engine_name.chop(count_string.length());
        ++count;
        engine_name.append(QString("%1").arg(count));
    }

    AbstractLoggerEngine* new_engine = d->logger_engine_factory.createInstance(tag);
    if (new_engine)
        new_engine->setName(engine_name);
    else
        return 0;

    // Install a formatting engine for the new logger engine
    if (formatting_engine)
        new_engine->installFormattingEngine(formatting_engine);

    return new_engine;
}

void Qtilities::Logging::Logger::registerLoggerEngineFactory(const QString& tag, LoggerFactoryInterface<AbstractLoggerEngine>* factory_iface) {
    d->logger_engine_factory.registerFactoryInterface(tag, factory_iface);
}

QStringList Qtilities::Logging::Logger::availableLoggerEnginesInFactory() const {
    return d->logger_engine_factory.tags();
}

int Qtilities::Logging::Logger::attachedFormattingEngineCount() const {
    return d->formatting_engines.count();
}

QString Qtilities::Logging::Logger::defaultFormattingEngine() const {
    return d->default_formatting_engine;
}

bool Qtilities::Logging::Logger::attachLoggerEngine(AbstractLoggerEngine* new_logger_engine, bool initialize_engine) {
    if (!new_logger_engine)
        return false;

    // Check that the name is unique:
    if (attachedLoggerEngineNames().contains(new_logger_engine->name())) {
        qDebug() << QObject::tr("Attempting to attach logger engines with duplicate names, this is not allowed. Name: ") << new_logger_engine->name();
        return false;
    }

    if (initialize_engine) {
        bool init_result = new_logger_engine->initialize();
        if (!init_result) {
            LOG_ERROR(tr("New file logger engine could not be added, it failed during initialization."));
            delete new_logger_engine;
            new_logger_engine = 0;
            return false;
        }
    }

    if (new_logger_engine) {
        new_logger_engine->setObjectName(new_logger_engine->name());
        d->logger_engines << new_logger_engine;
        connect(this,SIGNAL(newMessage(QString,Logger::MessageType,Logger::MessageContextFlags,QList<QVariant>)),new_logger_engine,SLOT(newMessages(QString,Logger::MessageType,Logger::MessageContextFlags,QList<QVariant>)));
    }

    emit loggerEngineCountChanged(new_logger_engine, EngineAdded);
    return true;
}

bool Qtilities::Logging::Logger::detachLoggerEngine(AbstractLoggerEngine* logger_engine, bool delete_engine) {
    if (logger_engine) {
        if (d->logger_engines.removeOne(logger_engine)) {
            emit loggerEngineCountChanged(logger_engine, EngineRemoved);
            if (delete_engine)
                delete logger_engine;
            return true;
        }
    }

    return false;
}

QString Qtilities::Logging::Logger::logLevelToString(Logger::MessageType log_level) const {
    if (log_level == None) {
        return "None";
    } else if (log_level == Info) {
        return "Info";
    } else if (log_level == Warning) {
        return "Warning";
    } else if (log_level == Error) {
        return "Error";
    } else if (log_level == Fatal) {
        return "Fatal";
    } else if (log_level == Debug) {
        return "Debug";
    } else if (log_level == Trace) {
        return "Trace";
    } else if (log_level == AllLogLevels) {
        return "All Log Levels";
    }

    return QString();
}

Qtilities::Logging::Logger::MessageType Qtilities::Logging::Logger::stringToLogLevel(const QString& log_level_string) const {
    if (log_level_string == QLatin1String("Info")) {
        return Logger::Info;
    } else if (log_level_string == QLatin1String("Warning")) {
        return Logger::Warning;
    } else if (log_level_string == QLatin1String("Error")) {
        return Logger::Error;
    } else if (log_level_string == QLatin1String("Fatal")) {
        return Logger::Fatal;
    } else if (log_level_string == QLatin1String("Debug")) {
        return Logger::Debug;
    } else if (log_level_string == QLatin1String("Trace")) {
        return Logger::Trace;
    } else if (log_level_string == QLatin1String("All Log Levels")) {
        return Logger::AllLogLevels;
    }
    return Logger::None;
}

QStringList Qtilities::Logging::Logger::allLogLevelStrings() const {
    QStringList strings;
    strings << "None";
    strings << "Information";
    strings << "Warning";
    strings << "Error";
    strings << "Fatal";

    #ifndef QT_NO_DEBUG
        strings << "Debug";
        strings << "Trace";
    #endif
    strings << "All Log Levels";
    return strings;
}

QString Qtilities::Logging::Logger::messageContextsToString(Logger::MessageContextFlags message_contexts) const {
    QString context_string;
    if (message_contexts & SystemWideMessages)
        context_string.append("System,");
    if (message_contexts & PriorityMessages)
        context_string.append("Priority,");
    if (message_contexts & EngineSpecificMessages)
        context_string.append("Engine");

    if (context_string.isEmpty())
        context_string.append("None");

    if (context_string.endsWith(","))
        context_string.remove(context_string.length()-1,1);

    return context_string;
}

Qtilities::Logging::Logger::MessageContextFlags Qtilities::Logging::Logger::stringToMessageContexts(const QString& message_contexts_string) const {
    MessageContextFlags flags = 0;
    if (message_contexts_string.contains("System"))
        flags |= SystemWideMessages;
    if (message_contexts_string.contains("Priority"))
        flags |= PriorityMessages;
    if (message_contexts_string.contains("Engine"))
        flags |= EngineSpecificMessages;

    return flags;
}

QStringList Qtilities::Logging::Logger::allMessageContextStrings() const {
    QStringList strings;
    strings << "None";
    strings << "System";
    strings << "Priority";
    strings << "Engine";
    return strings;
}

void Qtilities::Logging::Logger::deleteAllLoggerEngines() {
    // Delete all logger engines
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i))
            delete d->logger_engines.at(i);
    }
    d->logger_engines.clear();
}

void Qtilities::Logging::Logger::disableAllLoggerEngines() {
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i))
            d->logger_engines.at(i)->setActive(false);
    }
}

void Qtilities::Logging::Logger::enableAllLoggerEngines() {
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i))
            d->logger_engines.at(i)->setActive(true);
    }
}

void Qtilities::Logging::Logger::deleteEngine(const QString& engine_name) {
    AbstractLoggerEngine* engine = loggerEngineReference(engine_name);

    if (!engine)
        return;
//    else
//        engine->finalize();

    delete engine;
}

void Qtilities::Logging::Logger::enableEngine(const QString& engine_name) {
    AbstractLoggerEngine* engine = loggerEngineReference(engine_name);

    if (!engine)
        return;

    engine->setActive(true);
}

void Qtilities::Logging::Logger::disableEngine(const QString engine_name) {
    AbstractLoggerEngine* engine = loggerEngineReference(engine_name);

    if (!engine)
        return;

    engine->setActive(false);
}

QStringList Qtilities::Logging::Logger::attachedLoggerEngineNames() const {
    QStringList names;
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i))
            names << d->logger_engines.at(i)->name();
    }
    return names;
}

int Qtilities::Logging::Logger::attachedLoggerEngineCount() const {
    int count = 0;
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i))
            ++count;
    }
    return count;
}

Qtilities::Logging::AbstractLoggerEngine* Qtilities::Logging::Logger::loggerEngineReference(const QString& engine_name) {
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i)) {
            if (engine_name == d->logger_engines.at(i)->name())
                return d->logger_engines.at(i);
        }
    }
    return 0;
}

Qtilities::Logging::AbstractLoggerEngine *Qtilities::Logging::Logger::loggerEngineReferenceForFile(const QString &file_path) {
    if (file_path.isEmpty())
        return 0;

    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i)) {
            FileLoggerEngine* fe = qobject_cast<FileLoggerEngine*> (d->logger_engines.at(i));
            if (fe) {
                bool is_match = false;
                QFileInfo fi1(fe->getFileName());
                QFileInfo fi2(file_path);
                if (fi1.exists() && fi2.exists())
                    is_match = (fi1 == fi2);
                else {
                    QString cleaned_1 = QDir::cleanPath(fe->getFileName());
                    QString cleaned_2 = QDir::cleanPath(file_path);
                    if (cleaned_1.size() == cleaned_2.size()) {
                        #ifdef Q_OS_WIN
                        is_match = (QDir::toNativeSeparators(cleaned_1).compare(QDir::toNativeSeparators(cleaned_2),Qt::CaseInsensitive) == 0);
                        #else
                        is_match = (QDir::toNativeSeparators(cleaned_1).compare(QDir::toNativeSeparators(cleaned_2),Qt::CaseSensitive) == 0);
                        #endif
                    } else
                        is_match = false;
                }

                if (is_match)
                    return fe;
            }
        }
    }

    return 0;
}

Qtilities::Logging::AbstractLoggerEngine* Qtilities::Logging::Logger::loggerEngineReferenceAt(int index) {
    int valid_engine_count = 0;
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i)) {
            if (valid_engine_count == index)
                return d->logger_engines.at(i);
            else
                ++valid_engine_count;
        }
    }
    return 0;
}

void Qtilities::Logging::Logger::setGlobalLogLevel(Logger::MessageType new_log_level) {
    if (d->global_log_level == new_log_level)
        return;

    d->global_log_level = new_log_level;

    writeSettings();
    LOG_INFO("Global log level changed to " + logLevelToString(new_log_level));
}

Qtilities::Logging::Logger::MessageType Qtilities::Logging::Logger::globalLogLevel() const {
    return d->global_log_level;
}

void Qtilities::Logging::Logger::writeSettings() const {
    if (!d->settings_enabled)
        return;

    // Store settings using QSettings only if it was initialized
    QSettings settings(d->session_path + QDir::separator() + "qtilities.ini",QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Logging");
    settings.beginGroup("General");
    settings.setValue("global_log_level", QVariant(d->global_log_level));
    settings.setValue("is_qt_message_handler", d->is_qt_message_handler);
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::Logging::Logger::readSettings() {
    if (!d->settings_enabled)
        return;

    // Load logging paramaters using QSettings()
    QSettings settings(d->session_path + QDir::separator() + "qtilities.ini",QSettings::IniFormat);
        settings.beginGroup("Qtilities");
    settings.beginGroup("Logging");
    settings.beginGroup("General");
    QVariant log_level =  settings.value("global_log_level", Fatal);
    d->global_log_level = (MessageType) log_level.toInt();
    if (settings.value("is_qt_message_handler", false).toBool())
        installAsQtMessageHandler(false);
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::Logging::Logger::setRememberSessionConfig(bool remember) {
    if (d->remember_session_config == remember)
        return;

    d->remember_session_config = remember;
    writeSettings();
}

bool Qtilities::Logging::Logger::rememberSessionConfig() const {
    return d->remember_session_config;
}

void Qtilities::Logging::Logger::installAsQtMessageHandler(bool update_stored_settings) {
    #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        qInstallMsgHandler(installLoggerMessageHandler);
    #else
        qInstallMessageHandler(installLoggerMessageHandler);
    #endif

    d->is_qt_message_handler = true;
    if (update_stored_settings)
        writeSettings();

    LOG_DEBUG("Capturing of Qt debug system messages is now enabled.");
}

void Qtilities::Logging::Logger::uninstallAsQtMessageHandler() {   
    #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        qInstallMsgHandler(0);
    #else
        qInstallMessageHandler(0);
    #endif

    d->is_qt_message_handler = false;
    writeSettings();

    LOG_DEBUG("Capturing of Qt debug system messages is now disabled.");
}

bool Qtilities::Logging::Logger::isQtMessageHandler() const {
    return d->is_qt_message_handler;
}

void Qtilities::Logging::Logger::setIsQtMessageHandler(bool toggle) {
    d->is_qt_message_handler = toggle;
    writeSettings();

    if (toggle) {
        #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            qInstallMsgHandler(installLoggerMessageHandler);
        #else
            qInstallMessageHandler(installLoggerMessageHandler);
        #endif
        LOG_DEBUG("Capturing of Qt debug system messages is now enabled.");
    } else {
        #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            qInstallMsgHandler(0);
        #else
            qInstallMessageHandler(0);
        #endif
        LOG_DEBUG("Capturing of Qt debug system messages is now disabled.");
    }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void Qtilities::Logging::installLoggerMessageHandler(QtMsgType type, const char *msg)
{
    static QMutex msgMutex;
    if (!msgMutex.tryLock())
        return;

    switch (type)
    {
    case QtDebugMsg:
        Log->logMessage(QString(),Logger::Debug, msg);
        break;
    case QtWarningMsg:
        Log->logMessage(QString(),Logger::Warning, msg);
        break;
    case QtCriticalMsg:
        Log->logMessage(QString(),Logger::Error, msg);
        break;
    case QtFatalMsg:
        Log->logMessage(QString(),Logger::Fatal, msg);
        msgMutex.unlock();
        abort();
    }

    msgMutex.unlock();
}
#else
void Qtilities::Logging::installLoggerMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex msgMutex;
    if (!msgMutex.tryLock())
        return;

    QString detailed_msg = QString("%1 (%2:%3. %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    switch (type)
    {
    case QtDebugMsg:
        Log->logMessage(QString(),Logger::Debug, detailed_msg);
        break;
    case QtWarningMsg:
        Log->logMessage(QString(),Logger::Warning, detailed_msg);
        break;
    case QtCriticalMsg:
        Log->logMessage(QString(),Logger::Error, detailed_msg);
        break;
    case QtFatalMsg:
        Log->logMessage(QString(),Logger::Fatal, detailed_msg);
        msgMutex.unlock();
        abort();
    }

    msgMutex.unlock();
}
#endif

// ------------------------------------
// Convenience functions provided to create new engines
// ------------------------------------
Qtilities::Logging::AbstractLoggerEngine* Qtilities::Logging::Logger::newFileEngine(const QString& engine_name, const QString& file_name, const QString& formatting_engine) {
    if (file_name.isEmpty())
        return 0;

    // Check that the name is unique:
    if (attachedLoggerEngineNames().contains(engine_name)) {
        qDebug() << QObject::tr("Attempting to attach logger engines with duplicate names, this is not allowed. Name: ") << engine_name;
        return 0;
    }

    QPointer<FileLoggerEngine> file_engine;
    QPointer<AbstractLoggerEngine> new_engine = d->logger_engine_factory.createInstance(qti_def_FACTORY_TAG_FILE_LOGGER_ENGINE);
    Q_ASSERT(new_engine);
    new_engine->setName(engine_name);

    file_engine = qobject_cast<FileLoggerEngine*> (new_engine);
    file_engine->setFileName(file_name);

    // Install a formatting engine for the new logger engine
    AbstractFormattingEngine* formatting_engine_inst = formattingEngineReference(formatting_engine);
    if (formatting_engine_inst)
        new_engine->installFormattingEngine(formatting_engine_inst);
    else {
        // Attempt to get the formatting engine with the specified file format.
        QFileInfo fi(file_name);
        AbstractFormattingEngine* formatting_engine_inst = formattingEngineReferenceFromExtension(fi.completeSuffix());
        if (!formatting_engine_inst) {
            if (!formatting_engine_inst) {
                // In this case, revert back to the default engine and log warning:
                LOG_WARNING(QString("Failed to find formatting engine \"%1\", or default engine for file extension \"%2\". Using default formatting engine for log file \"%3\"").arg(formatting_engine).arg(fi.completeSuffix()).arg(file_name));
                formatting_engine_inst = FormattingEngine_Default::instance();
            }
        }
        new_engine->installFormattingEngine(formatting_engine_inst);
    }

    if (attachLoggerEngine(new_engine, true)) {
        return new_engine;
    } else {
        delete new_engine;
        return 0;
    }
}

void Qtilities::Logging::Logger::toggleQtMsgEngine(bool toggle) {
    if (d->logger_engines.contains(QtMsgLoggerEngine::instance()))
        QtMsgLoggerEngine::instance()->setActive(toggle);
}

bool Qtilities::Logging::Logger::qtMsgEngineActive() const {
    if (d->logger_engines.contains(QtMsgLoggerEngine::instance()))
        return QtMsgLoggerEngine::instance()->isActive();
    else
        return false;
}

void Qtilities::Logging::Logger::toggleConsoleEngine(bool toggle) {
    if (d->logger_engines.contains(ConsoleLoggerEngine::instance()))
        ConsoleLoggerEngine::instance()->setActive(toggle);
}

bool Qtilities::Logging::Logger::consoleEngineActive() const {
    if (d->logger_engines.contains(ConsoleLoggerEngine::instance()))
        return ConsoleLoggerEngine::instance()->isActive();
    else
        return false;
}

void Qtilities::Logging::Logger::setLoggerSessionConfigPath(const QString path) {
    d->session_path = path;
}

void Qtilities::Logging::Logger::setLoggerSettingsEnabled(bool is_enabled) {
    d->settings_enabled = is_enabled;
}

bool Qtilities::Logging::Logger::loggerSettingsEnabled() const {
    return d->settings_enabled;
}

quint32 MARKER_LOGGER_CONFIG_TAG = 0xFAC0000F;

bool Qtilities::Logging::Logger::saveSessionConfig(QString file_name, Qtilities::ExportVersion version) const {
    if (!d->settings_enabled)
        return false;

    if (file_name.isEmpty())
        file_name = d->session_path + QDir::separator() + qti_def_PATH_LOGCONFIG_FILE;

    LOG_DEBUG(tr("Logging configuration export started to ") + file_name);

    QFileInfo fi(file_name);
    if (!fi.dir().exists())
        fi.dir().mkpath(fi.path());

    // Check if the directory exists:
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly)) {
        LOG_DEBUG(tr("Logging configuration export failed to ") + file_name + tr(". The file could not be opened in WriteOnly mode."));
        return false;
    }
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_4_7);
    stream << MARKER_LOGGER_CONFIG_TAG;
    stream << (quint32) version;

    // Stream exportable engines:
    QList<ILoggerExportable*> export_list;
    for (int i = 0; i < d->logger_engines.count(); ++i) {
        if (d->logger_engines.at(i)) {
            ILoggerExportable* log_export_iface = qobject_cast<ILoggerExportable*> (d->logger_engines.at(i));
            if (log_export_iface)
                export_list << log_export_iface;
        }
    }

    stream << MARKER_LOGGER_CONFIG_TAG;
    stream << (quint32) d->global_log_level;
    stream << (quint32) export_list.count();

    bool success = true;
    for (int i = 0; i < export_list.count(); ++i) {
        if (success) {
            LOG_DEBUG(tr("Exporting logger factory instance: ") + export_list.at(i)->factoryTag());
            stream << export_list.at(i)->factoryTag();
            stream << export_list.at(i)->instanceName();
            success = export_list.at(i)->exportBinary(stream);
        } else
            break;
    }

    stream << MARKER_LOGGER_CONFIG_TAG;

    // Stream activity and formatting engines of all current engines:
    if (success) {
        stream << (quint32) d->logger_engines.count();
        for (int i = 0; i < d->logger_engines.count(); ++i) {
            if (d->logger_engines.at(i)) {
                LOG_DEBUG(tr("Saving configuration for logger engine: ") + d->logger_engines.at(i)->name());
                stream << d->logger_engines.at(i)->name();
                stream << d->logger_engines.at(i)->formattingEngineName();
                stream << d->logger_engines.at(i)->isActive();
                stream << (quint32) d->logger_engines.at(i)->messageContexts();
            }
        }
    }

    // End properly:
    if (success) {
        stream << MARKER_LOGGER_CONFIG_TAG;
        file.close();
        LOG_INFO(tr("Successfully exported logging configuration exported to ") + file_name);
        return true;
    } else {
        file.close();
        LOG_ERROR(tr("Logging configuration export failed to ") + file_name);
        return false;
    }
}

bool Qtilities::Logging::Logger::loadSessionConfig(QString file_name) {
    if (file_name.isEmpty())
        file_name = d->session_path + QDir::separator() + qti_def_PATH_LOGCONFIG_FILE;

    LOG_DEBUG(tr("Logging configuration import started from ") + file_name);
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        LOG_INFO(tr("Logging configuration import failed from ") + file_name + tr(". This file could not be opened in read mode."));
        return false;
    }
    QDataStream stream(&file);

    // ---------------------------------------------------
    // Inspect file format:
    // ---------------------------------------------------
    quint32 ui32;
    stream >> ui32;
    if (ui32 != MARKER_LOGGER_CONFIG_TAG) {
        file.close();
        LOG_INFO(tr("Logging configuration import failed from ") + file_name + tr(". The file contains invalid data or does not exist."));
        return false;
    }

    stream >> ui32;
    Qtilities::ExportVersion read_version = (Qtilities::ExportVersion) ui32;
    Q_UNUSED(read_version);

    stream >> ui32;
    if (ui32 != MARKER_LOGGER_CONFIG_TAG) {
        file.close();
        LOG_INFO(tr("Logging configuration import failed from ") + file_name + tr(". The file contains invalid data or does not exist."));
        return false;
    }

    // ---------------------------------------------------
    // Check if input format is supported:
    // ---------------------------------------------------
//    bool is_supported_format = false;
//    if (read_version == Qtilities::Qtilities_1_0)
//        is_supported_format = true;

//    if (!is_supported_format) {
//        LOG_ERROR(QString(tr("Unsupported logger configuration file found with export version: %1. The project file will not be parsed.")).arg(read_version));
//        return IExportable::Failed;
//    }

    // ---------------------------------------------------
    // Do the actual import:
    // ---------------------------------------------------
    quint32 global_log_level;
    stream >> global_log_level;

    quint32 import_count;
    stream >> import_count;
    int import_count_int = import_count;

    // Create all engines:
    bool success = true;
    QList<AbstractLoggerEngine*> engine_list;
    for (int i = 0; i < import_count_int; ++i) {
        if (!success)
            break;

        QString tag;
        stream >> tag;
        LOG_DEBUG(tr("Creating logger factory instance with tag: ") + tag);
        AbstractLoggerEngine* engine = d->logger_engine_factory.createInstance(tag);      
        if (engine) {
            QString name;
            stream >> name;
            engine->setName(name);
            ILoggerExportable* log_export_iface = qobject_cast<ILoggerExportable*> (engine);
            if (log_export_iface) {
                log_export_iface->importBinary(stream);
                engine_list.append(engine);
            } else {
                LOG_WARNING(tr("Logger engine could not be constructed for factory tag: ") + tag);
                success = false;
            }
        } else {
            success = false;
        }
    }

    stream >> ui32;
    if (ui32 != MARKER_LOGGER_CONFIG_TAG) {
        file.close();
        LOG_INFO(tr("Logging configuration import failed from ") + file_name + tr(". The file contains invalid data or does not exist."));
        return false;
    }

    // Now attach all created engines to the logger, or delete them if neccesarry:
    int count = engine_list.count();
    if (success) {
        // First clear all engines in the logger which have exportable interfaces:
        QList<AbstractLoggerEngine*> iface_list;
        for (int i = 0; i < d->logger_engines.count(); ++i) {
            ILoggerExportable* log_export_iface = qobject_cast<ILoggerExportable*> (d->logger_engines.at(i));
            if (log_export_iface)
                iface_list.append(d->logger_engines.at(i));
        }
        for (int i = 0; i < iface_list.count(); ++i) {
            detachLoggerEngine(iface_list.at(i));
        }

        for (int i = 0; i < count; ++i) {
            if (!attachLoggerEngine(engine_list.at(i)))
                success = false;
        }
    } else {
        for (int i = 0; i < count; ++i) {
            delete engine_list.at(0);
        }
    }

    // Restore activity and formatting engines of all logger engines:
    bool complete = true;
    if (success) {
        stream >> import_count;
        int import_count_int = import_count;
        QString current_name;
        QString current_engine;
        bool is_active;
        MessageContextFlags message_context_flags;
        for (int i = 0; i < import_count_int; ++i) {
            if (!success)
                break;

            stream >> current_name;
            stream >> current_engine;
            stream >> is_active;
            quint32 context_int;
            stream >> context_int;
            message_context_flags = (MessageContextFlags) context_int;

            // Now check if the engine with the name is present, if so we set it's properties:
            AbstractLoggerEngine* engine = loggerEngineReference(current_name);
            if (engine) {
                LOG_DEBUG(tr("Restoring configuration for logger engine: ") + current_name);
                engine->installFormattingEngine(formattingEngineReference(current_engine));
                engine->setActive(is_active);
                engine->setMessageContexts(message_context_flags);
            } else {
                LOG_DEBUG(tr("Found logger engine configuration for an engine which does not exist yet with name: ") + current_name);
                complete = false;
            }
        }
    }

    // Report the correct message:
    if (success) {
        file.close();
        setGlobalLogLevel((Logger::MessageType) global_log_level);
        if (complete)
            LOG_INFO(tr("Successfully imported logging configuration (complete) imported from ") + file_name);
        else
            LOG_WARNING(tr("Logging configuration successfully (incomplete) imported from ") + file_name);
        return true;
    } else {
        file.close();
        LOG_INFO(tr("Logging configuration import failed from ") + file_name);
        return false;
    }
}
