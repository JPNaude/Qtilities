/****************************************************************************
**
** Copyright 2010-2011, CSIR
** Author: JP Naude, jpnaude@csir.co.za
**
****************************************************************************/

#include "QtilitiesProcess.h"

#include <QCoreApplication>

#include <Logger>

using namespace Qtilities::Logging;

struct Qtilities::Core::QtilitiesProcessPrivateData {
    QtilitiesProcessPrivateData() : process(0) { }

    QProcess* process;
    QString buffer_std_out;
    QString buffer_std_error;
    QStringList line_break_strings;
};

Qtilities::Core::QtilitiesProcess::QtilitiesProcess(const QString& task_name, bool enable_logging, QObject* parent) : Task(task_name,enable_logging,parent) {
    d = new QtilitiesProcessPrivateData;
    d->process = new QProcess;

    connect(d->process, SIGNAL(started()), this, SLOT(procStarted()));
    connect(this, SIGNAL(stopTaskRequest()), this, SLOT(stopProcess()));

    connect(d->process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procError(QProcess::ProcessError)));
    connect(d->process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procFinished(int, QProcess::ExitStatus)));
    connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(logProgressOutput()));
    connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(logProgressError()));

    setCanStop(true);
}

Qtilities::Core::QtilitiesProcess::~QtilitiesProcess() {
    if (d->process) {
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

bool Qtilities::Core::QtilitiesProcess::startProcess(const QString& program, const QStringList& arguments, QProcess::OpenMode mode) {
    if (state() == ITask::TaskBusy || state() == ITask::TaskPaused)
        return false;

    startTask();
    logMessage("Executing Process: " + program + " " + arguments.join(" "));
    logMessage("-> working directory of process: " + d->process->workingDirectory());
    d->process->start(program, arguments, mode);

    if (!d->process->waitForStarted()) {
        logMessage("Failed to open " + program + ". Make sure the executable is visible in your system's paths.", Logger::Error);
        completeTask(ITask::TaskFailed);
        return false;
    }

    return true;
}

void Qtilities::Core::QtilitiesProcess::stopProcess() {
    d->process->kill();
}

void Qtilities::Core::QtilitiesProcess::procStarted() {

}

void Qtilities::Core::QtilitiesProcess::procFinished(int exit_code, QProcess::ExitStatus exit_status) {
    if (exit_code == 0) {
        logMessage("Process " + taskName() + " finished successfully.");
        completeTask(ITask::TaskSuccessful);
    } else {
        logMessage("Process " + taskName() + " finished with code " + QString::number(exit_code),Logger::Error);
        completeTask(ITask::TaskFailed);
    }

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

void Qtilities::Core::QtilitiesProcess::logProgressOutput() {
    d->buffer_std_out.append(d->process->readAllStandardOutput());

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \r and split messages up:
        split_list = d->buffer_std_out.split("\r",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            if (split_list.front().trimmed().startsWith("WARNING",Qt::CaseInsensitive))
                logWarning(split_list.front());
            else if (split_list.front().trimmed().startsWith("ERROR",Qt::CaseInsensitive))
                logError(split_list.front());
            else
                logMessage(split_list.front());
            split_list.pop_front();
        }
    } else {
        // We search for any break strings and split messages up:
        bool matched_break_string;
        foreach (QString break_string, d->line_break_strings) {
            matched_break_string = false;
            split_list = d->buffer_std_out.split(break_string,QString::SkipEmptyParts);
            while (split_list.count() > 1) {
                matched_break_string = true;
                QString msg = split_list.front().trimmed();
                msg.prepend(break_string);
                if (break_string.compare("WARNING",Qt::CaseInsensitive) == 0)
                    logWarning(msg);
                else if (break_string.compare("ERROR",Qt::CaseInsensitive) == 0)
                    logError(msg);
                else
                    logMessage(msg);
                split_list.pop_front();
            }
            if (matched_break_string)
                break;
        }
    }

    d->buffer_std_out = split_list.front();
}

void Qtilities::Core::QtilitiesProcess::logProgressError() {
    d->buffer_std_error.append(d->process->readAllStandardError());

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \r and split messages up:
        split_list = d->buffer_std_error.split("\r",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            if (split_list.front().trimmed().startsWith("WARNING",Qt::CaseInsensitive))
                logWarning(split_list.front());
            else if (split_list.front().trimmed().startsWith("INFO",Qt::CaseInsensitive))
                logMessage(split_list.front());
            else
                logError(split_list.front());
            split_list.pop_front();
        }
    } else {
        // We search for any break strings and split messages up:
        bool matched_break_string;
        foreach (QString break_string, d->line_break_strings) {
            matched_break_string = false;
            split_list = d->buffer_std_error.split(break_string,QString::SkipEmptyParts);
            while (split_list.count() > 1) {
                matched_break_string = true;
                QString msg = split_list.front().trimmed();
                msg.prepend(break_string);
                if (break_string.compare("WARNING",Qt::CaseInsensitive) == 0)
                    logWarning(msg);
                else if (break_string.compare("INFO",Qt::CaseInsensitive) == 0)
                    logMessage(msg);
                else
                    logError(msg);
                split_list.pop_front();
            }
            if (matched_break_string)
                break;
            // This is not perfect, can end up with problems where you have something like this: INFO WARNING INFO. In that
            // case the first message will be INFO WARNING and the second just INFO, thus the WARNING is missed in the first message.
        }
    }

    d->buffer_std_error = split_list.front();
}

