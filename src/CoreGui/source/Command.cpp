/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#include <Logger>
#include <QtilitiesProperty>

#include <QPointer>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core;

int Qtilities::CoreGui::Command::d_category_context;

// --------------------------------
// Command Implementation
// --------------------------------
Qtilities::CoreGui::Command::Command(int category_context, QObject* parent) : QObject(parent)
{
    c = new CommandPrivateData;
    d_category_context = category_context;
    setCategory(QtilitiesCategory("General"));
}

Qtilities::CoreGui::Command::~Command() {
    delete c;
}

void Qtilities::CoreGui::Command::setDefaultKeySequence(const QKeySequence &key) {
    QKeySequence old_key_sequence = c->current_key_sequence;
    c->default_key_sequence = key;
    handleKeySequenceChange(old_key_sequence);
    emit keySequenceChanged();
}

void Qtilities::CoreGui::Command::setKeySequence(const QKeySequence &key) {
    QKeySequence old_key_sequence = c->current_key_sequence;
    c->current_key_sequence = key;
    handleKeySequenceChange(old_key_sequence);
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

void Qtilities::CoreGui::Command::setCategory(Qtilities::Core::QtilitiesCategory category) {
    // Ok it changed, thus set it again:
    if (ObjectManager::propertyExists(this,qti_prop_CATEGORY_MAP)) {
        MultiContextProperty category_property = ObjectManager::getMultiContextProperty(this,qti_prop_CATEGORY_MAP);
        category_property.setValue(qVariantFromValue(category),d_category_context);
        ObjectManager::setMultiContextProperty(this,category_property);
    } else {
        MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
        category_property.setValue(qVariantFromValue(category),d_category_context);
        ObjectManager::setMultiContextProperty(this,category_property);
    }
}

Qtilities::Core::QtilitiesCategory Qtilities::CoreGui::Command::category() const {
    Observer* obs = OBJECT_MANAGER->observerReference(d_category_context);
    if (obs) {
        QVariant category_variant = obs->getMultiContextPropertyValue(this,qti_prop_CATEGORY_MAP);
        if (category_variant.isValid()) {
            return category_variant.value<QtilitiesCategory>();
        }
    }
    return QtilitiesCategory();
}

// --------------------------------
// ProxyAction Implementation
// --------------------------------
struct Qtilities::CoreGui::ProxyActionPrivateData {
    ProxyActionPrivateData() : proxy_action(0),
    initialized(false),
    is_active(false) { }

    QAction* proxy_action;
    QString original_tooltip;
    bool initialized;
    bool is_active;
    QList<int> active_contexts;
    QPointer<QAction> active_backend_action;
    //! Hash with context IDs and their corresponding backend_action:
    QHash<int, QPointer<QAction> > id_action_map;
};

Qtilities::CoreGui::ProxyAction::ProxyAction(QAction* user_visible_action, int category_context, QObject* parent) : Command(category_context, parent) {
    d = new ProxyActionPrivateData;

    d->proxy_action = user_visible_action;
    if (d->proxy_action) {
        d->proxy_action->setEnabled(false);
        d->proxy_action->setParent(this);
        d->original_tooltip = d->proxy_action->toolTip();
    }
}

Qtilities::CoreGui::ProxyAction::~ProxyAction() {
    delete d;
}

QAction* Qtilities::CoreGui::ProxyAction::action() const {
    return d->proxy_action;
}

QShortcut* Qtilities::CoreGui::ProxyAction::shortcut() const {
    return 0;
}

QString Qtilities::CoreGui::ProxyAction::text() const
{
    if (!d->proxy_action)
        return QString();

    return d->proxy_action->text();
}

void Qtilities::CoreGui::ProxyAction::addAction(QAction* action, QList<int> context_ids) {
    if (!action)
        return;

    if (context_ids.isEmpty()) {
        // Check if there is already an action for the standard context.
        if (d->id_action_map.contains(0)) {
            LOG_WARNING(QString(tr("Attempting to register an action for a multi context (\"Standard Context\") action twice. Last action will be ignored: %1")).arg(action->text()));
            return;
        }

        // Add the action to the standard context (which will always be 0)
        d->id_action_map[0] = action;
    } else {
        for (int i = 0; i < context_ids.count(); ++i) {
            // Check if there is already an action for this context
            if (d->id_action_map.contains(context_ids.at(i))) {
                if (d->id_action_map.contains(context_ids.at(i))) {
                    LOG_DEBUG(tr("Attempting to register a backend action for a proxy action twice for a single context with name: ") + CONTEXT_MANAGER->contextString(context_ids.at(i)) + tr(". Last action will be ignored: ") + action->text());
                    qWarning() << "Attempting to register a backend action for a proxy action twice for a single context with name: " << CONTEXT_MANAGER->contextString(context_ids.at(i)) <<  ". Last action will be ignored: " <<  action->text();
                    continue;
                } else
                    d->id_action_map[context_ids.at(i)] = action;
            } else
                d->id_action_map[context_ids.at(i)] = action;

            if (action->objectName().isEmpty()) {
                if (action->text().isEmpty())
                    action->setObjectName(objectName());
                else
                    action->setObjectName(action->text());
            }
        }
    }

    // If any of the context_ids are active, we need to update the active backend action:
    setCurrentContext(CONTEXT_MANAGER->activeContexts());
    //qDebug() << "Adding action to proxy action:" << defaultText() << activeBackendAction() << CONTEXT_MANAGER->activeContexts();
}

bool Qtilities::CoreGui::ProxyAction::isActive() {
    return d->is_active;
}

bool Qtilities::CoreGui::ProxyAction::setCurrentContext(QList<int> context_ids) {
    #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
    LOG_TRACE("Context update request on command (proxy action): " + defaultText());
    #endif

    // If this is just a place holder without any backend action we do nothing in here.
    if (d->id_action_map.count() == 0)
        return false;

    d->active_contexts = context_ids;
    QAction *old_action = d->active_backend_action;
    d->active_backend_action = 0;
    for (int i = 0; i < d->active_contexts.size(); ++i) {
        if (QAction *a = d->id_action_map.value(d->active_contexts.at(i), 0)) {
            d->active_backend_action = a;
            d->active_backend_action->setObjectName(a->text());

            #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
            LOG_TRACE("Backend action found: " + d->active_backend_action->text() + ", backend shortcut: " + d->active_backend_action->shortcut().toString() + ", ProxyAction shortcut: " + d->proxy_action->shortcut().toString());
            qDebug() << "Backend action found: " + d->active_backend_action->text() + ", backend shortcut: " + d->active_backend_action->shortcut().toString() + ", ProxyAction shortcut: " + d->proxy_action->shortcut().toString();
            #endif

            // This break will ensure that the first context is used for the case where multiple contexts are active at once.
            break;
        }
    }

    if (d->active_backend_action == old_action && d->initialized)  {
        updateFrontendAction();
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        LOG_TRACE("New backend action is the same as the current active backend action. Nothing to be done in here.");
        qDebug() << "New backend action is the same as the current active backend action. Nothing to be done in here.";
        #endif
        return true;
    }

    // Disconnect signals from old action
    if (old_action) {
        disconnect(old_action, SIGNAL(changed()), this, SLOT(updateFrontendAction()));
        disconnect(d->proxy_action, SIGNAL(triggered(bool)), old_action, SIGNAL(triggered(bool)));
        disconnect(d->proxy_action, SIGNAL(toggled(bool)), old_action, SLOT(setChecked(bool)));
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
    if (d->active_backend_action) {
        connect(d->active_backend_action, SIGNAL(changed()), this, SLOT(updateFrontendAction()));
        connect(d->proxy_action, SIGNAL(triggered(bool)), d->active_backend_action, SIGNAL(triggered(bool)));
        connect(d->proxy_action, SIGNAL(toggled(bool)), d->active_backend_action, SLOT(setChecked(bool)));
        updateFrontendAction();
        d->is_active = true;
        d->initialized = true;
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        QObject* parent = d->active_backend_action->parent();
        QString parent_name = "Unspecified parent";
        if (parent) {
             parent_name = parent->objectName();
        }
        LOG_TRACE("Base action connected: " + d->active_backend_action->text() + ", Base shortcut: " + d->proxy_action->shortcut().toString() + ", Parent: " + parent_name);
        qDebug() << "Base action connected: " << d->active_backend_action->text() << ", Base shortcut: " << d->proxy_action->shortcut().toString() << ", Parent: " << parent_name;
        #endif
        return true;
    } else {
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        LOG_TRACE("New backend action could not be found. Action will be disabled in this context.");
        #endif
    }
    // We can hide the action here if needed
    // d->proxy_action->setVisible(false);
    d->proxy_action->setEnabled(false);
    d->is_active = false;
    return false;
}

void Qtilities::CoreGui::ProxyAction::updateFrontendAction() {
    // If this is just a place holder without any backend action we do nothing in here
    if (d->id_action_map.count() == 0)
        return;

    if (!d->active_backend_action || !d->proxy_action)
        return;

    // Update the icon
    // Only use the backend action icon if it has one:
    if (!d->active_backend_action->icon().isNull())
        d->proxy_action->setIcon(d->active_backend_action->icon());
    d->proxy_action->setIconText(d->active_backend_action->iconText());

    // Update the text
    d->proxy_action->setText(d->active_backend_action->text());
    d->proxy_action->setStatusTip(d->active_backend_action->statusTip());
    d->proxy_action->setWhatsThis(d->active_backend_action->whatsThis());

    d->proxy_action->setCheckable(d->active_backend_action->isCheckable());
    d->proxy_action->setEnabled(d->active_backend_action->isEnabled());
    d->proxy_action->setVisible(d->active_backend_action->isVisible());

    bool previous_block_value = d->proxy_action->blockSignals(true);
    d->proxy_action->setChecked(d->active_backend_action->isChecked());
    d->proxy_action->blockSignals(previous_block_value);
}

void Qtilities::CoreGui::ProxyAction::handleKeySequenceChange(const QKeySequence& old_key) {
    // Check if the old key is part of the backend actions' tooltips:
    QString old_key_tooltip = QString("<span style=\"color: gray; font-size: small\">%2</span>").arg(old_key.toString(QKeySequence::NativeText));
    QAction* backend_action;
    QList<QPointer<QAction> > actions = d->id_action_map.values();
    for (int i = 0; i < actions.count(); ++i) {
        backend_action = actions.at(i);
        if (backend_action) {
            // Update the tooltip:
            if (backend_action->toolTip().endsWith(old_key_tooltip)) {
                QString chopped_tooltip = backend_action->toolTip();
                chopped_tooltip.chop(old_key_tooltip.size());
                backend_action->setToolTip(chopped_tooltip);
            }
        }
    }

    // Check if there is no current action but a default. In that case we set the current to the default:
    if (c->current_key_sequence.isEmpty() && !c->default_key_sequence.isEmpty()) {
        c->current_key_sequence = c->default_key_sequence;
    }

    // Change the shortcut of the frontend action:
    d->proxy_action->setShortcut(c->current_key_sequence);

    // Add new key sequence to frontend action and all backend actions (backend actions only if there is a shortcut):
    QString new_key_tooltip = QString("<span style=\"color: gray; font-size: small\">%1</span>").arg(keySequence().toString(QKeySequence::NativeText));
    if (d->proxy_action->shortcut().isEmpty()) {
        if (d->active_backend_action)
            d->proxy_action->setToolTip(d->active_backend_action->toolTip());
        else
            d->proxy_action->setToolTip(d->original_tooltip);
    } else {
        if (d->active_backend_action)
            d->proxy_action->setToolTip(d->active_backend_action->toolTip().trimmed() + " " + new_key_tooltip);
        else
            d->proxy_action->setToolTip(d->original_tooltip.trimmed() + " " + new_key_tooltip);

        // Add the new tooltip to all the backend actions' tooltips:
        for (int i = 0; i < actions.count(); ++i) {
            backend_action = actions.at(i);
            if (backend_action)
                backend_action->setToolTip(backend_action->toolTip().trimmed() + " " + new_key_tooltip);
        }
    }
}

QHash<int, QPointer<QAction> > Qtilities::CoreGui::ProxyAction::contextIDActionMap() const {
    return d->id_action_map;
}

QPointer<QAction> Qtilities::CoreGui::ProxyAction::activeBackendAction() const {
    return d->active_backend_action;
}

// --------------------------------
// ShortcutCommand Implementation
// --------------------------------
struct Qtilities::CoreGui::ShortcutCommandPrivateData {
    ShortcutCommandPrivateData() : initialized(false),
    is_active(false) { }

    QShortcut*      shortcut;
    bool            initialized;
    bool            is_active;
    QString         user_text;
    QList<int>      active_contexts;
};

Qtilities::CoreGui::ShortcutCommand::ShortcutCommand(const QString& user_text, QShortcut *shortcut, const QList<int> &active_contexts, int category_context, QObject* parent) : Command(category_context, parent) {
    d = new ShortcutCommandPrivateData;
    Q_ASSERT(shortcut);

    d->user_text = user_text;
    d->shortcut = shortcut;
    d->active_contexts = active_contexts;
    d->shortcut->setEnabled(false);
}

Qtilities::CoreGui::ShortcutCommand::~ShortcutCommand() {
    delete d;
}

QAction* Qtilities::CoreGui::ShortcutCommand::action() const {
    return 0;
}

QShortcut* Qtilities::CoreGui::ShortcutCommand::shortcut() const {
    return d->shortcut;
}

QString Qtilities::CoreGui::ShortcutCommand::text() const {
    return d->user_text;
}

bool Qtilities::CoreGui::ShortcutCommand::isActive() {
    return d->is_active;
}

bool Qtilities::CoreGui::ShortcutCommand::setCurrentContext(QList<int> context_ids) {
    #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
    LOG_TRACE("Context update request on command (shortcut): " + defaultText());
    #endif

    bool must_become_active = false;

    for (int i = 0; i < d->active_contexts.size(); ++i) {
        if (context_ids.contains(d->active_contexts.at(i))) {
            must_become_active = true;

            // This break will ensure that the first context is used for the case where multiple contexts are active at once.
            break;
        }
    }

    if (must_become_active) {
        d->shortcut->setEnabled(true);
        d->is_active = true;
    } else {
        d->shortcut->setEnabled(false);
        d->is_active = false;
    }

    return true;
}

void Qtilities::CoreGui::ShortcutCommand::handleKeySequenceChange(const QKeySequence& old_key) {
    Q_UNUSED(old_key)
    d->shortcut->setKey(keySequence());
}

QList<int> Qtilities::CoreGui::ShortcutCommand::activeContexts() const {
    return d->active_contexts;
}

