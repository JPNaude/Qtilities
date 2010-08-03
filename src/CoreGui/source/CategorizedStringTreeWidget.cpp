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

#include "CategorizedStringTreeWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QMetaObject>
#include <QMetaMethod>
#include <QEvent>
#include <QHeaderView>
#include <QtDebug>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::CategorizedStringTreeWidgetData {
    CategorizedStringTreeWidgetData() {}
};

Qtilities::CoreGui::CategorizedStringTreeWidget::CategorizedStringTreeWidget(QWidget *parent) : QTreeWidget(parent) {
    d = new CategorizedStringTreeWidgetData;

    setColumnCount(1);
    setSortingEnabled(true);
    headerItem()->setText(0,tr("Available Items"));
}

Qtilities::CoreGui::CategorizedStringTreeWidget::~CategorizedStringTreeWidget() {
    delete d;
}

void Qtilities::CoreGui::CategorizedStringTreeWidget::appendStringMap(QMap<QString, QStringList> item_map, int source_type) {
    for (int i = 0; i < item_map.count(); i++) {
        // Get the item's parent for each category hierarchy.
        QTreeWidgetItem* item_parent = getItemParent(item_map.values().at(i));
        if (item_parent) {
            QStringList text;
            text << item_map.keys().at(i);
            QTreeWidgetItem* item = new QTreeWidgetItem(item_parent,text,source_type);
            if (item) {
                // Do nothing, just get rid of MingW warning.
            }
        }
    }

    expandToDepth(0);
    sortItems(0,Qt::AscendingOrder);
}

QTreeWidgetItem* Qtilities::CoreGui::CategorizedStringTreeWidget::getItemParent(QStringList category) {
    QTreeWidgetItem* lowest_level_item = 0;
    QTreeWidgetItem* item = 0;
    for (int i = 0; i < category.count(); i++) {
        // i represents the current depth in the tree
        // Check if the category already exists, if so step into that level of hierarhcy.
        bool exists = false;

        // Check top level items first
        if (i == 0) {
            for (int t = 0; t < topLevelItemCount(); t++) {
                if (topLevelItem(t)->text(0) == category.at(0)) {
                    item = topLevelItem(t);
                    exists = true;
                }
            }

            if (!exists) {
                QStringList text;
                text << category.at(i);
                item = new QTreeWidgetItem(this,text,0);
                item->setData(0,Qt::DecorationRole,QVariant(QIcon(ICON_FOLDER_16X16)));
            }
        } else {
            // Handle lower level items
            // Get children of item
            for (int t = 0; t < item->childCount(); t++) {
                if (item->child(t)->text(0) == category.at(i)) {
                    if (item->child(t)->type() == 0)
                        exists = true;
                    else {
                        qWarning() << tr("Found conflicting category and item strings in CategorizedStringTreeWidget. Conflicting item will not be added.");
                        return 0;
                    }
                    item = item->child(t);
                }
            }

            if (!exists) {
                QStringList text;
                text << category.at(i);
                item = new QTreeWidgetItem(item,text,0);
                item->setData(0,Qt::DecorationRole,QVariant(QIcon(ICON_FOLDER_16X16)));
            }
        }

        if (i == category.count()-1)
            lowest_level_item = item;
    }

    return lowest_level_item;
}
