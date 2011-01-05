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

#ifndef LOGGER_H
#define LOGGER_H

#include <QtCore>

#include "Logging_global.h"
#include "LoggerFactory.h"

namespace Qtilities {
    namespace Logging {
        /*!
        \struct LoggerData
        \brief The logger class uses this struct to store its private data.
          */
        struct LoggerData;
        class AbstractFormattingEngine;
        class AbstractLoggerEngine;

        /*!
        \class Logger
        \brief The Logger class is a singleton which manages logging in Qt application.

        The Logger class provides logging functionality to any Qt application.

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
                \sa LOG_INITIALIZE, finalize()
                */
            void initialize();
            //! Finalizes the engine.
            /*!
              This saves the current session configuration.
                \sa LOG_FINALIZE, initialize()
              */
            void finalize();

        public slots:
            //! Delete all logger engines.
            /*!
              Note that this does not include the Qt Message Logger or Console Logging engines.
              */
            void clear();        

        public:
            //! Indication used to indicate if an engine was added or removed to/from the logger.
            enum EngineChangeIndication {
                EngineAdded,        /*!< Engine was added to the logger. */
                EngineRemoved       /*!< Engine was removed from the logger. */
            };
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
            Q_DECLARE_FLAGS(MessageTypeFlags, MessageType);
            Q_FLAGS(MessageTypeFlags);
            Q_ENUMS(MessageType);

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
            QStringList availableFormattingEngines() const;
            //! Returns a reference to the formatting engine specified by the name given.
            AbstractFormattingEngine* formattingEngineReference(const QString& name);
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
            QStringList availableLoggerEngines() const;
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

            // -----------------------------------------
            // Functions related to updating of QSettings
            // -----------------------------------------
            //! Stores the logging parameters using QSettings.
            void writeSettings() const;
            //! Reads the current logging paramaters stored in QSettings.
            void readSettings();
            //! Tell the logger to remember the current logging configuration the next time the appplication starts.
            /*!
              \sa rememberSessionConfig()
              */
            void setRememberSessionConfig(bool remember);
            //! Returns true if the logger is configured to remember the current session information.
            /*!
              \sa setRememberSessionConfig()
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
            //! Returns a reference to the logger engine at a specific position.
            AbstractLoggerEngine* loggerEngineReferenceAt(int index);
            //! Creates a new logger with the parameters specified.
            AbstractLoggerEngine* newLoggerEngine(const QString& engine_tag, AbstractFormattingEngine* formatting_engine = 0);

            // -----------------------------------------
            // Convenience functions to create engines
            // -----------------------------------------
            //! Convenience function to create a new instance of a file engine.
            /*!
              \param engine The of the engine. This name can be used to reference the engine at a later stage.
              \param file_name The name of the file to which logging must be done. If no engine name is provided the function will return false.
              \param formatting_engine The name of the formatting engine which must be used to format messages in the engine. When empty the file extension of file_name is used to determine the formatting engine used for the file by checking the Qtilities::Logging::AbstractFormattingEngine::fileExtension() functions of all the available formatting engines. If the file extension does not match any of the available file extensions, the Qtilities::Logging::FormattingEngine_Default formatting engine is used.

              \return True if new instance was successfully created and initialized. False otherwise.
            */
            bool newFileEngine(const QString& engine_name, const QString& file_name, const QString& formatting_engine = QString());
            //! Convenience function to enable a Qt Message engine which pipes messages through to the Qt Debugging System.
            /*!
              Only one qt message engine can be created. This engine can be enabled/disabled using this function.
            */
            void toggleQtMsgEngine(bool toggle);
            //! Convenience function to enable a console engine.
            /*!
              Only one console engine can be created. This engine can be enabled/disabled using this function.
            */
            void toggleConsoleEngine(bool toggle);
            //! Attaches the new logger engine. By default the new engine will be intialized by this function. If you want to initialize the engine manually, set the initialize_engine paramater to false.
            bool attachLoggerEngine(AbstractLoggerEngine* new_logger_engine, bool initialize_engine = true);
            //! Detaches the logger engine specified.
            bool detachLoggerEngine(AbstractLoggerEngine* logger_engine);

            //! Loads the session config from a previous session. If no file name is passed, the default session log file is used.
            bool loadSessionConfig(QString file_name = QString());
            //! Save the current config to a file. If no file name is passed, the default session log file is used.
            bool saveSessionConfig(QString file_name = QString()) const;

