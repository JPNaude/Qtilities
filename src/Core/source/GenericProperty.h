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

#ifndef GENERIC_PROPERTY_H
#define GENERIC_PROPERTY_H

#include <QObject>

#include "QtilitiesCore_global.h"

#include <IExportable>
#include <IModificationNotifier>
#include <QtilitiesCategory>
#include <Factory>

namespace Qtilities {
    namespace Core {

        using namespace Qtilities::Core;
        using namespace Qtilities::Core::Interfaces;
        class BuildConfiguration;

        /*!
          \struct PropertyAlias
          \brief The PropertyAlias structure is used to define an alias for a GenericProperty.
         */
        struct PropertyAlias {
        public:
            PropertyAlias(const QString& name = QString(), const QString& environment = QString(), bool inverted = false) {
                d_name = name;
                d_environment = environment;
                d_inverted = inverted;
            }
            PropertyAlias(const PropertyAlias& ref) {
                d_name = ref.d_name;
                d_environment = ref.d_environment;
                d_inverted = ref.d_inverted;
            }
            PropertyAlias& operator=(const PropertyAlias& ref) {
                if (this==&ref) return *this;

                d_name = ref.d_name;
                d_environment = ref.d_environment;
                d_inverted = ref.d_inverted;

                return *this;
            }
            bool operator==(const PropertyAlias& ref) const {
                if (d_name != ref.d_name)
                    return false;
                if (d_environment != ref.d_environment)
                    return false;
                if (d_inverted != ref.d_inverted)
                    return false;

                return true;
            }
            bool operator!=(const PropertyAlias& ref) const {
                return !(*this==ref);
            }

            //! The name of the alias.
            QString             d_name;
            //! The environment in which the alias is used, if any.
            QString             d_environment;
            //! Indicates if the alias represents an inverted alias.
            /*!
              Only applicable to TypeBool properties. To specify an inverted alias, start its name with the - character in the build step's property file
              */
            bool                d_inverted;
        };

        /*!
        \struct GenericPropertyData
        \brief A structure storing private data in the GenericProperty class.
          */
        struct GenericPropertyData;

        /*!
        \class GenericProperty
        \brief A generic property.

        <i>This class was added in %Qtilities v1.5.</i>
          */
        class QTILIITES_CORE_SHARED_EXPORT GenericProperty : public QObject, public IExportable, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
            //! Possible property types.
            enum PropertyType {
                TypeUndefined = 0,
                TypeInteger = 1,
                TypeString = 2,
                TypeEnum = 4,
                TypeBool = 8,
                TypeFile = 16,
                TypeFileList = 32,
                TypePath = 64,
                TypePathList = 128,
                TypeVariant = 256,  /*!< Note that a variant's property values are never exported, displayed and can only be accessed through value() and setValue(). Variant property automatically have their category set to qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL. */
                TypeDouble = 512,
                TypeListBased = TypeFileList | TypePathList
            };
            Q_ENUMS(PropertyType)
            //! Possible property levels.
            enum PropertyLevel {
                LevelStandard = 0,
                LevelAdvanced = 1
            };
            Q_ENUMS(PropertyLevel)
            //! Macro mode of this property.
            /*!
              Default is MacroUnexpanded.

              The macro mode defines if a macro must be used in an expanded state or not.
              */
            enum MacroMode {
                MacroUnexpanded     = 1,    /*!< This macro's value is still unexpanded, thus a design has not been set on the configuration. */
                MacroExpanded       = 2,    /*!< This macro's value is expanded, thus a design has been set on the configuration. */
                MacroExpandedCustom = 4,     /*!< This macro's value is expanded with a custom value, thus a design has been set on the configuration but the macro has been overwritten. It will be saved to build configuration files. */
                MacroExpandedAll = MacroExpanded | MacroExpandedCustom,
                MacrosAll = MacroExpandedAll | MacroUnexpanded
            };
            Q_ENUMS(MacroMode)

