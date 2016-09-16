/****************************************************************************
**
** Copyright 2010-2013, CSIR
** Author: JP Naude, jpnaude@csir.co.za
**
****************************************************************************/

#include "GenericProperty.h"
#include "FileUtils.h"
#include "QtilitiesCoreConstants.h"
using namespace Qtilities::Core::Constants;

#include <ObjectManager>
#include <QDomDocument>

#include "limits.h"
#include "float.h"

namespace Qtilities {
namespace Core {

struct GenericPropertyData {
    GenericPropertyData() : is_modified(false),
        type(GenericProperty::TypeString),
        list_backend_separator("|"),
        list_storage_separator("|"),
        level(GenericProperty::LevelStandard),
        editable(true),
        default_editable(true),
        state_dependent(true),
        context_dependent(false),
        is_internal(false),
        visible(true),
        default_visible(true),
        default_value_set(false),
        int_max(INT_MAX),
        int_min(INT_MIN),
        int_step(1),
        double_max(INT_MAX),
        double_min(INT_MIN),
        double_step(1),
        file_name_filter("*.*"),
        macro_mode(GenericProperty::MacroUnexpanded),
        is_macro(false) { }

    bool is_modified;
    QString name;
    QMap<QString,PropertyAlias> aliases;
    GenericProperty::PropertyType type;
    QtilitiesCategory category;
    QString value;
    QString default_value;
    QString switch_name;
    QString description;
    QString help_id;
    QString note;
    QString list_backend_separator;
    QString list_storage_separator;
    GenericProperty::PropertyLevel level;
    bool editable;
    bool default_editable;
    bool state_dependent;
    bool context_dependent;
    bool is_internal;
    QString property_filter_string;
    bool visible;
    bool default_visible;
    bool default_value_set;

    int int_max;
    int int_min;
    int int_step;
    int double_max;
    int double_min;
    int double_step;
    QStringList enum_possible_values_displayed;
    QStringList enum_possible_values_command_line;
    QRegExp string_reg_exp;
    QVariant variant_value;
    QString file_name_filter;

