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

#include "QtilitiesApplication_p.h"
#include "QtilitiesCoreApplication_p.h"
#include "ObjectManager.h"
#include "ContextManager.h"

#include <LoggingConstants>

#include <QObject>

using namespace Qtilities::Core;

Qtilities::CoreGui::QtilitiesApplicationPrivate* Qtilities::CoreGui::QtilitiesApplicationPrivate::m_Instance = 0;

Qtilities::CoreGui::QtilitiesApplicationPrivate* Qtilities::CoreGui::QtilitiesApplicationPrivate::instance() {
    static QMutex mutex;
    if (!m_Instance)
    {
        mutex.lock();

        if (!m_Instance)
            m_Instance = new QtilitiesApplicationPrivate;

        mutex.unlock();
    }

    return m_Instance;
}

Qtilities::CoreGui::QtilitiesApplicationPrivate::QtilitiesApplicationPrivate() {
    // Action Manager
    d_actionManager = new ActionManager;
    QObject* actionManagerQ = qobject_cast<QObject*> (d_actionManager);
    d_actionManagerIFace = qobject_cast<IActionManager*> (actionManagerQ);
    QObject::connect(QtilitiesCoreApplicationPrivate::instance()->contextManager(),SIGNAL(contextChanged(QList<int>)),d_actionManager,SLOT(handleContextChanged(QList<int>)));

    // Clipboard Manager
    d_clipboardManager = new ClipboardManager;
    QObject* clipboardManagerQ = qobject_cast<QObject*> (d_clipboardManager);
    d_clipboardManagerIFace = qobject_cast<IClipboard*> (clipboardManagerQ);

    #ifndef QTILITIES_NO_HELP
    // Help Manager
    d_helpManager = new HelpManager;
    #endif
}

Qtilities::CoreGui::QtilitiesApplicationPrivate::~QtilitiesApplicationPrivate() {

}

Qtilities::CoreGui::Interfaces::IActionManager* Qtilities::CoreGui::QtilitiesApplicationPrivate::actionManager() const {
    return d_actionManagerIFace;
}

Qtilities::CoreGui::Interfaces::IClipboard* Qtilities::CoreGui::QtilitiesApplicationPrivate::clipboardManager() const {
    return d_clipboardManagerIFace;
}

#ifndef QTILITIES_NO_HELP
Qtilities::CoreGui::HelpManager* Qtilities::CoreGui::QtilitiesApplicationPrivate::helpManager() const {
    return d_helpManager;
}
#endif

void Qtilities::CoreGui::QtilitiesApplicationPrivate::setMainWindow(QWidget* mainWindow) {
    d_mainWindow = mainWindow;
}

QWidget* Qtilities::CoreGui::QtilitiesApplicationPrivate::mainWindow() const {
    return d_mainWindow;
}

void Qtilities::CoreGui::QtilitiesApplicationPrivate::setConfigWidget(QWidget* configWidget) {
    d_configWindow = configWidget;
}

QWidget* Qtilities::CoreGui::QtilitiesApplicationPrivate::configWidget() {
    return d_configWindow;
}
