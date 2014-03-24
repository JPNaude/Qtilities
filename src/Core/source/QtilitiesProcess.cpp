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

#include "QtilitiesProcess.h"

#include <QCoreApplication>
#include <FileUtils>
#include <QRegExp>

#include <Logger>
#include <LoggerEngines.h>
using namespace Qtilities::Logging;

struct Qtilities::Core::QtilitiesProcessPrivateData {
    QtilitiesProcessPrivateData() : process(0),
        read_process_buffers(false),
        last_run_buffer_enabled(false),
        process_info_messages_enabled(true),
        message_disabler_active(false),
        ignore_read_buffer_slot(false) {}

    QProcess* process;
    QString default_qprocess_error_string;
    QList<ProcessBufferMessageTypeHint> buffer_message_type_hints;
    bool read_process_buffers;
    QByteArray last_run_buffer;
    bool last_run_buffer_enabled;
    bool process_info_messages_enabled;
    bool message_disabler_active;
    bool ignore_read_buffer_slot;
};

Qtilities::Core::QtilitiesProcess::QtilitiesProcess(const QString& task_name,
                                                    bool enable_logging,
                                                    bool read_process_buffers,
                                                    QObject* parent) : Task(task_name,enable_logging,parent)
{
    d = new QtilitiesProcessPrivateData;
    d->process = new QProcess;
    d->default_qprocess_error_string = d->process->errorString();

    connect(this, SIGNAL(stopTaskRequest()), this, SLOT(stopProcess()));
    connect(d->process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procError(QProcess::ProcessError)));
    connect(d->process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procFinished(int, QProcess::ExitStatus)));

    d->read_process_buffers = read_process_buffers;
    if (d->read_process_buffers) {
        connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
        connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    } else {
        // If the buffers are not processed, we need to read them and append to the last buffer manually,
        // if they are processed, the readStandardOutput() function will update the
        // last run buffer for us.
        connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(manualAppendLastRunBuffer()));
        connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(manualAppendLastRunBuffer()));
    }

    setCanStop(true);
}

Qtilities::Core::QtilitiesProcess::~QtilitiesProcess() {
    if (d->process) {
        if (state() == ITask::TaskBusy)
            completeTask();
        d->process->kill();
        d->process->deleteLater();
    }
    delete d;
}

QProcess* Qtilities::Core::QtilitiesProcess::process() {
    return d->process;
}

void Qtilities::Core::QtilitiesProcess::addProcessBufferMessageTypeHint(ProcessBufferMessageTypeHint hint) {
    d->buffer_message_type_hints.append(hint);
}

void Qtilities::Core::QtilitiesProcess::setProcessInfoMessagesEnabled(bool is_enabled) {
    d->process_info_messages_enabled = is_enabled;
}

bool Qtilities::Core::QtilitiesProcess::processInfoMessagesEnabled() const {
    return d->process_info_messages_enabled;
}

void Qtilities::Core::QtilitiesProcess::setLastRunBufferEnabled(bool is_enabled) {
    d->last_run_buffer_enabled = is_enabled;
}

bool Qtilities::Core::QtilitiesProcess::lastRunBufferEnabled() const {
    return d->last_run_buffer_enabled;
}

QByteArray Qtilities::Core::QtilitiesProcess::lastRunBuffer() const {
    return d->last_run_buffer;
}

void Qtilities::Core::QtilitiesProcess::clearLastRunBuffer() {
    d->last_run_buffer.clear();
}

void Qtilities::Core::QtilitiesProcess::manualAppendLastRunBuffer() {
    if (d->last_run_buffer_enabled)  {
        while (d->process->canReadLine())
            d->last_run_buffer.append(d->process->readLine().constData());
    }
}