    GenericProperty::MacroMode macro_mode;
    bool is_macro;
};

FactoryItem<QObject, GenericProperty> GenericProperty::factory;

GenericProperty::GenericProperty(QObject *parent):
    QObject(parent)
{
    d = new GenericPropertyData;
}

GenericProperty::GenericProperty(const QString &property_name, QObject *parent):
    QObject(parent)
{
    d = new GenericPropertyData;
    d->name = property_name;
}

GenericProperty::~GenericProperty() {
    delete d;
}

GenericProperty::GenericProperty(const GenericProperty& ref) : QObject(ref.parent()) {
    d = new GenericPropertyData;
    d->name = ref.propertyName();
    d->aliases = ref.aliases();
    d->type = ref.type();
    d->category = ref.category();
    d->value = ref.valueString();
    d->default_value = ref.defaultValueString();
    d->switch_name = ref.switchName();
    d->description = ref.description();
    d->help_id = ref.helpID();
    d->level = ref.level();
    d->editable = ref.editable();
    d->default_editable = ref.defaultEditable();
    d->visible = ref.defaultVisible();
    d->default_visible = ref.defaultVisible();
    d->int_max = ref.intMax();
    d->int_min = ref.intMin();
    d->int_step = ref.intStep();
    d->enum_possible_values_displayed = ref.enumPossibleValuesDisplayed();
    d->enum_possible_values_command_line = ref.enumPossibleValuesCommandLine();
    d->string_reg_exp = ref.stringRegExp();
    d->file_name_filter = ref.fileNameFilter();
    d->state_dependent = ref.stateDependent();
    d->context_dependent = ref.contextDependent();
    d->is_internal = ref.isInternal();
    d->list_backend_separator = ref.listSeparatorBackend();
    d->list_storage_separator = ref.listSeparatorStorage();
    setIsExportable(ref.isExportable());
}

GenericProperty& GenericProperty::operator=(const GenericProperty& ref) {
    if (this==&ref) return *this;

    d->name = ref.propertyName();
    d->aliases = ref.aliases();
    d->type = ref.type();
    d->category = ref.category();
    d->value = ref.valueString();
    d->default_value = ref.defaultValueString();
    d->switch_name = ref.switchName();
    d->description = ref.description();
    d->help_id = ref.helpID();
    d->level = ref.level();
    d->editable = ref.editable();
    d->default_editable = ref.defaultEditable();
    d->visible = ref.defaultVisible();
    d->default_visible = ref.defaultVisible();
    d->int_max = ref.intMax();
    d->int_min = ref.intMin();
    d->int_step = ref.intStep();
    d->enum_possible_values_displayed = ref.enumPossibleValuesDisplayed();
    d->enum_possible_values_command_line = ref.enumPossibleValuesCommandLine();
    d->string_reg_exp = ref.stringRegExp();
    d->file_name_filter = ref.fileNameFilter();
    d->state_dependent = ref.stateDependent();
    d->context_dependent = ref.contextDependent();
    d->is_internal = ref.isInternal();
    d->list_backend_separator = ref.listSeparatorBackend();
    d->list_storage_separator = ref.listSeparatorStorage();
    setIsExportable(ref.isExportable());

    setModificationState(true);

    return *this;
}

bool GenericProperty::operator==(const GenericProperty& ref) {
    if (d->type != ref.type())
        return false;
    else if (d->name != ref.propertyName())
        return false;
    else if (d->aliases != ref.aliases())
        return false;
    else if (d->category != ref.category())
        return false;
    else if (d->value != ref.value())
        return false;
    else if (d->default_value != ref.defaultValueString())
        return false;
    else if (d->switch_name != ref.switchName())
        return false;
    else if (d->description != ref.description())
        return false;
    else if (d->help_id != ref.helpID())
        return false;
    else if (d->level != ref.level())
        return false;
    else if (d->editable != ref.editable())
        return false;
    else if (d->default_editable != ref.defaultEditable())
        return false;
    else if (d->visible != ref.visible())
        return false;
    else if (d->default_visible != ref.defaultVisible())
        return false;
    else if (d->int_max != ref.intMax())
        return false;
    else if (d->int_min != ref.intMin())
        return false;
    else if (d->int_step != ref.intStep())
        return false;
    else if (d->enum_possible_values_displayed != ref.enumPossibleValuesDisplayed())
        return false;
    else if (d->enum_possible_values_command_line != ref.enumPossibleValuesCommandLine())
        return false;
    else if (d->string_reg_exp != ref.stringRegExp())
        return false;
    else if (d->file_name_filter != ref.fileNameFilter())
        return false;
    else if (d->state_dependent != ref.stateDependent())
        return false;
    else if (d->context_dependent != ref.contextDependent())
        return false;
    else if (d->is_internal != ref.isInternal())
        return false;
    else if (d->list_backend_separator != ref.listSeparatorBackend())
        return false;
    else if (d->list_storage_separator != ref.listSeparatorStorage())
        return false;
    else if (isExportable() != ref.isExportable())
        return false;
    else
        return true;
}

bool GenericProperty::operator!=(const GenericProperty& ref) {
    return !(*this==ref);
}

QString GenericProperty::propertyName() const {
    return d->name;
}

void GenericProperty::setPropertyName(const QString &property_name) {
    d->name = property_name;
    setObjectName(property_name);
}

QMap<QString, PropertyAlias> GenericProperty::aliases() const {
    return d->aliases;
}

bool GenericProperty::matchesPropertyName(const QString &match_name, bool search_aliases, const QString &alias_environment, Qt::CaseSensitivity cs) const {
    // Check the property name:
    if (d->name.compare(match_name,cs) == 0)
        return true;

    if (search_aliases) {
        foreach (const QString& alias, aliasNames(alias_environment)) {
            if (alias.compare(match_name,cs) == 0)
                return true;
        }
    }

    return false;
}

QStringList GenericProperty::aliasNames(const QString &alias_environment) const {
    if (alias_environment.isEmpty())
        return d->aliases.keys();
    else {
        QStringList environment_aliases;
        QList<PropertyAlias> aliases = d->aliases.values();
        foreach (const PropertyAlias& alias, aliases) {
            if (alias.d_environment.compare(alias_environment,Qt::CaseInsensitive) == 0)
                environment_aliases << alias.d_name;
        }
        return environment_aliases;
    }
}

PropertyAlias GenericProperty::alias(const QString &alias, bool *ok) const {
    if (d->aliases.contains(alias)) {
        if (ok)
            *ok = true;
        return d->aliases[alias];
    }

    if (ok)
        *ok = false;
    return PropertyAlias();
}

GenericProperty::PropertyType GenericProperty::type() const {
    return d->type;
}

void GenericProperty::setType(GenericProperty::PropertyType type) {
    d->type = type;
}

QVariant::Type GenericProperty::qVariantType() const {
    return propertyTypeQVariantType(d->type);
}

QtilitiesCategory GenericProperty::category() const {
    return d->category;
}

void GenericProperty::setCategory(QtilitiesCategory category) {
    if (d->category != category) {
        d->category = category;
        // TODO: Get a way to refresh the property browser from here.
    }
}

QString GenericProperty::valueString() const {
    return d->value;
}

bool GenericProperty::setValueString(const QString &value, QString *errorMsg) {
    if (d->value != value) {
        bool set = false;
        // Validate the new value according to the type of property:
        if (d->type == TypeInteger) {
            bool valid_int;
            int tmp_int = value.toInt(&valid_int);
            if (!valid_int) {
                if (errorMsg)
                    *errorMsg = "The specified value " + value + " is not a valid integer.";
            } else {
                blockSignals(true);
                setIntValue(tmp_int);
                blockSignals(false);
                set = true;
            }
        } else if (d->type == TypeDouble) {
            bool valid_double;
            double tmp_dbl = value.toDouble(&valid_double);
            if (!valid_double) {
                if (errorMsg)
                    *errorMsg = "The specified value " + value + " is not a valid double.";
            } else {
                blockSignals(true);
                setDoubleValue(tmp_dbl);
                blockSignals(false);
                set = true;
            }
        } else if (d->type == TypeEnum) {
            if (d->enum_possible_values_displayed.contains(value)) {
                d->value = value;
                set = true;
            } else {
                if (errorMsg)
                    *errorMsg = "The specified value \"" + value + "\" is not one of the possible enum values which are: " + d->enum_possible_values_displayed.join("|");
            }
        } else if (d->type == TypeBool) {
            // In here we check against known bool string representations:
            if (value.compare("YES",Qt::CaseInsensitive) == 0 || value.compare("TRUE",Qt::CaseInsensitive) == 0 || value.compare("1",Qt::CaseInsensitive) == 0) {
                blockSignals(true);
                setBoolValue(true);
                blockSignals(false);
                set = true;
            } else if (value.compare("NO",Qt::CaseInsensitive) == 0 || value.compare("FALSE",Qt::CaseInsensitive) == 0 || value.compare("0",Qt::CaseInsensitive) == 0) {
                blockSignals(true);
                setBoolValue(false);
                blockSignals(false);
                set = true;
            } else {
                if (errorMsg)
                    *errorMsg = "The specified value " + value + " is not a valid bool. Either use TRUE, YES or 1 or their opposites.";
            }
        } else if (d->type == TypeFile || d->type == TypePath) {
            d->value = value;
            set = true;
        } else if (d->type == TypeFileList || d->type == TypePathList) {
            // Split using the known backend separator and set using setFileList in order to remove duplicates.
            setFileList(value.split(d->list_backend_separator));
            set = true;
        } else if (d->type == TypeString || d->type == TypeVariant) {
            d->value = value;
            set = true;
        }

        if (set) {
            // If it does not have a default yet, we set the default to this value:
            if (d->default_value.isEmpty() && !d->default_value_set)
                d->default_value = value;

            setModificationState(true);
            emit valueChanged(this);
        }

        return set;
    }

    return true;
}

bool GenericProperty::setValueFromProperty(GenericProperty *property) {
    if (!property)
        return false;

    setValueString(property->valueString().split(property->listSeparatorBackend()).join(listSeparatorBackend()));
    return true;
}

bool GenericProperty::compareValue(GenericProperty *property) {
    if (!property)
        return false;

    QStringList string_list_base = d->value.split(listSeparatorBackend());
    QStringList string_list_ref = property->valueString().split(property->listSeparatorBackend());

    return (string_list_base == string_list_ref);
}

QVariant GenericProperty::value() const {
    if (d->type == GenericProperty::TypeVariant) {
        return d->variant_value;
    } else {
        return QtilitiesProperty::constructVariant(propertyTypeQVariantType(d->type),d->value);
    }
}

bool GenericProperty::setValue(const QVariant &value, QString* errorMsg) {
    if (d->type == GenericProperty::TypeVariant) {
        d->variant_value = value;
        return true;
    } else {
        return setValueString(value.toString(),errorMsg);
    }
}

QString GenericProperty::defaultValueString() const {
    return d->default_value;
}

bool GenericProperty::setDefaultValueString(const QString &value, QString* errorMsg) {
    if (d->default_value != value) {
        bool set = false;
        // Validate the new value according to the type of property:
        if (d->type == TypeInteger) {
            bool valid_int;
            value.toInt(&valid_int);
            if (!valid_int) {
                if (errorMsg)
                    *errorMsg = "The specified value " + value + " is not a valid integer.";
            } else {
                set = true;
            }
        } else if (d->type == TypeDouble) {
            bool valid_double;
            value.toDouble(&valid_double);
            if (!valid_double) {
                if (errorMsg)
                    *errorMsg = "The specified value " + value + " is not a valid double.";
            } else {
                set = true;
            }
        } else if (d->type == TypeEnum) {
            if (d->enum_possible_values_displayed.isEmpty()) {
                set = true;
            } else {
                if (d->enum_possible_values_displayed.contains(value)) {
                    set = true;
                } else {
                    if (errorMsg)
                        *errorMsg = "The specified value \"" + value + "\" is not one of the possible enum values which are: " + d->enum_possible_values_displayed.join("|");
                }
            }
        } else if (d->type == TypeBool) {
            // In here we check against known bool string representations:
            if (value.compare("YES",Qt::CaseInsensitive) == 0 || value.compare("TRUE",Qt::CaseInsensitive) == 0 || value.compare("1",Qt::CaseInsensitive) == 0) {
                set = true;
            } else if (value.compare("NO",Qt::CaseInsensitive) == 0 || value.compare("FALSE",Qt::CaseInsensitive) == 0 || value.compare("0",Qt::CaseInsensitive) == 0) {
                set = true;
            } else {
                if (errorMsg)
                    *errorMsg = "The specified value " + value + " is not a valid bool. Either use TRUE, YES or 1 or their opposites.";
            }
        } else if (d->type == TypeFile || d->type == TypePath)
            set = true;
        else if (d->type == TypeFileList || d->type == TypePathList)
            set = true;
        else if (d->type == TypeString || d->type == TypeVariant)
            set = true;

        if (set) {
            d->default_value = value;
            d->default_value_set = true;

            if (d->value.isEmpty())
                d->value = value;
            else {
                setModificationState(true);
                emit valueChanged(this);
            }
        }

        return set;
    } else
        d->default_value_set = true;

    return true;
}

bool GenericProperty::setDefaultValue(const QVariant &value, QString *errorMsg) {
    return setDefaultValueString(value.toString(),errorMsg);
}

QString GenericProperty::switchName() const {
    return d->switch_name;
}

QString GenericProperty::description() const {
    return d->description;
}

void GenericProperty::setDescription(const QString &description) {
    if (d->description != description) {
        d->description = description;
        setModificationState(true);
        emit noteChanged(this);
    }
}

QString GenericProperty::note() const {
    return d->note;
}

void GenericProperty::setNote(const QString &note) {
    if (d->note != note) {
        d->note = note;
        setModificationState(true);
        emit noteChanged(this);
    }
}

QString GenericProperty::helpID() const {
    return d->help_id;
}

GenericProperty::PropertyLevel GenericProperty::level() const {
    return d->level;
}

void GenericProperty::setLevel(GenericProperty::PropertyLevel level) {
    d->level = level;
}

bool GenericProperty::editable() const {
    return d->editable;
}

void GenericProperty::setEditable(bool editable) {
    if (d->editable != editable) {
        d->editable = editable;
        emit editableChanged(this);
    }
}

bool GenericProperty::stateDependent() const {
    return d->state_dependent;
}

void GenericProperty::setStateDependent(bool state_dependent) {
    if (d->state_dependent != state_dependent) {
        d->state_dependent = state_dependent;
        emit contextDependentChanged(this);
    }
}

bool GenericProperty::contextDependent() const {
    return d->context_dependent;
}

void GenericProperty::setContextDependent(bool context_dependent) {
    if (d->context_dependent != context_dependent) {
        d->context_dependent = context_dependent;
    }
}

bool GenericProperty::defaultEditable() const {
    return d->default_editable;
}

bool GenericProperty::visible() const {
    return d->visible;
}

void GenericProperty::setVisible(bool is_visible) {
    d->visible = is_visible;
}

bool GenericProperty::defaultVisible() const {
    return d->default_visible;
}

QString GenericProperty::filterString() const {
    return d->property_filter_string;
}

bool GenericProperty::isInternal() const {
    return d->is_internal;
}

bool GenericProperty::matchesDefault() const {
    if (d->type == GenericProperty::TypeVariant) {
        return true;
    } else {
        return (d->value.compare(d->default_value,Qt::CaseInsensitive) == 0);
    }
}

QString GenericProperty::listSeperatorBackend() const {
    return listSeparatorBackend();
}

QString GenericProperty::listSeparatorBackend() const {
    return d->list_backend_separator;
}

void GenericProperty::setListSeperatorBackend(const QString& sep) {
    setListSeparatorBackend(sep);
}

void GenericProperty::setListSeparatorBackend(const QString &sep) {
    if (d->list_backend_separator != sep) {
        // Convert the value from the old separator to the new separator:
        if (!d->value.isEmpty())
            d->value = d->value.split(d->list_backend_separator,QString::SkipEmptyParts).join(sep);
        if (!d->default_value.isEmpty())
            d->default_value = d->default_value.split(d->list_backend_separator,QString::SkipEmptyParts).join(sep);
        d->list_backend_separator = sep;
    }
}

QString GenericProperty::listSeperatorStorage() const {
    return listSeparatorStorage();
}

QString GenericProperty::listSeparatorStorage() const {
    return d->list_storage_separator;
}

void GenericProperty::setListSeperatorStorage(const QString &sep) {
    setListSeparatorBackend(sep);
}

void GenericProperty::setListSeparatorStorage(const QString &sep) {
    d->list_storage_separator = sep;
}

// --------------------------------
// Limits and Possible Values
// --------------------------------
int GenericProperty::intMax() const {
    return d->int_max;
}

void GenericProperty::setIntMax(int new_value) {
    if (d->int_max != new_value) {
        if (d->type == TypeInteger && d->value.toInt() > new_value)
            setValueString(QString::number(new_value));
        d->int_max = new_value;
        emit possibleValuesDisplayedChanged(this);
    }
}

int GenericProperty::intMin() const {
    return d->int_min;
}

void GenericProperty::setIntMin(int new_value) {
    if (d->int_min != new_value) {
        if (d->type == TypeInteger && d->value.toInt() < new_value)
            setValueString(QString::number(new_value));
        d->int_min = new_value;
        emit possibleValuesDisplayedChanged(this);
    }
}

int GenericProperty::intStep() const {
    return d->int_step;
}

void GenericProperty::setIntStep(int new_value) {
    if (d->int_step != new_value) {
        d->int_step = new_value;
        emit possibleValuesDisplayedChanged(this);
    }
}

double GenericProperty::doubleMax() const {
    return d->double_max;
}

void GenericProperty::setDoubleMax(double new_value) {
    if (d->double_max != new_value) {
        if (d->type == TypeDouble && d->value.toDouble() > new_value)
            setValueString(QString::number(new_value));
        d->double_max = new_value;
        emit possibleValuesDisplayedChanged(this);
    }
}

double GenericProperty::doubleMin() const {
    return d->double_min;
}

void GenericProperty::setDoubleMin(double new_value) {
    if (d->double_min != new_value) {
        if (d->type == TypeDouble && d->value.toDouble() < new_value)
            setValueString(QString::number(new_value));
        d->double_min = new_value;
        emit possibleValuesDisplayedChanged(this);
    }
}

double GenericProperty::doubleStep() const {
    return d->double_step;
}

void GenericProperty::setDoubleStep(double new_value) {
    if (d->double_step != new_value) {
        d->double_step = new_value;
        emit possibleValuesDisplayedChanged(this);
    }
}

QStringList GenericProperty::enumPossibleValuesDisplayed() const {
    return d->enum_possible_values_displayed;
}

void GenericProperty::setEnumPossibleValuesDisplayed(QStringList new_values) {
    if (new_values.isEmpty())
        return;

    if (d->enum_possible_values_displayed != new_values) {
        d->enum_possible_values_displayed = new_values;
        if (d->type == TypeEnum && !new_values.contains(d->value)) {
            if (new_values.contains(d->default_value))
                setValueString(d->default_value);
            else
                setValueString(new_values.front());
        }
        emit possibleValuesDisplayedChanged(this);
    }
}

QStringList GenericProperty::enumPossibleValuesCommandLine() const {
    return d->enum_possible_values_command_line;
}

void GenericProperty::setEnumPossibleValuesCommandLine(QStringList new_values) {
    if (new_values.isEmpty())
        return;

    if (d->enum_possible_values_command_line != new_values) {
        d->enum_possible_values_command_line = new_values;
    }
}

QRegExp GenericProperty::stringRegExp() const {
    return d->string_reg_exp;
}

void GenericProperty::setStringRegExp(const QRegExp &new_value) {
    if (!new_value.isValid() || new_value.isEmpty())
        return;

    if (d->string_reg_exp != new_value) {
        if (d->type == TypeString && !new_value.exactMatch(d->value))
            setValueString(d->default_value);
        d->string_reg_exp = new_value;
        emit possibleValuesDisplayedChanged(this);
    }
}

QString GenericProperty::fileNameFilter() const {
    return d->file_name_filter;
}

void GenericProperty::setFileNameFilter(const QString &new_value) {
    if (d->file_name_filter != new_value) {
        d->file_name_filter = new_value;
    }
}

// --------------------------------
// Set & Get Functions For Specific Types
// These function will only do something if the property type matches the function parameter type.
// --------------------------------

void GenericProperty::setBoolValue(bool value) {
    Q_ASSERT(type() == TypeBool);
    if (type() == TypeBool) {
        if (value)
            d->value = "TRUE";
        else
            d->value = "FALSE";
        emit valueChanged(this);
    }
}

bool GenericProperty::boolValue() const {
    Q_ASSERT(type() == TypeBool);
    if (type() == TypeBool) {
        if (valueString().compare("TRUE",Qt::CaseInsensitive) == 0)
            return true;
        else
            return false;
    }
    return false;
}

void GenericProperty::setIntValue(int value) {
    Q_ASSERT(type() == TypeInteger);
    if (type() == TypeInteger) {
        QString value_str = QString::number(value);
        if (d->value == value_str)
            return;
        if (value < d->int_min)
            value = d->int_min;
        else if (value > d->int_max)
            value = d->int_max;
        d->value = value_str;
        emit valueChanged(this);
    }
}

int GenericProperty::intValue() const {
    Q_ASSERT(type() == TypeInteger);
    if (type() == TypeInteger)
        return valueString().toInt();
    return -1;
}

void GenericProperty::setDoubleValue(double value) {
    Q_ASSERT(type() == TypeDouble);
    if (type() == TypeDouble) {
        QString value_str = QString::number(value);
        if (d->value == value_str)
            return;
        if (value < d->double_min)
            value = d->double_min;
        else if (value > d->double_max)
            value = d->double_max;
        d->value = value_str;
        emit valueChanged(this);
    }
}

double GenericProperty::doubleValue() const {
    Q_ASSERT(type() == TypeDouble);
    if (type() == TypeDouble)
        return valueString().toDouble();
    return -1;
}

void GenericProperty::setFileList(const QStringList &list) {
    //Q_ASSERT(type() == TypeFileList || type() == TypePathList);

    if (type() == TypeFileList|| type() == TypePathList) {
        QStringList clean_list;
        foreach (const QString& path, list)
            clean_list << FileUtils::toNativeSeparators(QDir::cleanPath(path));
        clean_list.removeDuplicates();
        clean_list.sort();
        d->value = clean_list.join(d->list_backend_separator);
        emit valueChanged(this);
    }
}

void GenericProperty::addFiles(const QStringList &list) {
    QStringList paths = fileList();
    paths.append(list);
    paths.sort();
    setFileList(paths);
}

void GenericProperty::addFile(const QString &file) {
    addFiles(QStringList(file));
}

QString GenericProperty::fileName() const {
    return valueString();
}

void GenericProperty::setFileName(const QString &file_name) {
    setValueString(file_name);
}

QString GenericProperty::path() const {
    return valueString();
}

void GenericProperty::setPath(const QString &file_name) {
    setValueString(file_name);
}

QStringList GenericProperty::fileList() const {
    if (type() == TypeFileList|| type() == TypePathList)
        return valueString().split(d->list_backend_separator,QString::SkipEmptyParts);
    Q_ASSERT(type() == TypeFileList || type() == TypePathList);
    return QStringList();
}

QString GenericProperty::fileListString(GenericProperty::FileListStringFormat format) const {
    QStringList file_list = fileList();
    if (file_list.isEmpty())
        return "";

    QString string;
    if (format == FileListStringFormat_0) {
        string.append("{");
        foreach (const QString& file, file_list) {
            string.append("\"" + file + "\" ");
        }
        string.append("}");
    } else {
        return d->value;
    }
    return string;
}

QStringList GenericProperty::pathList() const {
    return fileList();
}

QString GenericProperty::pathListString(GenericProperty::FileListStringFormat format) const {
    Q_UNUSED(format)
    return fileListString();
}

QString GenericProperty::mapDisplayedEnumToCommandLineEnum(QString displayed_enum, bool* ok) const {
    if (displayed_enum.isEmpty())
        displayed_enum  = d->value;

    // Get the index of the displayed item and loop up in the command line string list at this index:
    int index = d->enum_possible_values_displayed.indexOf(displayed_enum);
    if (index != -1) {
        if (ok)
            *ok = true;
        if (index >= 0 && index < d->enum_possible_values_displayed.count()) {
            return d->enum_possible_values_command_line.at(index);
        } else {
            if (ok)
                *ok = false;
            return QString();
        }
    } else {
        if (ok)
            *ok = false;
        return QString();
    }
}

// --------------------------------
// IExportable Implementation
// --------------------------------

//QString GenericProperty::toCsvString() const {
//    QStringList csv_list;
//    csv_list << objectName();
//    csv_list << propertyTypeToString(d->type);
//    csv_list << d->list_backend_separator;
//    csv_list << listToStorageFormat(d->value);
//    csv_list << listToStorageFormat(d->default_value);
//    csv_list << d->description;
//    csv_list << propertyLevelToString(d->level);
//    csv_list << d->category.toString(d->list_storage_separator);

//    if (d->editable)
//        csv_list << "1";
//    else
//        csv_list << "0";

//    csv_list << d->switch_name;
//    csv_list << d->help_id;
//    csv_list << QString::number(d->int_max);
//    csv_list << QString::number(d->int_min);
//    csv_list << QString::number(d->int_step);
//    csv_list << d->enum_possible_values_displayed.join(d->list_storage_separator);
//    Command line values missing here.
//    csv_list << d->string_reg_exp.pattern();
//    csv_list << QString::number((int) d->string_reg_exp.patternSyntax());

//    if (d->string_reg_exp.caseSensitivity() == Qt::CaseSensitive)
//        csv_list << "1";
//    else
//        csv_list << "0";

//    return csv_list.join(",");
//}

bool GenericProperty::fromCsvString(const QString &csv_string) {
    QStringList csv_list = csv_string.split(",");
    if (csv_list.count() != 23)
        qDebug() << Q_FUNC_INFO << "Invalid number of items in input list, this property will be incomplete. Expected vs. Found: " << 23 << csv_list.count() << csv_list;

    d->is_internal = false;

    if (csv_list.count() < 1)
        return false;

    if (csv_list[0].isEmpty())
        return false;
    setPropertyName(csv_list[0]);

    if (csv_list.count() < 2)
        return false;

    QStringList aliases = csv_list[1].split(d->list_storage_separator);
    foreach (const QString& alias, aliases) {
        QStringList alias_split = alias.split("::");
        if (alias_split.count() == 2) {
            PropertyAlias property_alias;
            property_alias.d_environment = alias_split.front();

            // Check if the name starts with a -:
            QString name = alias_split.last();
            if (name.startsWith("-")) {
                property_alias.d_name = name.remove(0,1);
                property_alias.d_inverted = true;
            } else {
                property_alias.d_name = name;
                property_alias.d_inverted = false;
            }
            d->aliases[property_alias.d_name] = property_alias;
        }
    }

    if (csv_list.count() < 3)
        return false;

    d->type = stringToPropertyType(csv_list[2]);

    if (csv_list.count() < 4)
        return false;

    d->list_backend_separator = csv_list[3].simplified();
    if (d->list_backend_separator.isEmpty())
        d->list_backend_separator = d->list_storage_separator;

    if (csv_list.count() < 5)
        return false;

    d->default_value = listToBackendFormat(csv_list[4]);

    if (d->value.isEmpty())
        d->value = d->default_value;

    if (csv_list.count() < 6)
        return false;

    d->level = stringToPropertyLevel(csv_list[5]);

    if (csv_list.count() < 7)
        return false;

    d->category = QtilitiesCategory(csv_list[6],d->list_storage_separator);

    if (csv_list.count() < 8)
        return false;

    if (csv_list[7].simplified().compare("true",Qt::CaseInsensitive) == 0)
        d->default_editable = true;
    else
        d->default_editable = false;
    d->editable = d->default_editable;

    if (csv_list.count() < 9)
        return false;

    if (csv_list[8].simplified().compare("true",Qt::CaseInsensitive) == 0)
        d->default_visible = true;
    else
        d->default_visible = false;
    d->visible = d->default_visible;

    if (csv_list.count() < 10)
        return false;

    d->switch_name = csv_list[9].simplified();

    if (csv_list.count() < 11)
        return false;

    d->help_id = csv_list[10].simplified();

    if (csv_list.count() < 12)
        return false;

    d->int_max = csv_list[11].toInt();

    if (csv_list.count() < 13)
        return false;

    d->int_min = csv_list[12].toInt();

    if (csv_list.count() < 14)
        return false;

    d->int_step = csv_list[13].toInt();

    if (csv_list.count() < 15)
        return false;

    d->enum_possible_values_displayed = csv_list[14].split(d->list_storage_separator,QString::SkipEmptyParts);

    if (csv_list.count() < 16)
        return false;

    d->enum_possible_values_command_line = csv_list[15].split(d->list_storage_separator,QString::SkipEmptyParts);
    if (d->enum_possible_values_command_line.isEmpty())
        d->enum_possible_values_command_line = d->enum_possible_values_displayed;
    Q_ASSERT(d->enum_possible_values_command_line.count() == d->enum_possible_values_displayed.count());

    if (d->value.isEmpty() && d->default_value.isEmpty() && d->enum_possible_values_displayed.count() > 0) {
        d->default_value = d->enum_possible_values_displayed.front();
        d->value = d->enum_possible_values_displayed.front();
    }

    if (csv_list.count() < 17)
        return false;

    QRegExp reg_exp;
    reg_exp.setPattern(csv_list[16].simplified());

    if (csv_list.count() < 18)
        return false;

    reg_exp.setPatternSyntax((QRegExp::PatternSyntax) csv_list[17].toInt());

    if (csv_list.count() < 19)
        return false;

    if (csv_list[18].simplified().compare("true",Qt::CaseInsensitive) == 0)
        reg_exp.setCaseSensitivity(Qt::CaseSensitive);
    else
        reg_exp.setCaseSensitivity(Qt::CaseInsensitive);
    d->string_reg_exp = reg_exp;

    if (csv_list.count() < 20)
        return false;

    d->description = csv_list[19];
    d->description = d->description.simplified();

    if (csv_list.count() < 21)
        return false;

    d->file_name_filter = csv_list[20];
    d->file_name_filter = d->file_name_filter.simplified();

    if (csv_list.count() < 22)
        return false;

    if (csv_list[21].simplified().compare("false",Qt::CaseInsensitive) == 0)
        d->state_dependent = false;
    else
        d->state_dependent = true;

    if (csv_list.count() < 23)
        return false;

    if (csv_list[22].simplified().compare("true",Qt::CaseInsensitive) == 0)
        d->context_dependent = true;
    else
        d->context_dependent = false;

    return true;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags GenericProperty::supportedFormats() const {
    return IExportable::XML;
}

Qtilities::Core::InstanceFactoryInfo GenericProperty::staticInstanceFactoryInfo() {
    InstanceFactoryInfo factoryData(qti_def_FACTORY_TAG_GENERIC_PROPERTY,qti_def_FACTORY_QTILITIES,"");
    return factoryData;
}

Qtilities::Core::InstanceFactoryInfo GenericProperty::instanceFactoryInfo() const {
    InstanceFactoryInfo fi = GenericProperty::staticInstanceFactoryInfo();
    fi.d_instance_name = propertyName();
    return fi;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags GenericProperty::exportXml(QDomDocument* doc, QDomElement* object_node) const {
    Q_UNUSED(doc);
    if (!object_node)
        return IExportable::Failed;

    QList<SharedProperty> shared_properties;
    SharedProperty param_1("name",propertyName());
    shared_properties << param_1;

    if (d->type & TypeListBased) {
        SharedProperty param_2("separatorStorage",d->list_storage_separator);
        shared_properties << param_2;
        SharedProperty param_3("separatorBackend",d->list_backend_separator);
        shared_properties << param_3;
    }

    if (isMacro() && macroMode() == MacroExpandedCustom) {
        SharedProperty param_4("value",macroValueString(MacroExpandedCustom));
        shared_properties << param_4;
    } else {
        SharedProperty param_5("value",listToStorageFormat(d->value));
        shared_properties << param_5;
        SharedProperty param_6("editable",d->editable);
        shared_properties << param_6;
        if (!d->note.isEmpty()) {
            SharedProperty param_7("note",d->note);
            shared_properties << param_7;
        }
    }

    int count = shared_properties.count();
    if (count == 0)
        return IExportable::Complete;

    for (int i = 0; i < count; ++i) {
        SharedProperty property = shared_properties.at(i);

        // Now do the export:
        QDomElement property_element = doc->createElement(property.propertyNameString());
        QDomText property_element_text = doc->createTextNode(property.value().toString());
        property_element.appendChild(property_element_text);
        object_node->appendChild(property_element);
    }

    return IExportable::Complete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags GenericProperty::importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list) {
    Q_UNUSED(doc)
    Q_UNUSED(import_list)
    if (!object_node)
        return IExportable::Failed;

    bool has_value = false;
    bool has_name = false;

    QDomElement element_1 = object_node->firstChildElement("name");
    if (!element_1.isNull()) {
        setPropertyName(element_1.text());
        has_name = true;
    }
    QDomElement element_2 = object_node->firstChildElement("separatorStorage");
    if (element_2.isNull())
      element_2 = object_node->firstChildElement("seperatorStorage");
    // backwards compatibility with the "seperator" typo

    if (!element_2.isNull())
        d->list_storage_separator = element_2.text();

    QDomElement element_3 = object_node->firstChildElement("separatorBackend");
    if (element_3.isNull())
        element_3 = object_node->firstChildElement("seperatorBackend");
    // backwards compatibility with the "seperator" typo

    if (!element_3.isNull())
        d->list_backend_separator = element_3.text();

    // Note: Do the value only after the separators have been restored:
    QDomElement element_4 = object_node->firstChildElement("value");
    if (!element_4.isNull()) {
        d->value = listToBackendFormat(element_4.text());
        has_value = true;
    }

    QDomElement element_5 = object_node->firstChildElement("note");
    if (!element_5.isNull())
        d->note = element_5.text();
    QDomElement element_6 = object_node->firstChildElement("editable");
    if (!element_6.isNull()) {
        if (element_6.text().compare("true",Qt::CaseInsensitive) == 0)
            d->editable = true;
        else
            d->editable = false;
    }

    setModificationState(true);
    if (has_name && has_value)
        return IExportable::Complete;
    else
        return IExportable::Incomplete;
}


void GenericProperty::setMacroMode(GenericProperty::MacroMode macro_mode) {
    d->macro_mode = macro_mode;
}

GenericProperty::MacroMode GenericProperty::macroMode() const {
    return d->macro_mode;
}

void GenericProperty::setIsMacro(bool is_macro) {
    d->is_macro = is_macro;

    // Set the value and default value to the property name:
    setValueString("%{" + propertyName() + "}");
    setDefaultValueString("%{" + propertyName() + "}");
    setCategory(QtilitiesCategory(qti_def_GENERIC_PROPERTY_CATEGORY_MACROS));
    setEditable(false);
    setType(TypeString);
}

bool GenericProperty::isMacro() const {
    return d->is_macro;
}

QString GenericProperty::macroValueString(GenericProperty::MacroMode macro_mode) const {
    if (macro_mode == MacroUnexpanded)
        return "%{" + propertyName() + "}";
    else if (macro_mode == MacroExpanded)
        return valueString();
    else if (macro_mode == MacroExpandedCustom)
        return valueString();

    Q_ASSERT(0);
    return valueString();
}


bool GenericProperty::isModified() const {
    return d->is_modified;
}

void GenericProperty::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

QString GenericProperty::listToBackendFormat(const QString &value) const {
    if (d->list_storage_separator.isEmpty() || d->list_backend_separator.isEmpty())
        return value;
    else if (d->list_storage_separator == d->list_backend_separator)
        return value;
    else
        return value.split(d->list_storage_separator,QString::SkipEmptyParts).join(d->list_backend_separator);
}

QString GenericProperty::listToStorageFormat(const QString &value) const {
    if (d->list_storage_separator.isEmpty() || d->list_backend_separator.isEmpty())
        return value;
    else if (d->list_storage_separator == d->list_backend_separator)
        return value;
    else
        return value.split(d->list_backend_separator,QString::SkipEmptyParts).join(d->list_storage_separator);
}

}
}
