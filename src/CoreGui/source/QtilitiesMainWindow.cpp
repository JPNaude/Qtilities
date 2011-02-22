/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#include <QBoxLayout>
#include <QSettings>
#include <QLabel>

using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::QtilitiesMainWindowPrivateData {
    QtilitiesMainWindowPrivateData() : initialized(false),
    current_widget(0),
    mode_manager(0),
    central_widget(0),
    priority_messages_enabled(true) {}

    bool                            initialized;
    QWidget*                        current_widget;
    ModeManager*                    mode_manager;
    QWidget*                        central_widget;
    bool                            priority_messages_enabled;
    QWidget                         priority_messages_widget;
    QLabel                          priority_messages_icon;
    QLabel                          priority_messages_text;
    QtilitiesMainWindow::ModeLayout mode_layout;
    QTimer                          priority_message_timer;
};

Qtilities::CoreGui::QtilitiesMainWindow::QtilitiesMainWindow(ModeLayout modeLayout, QWidget* parent, Qt::WindowFlags flags) :
        QMainWindow(parent, flags), ui(new Ui::QtilitiesMainWindow)
{
    ui->setupUi(this);
    d = new QtilitiesMainWindowPrivateData;
    d->mode_layout = modeLayout;

    if (modeLayout != ModesNone) {
        d->central_widget = new QWidget();
        d->current_widget = new QWidget();
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
    d->priority_messages_text.setVisible(false);

    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_text, SLOT(clear()));
    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_text, SLOT(hide()));
    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_icon, SLOT(clear()));
    connect(&d->priority_message_timer,SIGNAL(timeout()), &d->priority_messages_icon, SLOT(hide()));
}

Qtilities::CoreGui::QtilitiesMainWindow::~QtilitiesMainWindow() {
    delete ui;
    delete d;
}

void Qtilities::CoreGui::QtilitiesMainWindow::writeSettings() {
    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("state", saveState());
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::QtilitiesMainWindow::readSettings() {
    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1000, 1000)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::QtilitiesMainWindow::enablePriorityMessages() {
    d->priority_messages_enabled = true;
}

void Qtilities::CoreGui::QtilitiesMainWindow::disablePriorityMessages() {
    d->priority_messages_enabled = false;
}

Qtilities::CoreGui::ModeManager* Qtilities::CoreGui::QtilitiesMainWindow::modeManager() {
    return d->mode_manager;
}

Qtilities::CoreGui::QtilitiesMainWindow::ModeLayout Qtilities::CoreGui::QtilitiesMainWindow::modeLayout() const {
    return d->mode_layout;
}

void Qtilities::CoreGui::QtilitiesMainWindow::changeCurrentWidget(QWidget* new_central_widget) {
    if (!new_central_widget)
        return;

    // Hide current widget
    if (d->current_widget)
        d->current_widget->hide();

    d->current_widget = new_central_widget;

    if (d->mode_layout == ModesTop || d->mode_layout == ModesBottom) {
        if (!d->mode_manager) {
            d->mode_manager = new ModeManager(Qt::Vertical,this);
            connect(d->mode_manager,SIGNAL(changeCentralWidget(QWidget*)),SLOT(changeCurrentWidget(QWidget*)));
        }

        if (d->central_widget->layout())
            delete d->central_widget->layout();

        QVBoxLayout* layout = new QVBoxLayout(d->central_widget);
        if (d->mode_layout == ModesTop) {
            layout->addWidget(d->mode_manager->modeListWidget());
            layout->addWidget(d->current_widget);
        } else {
            layout->addWidget(d->current_widget);
            layout->addWidget(d->mode_manager->modeListWidget());
        }
        layout->setMargin(0);
        d->mode_manager->modeListWidget()->setMinimumSize(d->mode_manager->modeListWidget()->sizeHint());
        d->mode_manager->modeListWidget()->show();
        d->current_widget->show();
        setCentralWidget(d->central_widget);
        d->central_widget->show();
    } else if (d->mode_layout == ModesLeft || d->mode_layout == ModesRight) {
        if (!d->mode_manager) {
            d->mode_manager = new ModeManager(Qt::Horizontal,this);
            connect(d->mode_manager,SIGNAL(changeCentralWidget(QWidget*)),SLOT(changeCurrentWidget(QWidget*)));
        }

        if (d->central_widget->layout())
            delete d->central_widget->layout();

        QHBoxLayout* layout = new QHBoxLayout(d->central_widget);
        if (d->mode_layout == ModesLeft) {
            layout->addWidget(d->mode_manager->modeListWidget());
            layout->addWidget(d->current_widget);
        } else {
            layout->addWidget(d->current_widget);
            layout->addWidget(d->mode_manager->modeListWidget());
        }
        layout->setMargin(0);
        d->mode_manager->modeListWidget()->setMinimumSize(d->mode_manager->modeListWidget()->sizeHint());
        d->mode_manager->modeListWidget()->show();
        d->current_widget->show();
        setCentralWidget(d->central_widget);
        d->central_widget->show();
    }
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
