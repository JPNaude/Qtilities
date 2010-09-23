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
          \brief The PluginTreeModel class is an extension of AbstractObserverTreeModel used to display plugin information.

          <i>This class was added in %Qtilities v0.2.</i>
         */
        class PluginTreeModel : public AbstractObserverTreeModel
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
