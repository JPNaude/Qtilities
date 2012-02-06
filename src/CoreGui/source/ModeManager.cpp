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
        mode_id_counter(1000) {}

    // All int values in here reffers to the modeID()s of the modes.
    ModeListWidget*         mode_list_widget;
    int                     active_mode;
    QMap<int, IMode*>       id_iface_map;
    QList<int>              mode_order;
    Qt::Orientation         orientation;
    QList<int>              disabled_modes;
    int                     mode_id_counter;
    int                     manager_id;
    //! ModeID & Shortcut pair
    QMap<int, QShortcut*>   mode_shortcuts;
};

Qtilities::CoreGui::ModeManager::ModeManager(int manager_id, Qt::Orientation orientation, QObject *parent) :
    QObject(parent)
{
    d = new ModeManagerPrivateData;
    d->orientation = orientation;
    d->manager_id = manager_id;

    // Setup the mode list widget in the way we need it:
    d->mode_list_widget = new ModeListWidget(orientation);
    connect(d->mode_list_widget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(handleModeListCurrentItemChanged(QListWidgetItem*)));

    if (d->orientation == Qt::Horizontal)
        d->mode_list_widget->setFlow(QListWidget::TopToBottom);
    else
        d->mode_list_widget->setFlow(QListWidget::LeftToRight);
    d->mode_list_widget->setMovement(QListView::Static);
    d->mode_list_widget->setViewMode(QListView::IconMode);
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

        if (isValidModeID(mode->modeID())) {
            // Check if the mode needs to be assigned a mode ID:
            if (mode->modeID() == -1) {
                while (!isValidModeID(d->mode_id_counter)) {
                    ++d->mode_id_counter;
                }
                mode->setModeID(d->mode_id_counter);
                LOG_DEBUG(QString("Mode Manager: Auto-assigning mode ID %1 found for mode \"%2\".").arg(d->mode_id_counter).arg(mode->modeName()));
                ++d->mode_id_counter;
            }

            // Add the mode to our ID IFace map:
            d->id_iface_map[mode->modeID()] = mode;

            // Create a shortcut for the mode:
            QString shortcut_id = QString("ApplicationMode.%1").arg(mode->modeID());
            Command* command = ACTION_MANAGER->command(shortcut_id);
            if (!command) {
                QShortcut* shortcut = new QShortcut(QKeySequence(QString("Ctrl+%1").arg(d->mode_shortcuts.count()+1)),QtilitiesApplication::mainWindow());
                d->mode_shortcuts[mode->modeID()] = shortcut;
                shortcut->setContext(Qt::ApplicationShortcut);
                Command* command = ACTION_MANAGER->registerShortcut(QString("ApplicationMode.%1").arg(mode->modeID()),mode->modeName(),shortcut);
                command->setCategory(QtilitiesCategory("Application Modes"));
                connect(shortcut,SIGNAL(activated()),SLOT(handleModeShortcutActivated()));
            }

            // Refresh the list:
            if (refresh_list)
                refreshList();
        } else {
            LOG_ERROR(QString(tr("Mode Manager: Duplicate mode IDs found for mode \"%1\" with ID %2. Mode \"%3\" already uses this ID.")).arg(mode->modeName()).arg(mode->modeID()).arg(d->id_iface_map[mode->modeID()]->modeName()));
        }
    }
}

void Qtilities::CoreGui::ModeManager::addModes(QList<IMode*> modes, bool initialize_modes) {
    for (int i = 0; i < modes.count(); i++) {
        addMode(modes.at(i), initialize_modes, false);
    }

    refreshList();
}

void Qtilities::CoreGui::ModeManager::initialize() {
    d->id_iface_map.clear();
    d->active_mode = -1;

    QList<QObject*> modes = OBJECT_MANAGER->registeredInterfaces("IMode");
    LOG_DEBUG(QString("%1 mode(s) found.").arg(modes.count()));
    addModes(modes);
}

void Qtilities::CoreGui::ModeManager::addModes(QList<QObject*> modes, bool initialize_modes) {
    for (int i = 0; i < modes.count(); i++) {
        IMode* mode = qobject_cast<IMode*> (modes.at(i));
        if (mode)
            addMode(mode, initialize_modes, false);
    }

    refreshList();
}

QList<Qtilities::CoreGui::Interfaces::IMode*> Qtilities::CoreGui::ModeManager::modes() const {
    return d->id_iface_map.values();
}

QString Qtilities::CoreGui::ModeManager::modeShortcut(int mode_id) const {
    if (d->mode_shortcuts.keys().contains(mode_id))
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
    return d->id_iface_map[d->active_mode];
}

int Qtilities::CoreGui::ModeManager::activeModeID() const {
    return d->id_iface_map[d->active_mode]->modeID();
}

QString Qtilities::CoreGui::ModeManager::activeModeName() const {
    return d->id_iface_map[d->active_mode]->modeName();
}

void Qtilities::CoreGui::ModeManager::setPreferredModeOrder(const QStringList& preferred_order) {
    d->mode_order = modeNamesToIDs(preferred_order);  
    refreshList();
}

void Qtilities::CoreGui::ModeManager::setPreferredModeOrder(QList<int> preferred_order) {
    d->mode_order = preferred_order;
    refreshList();
}

