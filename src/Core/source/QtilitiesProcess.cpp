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
        last_run_buffer_enabled(false),
        read_process_buffers(true) { }

    QProcess* process;
    QString buffer_std_out;
    QString buffer_std_error;
    QStringList line_break_strings;
    QString default_qprocess_error_string;
    QList<ProcessBufferMessageTypeHint> buffer_message_type_hints;

    QMutex buffer_mutex_std_out;
    QMutex buffer_mutex_std_err;

    QByteArray last_run_buffer;
    bool last_run_buffer_enabled;
    bool read_process_buffers;
};

Qtilities::Core::QtilitiesProcess::QtilitiesProcess(const QString& task_name,
                                                    bool enable_logging,
                                                    bool read_process_buffers,
                                                    QObject* parent) : Task(task_name,enable_logging,parent)
{
    d = new QtilitiesProcessPrivateData;
    d->process = new QProcess;
    d->default_qprocess_error_string = d->process->errorString();

    connect(d->process, SIGNAL(started()), this, SLOT(procStarted()));
    connect(this, SIGNAL(stopTaskRequest()), this, SLOT(stopProcess()));

    connect(d->process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procError(QProcess::ProcessError)));
    connect(d->process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procFinished(int, QProcess::ExitStatus)));
    connect(d->process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(procStateChanged(QProcess::ProcessState)));

    d->read_process_buffers = read_process_buffers;
    if (d->read_process_buffers) {
        connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(logProgressOutput()));
        connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(logProgressError()));
    } else {
        // If the buffers are not processed, we need to read them and append to the last buffer manually,
        // if they are processed, the logProcessOutput() and logProcessError() functions will update the
        // last run buffer for us.
        connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(lastRunBufferAppendProgressOutput()));
        connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(lastRunBufferAppendProgressError()));
    }

    setCanStop(true);
}

Qtilities::Core::QtilitiesProcess::~QtilitiesProcess() {
    if (d->process) {
        if (state() == ITask::TaskBusy)
            completeTaskExt();
        d->process->kill();
        delete d->process;
    }
    delete d;
}

QProcess* Qtilities::Core::QtilitiesProcess::process() {
    return d->process;
}

void Qtilities::Core::QtilitiesProcess::setLineBreakStrings(const QStringList &line_break_strings) {
    d->line_break_strings = line_break_strings;
}

QStringList Qtilities::Core::QtilitiesProcess::lineBreakStrings() {
    return d->line_break_strings;
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

void Qtilities::Core::QtilitiesProcess::lastRunBufferAppendProgressOutput() {
    if (d->last_run_buffer_enabled)  {
        QByteArray new_output_ba = d->process->readAllStandardOutput();
        if (new_output_ba.isEmpty())
            return;

        d->last_run_buffer.append(new_output_ba);
    }
}

void Qtilities::Core::QtilitiesProcess::lastRunBufferAppendProgressError() {
    if (d->last_run_buffer_enabled)  {
        QByteArray new_output_ba = d->process->readAllStandardError();
        if (new_output_ba.isEmpty())
            return;

        d->last_run_buffer.append(new_output_ba);
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
                logWarning(QString(tr("Failed to find application \"%1\". An attempt to launch it will still be made.")).arg(program));
        }
    }

    QString native_program = FileUtils::toNativeSeparators(program);
    logMessage(tr("Executing Process: ") + native_program + " " + arguments.join(" "));
    if (d->process->workingDirectory().isEmpty())
        logMessage(tr("> working directory of process: %1").arg(QDir::current().path()));
    else
        logMessage(tr("> working directory of process: %1").arg(d->process->workingDirectory()));
    QDir dir(d->process->workingDirectory());
    if (!dir.exists())
        logWarning(tr("> working directory does not exist, process might fail to start."));

    QTimer timer;
    if (timeout_msecs > 0) {
        timer.setSingleShot(true);
        timer.start(timeout_msecs);
        connect(&timer,SIGNAL(timeout()),SLOT(stop()));
        logMessage(tr("A %1 msec timeout was specified for this process. It will be stopped if not completed before the timeout was reached.").arg(timeout_msecs));
    }

    logMessage("");
    d->process->start(native_program, arguments, mode);

    if (!d->process->waitForStarted(wait_for_started_msecs)) {
        logMessage("Failed to start " + native_program + ". Make sure the executable is visible in your system's paths.", Logger::Error);
        if (state() == ITask::TaskBusy)
            completeTaskExt();

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
        completeTaskExt();
}

void Qtilities::Core::QtilitiesProcess::procStarted() {

}

void Qtilities::Core::QtilitiesProcess::procFinished(int exit_code, QProcess::ExitStatus exit_status) {
    // Note that we log some empty messages here and when the process was started in order to
    // seperate the process's output and the task messages.
    if (exit_code == 0) {
        //logMessage("Process " + taskName() + " exited normal with code 0.");
    } else {
        if (exit_status == QProcess::NormalExit) {
            logMessage("");
            logMessage("Process " + taskName() + " exited normal with code " + QString::number(exit_code),Logger::Error);
        } else if (exit_status == QProcess::CrashExit) {
            logMessage("");
            logMessage("Process " + taskName() + " crashed with code " + QString::number(exit_code),Logger::Error);
        }
    }

//    bool current_active = true;
//    if (loggerEngine()) {
//        current_active = loggerEngine()->isActive();
//        loggerEngine()->setActive(false);
//    }
    if (state() == ITask::TaskBusy || state() == ITask::TaskPaused)
        completeTaskExt();
//    if (loggerEngine())
//        loggerEngine()->setActive(current_active);
    Q_UNUSED(exit_status)
}

void Qtilities::Core::QtilitiesProcess::procError(QProcess::ProcessError error) {
    switch (error)
    {
        case QProcess::FailedToStart:
            logMessage("Process " + taskName() + " failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.",Logger::Error);
            break;
        case QProcess::Crashed:
            logMessage("Process " + taskName() + " crashed some time after starting successfully.",Logger::Error);
            break;
        case QProcess::Timedout:
            logMessage("The last waitFor...() function of process " + taskName() + " timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.",Logger::Error);
            break;
        case QProcess::WriteError:
            logMessage("An error occurred when attempting to write to process " + taskName() + ". For example, the process may not be running, or it may have closed its input channel.",Logger::Error);
            break;
        case QProcess::ReadError:
            logMessage("An error occurred when attempting to read from process " + taskName() + ". For example, the process may not be running.",Logger::Error);
            break;
        case QProcess::UnknownError:
            logMessage("Process " + taskName() + " failed with an unknown error.",Logger::Error);
            break;
        default:
            logMessage("Process " + taskName() + " failed with an unknown error.",Logger::Error);
    }
}

void Qtilities::Core::QtilitiesProcess::procStateChanged(QProcess::ProcessState newState) {
    if (newState == QProcess::NotRunning && (state() == ITask::TaskBusy || state() == ITask::TaskPaused)) {
        QString error_string = d->process->errorString();
        if (error_string != d->default_qprocess_error_string)
            logError(error_string);
        completeTaskExt();
    }
}

void Qtilities::Core::QtilitiesProcess::logProgressOutput() {
    if (!d->read_process_buffers)
        return;

    QByteArray new_output_ba = d->process->readAllStandardOutput();
    if (new_output_ba.isEmpty())
        return;

    if (d->last_run_buffer_enabled)
        d->last_run_buffer.append(new_output_ba);

    QString new_output = QString(new_output_ba);

    d->buffer_mutex_std_out.lock();

    //qDebug() << Q_FUNC_INFO << new_output;
    emit newStandardOutputMessage(new_output);
    d->buffer_std_out.append(new_output);

    if (d->buffer_std_out.isEmpty()) {
        d->buffer_mutex_std_out.unlock();
        return;
    }

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \n and split messages up:
        split_list = d->buffer_std_out.split("\n");
        while (split_list.count() > 1) {
            processSingleBufferMessage(split_list.front().simplified());
            split_list.pop_front();
        }
    } else {
        // We loop through the string and replace all known break strings with &{_BREAKSTRING and then split
        // it using &{_:
        d->line_break_strings.append("\n");
        foreach (const QString& break_string, d->line_break_strings)
            d->buffer_std_out.replace(break_string,"&{_" + break_string);

        split_list = d->buffer_std_out.split("&{_");
        while (split_list.count() > 1) {
            QString msg = split_list.front().simplified();
            if (msg.startsWith("&{_"))
                msg = msg.remove(0,3);

            processSingleBufferMessage(msg);
            split_list.pop_front();
        }
    }

    if (split_list.isEmpty())
        d->buffer_std_out.clear();
    else
        d->buffer_std_out = split_list.front();

    d->buffer_mutex_std_out.unlock();
}

