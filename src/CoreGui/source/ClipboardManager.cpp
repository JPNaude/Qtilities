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

#include "ClipboardManager.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"
#include "Command.h"

#include <QtilitiesCoreConstants>
#include <Observer>
#include <QtilitiesProperty>

#include <QAction>
#include <QApplication>
#include <QClipboard>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core;

struct Qtilities::CoreGui::ClipboardManagerPrivateData {
    ClipboardManagerPrivateData() : initialized(false),
    actionPaste(0),
    clipboard(0) {}

    bool initialized;
    QAction* actionPaste;
    QClipboard* clipboard;
    IClipboard::ClipboardOrigin current_origin;
    QPointer<QMimeData> mime_data;
};

Qtilities::CoreGui::ClipboardManager::ClipboardManager(QObject* parent) : IClipboard(parent)
{
    d = new ClipboardManagerPrivateData;
    d->current_origin = IClipboard::Unspecified;

    // Connect to the application clipboard
    d->clipboard = QApplication::clipboard();
    connect(d->clipboard,SIGNAL(dataChanged()),SLOT(handleClipboardChanged()));

    setObjectName(tr("Clipboard Manager"));
}

Qtilities::CoreGui::ClipboardManager::~ClipboardManager()
{
    delete d;
}

Qtilities::CoreGui::IClipboard::ClipboardOrigin Qtilities::CoreGui::ClipboardManager::clipboardOrigin() {
    return d->current_origin;
}

void Qtilities::CoreGui::ClipboardManager::setClipboardOrigin(IClipboard::ClipboardOrigin new_origin) {
    d->current_origin = new_origin;
}

void Qtilities::CoreGui::ClipboardManager::initialize() {
    if (d->initialized)
        return;

    QList<int> context;
    context << CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD);

    // ---------------------------
    // Paste
    // ---------------------------
    d->actionPaste = new QAction(QIcon(qti_icon_EDIT_PASTE_16x16),tr("Paste"),this);
    d->actionPaste->setShortcut(QKeySequence(QKeySequence::Paste));
    d->actionPaste->setEnabled(false);
    Command* command = ACTION_MANAGER->registerAction(qti_action_EDIT_PASTE,d->actionPaste,context);
    command->setCategory(QtilitiesCategory(tr("Editing")));

    d->initialized = true;
}

void Qtilities::CoreGui::ClipboardManager::handleClipboardChanged() {   
    if (d->clipboard && d->actionPaste) {
        // Handle paste operation of mime data
        if (d->clipboard->mimeData())
            d->actionPaste->setEnabled(true);

        // Handle paste operation of text
        if (!d->clipboard->text().isEmpty())
            d->actionPaste->setEnabled(true);
    }
}

void Qtilities::CoreGui::ClipboardManager::acceptMimeData() {
    d->clipboard->clear();
    d->actionPaste->setEnabled(false);
}

void Qtilities::CoreGui::ClipboardManager::setMimeData(QMimeData* mimeData) {
    d->mime_data = mimeData;
}

QMimeData* Qtilities::CoreGui::ClipboardManager::mimeData() const {
    return d->mime_data;
}

void Qtilities::CoreGui::ClipboardManager::clearMimeData() {
    if (d->mime_data)
        delete d->mime_data;
}

