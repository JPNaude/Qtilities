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

#ifndef OBSERVER_TABLE_MODEL_PROXY_FILTER_H
#define OBSERVER_TABLE_MODEL_PROXY_FILTER_H

#include "Observer.h"
#include "QtilitiesCoreGui_global.h"

#include <ObserverHints>

#include <QSortFilterProxyModel>

using namespace Qtilities::Core;

namespace Qtilities {
    namespace CoreGui {
        /*!
          \class ObserverTableModelProxyFilter
          \brief The ObserverTableModelProxyFilter class is an implementation of a QSortFilterProxyModel which is used for advanced filtering in ObserverTreeModel.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTableModelProxyFilter : public QSortFilterProxyModel
        {
            Q_OBJECT

        public:
            ObserverTableModelProxyFilter(QObject* parent = 0);
            virtual ~ObserverTableModelProxyFilter();

            //! Function to toggle usage of hints from the active parent observer. If not custom hints will be used.
            /*!
              \sa activeHints(), setCustomHints()

              <i>This function was added in %Qtilities v1.2.</i>
              */
            void toggleUseObserverHints(bool toggle);
            //! Function to indicate if observer hints are used in this model. If not, the default hints in the model base class is used.
            /*!
                <i>This function was added in %Qtilities v1.2.</i>
             */
            bool usesObserverHints() const;
            //! This function allows you to copy the custom hints used by this proxy filter from a different ObserverHints instance.
            /*!
              \note These hints are only used when usesObserverHints() is false.
              \note The model will take ownership of the custom hints instance.

              \sa toggleUseObserverHints()

              <i>This function was added in %Qtilities v1.2.</i>
              */
            bool setCustomHints(ObserverHints* custom_hints);
            //! This function will provide the custom hints of this proxy filter.
            /*!
              \sa toggleUseObserverHints()

              <i>This function was added in %Qtilities v1.2.</i>
              */
            ObserverHints* customHints() const;

        protected:
            virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
            virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

        private:
            QPointer<ObserverHints>         hints_default;
            bool                            use_observer_hints;
        };
    }
}

#endif // OBSERVER_TABLE_MODEL_PROXY_FILTER_H
