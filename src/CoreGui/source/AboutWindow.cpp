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

#include "AboutWindow.h"
#include "ui_AboutWindow.h"
#include "QtilitiesApplication.h"

#include <QtilitiesCoreApplication_p>

#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QDesktopWidget>

Qtilities::CoreGui::qti_private_AboutWindow::qti_private_AboutWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qti_private_AboutWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("About ") + "Qtilities");
    ui->labelVersion->setText("v" + QtilitiesCoreApplicationPrivate::instance()->qtilitiesVersionString() + " (Built on " + QLatin1String(__DATE__) + " at " + QLatin1String(__TIME__) + ")");
    //ui->labelWebsite->setText("<a href=\"http://www.qtilities.org\">https://github.com/JPNaude/Qtilities</a>");
    ui->labelCopyright->setText(tr("Copyright") + " 2009-2016, Jaco Naudé");

    // Put the widget in the center of the screen
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());

    setAttribute(Qt::WA_QuitOnClose,false);
    setAttribute(Qt::WA_DeleteOnClose);
}

Qtilities::CoreGui::qti_private_AboutWindow::~qti_private_AboutWindow()
{
    delete ui;
}

void Qtilities::CoreGui::qti_private_AboutWindow::changeEvent(QEvent *e)
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
