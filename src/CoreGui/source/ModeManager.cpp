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

#include "ModeManager.h"
#include "IMode.h"
#include "ModeListWidget.h"
#include "QtilitiesApplication.h"

#include <QListWidgetItem>
#include <QShortcut>

#include <QtilitiesCore>
using namespace QtilitiesCore;
using namespace Qtilities::CoreGui::Interfaces;

struct Qtilities::CoreGui::ModeManagerPrivateData {
    ModeManagerPrivateData() : mode_list_widget(0),
        active_mode(-1),
        previous_active_modes_populate(true),
        mode_id_counter(1000),
        register_shortcuts(true),
        actionSwitchToPreviousMode(0) {}

    // All int values in here reffers to the modeID()s of the modes.
    ModeListWidget*             mode_list_widget;
    int                         active_mode;
    QStack<int>                 previous_active_modes;
    bool                        previous_active_modes_populate; // Indicates if mode change must be added to previous_active_modes
    QMap<int, IMode*>           id_iface_map;
    QList<int>                  mode_order;
    Qt::Orientation             orientation;
    QList<int>                  disabled_modes;
    int                         mode_id_counter;
    int                         manager_id;
    //! ModeID & Shortcut pair
    QMap<int, QShortcut*>       mode_shortcuts;
    QMap<QShortcut*,Command*>   command_shortcut_map;
    bool                        register_shortcuts;
    QPointer<QAction>           actionSwitchToPreviousMode;
};

Qtilities::CoreGui::ModeManager::ModeManager(int manager_id, Qt::Orientation orientation, QObject *parent) :
    QObject(parent)
{
    d = new ModeManagerPrivateData;
    d->orientation = orientation;
    d->manager_id = manager_id;

    // Setup the mode list widget in the way we need it:
    d->mode_list_widget = new ModeListWidget(orientation);
    connect(d->mode_list_widget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(handleModeListCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));

    if (d->orientation == Qt::Horizontal) {
        d->mode_list_widget->setFlow(QListWidget::TopToBottom);
        d->mode_list_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        d->mode_list_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    } else {
        d->mode_list_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        d->mode_list_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        d->mode_list_widget->setFlow(QListWidget::LeftToRight);
    }
    d->mode_list_widget->setMovement(QListView::Static);
    d->mode_list_widget->setViewMode(QListView::IconMode);
    d->mode_list_widget->setResizeMode(QListView::Adjust);
    d->mode_list_widget->setIconSize(QSize(48,48));
    d->mode_list_widget->setWrapping(true);
    d->mode_list_widget->setUniformItemSizes(true);

    d->mode_list_widget->setSelectionRectVisible(true);
    // No border around the selected item's text:
    d->mode_list_widget->setFocusPolicy(Qt::NoFocus);

    QString stylesheet = "";
    // Give the view a colored background:
    stylesheet += "QListView { background-color: #FFFFFF; border-style: none; }";
    // The text underneath the unselected items:
    stylesheet += "QListView::item::text { font-weight: bold; border-style: none; color: black }";
    // The text underneath the selected item:
    stylesheet += "QListView::item:selected:active { font-weight: bold; border-style: none; color: white }";
    stylesheet += "QListView::item:selected:!active { font-weight: bold; border-style: none; color: white }";
    // Hover effect:
    stylesheet += "QListView::item:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 white, stop: 0.4 #EEEEEE, stop:1 #CCCCCC); }";
    // Selected item gradient:
    stylesheet += "QListView::item:selected:active { background: #CCCCCC; }";
    stylesheet += "QListView::item:selected:!active { background: #CCCCCC; }";
    // The padding of items in the list:
    if (d->orientation == Qt::Horizontal)
        stylesheet += "QListView::item { padding: 5px 1px 5px 1px;}";
    else
        stylesheet += "QListView::item { padding: 5px 0px 5px 0px;}";

    d->mode_list_widget->setStyleSheet(stylesheet);
}

