/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#include "EnvironmentTypesConfigWrapper.h"
#include "VisualWorkspaceCoreConstants.h"

#include <EnvironmentTypesConfig.h>

using namespace Qtilities::QtSF::Environments;

struct VisualWorkspace::Core::EnvironmentTypesConfigWrapperData {
    EnvironmentTypesConfigWrapperData() : widget(0) {}

    EnvironmentTypesConfig* widget;
};

VisualWorkspace::Core::EnvironmentTypesConfigWrapper::EnvironmentTypesConfigWrapper(QObject* parent) : QObject(parent)
{
    d = new EnvironmentTypesConfigWrapperData;
    setObjectName(tr("Environment Types Config Page"));
    d->widget = new EnvironmentTypesConfig();
}

VisualWorkspace::Core::EnvironmentTypesConfigWrapper::~EnvironmentTypesConfigWrapper() {
    delete d;
}

void VisualWorkspace::Core::EnvironmentTypesConfigWrapper::initialize() {
    d->widget->initialize();
}

QIcon VisualWorkspace::Core::EnvironmentTypesConfigWrapper::configPageIcon() const {
    if (d->widget)
        return d->widget->windowIcon();
    else
        return QIcon();
}

QWidget* VisualWorkspace::Core::EnvironmentTypesConfigWrapper::configPageWidget() {
    return d->widget;
}

QStringList VisualWorkspace::Core::EnvironmentTypesConfigWrapper::configPageTitle() const {
    QStringList text;
    text << "Environments" << "Types";
    return text;
}

void VisualWorkspace::Core::EnvironmentTypesConfigWrapper::configPageApply() {
    d->widget->applyChanges();
}