            static QString propertyTypeToString(PropertyType property_type) {
                if (property_type == TypeInteger) {
                    return "Int";
                } else if (property_type == TypeDouble) {
                    return "Double";
                } else if (property_type == TypeString) {
                    return "String";
                } else if (property_type == TypeEnum) {
                    return "Enum";
                } else if (property_type == TypeBool) {
                    return "Bool";
                } else if (property_type == TypeFile) {
                    return "File";
                } else if (property_type == TypeFileList) {
                    return "FileList";
                } else if (property_type == TypePath) {
                    return "Path";
                } else if (property_type == TypePathList) {
                    return "PathList";
                } else if (property_type == TypeVariant) {
                    return "Variant";
                }

                return QString();
            }
            static QVariant::Type propertyTypeQVariantType(PropertyType property_type) {
                if (property_type == TypeInteger) {
                    return QVariant::Int;
                } else if (property_type == TypeDouble) {
                    return QVariant::Double;
                } else if (property_type == TypeString) {
                    return QVariant::String;
                } else if (property_type == TypeEnum) {
                    return QVariant::StringList;
                } else if (property_type == TypeBool) {
                    return QVariant::Bool;
                } else if (property_type == TypeFile) {
                    return QVariant::String;
                } else if (property_type == TypeFileList) {
                    return QVariant::String;
                } else if (property_type == TypePath) {
                    return QVariant::String;
                } else if (property_type == TypePathList) {
                    return QVariant::String;
                } else if (property_type == TypeVariant) {
                    return QVariant::UserType;
                }

                return QVariant::String;
            }
            static PropertyType stringToPropertyType(const QString& property_type_string)  {
                if (property_type_string == QLatin1String("Int")) {
                    return TypeInteger;
                } else if (property_type_string == QLatin1String("Double")) {
                    return TypeDouble;
                } else if (property_type_string == QLatin1String("String")) {
                    return TypeString;
                } else if (property_type_string == QLatin1String("Enum")) {
                    return TypeEnum;
                } else if (property_type_string == QLatin1String("Bool")) {
                    return TypeBool;
                } else if (property_type_string == QLatin1String("File")) {
                    return TypeFile;
                } else if (property_type_string == QLatin1String("FileList")) {
                    return TypeFileList;
                } else if (property_type_string == QLatin1String("Path")) {
                    return TypePath;
                } else if (property_type_string == QLatin1String("PathList")) {
                    return TypePathList;
                } else if (property_type_string == QLatin1String("Variant")) {
                    return TypeVariant;
                } else if (property_type_string.isEmpty()) {
                    return TypeString;
                }

                qDebug() << "Got invalid property type" << property_type_string;
                Q_ASSERT(0);
                return TypeString;
            }
            static QString propertyLevelToString(PropertyLevel property_level) {
                if (property_level == LevelStandard) {
                    return "Standard";
                } else if (property_level == LevelAdvanced) {
                    return "Advanced";
                }

                return QString();
            }
            static PropertyLevel stringToPropertyLevel(const QString& property_level_string) {
                if (property_level_string == QLatin1String("Standard")) {
                    return LevelStandard;
                } else if (property_level_string == QLatin1String("Advanced")) {
                    return LevelAdvanced;
                } else if (property_level_string.isEmpty()) {
                    return LevelStandard;
                }

                qDebug() << "Got invalid property level" << property_level_string;
                Q_ASSERT(0);
                return LevelStandard;
            }

            //! Possible formats for string lists.
            enum FileListStringFormat {
                FileListStringFormat_0 = 0      /*!< Files are returned in this format (it does not care if there are spaces in the files): {"file_path_1" "file_path_2"} */
            };
            Q_ENUMS(FileListStringFormat)

            explicit GenericProperty(QObject *parent = 0);
            explicit GenericProperty(const QString& property_name, QObject *parent = 0);
            ~GenericProperty();

            GenericProperty(const GenericProperty& ref);
            GenericProperty& operator=(const GenericProperty& ref);
            bool operator==(const GenericProperty& ref);
            bool operator!=(const GenericProperty& ref);

