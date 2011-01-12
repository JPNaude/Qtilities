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

#ifndef ABSTRACTOBSERVERITEMMODEL_H
#define ABSTRACTOBSERVERITEMMODEL_H

#include "QtilitiesCoreGui_global.h"
#include "NamingPolicyFilter.h"

#include <Observer>
#include <ObserverHints>
#include <ActivityPolicyFilter>

#include <QModelIndex>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct AbstractObserverItemModelData
        \brief Structure used by AbstractObserverItemModel to store private data.
          */
        struct AbstractObserverItemModelData {
            AbstractObserverItemModelData() { }

            //! Used to store default observer hints to be used with this widget.
            QPointer<ObserverHints> hints_default;
            //! Used to store observer hints for the root observer (top level observer)
            QPointer<ObserverHints> hints_top_level_observer;
            //! Used to store observer hints for the current selection parent observer.
            QPointer<ObserverHints> hints_selection_parent;
            //! Indicates if the default observer hints, or the current selection parent observer hints are used.
            bool use_observer_hints;

            //! The activity policy filter of the current observer context, if present.
            QPointer<ActivityPolicyFilter>   activity_filter;
            //! The naming policy filter of the current observer context, if present.
            QPointer<NamingPolicyFilter>     naming_filter;
        };

        /*!
          \class AbstractObserverItemModel
          \brief The AbstractObserverItemModel is an abstract base class which is used by all the different observer models in the %Qtilities library.

          This class inhertis ObserverAwareBase which is used to define the context (top level context in tree views) to be used
          in the model. The hints which should be used by the model is provided by the displayHints() function.
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT AbstractObserverItemModel : public ObserverAwareBase
        {

        public:
            AbstractObserverItemModel();
            virtual ~AbstractObserverItemModel();

            //! The possible columns which can be requested by views for an module.
            enum ColumnID {
                ColumnSubjectID,
                ColumnName,
                ColumnChildCount,
                ColumnAccess,
                ColumnTypeInfo,
                ColumnCategory,
                ColumnLast
            };
            //! The model implementation must return the index (starting at 0) for the requested column.
            /*!
              \note If your model does not support a specific column, return -1.
              \note ColumnLast must return the number of columns you use in your model.
              */
            virtual int columnPosition(ColumnID column_id) const = 0;

            //! Abstract function which must be implemented to get the subject ID of the object at the given index.
            virtual int getSubjectID(const QModelIndex &index) const = 0;
            //! Abstract function which must be implemented to get the object at the given index.
            virtual QObject* getObject(const QModelIndex &index) const = 0;

            //! Function to toggle usage of hints from the active parent observer. If not default hints will be used.
            /*!
             When toggle is equal to usesObserverHints() this function does nothing. When this is not the case this function
             will call inheritObserverHints() with the base observer when toggle is false.  When true inheritObserverHints()
             will be called with \p observer as the parameter.
             */
            void toggleUsesObserverHints(bool toggle, Observer* observer = 0);
            //! Function to indicate if observer hints are used in this model. If not, the default hints in the model base class is used.
            bool usesObserverHints() const;
            //! Get hints from the specified observer and use it as the current selection parent's hints.
            /*!
              \returns True when hints was inherited successfully, false otherwise.
              */
            bool inheritObserverHints(const Observer* observer);
            //! This function will provide the hints which should be used by this model at any time.
            /*!
              \sa toggleUseObserverHints()
              */
            ObserverHints* activeHints() const;
            ObserverHints* activeHints();

            //! Implement the virtual function to get references to known filters.
            virtual bool setObserverContext(Observer* observer);

        protected:
            AbstractObserverItemModelData* model;
        };
    }
}

#endif //  ABSTRACTOBSERVERITEMMODEL_H
