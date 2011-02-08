/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#include "ClipboardManager.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"
#include "Command.h"

#include <QtilitiesCoreConstants.h>
#include <Observer.h>
#include <ObserverProperty.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core::Constants;
using namespace Qtilities::Core;

struct Qtilities::CoreGui::ClipboardManagerData {
    ClipboardManagerData() : initialized(false),
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
    d = new ClipboardManagerData;
    d->current_origin = IClipboard::Unspecified;

    // Connect to the application clipboard
    d->clipboard = QApplication::clipboard();
    connect(d->clipboard,SIGNAL(dataChanged()),SLOT(handleClipboardChanged()));

    setObjectName("Clipboard Manager");

    // Give the manager an icon
    SharedObserverProperty shared_icon_property(QVariant(QIcon(QString(ICON_MANAGER_16x16))),OBJECT_ROLE_DECORATION);
    shared_icon_property.setIsExportable(false);
    Observer::setSharedProperty(this,shared_icon_property);
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
    context << CONTEXT_MANAGER->contextID(CONTEXT_STANDARD);

    // ---------------------------
    // Paste
    // ---------------------------
    d->actionPaste = new QAction(QIcon(ICON_EDIT_PASTE_16x16),tr("Paste"),this);
    d->actionPaste->setShortcut(QKeySequence(QKeySequence::Paste));
    d->actionPaste->setEnabled(false);
    ACTION_MANAGER->registerAction(MENU_EDIT_PASTE,d->actionPaste,context);

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

