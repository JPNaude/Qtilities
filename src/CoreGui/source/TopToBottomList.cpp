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

#include "TopToBottomList.h"

Qtilities::CoreGui::TopToBottomList::TopToBottomList(QWidget* &parent) :
    QListWidget(parent)
{

}

QSize Qtilities::CoreGui::TopToBottomList::sizeHint() const
{
  QSize maxSize = QSize(0,0);
  for ( int i = 0; i < this->count(); i++ ) {
    const QModelIndex index = model()->index( i, 0 );
    const QSize tmpSize = this->sizeHintForIndex(index);

    if (tmpSize.width() > maxSize.width())
        maxSize.setWidth(tmpSize.width());
    if (tmpSize.height() > maxSize.height())
        maxSize.setHeight(tmpSize.height());
  }

  return QSize( maxSize.width() + rect().width() - contentsRect().width(), this->height());
}
QSize Qtilities::CoreGui::TopToBottomList::itemSizeHint() const
{
  QSize maxSize = QSize(0,0);
  for ( int i = 0; i < this->count(); i++ ) {
    const QModelIndex index = model()->index( i, 0 );
    const QSize tmpSize = this->sizeHintForIndex(index);

    if (tmpSize.width() > maxSize.width())
        maxSize.setWidth(tmpSize.width());
    if (tmpSize.height() > maxSize.height())
        maxSize.setHeight(tmpSize.height());
  }

  return QSize( maxSize.width(), maxSize.height());
}
