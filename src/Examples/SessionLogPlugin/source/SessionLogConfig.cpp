/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#include "SessionLogConfig.h"
#include "ApplicationGuiConstants.h"

#include <Logger.h>

struct Qtilities::ApplicationGui::SessionLogConfigData {
    SessionLogConfigData() : widget(0) {}

    QWidget* widget;
};

Qtilities::ApplicationGui::SessionLogConfig::SessionLogConfig(QObject* parent) : QObject(parent)
{
    d = new SessionLogConfigData;
    setObjectName(tr("Session Log Config Page"));
    LOG_INIT();
    // Track d->widget = Log->configWidget();
}

Qtilities::ApplicationGui::SessionLogConfig::~SessionLogConfig() {
    delete d;
}

QIcon Qtilities::ApplicationGui::SessionLogConfig::configPageIcon() const {
    return QIcon(Constants::ICON_SESSION_LOG_22x22);
}

QWidget* Qtilities::ApplicationGui::SessionLogConfig::configPageWidget() {
    return d->widget;
}

QStringList Qtilities::ApplicationGui::SessionLogConfig::configPageTitle() const {
    QStringList text;
    text << "General" << "Session Log";
    return text;
}

void Qtilities::ApplicationGui::SessionLogConfig::configPageApply() {

}
