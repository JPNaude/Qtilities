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

#include "LoggerEnginesTableModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Logger.h>
#include <AbstractLoggerEngine.h>

using namespace Qtilities::CoreGui::Constants;

Qtilities::CoreGui::LoggerEnginesTableModel::LoggerEnginesTableModel(QObject* parent) : QAbstractTableModel(parent)
{
    connect(Log,SIGNAL(loggerEngineCountChanged(AbstractLoggerEngine*,Logger::EngineChangeIndication)),SLOT(handleLoggerEngineChanged(AbstractLoggerEngine*,Logger::EngineChangeIndication)));
}

QVariant Qtilities::CoreGui::LoggerEnginesTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.column() == NameColumn) {
        if (role == Qt::DisplayRole) {
            return Log->attachedLoggerEngineNames().at(index.row());
        } else if (role == Qt::CheckStateRole) {
            return Log->loggerEngineReference(Log->attachedLoggerEngineNames().at(index.row()))->isActive();
        } else if (role == Qt::ToolTipRole) {
            return Log->attachedLoggerEngineNames().at(index.row());
        }
    }

    return QVariant();
}

Qt::ItemFlags Qtilities::CoreGui::LoggerEnginesTableModel::flags(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;
     item_flags &= ~Qt::ItemIsEditable;
     item_flags |= Qt::ItemIsSelectable;
     item_flags |= Qt::ItemIsUserCheckable;

     return item_flags;
}


QVariant Qtilities::CoreGui::LoggerEnginesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((section == NameColumn) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return tr("Logger Engines");
    }

    return QVariant();
}

bool Qtilities::CoreGui::LoggerEnginesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value)

    if (index.column() == NameColumn) {
        if (role == Qt::CheckStateRole) {
            if (Log->loggerEngineReference(Log->attachedLoggerEngineNames().at(index.row()))->isActive()) {
                Log->loggerEngineReferenceAt(index.row())->setActive(false);
            } else {
                Log->loggerEngineReferenceAt(index.row())->setActive(true);
            }
            return true;
        }
    }
    return false;
}

int Qtilities::CoreGui::LoggerEnginesTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return Log->attachedLoggerEngineCount();
}

int Qtilities::CoreGui::LoggerEnginesTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return NameColumn+1;
}

void Qtilities::CoreGui::LoggerEnginesTableModel::handleLoggerEngineChanged(AbstractLoggerEngine* engine, Logger::EngineChangeIndication change_indication) {
    Q_UNUSED(engine)
    Q_UNUSED(change_indication)

    emit dataChanged(index(0,0),index(rowCount(),columnCount()));
    emit layoutChanged();
}

void Qtilities::CoreGui::LoggerEnginesTableModel::requestRefresh() {
    emit dataChanged(index(0,0),index(rowCount(),columnCount()));
    emit layoutChanged();
}
