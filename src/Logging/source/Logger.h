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

#ifndef LOGGER_H
#define LOGGER_H

#include <QtCore>

#include "Logging_global.h"
#include "LoggerFactory.h"

#include <Qtilities.h>

namespace Qtilities {
    namespace Logging {
        class AbstractFormattingEngine;
        class AbstractLoggerEngine;

        /*!
        \struct LoggerPrivateData
        \brief The logger class uses this struct to store its private data.
          */
        struct LoggerPrivateData;

        /*!
        \class Logger
        \brief The Logger class provides thread safe logging functionality to any Qt application.

        See the \ref page_logging article for more information on how to use the logger.
          */
        class LOGGING_SHARED_EXPORT Logger : public QObject
        {
            Q_OBJECT

        public:
            static Logger* instance();
            ~Logger();
            //! Initializes the logger.
            /*!
             * \param configuration_file_name The file name containing the configuration which much be loaded during the loadSessionConfig() call inside initialize(). When empty, Qtilities::Logging::Constants::qti_def_PATH_LOGCONFIG_FILE will be used.
             *
             * \sa LOG_INITIALIZE, finalize()
             */
            void initialize(const QString &configuration_file_name = QString());
            //! Finalizes the engine.
            /*!
             * This saves the current session configuration.
             *
             * \param configuration_file_name The file name containing the configuration which much be loaded during the loadSessionConfig() call inside initialize(). When empty, Qtilities::Logging::Constants::qti_def_PATH_LOGCONFIG_FILE will be used.
             *
             * \sa LOG_FINALIZE, initialize()
             */
            void finalize(const QString &configuration_file_name = QString());

        public slots:
            //! Delete all logger engines.
            /*!
              Note that this does not include the Qt Message Logger or Console Logging engines.
              */
            void clear();

        public:
            //! The possible message contexts used by the logger.
            /*!
              Logger engines will use the AllMessageContexts context by default.
              */
            enum MessageContext {
                NoMessageContext        = 0,    /*!< No messages are accepted. */
                SystemWideMessages      = 1,    /*!< All system wide messages logged through the normal log marcos, for example LOG_INFO. Thus messages logged to all logger engines. */
                EngineSpecificMessages  = 2,    /*!< Messages logged to specific engines through the engine specific marcos, for example LOG_INFO_E. */
                PriorityMessages        = 4,    /*!< Messages logged to as priority messages through the priority marcos, for example LOG_INFO_P. */
                AllMessageContexts      = SystemWideMessages | EngineSpecificMessages | PriorityMessages /*!< Represents all message contexts. */
            };
            Q_DECLARE_FLAGS(MessageContextFlags, MessageContext)
            Q_FLAGS(MessageContextFlags)
            Q_ENUMS(MessageContext)

            //! Indication used to indicate if an engine was added or removed to/from the logger.
            enum EngineChangeIndication {
                EngineAdded,        /*!< Engine was added to the logger. */
                EngineRemoved       /*!< Engine was removed from the logger. */
            };
            Q_ENUMS(EngineChangeIndication)

            //! The possible message types supported by the logger.
            /*!
              \sa setGlobalLogLevel(), globalLogLevel()
              */
            enum MessageType {
                None            = 1 << 0, /*!< No messages will be logged when using this messages type. */
                Info            = 1 << 1, /*!< An information message. */
                Warning         = 1 << 2, /*!< A warning message. */
                Error           = 1 << 3, /*!< An error message. */
                Fatal           = 1 << 4, /*!< A fatal message. */
                Debug           = 1 << 5, /*!< A debug message. \note Trace messages are not part of release mode builds. */
                Trace           = 1 << 6, /*!< A trace message. \note Trace messages are not part of release mode builds. */
                AllLogLevels    = Info | Warning | Error | Fatal | Debug | Trace /*!< Represents all message types. */
            };
            Q_DECLARE_FLAGS(MessageTypeFlags, MessageType)
            Q_FLAGS(MessageTypeFlags)
            Q_ENUMS(MessageType)

        private:
            Logger(QObject* parent = 0);

