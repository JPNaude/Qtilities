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

#include "QtilitiesMainWindow.h"
#include "ui_QtilitiesMainWindow.h"
#include "QtilitiesCoreGuiConstants.h"
#include "TaskSummaryWidget.h"

#include <QtilitiesCoreApplication>
#include <QtilitiesApplication>

#include <QBoxLayout>
#include <QSettings>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::QtilitiesMainWindowPrivateData {
    QtilitiesMainWindowPrivateData() : initialized(false),
        mode_manager(0),
        central_widget(0),
        priority_messages_enabled(true),
        task_summary_widget_visible(true),
        task_summary_widget(0),
        last_restore_state_maximized(true) {}

    bool                            initialized;
    QPointer<QWidget>               current_widget;
    QWidget*                        current_widget_holder;
    ModeManager*                    mode_manager;
    QWidget*                        central_widget;
    QPointer<QWidget>               empty_central_widget;
    bool                            priority_messages_enabled;
    QWidget                         priority_messages_widget;
    QLabel                          priority_messages_icon;
    QLabel                          priority_messages_text;
    QtilitiesMainWindow::ModeLayout mode_layout;
    QTimer                          priority_message_timer;
    bool                            task_summary_widget_visible;
    QPointer<TaskSummaryWidget>     task_summary_widget;
    bool                            last_restore_state_maximized;
};

Qtilities::CoreGui::QtilitiesMainWindow::QtilitiesMainWindow(ModeLayout modeLayout, QWidget *parent, Qt::WindowFlags flags) :
        QMainWindow(parent, flags), ui(new Ui::QtilitiesMainWindow)
{
    ui->setupUi(this);
    d = new QtilitiesMainWindowPrivateData;
    d->mode_layout = modeLayout;

    // Make a call to application session path which will set the default path in QtilitiesCoreApplication to
    // use QDesktopServices. This might be different in Qt 5 with the new QStandardPaths class.
    // Note that we cannot do this in QtilitiesApplication's constructor since the application name and
    // organization has not been set at that stage. This call is also in QtilitiesApplication::initialize().
    QtilitiesApplication::applicationSessionPath();

    if (modeLayout != ModesNone) {
        d->current_widget_holder = new QWidget;
        d->central_widget = new QWidget;
        QWidget* widget = new QWidget;
        d->empty_central_widget = widget;
        d->empty_central_widget->setObjectName("Empty Central Widget");
        d->empty_central_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
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

    if (d->empty_central_widget)
        delete d->empty_central_widget;

    delete ui;
    delete d;
}

void Qtilities::CoreGui::QtilitiesMainWindow::writeSettings(const QString &gui_id) {
    if (!QtilitiesCoreApplication::qtilitiesSettingsEnabled())
        return;

    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup(gui_id);
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("state", saveState());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::QtilitiesMainWindow::readSettings(const QString &gui_id) {
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup(gui_id);
    resize(settings.value("size", QSize(1000, 1000)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreState(settings.value("state").toByteArray());
    d->last_restore_state_maximized = settings.value("maximized",false).toBool();
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

bool Qtilities::CoreGui::QtilitiesMainWindow::lastReadSettingsIsMaximized() const {
    return d->last_restore_state_maximized;
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

void Qtilities::CoreGui::QtilitiesMainWindow::forceCloseApplication() {
    qApp->quit();
    exit(-1);
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
//            QPalette palette = d->task_summary_widget->palette();
//            palette.setColor(QPalette::Window,QColor("#FFFFFF"));
//            d->task_summary_widget->setPalette(palette);
            // The above code gives issues when applying a style on QApplication.
            // Set the background using a style sheet in the line below fixes that...
            d->task_summary_widget->setStyleSheet("QWidget {background-color:#FFFFFF }");
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
        d->current_widget_holder->show();
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

        if (max_height == 0)
            max_height = 78;

        d->mode_manager->modeListWidget()->setMinimumHeight(max_height);
        d->mode_manager->modeListWidget()->setMaximumHeight(max_height);

        if (d->task_summary_widget) {
            d->task_summary_widget->setMaximumHeight(max_height);
            d->task_summary_widget->setMinimumHeight(max_height);
        }
    } else if (d->mode_layout == ModesLeft || d->mode_layout == ModesRight) {
        int max_width = d->mode_manager->modeListWidget()->sizeHint().width();

        if (max_width == 0)
            max_width = 78;

        d->mode_manager->modeListWidget()->setMinimumWidth(max_width);
        d->mode_manager->modeListWidget()->setMaximumWidth(max_width);

        if (d->task_summary_widget) {
            d->task_summary_widget->setMinimumWidth(max_width);
            d->task_summary_widget->setMaximumWidth(max_width);
        }
    }
}

void Qtilities::CoreGui::QtilitiesMainWindow::changeCurrentWidget(QWidget* new_central_widget) {
    // Hide current widget
    if (d->current_widget) {
        // Its not shown anywhere else yet:
        if (d->current_widget->parent() == d->current_widget_holder) {
            d->current_widget->hide();
            d->current_widget->setParent(0);
        }
        //} else {
            // Its shown so
//            d->current_widget->hide();
//            d->current_widget->setParent(0);
        //}

    }

    d->current_widget = new_central_widget;
    if (!d->current_widget) {
        d->current_widget = d->empty_central_widget;
        qWarning() << "Invalid mode widget found. Can't display it. Reverting to the empty widget in order to respect layout.";
    }

    if (d->current_widget_holder->layout())
        delete d->current_widget_holder->layout();

    QVBoxLayout* layout = new QVBoxLayout(d->current_widget_holder);
    layout->setMargin(0);
    layout->addWidget(d->current_widget);

    if (d->current_widget)
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
            msgBox.setWindowTitle(tr("Application Busy"));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("You cannot close the application while it is busy.<br>Wait for it to become idle and try again."));
            QPushButton *buttonOk = msgBox.addButton(QString(tr("Ok")), QMessageBox::AcceptRole);
            QPushButton *buttonForceClose = msgBox.addButton(QString(tr("Force Close")), QMessageBox::AcceptRole);
            msgBox.setDefaultButton(buttonOk);
            msgBox.exec();
            if (msgBox.clickedButton() == buttonOk) {
                event->ignore();
                return true;
            } else if (msgBox.clickedButton() == buttonForceClose) {
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Force Close"));
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText(tr("You are about to perform a force close while the application is busy."));
                msgBox.setInformativeText(tr("Are you sure you want to do this?"));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int ret = msgBox.exec();
                switch (ret) {
                  case QMessageBox::Yes:
                        event->accept();
                        forceCloseApplication();
                        break;
                  case QMessageBox::No:
                        {
                            event->ignore();
                            return true;
                        }
                        break;
                  default:
                          // should never be reached
                          break;
                }
            }
        }
    }

    return QWidget::eventFilter(object,event);
}

void Qtilities::CoreGui::QtilitiesMainWindow::showEvent(QShowEvent *e) {
    QMainWindow::showEvent(e);
}

