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

#include "SessionLogMode.h"
#include "SessionLogPluginConstants.h"

#include <QtilitiesCoreGui>

#include <QDockWidget>
#include <QMainWindow>

using namespace Qtilities::Plugins::SessionLog::Constants;
using namespace QtilitiesCoreGui;

struct Qtilities::Plugins::SessionLog::SessionLogModeData {
    SessionLogModeData() : session_mode_widget(0) {}

    QMainWindow* session_mode_widget;
};

Qtilities::Plugins::SessionLog::SessionLogMode::SessionLogMode(QObject* parent) : QObject(parent)
{
    d = new SessionLogModeData;
    setObjectName(tr("Session Log"));

    d->session_mode_widget = new QMainWindow();

    QDockWidget* session_log_dock = LoggerGui::createLogDockWidget(tr("Session Log"),true);
    connect(session_log_dock,SIGNAL(visibilityChanged(bool)),SLOT(handle_dockVisibilityChanged(bool)));
    d->session_mode_widget->addDockWidget(Qt::TopDockWidgetArea,session_log_dock);

    QDockWidget* warning_dock = LoggerGui::createLogDockWidget(tr("Warnings"),true,Logger::Warning);
    connect(warning_dock,SIGNAL(visibilityChanged(bool)),SLOT(handle_dockVisibilityChanged(bool)));
    d->session_mode_widget->addDockWidget(Qt::TopDockWidgetArea,warning_dock);

    QDockWidget* error_dock = LoggerGui::createLogDockWidget(tr("Errors"),true,Logger::Error | Logger::Fatal);
    connect(error_dock,SIGNAL(visibilityChanged(bool)),SLOT(handle_dockVisibilityChanged(bool)));
    d->session_mode_widget->addDockWidget(Qt::TopDockWidgetArea,error_dock);

    d->session_mode_widget->tabifyDockWidget(warning_dock,error_dock);
    d->session_mode_widget->tabifyDockWidget(error_dock,session_log_dock);
}

Qtilities::Plugins::SessionLog::SessionLogMode::~SessionLogMode() {
    delete d;
}

QWidget* Qtilities::Plugins::SessionLog::SessionLogMode::modeWidget() {
    return d->session_mode_widget;
}

QIcon Qtilities::Plugins::SessionLog::SessionLogMode::modeIcon() const {
    return QIcon(SESSION_LOG_MODE_ICON_48x48);
}

QString Qtilities::Plugins::SessionLog::SessionLogMode::modeName() const {
    return tr("Session Log");
}

void Qtilities::Plugins::SessionLog::SessionLogMode::handle_dockVisibilityChanged(bool visible) {
    QDockWidget* dock = qobject_cast<QDockWidget*> (sender());
    if (dock && visible) {
        WidgetLoggerEngineFrontend* front_end = qobject_cast<WidgetLoggerEngineFrontend*> (dock->widget());
        if (front_end) {
            CONTEXT_MANAGER->setNewContext(front_end->contextString(),true);
        }
    }
}
