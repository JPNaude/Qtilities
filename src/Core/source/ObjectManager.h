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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "IObjectManager.h"
#include "QtilitiesCore_global.h"
#include "IAvailablePropertyProvider.h"

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
            SubjectTypeInfo& operator=(const SubjectTypeInfo& ref) {
                if (this==&ref) return *this;

                d_meta_type = ref.d_meta_type;
                d_name = ref.d_name;

                return *this;
            }
            bool operator==(const SubjectTypeInfo& ref) const {
                if (d_meta_type != ref.d_meta_type)
                    return false;
                if (d_name != ref.d_name)
                    return false;

                return true;
            }
            bool operator!=(const SubjectTypeInfo& ref) const {
                return !(*this==ref);
            }

            QString d_meta_type;
            QString d_name;
        };

        /*!
          \class PropertyDiffInfo
          \brief The PropertyDiffInfo class is used to provide information of property differences between two objects.
         */
        class QTILIITES_CORE_SHARED_EXPORT PropertyDiffInfo {
        public:
            PropertyDiffInfo() {}
            PropertyDiffInfo(const PropertyDiffInfo& ref) {
                d_added_properties = ref.d_added_properties;
                d_removed_properties = ref.d_removed_properties;
                d_changed_properties = ref.d_changed_properties;
            }
            PropertyDiffInfo& operator=(const PropertyDiffInfo& ref) {
                if (this==&ref) return *this;

                d_added_properties = ref.d_added_properties;
                d_removed_properties = ref.d_removed_properties;
                d_changed_properties = ref.d_changed_properties;

                return *this;
            }
            bool operator==(const PropertyDiffInfo& ref) const {
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
            bool hasChanges() const {
                if (d_added_properties.count() > 0)
                    return true;
                if (d_removed_properties.count() > 0)
                    return true;
                if (d_changed_properties.count() > 0)
                    return true;
                return false;
            }
            bool operator!=(const PropertyDiffInfo& ref) const {
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
          \struct ObjectManagerPrivateData
          \brief The ObjectManagerPrivateData struct stores data used by the ObjectManager class.
         */
        struct ObjectManagerPrivateData;

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

            // -----------------------------------------
            // IFactoryProvider Implementation
            // -----------------------------------------
            QStringList providedFactories() const;
            QObject* createInstance(const InstanceFactoryInfo& ifactory_data);
            QStringList providedFactoryTags(const QString& factory_name, const QtilitiesCategory& category_filter = QtilitiesCategory(), bool* ok = 0) const;
            QtilitiesCategory categoryForTag(const QString& factory_name, const QString& factory_tag, bool* ok = 0) const;
            QMap<QString, QtilitiesCategory> tagCategoryMap(const QString& factory_name, bool* ok = 0) const;

            // --------------------------------
            // IObjectManager Implementation
            // --------------------------------
            Observer* observerReference(int id) const;             
            Observer* objectPool();
            int registerObserver(Observer* observer);
            int getNewIteratorID() const;
            bool moveSubjects(QList<QObject*> objects, int source_observer_id, int destination_observer_id, QString *error_msg, bool silent = false);
            bool moveSubjects(QList<QPointer<QObject> > objects, int source_observer_id, int destination_observer_id, QString *error_msg, bool silent = false);
            void registerObject(QObject* obj, QtilitiesCategory category = QtilitiesCategory());
            void removeObject(QObject* obj);
            void registerFactoryInterface(FactoryInterface<QObject>* factory_interface, FactoryItemID iface_tag);
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
            // Static / Dynamic Property Functions
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
            //! Convenience function which will create and set a SharedProperty from the given PropertySpecification.
            /*!
              \sa setMultiContextProperty(), getMultiContextProperty(), getSharedProperty(), propertyExists()

              This function was added in %Qtilities v1.2.
              */
            static bool setSharedProperty(QObject* obj, PropertySpecification property_specification);
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

            //! Convenience function to allow construction of all properties specified by IAvailablePropertyProvider interfaces in the global object pool on the given object.
            /*!
             *This function can be used to do automatic dynamic property construction on objects. This is usefull in
             *extendible applications where you use dynamic properties in order to extend objects at runtime. For example,
             *if you have an application and plugins can add dynamic properties on specific type of objects, you can do it by
             *having some sort of mechanism where the plugin is notified of new objects that it is interested in. One way to do it
             *is to register the new objects in the global object pool and listen to the new object added signal on the object pool.
             *When the plugin gets this notification it adds the needed dynamic properties on the object.
             *
             *This function provides a more efficient way to do this. When called, the function looks for all implementations of
             *IAvailablePropertyProvider registered in the global object pool and gets all PropertySpecification specifications for
             *for which the given object inherits the \p d_class_name specification of the property specification.
             *
             *It will check all these properties and if their \p d_add_during_construction parameter is set
             *it will add these properties with the parameters specified by the matching PropertySpecification.
             *
             *The intended use of this function is to call it in the constructor of your object.
             *
             *\param obj The object on which the properties must be constructed.ad.
             *\param errorMsg When the function returns false due to any issues, the reason for the failure can be found through this parameter.
             *
             *\returns True when successfull, false otherwise.
             *
             *This function was added in %Qtilities v1.2.
             */
            static bool constructDefaultPropertiesOnObject(QObject* obj, QString* errorMsg = 0);

            //! Convenience function to construct a property from a given PropertySpecification.
            /*!
             * \brief Constructs a SharedProperty from the given PropertySpecification.
             * \param specification The property specification
             * \return A reference to the constructed property.
             *
             *This function was added in %Qtilities v1.2.
             */
            static SharedProperty* constructPropertyFromSpecification(PropertySpecification specification);

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
