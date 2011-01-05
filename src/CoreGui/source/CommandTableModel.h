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

#ifndef COMMANDTABLEMODEL_H
#define COMMANDTABLEMODEL_H

#include "QtilitiesCoreGui_global.h"

#include <QMutex>
#include <QAbstractTableModel>
#include <QItemSelection>

namespace Qtilities {
    namespace CoreGui {

        //! The CommandTableModel class provides an ready-to-use model that can be used to show the contents of an Observer in a QTableView.
        /*!
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CommandTableModel : public QAbstractTableModel
        {
            Q_OBJECT

        public:
            CommandTableModel(QObject* parent = 0);
            virtual ~CommandTableModel() {}

            virtual Qt::ItemFlags flags(const QModelIndex &index) const;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            virtual QVariant data(const QModelIndex &index, int role) const;
            int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
            void refreshModel();

        protected:
            int id_column;
            int default_text_column;
            int shortcut_column;
        };
    }
}

#endif // COMMANDTABLEMODEL_H
