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

#ifndef QTILITIES_PROPERTY_H
#define QTILITIES_PROPERTY_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QMetaType>

#include "QtilitiesCore_global.h"
#include "Qtilities.h"
#include "IExportable.h"

namespace Qtilities {
    namespace Core {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class Qtilities::Core::QtilitiesProperty
            \brief The base class of all the different %Qtilities property types.

            QtilitiesProperty is the base class of all the different %Qtilities property types. These properties are ready to use properties which
            can be set as set on QObjects using normal dynamic properties which are part of the Qt Property System, using QObject::setProperty().
            Normally, QObject::setProperty() takes a name and QVariant for its value. Using the %Qtilities properties you do more advanced operations
            using the underlying QObject dynamic properties.

            These more advanced operations are frequently used internally by the %Qtilities observer system. For information about how QtilitiesProperty is
            used in the context of Qtilities::Core::Observer, please see \ref qtilities_properties.

            See the class specific documentation for the different types of %Qtilities properties for more information:
            - Qtilities::Core::SharedProperty
            - Qtilities::Core::MultiContextProperty

            %Qtilities provides the Qtilities::CoreGui::ObjectPropertyBrowser to browse properties set using Q_PROPERTY on any QObject, while
            Qtilities::CoreGui::ObjectDynamicPropertyBrowser allows browsing of dynamic properties. The dynamic property browser supports
            normal QVariant based properties, as well as all QtilitiesProperty based classes.

            Finally, these properties are meant to be used as part of the Qt Property System as well as the %Qtilities observer system. They are
            not meant to be used as generic type property classes.
            */
            class QTILIITES_CORE_SHARED_EXPORT QtilitiesProperty : public IExportable
            {
            public:
                QtilitiesProperty(const QString& property_name = "");
                QtilitiesProperty(const char* property_name);
                QtilitiesProperty(const QtilitiesProperty& property);
                QtilitiesProperty& operator=(const QtilitiesProperty& property);
                virtual ~QtilitiesProperty();
                //! Gets the name of this property as a QString.
                QString propertyNameString() const;
                //! Sets the name of this property.
                void setPropertyName(const char* new_name);
                //! Sets the name of this property.
                void setPropertyName(const QString& new_name);
                //! Function to check if an observer property is valid.
                bool isValid();

                //! Indicates if the property is reserved.
                /*!
                  Reserved properties are managed by %Qtilities and must not be changed by developers. Changes to these properties will automatically be
                  filtered by Observers observing them.

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
                //! Makes the property non removable. Properties can only be made not removable once, after that they always stay not removable.
                void makeNotRemovable() {
                    is_removable = false;
                }

                //! Indicates if the property is read only.
                /*!
                  Read only properties cannot be edited developers.

                  \note This attribute is not used by any Qtilities properties.
                  */
                bool isReadOnly() const {
                    return read_only;
                }
                //! Makes the property read only. Properties can only be made read only once, after that they always stay read only.
                void makeReadOnly() {
                    read_only = true;
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

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                /*!
                  \note QtilitiesProperty is not a QObject, thus it returns 0.
                  */
                QObject* objectBase() { return 0; }
                /*!
                  \note QtilitiesProperty is not a QObject, thus it returns 0.
                  */
                const QObject* objectBase() const { return 0; }

                // --------------------------------
                // IExportable Implementation
                // --------------------------------
                virtual ExportModeFlags supportedFormats() const;
                virtual IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
                virtual IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
                virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
                virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

                //! Converts a QString type_string and QString value_string to a matching QVariant.
                static QVariant constructVariant(const QString& type_string, const QString& value_string);
                //! Converts a QVariant::Type and QString value_string to a matching QVariant.
                static QVariant constructVariant(QVariant::Type type, const QString& value_string);
                //! Checks if a QVariant is exportable. Thus, if it can be converted to QString.
                static bool isExportableVariant(QVariant variant);

            protected:
                QString                 name;
                bool                    is_reserved;
                bool                    read_only;
                bool                    is_removable;
                bool                    supports_change_notifications;
            };

