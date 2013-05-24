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

#ifndef MODE_LIST_WIDGET_H
#define MODE_LIST_WIDGET_H

#include <QListWidget>

namespace Qtilities {
    namespace CoreGui {
        class ModeListWidget : public QListWidget
        {
        public:
            ModeListWidget(Qt::Orientation orientation, QWidget* parent = 0);
            virtual QSize sizeHint() const;

            void setMinimumItemSize(QSize size) {
                min_size = size;
            }
            void setMaximumItemSize(QSize size) {
                max_size = size;
            }
            QSize minimumItemSize() const {
                return min_size;
            }
            QSize maximumItemSize() const {
                return max_size;
            }

        private:
            Qt::Orientation d_orientation;
            QSize min_size;
            QSize max_size;
        };
    }
}

#endif // MODE_LIST_WIDGET_H