        public slots:
            //! Function to log a message.
            /*!
              This function can be used when the log macros are not sufficient. It allows you to pass
              nine additional paramaters along with the engine name, message type and message itself.

              You can write your own formatting engine to interpret these paramaters. This allows the creation
              of very powerfull and custom logging backends.

              From the formatting engines included in %Qtilities, the following ones support the additional parameters:
              - Qtilities::Logging::FormattingEngine_Default
              - Qtilities::Logging::FormattingEngine_XML

              \param engine_name The name of the engine to log to. If empty the message will be logged to all engines.
              \param message_type The type of message.
              \param message The message.
              \param msg1 More parameters which can be passed to the logger.

              \sa LOG_DEBUG, LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_TRACE, LOG_FATAL
              */
            void logMessage(const QString& engine_name, MessageType message_type, const QVariant& message, const QVariant& msg1 = QVariant(),
                      const QVariant& msg2 = QVariant(), const QVariant& msg3 = QVariant(),
                      const QVariant& msg4 = QVariant(), const QVariant& msg5 = QVariant(),
                      const QVariant& msg6 = QVariant(), const QVariant& msg7 = QVariant(),
                      const QVariant& msg8 = QVariant(), const QVariant& msg9 = QVariant());
            //! Function to log a priority message.
            /*!
              A priority message is handled in the same way as a normal message logged with logMessage() except that
              it also emits the newPriorityMessage() signal. This allows selective messages to be emitted and
              displayed in a convenient place in an application where all messages should not necessarily be displayed.
              An example is the case where selective messages should be displayed in a status bar.

              By default priority messages are unformatted and \p message is used.

              \sa LOG_DEBUG_P, LOG_ERROR_P, LOG_WARNING_P, LOG_INFO_P, LOG_TRACE_P, LOG_FATAL_P
              */
            void logPriorityMessage(const QString& engine_name, MessageType message_type, const QVariant& message, const QVariant& msg1 = QVariant(),
                      const QVariant& msg2 = QVariant(), const QVariant& msg3 = QVariant(),
                      const QVariant& msg4 = QVariant(), const QVariant& msg5 = QVariant(),
                      const QVariant& msg6 = QVariant(), const QVariant& msg7 = QVariant(),
                      const QVariant& msg8 = QVariant(), const QVariant& msg9 = QVariant());
            /*void funcMessage(const QString& function_name, MessageType message_type, const QVariant& message, const QVariant& msg1 = QVariant(),
                      const QVariant& msg2 = QVariant(), const QVariant& msg3 = QVariant(),
                      const QVariant& msg4 = QVariant(), const QVariant& msg5 = QVariant(),
                      const QVariant& msg6 = QVariant(), const QVariant& msg7 = QVariant(),
                      const QVariant& msg8 = QVariant(), const QVariant& msg9 = QVariant());
                      */

        public:
            // -----------------------------------------
            // Functions related to formatting engines
            // -----------------------------------------
            //! Function to change the formatting engine used for priority messages.
            /*!
              \return True if successfull, thus a formatting engine called \p name could be found. False otherwise.
              */
            bool setPriorityFormattingEngine(const QString& name);
            //! Function to change the formatting engine used for priority messages.
            void setPriorityFormattingEngine(AbstractFormattingEngine* engine);
            //! Returns the names of all available formatting engines.
            /*!
              The Logger provided a set of ready to use formatting engines and this function
              provides the names which can be used to access these engines.

              Note that the engines are registered in the logger during initialization,
              thus you must call initialize() before this function will return anything meaningfull.

              \sa initialize(), LOG_INITIALIZE
              */
            QStringList availableFormattingEnginesInFactory() const;
            //! Returns a reference to the formatting engine specified by the name given.
            AbstractFormattingEngine* formattingEngineReference(const QString& name);
            //! Function which allows custom formatting engines to be registered inside the logger.
            /*!
             * If custom formatting engines were registered, their names can be used in functions
             * such as newFileEngine() etc. to specify the engine to be used. Internally, the logger
             * will store the engine using a QPointer, thus it will use it as long as its valid. It is
             * recommended to use singletons for formatting engines to ensure they are available for the
             * complete session.
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void registerFormattingEngine(AbstractFormattingEngine* formatting_engine);
            //! Returns a reference to the formatting engine which provides the given file extension.
            /*!
              If multiple engines with the same file extension exists, the first one found will be used.

              \param file_extension See Qtilities::Logging::AbstractFormattingEngine::fileExtension() for the expected format.
              \returns Formatting engine if found, null otherwise.
              */
            AbstractFormattingEngine* formattingEngineReferenceFromExtension(const QString& file_extension);
            //! Returns a reference to the formatting engine at a specific position.
            AbstractFormattingEngine* formattingEngineReferenceAt(int index);
            //! Returns the name of the default formatting engine.
            QString defaultFormattingEngine() const;
            //! Register a new logger engine factory.
            void registerLoggerEngineFactory(const QString& tag, LoggerFactoryInterface<AbstractLoggerEngine>* factory_iface);
            //! Function used to get a QStringList with the tags of all available logger engines.
            QStringList availableLoggerEnginesInFactory() const;
            //! Provides the number of attached formatting engines.
            int attachedFormattingEngineCount() const;

