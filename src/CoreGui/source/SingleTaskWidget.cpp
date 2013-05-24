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

#include "SingleTaskWidget.h"
#include "ui_SingleTaskWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "WidgetLoggerEngine.h"
#include "TaskManagerGui.h"

#include <TaskManager>
#include <QtilitiesCoreApplication>

#include <QPointer>
#include <QMessageBox>

using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Interfaces;

struct Qtilities::CoreGui::SingleTaskWidgetPrivateData {
    SingleTaskWidgetPrivateData() : task(0),
        pause_button_visible(false),
        stop_button_visible(false),
        start_button_visible(false),
        show_log_button_visible(true) {}

    ITask*              task;
    QPointer<QObject>   task_base;
    bool                pause_button_visible;
    bool                stop_button_visible;
    bool                start_button_visible;
    bool                show_log_button_visible;
};

Qtilities::CoreGui::SingleTaskWidget::SingleTaskWidget(int task_id, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SingleTaskWidget),
    d(0)
{
    ui->setupUi(this);
    d = new SingleTaskWidgetPrivateData;
    ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_NOT_STARTED_22x22));
    ui->btnShowLog->setToolTip(tr("Task Has Not Been Started."));

    ui->progressBar->setValue(0);
    d->task = TASK_MANAGER->hasTask(task_id);
    if (!d->task) {
        qDebug() << Q_FUNC_INFO << "Showing SingleTaskWidget for a task which does not exist with ID:" << task_id;
        return;
    }

    // Pause Button
    setPauseButtonVisible(d->task->canPause());

    // Stop Button
    d->stop_button_visible = d->task->canStop();
    ui->btnStop->setVisible(d->stop_button_visible);
    ui->btnStop->setIcon(QIcon(qti_icon_TASK_STOP_22x22));
    ui->btnStop->setToolTip("Cancel Task");

    // Start Button
    d->start_button_visible = d->task->canStart();
    ui->btnStart->setVisible(d->start_button_visible);
    ui->btnStart->setIcon(QIcon(qti_icon_TASK_START_22x22));
    ui->btnStart->setToolTip(tr("Task Has Not Been Started. Click to start it."));

    // Hide Stop and Pause buttons widget if those buttons are not shown.
    if (!d->task->canStop() && !d->task->canPause()) {
        ui->widgetRightButtonsHolder->setVisible(false);
    } else {
        ui->widgetRightButtonsHolder->setVisible(true);
    }

    // Log Button
    ui->btnShowLog->setVisible(d->task->loggingEnabled());

    if (d->task) {
        d->task_base = d->task->objectBase();
        connect(d->task->objectBase(),SIGNAL(destroyed()),SLOT(handleTaskDeleted()));
        connect(d->task->objectBase(),SIGNAL(taskStarted(int,QString,Logger::MessageType)),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(subTaskCompleted(int,QString,Logger::MessageType)),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskCompleted(ITask::TaskResult,QString,Logger::MessageType)),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskPaused()),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskResumed()),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskStopped()),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(busyStateChanged(ITask::TaskBusyState,ITask::TaskBusyState)),SLOT(updateBusyState(ITask::TaskBusyState)));
        connect(d->task->objectBase(),SIGNAL(displayedNameChanged(QString)),SLOT(setDisplayedName(QString)));
        connect(d->task->objectBase(),SIGNAL(canStartChanged(bool)),SLOT(setStartButtonEnabled(bool)));
        connect(d->task->objectBase(),SIGNAL(canStopChanged(bool)),SLOT(setStopButtonEnabled(bool)));
        connect(d->task->objectBase(),SIGNAL(canPauseChanged(bool)),SLOT(setPauseButtonEnabled(bool)));
        update();
    }
}

Qtilities::CoreGui::SingleTaskWidget::~SingleTaskWidget() {
    delete ui;
    delete d;
}

ITask* Qtilities::CoreGui::SingleTaskWidget::task() const {
    return d->task;
}

void Qtilities::CoreGui::SingleTaskWidget::setPauseButtonVisible(bool is_visible) {
    d->pause_button_visible = is_visible;
    update();
}

