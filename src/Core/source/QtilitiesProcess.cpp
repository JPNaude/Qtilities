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

using namespace Qtilities::Logging;

struct Qtilities::Core::QtilitiesProcessPrivateData {
    QtilitiesProcessPrivateData() : process(0),
        read_process_buffers(false),
        last_run_buffer_enabled(false) { }

    QProcess* process;
    QString default_qprocess_error_string;
    QList<ProcessBufferMessageTypeHint> buffer_message_type_hints;
    bool read_process_buffers;
    QByteArray last_run_buffer;
    bool last_run_buffer_enabled;
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

    QString native_program = FileUtils::toNativeSeparators(program);
    logMessage(tr("Executing Process: ") + native_program + " " + arguments.join(" "));
    if (d->process->workingDirectory().isEmpty()) {
        logMessage(QString("> working directory of process: %1").arg(QDir::current().path()));
    } else {
        logMessage(QString("> working directory of process: %1").arg(d->process->workingDirectory()));
        QDir dir(d->process->workingDirectory());
        if (!dir.exists())
            logWarning("> working directory does not exist, process might fail to start.");
    }

    QTimer timer;
    if (timeout_msecs > 0) {
        timer.setSingleShot(true);
        timer.start(timeout_msecs);
        connect(&timer,SIGNAL(timeout()),SLOT(stop()));
        logMessage(QString("A %1 msec timeout was specified for this process. It will be stopped if not completed before the timeout was reached.").arg(timeout_msecs));
    }

    logMessage("");
    clearLastRunBuffer();
    d->process->start(native_program, arguments, mode);

    if (!d->process->waitForStarted(wait_for_started_msecs)) {
        logMessage("Failed to start " + native_program + ". Make sure the executable is visible in your system's paths.", Logger::Error);
        if (state() == ITask::TaskBusy)
            completeTask();

        d->process->waitForFinished();
        return false;
    }

    return true;
}

void Qtilities::Core::QtilitiesProcess::stopProcess() {
    d->process->terminate();
    d->process->waitForFinished(3000);
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
            foreach (const QString& split, splits)
                processSingleBufferMessage(split,Logger::Info);
        }
        msg = d->process->readAllStandardError();
        if (!msg.isEmpty()) {
            QStringList splits = msg.split("\n");
            foreach (const QString& split, splits)
                processSingleBufferMessage(split,Logger::Info);
        }
    }

    if (exit_code != 0) {
        QString error_string = d->process->errorString();
        if (error_string != d->default_qprocess_error_string)
            logError(error_string);

        if (exit_status == QProcess::NormalExit)
            logMessage("Process " + taskName() + " exited normal with code " + QString::number(exit_code),Logger::Error);
        else if (exit_status == QProcess::CrashExit)
            logMessage("Process " + taskName() + " crashed with code " + QString::number(exit_code),Logger::Error);
    }

    if (state() == ITask::TaskBusy || state() == ITask::TaskPaused)
        completeTask();
}

void Qtilities::Core::QtilitiesProcess::procError(QProcess::ProcessError error) {
    switch (error)
    {
        case QProcess::FailedToStart:
            logMessage("Process \"" + taskName() + "\" failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.",Logger::Error);
            break;
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
    while (d->process->canReadLine()) {
        QByteArray ba = d->process->readLine();
        processSingleBufferMessage(ba,Logger::Info);
        if (d->last_run_buffer_enabled)
            d->last_run_buffer.append(ba);
    }
}

void Qtilities::Core::QtilitiesProcess::readStandardError() {
    while (d->process->canReadLine()) {
        QByteArray ba = d->process->readLine();
        processSingleBufferMessage(ba,Logger::Error);
        if (d->last_run_buffer_enabled)
            d->last_run_buffer.append(ba);
    }
}

void Qtilities::Core::QtilitiesProcess::processSingleBufferMessage(const QString &buffer_message, Logger::MessageType msg_type) {
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
            if (hint.d_priority == highest_matching_hint_priority) {
                found_match = true;
                logMessage(buffer_message,hint.d_message_type);
            }
        }

        if (!found_match)
            logMessage(buffer_message,msg_type);
    }
}