        signals:
            //! Signal which is emitted when a new message was logged. The logger connects all logger engines to this signal.
            void newMessage(const QString& engine_name, Logger::MessageType message_type, const QList<QVariant>& message_contents);
            //! Signal which is emitted when a new priority message was logged.
            /*!
              \sa logPriorityMessage();
              */
            void newPriorityMessage(Logger::MessageType message_type, const QString& formatted_message);
            //! Indicates that the number of logger engines changed.
            void loggerEngineCountChanged(AbstractLoggerEngine* engine, Logger::EngineChangeIndication change_indication);

        private:
            static Logger* m_Instance;
            LoggerData* d;
        };

        void installLoggerMessageHandler(QtMsgType type, const char *msg);
        Q_DECLARE_OPERATORS_FOR_FLAGS(Logger::MessageTypeFlags)
     }
}

Q_DECLARE_METATYPE(Qtilities::Logging::Logger::MessageType);

// -----------------------------------
// Macro Definitions
// -----------------------------------
//! The log macro which returns a pointer to the logger singleton instance.
#define Log Qtilities::Logging::Logger::instance()
//! Initializes the logger.
/*!
    The initialization will create default formatting and logger engines for you. When rememberSessionConfig() is true, the initialization will also restore your previous configuration.
    \sa Qtilities::Logging::Logger::initialize()
    */
#define LOG_INITIALIZE() Log->initialize()
//! Finalizes the logger.
/*!
    The finalization will store your logging parameters and also clean up the logger, thus it will delete all engines.
    \sa Qtilities::Logging::Logger::finalize()
    */
#define LOG_FINALIZE() Log->finalize()

// -----------------------------------
// Basic Logging Macros
// -----------------------------------
//! Logs a trace message to all active engines.
/*!
    \note Trace messages are not part of release mode builds.
  */
#define LOG_TRACE(Msg) Log->logMessage(QString("All"),Qtilities::Logging::Logger::Trace, Msg)
//! Logs a debug message to all active engines.
/*!
    \note Debug messages are not part of release mode builds.
  */
#define LOG_DEBUG(Msg) Log->logMessage(QString("All"),Qtilities::Logging::Logger::Debug, Msg)
//! Logs an error message to all active engines.
#define LOG_ERROR(Msg) Log->logMessage(QString("All"),Qtilities::Logging::Logger::Error, Msg)
//! Logs a warning message to all active engines.
#define LOG_WARNING(Msg) Log->logMessage(QString("All"),Qtilities::Logging::Logger::Warning, Msg)
//! Logs a fatal message to all active engines.
#define LOG_FATAL(Msg) Log->logMessage(QString("All"),Qtilities::Logging::Logger::Fatal, Msg)
//! Logs an information message to all active engines.
#define LOG_INFO(Msg) Log->logMessage(QString("All"),Qtilities::Logging::Logger::Info, Msg)

// -----------------------------------
// Priority Logging Macros
// -----------------------------------
//! Logs a priority trace message to all active engines.
/*!
    \note Trace messages are not part of release mode builds.
  */
#define LOG_TRACE_P(Msg) Log->logPriorityMessage(QString("All"),Qtilities::Logging::Logger::Trace, Msg)
//! Logs a priority debug message to all active engines.
/*!
    \note Debug messages are not part of release mode builds.
  */
#define LOG_DEBUG_P(Msg) Log->logPriorityMessage(QString("All"),Qtilities::Logging::Logger::Debug, Msg)
//! Logs a priority error message to all active engines.
#define LOG_ERROR_P(Msg) Log->logPriorityMessage(QString("All"),Qtilities::Logging::Logger::Error, Msg)
//! Logs a priority warning message to all active engines.
#define LOG_WARNING_P(Msg) Log->logPriorityMessage(QString("All"),Qtilities::Logging::Logger::Warning, Msg)
//! Logs a priority fatal message to all active engines.
#define LOG_FATAL_P(Msg) Log->logPriorityMessage(QString("All"),Qtilities::Logging::Logger::Fatal, Msg)
//! Logs a priority information message to all active engines.
#define LOG_INFO_P(Msg) Log->logPriorityMessage(QString("All"),Qtilities::Logging::Logger::Info, Msg)

// -----------------------------------
// Engine Specific Logging
// -----------------------------------
//! Logs a trace message to the engine specified. Note that the engine must be active for the message to be logger.
#define LOG_TRACE_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Trace, Msg)
//! Logs a debug message to the engine specified. Note that the engine must be active for the message to be logger.
#define LOG_DEBUG_E(Engine_Name, Msg) Log->logMessage(Engine_Name,Qtilities::Logging::Logger::Debug, Msg)
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