bool Qtilities::Core::QtilitiesProcess::startProcess(const QString& program,
                                                     const QStringList& arguments,
                                                     QProcess::OpenMode mode,
                                                     int wait_for_started_msecs,
                                                     int timeout_msecs) {
    if (state() == ITask::TaskPaused)
        return false;

    if (state() != ITask::TaskBusy)
        startTask();

    QString native_program = FileUtils::toNativeSeparators(program);
    if (d->process_info_messages_enabled) {
        // Check if program exists:
        QFileInfo fi1(program);
        if (!fi1.exists() && fi1.isAbsolute()) {
            // Check if program.exe exists:
            QFileInfo fi2(program + ".exe");
            if (!fi2.exists() && fi2.isAbsolute()) {
                // Check if program.bat exists:
                QFileInfo fi3(program + ".bat");
                if (!fi3.exists() && fi3.isAbsolute())
                    logWarning(QString("Failed to find application \"%1\". An attempt to launch it will still be made.").arg(program));
            }
        }

        logMessage(tr("Executing Process: ") + native_program + " " + arguments.join(" "));
        if (d->process->workingDirectory().isEmpty()) {
            logMessage(QString("> working directory of process: %1").arg(QDir::current().path()));
        } else {
            logMessage(QString("> working directory of process: %1").arg(d->process->workingDirectory()));
            QDir dir(d->process->workingDirectory());
            if (!dir.exists())
                logWarning("> working directory does not exist, process might fail to start.");
        }


        logMessage("");
    }

    clearLastRunBuffer();
    d->process->start(native_program, arguments, mode);

    if (!d->process->waitForStarted(wait_for_started_msecs)) {
        if (d->process_info_messages_enabled)
            logMessage("Failed to start \"" + native_program + "\".", Logger::Error);
        if (state() == ITask::TaskBusy)
            completeTask();

        d->process->waitForFinished();
        return false;
    } else {
        if (timeout_msecs > 0) {
            QTimer* timer = new QTimer;
            timer->setSingleShot(true);
            timer->start(timeout_msecs);
            connect(timer,SIGNAL(timeout()),SLOT(stop()));
            connect(d->process,SIGNAL(finished(int)),timer,SLOT(deleteLater()));
            if (d->process_info_messages_enabled)
                logMessage(QString("A %1 msec timeout was specified for this process. It will be stopped if not completed before the timeout was reached.").arg(timeout_msecs));
        }
    }

    return true;
}

void Qtilities::Core::QtilitiesProcess::assignFileLoggerEngineToProcess(const QString &file_path, bool log_only_to_file, QString *engine_name) {
    if (file_path.isEmpty())
        return;

    // First check if any existing engines use this file path:
    AbstractLoggerEngine* engine = Log->loggerEngineReferenceForFile(file_path);
    FileLoggerEngine* fe = qobject_cast<FileLoggerEngine*> (engine);
    if (!engine || !fe) {
        QString new_logger_name;
        if (!engine_name) {
            QFileInfo fi(file_path);
            new_logger_name = fi.baseName();
        } else
            new_logger_name = *engine_name;

        int logger_count = -1;
        QStringList attached_logger_engine_names = Log->attachedLoggerEngineNames();
        while (attached_logger_engine_names.contains(new_logger_name)) {
            if (logger_count > -1)
                new_logger_name.chop(3 + QString::number(logger_count).length());
            ++logger_count;
            new_logger_name.append(" (" + QString::number(logger_count) + ")");
        }

        engine = Log->newFileEngine(new_logger_name,file_path);
        engine->setMessageContexts(Logger::EngineSpecificMessages);
    }

    // Set the log context:
    if (log_only_to_file)
        setLogContext(Logger::EngineSpecificMessages);

    // Set the custom log engine of the process:
    setCustomLoggerEngine(engine,log_only_to_file);
}

bool Qtilities::Core::QtilitiesProcess::processBackendProcessBuffersEnabled() const {
    return d->read_process_buffers;
}

void Qtilities::Core::QtilitiesProcess::stopProcess() {
    d->process->terminate();
    //d->process->waitForFinished(3000);
    d->process->kill();
    d->process->waitForFinished(3000);

    if (state() == ITask::TaskBusy)
        completeTask();
}

void Qtilities::Core::QtilitiesProcess::procFinished(int exit_code, QProcess::ExitStatus exit_status) {
    // Read whatever is in left in the process buffer:
    if (d->read_process_buffers) {
        QString msg = d->process->readAllStandardOutput();
        if (!msg.isEmpty()) {
            QStringList splits = msg.split("\n");
            foreach (const QString& split, splits) {
                processSingleBufferMessage(split,Logger::Info);
                if (d->last_run_buffer_enabled) {
                    d->last_run_buffer.append(split.toUtf8());
                    d->last_run_buffer.append("\n");
                }
            }
        }
        msg = d->process->readAllStandardError();
        if (!msg.isEmpty()) {
            QStringList splits = msg.split("\n");
            foreach (const QString& split, splits) {
                processSingleBufferMessage(split,Logger::Info);
                if (d->last_run_buffer_enabled) {
                    d->last_run_buffer.append(split.toUtf8());
                    d->last_run_buffer.append("\n");
                }
            }
        }
    }

    if (exit_code != 0) {
        QString error_string = d->process->errorString();
        if (error_string != d->default_qprocess_error_string)
            logError(error_string);

        if (d->process_info_messages_enabled) {
            if (exit_status == QProcess::NormalExit)
                logMessage("Process " + taskName() + " exited normal with code " + QString::number(exit_code),Logger::Error);
            else if (exit_status == QProcess::CrashExit)
                logMessage("Process " + taskName() + " crashed with code " + QString::number(exit_code),Logger::Error);
        }
    }

    if (state() == ITask::TaskBusy || state() == ITask::TaskPaused)
        completeTask();
}

