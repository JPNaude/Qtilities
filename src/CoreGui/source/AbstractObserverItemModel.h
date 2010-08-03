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

#ifndef ABSTRACTOBSERVERITEMMODEL_H
#define ABSTRACTOBSERVERITEMMODEL_H

#include "QtilitiesCoreGui_global.h"

#include <Observer.h>

#include <QModelIndex>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
          \class Qtilities::CoreGui::AbstractObserverItemModel
          \brief The AbstractObserverItemModel is an abstract base class which is used by all the different observer models in the Qtilities library.

          \todo
          - Wishlist: Add option to disable updating when observer context is not active.
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT AbstractObserverItemModel : public ObserverAwareBase
        {

        public:
            AbstractObserverItemModel() {
                d_naming_control = Observer::ReadOnlyNames;
                d_activity_display = Observer::NoActivityDisplay;
                d_activity_control = Observer::NoActivityControl;
                d_hierachical_display_hint = Observer::NoHierarhicalDisplayHint;
                d_item_selection_control = Observer::NonSelectableItems;
                d_item_view_column_flags = Observer::NoItemViewColumnHint;               
            }
            virtual ~AbstractObserverItemModel() {}

            //! Sets the naming control for this model.
            void setNamingControl(Observer::NamingControl naming_control) { d_naming_control = naming_control; }
            //! Gets the naming control for this model.
            inline Observer::NamingControl namingControl() const { return d_naming_control; }
            //! Sets the activity display for this model.
            void setActivityDisplay(Observer::ActivityDisplay activity_display) { d_activity_display = activity_display; }
            //! Gets the activity display for this model.
            inline Observer::ActivityDisplay activityDisplay() const { return d_activity_display; }
            //! Sets the activity control for this model.
            void setActivityControl(Observer::ActivityControl activity_control) { d_activity_control = activity_control; }
            //! Gets the activity control for this model.
            inline Observer::ActivityControl activityControl() const { return d_activity_control; }
            //! Sets the hierachical display hint for this model.
            void setHierachicalDisplayHint(Observer::HierarhicalDisplay hierarhical_display) { d_hierachical_display_hint = hierarhical_display; }
            //! Gets the hierachical display hint for this model.
            inline Observer::HierarhicalDisplay hierachicalDisplayHint() const { return d_hierachical_display_hint; }
            //! Sets the selection control for this model.
            void setItemSelectionControl(Observer::ItemSelectionControl item_selection_control) { d_item_selection_control = item_selection_control; }
            //! Gets the selection control for this model.
            inline Observer::ItemSelectionControl itemSelectionControl() const { return d_item_selection_control; }

            void setObserverContext(Observer* observer) { ObserverAwareBase::setObserverContext(observer); }

            virtual int getSubjectID(const QModelIndex &index) const = 0;
            virtual QObject* getObject(const QModelIndex &index) const = 0;

        protected:
            QStringList d_headers;
            Observer::NamingControl d_naming_control;
            Observer::ActivityDisplay d_activity_display;
            Observer::ActivityControl d_activity_control;
            Observer::HierarhicalDisplay d_hierachical_display_hint;
            Observer::ItemSelectionControl d_item_selection_control;
            Observer::ItemViewColumnFlags d_item_view_column_flags;
        };
    }
}

#endif //  ABSTRACTOBSERVERITEMMODEL_H
