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

#include "QtilitiesMainWindow.h"
#include "ui_QtilitiesMainWindow.h"
#include "ModeWidget.h"

#include <QBoxLayout>
#include <QSettings>

using namespace Qtilities::CoreGui::Interfaces;

struct Qtilities::CoreGui::QtilitiesMainWindowData {
    QtilitiesMainWindowData() : initialized(false),
    current_widget(0) {}

    bool initialized;
    QWidget* current_widget;
    ModeWidget mode_widget;
};

Qtilities::CoreGui::QtilitiesMainWindow::QtilitiesMainWindow(QWidget* parent, Qt::WindowFlags flags) :
        QMainWindow(parent, flags), ui(new Ui::QtilitiesMainWindow)
{
    ui->setupUi(this);
    d = new QtilitiesMainWindowData;

    if (ui->modeList->layout())
        delete ui->modeList->layout();

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,ui->modeList);
    layout->addWidget(&d->mode_widget);
    layout->setMargin(0);
    d->mode_widget.show();

    connect(&d->mode_widget,SIGNAL(changeCentralWidget(QWidget*)),SLOT(handleChangeCentralWidget(QWidget*)));
    readSettings();

    // Set the window title to the application name if not empty:
    if (!QApplication::applicationName().isEmpty())
        setWindowTitle(QApplication::applicationName());
}

Qtilities::CoreGui::QtilitiesMainWindow::~QtilitiesMainWindow() {
    delete ui;
    delete d;
}

bool Qtilities::CoreGui::QtilitiesMainWindow::addMode(IMode* mode, bool initialize_mode) {
    return d->mode_widget.addMode(mode,initialize_mode);
}

void Qtilities::CoreGui::QtilitiesMainWindow::addModes(QList<IMode*> modes, bool initialize_modes) {
    d->mode_widget.addModes(modes,initialize_modes);
}

void Qtilities::CoreGui::QtilitiesMainWindow::addModes(QList<QObject*> modes, bool initialize_modes) {
    d->mode_widget.addModes(modes,initialize_modes);
}

QList<Qtilities::CoreGui::Interfaces::IMode*> Qtilities::CoreGui::QtilitiesMainWindow::modes() const {
    return d->mode_widget.modes();
}

Qtilities::CoreGui::Interfaces::IMode* Qtilities::CoreGui::QtilitiesMainWindow::activeMode() const {
    return d->mode_widget.activeMode();
}

void Qtilities::CoreGui::QtilitiesMainWindow::setActiveMode(int mode_id) {
    d->mode_widget.setActiveMode(mode_id);
}

void Qtilities::CoreGui::QtilitiesMainWindow::setActiveMode(const QString& mode_name) {
    d->mode_widget.setActiveMode(mode_name);
}

void Qtilities::CoreGui::QtilitiesMainWindow::setActiveMode(IMode* mode_iface) {
    d->mode_widget.setActiveMode(mode_iface);
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

void Qtilities::CoreGui::QtilitiesMainWindow::handleChangeCentralWidget(QWidget* new_central_widget) {
    // Hide current widget
    if (d->current_widget)
        d->current_widget->hide();

    Q_ASSERT(new_central_widget);

    if (ui->modeSelectedWidget->layout())
        delete ui->modeSelectedWidget->layout();

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,ui->modeSelectedWidget);
    layout->addWidget(new_central_widget);
    new_central_widget->show();
    layout->setMargin(0);
    d->current_widget = new_central_widget;
}