void Qtilities::Core::QtilitiesProcess::logProgressError() {   
    if (!d->read_process_buffers)
        return;

    QByteArray new_output_ba = d->process->readAllStandardOutput();
    if (new_output_ba.isEmpty())
        return;

    if (d->last_run_buffer_enabled)
        d->last_run_buffer.append(new_output_ba);

    QString new_output = QString(new_output_ba);

    d->buffer_mutex_std_err.lock();

    //qDebug() << Q_FUNC_INFO << new_output;
    emit newStandardErrorMessage(new_output);
    d->buffer_std_error.append(new_output);

    if (d->buffer_std_error.isEmpty()) {
        d->buffer_mutex_std_err.unlock();
        return;
    }

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \n and split messages up:
        split_list = d->buffer_std_error.split("\n");
        while (split_list.count() > 1) {
            processSingleBufferMessage(split_list.front().simplified());
            split_list.pop_front();
        }
    } else { 
        // We loop through the string and replace all known break strings with &{_BREAKSTRING and then split
        // it using &{_:
        d->line_break_strings.append("\n");
        foreach (const QString& break_string, d->line_break_strings)
            d->buffer_std_error.replace(break_string,"&{_" + break_string);

        split_list = d->buffer_std_error.split("&{_");
        while (split_list.count() > 1) {
            QString msg = split_list.front().simplified();
            if (msg.startsWith("&{_"))
                msg = msg.remove(0,3);

            processSingleBufferMessage(msg);
            split_list.pop_front();
        }
    }

    if (split_list.isEmpty())
        d->buffer_std_error.clear();
    else
        d->buffer_std_error = split_list.front();

    d->buffer_mutex_std_err.unlock();
}

void Qtilities::Core::QtilitiesProcess::processSingleBufferMessage(const QString &buffer_message) {
    bool found_match = false;

    // Get all hints that match the message:
    QListIterator<ProcessBufferMessageTypeHint> itr(d->buffer_message_type_hints);
    int highest_matching_hint_priority = -1;
    QList<ProcessBufferMessageTypeHint> matching_hints;
    while (itr.hasNext()) {
        ProcessBufferMessageTypeHint hint = itr.next();
        if (hint.d_regexp.exactMatch(buffer_message)) {
            if (hint.d_priority > highest_matching_hint_priority) {
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
        logMessage(buffer_message);
}

void Qtilities::Core::QtilitiesProcess::completeTaskExt() {
    // We need to make sure the process buffer is clean:
    d->buffer_std_out.append("\n");
    logProgressOutput();
    d->buffer_std_error.append("\n");
    logProgressError();

    // Now we can complete the task.
    completeTask();
}
