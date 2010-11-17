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

#include "PluginTreeModel.h"
#include "ExtensionSystemCore.h"

#include "IPlugin.h"

using namespace Qtilities::ExtensionSystem::Interfaces;

Qtilities::ExtensionSystem::PluginTreeModel::PluginTreeModel(QObject* parent) : AbstractObserverTreeModel(parent) {

}

QVariant Qtilities::ExtensionSystem::PluginTreeModel::data(const QModelIndex &index, int role) const {
    if ((index.column() == columnCount() - 1) && (role == Qt::DisplayRole)) {
        QObject* obj = getObject(index);
        if (obj) {
            IPlugin* pluginIFace = qobject_cast<IPlugin*> (obj);
            if (pluginIFace)
                return pluginIFace->pluginVersion();         
        }
        return QVariant();
    } else if ((index.column() == columnCount() - 1) && (role == Qt::ForegroundRole)) {
        // Check if the plugin is a core plugin, in which case we must give it a disabled foreground color:
        QObject* obj = getObject(index);
        if (obj) {
            IPlugin* pluginIFace = qobject_cast<IPlugin*> (obj);
            if (pluginIFace) {
                QString plugin_name = pluginIFace->pluginName();
                if (EXTENSION_SYSTEM->corePlugins().contains(plugin_name)) {
                    return QApplication::palette().brush(QPalette::Disabled,QPalette::Text);
                } else {
                    return QApplication::palette().brush(QPalette::Active,QPalette::Text);
                }
            }
        }
    } else {
        return AbstractObserverTreeModel::dataHelper(index,role);
    }

    return QVariant();
}

Qt::ItemFlags Qtilities::ExtensionSystem::PluginTreeModel::flags(const QModelIndex &index) const {
    if (index.column() == columnCount() - 1) {
        Qt::ItemFlags item_flags = 0;
        item_flags |= Qt::ItemIsEnabled;
        item_flags |= Qt::ItemIsSelectable;
        return item_flags;
    } else {
        return AbstractObserverTreeModel::flagsHelper(index);
    }
}

QVariant Qtilities::ExtensionSystem::PluginTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((section == columnCount() - 1) && (role == Qt::DisplayRole)) {
        return QString("Version");
    } else {
        return AbstractObserverTreeModel::headerDataHelper(section,orientation,role);
    }
}

bool Qtilities::ExtensionSystem::PluginTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.column() == columnCount() - 1) {
        return false;
    } else {
        return AbstractObserverTreeModel::setDataHelper(index,value,role);
    }
}

int Qtilities::ExtensionSystem::PluginTreeModel::rowCount(const QModelIndex &parent) const {
    return AbstractObserverTreeModel::rowCountHelper(parent);
}

int Qtilities::ExtensionSystem::PluginTreeModel::columnCount(const QModelIndex &parent) const {
    return AbstractObserverTreeModel::columnCountHelper(parent) + 1;
}
