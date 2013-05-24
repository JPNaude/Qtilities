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

#include "Command.h"
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Logger>
#include <QtilitiesProperty>

#include <QPointer>
#include <QTextDocument>

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
        proxy_action_backup(0),
        initialized(false),
        is_active(false) { }

    QAction* proxy_action;
    QAction* proxy_action_backup;
    bool initialized;
    bool is_active;
    QList<int> active_contexts;
    QPointer<QAction> active_backend_action;
    //! Hash with context IDs and their corresponding backend_action:
    QHash<int, QPointer<QAction> > id_action_map;
};

Qtilities::CoreGui::ProxyAction::ProxyAction(QAction* user_visible_action,
                                             int category_context,
                                             QObject* parent) : Command(category_context, parent) {
    d = new ProxyActionPrivateData;

    d->proxy_action = user_visible_action;
    if (d->proxy_action) {
        d->proxy_action->setEnabled(false);
        d->proxy_action->setParent(this);
        // Copy it to the backup proxy action which will store the original parameters of the proxy action.
        d->proxy_action_backup = new QAction(0);
        copyActionParameters(d->proxy_action,d->proxy_action_backup);
    }
}

Qtilities::CoreGui::ProxyAction::~ProxyAction() {
    if (d->proxy_action)
        delete d->proxy_action;
    if (d->proxy_action_backup)
        delete d->proxy_action_backup;
    delete d;
}

QAction* Qtilities::CoreGui::ProxyAction::action() const {
    return d->proxy_action;
}

QShortcut* Qtilities::CoreGui::ProxyAction::shortcut() const {
    return 0;
}

QString Qtilities::CoreGui::ProxyAction::text() const {
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
    if (d->active_backend_action->icon().isNull())
        d->proxy_action->setIcon(d->proxy_action_backup->icon());
    else
        d->proxy_action->setIcon(d->active_backend_action->icon());

    if (d->active_backend_action->iconText().isEmpty())
        d->proxy_action->setIconText(d->proxy_action_backup->iconText());
    else
        d->proxy_action->setIconText(d->active_backend_action->iconText());

    // Update the text
    if (d->active_backend_action->text().isEmpty())
        d->proxy_action->setText(d->proxy_action_backup->text());
    else
        d->proxy_action->setText(d->active_backend_action->text());

    if (d->active_backend_action->statusTip().isEmpty())
        d->proxy_action->setStatusTip(d->proxy_action_backup->statusTip());
    else
        d->proxy_action->setStatusTip(d->active_backend_action->statusTip());

    if (d->active_backend_action->whatsThis().isEmpty())
        d->proxy_action->setWhatsThis(d->proxy_action_backup->whatsThis());
    else
        d->proxy_action->setWhatsThis(d->active_backend_action->whatsThis());

    d->proxy_action->setCheckable(d->active_backend_action->isCheckable());
    d->proxy_action->setEnabled(d->active_backend_action->isEnabled());
    d->proxy_action->setVisible(d->active_backend_action->isVisible());

    bool previous_block_value = d->proxy_action->blockSignals(true);
    d->proxy_action->setChecked(d->active_backend_action->isChecked());
    d->proxy_action->blockSignals(previous_block_value);
}

void Qtilities::CoreGui::ProxyAction::copyActionParameters(QAction *source_action, QAction *target_action) {
    if (!source_action || !target_action)
        return;

    target_action->setIcon(source_action->icon());
    target_action->setIconText(source_action->iconText());

    // Update the text
    target_action->setText(source_action->text());
    target_action->setStatusTip(source_action->statusTip());
    target_action->setWhatsThis(source_action->whatsThis());

    target_action->setCheckable(source_action->isCheckable());
    target_action->setEnabled(source_action->isEnabled());
    target_action->setVisible(source_action->isVisible());

    bool previous_block_value = target_action->blockSignals(true);
    target_action->setChecked(source_action->isChecked());
    target_action->blockSignals(previous_block_value);
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

    // Check if there is no current key sequence but a default. In that case we set the current to the default:
    if (c->current_key_sequence.isEmpty() && !c->default_key_sequence.isEmpty()) {
        c->current_key_sequence = c->default_key_sequence;
    }

    // Change the shortcut of the frontend action:
    d->proxy_action->setShortcut(c->current_key_sequence);

    // Add new key sequence to frontend action and all backend actions (backend actions only if there is a shortcut):
    QString key_sequence_string = keySequence().toString(QKeySequence::NativeText);
    #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString new_key_tooltip = QString("<span style=\"color: gray; font-size: small\">%1</span>").arg(Qt::escape(key_sequence_string));
    #else
    QString new_key_tooltip = QString("<span style=\"color: gray; font-size: small\">%1</span>").arg(key_sequence_string.toHtmlEscaped());
    #endif
    if (d->proxy_action->shortcut().isEmpty()) {
        if (d->active_backend_action)
            d->proxy_action->setToolTip(d->active_backend_action->toolTip());
        else
            d->proxy_action->setToolTip(d->proxy_action_backup->toolTip());
    } else {
        if (d->active_backend_action)
            d->proxy_action->setToolTip(d->active_backend_action->toolTip().trimmed() + " " + new_key_tooltip);
        else
            d->proxy_action->setToolTip(d->proxy_action_backup->toolTip().trimmed() + " " + new_key_tooltip);

        // Add the new tooltip to all the backend actions' tooltips:
        for (int i = 0; i < actions.count(); ++i) {
            backend_action = actions.at(i);
            if (backend_action)
                backend_action->setToolTip(backend_action->toolTip().trimmed() + " " + new_key_tooltip);
        }
    }
}

void Qtilities::CoreGui::ProxyAction::unregisterContext(int context_id) {
    if (d->id_action_map.contains(context_id)) {
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        LOG_TRACE("Context backend removed on command (action): " + defaultText());
        #endif
        d->id_action_map.remove(context_id);

        if (d->active_contexts.contains(context_id)) {
            d->active_contexts.removeOne(context_id);
            setCurrentContext(d->active_contexts); // Needed to update d->active_backend_action
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

Qtilities::CoreGui::ShortcutCommand::ShortcutCommand(const QString& user_text,
                                                     QShortcut *shortcut,
                                                     const QList<int> &active_contexts,
                                                     int category_context,
                                                     QObject* parent) : Command(category_context, parent) {
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

void Qtilities::CoreGui::ShortcutCommand::unregisterContext(int context_id) {
    if (d->active_contexts.contains(context_id)) {
        #if defined(QTILITIES_VERBOSE_ACTION_DEBUGGING)
        LOG_TRACE("Context backend removed on command (shortcut): " + defaultText());
        #endif

        d->active_contexts.removeOne(context_id);
        setCurrentContext(d->active_contexts); // Needed to update d->is_active
    }
}

QList<int> Qtilities::CoreGui::ShortcutCommand::activeContexts() const {
    return d->active_contexts;
}

