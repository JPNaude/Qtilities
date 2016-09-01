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

#ifndef QTILITIES_PROCESS_H
#define QTILITIES_PROCESS_H

#include "QtilitiesCore_global.h"
#include "Task.h"

#include <QObject>
#include <QProcess>
#include <Logger>
using namespace Qtilities::Logging;

namespace Qtilities {
    namespace Core {
        /*!
         * \struct ProcessBufferMessageTypeHint
         * \brief The ProcessBufferMessageTypeHint structure is used to define custom hints for process buffer message processing in QtilitiesProcess.
         *
         * A ProcessBufferMessageTypeHint contains the following information:
         * - d_regexp: The regular expression matched to the single message line after processing has been done by QtilitiesProcess (see \ref qtilities_process_buffering).
         * - d_message_type: The message type to assign to matched messages. When using Logger::None, the message will not be logged and filtered.
         * - d_priority: A priority for the match. If multiple hints match a message, the hint with the highest priority wil be used to log the message. If multiple hints match which have the same priority, the message will be linked using multiple times (where each logged message will be logged using the type specified by the applicable matching hint).
         * - d_is_disabler: When true, any message that matches this hint will cause QtilitiesProcess to ignore subsequent messages until a message
         *   matching an enabler is received. This allows sections of messages to be skipped. To control whether the disabler
         *   message itself must be logged, either set the message_type to Logger::None in order to not log it, or any other message type to log the
         *   message as that type of message.
         * - d_is_enabler: When true, any message that matches this hint will cause QtilitiesProcess to log subsequent messages. This allows sections of messages to be skipped.
         *   The message type of an enabler is not used unless d_is_enabler_log_match is true. To control whether the enabler
         *   message itself must be logged, either set the message_type to Logger::None in order to not log it, or any other message type to log the
         *   message as that type of message.
         * - d_is_stopper: When true, the process will be stopped as soon as a message match the expression of the stopper hint. To control whether the stopper
         *   message itself must be logged, either set the message_type to Logger::None in order to not log it, or any other message type to log the
         *   message as that type of message. Using d_stop_message it is also possible to provide an additional message logged directly after the stopper message
         *   to explain why the stopper caused the process to be stopped. The message type of the stopper message is determined by d_stop_message_type which is Logger::Error
         *   by default.
         * - d_disabled_unblocked_message_types: allow specific messages through while disablers are active. By default the messages which are unblocked (let through) are Error and Fatal messages.
         *   This allows errors in the QProcess to still be caught by default, even though disablers are active. It is possible to explicitly set which message types are let through, so if errors and fatals should not be let through, change this property to Logger::None.
         */
        struct ProcessBufferMessageTypeHint {
        public:
            ProcessBufferMessageTypeHint(const QRegExp& regexp,
                                         Logger::MessageType message_type,
                                         int priority = 0) {
                d_regexp = regexp;
                d_message_type = message_type;
                d_priority = priority;
                d_is_enabler = false;
                d_is_disabler = false;
                d_is_stopper = false;
                d_stop_message_type = Logger::Error;

                d_disabled_unblocked_message_types = 0;
                d_disabled_unblocked_message_types |= Logger::Error;
                d_disabled_unblocked_message_types |= Logger::Fatal;
            }
            ProcessBufferMessageTypeHint(const ProcessBufferMessageTypeHint& ref) {
                d_message_type = ref.d_message_type;
                d_regexp = ref.d_regexp;
                d_priority = ref.d_priority;
                d_is_enabler = ref.d_is_enabler;
                d_is_disabler = ref.d_is_disabler;
                d_is_stopper = ref.d_is_stopper;
                d_stop_message = ref.d_stop_message;
                d_stop_message_type = ref.d_stop_message_type;
                d_disabled_unblocked_message_types = ref.d_disabled_unblocked_message_types;
            }
            ProcessBufferMessageTypeHint& operator=(const ProcessBufferMessageTypeHint& ref) {
                if (this==&ref) return *this;

                d_message_type = ref.d_message_type;
                d_regexp = ref.d_regexp;
                d_priority = ref.d_priority;
                d_is_enabler = ref.d_is_enabler;
                d_is_disabler = ref.d_is_disabler;
                d_is_stopper = ref.d_is_stopper;
                d_stop_message = ref.d_stop_message;
                d_stop_message_type = ref.d_stop_message_type;
                d_disabled_unblocked_message_types = ref.d_disabled_unblocked_message_types;

                return *this;
            }
            bool operator==(const ProcessBufferMessageTypeHint& ref) const {
                if (d_message_type != ref.d_message_type)
                    return false;
                if (d_regexp != ref.d_regexp)
                    return false;
                if (d_priority != ref.d_priority)
                    return false;
                if (d_is_enabler != ref.d_is_enabler)
                    return false;
                if (d_is_disabler != ref.d_is_disabler)
                    return false;
                if (d_is_stopper != ref.d_is_stopper)
                    return false;
                if (d_stop_message != ref.d_stop_message)
                    return false;
                if (d_stop_message_type != ref.d_stop_message_type)
                    return false;
                if (d_disabled_unblocked_message_types != ref.d_disabled_unblocked_message_types)
                    return false;

                return true;
            }
            bool operator!=(const ProcessBufferMessageTypeHint& ref) const {
                return !(*this==ref);
            }

