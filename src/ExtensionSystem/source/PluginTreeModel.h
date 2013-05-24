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

#ifndef PLUGIN_TREE_MODEL_H
#define PLUGIN_TREE_MODEL_H

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <QAbstractItemModel>
#include <QStack>
#include <QItemSelection>

namespace Qtilities {
    namespace ExtensionSystem {
        using namespace Qtilities::Core;

        /*!
          \class PluginTreeModel
          \brief The PluginTreeModel class is an extension of ObserverTreeModel used to display plugin information.

          <i>This class was added in %Qtilities v0.2.</i>
         */
        class PluginTreeModel : public ObserverTreeModel
        {
            Q_OBJECT

        public:
            PluginTreeModel(QObject *parent = 0);
            virtual ~PluginTreeModel() {}

            // --------------------------------
            // ObserverTableModel Implementation
            // --------------------------------
            Qt::ItemFlags flags(const QModelIndex &index) const;
            QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            QVariant data(const QModelIndex &index, int role) const;
            int rowCount(const QModelIndex &parent = QModelIndex()) const;
            int columnCount(const QModelIndex &parent = QModelIndex()) const;
            bool setData(const QModelIndex &index, const QVariant &value, int role);
        };
    }
}

#endif // PLUGIN_TREE_MODEL_H
