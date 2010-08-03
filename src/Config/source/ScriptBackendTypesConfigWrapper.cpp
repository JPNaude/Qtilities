/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#include "ScriptBackendTypesConfigWrapper.h"
#include "VisualWorkspaceCoreConstants.h"

#include <ScriptBackendTypesConfig.h>

using namespace Qtilities::QtSF::Scripting;

struct VisualWorkspace::Core::ScriptBackendTypesConfigWrapperData {
    ScriptBackendTypesConfigWrapperData() : widget(0) {}

    ScriptBackendTypesConfig* widget;
};

VisualWorkspace::Core::ScriptBackendTypesConfigWrapper::ScriptBackendTypesConfigWrapper(QObject* parent) : QObject(parent)
{
    d = new ScriptBackendTypesConfigWrapperData;
    setObjectName(tr("Script Backend Types Config Page"));
    d->widget = new ScriptBackendTypesConfig();
}

VisualWorkspace::Core::ScriptBackendTypesConfigWrapper::~ScriptBackendTypesConfigWrapper() {
    delete d;
}

void VisualWorkspace::Core::ScriptBackendTypesConfigWrapper::initialize() {
    d->widget->initialize();
}

QIcon VisualWorkspace::Core::ScriptBackendTypesConfigWrapper::configPageIcon() const {
    if (d->widget)
        return d->widget->windowIcon();
    else
        return QIcon();
}

QWidget* VisualWorkspace::Core::ScriptBackendTypesConfigWrapper::configPageWidget() {
    return d->widget;
}

QStringList VisualWorkspace::Core::ScriptBackendTypesConfigWrapper::configPageTitle() const {
    QStringList text;
    text << "Scripting" << "Backend Options";
    return text;
}

void VisualWorkspace::Core::ScriptBackendTypesConfigWrapper::configPageApply() {
    d->widget->applyChanges();
}

