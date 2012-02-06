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

#include "TclScriptingMode.h"
#include "ui_TclScriptingMode.h"

#include <QtGui>

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include "commands.h"
#include "commandsManager.h"

struct Qtilities::Examples::TclScripting::TclScriptingModePrivateData {
    TclScriptingModePrivateData() : initialized(false)  {}

    bool            initialized;
    QDockWidget*    log_widget;
};

Qtilities::Examples::TclScripting::TclScriptingMode::TclScriptingMode(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TclScriptingMode)
{
    ui->setupUi(this);
    d = new TclScriptingModePrivateData;

    QString name = "Test Log";
    d->log_widget = LoggerGui::createLogDockWidget(&name);
    addDockWidget(Qt::BottomDockWidgetArea,d->log_widget);
}

Qtilities::Examples::TclScripting::TclScriptingMode::~TclScriptingMode()
{
    delete ui;
    delete d;
}

QWidget *Examples::TclScripting::TclScriptingMode::dock() {
    return d->log_widget;
}

QWidget* Qtilities::Examples::TclScripting::TclScriptingMode::modeWidget() {
    return this;
}

void Qtilities::Examples::TclScripting::TclScriptingMode::initializeMode() {
    if (d->initialized)
        return;

    d->initialized = true;
}

QIcon Qtilities::Examples::TclScripting::TclScriptingMode::modeIcon() const {
    return QIcon(qti_icon_QTILITIES_SYMBOL_64x64);
}

QString Qtilities::Examples::TclScripting::TclScriptingMode::modeName() const {
    return tr("Tcl Scripting Console");
}
