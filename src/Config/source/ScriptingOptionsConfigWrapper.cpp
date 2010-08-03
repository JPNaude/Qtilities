/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#include "ScriptingOptionsConfigWrapper.h"
#include "VisualWorkspaceCoreConstants.h"

#include <ScriptingOptionsConfig.h>

using namespace Qtilities::QtSF::Scripting;

struct VisualWorkspace::Core::ScriptingOptionsConfigWrapperData {
    ScriptingOptionsConfigWrapperData() : widget(0) {}

    ScriptingOptionsConfig* widget;
};

VisualWorkspace::Core::ScriptingOptionsConfigWrapper::ScriptingOptionsConfigWrapper(QObject* parent) : QObject(parent)
{
    d = new ScriptingOptionsConfigWrapperData;
    setObjectName(tr("Scripting Options Config Page"));
    d->widget = new ScriptingOptionsConfig();
}

VisualWorkspace::Core::ScriptingOptionsConfigWrapper::~ScriptingOptionsConfigWrapper() {
    delete d;
}

void VisualWorkspace::Core::ScriptingOptionsConfigWrapper::initialize() {
    if (d->widget)
        return d->widget->initialize();
}

QIcon VisualWorkspace::Core::ScriptingOptionsConfigWrapper::configPageIcon() const {
    if (d->widget)
        return d->widget->windowIcon();
    else
        return QIcon();
}

QWidget* VisualWorkspace::Core::ScriptingOptionsConfigWrapper::configPageWidget() {
    return d->widget;
}

QStringList VisualWorkspace::Core::ScriptingOptionsConfigWrapper::configPageTitle() const {
    QStringList text;
    text << "Scripting" << "Scriptable Objects";
    return text;
}

void VisualWorkspace::Core::ScriptingOptionsConfigWrapper::configPageApply() {
    if (d->widget)
        return d->widget->applyChanges();
}
