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

#include <Logger>

using namespace Qtilities::Logging;

struct Qtilities::Core::QtilitiesProcessPrivateData {
    QtilitiesProcessPrivateData() : process(0) { }

    QProcess* process;
    QString buffer_std_out;
    QString buffer_std_error;
    QStringList line_break_strings;
};

Qtilities::Core::QtilitiesProcess::QtilitiesProcess(const QString& task_name, bool enable_logging, bool read_process_buffers, QObject* parent) : Task(task_name,enable_logging,parent) {
    d = new QtilitiesProcessPrivateData;
    d->process = new QProcess;

    connect(d->process, SIGNAL(started()), this, SLOT(procStarted()));
    connect(this, SIGNAL(stopTaskRequest()), this, SLOT(stopProcess()));

    connect(d->process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procError(QProcess::ProcessError)));
    connect(d->process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procFinished(int, QProcess::ExitStatus)));
    connect(d->process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(procStateChanged(QProcess::ProcessState)));

    if (read_process_buffers) {
        connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(logProgressOutput()));
        connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(logProgressError()));
    }

    setCanStop(true);
}

Qtilities::Core::QtilitiesProcess::~QtilitiesProcess() {
    if (d->process) {
        if (state() == ITask::TaskBusy)
            completeTask();
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

bool Qtilities::Core::QtilitiesProcess::startProcess(const QString& program,
                                                     const QStringList& arguments,
                                                     QProcess::OpenMode mode,
                                                     int wait_for_started_msecs) {
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
        logMessage("> working directory of process: " + QDir::current().path());
    else
        logMessage("> working directory of process: " + d->process->workingDirectory());
    QDir dir(d->process->workingDirectory());
    if (!dir.exists())
        logWarning("> working directory does not exist, process might fail to start.");

    logMessage("");
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
        completeTask();
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
        completeTask();
    }
}

void Qtilities::Core::QtilitiesProcess::logProgressOutput() {
    //QCoreApplication::processEvents();

    QString new_output = d->process->readAllStandardOutput();
    emit newStandardOutputMessage(new_output);
    d->buffer_std_out.append(new_output);

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \n and split messages up:
        split_list = d->buffer_std_out.split("\n",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            if (split_list.front().trimmed().startsWith("WARNING:",Qt::CaseSensitive))
                logWarning(split_list.front());
            else if (split_list.front().trimmed().startsWith("ERROR:",Qt::CaseSensitive))
                logError(split_list.front());
            else
                logMessage(split_list.front());
            split_list.pop_front();
        }
    } else {
        // We loop through the string and replace all known break strings with &{_BREAKSTRING and then split
        // it using &{_:
        d->line_break_strings.append("\n");
        foreach (const QString& break_string, d->line_break_strings)
            d->buffer_std_out.replace(break_string,"&{_" + break_string);

        split_list = d->buffer_std_out.split("&{_",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            QString msg = split_list.front().simplified();
            if (msg.startsWith("&{_"))
                msg = msg.remove(0,3);

            if (msg.startsWith("WARNING:",Qt::CaseSensitive))
                logWarning(msg);
            else if (msg.startsWith("ERROR:",Qt::CaseSensitive))
                logError(msg);
            else
                logMessage(msg);
            split_list.pop_front();
        }
    }

    if (split_list.isEmpty())
        d->buffer_std_out.clear();
    else
        d->buffer_std_out = split_list.front();
}

void Qtilities::Core::QtilitiesProcess::logProgressError() {   
    //QCoreApplication::processEvents();

    QString new_output = d->process->readAllStandardError();
    emit newStandardErrorMessage(new_output);
    d->buffer_std_error.append(new_output);

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \n and split messages up:
        split_list = d->buffer_std_error.split("\n",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            if (split_list.front().trimmed().startsWith("WARNING:",Qt::CaseSensitive))
                logWarning(split_list.front());
            else if (split_list.front().trimmed().startsWith("INFO:",Qt::CaseSensitive))
                logMessage(split_list.front());
            else
                logError(split_list.front());
            split_list.pop_front();
        }   
    } else { 
        // We loop through the string and replace all known break strings with &{_BREAKSTRING and then split
        // it using &{_:
        d->line_break_strings.append("\n");
        foreach (const QString& break_string, d->line_break_strings)
            d->buffer_std_error.replace(break_string,"&{_" + break_string);

        split_list = d->buffer_std_error.split("&{_",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            QString msg = split_list.front().simplified();
            if (msg.startsWith("&{_"))
                msg = msg.remove(0,3);

            if (msg.startsWith("WARNING:",Qt::CaseSensitive))
                logWarning(msg);
            else if (msg.startsWith("INFO:",Qt::CaseSensitive))
                logMessage(msg);
            else
                logError(msg);
            split_list.pop_front();
        }
    }

    if (split_list.isEmpty())
        d->buffer_std_error.clear();
    else
        d->buffer_std_error = split_list.front();
}
