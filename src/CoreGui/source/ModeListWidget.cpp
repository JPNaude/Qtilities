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

#include "ModeListWidget.h"

#include <QApplication>
#include <QtDebug>

Qtilities::CoreGui::ModeListWidget::ModeListWidget(Qt::Orientation orientation, QWidget* parent) : QListWidget(parent) {
    d_orientation = orientation;
    min_size = QSize();
    max_size = QSize();
}

QSize Qtilities::CoreGui::ModeListWidget::sizeHint() const {
    int min_w = 0;
    int min_h = 0;
    if (min_size.width() >= 0)
        min_w = min_size.width();
    if (min_size.height() >= 0)
        min_h = min_size.height();

    int max_w;
    int max_h;
    if (max_size.width() >= 0)
        max_w = min_size.width();
    else
        max_w = 1000;
    if (max_size.height() >= 0)
        max_h = min_size.height();
    else
        max_h = 1000;

    QSize maxSize = QSize(min_w,min_h);
    for (int i = 0; i < count(); i++) {
        const QModelIndex index = model()->index(i,0);
        const QSize tmpSize = sizeHintForIndex(index);

        if (tmpSize.width() > maxSize.width() && tmpSize.width() < max_w)
            maxSize.setWidth(tmpSize.width());
        if (tmpSize.height() > maxSize.height() && tmpSize.height() < max_h)
            maxSize.setHeight(tmpSize.height());
    }

    return QSize(maxSize.width() + rect().width() - contentsRect().width(), maxSize.height() + rect().height() - contentsRect().height());
    //    return QSize(QListWidget::sizeHint().width(), maxSize.height() + rect().height() - contentsRect().height());
}


