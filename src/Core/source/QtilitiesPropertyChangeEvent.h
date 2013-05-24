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
