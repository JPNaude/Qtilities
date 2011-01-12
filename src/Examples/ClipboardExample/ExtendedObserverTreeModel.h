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

#ifndef EXTENDEDOBSERVERTREEMODEL_H
#define EXTENDEDOBSERVERTREEMODEL_H

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

#include <QAbstractItemModel>
#include <QStack>
#include <QItemSelection>

namespace Qtilities {
    namespace Examples {
        namespace Clipboard {
            using namespace Qtilities::Core;

            //! The ExtendedObserverTreeModel class is an example of how Qtilities::CoreGui::ObserverTableModel can be subclassed.
            /*!
              The ExtendedObserverTreeModel class is an example of how Qtilities::CoreGui::ObserverTreeModel can be subclassed
              to add additional functionality to your model and to Qtilities::CoreGui::ObserverWidget widgets.

              This example will add additional columns.
            */
            class ExtendedObserverTreeModel : public ObserverTreeModel
            {
                Q_OBJECT

            public:
                ExtendedObserverTreeModel(QObject *parent = 0);
                virtual ~ExtendedObserverTreeModel() {}

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
}

#endif // EXTENDEDOBSERVERTREEMODEL_H
