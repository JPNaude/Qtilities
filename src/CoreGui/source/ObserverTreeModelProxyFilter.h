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

#ifndef OBSERVER_TREE_MODEL_PROXY_FILTER_H
#define OBSERVER_TREE_MODEL_PROXY_FILTER_H

#include "Observer.h"
#include "ObserverTreeItem.h"
#include "QtilitiesCoreGui_global.h"

#include <QSortFilterProxyModel>

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
            virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

        private:
            ObserverTreeItem::TreeItemTypeFlags row_filter_types;
        };
    }
}

#endif // OBSERVER_TREE_MODEL_PROXY_FILTER_H
