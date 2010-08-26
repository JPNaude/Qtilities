/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include "Command.h"
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Logger.h>

#include <QPointer>

using namespace Qtilities::CoreGui::Constants;

// --------------------------------
// Command Implemenation
// --------------------------------
Qtilities::CoreGui::Command::Command(QObject* parent) : QObject(parent)
{
    c = new CommandData;
}

Qtilities::CoreGui::Command::~Command() {
    delete c;
}

void Qtilities::CoreGui::Command::setDefaultKeySequence(const QKeySequence &key) {
    c->default_key_sequence = key;
    handleKeySequenceChange();
    emit keySequenceChanged();
}

void Qtilities::CoreGui::Command::setKeySequence(const QKeySequence &key) {
    c->current_key_sequence = key;
    handleKeySequenceChange();
    emit keySequenceChanged();
}

QKeySequence Qtilities::CoreGui::Command::defaultKeySequence() const {
    return c->default_key_sequence;
}

QKeySequence Qtilities::CoreGui::Command::keySequence() const {
    return c->current_key_sequence;
}

void Qtilities::CoreGui::Command::setDefaultText(const QString &text) {
    c->default_text = text;
}

QString Qtilities::CoreGui::Command::defaultText() const {
    return c->default_text;
}

// --------------------------------
// Action Implemenation
// --------------------------------
struct Qtilities::CoreGui::ActionData {
    ActionData() : base_action(0) { }

    QAction* base_action;
    QString original_tooltip;
    Action::InActivePolicy inActivePolicy;
};

Qtilities::CoreGui::Action::Action(QAction* user_visible_action, QObject* parent) : Command(parent) {
    b = new ActionData;

    b->base_action = user_visible_action;
    if (b->base_action) {
        b->base_action->setEnabled(false);
        b->base_action->setParent(this);
        b->original_tooltip = b->base_action->toolTip();
    }

    b->inActivePolicy = Action::DisableAction;
}

Qtilities::CoreGui::Action::~Action() {
    delete b;
}

void Qtilities::CoreGui::Action::setInActivePolicy(Action::InActivePolicy policy) {
    b->inActivePolicy = policy;
}

Qtilities::CoreGui::Action::InActivePolicy Qtilities::CoreGui::Action::inActivePolicy() {
    return b->inActivePolicy;
}

QAction* Qtilities::CoreGui::Action::action() const {
    return b->base_action;
}

QShortcut* Qtilities::CoreGui::Action::shortcut() const {
    return 0;
}

void Qtilities::CoreGui::Action::handleKeySequenceChange() {
    // Check if there is no current action but a default. In that case we set the current to the default.
    if (c->current_key_sequence.isEmpty() && !c->default_key_sequence.isEmpty()) {
        c->current_key_sequence = c->default_key_sequence;
    }

    b->base_action->setShortcut(c->current_key_sequence);
    updateToolTipWithKeySequence();
}

QString Qtilities::CoreGui::Action::text() const
{
    if (!b->base_action)
        return QString();

    return b->base_action->text();
}

void Qtilities::CoreGui::Action::updateToolTipWithKeySequence()
{
    if (b->base_action->shortcut().isEmpty())
        b->base_action->setToolTip(b->original_tooltip);
    else {
        QString appendedString = QString("%1 <span style=\"color: gray; font-size: small\">%2</span>").arg(b->original_tooltip).arg(
                keySequence().toString(QKeySequence::NativeText));
        b->base_action->setToolTip(appendedString);
    }
}

// --------------------------------
// MultiContextAction Implemenation
// --------------------------------
struct Qtilities::CoreGui::MultiContextActionData {
    MultiContextActionData() : initialized(false),
    is_active(false) { }

    bool initialized;
    bool is_active;
    QList<int> active_contexts;
    QPointer<QAction> active_action;
    QHash<int, QPointer<QAction> > id_action_map;
};

Qtilities::CoreGui::MultiContextAction::MultiContextAction(QAction* user_visible_action, QObject* parent) : Action(user_visible_action, parent) {
    d = new MultiContextActionData;

}

Qtilities::CoreGui::MultiContextAction::~MultiContextAction() {
    delete d;
}