Qtilities::CoreGui::ModeManager::~ModeManager() {
    if (d->actionSwitchToPreviousMode)
        delete d->actionSwitchToPreviousMode;
    delete d;
}

void CoreGui::ModeManager::setMinimumItemSize(QSize size) {
    d->mode_list_widget->setMinimumItemSize(size);
}

void CoreGui::ModeManager::setMaximumItemSize(QSize size) {
    d->mode_list_widget->setMaximumItemSize(size);
}

QSize CoreGui::ModeManager::minimumItemSize() const {
    return d->mode_list_widget->minimumItemSize();
}

QSize CoreGui::ModeManager::maximumItemSize() const {
    return d->mode_list_widget->maximumItemSize();
}

QListWidget* Qtilities::CoreGui::ModeManager::modeListWidget() {
    return d->mode_list_widget;
}

void CoreGui::ModeManager::refreshModeList() {
    // Disconnect from all existing modes in the list:
    QMapIterator<int, IMode*> i(d->id_iface_map);
    while (i.hasNext()) {
        i.next();
        if (i.value())
            i.value()->objectBase()->disconnect(this);
    }

    d->id_iface_map.clear();
    QList<QObject*> modes = OBJECT_MANAGER->registeredInterfaces("com.Qtilities.CoreGui.IMode/1.0");
    LOG_DEBUG(QString("Mode manager \"%1\" found %2 mode(s) during initialization.").arg(objectName()).arg(modes.count()));
    addModes(modes,true,false);
}

void Qtilities::CoreGui::ModeManager::addMode(IMode* mode, bool initialize_mode, bool refresh_list) {
    if (mode) {
        // Check if this manager's ID is part of the target manager list of the mode:
        if (!mode->targetManagerIDs().contains(d->manager_id))
            return;

        // Check if it has a valid widget, otherwise we don't add the mode.
        if (!mode->modeWidget())
            return;

        if (initialize_mode)
            mode->initializeMode();

        // Set the object name of the mode:
        mode->objectBase()->setObjectName(mode->modeName());

        if (!mode->contextString().isEmpty())
            CONTEXT_MANAGER->registerContext(mode->contextString());

        if (!containsModeId(mode->modeID())) {
            // Check if the mode needs to be assigned a mode ID:
            if (mode->modeID() == -1) {
                while (containsModeId(d->mode_id_counter)) {
                    ++d->mode_id_counter;
                }
                mode->setModeID(d->mode_id_counter);
                LOG_DEBUG(QString("Mode Manager: Auto-assigning mode ID %1 found for mode \"%2\".").arg(d->mode_id_counter).arg(mode->modeName()));
                ++d->mode_id_counter;
            }

            // Connect to the modeIconChanged() signal:
            if (mode->objectBase()) {
                int signal_index = mode->objectBase()->metaObject()->indexOfSignal("modeIconChanged()");
                if (signal_index != -1)
                    connect(mode->objectBase(),SIGNAL(modeIconChanged()),SLOT(handleModeIconChanged()));
            }

            // Add the mode to our ID IFace map:
            d->id_iface_map[mode->modeID()] = mode;

            if (refresh_list)
                refreshList();
        } else {
            LOG_ERROR(QString(tr("Mode Manager: Duplicate mode IDs found for mode \"%1\" with ID %2. Mode \"%3\" already uses this ID.")).arg(mode->modeName()).arg(mode->modeID()).arg(d->id_iface_map[mode->modeID()]->modeName()));
        }
    }
}

void Qtilities::CoreGui::ModeManager::addModes(QList<IMode*> modes, bool initialize_modes, bool refresh_list) {
    for (int i = 0; i < modes.count(); ++i)
        addMode(modes.at(i), initialize_modes,false);

    if (refresh_list)
        refreshList();
}

void Qtilities::CoreGui::ModeManager::initialize() {
    if (d->id_iface_map.isEmpty())
        refreshModeList();

    refreshList();
}

