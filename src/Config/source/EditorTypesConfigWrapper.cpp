/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#include "EditorTypesConfigWrapper.h"
#include "VisualWorkspaceCoreConstants.h"

#include <EditorTypesConfig.h>

using namespace Qtilities::QtSF::Scripting;

struct VisualWorkspace::Core::EditorTypesConfigWrapperData {
    EditorTypesConfigWrapperData() : widget(0) {}

    EditorTypesConfig* widget;
};

VisualWorkspace::Core::EditorTypesConfigWrapper::EditorTypesConfigWrapper(QObject* parent) : QObject(parent)
{
    d = new EditorTypesConfigWrapperData;
    setObjectName(tr("Editor Types Config Page"));
    d->widget = new EditorTypesConfig();
}

VisualWorkspace::Core::EditorTypesConfigWrapper::~EditorTypesConfigWrapper() {
    delete d;
}

void VisualWorkspace::Core::EditorTypesConfigWrapper::initialize() {
    d->widget->initialize();
}

QIcon VisualWorkspace::Core::EditorTypesConfigWrapper::configPageIcon() const {
    if (d->widget)
        return d->widget->windowIcon();
    else
        return QIcon();
}

QWidget* VisualWorkspace::Core::EditorTypesConfigWrapper::configPageWidget() {
    return d->widget;
}

QStringList VisualWorkspace::Core::EditorTypesConfigWrapper::configPageTitle() const {
    QStringList text;
    text << "Scripting" << "Editor Options";
    return text;
}

void VisualWorkspace::Core::EditorTypesConfigWrapper::configPageApply() {
    d->widget->applyChanges();
}
