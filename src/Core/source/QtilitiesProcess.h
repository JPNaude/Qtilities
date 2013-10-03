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
          \struct ProcessBufferMessageTypeHint
          \brief The ProcessBufferMessageTypeHint structure is used to define custom hints for process buffer message processing in QtilitiesProcess.

          A ProcessBufferMessageTypeHint contains the following information:
          - The regular expression matched to the single message line after processing has been done by QtilitiesProcess (see \ref qtilities_process_buffering).
          - The message type to assign to matched messages.
          - A priority for the match. If multiple hints match a message, the hint with the highest priority wil be used to log the message. If multiple hints match which have the same priority, the message will be linked using multiple times (where each logged message will be logged using the type specified by the applicable matching hint).
         */
        struct ProcessBufferMessageTypeHint {
        public:
            ProcessBufferMessageTypeHint(const QRegExp& regexp,
                                         Logger::MessageType message_type,
                                         int priority = 0) {
                d_regexp = regexp;
                d_message_type = message_type;
                d_priority = priority;
            }
            ProcessBufferMessageTypeHint(const ProcessBufferMessageTypeHint& ref) {
                d_message_type = ref.d_message_type;
                d_regexp = ref.d_regexp;
                d_priority = ref.d_priority;
            }
            ProcessBufferMessageTypeHint& operator=(const ProcessBufferMessageTypeHint& ref) {
                if (this==&ref) return *this;

                d_message_type = ref.d_message_type;
                d_regexp = ref.d_regexp;
                d_priority = ref.d_priority;

                return *this;
            }
            bool operator==(const ProcessBufferMessageTypeHint& ref) {
                if (d_message_type != ref.d_message_type)
                    return false;
                if (d_regexp != ref.d_regexp)
                    return false;
                if (d_priority != ref.d_priority)
                    return false;

                return true;
            }
            bool operator!=(const ProcessBufferMessageTypeHint& ref) {
                return !(*this==ref);
            }

            QRegExp                     d_regexp;
            Logger::MessageType         d_message_type;
            int                         d_priority;
        };

        /*!
        \struct QtilitiesProcessPrivateData
        \brief Structure used by QtilitiesProcess to store private data.
          */
        struct QtilitiesProcessPrivateData;

        /*!
        \class QtilitiesProcess
        \brief An easy to use way to launch external processes through an extended wrapper around QProcess.

        The QtilitiesProcess class simplifies usage of QProcess and provides ready to use logging and task integration capablities.

        When logging is enabled, QtilitiesProcess will automatically log all \p stdout and \p stderr outputs in a logger engine.
        It takes care of splitting up messages received from the QProcess buffer for you, thus individual messages are logged to the
        logger engine.

        \section qtilities_process_buffering Buffering of process buffers

        When the \p read_process_buffers parameter in the QtilitiesProcess constructor is enabled, the
        process buffers of the backend QProcess will be processed. Processed messages
        will be emitted using newStandardErrorMessage() and newStandardErrorMessage(), and if the enable_logging
        parameter in the QtilitiesProcess was enabled, the processed messages will also be logged to the task
        representing the process.

        In addition, QtilitiesProcess can buffer all messages from the backend process in a \"last run buffer\". This can
        be enabled using setLastRunBuffer() enabled. Note that the last run buffer can be used along with the processing
        of messages through \p read_process_buffers. The last run buffer can be cleared using clearLastRunBuffer() and accessed
        through lastRunBuffer(). The last run buffer is disabled by default.

        If \p read_process_buffers is false and the last run buffer is not used, the process buffer won't be touched and you can manually access it
        through the internal QIODevice exposed through the process() function.

        Processing of messages allows QtilitiesProcess to properly log messages received from the backend process
        using the %Qtilities logger.

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
          */
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesProcess : public Task
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::ITask)

        public:
            //! Constructs a new QtilitiesProcess instance.
            /*!
             * \param task_name The name of the task.
             * \param enable_logging Indicates if messages received from the backend QProcess must be buffered and logged to a task log assigned to this task.
             * \param read_process_buffers Indicates if messages in the process's buffers must be processed. When false, the process buffer won't be touched and you can manually access it through the process() function.
             * \param parent The parent of this process.
             *
             * \note For more details on how QtilitiesProcess can buffer and log process message, see \ref qtilities_process_buffering.
             */
            QtilitiesProcess(const QString& task_name, bool enable_logging = true, bool read_process_buffers = true, QObject* parent = 0);
            virtual ~QtilitiesProcess();

            //! Starts the process, similar to QProcess::start().
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

            //! Access to the QProcess instance contained and used within this object.
            QProcess* process();

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

            //! Sets if the last run buffer is enabled for this process.
            /*!
             * \param is_enabled True if it must be enabled, false otherwise.
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
