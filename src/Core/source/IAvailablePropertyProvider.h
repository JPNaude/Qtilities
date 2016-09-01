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
#ifndef IAVAILABLE_PROPERTY_PROVIDER_H
#define IAVAILABLE_PROPERTY_PROVIDER_H

#include "QtilitiesCore_global.h"
#include "QtilitiesCoreConstants.h"

#include <IObjectBase>

#include <QVariant>
#include <QStringList>

namespace Qtilities {
    namespace Core {
        /*!
          \struct PropertySpecification
          \brief The PropertySpecification structure is used to define an available property through the .

          The PropertySpecification structure defines a subject type in the context of an observer and is used by the
          Qtilities::CoreGui::Interfaces::IAvailablePropertyProvider interface.

          <i>This struct was added in %Qtilities v1.2.</i>
         */
        struct PropertySpecification {
        public:
            PropertySpecification() {
                d_removable = true;
                d_read_only = false;
                d_add_during_construction = false;
                d_internal = false;
                d_is_exportable = true;
            }
            /*!
             * \brief Quick constructor for PropertySpecification with important property details as parameters.
             * \param displayed_name The displayed name that will be used in AddDynamicPropertyWizard.
             * \param description The description for the property.
             * \param data_type The type of property.
             * \param class_name The base className() to which this property is applicable. When empty, will be available for all classes.
             * \param property_name When empty, will use displayed_name as the property name.
             */
            PropertySpecification(const QString& displayed_name, const QString& description, QVariant::Type data_type, const QString& class_name = QString(), const QString& property_name = QString()) {
                d_displayed_name = displayed_name;
                d_property_name = property_name;
                d_description = description;
                d_data_type = data_type;
                d_class_name = class_name;

                d_removable = true;
                d_read_only = false;
                d_add_during_construction = false;
                d_internal = false;
                d_is_exportable = true;
            }
            PropertySpecification(const PropertySpecification& ref) {
                d_displayed_name = ref.d_displayed_name;
                d_property_name = ref.d_property_name;
                d_description = ref.d_description;
                d_data_type = ref.d_data_type;
                d_default_value = ref.d_default_value;
                d_removable = ref.d_removable;
                d_read_only = ref.d_read_only;
                d_class_name = ref.d_class_name;
                d_add_during_construction = ref.d_add_during_construction;
                d_internal = ref.d_internal;
                d_is_exportable = ref.d_is_exportable;
            }
            PropertySpecification& operator=(const PropertySpecification& ref) {
                if (this==&ref) return *this;

                d_displayed_name = ref.d_displayed_name;
                d_property_name = ref.d_property_name;
                d_description = ref.d_description;
                d_data_type = ref.d_data_type;
                d_default_value = ref.d_default_value;
                d_removable = ref.d_removable;
                d_read_only = ref.d_read_only;
                d_class_name = ref.d_class_name;
                d_add_during_construction = ref.d_add_during_construction;
                d_internal = ref.d_internal;
                d_is_exportable = ref.d_is_exportable;

                return *this;
            }
            bool operator==(const PropertySpecification& ref) const {
                if (d_displayed_name != ref.d_displayed_name)
                    return false;
                if (d_property_name != ref.d_property_name)
                    return false;
                if (d_description != ref.d_description)
                    return false;
                if (d_default_value != ref.d_default_value)
                    return false;
                if (d_data_type != ref.d_data_type)
                    return false;
                if (d_removable != ref.d_removable)
                    return false;
                if (d_read_only != ref.d_read_only)
                    return false;
                if (d_class_name != ref.d_class_name)
                    return false;
                if (d_add_during_construction != ref.d_add_during_construction)
                    return false;
                if (d_internal != ref.d_internal)
                    return false;
                if (d_is_exportable != ref.d_is_exportable)
                    return false;

                return true;
            }
            bool operator!=(const PropertySpecification& ref) const {
                return !(*this==ref);
            }
            /*!
             * \brief Indicates if this property specification is valid, thus it has a displayed_name and a type.
             * \return True if valid, false otherwise.
             */
            bool isValid() const {
                if (d_displayed_name.isEmpty())
                    return false;
                if (d_data_type == QVariant::Invalid)
                    return false;

                return true;
            }
            //! Returns the property name to use. Use this instead of d_property_name directly.
            /*!
             *Function which check if d_property_name is valid and if so returns it, otherwise returns d_displayed_name.
             */
            QString propertyName() const {
                if (d_property_name.isEmpty())
                    return d_displayed_name;
                else
                    return d_property_name;
            }

            //! A display name for the property.
            QString d_displayed_name;
            //! The actual property name used by QObject::setProperty(). When empty, the displayed name will be used as the actual property name as well.
            QString d_property_name;
            //! A description of the property.
            QString d_description;
            //! The property's data type.
            QVariant::Type d_data_type;
            //! The default value of the property.
            QVariant d_default_value;
            //! Indicates if the property must be removable. True by default.
            bool d_removable;
            //! Indicates if the property must be read only. False by default.
            bool d_read_only;
            //! Indicates the class names to which this property applies. Checked with QObject::inherits().
            QString d_class_name;
            //! Indicates if the property must be added to objects that matches the specified class name by default. False by default.
            bool d_add_during_construction;
            //! Indicates if the property is internal, thus it will not be shown as an available property when users add properties to an object. False by default.
            bool d_internal;
            //! Indicates if the property is exportable. True by default.
            bool d_is_exportable;
        };

        namespace Interfaces {
            /*!
            \class IAvailablePropertyProvider
            \brief Used by the AddDynamicPropertyWizard class to determine the available properties for a specific type of object.

            <i>This class was added in %Qtilities v1.2.</i>
              */
            class QTILIITES_CORE_SHARED_EXPORT IAvailablePropertyProvider: virtual public IObjectBase
            {

            public:
                IAvailablePropertyProvider() {}
                virtual ~IAvailablePropertyProvider() {}

                //! Function which returns the available PropertySpecification items provided by this interface.
                virtual QList<PropertySpecification> availableProperties() const {
                    return QList<PropertySpecification>();
                }
                //! Function which returns all base classes on which custom properties should be prohibited according to this provider.
                virtual QStringList prohibitedCustomPropertiesClasses() const {
                    return QStringList();
                }
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IAvailablePropertyProvider,"com.Qtilities.Core.IAvailablePropertyProvider/1.0")

#endif // IAVAILABLE_PROPERTY_PROVIDER_H
