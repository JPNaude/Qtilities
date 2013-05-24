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

#include "ObserverTreeModelProxyFilter.h"
#include "ObserverTreeModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Observer.h>
#include <QtilitiesCoreConstants.h>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

Qtilities::CoreGui::ObserverTreeModelProxyFilter::ObserverTreeModelProxyFilter(QObject* parent) : QSortFilterProxyModel(parent) {
    row_filter_types = ObserverTreeItem::TreeItem;
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

Qtilities::CoreGui::ObserverTreeModelProxyFilter::~ObserverTreeModelProxyFilter() {

}

bool Qtilities::CoreGui::ObserverTreeModelProxyFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    ObserverTreeModel* tree_model = dynamic_cast<ObserverTreeModel*> (sourceModel());

    if (tree_model) {
        // Get the ObserverTreeItem:
        QModelIndex name_index = sourceModel()->index(sourceRow, tree_model->columnPosition(AbstractObserverItemModel::ColumnName), sourceParent);        
        ObserverTreeItem* tree_item = tree_model->getItem(name_index);
        if (tree_item) {
            // Don't ever filter the root item:
            if (tree_item->itemType() == ObserverTreeItem::TreeNode && tree_item->parentItem()) {
                if (tree_item->parentItem()->objectName() == QLatin1String("Root Item"))
                    return true;
            }

            // Filter by type:
            if (!(row_filter_types & tree_item->itemType()))
                return true;
        }
    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow,sourceParent);
}

void Qtilities::CoreGui::ObserverTreeModelProxyFilter::setRowFilterTypes(ObserverTreeItem::TreeItemTypeFlags type_flags) {
    row_filter_types = type_flags;
}

Qtilities::CoreGui::ObserverTreeItem::TreeItemTypeFlags Qtilities::CoreGui::ObserverTreeModelProxyFilter::rowFilterTypes() const {
    return row_filter_types;
}

bool Qtilities::CoreGui::ObserverTreeModelProxyFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    ObserverTreeModel* tree_model = dynamic_cast<ObserverTreeModel*> (sourceModel());

    if (tree_model) {
        int name_pos = tree_model->columnPosition(AbstractObserverItemModel::ColumnName);
        // Only do this for the name column:
        if (left.column() == name_pos && right.column() == name_pos) {
            QModelIndex left_name_index = sourceModel()->index(left.row(), name_pos, left.parent());
            QModelIndex right_name_index = sourceModel()->index(right.row(), name_pos, right.parent());
            ObserverTreeItem* left_item = tree_model->getItem(left_name_index);
            ObserverTreeItem* right_item = tree_model->getItem(right_name_index);

            if (left_item && right_item) {
                // We check according to the following criteria:
                // Categories are smaller than normal items:
                if (left_item->itemType() == ObserverTreeItem::CategoryItem && right_item->itemType() == ObserverTreeItem::CategoryItem) {
                    // Check the names of the category
                    return QString::localeAwareCompare(left_item->category().categoryTop(), right_item->category().categoryTop()) < 0;
                } else if (left_item->itemType() == ObserverTreeItem::CategoryItem && right_item->itemType() == ObserverTreeItem::TreeItem)
                    return true;
                else if (left_item->itemType() == ObserverTreeItem::TreeItem && right_item->itemType() == ObserverTreeItem::CategoryItem)
                    return false;
                else if (left_item->itemType() == ObserverTreeItem::TreeNode && right_item->itemType() == ObserverTreeItem::CategoryItem)
                    return true;
                else if (left_item->itemType() == ObserverTreeItem::CategoryItem && right_item->itemType() == ObserverTreeItem::TreeNode)
                    return false;
                else if (left_item->itemType() == ObserverTreeItem::TreeNode && right_item->itemType() == ObserverTreeItem::TreeItem)
                    return true;
                else if (left_item->itemType() == ObserverTreeItem::TreeItem && right_item->itemType() == ObserverTreeItem::TreeNode)
                    return false;
            }
        }
    }

    return QSortFilterProxyModel::lessThan(left,right);
}
