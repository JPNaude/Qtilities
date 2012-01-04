/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#ifndef QTILITIESPROPERTYCHANGEEVENT_H
#define QTILITIESPROPERTYCHANGEEVENT_H

#include <QEvent>
#include <QByteArray>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {
        /*!
          \class Qtilities::Core::QtilitiesPropertyChangeEvent
          \brief The QtilitiesPropertyChangeEvent is an event which is posted to objects to notify them about property changes.
          */
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesPropertyChangeEvent : public QEvent
        {
        public:
            //! Constructs a qtilities property change event.
            /*!
              \param property_name The name of the property that changed.
              \param observer_id The ID of the observer that posted the event. When < -1 the property is a shared property and the observer context is irrelevant.
              */
            QtilitiesPropertyChangeEvent(const QByteArray& property_name, int observer_id);

            //! Function to access the property name for which the event is delivered.
            inline QByteArray propertyName() const { return d_property_name; }
            //! Function to access the observer_id of the observer which posted the event.
            inline int observerID() { return d_observer_id; }

        private:
            int d_observer_id;
            QByteArray d_property_name;
        };
    }
}

#endif // QTILITIESPROPERTYCHANGEEVENT_H
