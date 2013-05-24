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
    for (int i = 0; i < count(); ++i) {
        const QModelIndex index = model()->index(i,0);
        const QSize tmpSize = sizeHintForIndex(index);

        if (tmpSize.width() > maxSize.width() && tmpSize.width() < max_w)
            maxSize.setWidth(tmpSize.width());
        if (tmpSize.height() > maxSize.height() && tmpSize.height() < max_h)
            maxSize.setHeight(tmpSize.height());
    }

    #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return QSize(maxSize.width() + rect().width() - contentsRect().width(), maxSize.height() + rect().height() - contentsRect().height());
    #else
    return QSize(maxSize.width() + rect().width() - contentsRect().width() - 27, maxSize.height() + rect().height() - contentsRect().height() + 4);
    #endif
}


