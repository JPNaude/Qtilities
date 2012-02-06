/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#include "QtilitiesMainWindow.h"
#include "ui_QtilitiesMainWindow.h"
#include "QtilitiesCoreGuiConstants.h"
#include "TaskSummaryWidget.h"

#include <QtilitiesCoreApplication>

#include <QBoxLayout>
#include <QSettings>
#include <QLabel>
#include <QMessageBox>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::QtilitiesMainWindowPrivateData {
    QtilitiesMainWindowPrivateData() : initialized(false),
        current_widget(0),
        mode_manager(0),
        central_widget(0),
        priority_messages_enabled(true),
        task_summary_widget_visible(true),
        task_summary_widget(0) {}

    bool                            initialized;
    QWidget*                        current_widget;
    QWidget*                        current_widget_holder;
    ModeManager*                    mode_manager;
    QWidget*                        central_widget;
    bool                            priority_messages_enabled;
    QWidget                         priority_messages_widget;
    QLabel                          priority_messages_icon;
    QLabel                          priority_messages_text;
    QtilitiesMainWindow::ModeLayout mode_layout;
    QTimer                          priority_message_timer;
    bool                            task_summary_widget_visible;
    QPointer<TaskSummaryWidget>     task_summary_widget;
};

Qtilities::CoreGui::QtilitiesMainWindow::QtilitiesMainWindow(ModeLayout modeLayout, QWidget *parent, Qt::WindowFlags flags) :
        QMainWindow(parent, flags), ui(new Ui::QtilitiesMainWindow)
{
    ui->setupUi(this);
    d = new QtilitiesMainWindowPrivateData;
    d->mode_layout = modeLayout;

    if (modeLayout != ModesNone) {
        d->current_widget_holder = new QWidget;
        d->central_widget = new QWidget;
        d->current_widget = new QWidget;
        changeCurrentWidget(d->current_widget);
    }

    readSettings();

    // Set the window title to the application name if not empty:
    if (!QApplication::applicationName().isEmpty())
        setWindowTitle(QApplication::applicationName());

    connect(Log,SIGNAL(newPriorityMessage(Logger::MessageType,QString)),SLOT(processPriorityMessage(Logger::MessageType,QString)));

    // Construct a proper priority_message_widget:
    if (d->priority_messages_widget.layout())
        delete d->priority_messages_widget.layout();

    QHBoxLayout* priority_messages_layout = new QHBoxLayout(&d->priority_messages_widget);
    priority_messages_layout->addWidget(&d->priority_messages_icon);
    priority_messages_layout->addWidget(&d->priority_messages_text);
    priority_messages_layout->setContentsMargins(0,0,6,0);
    statusBar()->addWidget(&d->priority_messages_widget);

    d->priority_messages_icon.setVisible(false);
    d->priority_messages_text.setWordWrap(false);

    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_text, SLOT(clear()));
    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_text, SLOT(hide()));
    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_icon, SLOT(clear()));
    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_icon, SLOT(hide()));

    doLayout();

    installEventFilter(this);
}

Qtilities::CoreGui::QtilitiesMainWindow::~QtilitiesMainWindow() {
    if (d->current_widget)
        d->current_widget->setParent(0);

    emit aboutToBeDestroyed(this);

    delete ui;
    delete d;
}

