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

#ifndef ADD_DYNAMIC_PROPERTY_HANDLER_H
#define ADD_DYNAMIC_PROPERTY_HANDLER_H

#include <QtilitiesCoreApplication>

#include "QtilitiesCoreGui_global.h"
#include "IAvailablePropertyProvider.h"

#include <QWizard>
#include <QPointer>

using namespace Qtilities::Core;
using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace CoreGui {
        /*!
        \struct AddDynamicPropertyWizardPrivateData
        \brief The AddDynamicPropertyWizardPrivateData stores data used by the AddDynamicPropertyWizard class.
          */
        struct AddDynamicPropertyWizardPrivateData;

        /*!
        \class AddDynamicPropertyWizard
        \brief The AddDynamicPropertyWizard class handles addition of dynamic properties on an object.

        <i>This class was added in %Qtilities v1.2.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT AddDynamicPropertyWizard : public QWizard {
            Q_OBJECT

        public:
            //! Enumeration which describes the property creation modes of this wizard.
            /*!
              \sa propertyCreationHint()
              */
            enum PropertyCreationHint {
                ConstructAndAdd        = 1 << 1, /*!< Constructs the property and adds it to the object set using setObject(). */
                ConstructDoNotAdd      = 1 << 2  /*!< Constructs the property, but don't add it to the property to the object set using
                                                      setObject(). Instead, the constructed property can be obtained through constructedProperty(). */
            };
            Q_ENUMS(PropertyCreationHint)

            explicit AddDynamicPropertyWizard(PropertyCreationHint property_creation_hint = ConstructAndAdd, QWidget *parent = 0);
            virtual ~AddDynamicPropertyWizard();

            //! Returns the current PropertyCreationHint used by the wizard.
            PropertyCreationHint propertyCreationHint() const;
            //! Function to access the constructed property after the user accepted the dialog.
            /*!
             * \brief The constructed property variant can be obtained using this function. When newPropertyType() is ObjectManager::SharedProperties, this function
             * will return the QVariant set as the value of the SharedProperty, not the SharedProperty itself.
             */
            QVariant constructedProperty() const;

            void accept();
            bool validateCurrentPage();

            //! Sets what property type must be used when the user adds new properties.
            /*!
              \note Only ObjectManager::SharedProperties and ObjectManager::NonQtilitiesProperties types are allowed, when trying to set it to something else this function does nothing.
              */
            void setNewPropertyType(ObjectManager::PropertyTypes new_property_type);
            //! Gets what property type must be used when the user adds new properties.
            /*!
              \returns The property type used when the user adds new properties. Default is normal QVariants, thus ObjectManager::NonQtilitiesProperties.
              */
            ObjectManager::PropertyTypes newPropertyType() const;

            //! Gets the name of the property to be used when adding it.
            QString newPropertyName() const;
            //! Gets the type for the given displayed property name. If displayed_name is empty, the current selected displayed name is used.
            QVariant::Type propertyType(QString displayed_name = QString()) const;

            //! Sets if this wizard should warn you if conflicting new property name warnings should be shown.
            void setWarnAboutDuplicateWarnings(bool enable);
            //! Gets if this wizard should warn you if conflicting new property name warnings should be shown.
            /*!
             * Enabled by default.
             */
            bool warnAboutDuplicateWarnings() const;

        public slots:
            //! Sets the object on which the property must be added.
            /*!
              \param obj The object which must be used.
              */
            void setObject(QObject* obj);
            //! Sets the object on which the property must be added.
            /*!
              \param obj The object which must be used.
              */
            void setObject(QPointer<QObject> obj);
            //! Sets the object on which the property must be added.
            /*!
              \param objects A list of objects. When the list contains 1 item, it will be used in this widget.
              */
            void setObject(QList<QObject*> objects);
            //! Sets the object by providing a list of smart pointers.
            void setObject(QList<QPointer<QObject> > objects);
            //! Handles the event where the current object is destroyed.
            void handleObjectDestroyed();

            //! Gets the removability for the given displayed property name.
            bool propertyRemovable(const QString& displayed_name) const;
            //! Gets if the property is exportable for the given displayed property name.
            bool propertyIsExportable(const QString& displayed_name) const;
            //! Gets the read only state for the given displayed property name.
            bool propertyReadOnly(const QString& displayed_name) const;
            //! Gets the default value for the given displayed property name.
            QVariant propertyDefaultValue(const QString& displayed_name) const;

        private slots:
            void handleSelectedPropertyChanged(const QString& property_displayed_name);
            void handleCustomPropertyNameChanged(const QString& name);
            void handleCurrentIdChanged();

        private:
            //! Validates the name to be used for the new property.
            bool validateNewPropertyName(const QString& property_name);
            //! Gets the name of the currently selected item in the available property list.
            QString selectedAvailableProperty() const;
            //! Initializes the available properties for the given object.
            void initializePropertyProviders();
            //! Gets all provided property specifications for the given object.
            /*!
             * Its not neccessarry to call this function directly, it will be called when setting the object using setObject().
             */
            void getAvailableProperties() const;
            //! Gets if custom properties are prohibited for the given class name.
            bool prohibitCustomProperties() const;
            //! Gets the names of all available properties.
            QStringList availablePropertyDisplayedNames() const;
            //! Gets the description for the given displayed property name.
            QString propertyDescription(const QString& displayed_name) const;
            //! Gets the actual property name for the given displayed property name.
            QString propertyName(const QString& displayed_name) const;
            //! Indicates if the current property selection is the custom property option.
            bool isCurrentCustomProperty() const;

            AddDynamicPropertyWizardPrivateData* d;
        };
    }
}

#endif // ADD_DYNAMIC_PROPERTY_HANDLER_H
