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

#ifndef ABSTRACTOBSERVERTABLEMODEL_H
#define ABSTRACTOBSERVERTABLEMODEL_H

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

        /*!
        \struct ObserverTableModelData
        \brief Structure used by ObserverTableModel to store private data.
          */
        struct ObserverTableModelData;

        //! The ObserverTableModel class provides an ready-to-use model that can be used to show the contents of an Qtilities::Core::Observer in a QTableView.
        /*!
        The ObserverTableModel class provides an ready-to-use model that can be used to show and manage the contents of an Qtilities::Core::Observer in a QTableView.

        To view the contents of an Observer, use the setObserverContext function. This function will initialize the model and show the names
        of subjects attached to a specific observer in a QTableView. ObserverTableModel has knowledge of the standard subject filters which
        are included in the %Qtilities libraries. Thus it will for example automatically show check boxes etc. if the observer context has
        an Qtilities::Core::ActivityPolicyFilter installed.

        To customize this model you can simply subclass it and reimplement the virtual functions of QAbstractItemModel. This allows you to
        add columns etc. to your view. The <a class="el" href="namespace_qtilities_1_1_examples_1_1_clipboard.html">Clipboard Example</a> shows how
        to do this.

        \sa ObserverTreeModel
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTableModel : public QAbstractTableModel, public AbstractObserverItemModel
        {
            Q_OBJECT

        public:
            ObserverTableModel(QObject* parent = 0);
            virtual ~ObserverTableModel() {}

            // --------------------------------
            // QAbstractTableModel Implementation
            // --------------------------------
            virtual Qt::ItemFlags flags(const QModelIndex &index) const;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
            virtual QVariant data(const QModelIndex &index, int role) const;
            virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

            // --------------------------------
            // AbstractObserverItemModel Implementation
            // --------------------------------
            virtual bool setObserverContext(Observer* observer);
            int columnPosition(AbstractObserverItemModel::ColumnID column_id) const;
            int getSubjectID(const QModelIndex &index) const;
             QObject* getObject(const QModelIndex &index) const;

            // --------------------------------
            // ObserverTableModel Implementation
            // --------------------------------
            //! Function which gives the visible column position. Thus it takes into account if columns are hidden.
            int columnVisiblePosition(AbstractObserverItemModel::ColumnID column_id) const;
            //! Convenience function to get the QModelIndex of an object in the table.
            /*!
               \returns The QModelIndex of the specified object. If the object was not found QModelIndex() is returned.
             */
            QModelIndex getIndex(QObject* obj) const;
            //! Convenience function to get the QObject at a specific row.
            /*!
               \returns The the object at the specified row. If the row is invalid, 0 is returned.
             */
            QObject* getObject(int row) const;
            //! Convenience function to get the subject ID of a QObject at a specific row.
            /*!
               \returns The the subject ID of the object at the specified row. If the row is invalid, -1 is returned.
             */
            int getSubjectID(int row) const;
            //! Sets if this model must be read only, thus its actions and property editor will be read only.
            /*!
              \sa readOnly()
              */
            virtual void setReadOnly(bool read_only);
            //! Gets if this model must be read only, thus its actions and property editor will be read only.
            /*!
              \sa setReadOnly()
              */
            bool readOnly() const;

        private slots:
            //! Slot which will emit the correct signals in order for the view using the model to refresh its data.
            /*!
              This slot will automatically be connected to the dataChanged() signal on the observer context displayed.
              It will call the dataChanged() signal for the complete view with the correct parameters.
              */
            virtual void handleDataChanged();
            //! Slot which will emit the correct signals in order for the view using the model to refresh its layout.
            /*!
              This slot will automatically be connected to the layoutChanged() signal on the observer context displayed.
              */
            virtual void handleLayoutChanged();

        protected:
            ObserverTableModelData* d;
        };
    }
}

#endif // ABSTRACTOBSERVERTABLEMODEL_H
