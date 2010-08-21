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

#include "ObserverTableModel.h"

Qtilities::CoreGui::ObserverTableModel::ObserverTableModel(QObject* parent) : AbstractObserverTableModel(parent) {

}

QVariant Qtilities::CoreGui::ObserverTableModel::data(const QModelIndex &index, int role) const {
    return AbstractObserverTableModel::dataHelper(index,role);
}

Qt::ItemFlags Qtilities::CoreGui::ObserverTableModel::flags(const QModelIndex &index) const
{
    return AbstractObserverTableModel::flagsHelper(index);
}

QVariant Qtilities::CoreGui::ObserverTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return AbstractObserverTableModel::headerDataHelper(section,orientation,role);
}

bool Qtilities::CoreGui::ObserverTableModel::setData(const QModelIndex &index, const QVariant &value, int role){
    return AbstractObserverTableModel::setDataHelper(index,value,role);
}

int Qtilities::CoreGui::ObserverTableModel::rowCount(const QModelIndex &parent) const {
    return AbstractObserverTableModel::rowCountHelper(parent);
}

int Qtilities::CoreGui::ObserverTableModel::columnCount(const QModelIndex &parent) const {
    return AbstractObserverTableModel::columnCountHelper(parent);
}
