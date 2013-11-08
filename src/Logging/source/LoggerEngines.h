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

#ifndef LOGGERENGINES_H
#define LOGGERENGINES_H

#include "Logging_global.h"
#include "AbstractLoggerEngine.h"
#include "LoggingConstants.h"
#include "LoggerFactory.h"
#include "ILoggerExportable.h"

#include <QList>
#include <QVariant>

#define CONSOLE_RESET   "\033[0m"
#define CONSOLE_BLACK   "\033[30m"      /* Black */
#define CONSOLE_RED     "\033[31m"      /* Red */
#define CONSOLE_GREEN   "\033[32m"      /* Green */
#define CONSOLE_YELLOW  "\033[33m"      /* Yellow */
#define CONSOLE_BLUE    "\033[34m"      /* Blue */
#define CONSOLE_MAGENTA "\033[35m"      /* Magenta */
#define CONSOLE_CYAN    "\033[36m"      /* Cyan */
#define CONSOLE_WHITE   "\033[37m"      /* White */
#define CONSOLE_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define CONSOLE_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define CONSOLE_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define CONSOLE_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define CONSOLE_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define CONSOLE_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define CONSOLE_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define CONSOLE_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

namespace Qtilities {
    namespace Logging {
        using namespace Qtilities::Logging::Interfaces;
        using namespace Qtilities::Logging::Constants;

        // ------------------------------------
        // File Logger Engine
        // ------------------------------------
        /*!
        \class FileLoggerEngine
        \brief A logger engine which stores the logged messages in a file.

        A logger engine which stores the logged messages in a file.
          */
        class LOGGING_SHARED_EXPORT FileLoggerEngine : public AbstractLoggerEngine, public ILoggerExportable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Logging::Interfaces::ILoggerExportable)
            Q_PROPERTY(QString FileName READ getFileName)

        public:
            FileLoggerEngine();
            ~FileLoggerEngine();

            // --------------------------------
            // AbstractLoggerEngine Implementation
            // --------------------------------
            bool initialize();
            void finalize();
            QString description() const;
            QString status() const;
            bool isFormattingEngineConstant() const { return true; }
            /*!
              Clearing of FileLoggerEngine was introduced in %Qtilities v1.1.
              */
            void clearLog();

            // --------------------------------
            // ILoggerExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            bool exportBinary(QDataStream& stream) const;
            bool importBinary(QDataStream& stream);
            QString factoryTag() const { return qti_def_FACTORY_TAG_FILE_LOGGER_ENGINE; }
            QString instanceName() const { return name(); }

            //! Sets the file name to which this engine will write the log output.
            /*!
                Its not possible to change the file name while the logger engine is in a initialized state.
                To change the file name: call finalize(), setFileName() and then call initialize() again.
              */
            void setFileName(const QString& fileName);
            //! Gets the file name to which the logger is currently logging.
            QString getFileName();

            // Make this class a factory item
            static LoggerFactoryItem<AbstractLoggerEngine, FileLoggerEngine> factory;

        public slots:
            void logMessage(const QString& message, Logger::MessageType message_type);

        private:
            QString file_name;
        };

        // ------------------------------------
        // Qt Message Logger Engine
        // ------------------------------------
        /*!
        \class QtMsgLoggerEngine
        \brief A logger engine which pipes messages to the Qt Messaging System as debug messages.

        A logger engine which pipes messages to the Qt Messaging System as debug messages.

        \note Clearing the log through clearLog() is not supported by this logger engine.
        \note QtMsgLoggerEngine engines are not removeable.
          */
        class LOGGING_SHARED_EXPORT QtMsgLoggerEngine : public AbstractLoggerEngine
        {
            Q_OBJECT

        public:
            static QtMsgLoggerEngine* instance();
            ~QtMsgLoggerEngine();

        protected:
            QtMsgLoggerEngine();

        public:
            // AbstractLoggerEngine implementation
            bool initialize();
            void finalize();
            QString description() const;
            QString status() const;
            bool removable() const { return false; }
            bool isFormattingEngineConstant() const { return true; }

        public slots:
            void logMessage(const QString& message, Logger::MessageType message_type);

        private:
            static QtMsgLoggerEngine* m_Instance;
        };

        // ------------------------------------
        // Console Logger Engine
        // ------------------------------------
        /*!
        \class ConsoleLoggerEngine
        \brief A logger engine which pipes messages to a console using the stdio.h fprintf function.

        A logger engine which pipes messages to a console using the stdio.h fprintf function.

        \note Clearing the log through clearLog() is not supported by this logger engine.
        \note ConsoleLoggerEngine engines are not removeable.
          */
        class LOGGING_SHARED_EXPORT ConsoleLoggerEngine : public AbstractLoggerEngine
        {
            Q_OBJECT

        public:
            static ConsoleLoggerEngine* instance();
            ~ConsoleLoggerEngine();

        protected:
            ConsoleLoggerEngine();

        public:
            //! Sets if ANSI escape codes are enabled, in order to color messages appearing in the console according to their types.
            /*!
             * For more information on these escape codes, including OS support etc, see http://en.wikipedia.org/wiki/ANSI_escape_code.
             *
             * \note Enabled by default and does not work on Windows.
             *
             * \sa consoleFormattingEnabled(), setConsoleFormattingHint()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void setConsoleFormattingEnabled(bool is_enabled);
            //! Gets if ANSI escape codes are enabled, in order to color messages appearing in the console according to their types.
            /*!
             * For more information on these escape codes, including OS support etc, see http://en.wikipedia.org/wiki/ANSI_escape_code.
             *
             * \note Enabled by default and does not work on Windows.
             *
             * \sa setConsoleFormattingEnabled(), setConsoleFormattingHint(), resetConsoleEscapeCodes()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            bool consoleFormattingEnabled() const;
            //! Sets the formatting hint to use for console messages when consoleFormattingEnabled() is true.
            /*!
             * A set of codes to use are defined in %Qtilities, for example CONSOLE_RED, CONSOLE_BLUE etc.
             *
             * Default colors are:
             * Info - White
             * Warning - Blue
             * Error & Fatal - Red
             *
             * For more information on these escape codes, including OS support etc, see http://en.wikipedia.org/wiki/ANSI_escape_code.
             *
             * \note Enabled by default and does not work on Windows.
             *
             * \sa consoleFormattingEnabled(), setConsoleFormattingEnabled(), resetConsoleEscapeCodes()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void setConsoleFormattingHint(Logger::MessageType message_type, QString hint_color);
            //! Sets the formatting hint to use for console messages when consoleFormattingEnabled() is true.
            /*!
             * Resets the escape codes on the console using the ANSI escape code character.
             *
             * For more information on these escape codes, including OS support etc, see http://en.wikipedia.org/wiki/ANSI_escape_code.
             *
             * \note Does nothing on Windows.
             *
             * \sa consoleFormattingEnabled(), setConsoleFormattingEnabled()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void resetConsoleEscapeCodes();

            // AbstractLoggerEngine implementation
            bool initialize();
            void finalize();
            QString description() const;
            QString status() const;
            bool removable() const { return false; }
            bool isFormattingEngineConstant() const { return false; }

        public slots:
            void logMessage(const QString& message, Logger::MessageType message_type);

        private:
            static ConsoleLoggerEngine* m_Instance;
            bool color_formatting_enabled;
            QMap<Logger::MessageType,QString> message_colors;
        };
    }
}

#endif // FILELOGGERENGINE_H
