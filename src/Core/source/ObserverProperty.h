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

#ifndef OBSERVERPROPERTY_H
#define OBSERVERPROPERTY_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QMetaType>
#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {

/*! \class Qtilities::Core::ObserverProperty
\brief An ObserverProperty object represents a single property which can be added as a dynamic property to a QObject when observed by an observer.

An ObserverProperty is a property which is different between different observer contexts. An observer context (which is obtained by observerID() on
the context that you are interested in) is needed when getting or setting the value of the property.

A complete map of the observer contexts and their respective QVariant() values can be obtained sing observerMap(). To get the last observer context
for which the property changed use lastChangedContext(). You can check if a ObserverProperty is valid using the isValid() call.

Convenience functions for working with different contexts include hasContext(), addContext() and removeContext().

Next an example is provided which will show how to create an observer property and add it to an object. Unless you want to create new subject filters,
it should probably not be neccesary to ever create properties yourself. One exception is object properties which guides observers during subject
attachment. A list of properties which are evaluated by observers during attachment are provided in the Observer class documentation. In most cases
it will only be neccesary to set or get the values of properties added to an object by observers and their installed subject filters.

As an example we will define a limit on the number of observer which may observer a object. Since this property will be shared among various contexts,
we use SharedObserverProperty which is a unique type of ObserverProperty.

\code
Observer* my_observer_1 = new Observer("Test Observer 1","Test Subjects");
Observer* my_observer_2 = new Observer("Test Observer 2","Test Subjects");
QObject* test_subject = new QObject();

// Define a limit of 1, thus this object may only be observed by 1 observer at any given time.
SharedObserverProperty observer_limit(QVariant(1),Qtilities::Core::Constants::OBSERVER_LIMIT);

// Use the convenience function provided by the Observer class to set the shared property.
my_observer_1->setSharedProperty(test_subject,observer_limit);
bool attach_success = false;

attach_success = my_observer_1->attachSubject(test_subject);
// Here attach_success = true
attach_success = my_observer_2->attachSubject(test_subject);
// Here attach_success = false since the observer limit is already reached.

// Once a property is defined, we can change the property value by using another convenience function provided by the Observer class.
// We set the limit to -1 which is interpreted as unlimited.
my_observer_2->setObserverPropertyValue(test_subject,Qtilities::Core::Constants::OBSERVER_LIMIT,QVariant(-1));

attach_success = my_observer_2->attachSubject(test_subject);
// Now attach_success = true.
\endcode
*/
        class QTILIITES_CORE_SHARED_EXPORT ObserverProperty
        {
        public:
            ObserverProperty(const char* property_name = "")  {
                name = property_name;
                last_change_context = -1;
                is_exportable = true;
            }
            ObserverProperty(QDataStream &ds) {
                importObserverPropertyBinary(ds);
            }
            ObserverProperty(const ObserverProperty& observer_property) {
                observer_map = observer_property.observerMap();
                name = observer_property.propertyName();
                last_change_context = observer_property.lastChangedContext();
                is_exportable = observer_property.isExportable();
            }
            virtual ~ObserverProperty() {}

            //! Exports the observer properties to a QDataStream.
            bool exportObserverPropertyBinary(QDataStream& stream) const;
            //! Imports the observer properties from a QDataStream.
            bool importObserverPropertyBinary(QDataStream& stream);
            //! Get the name of this property.
            inline const char* propertyName() const { return name; }

            //! Function to check if an observer property is valid.
            inline bool isValid() { return (name != QString()); }

            //! Returns the observer_map of this observer.
            inline QMap<int,QVariant> observerMap() const { return observer_map; }

            //! Returns the value of the property.
            /*!
              \param observer_context Indicates the observer context for which the property value is required. If the property is
              shared between context's, the observer_context paramater is ignored.
              */
            virtual QVariant value(int observer_context = -1) const {
                return (observer_map.value(observer_context));
            }

            //! Sets the value of the property.
            /*!
              Sets the value for a specific observer context. If the observer context does not exist, it is created with the
              specified value assigned to it.
              \param new_value The new QVariant value which must be assigned to the property.
              \param observer_context Indicates the observer context for which the property value needs to be set.
              */
            virtual void setValue(QVariant new_value, int observer_context) {
                if (observer_context == -1)
                    return;
                if (!new_value.isValid())
                    return;
                observer_map[observer_context] = new_value;
                last_change_context = observer_context;
            }

            //! Returns the context in which the last change took place.
            int lastChangedContext() const {
                return last_change_context;
            }

            //! Checks if this property is defined within the context of the specified observer.
            bool hasContext(int observer_context) const {
                return observer_map.contains(observer_context);
            }

            //! Removes an observer context from the property.
            void removeContext(int observer_context) {
                observer_map.remove(observer_context);
            }

            //! Adds an observer context from the property.
            void addContext(QVariant new_value, int observer_context) {
                if (!observer_map.keys().contains(observer_context))
                    observer_map[observer_context] = new_value;
            }

            //! Set if this property must be exportable. That is, when a binary export is performed on an object with this property set, it will be exported with the object and restored when the object is imported again. Default is true.
            void setIsExportable(bool new_is_exportable) {
                is_exportable = new_is_exportable;
            }

            //! Returns true if this property is exportable. That is, when a binary export is performed on an object with this property set, it will be exported with the object and restored when the object is imported again. Default is true.
            bool isExportable() const {
                return is_exportable;
            }

        protected:
            const char*             name;
            QMap<int,QVariant>      observer_map;
            int                     last_change_context;
            bool                    is_exportable;
        };

        /*! \class Qtilities::Core::SharedObserverProperty
            \brief A SharedObserverProperty object represents an ObserverProperty which is shared between multiple observer contexts.

            A SharedObserverProperty is a subclass of ObserverProperty which provides an implementation where the property value is
            the same (shared) in more than one observer context.

            Note that the convenience functions for working with different contexts (hasContext(), addContext() and removeContext()) which
            are implemented by the ObserverProperty base class are meaningless on SharedObserverProperty objects. Also when getting and
            setting the value of the property using value() and setValue(), you do not need to specify a context.
        */

        class QTILIITES_CORE_SHARED_EXPORT SharedObserverProperty : public ObserverProperty
        {
        public:
            SharedObserverProperty(QVariant init_value = QVariant(), const char* property_name = "") : ObserverProperty(property_name)  {
                property_value = init_value;
            }
            SharedObserverProperty(QDataStream &ds) {
                importSharedPropertyBinary(ds);
            }
            SharedObserverProperty(const SharedObserverProperty& shared_property) : ObserverProperty(shared_property.propertyName()) {
                name = shared_property.propertyName();
                last_change_context = -1;
                property_value = shared_property.value();
                is_exportable = shared_property.isExportable();
            }
            ~SharedObserverProperty() {}

            //! Exports the shared observer properties to a QDataStream.
            bool exportSharedPropertyBinary(QDataStream& stream) const;
            //! Imports the shared observer properties from a QDataStream.
            bool importSharedPropertyBinary(QDataStream& stream);

            //! Returns the value of the property.
            /*!
              \param observer_context Ignored since this is a shared property.
              */
            QVariant value(int observer_context = -1) const {
                Q_UNUSED(observer_context)

                return property_value;
            }
            //! Sets the value of the property.
            /*!
              \param new_value The new QVariant value which must be assigned to the property.
              \param observer_context Ignored since this is a shared property.
              */
            void setValue(QVariant new_value, int observer_context = -1) {
                Q_UNUSED(observer_context)

                if (!new_value.isValid())
                    return;

                property_value = new_value;
            }

        private:
            QVariant property_value;
        };
    }
}

Q_DECLARE_METATYPE(Qtilities::Core::ObserverProperty);
Q_DECLARE_METATYPE(Qtilities::Core::SharedObserverProperty);

#endif // OBSERVERPROPERTY_H