void Qtilities::CoreGui::ModeManager::addModes(QList<QObject*> modes, bool initialize_modes, bool refresh_list) {
    for (int i = 0; i < modes.count(); ++i) {
        IMode* mode = qobject_cast<IMode*> (modes.at(i));
        if (mode)
            addMode(mode, initialize_modes,false);
    }

    if (refresh_list)
        refreshList();
}

QList<Qtilities::CoreGui::Interfaces::IMode*> Qtilities::CoreGui::ModeManager::modes() const {
    return d->id_iface_map.values();
}

QString Qtilities::CoreGui::ModeManager::modeShortcut(int mode_id) const {
    if (d->mode_shortcuts.contains(mode_id))
        return d->mode_shortcuts[mode_id]->key().toString();
    else
        return QString();
}

int Qtilities::CoreGui::ModeManager::managerID() const {
    return d->manager_id;
}

void Qtilities::CoreGui::ModeManager::setManagerID(int manager_id) {
    d->manager_id = manager_id;
}

Qtilities::CoreGui::Interfaces::IMode* Qtilities::CoreGui::ModeManager::activeModeIFace() const {
    if (d->id_iface_map.contains(d->active_mode))
        return d->id_iface_map[d->active_mode];
    else
        return 0;
}

int Qtilities::CoreGui::ModeManager::activeModeID() const {
    if (d->id_iface_map.contains(d->active_mode))
        return d->id_iface_map[d->active_mode]->modeID();
    else
        return -1;
}

QString Qtilities::CoreGui::ModeManager::activeModeName() const {
    if (d->id_iface_map.contains(d->active_mode))
        return d->id_iface_map[d->active_mode]->modeName();
    else
        return QString();
}

void CoreGui::ModeManager::handleModeIconChanged() {
    refreshList();
}

void Qtilities::CoreGui::ModeManager::setPreferredModeOrder(const QStringList& preferred_order, bool refresh_list) {
    d->mode_order = modeNamesToIDs(preferred_order);

    if (refresh_list)
        refreshList();
}

void Qtilities::CoreGui::ModeManager::setPreferredModeOrder(QList<int> preferred_order, bool refresh_list) {
    d->mode_order = preferred_order;

    if (refresh_list)
        refreshList();
}

void Qtilities::CoreGui::ModeManager::setPreferredModeOrder(QList<IMode*> preferred_order, bool refresh_list) {
    d->mode_order = modeIFacesToIDs(preferred_order);

    if (refresh_list)
        refreshList();
}

QStringList Qtilities::CoreGui::ModeManager::preferredModeOrderNames() const {
    return modeIDsToNames(d->mode_order);
}

QList<Qtilities::CoreGui::Interfaces::IMode*> Qtilities::CoreGui::ModeManager::preferredModeOrderIFaces() const {
    return modeIDsToIFaces(d->mode_order);
}

QList<int> Qtilities::CoreGui::ModeManager::preferredModeOrderIDs() const {
    return d->mode_order;
}

void Qtilities::CoreGui::ModeManager::setDisabledModes(const QStringList& disabled_modes) {
    d->disabled_modes = modeNamesToIDs(disabled_modes);
    refreshList();
}

void Qtilities::CoreGui::ModeManager::setDisabledModes(QList<int> disabled_modes) {
    d->disabled_modes = disabled_modes;
}

void Qtilities::CoreGui::ModeManager::setDisabledModes(QList<IMode*> disabled_modes) {
    d->disabled_modes = modeIFacesToIDs(disabled_modes);
    refreshList();
}

QStringList Qtilities::CoreGui::ModeManager::disabledModeNames() const {
    return modeIDsToNames(d->disabled_modes);
}

QList<IMode*> Qtilities::CoreGui::ModeManager::disabledModeIFaces() const {
    return modeIDsToIFaces(d->disabled_modes);
}

QList<int> Qtilities::CoreGui::ModeManager::disabledModeIDs() const {
    return d->disabled_modes;
}