void Qtilities::CoreGui::MultiContextAction::addAction(QAction* action, QList<int> context_ids) {
    if (!action)
        return;

    if (context_ids.isEmpty()) {
        // Check if there is already an action for the standard context.
        if (d->id_action_map.keys().contains(0)) {
            LOG_WARNING(QString(tr("Attempting to register an action for a multi context (\"Standard Context\") action twice. Last action will be ignored: %1")).arg(action->text()));
            return;
        }

        // Add the action to the standard context (which will always be 0)
        d->id_action_map[0] = action;
    } else {
        for (int i = 0; i < context_ids.count(); i++) {
            // Check if there is already an action for this context
            if (d->id_action_map.keys().contains(context_ids.at(i))) {
                if (d->id_action_map[context_ids.at(i)] != 0) {
                    LOG_WARNING(tr("Attempting to register an action for a multi context action twice for a single context. Last action will be ignored: ") + action->text());
                    return;
                } else {
                    d->id_action_map[context_ids.at(i)] = action;
                }
            } else {
                d->id_action_map[context_ids.at(i)] = action;
            }
        }
    }

    updateAction();
}

bool Qtilities::CoreGui::MultiContextAction::isActive() {
    return d->is_active;
}

bool Qtilities::CoreGui::MultiContextAction::setCurrentContext(QList<int> context_ids) {
    #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
    LOG_TRACE("Context update request on command: " + defaultText());
    #endif

    // If this is just a place holder without any backend action we do nothing in here.
    if (d->id_action_map.count() == 0)
        return false;

    d->active_contexts = context_ids;

    QAction *old_action = d->active_action;
    d->active_action = 0;
    for (int i = 0; i < d->active_contexts.size(); ++i) {
        if (QAction *a = d->id_action_map.value(d->active_contexts.at(i), 0)) {
            d->active_action = a;

            #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
            LOG_TRACE("Backend action found: " + d->active_action->text() + ", backend shortcut: " + d->active_action->shortcut().toString() + ", MultiContextAction shortcut: " + b->base_action->shortcut().toString());
            #endif

            // This break will ensure that the first context is used for the case where multiple contexts are active at once.
            break;
        }
    }

    if (d->active_action == old_action && d->initialized)  {
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        LOG_TRACE("New backend action is the same as the current active backend action. Nothihng to be done in here.");
        #endif
        return true;
    }

    // Disconnect signals from old action
    if (old_action) {
        disconnect(old_action, SIGNAL(changed()), this, SLOT(updateAction()));
        disconnect(b->base_action, SIGNAL(triggered(bool)), old_action, SIGNAL(triggered(bool)));
        disconnect(b->base_action, SIGNAL(toggled(bool)), old_action, SLOT(setChecked(bool)));
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        QObject* parent = old_action->parent();
        QString parent_name = "Unspecified parent";
        if (parent) {
             parent_name = parent->objectName();
        }
        LOG_TRACE("Disconnecting multicontext action from previous backend action in parent: " + parent_name);
        #endif
    }

    // Connect signals for new action
    if (d->active_action) {
        connect(d->active_action, SIGNAL(changed()), this, SLOT(updateAction()));
        Q_ASSERT(connect(b->base_action, SIGNAL(triggered(bool)), d->active_action, SIGNAL(triggered(bool))));
        connect(b->base_action, SIGNAL(toggled(bool)), d->active_action, SLOT(setChecked(bool)));
        updateAction();
        b->base_action->setParent(d->active_action->parent());
        d->is_active = true;
        d->initialized = true;
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        QObject* parent = d->active_action->parent();
        QString parent_name = "Unspecified parent";
        if (parent) {
             parent_name = parent->objectName();
        }
        LOG_TRACE("Base action connected: " + d->active_action->text() + ", Base shortcut: " + b->base_action->shortcut().toString() + ", Parent: " + parent_name);
        #endif
        return true;
    } else {
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        LOG_TRACE("New backend action could not be found. Action will be disabled in this context.");
        #endif
    }
    // We can hide the action here if needed
    // b->base_action->setVisible(false);
    b->base_action->setEnabled(false);
    d->is_active = false;
    return false;
}

void Qtilities::CoreGui::MultiContextAction::updateAction() {
    // If this is just a place holder without any backend action we do nothing in here
    if (d->id_action_map.count() == 0)
        return;

    if (!d->active_action || !b->base_action)
        return;

    // Update the icon
    b->base_action->setIcon(d->active_action->icon());
    b->base_action->setIconText(d->active_action->iconText());

    // Update the text
    b->base_action->setText(d->active_action->text());
    b->original_tooltip = d->active_action->toolTip();
    updateToolTipWithKeySequence();
    b->base_action->setStatusTip(d->active_action->statusTip());
    b->base_action->setWhatsThis(d->active_action->whatsThis());

    b->base_action->setCheckable(d->active_action->isCheckable());
    b->base_action->setEnabled(d->active_action->isEnabled());
    b->base_action->setVisible(d->active_action->isVisible());

    bool previous_block_value = b->base_action->blockSignals(true);
    b->base_action->setChecked(d->active_action->isChecked());
    b->base_action->blockSignals(previous_block_value);
}

