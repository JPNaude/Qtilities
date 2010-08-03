/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#include "EnvironmentOptionsConfigWrapper.h"
#include "VisualWorkspaceCoreConstants.h"

#include <EnvironmentOptionsConfig.h>

using namespace Qtilities::QtSF::Environments;

struct VisualWorkspace::Core::EnvironmentOptionsConfigWrapperData {
    EnvironmentOptionsConfigWrapperData() : widget(0) {}

    EnvironmentOptionsConfig* widget;
};

VisualWorkspace::Core::EnvironmentOptionsConfigWrapper::EnvironmentOptionsConfigWrapper(QObject* parent) : QObject(parent)
{
    d = new EnvironmentOptionsConfigWrapperData;
    setObjectName(tr("Environment Options Config Page"));
    d->widget = new EnvironmentOptionsConfig();
}

VisualWorkspace::Core::EnvironmentOptionsConfigWrapper::~EnvironmentOptionsConfigWrapper() {
    delete d;
}

void VisualWorkspace::Core::EnvironmentOptionsConfigWrapper::initialize() {
    if (d->widget)
        return d->widget->initialize();
}

QIcon VisualWorkspace::Core::EnvironmentOptionsConfigWrapper::configPageIcon() const {
    if (d->widget)
        return d->widget->windowIcon();
    else
        return QIcon();
}

QWidget* VisualWorkspace::Core::EnvironmentOptionsConfigWrapper::configPageWidget() {
    return d->widget;
}

QStringList VisualWorkspace::Core::EnvironmentOptionsConfigWrapper::configPageTitle() const {
    QStringList text;
    text << "Environments" << "General Options";
    return text;
}

void VisualWorkspace::Core::EnvironmentOptionsConfigWrapper::configPageApply() {

}