void Qtilities::CoreGui::ModeManager::refreshList() {
    if (d->id_iface_map.isEmpty()) {
        emit changeCentralWidget(0);
        emit modeListItemSizesChanged();
        d->mode_list_widget->clear();
        return;
    }

    // Note: This function should not modify d->id_iface_map. It should just display its contents according
    // to the preferred order etc.

    // Clear all lists:
    disconnect(d->mode_list_widget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(handleModeListCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));
    d->mode_list_widget->clear();
    connect(d->mode_list_widget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(handleModeListCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)),Qt::UniqueConnection);

    // Will indicate if an item has been set as active in the loops below:
    bool set_active_done = false;

    // Now add them in the correct order:
    QList<int> added_ids;
    QList<QString> added_names;

    QMap<int,QListWidgetItem*> added_items;
    foreach(int id, d->mode_order) {
        // Get the item with the mode id:
        IMode* mode = 0;

        if(d->id_iface_map.contains(id))
            mode = d->id_iface_map[id];

        if (mode) {
            QListWidgetItem* new_item = new QListWidgetItem(mode->modeIcon(),mode->modeName(),d->mode_list_widget,mode->modeID());
            if (d->disabled_modes.contains(mode->modeID())) {
                Qt::ItemFlags flags = new_item->flags();
                flags &= ~Qt::ItemIsEnabled;
                new_item->setFlags(flags);
            }
            new_item->setSizeHint(d->mode_list_widget->sizeHint());
            new_item->setToolTip(QString(tr("Go to <b>%1</b> ")).arg(mode->modeName()));

            d->mode_list_widget->addItem(new_item);

            // Check if this mode is the active mode:
            if (d->active_mode != -1) {
                if (mode->modeID() == d->active_mode && !d->disabled_modes.contains(mode->modeID())) {
                    d->mode_list_widget->setCurrentItem(new_item);
                    set_active_done = true;
                }
            }

            // Set the mode icon as the object decoration for mode:
            if (mode->objectBase()) {
                SharedProperty icon_property(qti_prop_DECORATION,mode->modeIcon());
                ObjectManager::setSharedProperty(mode->objectBase(),icon_property);
            }
            added_ids << id;
            added_names << mode->modeName();
            added_items[id] = new_item;
        }
    }

    // Now add all items that was not specified in the order list:
    foreach(IMode* mode, d->id_iface_map.values()) {
        if (mode) {
            if (!added_ids.contains(mode->modeID())) {
                QListWidgetItem* new_item = new QListWidgetItem(mode->modeIcon(),mode->modeName(),d->mode_list_widget,mode->modeID());
                if (d->disabled_modes.contains(mode->modeID())) {
                    Qt::ItemFlags flags = new_item->flags();
                    flags &= ~Qt::ItemIsEnabled;
                    new_item->setFlags(flags);
                }
                new_item->setToolTip(QString(tr("Go to <b>%1</b> ")).arg(mode->modeName()));
                new_item->setSizeHint(d->mode_list_widget->sizeHint());

                d->mode_list_widget->addItem(new_item);

                // Check if this mode is the active mode:
                if (d->active_mode != -1) {
                    if (mode->modeID() == d->active_mode && !d->disabled_modes.contains(mode->modeID())) {
                        d->mode_list_widget->setCurrentItem(new_item);
                        set_active_done = true;
                    }
                }

                // Set the mode icon as the object decoration for mode:
                if (mode->objectBase()) {
                    SharedProperty icon_property(qti_prop_DECORATION,mode->modeIcon());
                    ObjectManager::setSharedProperty(mode->objectBase(),icon_property);
                }

                added_ids << mode->modeID();
                added_names << mode->modeName();
                added_items[mode->modeID()] = new_item;
            }
        }
    }

    // Assign shortcuts for modes in this window:
    // Delete all shortcut commands:
//    foreach (Command* command, d->command_shortcut_map.values())
//        delete command;