void Qtilities::Core::QtilitiesProcess::procError(QProcess::ProcessError error) {
    if (!d->process_info_messages_enabled)
        return;

    switch (error)
    {
        case QProcess::FailedToStart:
        {
            // If the working directory of the process does not exist, we will get in here as well. So handle that case specifically to give a more descriptive message:
            QDir working_dir(d->process->workingDirectory());
            if (working_dir.exists())
                logMessage("Process \"" + taskName() + "\" failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.",Logger::Error);
            else
                logMessage("Process \"" + taskName() + "\" failed to start. The working directory of the process does not exist at: " + d->process->workingDirectory(),Logger::Error);
            break;
        }
        case QProcess::Crashed:
            logMessage("Process \"" + taskName() + "\" crashed some time after starting successfully.",Logger::Error);
            break;
        case QProcess::Timedout:
            logMessage("The last waitFor...() function of process \"" + taskName() + "\" timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.",Logger::Error);
            break;
        case QProcess::WriteError:
            logMessage("An error occurred when attempting to write to process \"" + taskName() + "\". For example, the process may not be running, or it may have closed its input channel.",Logger::Error);
            break;
        case QProcess::ReadError:
            logMessage("An error occurred when attempting to read from process \"" + taskName() + "\". For example, the process may not be running.",Logger::Error);
            break;
        case QProcess::UnknownError:
            logMessage("Process \"" + taskName() + "\" failed with an unknown error.",Logger::Error);
            break;
        default:
            logMessage("Process \"" + taskName() + "\" failed with an unknown error.",Logger::Error);
    }
}

void Qtilities::Core::QtilitiesProcess::readStandardOutput() {
    if (d->ignore_read_buffer_slot)
        return;

    int msg_count = 0;
    while (d->process->canReadLine()) {
        ++msg_count;
        if (msg_count > 20) {
            msg_count = 0;
            // To keep GUI applications responsive in case the backend process dumps tons of data for us.
            d->ignore_read_buffer_slot = true;
            QCoreApplication::processEvents(QEventLoop::ExcludeSocketNotifiers);
            d->ignore_read_buffer_slot = false;
        }

        QByteArray ba = d->process->readLine();
        processSingleBufferMessage(ba,Logger::Info);
        if (d->last_run_buffer_enabled)
            d->last_run_buffer.append(ba);
    }
}

void Qtilities::Core::QtilitiesProcess::readStandardError() {
    if (d->ignore_read_buffer_slot)
        return;

    int msg_count = 0;
    while (d->process->canReadLine()) {
        ++msg_count;
        if (msg_count > 20) {
            msg_count = 0;
            // To keep GUI applications responsive in case the backend process dumps tons of data for us.
            d->ignore_read_buffer_slot = true;
            QCoreApplication::processEvents(QEventLoop::ExcludeSocketNotifiers);
            d->ignore_read_buffer_slot = false;
        }

        QByteArray ba = d->process->readLine();
        processSingleBufferMessage(ba,Logger::Error);
        if (d->last_run_buffer_enabled)
            d->last_run_buffer.append(ba);
    }
}

void Qtilities::Core::QtilitiesProcess::processSingleBufferMessage(const QString &buffer_message, Logger::MessageType msg_type) {
    // If logging is disabled, we can skip the processing of the buffer message altogether:
    if (loggingEnabled()) {
        if (d->buffer_message_type_hints.isEmpty())
            logMessage(buffer_message,msg_type);
        else {
            bool found_match = false;
            // Get all hints that match the message:
            QListIterator<ProcessBufferMessageTypeHint> itr(d->buffer_message_type_hints);
            int highest_matching_hint_priority = -1;
            QList<ProcessBufferMessageTypeHint> matching_hints;
            while (itr.hasNext()) {
                ProcessBufferMessageTypeHint hint = itr.next();
                if (hint.d_regexp.exactMatch(buffer_message)) {
                    if (hint.d_priority >= highest_matching_hint_priority) {
                        highest_matching_hint_priority = hint.d_priority;
                        matching_hints << hint;
                    }
                }
            }

            // Next, log the message using all hints that match the highest_matching_hint_priority:
            QListIterator<ProcessBufferMessageTypeHint> itr_log(matching_hints);
            while (itr_log.hasNext()) {
                ProcessBufferMessageTypeHint hint = itr_log.next();

                bool log_this_message = !d->message_disabler_active;
                if (d->message_disabler_active) {
                    // When a message disabler is active:
                    if (hint.d_is_enabler) {
                        d->message_disabler_active = false;
                        log_this_message = hint.d_is_enabler_log_match;
                    }
                } else {
                    // When a message disabler is not active:
                    if (hint.d_is_disabler) {
                        d->message_disabler_active = true;
                        log_this_message = hint.d_is_disabler_log_match;
                    }
                }

                if (log_this_message) {
                    if (hint.d_priority == highest_matching_hint_priority) {
                        found_match = true;
                        if (hint.d_message_type != Logger::None)
                            logMessage(buffer_message,hint.d_message_type);
                    }
                }
            }

            if (!found_match && !d->message_disabler_active)
                logMessage(buffer_message,msg_type);
        }
    }
}
