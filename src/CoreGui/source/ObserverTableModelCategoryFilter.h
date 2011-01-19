/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#ifndef OBSERVERTABLEMODELCATEGORYFILTER_H
#define OBSERVERTABLEMODELCATEGORYFILTER_H

#include <QSortFilterProxyModel>

#include "Observer.h"
#include "QtilitiesCoreGui_global.h"

namespace Qtilities {
    namespace CoreGui {
        /*!
          \class Qtilities::CoreGui::ObserverTableModelCategoryFilter
          \brief The ObserverTableModelCategoryFilter class is an implementation of a QSortFilterProxyModel which filters table items according to category hints presented by an observer.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTableModelCategoryFilter : public QSortFilterProxyModel
        {
            Q_OBJECT

        public:
            ObserverTableModelCategoryFilter(QObject* parent = 0);
            virtual ~ObserverTableModelCategoryFilter();

        protected:
            virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
        };
    }
}

#endif // OBSERVERTABLEMODELCATEGORYFILTER_H
