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

#ifndef GENERIC_PROPERTY_MANAGER_H
#define GENERIC_PROPERTY_MANAGER_H

#include <QObject>

#include "QtilitiesCore_global.h"

#include <IExportable>
#include <IModificationNotifier>
#include <QtilitiesCategory>
#include <Factory>
#include <Observer>
#include <GenericProperty>

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core;
        using namespace Qtilities::Core::Interfaces;

        /*!
        \struct GenericPropertyManagerData
        \brief A structure storing private data in the GenericPropertyManager class.
          */
        struct GenericPropertyManagerData;

        /*!
        \class GenericPropertyManager
        \brief A class that manages a set of GenericProperty properties.

        <i>This class was added in %Qtilities v1.5.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT GenericPropertyManager : public QObject, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
            //! Constructs a GenericPropertyManager object.
            explicit GenericPropertyManager(QObject *parent = 0);
            ~GenericPropertyManager();

            // --------------------------------
            // GenericPropertyManager Implementation
            // --------------------------------
            //! Returns the properties file loaded for this build step, if any.
            QString propertiesFileName() const;

            //! Clears all the properties in the property manager.
            /*!
             * \note This function does not clear properties with category qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL or BUILD_CONFIGURATION_PROP_CATEGORY_STEP_CONTROL.
             */
            void clear();
            //! Access function to the observer managing properties.
            Observer* propertiesObserver() const;
            //! Checks if a specific property is present.
            /*!
              \note A case insensitive comparison is done on property names.
              */
            GenericProperty* containsProperty(const QString& property_name, bool search_aliases = false, const QString& alias_environment = QString()) const;
            //! Returns all properties in this manager.
            /*!
              \param only_editable Only editable properties should be returned.
              \param only_non_default Only non default properties should be returned.
              */
            QList<GenericProperty*> allProperties(bool only_editable = false, bool only_non_default = false) const;
            //! Returns all propertiess in this manager, filtered by category.
            /*!
              \param filter_category A filter category applied. When invert_filter is false, all properties matching the given category are returned. When its true, all properties NOT matching the given category are returned.
              \param invert_filter Indicates if the filter_category must be applied as an inverted filter.
              */
            QList<GenericProperty*> allProperties(const QtilitiesCategory& filter_category, bool invert_filter = false) const;
            //! Checks if this property manager has any Advanced properties.
            /*!
              If no advanced properties are present, the "Show Advanced Settings" button will not be visible in the details widget.
              */
            bool hasAdvancedSettings() const;
            //! Gets if advanced settings must be shown for this property manager.
            bool showAdvancedSettings() const;
            //! Sets if advanced settings must be shown for this property manager.
            void setShowAdvancedSettings(bool show);

            //! Gets if switch names must be shown for this property manager.
            bool showSwitchNames() const;
            //! Sets if switch names must be shown for this property manager.
            void setShowSwitchNames(bool show);

            //! Checks if this property manager has any properties that does not match their default values.
            /*!
              \param modified_property_list When passed a valid reference, the string list will be populated with the details of all modified properties.
              */
            bool hasModifiedProperties(QStringList* modified_property_list = 0) const;

        signals:
            //! Signal which is emitted when showing/hiding advanced settings is changed/toggled.
            void toggleAdvancedSettings(bool show);
            //! Signal which is emitted when showing/hiding switch names is changed/toggled.
            void toggleSwitchNames(bool show);
            //! Emitted when changes are made to the value of any property managed by this property manager.
            void propertyValueChanged(GenericProperty* property);
            //! Emitted when changes are made to the editabililty of any property managed by this property manager.
            void propertyEditableChanged(GenericProperty* property);
            //! Emitted when changes are made to the design dependency of any property managed by this property manager.
            void propertyContextDependentChanged(GenericProperty* property);
            //! Emitted when changes are made to the possible values any property managed by this property manager.
            void propertyPossibleValuesChanged(GenericProperty* property);
            //! Emitted when changes are made to the default value of any property managed by this property manager.
            void propertyDefaultValueChanged(GenericProperty* property);
            //! Emitted when changes are made to the note of any property managed by this property manager.
            void propertyNoteChanged(GenericProperty* property);
            //! Requests a full refresh in any property browsers showing this manager.
            void refresh();

        public:
            //! Adds a property. If a property with the same name already exists, this function returns the existing property.
            /*!
             * \param property_name The name of the property to add.
             * \param value The value of the property to add.
             * \param refresh Indicates if the property browser must be refreshed after the property was added.
             * \return The added property.
             */
            GenericProperty* addProperty(const QString& property_name, QVariant value = QVariant(), bool refresh_browser = true);
            //! Adds an existing property.
            /*!
             * \param property The property to add.
             * \param refresh Indicates if the property browser must be refreshed after the property was added.
             * \return True when successfull, false otherwise.
             */
            bool addProperty(GenericProperty* property, bool refresh_browser = true);
            //! Adds all properties in a list.
            void addProperties(QList<GenericProperty*> properties);
            //! Adds all properties in a list with safe pointers.
            void addProperties(QList<QPointer<GenericProperty> > properties);
            //! Removes a property. If a property with the name does not exists, this function does nothing and returns false.
            /*!
             * \param property_name The name of the property to remove.
             * \param refresh Indicates if the property browser must be refreshed after the property was added.
             * \return True when successfull, false otherwise.
             */
            bool removeProperty(const QString& property_name, bool refresh_browser = true);

            //! Sets the value of the specified property.
            /*!
              This function will inspect the found property to determine its type and check if the variant type of value matches the type of the property. If it
    does not match, the function will fail.
              */
            bool setPropertyValue(const QString& property_name, QVariant value);
            //! Sets the value of the specified property.
            /*!
              This function will inspect the found property to determine its type and convert the value to the correct type.
              */
            bool setPropertyValueString(const QString& property_name, const QString& value);
            //! Gets the value of the specified property in the form of a QVariant.
            QVariant getPropertyValue(const QString& property_name) const;
            //! Gets the value of the specified property in the form of a QString.
            QString getPropertyValueString(const QString& property_name) const;

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // Saving and Loading
            // --------------------------------
            //! Loads the default set of properties from the given file.
            Qtilities::Core::Interfaces::IExportable::ExportResultFlags loadDefaultProperties(const QString& file_name, ITask* task_ref = 0, bool add_property_file_property = false);
            //! Loads properties from a new file and updates all existing properties to match them.
            /*!
              This function will:
              - Make a clone of all current properties.
              - Clear the internal property storage.
              - Load the base set of properties from the specified file.
              - Try to match all properties from the clone to new properties from the specified file, and if found update their values.
              - Finally it will emit reloadPropertyBrowsersRequest() in order for property browsers to reload themselves.
              */
            virtual Qtilities::Core::Interfaces::IExportable::ExportResultFlags loadNewPropertiesFile(const QString& file_name, ITask* task_ref = 0);
            //! Exports all properties which does not match their default state to the given QDomDocument.
            /*!
              \note This function excludes any properties with their category set to qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL.
              */
            IExportable::ExportResultFlags exportManagerProperties(QDomDocument* doc,
                                                                   QDomElement* object_node,
                                                                   bool export_non_default = true,
                                                                   bool export_default = false,
                                                                   const QString &export_node_name = "generic_properties") const;
            //! Imports all properties which does not match their default state from the given QDomDocument.
            /*!
              This function should be called after loadDefaultProperties(). It will load all changed properties and change the
              matching properties in the default set of properties to matches the imported, changed properties.

              \note This function will not import any macros. Use importMacros() for that.
              */
            IExportable::ExportResultFlags importManagerProperties(QDomDocument* doc,
                                                                   QDomElement* object_node,
                                                                   const QString &export_node_name = "generic_properties");

            //! Exports all macros matching the given macro mode to the given QDomDocment.
            IExportable::ExportResultFlags exportMacros(GenericProperty::MacroMode macro_mode,QDomDocument* doc, QDomElement* object_node);
            //! Imports macros from given QDomDocument.
            /*!
                This function will automatically set the macros to macro mode to macro_mode.
            */
            IExportable::ExportResultFlags importMacros(GenericProperty::MacroMode macro_mode, QDomDocument* doc, QDomElement* object_node);
            //! Returns a QHash with macro values. Keys = macro name, Values = macro value.
            QHash<QString,QString> macroValues(GenericProperty::MacroMode macro_mode = GenericProperty::MacrosAll);
            //! Returns a list with all macro properties.
            QList<GenericProperty*> macroProperties(GenericProperty::MacroMode macro_mode = GenericProperty::MacrosAll);

            // --------------------------------
            // Interaction between different property managers:
            // --------------------------------
            //! Clones properties from a different property manager.
            virtual void clone(GenericPropertyManager* property_manager, bool only_state_dependent_properties = true);
            //! Compares the properties in this manager with the properties in a different manager.
            /*!
             * \brief compare
             * \param property_manager The property manager containing the properties to be compared.
             * \param property_diff_info When valid, will be populated with the details of the differences.
             * \return True when identical, false otherwise.
             */
            virtual bool compare(GenericPropertyManager* property_manager,
                                 PropertyDiffInfo* property_diff_info = 0,
                                 bool state_independent_properties = false,
                                 bool context_dependent_properties = true) const;

            //----------------------------
            // Tasking
            //----------------------------
            //! Sets the task which must be used to log import/export information to.
            /*!
              To remove the task, use clearExportTask()

              \sa exportTask(), clearExportTask()
              */
            void setExportTask(ITask* task);
            //! Gets the task which must be used to log import/export information to.
            /*!

              \sa setExportTask(), clearExportTask()
              */
            ITask* exportTask() const;
            //! Clears the export task.
            /*!
              \sa setExportTask(), exportTask()
              */
            void clearExportTask();

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            virtual bool isModified() const;
        public slots:
            virtual void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            /*!
              The manager is marked as modified when:
              - The value of a property changes.
              - The editability of a property changes.
              - The list of properties changes.
              */
            void modificationStateChanged(bool is_modified) const;
            //! A request that GenericPropertyBrowsers showing properties for this property manager must reload themselves.
            void reloadPropertyBrowsersRequest();

        private:
            //! Connects to a property.
            void connectToProperty(GenericProperty* property);

            GenericPropertyManagerData* d;
        };
    }
}

#endif // GENERIC_PROPERTY_MANAGER_H

