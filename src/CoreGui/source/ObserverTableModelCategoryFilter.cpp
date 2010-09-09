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

#include "ObserverTableModelCategoryFilter.h"
#include "AbstractObserverTableModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Observer.h>
#include <QtilitiesCoreConstants.h>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

Qtilities::CoreGui::ObserverTableModelCategoryFilter::ObserverTableModelCategoryFilter(QObject* parent) : QSortFilterProxyModel(parent) {

}

Qtilities::CoreGui::ObserverTableModelCategoryFilter::~ObserverTableModelCategoryFilter() {

}

bool Qtilities::CoreGui::ObserverTableModelCategoryFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    AbstractObserverTableModel* table_model = dynamic_cast<AbstractObserverTableModel*> (sourceModel());

    if (table_model) {
        QModelIndex name_index = sourceModel()->index(sourceRow, table_model->columnPosition(AbstractObserverItemModel::ColumnName), sourceParent);
        if (!sourceModel()->data(name_index).toString().contains(filterRegExp()))
            return false;

        Observer* observer = table_model->observerContext();
        if (observer) {
            if (observer->displayHints()) {
                if (observer->displayHints()->hierarchicalDisplayHint() == ObserverHints::CategorizedHierarchy) {
                    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
                    QObject* object_at_index = table_model->getObject(index);
                    if (object_at_index) {
                        QVariant category_variant = observer->getObserverPropertyValue(object_at_index,OBJECT_CATEGORY);
                        QString category = category_variant.toString();
                        if (category.isEmpty())
                            category = QString(OBSERVER_UNCATEGORIZED_CATEGORY);

                        if (observer->displayHints()->categoryFilterEnabled()) {
                            if (observer->displayHints()->hasInversedCategoryDisplay()) {
                                if (!observer->displayHints()->displayedCategories().contains(category))
                                    return true;
                                else
                                    return false;
                            } else {
                                if (observer->displayHints()->displayedCategories().contains(category))
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
        }
    }
    return true;
}
