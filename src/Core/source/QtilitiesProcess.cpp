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
};

Qtilities::Core::QtilitiesProcess::QtilitiesProcess(const QString& task_name, bool enable_logging, QObject* parent) : Task(task_name,enable_logging,parent) {
    d = new QtilitiesProcessPrivateData;
    d->process = new QProcess;

    connect(d->process, SIGNAL(started()), this, SLOT(procStarted()));
    connect(this, SIGNAL(taskStopped()), this, SLOT(stopProcess()));
    connect(d->process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procError(QProcess::ProcessError)));
    connect(d->process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procFinished(int, QProcess::ExitStatus)));
    connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(logProgressOutput()));
    connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(logProgressError()));

    setCanStop(true);
}

Qtilities::Core::QtilitiesProcess::~QtilitiesProcess() {
    if (d->process)
        delete d->process;
    delete d;
}

QProcess* Qtilities::Core::QtilitiesProcess::process() {
    return d->process;
}

bool Qtilities::Core::QtilitiesProcess::startProcess(const QString& program, const QStringList& arguments, QProcess::OpenMode mode) {
    if (state() == ITask::TaskBusy || state() == ITask::TaskPaused)
        return false;

    startTask();
    logMessage("Executing Process: " + program + " " + arguments.join(" "));
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
    Task::stop();
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
    QString tmp_str = d->process->readAllStandardOutput();
    if (!tmp_str.isEmpty())
        logMessage(tmp_str);
    QCoreApplication::processEvents();
}

void Qtilities::Core::QtilitiesProcess::logProgressError() {
    QString tmp_str = d->process->readAllStandardError();
    if (!tmp_str.isEmpty())
        logMessage(tmp_str,Logger::Error);
    QCoreApplication::processEvents();
}