void Qtilities::CoreGui::QtilitiesMainWindow::writeSettings() {
    if (!QtilitiesCoreApplication::qtilitiesSettingsPathEnabled())
        return;

    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("state", saveState());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::QtilitiesMainWindow::readSettings() {
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1000, 1000)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreState(settings.value("state").toByteArray());
    bool is_maximized = settings.value("maximized",false).toBool();
    if (is_maximized)
        showMaximized();
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::QtilitiesMainWindow::enablePriorityMessages() {
    d->priority_messages_enabled = true;
}

void Qtilities::CoreGui::QtilitiesMainWindow::disablePriorityMessages() {
    d->priority_messages_enabled = false;
}

QLabel *Qtilities::CoreGui::QtilitiesMainWindow::priorityMessageLabel() const {
    return &d->priority_messages_text;
}

Qtilities::CoreGui::ModeManager* Qtilities::CoreGui::QtilitiesMainWindow::modeManager() {
    return d->mode_manager;
}

Qtilities::CoreGui::QtilitiesMainWindow::ModeLayout Qtilities::CoreGui::QtilitiesMainWindow::modeLayout() const {
    return d->mode_layout;
}

bool Qtilities::CoreGui::QtilitiesMainWindow::taskSummaryWidgetVisible() const {
    return d->task_summary_widget_visible;
}

Qtilities::CoreGui::TaskSummaryWidget *Qtilities::CoreGui::QtilitiesMainWindow::taskSummaryWidget() const {
    return d->task_summary_widget;
}

void Qtilities::CoreGui::QtilitiesMainWindow::showTaskSummaryWidget() {
    d->task_summary_widget_visible = true;
     if (d->task_summary_widget)
        d->task_summary_widget->show();
    d->task_summary_widget->setTaskSummaryEnabled(d->task_summary_widget_visible);
}

void Qtilities::CoreGui::QtilitiesMainWindow::hideTaskSummaryWidget() {
    d->task_summary_widget_visible = false;
    if (d->task_summary_widget)
        d->task_summary_widget->hide();
    d->task_summary_widget->setTaskSummaryEnabled(d->task_summary_widget_visible);
}

void Qtilities::CoreGui::QtilitiesMainWindow::doLayout() {
    if (d->mode_layout == ModesTop || d->mode_layout == ModesBottom) {
        if (!d->mode_manager) {
            d->mode_manager = new ModeManager(qti_def_DEFAULT_MODE_MANAGER,Qt::Vertical,this);
            connect(d->mode_manager,SIGNAL(changeCentralWidget(QWidget*)),SLOT(changeCurrentWidget(QWidget*)));
            connect(d->mode_manager,SIGNAL(modeListItemSizesChanged()),SLOT(updateItemSizes()));
        }

        d->central_widget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
        if (d->central_widget->layout())
            delete d->central_widget->layout();

        // Create a layout for the task summary widget if needed:
        QHBoxLayout* horizontal_layout = new QHBoxLayout;
        horizontal_layout->setMargin(0);
        horizontal_layout->setSpacing(0);
        horizontal_layout->addWidget(d->mode_manager->modeListWidget());

        if (!d->task_summary_widget) {
            d->task_summary_widget = new TaskSummaryWidget;
            d->task_summary_widget->setNoActiveTaskHandling(TaskSummaryWidget::ShowSummaryWidget);
            QPalette palette = d->task_summary_widget->palette();
            palette.setColor(QPalette::Window,QColor("#FFFFFF"));
            d->task_summary_widget->setPalette(palette);
            d->task_summary_widget->setMaximumHeight(0);
            d->task_summary_widget->setMaximumWidth(350);
            d->task_summary_widget->setMinimumWidth(350);
            d->task_summary_widget->findCurrentTasks();
            //d->task_summary_widget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
            d->task_summary_widget->setTaskSummaryEnabled(d->task_summary_widget_visible);
        }
        horizontal_layout->addWidget(d->task_summary_widget);

        QVBoxLayout* layout = new QVBoxLayout(d->central_widget);
        if (d->mode_layout == ModesTop) {
            layout->addLayout(horizontal_layout);
            layout->addWidget(d->current_widget_holder);
        } else {
            layout->addWidget(d->current_widget_holder);
            layout->addLayout(horizontal_layout);
        }
        layout->setMargin(0);

        setCentralWidget(d->central_widget);
        d->central_widget->show();
        if (d->task_summary_widget_visible)
            d->task_summary_widget->show();

        d->mode_manager->modeListWidget()->show();
    } else if (d->mode_layout == ModesLeft || d->mode_layout == ModesRight) {
        if (!d->mode_manager) {
            d->mode_manager = new ModeManager(qti_def_DEFAULT_MODE_MANAGER,Qt::Horizontal,this);
            connect(d->mode_manager,SIGNAL(changeCentralWidget(QWidget*)),SLOT(changeCurrentWidget(QWidget*)));
            connect(d->mode_manager,SIGNAL(modeListItemSizesChanged()),SLOT(updateItemSizes()));
        }

        d->central_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
        if (d->central_widget->layout())
            delete d->central_widget->layout();

        // Create a layout for the task summary widget if needed:
        QVBoxLayout* vertical_layout = new QVBoxLayout;
        vertical_layout->setMargin(0);
        vertical_layout->setSpacing(0);
        vertical_layout->addWidget(d->mode_manager->modeListWidget());

        if (!d->task_summary_widget) {
            d->task_summary_widget = new TaskSummaryWidget;
            QPalette palette = d->task_summary_widget->palette();
            palette.setColor(QPalette::Window,QColor("#FFFFFF"));
            d->task_summary_widget->setPalette(palette);
            d->task_summary_widget->findCurrentTasks();
            d->task_summary_widget->setMaximumWidth(0);
            d->task_summary_widget->findCurrentTasks();
            d->task_summary_widget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
            d->task_summary_widget->setTaskSummaryEnabled(d->task_summary_widget_visible);
        }
        vertical_layout->addWidget(d->task_summary_widget);

        QHBoxLayout* layout = new QHBoxLayout(d->central_widget);
        if (d->mode_layout == ModesLeft) {
            layout->addLayout(vertical_layout);
            layout->addWidget(d->current_widget_holder);
        } else {
            layout->addWidget(d->current_widget_holder);
            layout->addLayout(vertical_layout);
        }

        layout->setMargin(0);
        setCentralWidget(d->central_widget);
        d->central_widget->show();
        d->mode_manager->modeListWidget()->show();
    }
}

void Qtilities::CoreGui::QtilitiesMainWindow::updateItemSizes() {
    if (d->mode_layout == ModesTop || d->mode_layout == ModesBottom) {
        int max_height = d->mode_manager->modeListWidget()->sizeHint().height();

        d->mode_manager->modeListWidget()->setMinimumHeight(max_height);
        d->mode_manager->modeListWidget()->setMaximumHeight(max_height);

        if (d->task_summary_widget) {
            d->task_summary_widget->setMaximumHeight(max_height);
            d->task_summary_widget->setMinimumHeight(max_height);
        }
    } else if (d->mode_layout == ModesLeft || d->mode_layout == ModesRight) {
        int max_width = d->mode_manager->modeListWidget()->sizeHint().width();

        d->mode_manager->modeListWidget()->setMinimumWidth(max_width);
        d->mode_manager->modeListWidget()->setMaximumWidth(max_width);

        if (d->task_summary_widget) {
            d->task_summary_widget->setMinimumWidth(max_width);
            d->task_summary_widget->setMaximumWidth(max_width);
        }
    }
}

void Qtilities::CoreGui::QtilitiesMainWindow::changeCurrentWidget(QWidget* new_central_widget) {
    if (!new_central_widget) {
        new_central_widget = new QWidget(this);
        new_central_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    }

    // Hide current widget
    if (d->current_widget) {
        d->current_widget->hide();
        d->current_widget->setParent(0);
    }

    d->current_widget = new_central_widget;

    if (d->current_widget_holder->layout())
        delete d->current_widget_holder->layout();

    QVBoxLayout* layout = new QVBoxLayout(d->current_widget_holder);
    layout->setMargin(0);
    layout->addWidget(d->current_widget);

    d->current_widget_holder->show();
    d->current_widget->show();
}

void Qtilities::CoreGui::QtilitiesMainWindow::processPriorityMessage(Logger::MessageType message_type, const QString& message) {
    QApplication::processEvents();

    if (message.isEmpty())
        return;

    if (d->priority_messages_enabled) {
        d->priority_messages_text.setVisible(true);
        if (message_type == Logger::Warning) {
            d->priority_messages_icon.setPixmap(QIcon(qti_icon_WARNING_12x12).pixmap(12));
            d->priority_messages_icon.setVisible(true);
        } else if (message_type == Logger::Error || message_type == Logger::Fatal) {
            d->priority_messages_icon.setPixmap(QIcon(qti_icon_ERROR_12x12).pixmap(12));
            d->priority_messages_icon.setVisible(true);
        } else {
            if (message.startsWith(tr("Successfully")))
                d->priority_messages_icon.setPixmap(QIcon(qti_icon_SUCCESS_12x12).pixmap(12));
            else
                d->priority_messages_icon.setPixmap(QIcon(qti_icon_INFO_12x12).pixmap(12));
            d->priority_messages_icon.setVisible(true);
        }

        QApplication::processEvents();
        d->priority_messages_text.setText(message);
        QApplication::processEvents();
        d->priority_message_timer.start(5000);
    }
    QApplication::processEvents();
}

bool Qtilities::CoreGui::QtilitiesMainWindow::eventFilter(QObject *object, QEvent *event) {
    if (object == this && event->type() == QEvent::Close) {
        if (QtilitiesCoreApplication::applicationBusy()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Application Busy");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText("You cannot close the application while it is busy.<br>Wait for it to become idle and try again.");
            msgBox.exec();
            event->ignore();
            return true;
            // TODO: We possibly want allow setting a custom message on QtilitiesApplication (not QtilitiesCoreApplication)
        }
    }

    return QWidget::eventFilter(object,event);
}