            // -----------------------------------------
            // Functions related to enabled log level
            // -----------------------------------------
            //! Sets the global log level. All messages are filtered by the logger according to the global log level before being sent to any logger engines.
            void setGlobalLogLevel(Logger::MessageType new_log_level);
            //! Returns the current global log level.
            Logger::MessageType globalLogLevel() const;
            //! Function which returns a string associated with a log level.
            QString logLevelToString(Logger::MessageType log_level) const;
            //! Function which returns the log level associated with an input string.
            Logger::MessageType stringToLogLevel(const QString& log_level_string) const;
            //! Function which returns all available log level strings.
            QStringList allLogLevelStrings() const;
            //! Function which returns a string associated with set of message contexts.
            QString messageContextsToString(Logger::MessageContextFlags message_contexts) const;
            //! Function which returns the message contexts associated with an input string.
            Logger::MessageContextFlags stringToMessageContexts(const QString& message_contexts_string) const;
            //! Function which returns all available message contexts in a QStringList.
            QStringList allMessageContextStrings() const;

            // -----------------------------------------
            // Functions related to updating of QSettings
            // -----------------------------------------
            //! Stores the logging parameters using QSettings.
            /*!
              For more information see \ref configuration_widget_storage_layout and loggerSettingsEnabled()
              */
            void writeSettings() const;
            //! Reads the current logging paramaters stored in QSettings.
            /*!
              For more information see \ref configuration_widget_storage_layout.
              */
            void readSettings();
            //! Tell the logger to save the current logging engine configuration when the application exits, and load remember it (load it) the next time the appplication starts.
            /*!
             * To keep logger engine configurations between sessions, just call Log->setRememberSessionConfig(true) before LOG_INITIALIZE().
             *
             * \sa rememberSessionConfig()
             */
            void setRememberSessionConfig(bool remember);
            //! Returns true if the logger is configured to save the current logging engine configuration when the application exits, and load remember it (load it) the next time the appplication starts.
            /*!
             * \sa setRememberSessionConfig()
             */
            bool rememberSessionConfig() const;

            // -----------------------------------------
            // Functions related to Qt Debugging output
            // -----------------------------------------
            //! Installs the logger as the Qt Message Handler.
            void installAsQtMessageHandler(bool update_stored_settings = true);
            //! Uninstalls the logger as the Qt Message Handler.
            void uninstallAsQtMessageHandler();
            //! Returns true if the logger is the Qt Message Handler.
            bool isQtMessageHandler() const;
            //! Toggles the logger as the Qt Message Handler.
            void setIsQtMessageHandler(bool toggle);