//    d->mode_shortcuts.clear();
//    d->command_shortcut_map.clear();

    // Now assign correct shortcuts for all modes:
    if (d->register_shortcuts) {
        int id_num = 0;
        for (int i = 0; i < added_ids.count(); ++i) {
            QString shortcut_id = QString("%1.Mode.%2").arg(objectName()).arg(added_ids.at(i));
            Command* command = ACTION_MANAGER->command(shortcut_id);
            if (!command) {
                // Determine a valid shortcut that we can use:
                ++id_num;
                QKeySequence key_sequence = QKeySequence(QString("Ctrl+%1").arg(id_num));
                while (ACTION_MANAGER->commandsWithKeySequence(key_sequence).count() > 0) {
                    ++id_num;
                    key_sequence = QKeySequence(QString("Ctrl+%1").arg(id_num));
                }

                ACTION_MANAGER->commandObserver()->startProcessingCycle();

                // Create the shortcut
                QShortcut* shortcut = new QShortcut(key_sequence,QtilitiesApplication::mainWindow());
                d->mode_shortcuts[added_ids.at(i)] = shortcut;
                shortcut->setContext(Qt::ApplicationShortcut);

                // Register the command:
                command = ACTION_MANAGER->registerShortcut(shortcut_id,added_names.at(i),shortcut);
                command->setCategory(QtilitiesCategory("Application Modes"));
                command->setKeySequence(key_sequence);
                d->command_shortcut_map[shortcut] = command;
                connect(shortcut,SIGNAL(activated()),SLOT(handleModeShortcutActivated()));

                ACTION_MANAGER->commandObserver()->endProcessingCycle();
            }

            // We need to set the tootip on the QListWidgetItem every time that refresh is called since the list widget
            // items are constructed everytime:
            QKeySequence sequence = command->keySequence();
            QString new_key_tooltip = QString("<span style=\"color: gray; font-size: small\">%1</span>").arg(sequence.toString());

            QListWidgetItem* item = added_items[added_ids.at(i)];
            if (item) {
                QString tooltip = QString(item->toolTip() + new_key_tooltip);
                item->setToolTip(tooltip);
            }
        }
    }

    // If no modes has been set active yet, we set a mode to be active here:
    // We take the first mode that does not appear in the list of disabled modes.
    if (!set_active_done) {
        foreach(int id, d->id_iface_map.keys()) {
            if (!d->disabled_modes.contains(id)) {
                d->mode_list_widget->setCurrentItem(added_items[id]);
                set_active_done = true;
                break;
            }
        }
    }

    // If we still don't have an active item it means that all modes are disabled. In this case
    // set the first mode as active.
    if (!set_active_done && (added_items.count() > 0)) {
        d->mode_list_widget->setCurrentItem(added_items.values().at(0));
        set_active_done = true;
    }

    // If we still don't have an active item it means no modes are present. We don't have to do anything in that case.
    if (!set_active_done || d->mode_list_widget->count() == 0) {
        emit changeCentralWidget(0);
        emit modeListItemSizesChanged();
    } else {
        QSize hint = d->mode_list_widget->sizeHint();
        // Set the min & max widths and heigths:
        if (d->orientation == Qt::Horizontal) {
            d->mode_list_widget->setMinimumWidth(hint.width());
            d->mode_list_widget->setMaximumWidth(hint.width());
            emit modeListItemSizesChanged();
        } else {
            d->mode_list_widget->setMinimumHeight(hint.height());
            d->mode_list_widget->setMaximumHeight(hint.height());
            emit modeListItemSizesChanged();
        }

        // Set size hint for all items:
        QList<QListWidgetItem*> added_item_values = added_items.values();
        for (int i = 0; i < added_item_values.count(); ++i) {
            if (added_item_values.at(i))
                added_item_values.at(i)->setSizeHint(hint);
        }
    }

    if ((d->mode_list_widget->count() > 0) && (d->active_mode == -1))
        qWarning() << "Failed to make selection in mode manager" << objectName();
}

QList<int> Qtilities::CoreGui::ModeManager::modeNamesToIDs(QStringList name_list) const {
    QList<int> int_list;

    name_list.removeDuplicates();
    foreach (const QString& name, name_list) {
        foreach (IMode* mode, d->id_iface_map.values()) {
            // Get the IMode interface corresponding to the name:
            if (mode->modeName() == name) {
                int_list << mode->modeID();
                break;
            }
        }
    }

    return int_list;
}

