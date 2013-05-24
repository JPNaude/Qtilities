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

#include "LoggerEnginesTableModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Logger.h>
#include <AbstractLoggerEngine.h>

using namespace Qtilities::CoreGui::Constants;

Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::qti_private_LoggerEnginesTableModel(QObject* parent) : QAbstractTableModel(parent) {

}

QVariant Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::data(const QModelIndex &index, int role) const {
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

Qt::ItemFlags Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::flags(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags item_flags = Qt::ItemIsEnabled;
     item_flags &= ~Qt::ItemIsEditable;
     item_flags |= Qt::ItemIsSelectable;
     item_flags |= Qt::ItemIsUserCheckable;

     return item_flags;
}

QVariant Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((section == NameColumn) && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        return tr("Logger Engines");
    }

    return QVariant();
}

bool Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

int Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return Log->attachedLoggerEngineCount();
}

int Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return NameColumn+1;
}

void Qtilities::CoreGui::qti_private_LoggerEnginesTableModel::requestRefresh() {
    emit layoutChanged();
}