            // -----------------------------------------
            // Public functions related to attached logger eninges
            // -----------------------------------------
            //! Deletes all logger engines, thus deactivate them.
            void deleteAllLoggerEngines();
            //! Disable all logger engines, thus deactivate them.
            void disableAllLoggerEngines();
            //! Enable all logger engines, thus activate them.
            void enableAllLoggerEngines();
            //! Deletes the specified engine.
            void deleteEngine(const QString& engine_name);
            //! Enable the specified engine.
            void enableEngine(const QString& engine_name);
            //! Disables the specified backengineed.
            void disableEngine(const QString engine_name);
            //! Provides a QStringList with the names of all attached logger engines.
            QStringList attachedLoggerEngineNames() const;
            //! Provides the number of attached logger engines.
            int attachedLoggerEngineCount() const;
            //! Returns a reference to the logger known by the specified name.
            AbstractLoggerEngine* loggerEngineReference(const QString& engine_name);
            //! Returns a reference to the FileLoggerEngine logging to the specified file name.
            /*!
             * \note If no FileLoggerEngine exists which currently logs to the specified file name, 0 is returned.
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            AbstractLoggerEngine *loggerEngineReferenceForFile(const QString& file_path);
            //! Returns a reference to the logger engine at a specific position.
            AbstractLoggerEngine* loggerEngineReferenceAt(int index);
            //! Creates a new logger with the parameters specified.
            AbstractLoggerEngine* newLoggerEngine(QString engine_tag, AbstractFormattingEngine* formatting_engine = 0);

            // -----------------------------------------
            // Convenience functions to create engines
            // -----------------------------------------
            //! Convenience function to create a new instance of a file engine.
            /*!
              \param engine The of the engine. This name can be used to reference the engine at a later stage.
              \param file_name The name of the file to which logging must be done. If no engine name is provided the function will return false.
              \param formatting_engine The name of the formatting engine which must be used to format messages in the engine. When empty the file extension of file_name is used to determine the formatting engine used for the file by checking the Qtilities::Logging::AbstractFormattingEngine::fileExtension() functions of all the available formatting engines. If the file extension does not match any of the available file extensions, the Qtilities::Logging::FormattingEngine_Default formatting engine is used.

              \return The constructed logger engine if successfull, null otherwise.
            */
            AbstractLoggerEngine* newFileEngine(const QString& engine_name, const QString& file_name, const QString& formatting_engine = QString());
            //! Convenience function to enable a Qt Message engine which pipes messages through to the Qt Debugging System.
            /*!
             * \sa qtMessageEngineActive()
             *
             * Only one qt message engine exists for every application. This engine can be enabled/disabled using this function.
             */
            void toggleQtMsgEngine(bool toggle);
            //! Convenience function to check if the QtMsgEngine is active.
            /*!
             * \sa toggleQtMsgEngine()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            bool qtMsgEngineActive() const;
            //! Convenience function to enable a console engine.
            /*!
             * \sa consoleEngineActive()
             *
             * Only one console engine exists for every application. This engine can be enabled/disabled using this function.
             */
            void toggleConsoleEngine(bool toggle);
            //! Convenience function to check if the ConsoleEngine is active.
            /*!
             * \sa toggleConsoleEngine()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            bool consoleEngineActive() const;
            //! Attaches the new logger engine. By default the new engine will be intialized by this function. If you want to initialize the engine manually, set the initialize_engine paramater to false.
            bool attachLoggerEngine(AbstractLoggerEngine* new_logger_engine, bool initialize_engine = true);
            //! Detaches the logger engine specified.
            /*!
             * \brief detachLoggerEngine
             * \param logger_engine The engine to be deleted.
             * \param delete_engine Indicates if the engine must be deleted after it was detached.
             * \return True when successfull, false otherwise.
             */
            bool detachLoggerEngine(AbstractLoggerEngine* logger_engine, bool delete_engine = true);

            //! Loads the session config from a previous session.
            /*!
                If no file name is passed, the default session log file is used at QCoreApplication::applicationDirPath() + Qtilities::Logging::Constants::qti_def_PATH_SESSION + QDir::separator() + Qtilities::Logging::Constants::qti_def_PATH_LOGCONFIG_FILE.
                However in GUI applications the default path changes, see Qtilities::CoreGui::QtilitiesApplication::applicationSessionPath().
            */
            bool loadSessionConfig(QString file_name = QString());
            //! Save the current config to a file.
            /*!
                If no file name is passed, the default session log file in GUI applications is used at QCoreApplication::applicationDirPath() + Qtilities::Logging::Constants::qti_def_PATH_SESSION + QDir::separator() + Qtilities::Logging::Constants::qti_def_PATH_LOGCONFIG_FILE.
                However in GUI applications the default path changes, see Qtilities::CoreGui::QtilitiesApplication::applicationSessionPath().

                This function is automatically called in:
                - finalize() only when loggerSettingsEnabled() is true.
                - When the user clicks apply in Qtilities::CoreGui::LoggerConfigWidget.
            */
            bool saveSessionConfig(QString file_name = QString(), Qtilities::ExportVersion version = Qtilities::Qtilities_Latest) const;