QList<int> Qtilities::CoreGui::ModeManager::modeIFacesToIDs(QList<IMode*> imode_list_widget) const {
    QList<int> id_list;

    foreach(IMode* mode, imode_list_widget) {
        if (!id_list.contains(mode->modeID()))
            id_list << mode->modeID();
    }

    return id_list;
}

QStringList Qtilities::CoreGui::ModeManager::modeIDsToNames(QList<int> mode_ids) const {
    QStringList name_list;

    foreach(int id, mode_ids) {
        if (d->id_iface_map.contains(id)) {
            if (d->id_iface_map[id])
                name_list << d->id_iface_map[id]->modeName();
        }
    }

    return name_list;
}

QList<Qtilities::CoreGui::Interfaces::IMode*> Qtilities::CoreGui::ModeManager::modeIDsToIFaces(QList<int> mode_ids) const {
    QList<IMode*> iface_list;

    foreach(int id, mode_ids) {
        if (d->id_iface_map.contains(id))
            iface_list << d->id_iface_map[id];
    }

    return iface_list;
}

QListWidgetItem* Qtilities::CoreGui::ModeManager::listWidgetItemForID(int id) const {
    for (int i = 0; d->mode_list_widget->count(); ++i) {
        QListWidgetItem* current = d->mode_list_widget->item(i);
        if (current->type() == id)
            return current;
    }

    return 0;
}

void CoreGui::ModeManager::updateSwitchToPreviousModeAction() {
    if (d->actionSwitchToPreviousMode) {
        d->actionSwitchToPreviousMode->setEnabled(d->previous_active_modes.count() > 0);
        bool restore_default_name = true;
        if (d->previous_active_modes.count() > 0) {
            int last_id = d->previous_active_modes.front();
            if (d->id_iface_map.contains(last_id)) {
                IMode* mode = d->id_iface_map[last_id];
                if (mode) {
                     d->actionSwitchToPreviousMode->setText(QString(tr("Switch To Previous Mode (%1)")).arg(mode->modeName()));
                     restore_default_name = false;
                }
            }
        }
        if (restore_default_name)
            d->actionSwitchToPreviousMode->setText(tr("Switch To Previous Mode"));
    }
}

bool Qtilities::CoreGui::ModeManager::containsModeId(int mode_id) const {
    return d->id_iface_map.contains(mode_id);
}

bool CoreGui::ModeManager::registerModeShortcuts() const {
    return d->register_shortcuts;
}

void CoreGui::ModeManager::setRegisterModeShortcuts(bool register_shortcuts) {
    d->register_shortcuts = register_shortcuts;
}

QStack<int> CoreGui::ModeManager::previousModeIDs() const {
    return d->previous_active_modes;
}

QAction *CoreGui::ModeManager::switchToPreviousModeAction() {
    if (!d->actionSwitchToPreviousMode) {
        d->actionSwitchToPreviousMode = new QAction(tr("Switch To Previous Mode"),0);
        connect(d->actionSwitchToPreviousMode,SIGNAL(triggered()),SLOT(switchToPreviousMode()));
        updateSwitchToPreviousModeAction();
    }
    return d->actionSwitchToPreviousMode;
}