        /*!
        \class Qtilities::Core::MultiContextProperty
        \brief A MultiContextProperty provides a property which has different values in different contexts.

        A complete map of the property's contexts and their respective QVariant() values can be obtained using contextMap(). To get the last context ID
        for which the property changed use lastChangedContext(). You can check if an MultiContextProperty is valid using the isValid() call. Convenience functions for working with different contexts include hasContext(), addContext(),removeContext() and value().

        \subsection MultiContextProperty_changing_properties Getting and setting multi context properties properties

        Getting and setting shared properties are done by the Qtilities::Core::Observer class which provides easy to use functions to set and get properties and their values. For multi context properties we need to know the context to get a value since the value is different for different contexts.

        The following example shows how to construct and work with Qtilities::Core::MultiContextProperty.
\code
QObject* obj = new QObject();
Observer* observerA = new Observer("My Observer A","Example observer description");
int observerA_ID = observerA->observerID();
Observer* observerB = new Observer("My Observer B","Example observer description");
int observerB_ID = observerB->observerID();

// Create observer property
QString property_name = "Example Property Name";
MultiContextProperty observer_property(property_name);
observer_property.setIsExportable(false);

// Attach object to observers
observerA->attachSubject(obj);
observerB->attachSubject(obj);

// Now we can add values to our property for each context
observer_property.addContext(QVariant(true),observerA->observerID());
observer_property.addContext(QVariant(false),observerB->observerID());

// Now set the observer property on the object
ObjectManager::setMultiContextProperty(obj,observer_property);

// Now we can get the value of the property for each observer using the observer references
bool value_contextA = observerA->getMultiContextPropertyValue(obj,property_name).toBool();
bool value_contextB = observerB->getMultiContextPropertyValue(obj,property_name).toBool();

// Or get the value of the property without an observer reference: Option 1
MultiContextProperty observer_property1 = ObjectManager::getMultiContextProperty(property_name);
if (observer_property1.isValid()) {
  bool value_contextA_1 = observer_property1->value(observerA_ID).toBool();
}

// Or get the value of the property without an observer reference: Option 2
QVariant prop = obj->property(property_name);
if (prop.isValid() && prop.canConvert<MultiContextProperty>()) {
  bool value_contextA_2 = (prop.value<MultiContextProperty>()).value(observerA_ID).toBool();
}
\endcode

        For more information about how MultiContextProperty are used in the context of Qtilities::Core::Observer, please see \ref qtilities_properties.

        Note that you can inspect all MultiContextProperty properties on an object through the Qtilities::CoreGui::ObjectDynamicPropertyBrowser widget.
        See the \p qti.core.CategoryMap property as an example below:

        \image html debugging_dynamic_properties.jpg "Dynamic Properties Browser"

        \sa Qtilities::Core::SharedProperty
        */
        class QTILIITES_CORE_SHARED_EXPORT MultiContextProperty : public QtilitiesProperty
        {
        public:
            MultiContextProperty(const char* property_name = "");
            MultiContextProperty(QDataStream &ds, Qtilities::ExportVersion version);
            MultiContextProperty(const MultiContextProperty& property);
            MultiContextProperty(const QtilitiesProperty& qtilities_property);
            virtual ~MultiContextProperty();
            MultiContextProperty& operator=(const MultiContextProperty& property);
            bool operator==(const MultiContextProperty& other) const;
            bool operator!=(const MultiContextProperty& other) const;

            //! Returns a map with the contexts and their respective values for this property.
            QMap<quint32,QVariant> contextMap() const;
            //! Returns the value of the property.
            /*!
              \param context_id Indicates the context ID for which the property value is required.
              */
            virtual QVariant value(int context_id) const;
            //! Sets the value of the property.
            /*!
              Sets the value for a specific context ID. If the context ID does not exist, it is created with the
              specified value assigned to it.
              \param new_value The new QVariant value which must be assigned to the property.
              \param context_id Indicates the context ID for which the property value needs to be set.
              */
            virtual bool setValue(QVariant new_value, int context_id);
            //! Returns a string representation of this property.
            /*!
              \returns A string representation which is basically a comma seperated context values list.
              */
            virtual QString valueString() const;
            //! Returns the context in which the last change took place.
            int lastChangedContext() const;
            //! Checks if this property is defined within the context of the specified observer.
            bool hasContext(int context_id) const;
            //! Removes an context ID from the property.
            void removeContext(int context_id);
            //! Adds an context ID from the property.
            void addContext(QVariant new_value, int context_id);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            /*!
              \note MultiContextProperty is not a QObject, thus it returns 0.
              */
            QObject* objectBase() { return 0; }
            /*!
              \note MultiContextProperty is not a QObject, thus it returns 0.
              */
            const QObject* objectBase() const { return 0; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            virtual ExportModeFlags supportedFormats() const;
            virtual IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            virtual IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            /*!
              This function will add a set of attributes directly to the object_node passed to it.
              */
            virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        protected:
            QMap<quint32,QVariant>  context_map;
            int                     last_change_context;
        };