void Qtilities::CoreGui::ModeManager::setPreferredModeOrder(QList<IMode*> preferred_order) {
    d->mode_order = modeIFacesToIDs(preferred_order);
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
    // Note: This function should not modify d->id_iface_map. It should just display its contents according
    // to the preferred order etc.

    // Clear all lists:
    d->mode_list_widget->clear();

    // Will indicate if an item has been set as active in the loops below:
    bool set_active_done = false;

    // Now add them in the correct order:
    QList<int> added_ids;
    QMap<int, QListWidgetItem*> added_items;
    foreach(int id, d->mode_order) {
        // Get the item with the mode id:
        IMode* mode = d->id_iface_map[id];

        if (mode) {
            QListWidgetItem* new_item = new QListWidgetItem(mode->modeIcon(),mode->modeName(),d->mode_list_widget,mode->modeID());
            if (d->disabled_modes.contains(mode->modeID())) {
                Qt::ItemFlags flags = new_item->flags();
                flags &= ~Qt::ItemIsEnabled;
                new_item->setFlags(flags);
            }
            new_item->setToolTip(QString(tr("Go to <b>%1</b> mode ")).arg(mode->modeName()));

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
                new_item->setToolTip(QString(tr("Go to <b>%1</b> mode ")).arg(mode->modeName()));

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
                added_items[mode->modeID()] = new_item;
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
    if (!set_active_done) {       
        emit changeCentralWidget(0);
        emit modeListItemSizesChanged();
    } else {
        // Now assign correct shortcuts for all modes:
        for (int i = 0; i < added_ids.count(); i++) {
            QString shortcut_id = QString("ApplicationMode.%1").arg(added_ids.at(i));
            Command* command = ACTION_MANAGER->command(shortcut_id);
            if (command) {
                command->setKeySequence(QKeySequence(QString("Ctrl+%1").arg(i+1)));
                QString new_key_tooltip = QString("<span style=\"color: gray; font-size: small\">Ctrl+%1</span>").arg(i+1);
                QString tooltip = QString(added_items[added_ids.at(i)]->toolTip() + new_key_tooltip);
                added_items[added_ids.at(i)]->setToolTip(tooltip);
            }
        }

        // Set the min & max widths and heigths:
        if (d->orientation == Qt::Horizontal) {
            d->mode_list_widget->setMinimumWidth(d->mode_list_widget->sizeHint().width());
            d->mode_list_widget->setMaximumWidth(d->mode_list_widget->sizeHint().width());
            emit modeListItemSizesChanged();
        } else {
            d->mode_list_widget->setMinimumHeight(d->mode_list_widget->sizeHint().height());
            d->mode_list_widget->setMaximumHeight(d->mode_list_widget->sizeHint().height());
            emit modeListItemSizesChanged();
        }

        // Set size hint for all items:
        for (int i = 0; i < d->mode_list_widget->count(); i++)
            d->mode_list_widget->item(i)->setSizeHint(d->mode_list_widget->sizeHint());
    }
}

QList<int> Qtilities::CoreGui::ModeManager::modeNamesToIDs(QStringList name_list) const {
    QList<int> int_list;

    name_list.removeDuplicates();
    foreach (QString name, name_list) {
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

bool Qtilities::CoreGui::ModeManager::isValidModeID(int mode_id) const {
    return !(d->id_iface_map.keys().contains(mode_id));
}

void Qtilities::CoreGui::ModeManager::handleModeListCurrentItemChanged(QListWidgetItem * item) {
    if (!item)
        return;

    if (!d->id_iface_map[item->type()])
        return;

    if (d->id_iface_map.keys().contains(item->type())) {
        d->id_iface_map[item->type()]->aboutToBeActivated();
        if (!d->id_iface_map[item->type()]->contextString().isEmpty())
            CONTEXT_MANAGER->setNewContext(d->id_iface_map[item->type()]->contextString());
        emit changeCentralWidget(d->id_iface_map[item->type()]->modeWidget());
        d->id_iface_map[item->type()]->justActivated();
    }

    d->active_mode = item->type();
}

void Qtilities::CoreGui::ModeManager::handleModeShortcutActivated() {
    QShortcut* shortcut = qobject_cast<QShortcut*> (sender());
    if (shortcut) {
        for (int i = 0; i < d->mode_shortcuts.count(); i++) {
            if (d->mode_shortcuts.values().at(i) == shortcut) {
                setActiveMode(d->mode_shortcuts.keys().at(i));
                break;
            }
        }
    }
}

void Qtilities::CoreGui::ModeManager::setActiveMode(int mode_id) {
    if (d->disabled_modes.contains(mode_id))
        return;

    d->active_mode = mode_id;
    refreshList();
}

void Qtilities::CoreGui::ModeManager::setActiveMode(const QString& mode_name) {
    // Go through all the registered mode and try to match each mode with new_mode.
    for (int i = 0; i < d->id_iface_map.count(); i++) {
        if (d->id_iface_map.values().at(i)->modeName() == mode_name) {
            if (d->disabled_modes.contains(d->id_iface_map.values().at(i)->modeID()))
                return;

            d->active_mode = d->id_iface_map.values().at(i)->modeID();
            break;
        }
    }
    refreshList();
}

void Qtilities::CoreGui::ModeManager::setActiveMode(IMode* mode_iface) {
    // Go through all the registered mode and try to match each mode with new_mode.
    for (int i = 0; i < d->id_iface_map.count(); i++) {
        if (d->id_iface_map.values().at(i) == mode_iface) {
            if (d->disabled_modes.contains(d->id_iface_map.values().at(i)->modeID()))
                return;

            d->active_mode = d->id_iface_map.values().at(i)->modeID();
            break;
        }
    }
    refreshList();
}