            QRegExp                     d_regexp;
            Logger::MessageType         d_message_type;
            int                         d_priority;
            bool                        d_is_enabler;
            bool                        d_is_disabler;
            bool                        d_is_stopper;
            QString                     d_stop_message;
            Logger::MessageType         d_stop_message_type;
            Logger::MessageTypeFlags    d_disabled_unblocked_message_types;
        };

        /*!
        \struct QtilitiesProcessPrivateData
        \brief Structure used by QtilitiesProcess to store private data.
          */
        struct QtilitiesProcessPrivateData;

        /*!
        \class QtilitiesProcess
        \brief An easy to use way to launch external processes through an extended wrapper around QProcess.

        The QtilitiesProcess class simplifies usage of QProcess and provides ready to use logging and task integration capablities. It allows
        messages received from the backend process through \p stdout and \p stderr to be classified by type, filtered and then logged. In addition
        it can log details about the process itself. Logged messages are forwarded to the logger engine assigned to the Task base class, whether
        its a GUI logger widget, console, file etc.

        It is important to distinguish between information messages logged by QtilitiesProcess and messages received from the backend process being launched
        through \p stdout and \p stderr. Information messages are logged by QtilitiesProcess to give information about the backend process and can be
        enabled/disabled through processInfoMessagesEnabled(). Backend process messages are processed as described below and then logged and can be enabled/disabled
        through the \p read_process_buffers parameter on the constructor. To check if backend messages are processed use processBackendProcessBuffersEnabled().

        To enable logging (that is, whichever of the above messages are enabled), the \p enable_logging parameter on the consturctor must be set to true.
        When true, messages will be logged using the logMessage() function to the Task base class of QtilitiesProcess. To facilitate logging the process of
        logging the process output to a file, the assignFileLoggerEngineToProcess() convenience function can be used. For more information see the Qtilities::Core::Task class
        documentation.

        \section qtilities_process_buffering Buffering of process buffers

        When the \p read_process_buffers parameter in the QtilitiesProcess constructor is enabled and logging is enabled, the
        process buffers of the backend QProcess will be processed and logged to the Task base class using the logMessage() function.

        In addition QtilitiesProcess can buffer all messages from the backend process in a \"last run buffer\". This can
        be enabled using setLastRunBuffer() enabled. Note that the last run buffer can be used along with the processing
        of messages through \p read_process_buffers. The last run buffer can be cleared using clearLastRunBuffer() and accessed
        through lastRunBuffer(). The last run buffer is disabled by default.

        If \p read_process_buffers is false and the last run buffer is not used, the process buffer won't be touched and you can manually access it
        through the internal QIODevice exposed through the process() function.

        \subsection qtilities_process_buffering_default Classification of received messages

        Using setProcessBufferMessageTypeHint() it is possible to classify these individual messages as different types
        of messages. For example, if the backend process starts error messages with "ERROR:", it is possible to add a process
        buffer message type hint using the following regular expression:

\code
QRegExp reg_exp_error = QRegExp(QObject::tr("ERROR:") + "*",Qt::CaseInsensitive,QRegExp::Wildcard);
ProcessBufferMessageTypeHint message_hint_error(reg_exp_error,Logger::Error);
my_process.addProcessBufferMessageTypeHint(message_hint_error);
\endcode

        The processing logic will then know to log these messages as errors instead of normal information messages.

        Note that it is also possible to filter specific messages by assigning a message type of Logger::None in the ProcessBufferMessageTypeHint.
          */
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesProcess : public Task
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::ITask)