void Qtilities::CoreGui::SingleTaskWidget::setPauseButtonEnabled(bool enabled) {
    ui->btnPause->setEnabled(enabled);
}

bool Qtilities::CoreGui::SingleTaskWidget::pauseButtonVisible() const {
    return d->pause_button_visible;
}

void Qtilities::CoreGui::SingleTaskWidget::setStopButtonVisible(bool is_visible) {
    d->stop_button_visible = is_visible;
    update();
}

void Qtilities::CoreGui::SingleTaskWidget::setStopButtonEnabled(bool enabled) {
    ui->btnStop->setEnabled(enabled);
    if (enabled)
        ui->btnStop->setToolTip(tr("Cancel Task"));
    else
        ui->btnStop->setToolTip(tr("This Task Can't Be Stopped Manually Right Now"));
}

bool Qtilities::CoreGui::SingleTaskWidget::stopButtonVisible() const {
    return d->stop_button_visible;
}

void Qtilities::CoreGui::SingleTaskWidget::setStartButtonVisible(bool is_visible) {
    d->start_button_visible = is_visible;
    if (is_visible) {
        ui->widgetLeftButtonsHolder->setVisible(is_visible);
        ui->btnStart->setVisible(is_visible);
    }
}

void Qtilities::CoreGui::SingleTaskWidget::setStartButtonEnabled(bool enabled) {
    ui->btnStart->setEnabled(enabled);
}

bool Qtilities::CoreGui::SingleTaskWidget::startButtonVisible() const {
    return d->start_button_visible;
}

void Qtilities::CoreGui::SingleTaskWidget::setShowLogButtonVisible(bool is_visible) {
    d->show_log_button_visible = is_visible;
    ui->btnShowLog->setVisible(is_visible);
}

bool Qtilities::CoreGui::SingleTaskWidget::showLogButtonVisible() const {
    return d->show_log_button_visible;
}

QProgressBar* Qtilities::CoreGui::SingleTaskWidget::progressBar() {
    return ui->progressBar;
}

QToolButton *Qtilities::CoreGui::SingleTaskWidget::stopButton() const {
    return ui->btnStop;
}

QToolButton *Qtilities::CoreGui::SingleTaskWidget::pauseButton() const {
    return ui->btnPause;
}

QToolButton *Qtilities::CoreGui::SingleTaskWidget::startButton() const {
    return ui->btnStart;
}

