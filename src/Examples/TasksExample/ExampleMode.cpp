/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#include "ExampleMode.h"
#include "ui_ExampleMode.h"

#include <QtilitiesProjectManagement>
#include <QtGui>

using namespace QtilitiesProjectManagement;

struct Qtilities::Examples::TasksExample::ExampleModePrivateData {
    ExampleModePrivateData() {}

    QMap<int,FileUtils*> workers;
    QList<QListWidget*> list_widgets;
};

Qtilities::Examples::TasksExample::ExampleMode::ExampleMode(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExampleMode)
{
    ui->setupUi(this);
    d = new ExampleModePrivateData;

    if (ui->widgetResultsHolder->layout())
        delete ui->widgetResultsHolder->layout();

    new QHBoxLayout(ui->widgetResultsHolder);
}

Qtilities::Examples::TasksExample::ExampleMode::~ExampleMode()
{
    delete ui;
    delete d;
}

QWidget* Qtilities::Examples::TasksExample::ExampleMode::modeWidget() {
    return this;
}

void Qtilities::Examples::TasksExample::ExampleMode::initializeMode() {

}

QIcon Qtilities::Examples::TasksExample::ExampleMode::modeIcon() const {
    return QIcon(qti_icon_QTILITIES_SYMBOL_64x64);
}

QString Qtilities::Examples::TasksExample::ExampleMode::modeName() const {
    return tr("Task Creation Mode");
}

void Qtilities::Examples::TasksExample::ExampleMode::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::Examples::TasksExample::ExampleMode::on_btnNewTask_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),QtilitiesApplication::applicationSessionPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        FileUtils* utils = new FileUtils;
        utils->setFindFilesUnderDirParams(dir);

        ITask* task = utils->task(utils->findTaskID(utils->taskNameToString(FileUtils::TaskFindFilesUnderDir)));
        connect(task->objectBase(),SIGNAL(taskCompleted(ITask::TaskResult,QString)),SLOT(handleTaskCompleted()));
        d->workers[task->taskID()] = utils;
    }
}

void Qtilities::Examples::TasksExample::ExampleMode::handleTaskCompleted() {
    QObject* sender_task = sender();
    if (sender_task) {
        ITask* task = qobject_cast<ITask*> (sender());
        if (task) {
            FileUtils* utils = d->workers[task->taskID()];
            if (utils) {
                QListWidget* list_widget = new QListWidget;
                list_widget->addItems(convertQFileInfoList(utils->lastFilesUnderDir()));
                d->list_widgets << list_widget;
                ui->widgetResultsHolder->layout()->addWidget(list_widget);
            }
        }
    }
}

QStringList Qtilities::Examples::TasksExample::ExampleMode::convertQFileInfoList(QFileInfoList list) {
    QStringList file_names;
    foreach (QFileInfo fi, list)
        file_names << fi.filePath();
    return file_names;
}

void Qtilities::Examples::TasksExample::ExampleMode::on_btnShortTask_clicked() {
    Task* task = new Task("Short Timer Task");
    task->setCanStop(true);
    task->setTaskLifeTimeFlags(Task::LifeTimeDestroyWhenSuccessful | Task::LifeTimeDestroyWhenStopped);
    OBJECT_MANAGER->registerObject(task);

    QTimer* timer = new QTimer(task);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),task,SLOT(completeTask()));
    connect(task,SIGNAL(stopTaskRequest()),timer,SLOT(stop()));
    connect(task,SIGNAL(stopTaskRequest()),task,SLOT(stopTask()));
    timer->setInterval(3000);

    timer->start();
    task->startTask();
}

void Qtilities::Examples::TasksExample::ExampleMode::on_btnLongTask_clicked() {
    Task* task = new Task("Long Timer Task");
    task->setCanStop(true);
    task->setTaskLifeTimeFlags(Task::LifeTimeDestroyWhenSuccessful | Task::LifeTimeDestroyWhenStopped);
    OBJECT_MANAGER->registerObject(task);

    QTimer* timer = new QTimer(task);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),task,SLOT(completeTask()));
    connect(task,SIGNAL(stopTaskRequest()),timer,SLOT(stop()));
    connect(task,SIGNAL(stopTaskRequest()),task,SLOT(stopTask()));
    timer->setInterval(10000);

    timer->start();
    task->startTask();
}

void Qtilities::Examples::TasksExample::ExampleMode::on_btnDoxygen_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Doxyfile"),QApplication::applicationDirPath(),tr("All Files (*.*)"));
    if (!fileName.isEmpty()) {
        QtilitiesProcess* process = new QtilitiesProcess("Doxygen Generation",true,this);
        QFileInfo fi(fileName);
        process->process()->setWorkingDirectory(fi.dir().path());
        process->setTaskStopAction(ITask::TaskDoNothingWhenStopped);
        process->setTaskRemoveAction(ITask::TaskDeleteWhenRemoved);
        OBJECT_MANAGER->registerObject(process);
        process->startProcess("Doxygen",QStringList(fileName));
    }
}