        public:
            //! Constructs a new QtilitiesProcess instance.
            /*!
             * \param task_name A name to identify this process. This name is used as the task name on the Task base class.
             * \param enable_logging Indicates if messages received from the backend QProcess (see and general process information messages
             * must logged to the Task base class of QtilitiesProcess.
             * \param read_process_buffers Indicates if messages in the process's buffers must be processed. When false, the process
             * buffer won't be touched and you can manually access it through the process() function.
             * \param parent The parent of this process.
             *
             * \note For more details on how QtilitiesProcess can buffer and log process message, see \ref qtilities_process_buffering.
             */
            QtilitiesProcess(const QString& task_name, bool enable_logging = true, bool read_process_buffers = true, QObject* parent = 0);
            virtual ~QtilitiesProcess();

            //! Access to the QProcess instance contained and used within this object.
            QProcess* process();

            //! Starts the process, when using QtiltiesProcess use this function instead of QProcess::start().
            /*!
              \param program The program to start.
              \param arguments The arguments to send to the QProcess.
              \param mode The OpenMode of the QProcess.
              \param wait_for_started_msecs The wait for started time in milli seconds to be passed to the waitForStarted() call on the QProcess().
              \param timeout When other than -1, the timeout in milli seconds for the process. If it has not completed before the timeout is reached, stop() will be called on the task associated with this process.
              \returns True when the task was started successfully (thus waitForStarted() returned true), false otherwise.
              */
            virtual bool startProcess(const QString& program,
                                      const QStringList& arguments,
                                      QProcess::OpenMode mode = QProcess::ReadWrite,
                                      int wait_for_started_msecs = 30000,
                                      int timeout_msecs = -1);

            // --------------------------------------------------------
            // Convenience Logger Setup Functions
            // --------------------------------------------------------
            //! Assigns a file logger engine to the process, causing all messages from the process to be logged to the specified file.
            /*!
             * This function assigns a file logger engine to the process and forwards all log messages received from the backend process to
             * this file. This is achieved by calling ITask::setCustomLoggerEngine() on the process.
             *
             * \param file_path The file path of the file to log to.
             * \param log_only_to_file When true, all messages will only be logged to this file instead of to this file and the system wide context. Setting this
             * parameter to true is equivalent to setting the log context (see ITask::setLogContext()) of the process to Logger::EngineSpecificMessages, as well
             * as calling ITask::setCustomLoggerEngine() with its use_only_engine_parameter to true. When setting parameter to false however, the log context of the process is left untouched.
             * \param engine_name The engine name used to create a new engine. The function will first check if any existing engines log to the specified file,
             * and if an existing engine already logs to the specified file, the existing engine will be used and the \p engine_name parameter will be ignored. Alternatively
             * a new file engine will be created for the process using the engine name when specified, if not specified the base name of the file will be used as the engine name.
             * If an engine with the same name already exists this function will assign an unique name to the new widget by appending a number to \p engine_name.
             *
             * \note Call this function before starting the process.
             * \note When creating a new file logger engine, the process takes ownership of the new engine. However, if a logger engine logging to this
             * file is already in use, the process will not take ownership of the engine.
             *
             * \returns A reference to the newly constructed logger engine when successfull, 0 otherwise with the reason why it failed available using errorMsg.
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            AbstractLoggerEngine* assignFileLoggerEngineToProcess(const QString &file_path, bool log_only_to_file = false, QString *engine_name = 0, QString* errorMsg = 0);

            // --------------------------------------------------------
            // Process Buffer
            // --------------------------------------------------------
            //! Sets a regular expression used to associate messages received from the process buffer with logger message types.
            /*!
             * When the read_process_buffers parameter in the QtilitiesProcess constructor is enabled, the
             * process buffers of the backend QProcess will be processed. Processed messages
             * will be emitted using newStandardErrorMessage() and newStandardErrorMessage(), and if the enable_logging
             * parameter in the QtilitiesProcess was enabled, the processed messages will also be logged to the task
             * representing the process.
             *
             * For more details on how QtilitiesProcess can buffer and log process message, see \ref qtilities_process_buffering.
             *
             * \code
             * QRegExp reg_exp_error = QRegExp(QObject::tr("ERROR:") + "*",Qt::CaseInsensitive,QRegExp::Wildcard);
             * ProcessBufferMessageTypeHint message_hint_error(reg_exp_error,Logger::Error);
             * my_process.addProcessBufferMessageTypeHint(message_hint_error);
             * \endcode
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             *
             * \note Prior to %Qtilities v1.5, error messages were identified as messages starting QRegExp(QObject::tr("ERROR:") + "*",Qt::CaseInsensitive,QRegExp::Wildcard),
             * and warning message were identified as messages starting with QRegExp(QObject::tr("WARNING:") + "*",Qt::CaseInsensitive,QRegExp::Wildcard).
             * There was no way to customize this before this function was added.
             */
            void addProcessBufferMessageTypeHint(ProcessBufferMessageTypeHint hint);
            //! Gets if process information log messages are enabled.
            /*!
             * \return True when enabled, false otherwise.
             *
             * \note Logging must be enabled using the \p enable_logging parameter of the constructor in order for backend information
             *  messages to appear. To check if logging is enabled use loggingEnabled().
             *
             * \sa The \p read_process_buffers parameter on the constructor.
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            bool processBackendProcessBuffersEnabled() const;
            //! Sets the backend buffer UI refresh frequency.
            /*!
             * In cases where the backend buffer dumps large amounts of data into its STDOUT or STDERR buffers,
             * the message processing hint based processing of the dump can cause applications where QtilitiesProcess
             * lives in the GUI process to become unresponsive for short periods. To counter this, it is possible
             * to set the buffer UI refresh frequency which is the number of lines to process in the buffer before
             * issuing an UI refresh as shown below:
             *
             * \code
             * QCoreApplication::processEvents(QEventLoop::ExcludeSocketNotifiers);
             * \endcode
             *
             * To disable any UI refreshes, set the refresh frequency to 0. By default, refreshing is disabled.
             */
            void setGuiRefreshFrequency(int refresh_frequency);
            //! Gets the backend buffer UI refresh frequency.
            int guiRefreshFrequency() const;

