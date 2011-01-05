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

#include "CommandTableModel.h"
#include "QtilitiesCoreGuiConstants.h"
#include "QtilitiesApplication.h"
#include "Command.h"

#include <QKeySequence>

Qtilities::CoreGui::CommandTableModel::CommandTableModel(QObject* parent) : QAbstractTableModel(parent) {
    // Init column numbers
    id_column = 0;
    default_text_column = 1;
    shortcut_column = 2;

    // Make sure the command table updates itself when new actions are registered.
    connect(ACTION_MANAGER,SIGNAL(numberOfCommandsChanged()),SIGNAL(layoutChanged()));
}

QVariant Qtilities::CoreGui::CommandTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Command* command = ACTION_MANAGER->commandMap().values().at(index.row());
    if (!command)
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.column() == id_column) {
            return command->defaultText();
        } else if (index.column() == default_text_column) {
            return command->text();
        } else if (index.column() == shortcut_column) {
            return command->keySequence().toString(QKeySequence::NativeText);
        }
    }

    return QVariant();
}

Qt::ItemFlags Qtilities::CoreGui::CommandTableModel::flags(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;
     item_flags |= Qt::ItemIsSelectable;

     return item_flags;
}

QVariant Qtilities::CoreGui::CommandTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == id_column)
                return tr("Command Name");
            else if (section == default_text_column)
                return tr("Label");
            else if (section == shortcut_column)
                return tr("Shortcut");
        }
    }

    if (orientation == Qt::Vertical)
        return tr("hide me");

    return QVariant();
}

int Qtilities::CoreGui::CommandTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return ACTION_MANAGER->commandMap().count();
}

int Qtilities::CoreGui::CommandTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return 3;
}

void Qtilities::CoreGui::CommandTableModel::refreshModel() {
    reset();
}
