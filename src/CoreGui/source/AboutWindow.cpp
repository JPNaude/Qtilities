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

#include "AboutWindow.h"
#include "ui_AboutWindow.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFileInfo>

Qtilities::CoreGui::AboutWindow::AboutWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString(tr("About %1")).arg(QApplication::applicationName()));


    // Set labels
    if (QApplication::applicationVersion().isEmpty())
        ui->labelApplicationVersion->setVisible(false);
    else
        ui->labelApplicationVersion->setText("v" + QApplication::applicationVersion());
    QFileInfo fi(QApplication::applicationFilePath());
    QString build_date = fi.created().toString();
    ui->labelBuildDate->setText(build_date);
    ui->labelCopyright->setText(tr("Copyright ©") + " 2010, Jaco Naude");
    ui->labelWebsite->setText("");
    ui->labelExtendedDescription->setVisible(false);
    ui->labelApplicationName->setText(QApplication::applicationName());

    // Put the widget in the center of the screen
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    // By default the logo is visible:
    setLogoVisible(true);
}

Qtilities::CoreGui::AboutWindow::~AboutWindow()
{
    delete ui;
}

void Qtilities::CoreGui::AboutWindow::setCopyright(const QString& copyright) {
    ui->labelCopyright->setText(copyright);
}

void Qtilities::CoreGui::AboutWindow::setLogo(const QPixmap& pixmap) {
    ui->labelImage->setPixmap(pixmap);
}

void Qtilities::CoreGui::AboutWindow::setVersionString(const QString& version_string) {
    ui->labelApplicationVersion->setText(version_string);
}

void Qtilities::CoreGui::AboutWindow::setWebsite(const QString& website, const QString& displayed_name) {
    if (displayed_name.isEmpty())
        ui->labelWebsite->setText(QString("<a href=\"%1\">%2</a>").arg(website).arg(website));
    else
        ui->labelWebsite->setText(QString("<a href=\"%1\">%2</a>").arg(website).arg(displayed_name));
}

void Qtilities::CoreGui::AboutWindow::setExtendedDescription(const QString& extended_description) {
    if (extended_description.isEmpty()) {
        ui->labelExtendedDescription->setVisible(false);
    } else{
        ui->labelExtendedDescription->setVisible(true);
        ui->labelExtendedDescription->setText(extended_description);
    }
}

bool Qtilities::CoreGui::AboutWindow::logoVisible() const {
    return ui->labelImage->isVisible();
}

void Qtilities::CoreGui::AboutWindow::setLogoVisible(bool is_visible) {
    ui->labelImage->setVisible(is_visible);
    ui->labelApplicationName->setVisible(!is_visible);
    if (is_visible)
        resize(475,400);
    else
        resize(475,150);
}

void Qtilities::CoreGui::AboutWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
