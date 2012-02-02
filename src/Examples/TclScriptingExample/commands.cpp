/***************************************************************************
commands.cpp  -  description
-------------------
begin                : lun jun 27 2005
copyright            : (C) 2005 by houssem
email                : houssem@localhost
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "commands.h"

#include <qstringlist.h>
#include <qmessagebox.h>

#include <Logger>
using namespace Qtilities::Logging;

int CallShowWidget( ClientData, Tcl_Interp* interp, int argc, char *argv[]) {
    QString usageMsg = QString("Usage: %1 title text\n").arg(argv[0]);
    // Reset result data
    Tcl_ResetResult(interp);

    //Help message in case of wrong parameters
    if (argc != 3)
    {
            Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
            return TCL_ERROR;
    }

    LOG_INFO_P("In handler");
    QWidget* widget = new QWidget();
    widget->show();

    //calls the messagebox with the parameters
    int result = 0;

    //result = QMessageBox::warning(0, argv[1] , argv[2], QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

    //displays the return value
    Tcl_AppendResult(interp, qPrintable(QString("QMessageBox exited with result: %1\n").arg(result)), (char*) NULL);

    return TCL_OK;
}

int LogQtiMessage( ClientData, Tcl_Interp* interp, int argc, char *argv[]) {
    QString usageMsg = QString("Usage: %1 message type\n\nSupported Types:\n - info \n - warning \n - error").arg(argv[0]);
    // Reset result data
    Tcl_ResetResult(interp);

    //Help message in case of wrong parameters
    if (argc != 3) {
        Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
        return TCL_ERROR;
    }

    qDebug() << argv[0];
    qDebug() << argv[1];
    qDebug() << argv[2];
    if (QString(argv[2]) == "info")
        LOG_INFO(argv[1]);
    else if (QString(argv[2]) == "warning")
        LOG_WARNING(argv[1]);
    else if (QString(argv[2]) == "error")
        LOG_ERROR(argv[1]);
    else  {
        Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
        return TCL_ERROR;
    }

    //displays the return value
    Tcl_AppendResult(interp, qPrintable(QString("Message logged")), (char*) NULL);

    return TCL_OK;
}

int LogQtiPriorityMessage( ClientData, Tcl_Interp* interp, int argc, char *argv[]) {
    QString usageMsg = QString("Usage: %1 message type\n\nSupported Types:\n - info \n - warning \n - error").arg(argv[0]);
    // Reset result data
    Tcl_ResetResult(interp);

    //Help message in case of wrong parameters
    if (argc != 3) {
        Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
        return TCL_ERROR;
    }

    if (QString(argv[2]) == "info")
        LOG_INFO_P(argv[1]);
    else if (QString(argv[2]) == "warning")
        LOG_WARNING_P(argv[1]);
    else if (QString(argv[2]) == "error")
        LOG_ERROR_P(argv[1]);
    else  {
        Tcl_AppendResult(interp, qPrintable(usageMsg), (char*) NULL);
        return TCL_ERROR;
    }

    //displays the return value
    Tcl_AppendResult(interp, qPrintable(QString("Message logged")), (char*) NULL);

    return TCL_OK;
}


