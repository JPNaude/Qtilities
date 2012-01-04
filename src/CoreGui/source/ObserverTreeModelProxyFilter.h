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

#ifndef OBSERVER_TREE_MODEL_PROXY_FILTER_H
#define OBSERVER_TREE_MODEL_PROXY_FILTER_H

#include <QSortFilterProxyModel>

#include "Observer.h"
#include "ObserverTreeItem.h"
#include "QtilitiesCoreGui_global.h"

namespace Qtilities {
    namespace CoreGui {
        /*!
          \class ObserverTreeModelProxyFilter
          \brief The ObserverTreeModelProxyFilter class is an implementation of a QSortFilterProxyModel which is used for advanced filtering in ObserverTreeModel.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTreeModelProxyFilter : public QSortFilterProxyModel
        {
            Q_OBJECT

        public:
            ObserverTreeModelProxyFilter(QObject* parent = 0);
            virtual ~ObserverTreeModelProxyFilter();

            //! Sets the tree item types to be filtered in filterAcceptsRow().
            void setRowFilterTypes(ObserverTreeItem::TreeItemTypeFlags type_flags);
            //! Gets the tree item types to be filtered in filterAcceptsRow().
            ObserverTreeItem::TreeItemTypeFlags rowFilterTypes() const;

        protected:
            virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        private:
            ObserverTreeItem::TreeItemTypeFlags row_filter_types;
        };
    }
}

#endif // OBSERVER_TREE_MODEL_PROXY_FILTER_H