            //! Sets the session config path to be used by the logger.
            /*!
              The logger uses this path to save internal settings, and to save its logger configurations.

              For more information about internal settings see \ref configuration_widget_storage_layout and loggerSettingsEnabled().
              */
            void setLoggerSessionConfigPath(const QString path);
            //! Enables/disables the saving of settings by the logger.
            /*!
              Saving is enabled by default.

              \sa loggerSettingsEnabled(), \ref configuration_widget_storage_layout
              */
            void setLoggerSettingsEnabled(bool is_enabled);
            //! Gets if the saving of settings by the logger is enabled.
            /*!
              Saving is enabled by default.

              \sa setLoggerSettingsEnabled(), \ref configuration_widget_storage_layout
              */
            bool loggerSettingsEnabled() const;

        signals:
            //! Signal which is emitted when a new message was logged. The logger connects all logger engines to this signal.
            void newMessage(const QString& engine_name, Logger::MessageType message_type, Logger::MessageContextFlags message_context, const QList<QVariant>& message_contents);
            //! Signal which is emitted when a new priority message was logged.
            /*!
              \sa logPriorityMessage();
              */
            void newPriorityMessage(Logger::MessageType message_type, const QString& formatted_message);
            //! Indicates that the number of logger engines changed.
            void loggerEngineCountChanged(AbstractLoggerEngine* engine, Logger::EngineChangeIndication change_indication);

        private:
            static Logger* m_Instance;
            LoggerPrivateData* d;
        };