            // --------------------------------------------------------
            // Process Information Messages
            // --------------------------------------------------------
            //! Sets if process information log messages are enabled.
            /*!
             * This includes messages such as:
             * - Executing Process: ...
             * - > working directory of process: ...
             * - > Task completed successfully
             * etc.
             *
             * When disabled, only messages received from the spawned process will be logged. Enabled by default.
             *
             * \note Logging must be enabled using the \p enable_logging parameter of the constructor in order for process information
             *  messages to appear. To check if logging is enabled use loggingEnabled().
             *
             * \sa setProcessInfoMessagesEnabled()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void setProcessInfoMessagesEnabled(bool is_enabled);
            //! Gets if process information log messages are enabled.
            /*!
             * \return True when enabled, false otherwise.
             *
             * \note Logging must be enabled using the \p enable_logging parameter of the constructor in order for process information
             *  messages to appear. To check if logging is enabled use loggingEnabled().
             *
             * \sa setProcessInfoMessagesEnabled()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            bool processInfoMessagesEnabled() const;

            // --------------------------------------------------------
            // Last Run Buffer
            // --------------------------------------------------------
            //! Sets if the last run buffer is enabled for this process.
            /*!
             * \param is_enabled True if it must be enabled, false otherwise.
             *
             * \note For the last run buffer to work, the \p read_process_buffer parameter of the constructor must be set to true.
             *
             * \sa lastRunBufferEnabled(), lastRunBuffer(), clearLastRunBuffer()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void setLastRunBufferEnabled(bool is_enabled);
            //! Gets if the last run buffer is enabled for this process.
            /*!
             * The last run buffer is disabled by default.
             *
             * \return True if enabled, false otherwise.
             *
             * \note For the last run buffer to work, the \p read_process_buffer parameter of the constructor must be set to true.
             *
             * \sa setLastRunBufferEnabled(), lastRunBuffer(), clearLastRunBuffer()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            bool lastRunBufferEnabled() const;
            //! Gets the last run buffer.
            /*!
             * QtilitiesProcess can buffer all messages from the backend process in a \"last run buffer\". This can
             * be enabled using setLastRunBuffer() enabled. The last run buffer can be cleared using clearLastRunBuffer() and accessed
             * through lastRunBuffer(). The last run buffer is disabled by default.
             *
             * \sa setLastRunBufferEnabled(), lastRunBufferEnabled(), clearLastRunBuffer()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            QByteArray lastRunBuffer() const;
            //! Clears the last run buffer.
            /*!
             * \sa setLastRunBufferEnabled(), lastRunBufferEnabled(), lastRunBuffer()
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            void clearLastRunBuffer();

        protected slots:
            void manualAppendLastRunBuffer();
            void readStandardOutput();
            void readStandardError();

        private slots:
            void procFinished(int exit_code, QProcess::ExitStatus exit_status);
            void procError(QProcess::ProcessError error);
            void stopTimedOut();

        public slots:
            //! Stops the process.
            /*!
             * This function will first call terminate() on the process, wait and then call kill().
             * \note When reimplementing this function, it is important to call Task::stop() at the end of your implementation.
             */
            virtual void stopProcess();

        protected:
            virtual void processSingleBufferMessage(const QString &buffer_message, Logger::MessageType msg_type);

        private:
            QtilitiesProcessPrivateData* d;
        };
    }
}

#endif // QTILITIES_PROCESS_H