QToolButton *Qtilities::CoreGui::SingleTaskWidget::showLogButton() const {
    return ui->btnShowLog;
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnShowLog_clicked() {
    if (!d->task || !d->task_base)
        return;

    // Check if the task has a log widget:
    if (d->task->loggerEngine()) {
        WidgetLoggerEngine* engine_widget = qobject_cast<WidgetLoggerEngine*> (d->task->loggerEngine());
        if (engine_widget) {
            QWidget* widget = engine_widget->getWidget();
            widget->setAttribute(Qt::WA_QuitOnClose,false);
            widget->resize(QSize(1000,600));
            widget->show();
            widget->activateWindow();
            return;
        }
    }

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Task Log: ") + d->task->taskName());
    msgBox.setText(tr("The activities of this task are not logged."));
    msgBox.exec();
}

void Qtilities::CoreGui::SingleTaskWidget::setDisplayedName(const QString& name) {
    ui->lblTaskName->clear();
    QFontMetrics fm(ui->lblTaskName->font());
    QString elided_text = fm.elidedText(name,Qt::ElideRight,ui->lblTaskName->width());
    ui->lblTaskName->setText(elided_text);
    ui->lblTaskName->setToolTip(name);
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnPause_clicked() {
    if (!d->task || !d->task_base)
        return;

    if (d->task->state() == ITask::TaskBusy)
        d->task->pause();
    else if (d->task->state() == ITask::TaskPaused)
        d->task->resume();
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnStop_clicked() {
    if (!d->task || !d->task_base)
        return;

    if (d->task->state() == ITask::TaskBusy) {
        if (d->task->taskStopConfirmation() == ITask::TaskStopConfirmationMsgBox) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setWindowTitle(QString(tr("Stop %1?")).arg(d->task->taskName()));
            msgBox.setText(QString(tr("Are you sure you want to stop %1?")).arg(d->task->taskName()));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();

            switch (ret) {
              case QMessageBox::Yes:
                  break;
              case QMessageBox::No:
                  return;
                  break;
              default:
                  break;
            }
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        d->task->stop();
        QApplication::restoreOverrideCursor();
        if (d->task_base) {
            emit hiddenByStopButton();
            if (d->task->taskStopAction() == ITask::TaskDeleteWhenStopped) {
                setVisible(false);
                emit hiddenByStopButton();
                d->task->objectBase()->deleteLater();
            } else if (d->task->taskStopAction() == ITask::TaskHideWhenStopped) {
                setVisible(false);
                emit hiddenByStopButton();
            }
            // ITask::TaskDoNothingWhenStopped does not need to do anything.
        }
    } else {
        if (d->task->taskRemoveAction() == ITask::TaskDeleteWhenRemoved) {
            setVisible(false);
            emit hiddenByStopButton();
            d->task->objectBase()->deleteLater();
        } else if (d->task->taskRemoveAction() == ITask::TaskHideWhenRemoved) {
            setVisible(false);
            emit hiddenByStopButton();
        }
    }
}

void Qtilities::CoreGui::SingleTaskWidget::handleTaskDeleted() {
    //qDebug() << "Task deleted, deleting its SingleTaskWidget";
    deleteLater();
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnStart_clicked() {
    if (!d->task || !d->task_base)
        return;

    d->task->start();
}

void Qtilities::CoreGui::SingleTaskWidget::updateBusyState(ITask::TaskBusyState busy_state) {
    if (!d->task || !d->task_base)
        return;

    if (d->task->state() == ITask::TaskBusy) {
        if (busy_state == ITask::TaskBusyClean) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_22x22));
            ui->btnShowLog->setToolTip(tr("Task Busy. Click to view the task log"));
        } else if (busy_state == ITask::TaskBusyWithWarnings) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_WITH_WARNINGS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Busy (With Warnings). Click to view the task log"));
        } else if (busy_state == ITask::TaskBusyWithErrors) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_WITH_ERRORS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Busy (With Errors). Click to view the task log"));
        }
    }
}

void Qtilities::CoreGui::SingleTaskWidget::resizeEvent(QResizeEvent * event) {
    if (d) {
        if (d->task && d->task_base)
            setDisplayedName(d->task->displayName());
    }

    QWidget::resizeEvent(event);
}