        #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        void installLoggerMessageHandler(QtMsgType type, const char *msg);
        #else
        void installLoggerMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
        #endif
        Q_DECLARE_OPERATORS_FOR_FLAGS(Logger::MessageTypeFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(Logger::MessageContextFlags)
     }
}

Q_DECLARE_METATYPE(Qtilities::Logging::Logger::MessageType)

// -----------------------------------
// Macro Definitions
// -----------------------------------
//! The log macro which returns a pointer to the logger singleton instance.
#define Log Qtilities::Logging::Logger::instance()
//! Initializes the logger and specifies the logging engine configuration to load.
/*!
    The initialization will create default formatting and logger engines for you. When rememberSessionConfig() is true, the initialization will also
    restore your previous configuration.

    \sa Qtilities::Logging::Logger::initialize(), \sa Qtilities::Logging::Logger::rememberSessionConfig()

    In GUI applications, its recommended to call QtilitiesApplication::applicationSessionPath() directory before the initialization in order for
    QtilitiesApplication to set the session path used by the logger to the correct path.
    */
#define LOG_INITIALIZE() Log->initialize()
//! Finalizes the logger.
/*!
    The finalization will store your logging parameters and also clean up the logger, thus it will delete all engines.

    \sa Qtilities::Logging::Logger::finalize(), \sa Qtilities::Logging::Logger::rememberSessionConfig()
    */
#define LOG_FINALIZE() Log->finalize()

// -----------------------------------
// Basic Logging Macros
// -----------------------------------
//! Logs a trace message to all active engines.
/*!
    \note Trace messages are not part of release mode builds.
  */
#ifndef QT_NO_DEBUG
#define LOG_TRACE(Msg) Log->logMessage(QString(),Qtilities::Logging::Logger::Trace, Msg)
#else
#define LOG_TRACE(Msg) ((void)0)
#endif
//! Logs a debug message to all active engines.
/*!
    \note Debug messages are not part of release mode builds.
  */
#ifndef QT_NO_DEBUG
#define LOG_DEBUG(Msg) Log->logMessage(QString(),Qtilities::Logging::Logger::Debug, Msg)
#else
#define LOG_DEBUG(Msg) ((void)0)
#endif
//! Logs an error message to all active engines.
#define LOG_ERROR(Msg) Log->logMessage(QString(),Qtilities::Logging::Logger::Error, Msg)
//! Logs a warning message to all active engines.
#define LOG_WARNING(Msg) Log->logMessage(QString(),Qtilities::Logging::Logger::Warning, Msg)
//! Logs a fatal message to all active engines.
#define LOG_FATAL(Msg) Log->logMessage(QString(),Qtilities::Logging::Logger::Fatal, Msg)
//! Logs an information message to all active engines.
#define LOG_INFO(Msg) Log->logMessage(QString(),Qtilities::Logging::Logger::Info, Msg)

// -----------------------------------
// Priority Logging Macros
// -----------------------------------
//! Logs a priority trace message to all active engines.
/*!
    \note Trace messages are not part of release mode builds.
  */
#ifndef QT_NO_DEBUG
#define LOG_TRACE_P(Msg) Log->logPriorityMessage(QString(),Qtilities::Logging::Logger::Trace, Msg)
#else
#define LOG_TRACE_P(Msg) ((void)0)
#endif
//! Logs a priority debug message to all active engines.
/*!
    \note Debug messages are not part of release mode builds.
  */
#ifndef QT_NO_DEBUG
#define LOG_DEBUG_P(Msg) Log->logPriorityMessage(QString(),Qtilities::Logging::Logger::Debug, Msg)
#else
#define LOG_DEBUG_P(Msg) ((void)0)
#endif
//! Logs a priority error message to all active engines.
#define LOG_ERROR_P(Msg) Log->logPriorityMessage(QString(),Qtilities::Logging::Logger::Error, Msg)
//! Logs a priority warning message to all active engines.
#define LOG_WARNING_P(Msg) Log->logPriorityMessage(QString(),Qtilities::Logging::Logger::Warning, Msg)
//! Logs a priority fatal message to all active engines.
#define LOG_FATAL_P(Msg) Log->logPriorityMessage(QString(),Qtilities::Logging::Logger::Fatal, Msg)
//! Logs a priority information message to all active engines.
#define LOG_INFO_P(Msg) Log->logPriorityMessage(QString(),Qtilities::Logging::Logger::Info, Msg)

// -----------------------------------
// Engine Specific Logging
// -----------------------------------
//! Logs a trace message to the engine specified. Note that the engine must be active for the message to be logger.
#ifndef QT_NO_DEBUG
#define LOG_TRACE_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Trace, Msg)
#else
#define LOG_TRACE_E(Engine_Name, Msg) ((void)0)
#endif
//! Logs a debug message to the engine specified. Note that the engine must be active for the message to be logger.
#ifndef QT_NO_DEBUG//!
#define LOG_DEBUG_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Debug, Msg)
#else
#define LOG_DEBUG_E(Engine_Name, Msg) ((void)0)
#endif
//! Logs an error message to the engine specified. Note that the engine must be active for the message to be logger.
#define LOG_ERROR_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Error, Msg)
//! Logs a warning message to the engine specified. Note that the engine must be active for the message to be logger.
#define LOG_WARNING_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Warning, Msg)
//! Logs a fatal message to the engine specified. Note that the engine must be active for the message to be logger.
#define LOG_FATAL_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Fatal, Msg)
//! Logs an info message to the engine specified. Note that the engine must be active for the message to be logger.
#define LOG_INFO_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Info, Msg)

// -----------------------------------
// Function Specific Logging
// -----------------------------------
/*#define FUNC_DEBUG(Function_Name, Msg) Log->funcMessage(Engine_Name,Qtilities::Logging::Logger::Debug, Msg)
#define FUNC_ERROR(Function_Name, Msg) Log->funcMessage(Engine_Name,Qtilities::Logging::Logger::Error, Msg)
#define FUNC_WARNING(Function_Name, Msg) Log->funcgMessage(Engine_Name,Qtilities::Logging::Logger::Warning, Msg)
#define FUNC_FATAL(Function_Name, Msg) Log->funcMessage(Engine_Name,Qtilities::Logging::Logger::Fatal, Msg)
#define FUNC_INFO(Function_Name, Msg) Log->funcMessage(Engine_Name,Qtilities::Logging::Logger::Info, Msg)*/

#endif // LOGGER_H