void Qtilities::CoreGui::ModeManager::handleModeListCurrentItemChanged(QListWidgetItem * new_item, QListWidgetItem *old_item) {
    int old_id = -1;
    if (old_item)
        old_id = old_item->type();

    // Update previous modes storage:
    if (d->previous_active_modes_populate) {
        if (old_id != -1) {
            if (d->previous_active_modes.isEmpty()) {
                d->previous_active_modes.push_front(old_id);
                updateSwitchToPreviousModeAction();
            } else {
                if (d->previous_active_modes.front() != old_id) {
                    d->previous_active_modes.push_front(old_id);
                    updateSwitchToPreviousModeAction();
                }
            }
        }
    }

    int new_id = -1;
    if (new_item)
        new_id = new_item->type();

    if (!new_item) {
        if (d->mode_list_widget->count()) {
            d->mode_list_widget->setCurrentRow(0);
            emit activeModeChanged(new_id,old_id);
            return;
        } else {
            emit changeCentralWidget(0);
            d->active_mode = -1;
            emit activeModeChanged(new_id,old_id);
            return;
        }
    }

    if (!d->id_iface_map.contains(new_item->type())) {
        emit changeCentralWidget(0);
        d->active_mode = -1;
        emit activeModeChanged(new_id,old_id);
        return;
    }

    if (d->id_iface_map.contains(new_item->type())) {
        d->id_iface_map[new_item->type()]->aboutToBeActivated();
        if (!d->id_iface_map[new_item->type()]->contextString().isEmpty())
            CONTEXT_MANAGER->setNewContext(d->id_iface_map[new_item->type()]->contextString());
        emit changeCentralWidget(d->id_iface_map[new_item->type()]->modeWidget());
        d->id_iface_map[new_item->type()]->justActivated();
    }

    d->active_mode = new_item->type();
    emit activeModeChanged(new_id,old_id);
}

void Qtilities::CoreGui::ModeManager::handleModeShortcutActivated() {
    QShortcut* shortcut = qobject_cast<QShortcut*> (sender());
    if (shortcut) {
        QList<QShortcut*> values = d->mode_shortcuts.values();
        int count = values.count();
        for (int i = 0; i < count; ++i) {
            if (values.at(i) == shortcut) {
                setActiveMode(d->mode_shortcuts.keys().at(i));
                break;
            }
        }
    }
}

bool CoreGui::ModeManager::switchToPreviousMode() {
    if (d->previous_active_modes.count() > 0) {
        // Check if the previous mode is still available:
        if (d->id_iface_map.contains(d->previous_active_modes.front())) {
            d->previous_active_modes_populate = false;
            setActiveMode(d->previous_active_modes.front());
            d->previous_active_modes_populate = true;
            d->previous_active_modes.pop_front();
            updateSwitchToPreviousModeAction();
            return true;
        }
    }

    return false;
}

void Qtilities::CoreGui::ModeManager::setActiveMode(int mode_id, bool refresh_list) {
    if (d->disabled_modes.contains(mode_id))
        return;

    d->active_mode = mode_id;

    if (refresh_list) {
        refreshList();
    } else {
        if (containsModeId(d->active_mode))
            d->mode_list_widget->setCurrentItem(listWidgetItemForID(d->active_mode));
    }
}

void Qtilities::CoreGui::ModeManager::setActiveMode(const QString& mode_name, bool refresh_list) {
    // Go through all the registered mode and try to match each mode with new_mode.
    QList<IMode*> values = d->id_iface_map.values();
    for (int i = 0; i < d->id_iface_map.count(); ++i) {
        if (values.at(i)->modeName() == mode_name) {
            if (d->disabled_modes.contains(values.at(i)->modeID()))
                return;

            d->active_mode = values.at(i)->modeID();
            break;
        }
    }

    if (refresh_list) {
        refreshList();
    } else {
        if (containsModeId(d->active_mode))
            d->mode_list_widget->setCurrentItem(listWidgetItemForID(d->active_mode));
    }
}

void Qtilities::CoreGui::ModeManager::setActiveMode(IMode* mode_iface, bool refresh_list) {
    // Go through all the registered mode and try to match each mode with new_mode.
    QList<IMode*> values = d->id_iface_map.values();
    for (int i = 0; i < d->id_iface_map.count(); ++i) {
        if (values.at(i) == mode_iface) {
            if (d->disabled_modes.contains(values.at(i)->modeID()))
                return;

            d->active_mode = values.at(i)->modeID();
            break;
        }
    }

    if (refresh_list) {
        refreshList();
    } else {
        if (containsModeId(d->active_mode))
            d->mode_list_widget->setCurrentItem(listWidgetItemForID(d->active_mode));
    }
}
