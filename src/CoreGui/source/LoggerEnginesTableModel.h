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

#ifndef qti_private_LoggerEnginesTableModel_H
#define qti_private_LoggerEnginesTableModel_H

#include <QMutex>
#include <QAbstractTableModel>

#include <Logger>
#include <AbstractLoggerEngine>


using namespace Qtilities::Logging;

namespace Qtilities {
    namespace CoreGui {
        class qti_private_LoggerEnginesTableModel : public QAbstractTableModel
        {
            Q_OBJECT
            Q_ENUMS(ColumnIDs)

        public:
            qti_private_LoggerEnginesTableModel(QObject* parent = 0);
            virtual ~qti_private_LoggerEnginesTableModel() {}

            enum ColumnIDs {
                NameColumn = 0
            };

            virtual Qt::ItemFlags flags(const QModelIndex &index) const;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            virtual QVariant data(const QModelIndex &index, int role) const;
            int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

        public slots:
            void requestRefresh();
        };
    }
}

#endif // qti_private_LoggerEnginesTableModel_H