void Qtilities::CoreGui::SingleTaskWidget::update() {  
    if (!d->task || !d->task_base)
        return;

    bool show_progress = TaskManagerGui::instance()->taskProgressUpdatingEnabled();

    setDisplayedName(d->task->displayName());

    if (d->task->state() == ITask::TaskBusy) {
        if (show_progress) {
            ui->progressBar->setEnabled(true);
            if (d->task->numberOfSubTasks() == -1) {
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(0);
                ui->progressBar->setValue(0);
            } else {
                ui->progressBar->setMinimum(0);
                //qDebug() << "Current progress on task " << d->task->taskName() << " is " << d->task->currentProgress() << "/" << d->task->numberOfSubTasks();
                ui->progressBar->setValue(d->task->currentProgress());
                ui->progressBar->setMaximum(d->task->numberOfSubTasks());
            }
        } else {
            ui->progressBar->setEnabled(false);
        }

        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_22x22));
        ui->btnShowLog->setToolTip(tr("Task Busy. Click to view the task log"));
        ui->btnShowLog->setVisible(d->show_log_button_visible);

        bool show_stop_button = d->stop_button_visible;
        bool show_pause_button = d->pause_button_visible;
        ui->btnStop->setVisible(show_stop_button);
        ui->btnStop->setEnabled(d->task->canStop());
        ui->btnPause->setVisible(show_pause_button);
        ui->btnPause->setEnabled(d->task->canPause());
        ui->btnPause->setIcon(QIcon(qti_icon_TASK_PAUSE_22x22));
        ui->widgetRightButtonsHolder->setVisible(show_stop_button || show_pause_button);

        ui->btnStart->setVisible(false);
    } else if (d->task->state() == ITask::TaskNotStarted) {
        if (show_progress) {
            ui->progressBar->setEnabled(true);
            ui->progressBar->setMinimum(0);
            ui->progressBar->setMaximum(100);
        } else {
            ui->progressBar->setEnabled(false);
        }

        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_NOT_STARTED_22x22));
        ui->btnShowLog->setToolTip(tr("Task Has Not Been Started. Click to view the task log"));

        ui->btnStart->setToolTip(tr("Task Not Started. Click to start it again"));
        ui->btnStart->setVisible(d->task->canStart());
        ui->btnShowLog->setVisible(!d->task->canStart() && d->show_log_button_visible);

        ui->widgetRightButtonsHolder->setVisible(false);
    } else if (d->task->state() == ITask::TaskPaused) {
        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_22x22));
        ui->btnShowLog->setToolTip(tr("Task Has Been Paused. Click to view the task log"));

        bool show_stop_button = d->stop_button_visible;
        bool show_pause_button = d->pause_button_visible;
        ui->btnStop->setVisible(show_stop_button);
        ui->btnStop->setEnabled(d->task->canStop());
        ui->btnPause->setVisible(show_pause_button);
        ui->btnPause->setIcon(QIcon(qti_icon_TASK_RESUME_22x22));
        ui->btnPause->setToolTip(tr("Resume Task"));
        ui->btnPause->setEnabled(d->task->canPause());
        ui->widgetRightButtonsHolder->setVisible(show_stop_button || show_pause_button);

        ui->btnStart->setVisible(false);
    } else if (d->task->state() == ITask::TaskStopped) {
        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_NOT_STARTED_22x22));
        ui->btnShowLog->setToolTip(tr("Task Has Been Stopped. Click to view the task log"));
        ui->btnStart->setVisible(d->task->canStart());

        bool show_stop_button = d->stop_button_visible;
        ui->btnStop->setVisible(show_stop_button);
        ui->btnStop->setToolTip(tr("Remove Task"));
        ui->btnPause->setVisible(false);
        ui->widgetRightButtonsHolder->setVisible(show_stop_button);
    } else if (d->task->state() == ITask::TaskCompleted) {
        if (show_progress) {
            ui->progressBar->setEnabled(true);
            if (d->task->numberOfSubTasks() == -1) {
                // Handle progress bars acting as busy indicators:
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(100);
                ui->progressBar->setValue(100);
            } else {
                // Handle progress bars showing progress:
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(100);
                ui->progressBar->setValue(100);
            }
        } else {
            ui->progressBar->setEnabled(false);
        }

        ui->btnStart->setVisible(d->task->canStart());

        bool show_stop_button = d->stop_button_visible;
        ui->btnStop->setVisible(show_stop_button);
        ui->btnStop->setToolTip(tr("Remove Task"));
        ui->btnPause->setVisible(false);
        ui->widgetRightButtonsHolder->setVisible(show_stop_button);

        if (d->task->result() == ITask::TaskFailed) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_FAILED_22x22));
            ui->btnShowLog->setToolTip(tr("Task Failed. Click to view the task log"));
        } else if (d->task->result() == ITask::TaskSuccessful) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_DONE_22x22));
            ui->btnShowLog->setToolTip(tr("Task Finished Successfully. Click to view the task log"));
        } else if (d->task->result() == ITask::TaskSuccessfulWithWarnings) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_DONE_WITH_WARNINGS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Finished Successfully (With Warnings). Click to view the task log"));
        } else if (d->task->result() == ITask::TaskSuccessfulWithErrors) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_DONE_WITH_ERRORS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Finished Successfully (With errors and possibly warnings). Click to view the task log"));
        }
    }
}
