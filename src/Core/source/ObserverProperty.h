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

#ifndef OBSERVERPROPERTY_H
#define OBSERVERPROPERTY_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QMetaType>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {

/*!
\class Qtilities::Core::ObserverProperty
\brief An ObserverProperty object represents a single property which can be added as a dynamic property to a QObject when observed by an observer.

An ObserverProperty is a property which is different between different observer contexts. An observer context (which is obtained by \p observerID() on
the context that you are interested in) is needed when getting or setting the value of the property.

A complete map of the observer contexts and their respective QVariant() values can be obtained sing observerMap(). To get the last observer context
for which the property changed use lastChangedContext(). You can check if a ObserverProperty is valid using the isValid() call.

Convenience functions for working with different contexts include hasContext(), addContext() and removeContext().

Next an example is provided which will show how to create an observer property and add it to an object. Unless you want to create new subject filters,
it should probably not be neccesary to ever create properties yourself. One exception is object properties which guides observers during subject
attachment. A list of properties which are evaluated by observers during attachment are provided in the Observer class documentation. In most cases
it will only be neccesary to set or get the values of properties added to an object by observers and their installed subject filters.

See the \ref observer_properties section of the \ref page_observers article for an example of how to work with ObserverProperty objects.
*/
        class QTILIITES_CORE_SHARED_EXPORT ObserverProperty
        {
        public:
            ObserverProperty(const char* property_name = "")  {
                name = property_name;
                last_change_context = -1;
                is_exportable = ObserverProperty::propertyIsExportable(property_name);
                is_reserved = ObserverProperty::propertyIsReserved(property_name);
                supports_change_notifications = ObserverProperty::propertySupportsChangeNotifications(property_name);
                is_removable = ObserverProperty::propertyIsRemovable(property_name);
            }
            ObserverProperty(QDataStream &ds) {
                importObserverPropertyBinary(ds);
            }
            ObserverProperty(const ObserverProperty& observer_property) {
                observer_map = observer_property.observerMap();
                name = observer_property.propertyName();
                last_change_context = observer_property.lastChangedContext();
                is_exportable = observer_property.isExportable();
                is_reserved = observer_property.isReserved();
                is_removable = observer_property.isRemovable();
                supports_change_notifications = observer_property.supportsChangeNotifications();
            }
            void operator=(const ObserverProperty& observer_property) {
                observer_map = observer_property.observerMap();
                name = observer_property.propertyName();
                last_change_context = observer_property.lastChangedContext();
                is_exportable = observer_property.isExportable();
                is_reserved = observer_property.isReserved();
                is_removable = observer_property.isRemovable();
                supports_change_notifications = observer_property.supportsChangeNotifications();
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
            virtual bool setValue(QVariant new_value, int observer_context) {
                if (observer_context == -1)
                    return false;
                if (!new_value.isValid())
                    return false;
                observer_map[observer_context] = new_value;
                last_change_context = observer_context;
                return true;
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
            //! Indicates if the property is reserved.
            /*!
              Reserved properties are is managed by %Qtilities and must be be changed by developers. Changes to these properties will automatically be filtered by Observers observing them.

              For non-Qtilities properties this is false by default.
              */
            bool isReserved() const {
                return is_reserved;
            }
            //! Makes the property reserved. Properties can only be made reserved once, after that they always stay reserved.
            void makeReserved() {
                is_reserved = true;
            }
            //! Indicates if the property is removable.
            /*!
              Removable properties can be removed by developers. Properties that are not removable cannot be removed.

              For non-Qtilities properties this is true by default.
              */
            bool isRemovable() const {
                return is_removable;
            }
            //! Makes the property reserved. Properties can only be made reserved once, after that they always stay reserved.
            void makeNotRemovable() {
                is_removable = false;
            }
            //! Set if this property must be exportable. That is, when a binary export is performed on an object with this property set, it will be exported with the object and restored when the object is imported again.
            void setIsExportable(bool new_is_exportable) {
                is_exportable = new_is_exportable;
            }
            //! Returns true if this property is exportable. That is, when a binary export is performed on an object with this property set, it will be exported with the object and restored when the object is imported again.
            /*!
              For non-Qtilities properties this is true by default.
              */
            bool isExportable() const {
                return is_exportable;
            }
            //! Indicates if this property supports change notifications.
            /*!
              For non-Qtilities properties this is true by default.
              */
            bool supportsChangeNotifications() const {
                return supports_change_notifications;
            }

            //! Function to check if any %Qtilities property is exportable.
            static bool propertyIsExportable(const char* property_name);
            //! Function to check if any %Qtilities property is reserved.
            static bool propertyIsReserved(const char* property_name);
            //! Function to check if any %Qtilities property is removable.
            static bool propertyIsRemovable(const char* property_name);
            //! Function to check if any %Qtilities property supports change notifications.
            static bool propertySupportsChangeNotifications(const char* property_name);

        protected:
            const char*             name;
            QMap<int,QVariant>      observer_map;
            int                     last_change_context;
            bool                    is_exportable;
            bool                    is_reserved;
            bool                    is_removable;
            bool                    supports_change_notifications;
        };

        /*!
        \class Qtilities::Core::SharedObserverProperty
        \brief A SharedObserverProperty object represents an ObserverProperty which is shared between multiple observer contexts.

        A SharedObserverProperty is a subclass of ObserverProperty which provides an implementation where the property value is
        the same (shared) in more than one observer context.

        Note that the convenience functions for working with different contexts (hasContext(), addContext() and removeContext()) which
        are implemented by the ObserverProperty base class are meaningless on SharedObserverProperty objects. Also when getting and
        setting the value of the property using value() and setValue(), you do not need to specify a context.

        See the \ref observer_properties section of the \ref page_observers article for an example of how to work with SharedObserverProperty objects.
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
                supports_change_notifications = shared_property.supportsChangeNotifications();
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
            bool setValue(QVariant new_value, int observer_context = -1) {
                Q_UNUSED(observer_context)

                if (!new_value.isValid())
                    return false;

                property_value = new_value;
                return true;
            }

            //! Function to check if any %Qtilities property is exportable.
            static bool propertyIsExportable(const char* property_name) {
                return ObserverProperty::propertyIsExportable(property_name);
            }
            //! Function to check if any %Qtilities property is reserved.
            static bool propertyIsReserved(const char* property_name) {
                return ObserverProperty::propertyIsReserved(property_name);
            }
            //! Function to check if any %Qtilities property is removable.
            static bool propertyIsRemovable(const char* property_name) {
                return ObserverProperty::propertyIsRemovable(property_name);
            }
            //! Function to check if any %Qtilities property supports change notifications.
            static bool propertySupportsChangeNotifications(const char* property_name) {
                return ObserverProperty::propertySupportsChangeNotifications(property_name);
            }

        private:
            QVariant property_value;
        };
    }
}

Q_DECLARE_METATYPE(Qtilities::Core::ObserverProperty);
Q_DECLARE_METATYPE(Qtilities::Core::SharedObserverProperty);

#endif // OBSERVERPROPERTY_H
