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

#ifndef CATEGORIZEDSTRINGTREEWIDGET_H
#define CATEGORIZEDSTRINGTREEWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "ObserverMimeData.h"

#include <QTreeWidget>
#include <QMap>
#include <QMouseEvent>
#include <QStringList>

namespace Qtilities {
    namespace CoreGui {
        /*!
        \struct CategorizedStringTreeWidgetData
        \brief The CategorizedStringTreeWidgetData struct stores private data used by the CategorizedStringTreeWidget class.
          */
        struct CategorizedStringTreeWidgetData;

        /*!
        \class CategorizedStringTreeWidget
        \brief The CategorizedStringTreeWidget class provides a tree widget with items under specific categories.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CategorizedStringTreeWidget : public QTreeWidget {
            Q_OBJECT
            Q_ENUMS(MetaCategory)

        public:
            CategorizedStringTreeWidget(QWidget *parent = 0);
            virtual ~CategorizedStringTreeWidget();

            //! Enumeration which is used to classify items in the tree widget.
            enum MetaCategory {
                Category,
                Item
            };

            //! Sets the QMap which contains the the strings to be displayed as the keys, and the category hierarchies needed as the values.
            /*!
              \param source_type Used to keep track of which sources appended items to the widget. Pass -1 if to ignore, 0 is reserved and will be ignored.
              */
            virtual void appendStringMap(QMap<QString, QStringList> item_map, int source_type = -1);

        private:
            //! Takes care of building the category hierarchies.
            QTreeWidgetItem* getItemParent(QStringList category);

            CategorizedStringTreeWidgetData* d;
        };
    }
}

#endif // CATEGORIZEDSTRINGTREEWIDGET_H
