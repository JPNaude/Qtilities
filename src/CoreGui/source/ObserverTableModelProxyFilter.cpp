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

#include "ObserverTableModelProxyFilter.h"
#include "ObserverTableModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Observer.h>
#include <QtilitiesCoreConstants.h>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

Qtilities::CoreGui::ObserverTableModelProxyFilter::ObserverTableModelProxyFilter(QObject* parent) : QSortFilterProxyModel(parent) {
    hints_default = new ObserverHints;
    use_observer_hints = true;
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

Qtilities::CoreGui::ObserverTableModelProxyFilter::~ObserverTableModelProxyFilter() {
    delete hints_default;
}

void Qtilities::CoreGui::ObserverTableModelProxyFilter::toggleUseObserverHints(bool toggle) {
    if (use_observer_hints != toggle) {
        use_observer_hints = toggle;
        invalidateFilter();
    }
}

bool Qtilities::CoreGui::ObserverTableModelProxyFilter::usesObserverHints() const {
    return use_observer_hints;
}

bool Qtilities::CoreGui::ObserverTableModelProxyFilter::setCustomHints(ObserverHints *custom_hints) {
    if (!custom_hints)
        return false;

    *hints_default = *custom_hints;
    invalidateFilter();
    return true;
}

ObserverHints *Qtilities::CoreGui::ObserverTableModelProxyFilter::customHints() const {
    return hints_default;
}

bool Qtilities::CoreGui::ObserverTableModelProxyFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    ObserverTableModel* table_model = dynamic_cast<ObserverTableModel*> (sourceModel());

    if (table_model) {
        QModelIndex name_index = sourceModel()->index(sourceRow, table_model->columnPosition(AbstractObserverItemModel::ColumnName), sourceParent);
        if (!sourceModel()->data(name_index).toString().contains(filterRegExp()))
            return false;

        Observer* observer = table_model->observerContext();
        if (observer && hints_default) {
            ObserverHints* active_hints = hints_default;
            if (use_observer_hints)
                active_hints = observer->displayHints();

            QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
            QObject* object_at_index = table_model->getObject(index);
            if (object_at_index) {
                QVariant category_variant = observer->getMultiContextPropertyValue(object_at_index,qti_prop_CATEGORY_MAP);
                QtilitiesCategory category = category_variant.value<QtilitiesCategory>();
                if (!category.isValid())
                    category << QString();

                if (active_hints->categoryFilterEnabled()) {
                    if (active_hints->hasInversedCategoryDisplay()) {
                        if (!active_hints->displayedCategories().contains(category))
                            return true;
                        else
                            return false;
                    } else {
                        if (active_hints->displayedCategories().contains(category))
                            return true;
                        else
                            return false;
                    }
                } else {
                    return true;
                }
            } else
                return false;
        }
    }
    return true;
}

bool Qtilities::CoreGui::ObserverTableModelProxyFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    ObserverTableModel* table_model = dynamic_cast<ObserverTableModel*> (sourceModel());

    if (table_model) {
        QModelIndex left_name_index = sourceModel()->index(left.row(), table_model->columnPosition(AbstractObserverItemModel::ColumnName), left.parent());
        QModelIndex right_name_index = sourceModel()->index(right.row(), table_model->columnPosition(AbstractObserverItemModel::ColumnName), right.parent());
        QObject* left_item = table_model->getObject(left_name_index);
        QObject* right_item = table_model->getObject(right_name_index);

        if (left_item && right_item) {
            // We check according to the following criteria:
            // Nodes are smaller than normal items:
            Observer* left_obs = qobject_cast<Observer*> (left_item);
            Observer* right_obs = qobject_cast<Observer*> (right_item);
            if (left_obs && right_obs) {
                // Check the names of the category
                return QString::localeAwareCompare(left_obs->observerName(), right_obs->observerName()) < 0;;
            } else if (left_obs && !right_obs)
                return true;
            else if (!left_obs && right_obs)
                return false;
        }
    }

    return QSortFilterProxyModel::lessThan(left,right);
}
