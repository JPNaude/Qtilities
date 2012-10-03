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

#ifndef IAVAILABLE_PROPERTY_PROVIDER_H
#define IAVAILABLE_PROPERTY_PROVIDER_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"

#include <IObjectBase>

#include <QVariant>
#include <QStringList>

namespace Qtilities {
    namespace CoreGui {
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
            }
            /*!
             * \brief Quick constructor for PropertySpecification with important property details as parameters.
             * \param displayed_name The displayed name that will be used in AddDynamicPropertyWizard.
             * \param description The description for the property.
             * \param type The type of property.
             * \param class_name The base className() to which this property is applicable. When empty, will be available for all classes.
             * \param property_name When empty, will use displayed_name as the property name.
             */
            PropertySpecification(const QString& displayed_name, const QString& description, QVariant::Type type, const QString& class_name = QString(), const QString& property_name = QString()) {
                d_displayed_name = displayed_name;
                d_property_name = property_name;
                d_description = description;
                d_type = type;
                d_class_name = class_name;

                d_removable = true;
                d_read_only = false;
            }
            PropertySpecification(const PropertySpecification& ref) {
                d_displayed_name = ref.d_displayed_name;
                d_property_name = ref.d_property_name;
                d_description = ref.d_description;
                d_type = ref.d_type;
                d_default_value = ref.d_default_value;
                d_removable = ref.d_removable;
                d_read_only = ref.d_read_only;
                d_class_name = ref.d_class_name;
            }
            PropertySpecification& operator=(const PropertySpecification& ref) {
                if (this==&ref) return *this;

                d_displayed_name = ref.d_displayed_name;
                d_property_name = ref.d_property_name;
                d_description = ref.d_description;
                d_type = ref.d_type;
                d_default_value = ref.d_default_value;
                d_removable = ref.d_removable;
                d_read_only = ref.d_read_only;
                d_class_name = ref.d_class_name;

                return *this;
            }
            bool operator==(const PropertySpecification& ref) {
                if (d_displayed_name != ref.d_displayed_name)
                    return false;
                if (d_property_name != ref.d_property_name)
                    return false;
                if (d_description != ref.d_description)
                    return false;
                if (d_default_value != ref.d_default_value)
                    return false;
                if (d_type != ref.d_type)
                    return false;
                if (d_removable != ref.d_removable)
                    return false;
                if (d_read_only != ref.d_read_only)
                    return false;
                if (d_class_name != ref.d_class_name)
                    return false;

                return true;
            }
            bool operator!=(const PropertySpecification& ref) {
                return !(*this==ref);
            }
            /*!
             * \brief Indicates if this property specification is valid, thus it has a displayed_name and a type.
             * \return True if valid, false otherwise.
             */
            bool isValid() const {
                if (d_displayed_name.isEmpty())
                    return false;
                if (d_type == QVariant::Invalid)
                    return false;

                return true;
            }

            QString d_displayed_name;
            QString d_property_name;
            QString d_description;
            QVariant::Type d_type;
            QVariant d_default_value;
            bool d_removable;
            bool d_read_only;
            QString d_class_name;
        };

        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class IAvailablePropertyProvider
            \brief Used by the AddDynamicPropertyWizard class to determine the available properties for a specific type of object.

            <i>This class was added in %Qtilities v1.2.</i>
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IAvailablePropertyProvider: virtual public IObjectBase
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

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IAvailablePropertyProvider,"com.Qtilities.CoreGui.IAvailablePropertyProvider/1.0");

#endif // IAVAILABLE_PROPERTY_PROVIDER_H
