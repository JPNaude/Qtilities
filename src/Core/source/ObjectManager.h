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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "IObjectManager.h"
#include "QtilitiesCore_global.h"

#include <QList>
#include <QStringList>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;
        class Observer;

        /*!
          \struct SubjectTypeInfo
          \brief The SubjectTypeInfo structure is used to define subject types.

          The SubjectTypeInfo structure defines a subject type in the context of an observer and is used by the
          Qtilities::Core::SubjectTypeFilter to filter subject types within an observer context.

          \note The d_meta_type property property must match the metaObject->className() string of the QObject.
         */
        struct SubjectTypeInfo {
        public:
            SubjectTypeInfo() {}
            SubjectTypeInfo(QString meta_type, QString name = QString()) {
                d_meta_type = meta_type;
                d_name = name;
            }
            SubjectTypeInfo(const SubjectTypeInfo& ref) {
                d_meta_type = ref.d_meta_type;
                d_name = ref.d_name;
            }
            void operator=(const SubjectTypeInfo& ref) {
                d_meta_type = ref.d_meta_type;
                d_name = ref.d_name;
            }
            bool operator==(const SubjectTypeInfo& ref) {
                if (d_meta_type != ref.d_meta_type)
                    return false;
                if (d_name != ref.d_name)
                    return false;

                return true;
            }
            bool operator!=(const SubjectTypeInfo& ref) {
                return !(*this==ref);
            }

            QString d_meta_type;
            QString d_name;
        };

        /*!
          \class PropertyDiffInfo
          \brief The PropertyDiffInfo structure is used to define provide information of property differences between two objects.
         */
        class QTILIITES_CORE_SHARED_EXPORT PropertyDiffInfo {
        public:
            PropertyDiffInfo() {}
            PropertyDiffInfo(const PropertyDiffInfo& ref) {
                d_added_properties = ref.d_added_properties;
                d_removed_properties = ref.d_removed_properties;
                d_changed_properties = ref.d_changed_properties;
            }
            void operator=(const PropertyDiffInfo& ref) {
                d_added_properties = ref.d_added_properties;
                d_removed_properties = ref.d_removed_properties;
                d_changed_properties = ref.d_changed_properties;
            }
            bool operator==(const PropertyDiffInfo& ref) {
                if (d_added_properties != ref.d_added_properties)
                    return false;
                if (d_removed_properties != ref.d_removed_properties)
                    return false;
                if (d_changed_properties != ref.d_changed_properties)
                    return false;

                return true;
            }
            void clear() {
                d_added_properties.clear();
                d_removed_properties.clear();
                d_changed_properties.clear();
            }
            bool hasChanges() {
                if (d_added_properties.count() > 0)
                    return true;
                if (d_removed_properties.count() > 0)
                    return true;
                if (d_changed_properties.count() > 0)
                    return true;
                return false;
            }

            bool operator!=(const PropertyDiffInfo& ref) {
                return !(*this==ref);
            }

            //! Added properties: Key = Property Name, Value = Property Value.
            QMap<QString,QString> d_added_properties;
            //! Removed properties: Key = Property Name, Value = Old Property Value.
            QMap<QString,QString> d_removed_properties;
            //! Changed properties: Key = Property Name, Value = "Old Property Value","New Property Value"
            QMap<QString,QString> d_changed_properties;
        };

        /*!
          \class ObjectManagerPrivateData
          \brief The ObjectManagerPrivateData class stores data used by the ObjectManager class.
         */
        class ObjectManagerPrivateData;

        /*!
          \class ObjectManager
          \brief The ObjectManager provides object management features to the developer.

          For more information about object management in %Qtilities, see the \ref page_object_management article.
         */
        class QTILIITES_CORE_SHARED_EXPORT ObjectManager : public IObjectManager
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IObjectManager)
            Q_INTERFACES(Qtilities::Core::Interfaces::IFactoryProvider)
            friend class Observer;

        public:
            ObjectManager(QObject* parent = 0);
            ~ObjectManager();

        public:
            // -----------------------------------------
            // IFactoryProvider Implementation
            // -----------------------------------------
            QStringList providedFactories() const;
            QObject* createInstance(const InstanceFactoryInfo& ifactory_data);
            QStringList providedFactoryTags(const QString& factory_name, const QtilitiesCategory& category_filter = QtilitiesCategory()) const;

            // --------------------------------
            // IObjectManager Implemenation
            // --------------------------------
            Observer* observerReference(int id) const;             
            Observer* objectPool();
            int registerObserver(Observer* observer);
            bool moveSubjects(QList<QObject*> objects, int source_observer_id, int destination_observer_id, QString *error_msg, bool silent = false);
            bool moveSubjects(QList<QPointer<QObject> > objects, int source_observer_id, int destination_observer_id, QString *error_msg, bool silent = false);
            void registerObject(QObject* obj, QtilitiesCategory category = QtilitiesCategory());
            void removeObject(QObject* obj);
            void registerFactoryInterface(FactoryInterface<QObject>* interface, FactoryItemID iface_tag);
            bool registerIFactoryProvider(IFactoryProvider* factory_iface);
            IFactoryProvider* referenceIFactoryProvider(const QString& factory_name) const;
            QStringList allFactoryNames() const;
            QStringList tagsForFactory(const QString& factory_name) const;
            QList<QObject*> registeredInterfaces(const QString& iface) const;
            QList<QPointer<QObject> > metaTypeActiveObjects(const QString& meta_type) const;
            void setMetaTypeActiveObjects(QList<QObject*> objects, const QString& meta_type);
            void setMetaTypeActiveObjects(QList<QPointer<QObject> > objects, const QString& meta_type);

            // --------------------------------
            // Conversion Functions
            // --------------------------------
            //! Converts a QList<QPointer<QObject> > object list to a QList<QObject*> list.
            /*!
              This function was added in %Qtilities v1.1.
              */
            static QList<QObject*> convSafeObjectsToNormal(QList<QPointer<QObject> > safe_list);
            //! Converts a QList<QObject*> object list to a QList<QPointer<QObject> > list.
            /*!
              This function was added in %Qtilities v1.1.
              */
            static QList<QPointer<QObject> > convNormalObjectsToSafe(QList<QObject*> normal_list);

            // --------------------------------
            // Static Dynamic Property Functions
            // --------------------------------
            //! Streams exportable dynamic properties about the object to the given QDataStream.
            /*!
              To use this function make sure that all the QVariant properties have the streaming << / >> operators overloaded.
              This is the case for all properties used in %Qtilities.
              */
            static IExportable::ExportResultFlags exportObjectPropertiesBinary(const QObject* obj, QDataStream& stream, PropertyTypeFlags property_types = AllPropertyTypes, Qtilities::ExportVersion version = Qtilities::Qtilities_Latest);
            //! Streams exportable dynamic properties from the given data stream and add them to the QObject.
            /*!
              \note Conflicting properties will be replaced.
              */
            static IExportable::ExportResultFlags importObjectPropertiesBinary(QObject* obj, QDataStream& stream);
            //! Exports all exportable dynamic properties about the object to the given QDomDocument and QDomElement.
            /*!
              To use this function make sure that all the QVariants are exportable. See QtilitiesProperty::isExportableVariant() for more information.
              */
            static IExportable::ExportResultFlags exportObjectPropertiesXml(const QObject* obj, QDomDocument* doc, QDomElement* object_node, PropertyTypeFlags property_types = AllPropertyTypes, Qtilities::ExportVersion version = Qtilities::Qtilities_Latest);
            //! Streams exportable dynamic properties from the given QDomDocument and QDomElement and then add them to the QObject.
            /*!
              \note Conflicting properties will be replaced.
              */
            static IExportable::ExportResultFlags importObjectPropertiesXml(QObject* obj, QDomDocument* doc, QDomElement* object_node);
            //! Streams exportable dynamic properties from the given QDomDocument and QDomElement and then add them to the QObject.
            /*!
              \note Conflicting properties will be replaced.
              \sa removeDynamicProperties().
              */
            static bool cloneObjectProperties(const QObject* source_obj, QObject* target_obj, PropertyTypeFlags property_types = AllPropertyTypes);
            //! Convenience function which will get the specified MultiContextProperty of the specified object.
            /*!
              \sa setMultiContextProperty(), setSharedProperty(), getSharedProperty(), propertyExists()
              */
            static MultiContextProperty getMultiContextProperty(const QObject* obj, const char* property_name);
            //! Convenience function which will set the specified MultiContextProperty on the specified object.
            /*!
              Caution should be taken when using this function because you can easily overwrite property values for other
              contexts since the property has different values for different contexts.

              Therefore you must always check if an observer property exist before setting as shown in the example below:
\code
QtilitiesCategory category("Test Category");
// Check if the property exists:
if (ObjectManager::propertyExists(iface->objectBase(),qti_prop_CATEGORY_MAP)) {
    // If it does we MUST append the value for our context:
    MultiContextProperty category_property = ObjectManager::getMultiContextProperty(iface->objectBase(),qti_prop_CATEGORY_MAP);
    category_property.setValue(qVariantFromValue(category),observerID());
    ObjectManager::setMultiContextProperty(iface->objectBase(),category_property);
} else {
    // If not we create a new property with the value for our context:
    MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
    category_property.setValue(qVariantFromValue(category),observerID());
    ObjectManager::setMultiContextProperty(iface->objectBase(),category_property);
}
\endcode

                \sa getMultiContextProperty(), setSharedProperty(), getSharedProperty(), propertyExists()
              */
            static bool setMultiContextProperty(QObject* obj, MultiContextProperty multi_context_property);
            //! Convenience function which will get the specified SharedProperty of the specified object.
            /*!
              \sa setMultiContextProperty(), setSharedProperty(), getMultiContextProperty(), propertyExists()
              */
            static SharedProperty getSharedProperty(const QObject* obj, const char* property_name);
            //! Convenience function which will set the specified SharedProperty on the specified object.
            /*!
              \sa setMultiContextProperty(), getMultiContextProperty(), getSharedProperty(), propertyExists()
              */
            static bool setSharedProperty(QObject* obj, SharedProperty shared_property);
            //! Convenience function which will create and set a SharedProperty with the given name and value on the object.
            /*!
              \sa setMultiContextProperty(), getMultiContextProperty(), getSharedProperty(), propertyExists()
              */
            static bool setSharedProperty(QObject* obj, const char* property_name, QVariant property_value);
            //! Convenience function to check if a dynamic property exists on a object.
            static bool propertyExists(const QObject* obj, const char* property_name);
            //! Convenience function to remove all properties that match the PropertyTypeFlags from an object.
            static bool removeDynamicProperties(QObject* obj, PropertyTypeFlags property_types = AllPropertyTypes);
            //! Convenience function to compare all properties that match the PropertyTypeFlags on two objects.
            /*!
              This function checks each property using the == overload of the QVariant property type and returns true if they match exactly, false otherwise.

              \param obj1 The first object to use in the comparison. The results will be relative to this object, for example if a property exists on \p obj1 and not on \p obj2, the diff result will show that the property was added. Also, when a property exists on both objects and the value changed, the old value will be the value on \p obj2 and the new value the value on \p obj1.
              \param obj2 The second object to use in the comparison.
              \param property_types The property types which must be compared.
              \param property_diff_info Information about property changes on obj1 compared to obj2. By default the differences will not be calculated, it will only be done when the \p property_diff_info parameter is passed a valid reference.
              \param ignore_list A list of property names which should be ignored in the comparison.
              */
            static bool compareDynamicProperties(const QObject* obj1, const QObject* obj2, PropertyTypeFlags property_types = AllPropertyTypes, PropertyDiffInfo* property_diff_info = 0, QStringList ignore_list = QStringList());

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            ObjectManagerPrivateData* d;
        };
    }
}

#endif // OBJECTMANAGER_H