            // --------------------------------
            // GenericProperty Implementation
            // --------------------------------
            //! Gets the property name.
            QString propertyName() const;
            //! Sets the property name.
            void setPropertyName(const QString& property_name);
            //! Returns the internal alias map with the keys representing the alias names, and the values the corresponding PropertyAlias objects.
            QMap<QString,PropertyAlias> aliases() const;
            //! Returns all aliases for this proprety.
            QStringList aliasNames(const QString& alias_environment = QString()) const;
            //! Returns the PropertyAlias struct containing information for the specified alias.
            PropertyAlias alias(const QString& alias, bool* ok = 0) const;
            //! Checks if a string matches the propert name or any of its aliases.
            bool matchesPropertyName(const QString& match_name, bool search_aliases = false, const QString& alias_environment = QString(), Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

            //! Gets the value string of the property.
            /*!
              \note If the value is a list, it is always separated usign listBasedSeparator().
              \note When a boolean value, case insensitive variantions of "true" and "false" are used for both set and get sides.
              */
            QString valueString() const;
            //! Sets the value string of the property.
            /*!
              This function will validate the specified value string according to the type of the property. If the property was updates successfully, we return true, false otherwise.
              */
            bool setValueString(const QString& value, QString* errorMsg = 0);
            //! Sets the value of this property to match the value of a different property.
            bool setValueFromProperty(GenericProperty* property);
            //! Compares the value of this property with that of another property.
            /*!
             * Note that the == overload of GenericProperty not only checks the value. It checks all
             * parameters of the property. To just compare the value, use this property.
             **/
            bool compareValue(GenericProperty* property);
            //! Gets the QVariant value of the property, in the correct type.
            /*!
              \note If the value is a list, it is always seperated usign listBasedSeparator().
              */
            QVariant value() const;
            //! Sets the QVariant value of the property.
            bool setValue(const QVariant& value, QString* errorMsg = 0);

            //! Gets the type of property.
            PropertyType type() const;
            //! Sets the type of property.
            /*!
              Make sure to set the type before setting the value.
              */
            void setType(PropertyType type);
            //! Gets the QVariant::Type of property.
            QVariant::Type qVariantType() const;
            //! Gets the category of the property.
            QtilitiesCategory category() const;
            //! Sets the category of the property.
            /*!
             * \note When setting the category to qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL this function will make this property not exportable automatically.
             */
            void setCategory(QtilitiesCategory category);
            //! Gets the default value string of the property.
            QString defaultValueString() const;
            //! Sets the default value string of the property.
            /*!
             * \note This function does validation of the value parameter. If the value is invalid, the default
             * valid will not be set. For example, if an invalid integer is set for a TypeInteger propery, the function
             * will return false and an appropriate errorMsg will be set.
             */
            bool setDefaultValueString(const QString& value, QString* errorMsg = 0);
            //! Sets the default value of the property.
            bool setDefaultValue(const QVariant& value, QString* errorMsg = 0);
            //! Gets the switch name of the property. If no switch is available, an empty string is returned.
            QString switchName() const;
            //! Gets the description of the property.
            /*!
              Used as part of the tooltip for the property.
              */
            QString description() const;
            //! Sets the description of the property.
            void setDescription(const QString& description);
            //! Gets the note of the property.
            /*!
              Used as part of the tooltip for the property.
              */
            QString note() const;
            //! Sets the note of the property.
            void setNote(const QString& note);
            //! Gets the help ID of the property.
            QString helpID() const;
            //! Gets the level of the property.
            PropertyLevel level() const;
            //! Sets the property level of the property.
            void setLevel(PropertyLevel level);
            //! Gets if the property is editable.
            bool editable() const;
            //! Sets if the property is editable.
            /*!
              The reason that this editable() has a set function as well is that we update
              default properties with saved values and editibility when loading configurations.
              */
            void setEditable(bool editable);
            //! If a specific state is linked to the context linked to the GenericPropertyManager in which this property is managed, changing this property should make the state of the context outdated.
            /*!
             * True by default.
             *
             *\sa setStateDependent()
             */
            bool stateDependent() const;
            //! Sets if the property is state dependent.
            /*!
             * \sa stateDependent()
             */
            void setStateDependent(bool state_dependent);
            //! Gets if the property is dependend on a context linked to the GenericPropertyManager in which this property is managed. Thus, when the context changed, this property can be refreshed.
            /*!
             * False by default.
             *
             *\sa setContextDependent()
             */
            bool contextDependent() const;
            //! Sets if the property is context dependend.
            /*!
             * \sa contextDependent()
             */
            void setContextDependent(bool context_dependent);
            //! Gets if the property is editable by default.
            bool defaultEditable() const;
            //! Gets if the property is visible.
            bool visible() const;
            //! Sets if the property is visible.
            void setVisible(bool is_visible);
            //! Gets if the property is visible by default.
            bool defaultVisible() const;
            //! Gets a filter string applicable to this property.
            QString filterString() const;

            //! Indicates if this property is an external or an internal property.
            /*!
             * Internal properties are not loaded from a properties file but added at runtime (for example in the
             * constructor of GenericProperty). External properties are loaded from property files.
             *
             * Internal properties are not deleted in clear().
             **/
            bool isInternal() const;

            //! Checks if the property matches its default value.
            bool matchesDefault() const;
            //! Gets the backend list separator of this property.
            QString listSeparatorBackend() const;
            Q_DECL_DEPRECATED QString listSeperatorBackend() const;

            //! Sets the backend list separator of this property.
            void setListSeparatorBackend(const QString& sep);
            Q_DECL_DEPRECATED void setListSeperatorBackend(const QString& sep);

            //! Gets the storage list separator of this property.
            QString listSeparatorStorage() const;
            Q_DECL_DEPRECATED QString listSeperatorStorage() const;

            //! Sets the backend list separator of this property.
            void setListSeparatorStorage(const QString& sep);
            Q_DECL_DEPRECATED void setListSeperatorStorage(const QString& sep);

            // --------------------------------
            // Macro Related
            // --------------------------------
            //! Sets the macro mode of this property.
            void setMacroMode(MacroMode macro_mode);
            //! Gets the macro mode of this property.
            MacroMode macroMode() const;
            //! Sets the if the property is a macro.
            /*!
              Note that this function will change the following:
              - Overwrite the current value, and default value of the property to %{propertyName}
              - Make the property not editable.
              - Set its category to qti_def_CATEGOY_GENERIC_PROPERTY_MACROS.
              */
            void setIsMacro(bool is_macro);
            //! Gets the if the property is a macro.
            bool isMacro() const;
            //! Gets the value of the macro, using the given MacroMode.
            /*!
              For expanded modes, valueString() is returned.
              For unexpanded modes, the macro is returned. Thus: %{propertyName}
              */
            QString macroValueString(MacroMode macro_mode) const;

            // --------------------------------
            // Change Signals
            // --------------------------------
        signals:
            //! Emitted when the value of the property changed.
            void valueChanged(GenericProperty* property);
            //! Emitted when the editability of the property changed.
            void editableChanged(GenericProperty* property);
            //! Emitted when the design dependency of the property changed.
            void contextDependentChanged(GenericProperty* property);
            //! Emitted when the possible values of the property changed.
            void possibleValuesDisplayedChanged(GenericProperty* property);
            //! Emitted when the default value of the property changed.
            void defaultValueChanged(GenericProperty* property);
            //! Emitted when the note message of the property changed.
            void noteChanged(GenericProperty* property);

            // --------------------------------
            // Limits and Possible Values
            // --------------------------------
        public:
            //! Gets the max value if this property is of type TypeInteger.
            int intMax() const;
            //! Sets the max value if this property is of type TypeInteger.
            void setIntMax(int new_value);
            //! Gets the min value if this property is of type TypeInteger.
            int intMin() const;
            //! Sets the min value if this property is of type TypeInteger.
            void setIntMin(int new_value);
            //! Gets the step value if this property is of type TypeInteger.
            int intStep() const;
            //! Sets the step value if this property is of type TypeInteger.
            void setIntStep(int new_value);

            //! Gets the max value if this property is of type TypeDouble.
            double doubleMax() const;
            //! Sets the max value if this property is of type TypeDouble.
            void setDoubleMax(double new_value);
            //! Gets the min value if this property is of type TypeDouble.
            double doubleMin() const;
            //! Sets the min value if this property is of type TypeDouble.
            void setDoubleMin(double new_value);
            //! Gets the step value if this property is of type TypeDouble.
            double doubleStep() const;
            //! Sets the step value if this property is of type TypeDouble.
            void setDoubleStep(double new_value);

            //! Gets the possible displayed values if this property is of type TypeEnum.
            QStringList enumPossibleValuesDisplayed() const;
            //! Sets the possible displayed values if this property is of type TypeEnum.
            void setEnumPossibleValuesDisplayed(QStringList new_values);
            //! Gets the possible command line values if this property is of type TypeEnum.
            QStringList enumPossibleValuesCommandLine() const;
            //! Sets the possible command line values if this property is of type TypeEnum.
            void setEnumPossibleValuesCommandLine(QStringList new_values);
            //! Gets the regular expression used if this property is of type TypeString.
            QRegExp stringRegExp() const;
            //! Sets the regular expression used if this property is of type TypeString.
            void setStringRegExp(const QRegExp& new_value);

            // --------------------------------
            // Set & Get Functions For Specific Types
            // These function will only do something if the property type matches the function parameter type.
            // --------------------------------
            void setBoolValue(bool value);
            bool boolValue() const;
            void setIntValue(int value);
            int intValue() const;
            void setDoubleValue(double value);
            double doubleValue() const;
            //! The value will be joined using the backend storage separator which is , by default.
            void setFileList(const QStringList& list);
            //! Adds files to the current files in this property if it is of type TypeFileList.
            void addFiles(const QStringList& list);
            //! Adds a file to the current files in this property if it is of type TypeFileList.
            void addFile(const QString& file);
            //! Gets the file name of this property if it is of type TypeFile.
            QString fileName() const;
            //! Sets the file name of this property if it is of type TypeFile.
            void setFileName(const QString& file_name);
            //! Gets the path of this property if it is of type TypePath.
            QString path() const;
            //! Sets the path of this property if it is of type TypePath.
            void setPath(const QString& file_name);
            //! The value will be split using the backend storage separator which is , by default.
            QStringList fileList() const;
            //! Returns the file list string in the format specified, only for properties of type TypeFileList.
            QString fileListString(FileListStringFormat format = FileListStringFormat_0) const;
            //! The value will be split using the backend storage separator which is , by default.
            QStringList pathList() const;
            //! Returns the file list string in the format specified, only for properties of type TypePathList.
            QString pathListString(FileListStringFormat format = FileListStringFormat_0) const;
            //! Gets the command line value which maps to a displayed value.
            /*!
              \param displayed_enum The displayed enum to map. When empty, the current displayed value will be mapped.
              \param ok If the displayed_enum is not found in the list of possible values, \p ok will be set to false when valid. True otherwise. When false, the function returns QString().
              */
            QString mapDisplayedEnumToCommandLineEnum(QString displayed_enum, bool* ok = 0) const;
            //! Gets the file filter which is used type is TypeFile or TypeFileList.
            /*!
              In the form, for example: "Images (*.png *.xpm *.jpg)"
              */
            QString fileNameFilter() const;
            //! Sets the file filter which is used type is TypeFile or TypeFileList.
            void setFileNameFilter(const QString &new_value);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
    //        //! Converts the property to a line of a CSV file.
    //        /*!
    //          \sa fromCsvString()
    //          */
    //        QString toCsvString() const;
            //! Imports the property from a line of a CSV file.
            /*!
            The CSV String export format of the property is as follows.
            The position in the list below represents the column in which information is expected:
            - Name
            - Type
            - List based separator in backend process.
            - Default Value - List based values seperated by d->list_storage_separator
            - Level
            - Category - Multiple levels seperated by d->list_storage_separator
            - Default Editable
            - Default Visible
            - Switch Name
            - Help ID
            - Int Max
            - Int Min
            - Int Step
            - Enum Possible Values = Seperated by d->list_storage_separator
            - String RegExp Pattern
            - String RegExp Pattern Syntax (true of false)
            - String RegExp Case Sensitive: (int) QRegExp::PatternSyntax.
            - Description

            \note When value is empty, it will be set to the default value.
            \note Boolean values are stored as string of 1 and 0.
            \note This function sets the property as an external property, see isInternal() for more information.

              \sa toCsvString()
              */
            bool fromCsvString(const QString& csv_string);

            static InstanceFactoryInfo staticInstanceFactoryInfo();
            virtual IExportable::ExportModeFlags supportedFormats() const;
            virtual InstanceFactoryInfo instanceFactoryInfo() const;
            virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            virtual bool isModified() const;
        public slots:
            virtual void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            /*!
              The property is marked as modified when:
              - The value changes.
              - The editability changes.
              */
            void modificationStateChanged(bool is_modified) const;

        public:
            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, GenericProperty> factory;

        private:
            //! Converts a list based value to the format required by the backend process.
            QString listToBackendFormat(const QString& value) const;
            //! Converts a list based value to the format required for storage in a property storage formats (CSV and XML).
            QString listToStorageFormat(const QString &value) const;

            GenericPropertyData* d;
        };
    }
}



#endif // GENERIC_PROPERTY_H

