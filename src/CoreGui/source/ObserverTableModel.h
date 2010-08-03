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

#ifndef OBSERVERTABLEMODEL_H
#define OBSERVERTABLEMODEL_H

#include <QMutex>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QStack>
#include <QItemSelection>

#include "QtilitiesCoreGui_global.h"
#include "AbstractObserverItemModel.h"

#include <Observer.h>
#include <ActivityPolicyFilter.h>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        class NamingPolicyFilter;

        //! The ObserverTableModel class provides an ready-to-use model that can be used to show the contents of an Observer in a QTableView.
        /*!
          The ObserverTableModel class provides an ready-to-use model that can be used to show and manage the contents of an Observer in a QTableView.

          To view the contents of an Observer, use the setObserverContext function. This function will initialize the model and show the names
          of subjects attached to a specific observer in a QTableView. ObserverTableModel has knowledge of the standard subject filters which
          are included in the Qtilities library. For more information see setNamingControl() and setActivityControl() in the Observer class documentation.

          When you develop your own subject filters and want to make information controlled by these filters visible through ObserverTableModel,
          you would need to subclassing from ObserverTableModel and implement the needed virtual functions.
        */

        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTableModel : public QAbstractTableModel, public AbstractObserverItemModel
        {
            Q_OBJECT
            Q_ENUMS(ColumnIDs)

        public:
            ObserverTableModel(const QStringList &headers = QStringList(), QObject* parent = 0);
            virtual ~ObserverTableModel() {}

            enum ColumnIDs {
                IdColumn = 0,
                CategoryColumn = 1,
                NameColumn = 2,
                ChildCountColumn = 3,
                AccessColumn = 4,
                TypeInfoColumn = 5
            };

            virtual Qt::ItemFlags flags(const QModelIndex &index) const;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            virtual QVariant data(const QModelIndex &index, int role) const;
            int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

            //! Implement the virtual function to get references to known filters.
            void setObserverContext(Observer* observer);

            //! A function which allows you to set up the widget explicitly, causing it not to follow the hints provided by the observer.
            /*!
              \note This function must be set before calling setItemModel() and initialize() to work. If you want to use manual configuration on
              a widget which was already initialized, you need to call this function, set up your flags and then call initialize again.
              */
            inline void setUseManualConfiguration(bool toggle) { d_manual_mode = toggle; }
            //! Returns true if the widget is set up to use manual configuration.
            /*!
              \sa setUseManualConfiguration()
              */
            inline bool usingManualConfiguration() const { return d_manual_mode; }

            int getSubjectID(const QModelIndex &index) const;
            int getSubjectID(int row) const;
            QObject* getObject(const QModelIndex &index) const;
            QObject* getObject(int row) const;
            QModelIndex getIndex(QObject* obj) const;

        public slots:
            virtual void handleDataChange();
            void handleDirtyProperty(const char* property_name);

        protected:
            ActivityPolicyFilter*   d_activity_filter;
            NamingPolicyFilter*     d_naming_filter;

            bool                    d_manual_mode;
            QString                 d_type_grouping_name;
        };
    }
}

#endif // OBSERVERTABLEMODEL_H
