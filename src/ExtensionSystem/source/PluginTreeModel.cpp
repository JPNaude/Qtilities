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

#include "PluginTreeModel.h"
#include "ExtensionSystemCore.h"

#include "IPlugin.h"

using namespace Qtilities::ExtensionSystem::Interfaces;

Qtilities::ExtensionSystem::PluginTreeModel::PluginTreeModel(QObject* parent) : ObserverTreeModel(parent) {

}

QVariant Qtilities::ExtensionSystem::PluginTreeModel::data(const QModelIndex &index, int role) const {
    if ((index.column() == columnCount() - 1) && (role == Qt::DisplayRole)) {
        QObject* obj = getObject(index);
        if (obj) {
            IPlugin* pluginIFace = qobject_cast<IPlugin*> (obj);
            if (pluginIFace)
                return pluginIFace->pluginVersionInformation().version().toString();
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
        return ObserverTreeModel::data(index,role);
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
        return ObserverTreeModel::flags(index);
    }
}

QVariant Qtilities::ExtensionSystem::PluginTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((section == columnCount() - 1) && (role == Qt::DisplayRole)) {
        return QString("Version");
    } else {
        return ObserverTreeModel::headerData(section,orientation,role);
    }
}

bool Qtilities::ExtensionSystem::PluginTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.column() == columnCount() - 1) {
        return false;
    } else {
        return ObserverTreeModel::setData(index,value,role);
    }
}

int Qtilities::ExtensionSystem::PluginTreeModel::rowCount(const QModelIndex &parent) const {
    return ObserverTreeModel::rowCount(parent);
}

int Qtilities::ExtensionSystem::PluginTreeModel::columnCount(const QModelIndex &parent) const {
    return ObserverTreeModel::columnCount(parent) + 1;
}
