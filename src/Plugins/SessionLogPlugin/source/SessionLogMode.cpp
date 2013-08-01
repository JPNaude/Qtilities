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

#include "SessionLogMode.h"
#include "SessionLogPluginConstants.h"

#include <QtilitiesCoreGui>

#include <QDockWidget>
#include <QMainWindow>

using namespace Qtilities::Plugins::SessionLog::Constants;
using namespace QtilitiesCoreGui;

struct Qtilities::Plugins::SessionLog::SessionLogModePrivateData {
    SessionLogModePrivateData() : session_mode_widget(0) {}

    QMainWindow*    session_mode_widget;
    QIcon           mode_icon;
};

Qtilities::Plugins::SessionLog::SessionLogMode::SessionLogMode(QObject* parent) : QObject(parent)
{
    d = new SessionLogModePrivateData;
    setObjectName(tr("Session Log"));

    d->session_mode_widget = new QMainWindow;
    d->mode_icon = QIcon(qti_icon_SESSION_LOG_MODE_48x48);

    QString log_widget_name = tr("Session Log");
    QWidget* session_logger_widget = LoggerGui::createLogWidget(&log_widget_name,
                                                                WidgetLoggerEngine::AllMessagesPlainTextEdit |
                                                                WidgetLoggerEngine::WarningsPlainTextEdit |
                                                                WidgetLoggerEngine::ErrorsPlainTextEdit);
    d->session_mode_widget->setCentralWidget(session_logger_widget);
//    if (session_log_dock) {
//        connect(session_log_dock,SIGNAL(visibilityChanged(bool)),SLOT(handle_dockVisibilityChanged(bool)));
//        d->session_mode_widget->addDockWidget(Qt::TopDockWidgetArea,session_log_dock);
//    }

//    log_widget_name = tr("Warnings");
//    QDockWidget* warning_dock = LoggerGui::createLogDockWidget(&log_widget_name,QString(),true,Logger::Warning);
//    if (warning_dock) {
//        connect(warning_dock,SIGNAL(visibilityChanged(bool)),SLOT(handle_dockVisibilityChanged(bool)));
//        d->session_mode_widget->addDockWidget(Qt::TopDockWidgetArea,warning_dock);
//    }

//    log_widget_name = tr("Errors");
//    QDockWidget* error_dock = LoggerGui::createLogDockWidget(&log_widget_name,QString(),true,Logger::Error | Logger::Fatal);
//    if (error_dock) {
//        connect(error_dock,SIGNAL(visibilityChanged(bool)),SLOT(handle_dockVisibilityChanged(bool)));
//        d->session_mode_widget->addDockWidget(Qt::TopDockWidgetArea,error_dock);
//    }

//    if (warning_dock && error_dock)
//        d->session_mode_widget->tabifyDockWidget(warning_dock,error_dock);
//    if (error_dock && session_log_dock)
//        d->session_mode_widget->tabifyDockWidget(error_dock,session_log_dock);
}

Qtilities::Plugins::SessionLog::SessionLogMode::~SessionLogMode() {
    delete d;
}

QWidget* Qtilities::Plugins::SessionLog::SessionLogMode::modeWidget() {
    return d->session_mode_widget;
}

QIcon Qtilities::Plugins::SessionLog::SessionLogMode::modeIcon() const {
    return d->mode_icon;
}

bool Qtilities::Plugins::SessionLog::SessionLogMode::setModeIcon(QIcon icon) {
    d->mode_icon = icon;
    emit modeIconChanged();
    return true;
}

QString Qtilities::Plugins::SessionLog::SessionLogMode::modeName() const {
    return tr("Session Log");
}

void Qtilities::Plugins::SessionLog::SessionLogMode::handle_dockVisibilityChanged(bool visible) {
    Q_UNUSED(visible)
//    QDockWidget* dock = qobject_cast<QDockWidget*> (sender());
//    if (dock && visible) {
//        WidgetLoggerEngineFrontend* front_end = qobject_cast<WidgetLoggerEngineFrontend*> (dock->widget());
//        if (front_end) {
//            CONTEXT_MANAGER->setNewContext(front_end->contextString(),true);
//        }
//    }
}
