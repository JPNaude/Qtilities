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
         */
        struct ProcessBufferMessageTypeHint {
        public:
            ProcessBufferMessageTypeHint(const QRegExp& regexp,
                                         Logger::MessageType message_type) {
                d_regexp = regexp;
                d_message_type = message_type;
            }
            ProcessBufferMessageTypeHint(const ProcessBufferMessageTypeHint& ref) {
                d_message_type = ref.d_message_type;
                d_regexp = ref.d_regexp;
            }
            ProcessBufferMessageTypeHint& operator=(const ProcessBufferMessageTypeHint& ref) {
                if (this==&ref) return *this;

                d_message_type = ref.d_message_type;
                d_regexp = ref.d_regexp;

                return *this;
            }
            bool operator==(const ProcessBufferMessageTypeHint& ref) {
                if (d_message_type != ref.d_message_type)
                    return false;
                if (d_regexp != ref.d_regexp)
                    return false;

                return true;
            }
            bool operator!=(const ProcessBufferMessageTypeHint& ref) {
                return !(*this==ref);
            }

            QRegExp                     d_regexp;
            Logger::MessageType         d_message_type;
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

        When the read_process_buffers parameter in the QtilitiesProcess constructor is enabled, the
        process buffers of the backend QProcess will be processed. Processed messages
        will be emitted using newStandardErrorMessage() and newStandardErrorMessage(), and if the enable_logging
        parameter in the QtilitiesProcess was enabled, the processed messages will also be logged to the task
        representing the process.

        If read_process_buffers is false, the process buffer won't be touched and you can manually access it
        through the internal QIODevice exposed through the process() function.

        Processing of messages allows QtilitiesProcess to properly log messages received from the backend process
        using the %Qtilities logger. In many cases, messages received from the QIODevice used by QProcess are not nicely
        formatted. For example, readyReadStandardOutput() and readyReadStandardError() aren't emitted once for every message.
        They will normally be emitted with a buffered message which needs to be split up in order to identify individual messages.
        That is the goal of process buffering in QtilitiesProcess.

        \subsection qtilities_process_buffering_default Default buffer processing

        Default buffer processing is done when no line break strings are set which helps the processing logic
        to know which strings can be used to split up the individual messages. That is, when lineBreakStrings() is empty which
        is the default. The next section covers the case where custom line break strings have been set, enabling
        proper logging of process messages in more complicated situations.

        During default buffer processing, the process buffer is split into individual messages using a search for the
        new line character ("\n"). The text in the QStringList resulting from the split is then handled as individual messages.
        By default these individual messages are logged as normal info messages. However, using setProcessBufferMessageTypeHint() it
        is possible to classify these individual messages as different types of messages. For example, if the backend process
        starts error messages with "ERROR:", it is possible to add a process buffer message type hint using
        the following regular expression:

\code
QRegExp reg_exp_error = QRegExp(QObject::tr("ERROR:") + "*",Qt::CaseInsensitive,QRegExp::Wildcard);
ProcessBufferMessageTypeHint message_hint_error(reg_exp_error,Logger::Error);
my_process.addProcessBufferMessageTypeHint(message_hint_error);
\endcode

        The processing logic will then know to log these messages as errors instead of normal information messages.

        \subsection qtilities_process_buffering_line_break_strings Custom line break processing

        Using setLineBreakStrings() it is possible to specify custom strings which are used to split the process buffer
        into individual messages. This extra functionality is required in cases where individual messages are split across
        multiple lines. For example, if an error message is split accross multiple line, the standard buffer processing logic
        will log the first line of the message as an error, and the next line as a normal information message.

        To solve this problem, custom line break strings can be provided in order to split messages not usign the line
        break character, but using any of the specified line break strings. In order for the processing logic to work
        properly, it is required that the backend process always provide data in a consitant way. For example, info messages
        must start with a known string, warnings with another known string and errors with another known string. These known
        strings can be set as the line break strings on the QtilitiesProcess and all text between these known strings will be logged
        as individual messages.
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
              \returns True when the task was started successfully (thus waitForStarted() returned true), false otherwise.
              */
            virtual bool startProcess(const QString& program,
                                      const QStringList& arguments,
                                      QProcess::OpenMode mode = QProcess::ReadWrite,
                                      int wait_for_started_msecs = 30000);

            //! Access to the QProcess instance contained and used within this object.
            QProcess* process();

            //! Sets the strings to be used for splitting logged messages.
            /*!
              By default this list is empty an a line break character \r is used to received split messages.

              You can however specify multiple strings to use using this function.

              <i>This function was added in %Qtilities v1.1.</i>

              \sa lineBreakStrings()
              */
            void setLineBreakStrings(const QStringList& line_break_strings);
            //! Gets the strings to be used for splitting logged messages.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>

              \sa setLineBreakStrings()
              */
            QStringList lineBreakStrings();
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

        protected slots:
            //! Function connected to the
            /*!
             * \note For more details on how QtilitiesProcess can buffer and log process message, see \ref qtilities_process_buffering.
             */
            virtual void logProgressOutput();
            //! Function connected to the
            /*!
             * \note For more details on how QtilitiesProcess can buffer and log process message, see \ref qtilities_process_buffering.
             */
            virtual void logProgressError();

        private slots:
            void procStarted();
            void procFinished(int exit_code, QProcess::ExitStatus exit_status);
            void procError(QProcess::ProcessError error);
            void procStateChanged(QProcess::ProcessState newState);

        public slots:
            //! Stops the process.
            /*!
                This function will first call terminate() on the process, wait and then call kill().

                Its important to call Task::stop() at the end of your implementation.
            */
            virtual void stopProcess();

        signals:
            //! Emitted when new standard output becomes available.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void newStandardOutputMessage(const QString& message);
            //! Emitted when new standard error becomes available.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void newStandardErrorMessage(const QString& message);

        private:
            //! Process buffer work function.
            void processSingleBufferMessage(const QString& buffer_message);
            //! Internal function used to complete the task. Thus function will also process any remaining messages in the process buffer.
            void completeTaskExt();

            QtilitiesProcessPrivateData* d;
        };
    }
}

#endif // QTILITIES_PROCESS_H