        /*!
        \class Qtilities::Core::SharedProperty
        \brief A SharedProperty is a basic implementation of QtilitiesProperty.

        A SharedProperty is a basic implementation of QtilitiesProperty which stored a single QVariant value set with setValue() and can be obtained using value().

        \subsection SharedProperty_changing_properties Getting and setting shared properties

        Getting and setting shared properties are done by the Qtilities::Core::Observer class which provides easy to use functions to set and get properties and their values. For shared properties it is easy to get and set the value of the property because we do not need to know the context since the value is the same for all contexts.     
        
        The following example shows how to construct and work with Qtilities::Core::SharedProperty.
\code
QObject* obj = new QObject();
Observer* obs = new Observer("My Observer","Example observer description");

// Create shared property
QString property_name = "Example Property Name";
SharedProperty string_property(property_name,"Example text");
// For this example we don't need to make the property exportable. By default all properties are exportable.
string_property.setIsExportable(false);

// Set the property using the static convenience function provided by the object manager:
ObjectManager::setSharedProperty(obj,string_property);
ObjectManager::setSharedProperty(obj,string_property,value);

// Attach the object to the observer
obs->attachSubject(obj);

// Now we can get the value of the property using the observer reference
QString text = obs->getMultiContextPropertyValue(obj,property_name).toString();

// Or get the value of the property without the observer reference: Option 1
SharedProperty shared_property1 = ObjectManager::getSharedProperty(property_name);
if (shared_property1.isValid()) {
	QString text1 = shared_property1->value().toString();
}

// Or get the value of the property without the observer reference: Option 2
QVariant prop = obj->property(property_name);
if (prop.isValid() && prop.canConvert<SharedProperty>()) {
	QString text2 = (prop.value<SharedProperty>()).value().toString();
}
\endcode

        For information about how SharedProperty are used in the context of Qtilities::Core::Observer, please see \ref qtilities_properties.

        Note that you can inspect all SharedProperty properties on an object through the Qtilities::CoreGui::ObjectDynamicPropertyBrowser widget. See the example below:

        \image html debugging_dynamic_properties.jpg "Dynamic Properties Browser"

        \sa Qtilities::Core::MultiContextProperty
        */
        class QTILIITES_CORE_SHARED_EXPORT SharedProperty : public QtilitiesProperty
        {
        public:
            SharedProperty(const char* property_name = "", QVariant init_value = QVariant());
            SharedProperty(QDataStream &ds, Qtilities::ExportVersion version);
            SharedProperty(const SharedProperty& shared_property);
            SharedProperty(const QtilitiesProperty& qtilities_property);
            ~SharedProperty() {}
            SharedProperty& operator=(const SharedProperty& other);
            bool operator==(const SharedProperty& other) const;
            bool operator!=(const SharedProperty& other) const;

            //! Returns the value of the property.
            virtual QVariant value() const;
            //! Sets the value of the property.
            /*!
              \param new_value The new QVariant value which must be assigned to the property.
              */
            virtual bool setValue(QVariant new_value);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            /*!
              \note SharedProperty is not a QObject, thus it returns 0.
              */
            QObject* objectBase() { return 0; }
            /*!
              \note SharedProperty is not a QObject, thus it returns 0.
              */
            const QObject* objectBase() const { return 0; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            virtual IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            virtual IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            /*!
              This function will add a set of attributes directly to the object_node passed to it.
              */
            virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        private:
            QVariant property_value;
        };
    }
}

Q_DECLARE_METATYPE(Qtilities::Core::MultiContextProperty)
Q_DECLARE_METATYPE(Qtilities::Core::SharedProperty)

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::MultiContextProperty& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::MultiContextProperty& stream_obj);
QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::SharedProperty& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::SharedProperty& stream_obj);
QDebug operator<<(QDebug dbg, const Qtilities::Core::SharedProperty &prop);
QDebug operator<<(QDebug dbg, const Qtilities::Core::MultiContextProperty &prop);


#endif // QTILITIES_PROPERTY_H
