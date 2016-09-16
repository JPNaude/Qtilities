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

#include "GenericPropertyManager.h"

#include <ObjectManager>
#include <Observer>
#include <ObserverHints>

#include <QDomDocument>

using namespace Qtilities::Core;

namespace Qtilities {
namespace Core {


struct GenericPropertyManagerData {
    GenericPropertyManagerData() : properties_observer("Generic Properties"),
        show_advanced_settings(false),
        show_switch_names(false),
        task_base(0) { }

    Observer            properties_observer;
    bool                show_advanced_settings;
    bool                show_switch_names;
    QString             properties_file;

    QPointer<QObject>   task_base;
    ITask*              task;
};

GenericPropertyManager::GenericPropertyManager(QObject *parent):
    QObject(parent)
{
    d = new GenericPropertyManagerData;
    connect(&d->properties_observer,SIGNAL(modificationStateChanged(bool)),SIGNAL(modificationStateChanged(bool)));
}

GenericPropertyManager::~GenericPropertyManager() {
    delete d;
}

QString GenericPropertyManager::propertiesFileName() const {
    return d->properties_file;
}

void GenericPropertyManager::clear() {
    QList<GenericProperty*> props_to_delete;

    int count = d->properties_observer.subjectCount();
    for (int i = 0; i < count; ++i) {
        GenericProperty *prop = qobject_cast<GenericProperty*> (d->properties_observer.subjectAt(i));
        Q_ASSERT(prop);
        if (prop) {
//            if (!prop->isInternal())
                props_to_delete << prop;
        }
    }

    qDeleteAll(props_to_delete);
    refresh();
}

Observer *GenericPropertyManager::propertiesObserver() const {
    return &d->properties_observer;
}

bool GenericPropertyManager::hasAdvancedSettings() const {
    QList<GenericProperty*> properties = allProperties();
    foreach (GenericProperty* prop, properties) {
        if (prop->level() == GenericProperty::LevelAdvanced)
            return true;
    }
    return false;
}

bool GenericPropertyManager::showAdvancedSettings() const {
    return d->show_advanced_settings;
}

void GenericPropertyManager::setShowAdvancedSettings(bool show) {
    if (d->show_advanced_settings != show) {
        d->show_advanced_settings = show;
        emit toggleAdvancedSettings(show);
    }
}

bool GenericPropertyManager::showSwitchNames() const {
    return d->show_switch_names;
}

void GenericPropertyManager::setShowSwitchNames(bool show) {
    if (d->show_switch_names != show) {
        d->show_switch_names = show;
        emit toggleSwitchNames(show);
    }
}

bool GenericPropertyManager::hasModifiedProperties(QStringList* modified_property_list) const {
    QStringList modified_property_list_int;

    QList<GenericProperty*> properties = allProperties();
    foreach (GenericProperty* prop, properties) {
        if (!prop->matchesDefault() && (prop->category().toString() != qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL)) {
            modified_property_list_int << QString("%4. %1: %2 (Default = %3)").arg(prop->propertyName()).arg(prop->valueString()).arg(prop->defaultValueString()).arg(modified_property_list_int.count() + 1);
        }
    }

    if (modified_property_list)
        *modified_property_list = modified_property_list_int;

    return (modified_property_list_int.count() > 0);
}

GenericProperty *GenericPropertyManager::containsProperty(const QString &property_name, bool search_aliases, const QString &alias_environment) const {
    // Search each property:
    for (int i = 0; i < d->properties_observer.subjectCount(); ++i) {
        GenericProperty* prop = qobject_cast<GenericProperty*> (d->properties_observer.subjectAt(i));
        Q_ASSERT(prop);
        if (prop) {
            if (prop->matchesPropertyName(property_name,search_aliases,alias_environment))
                return prop;
        }
    }

    return 0;
}

QList<GenericProperty *> GenericPropertyManager::allProperties(bool only_editable, bool only_non_default) const {
    QList<GenericProperty*> properties;
    for (int i = 0; i < d->properties_observer.subjectCount(); ++i) {
        GenericProperty* prop = qobject_cast<GenericProperty*> (d->properties_observer.subjectAt(i));
        if (prop) {
            if (only_editable && only_non_default) {
                if (prop->editable() && !prop->matchesDefault())
                    properties << prop;
            } else if (only_editable) {
                if (prop->editable())
                    properties << prop;
            } else if (only_non_default) {
                if (!prop->matchesDefault())
                    properties << prop;
            } else
                properties << prop;
        }
    }
    return properties;
}

QList<GenericProperty *> GenericPropertyManager::allProperties(const QtilitiesCategory &filter_category, bool invert_filter) const {
    QList<GenericProperty*> properties;
    for (int i = 0; i < d->properties_observer.subjectCount(); ++i) {
        GenericProperty* prop = qobject_cast<GenericProperty*> (d->properties_observer.subjectAt(i));
        if (prop) {
            if (invert_filter) {
                if (filter_category != prop->category())
                    properties << prop;
            } else {
                if (filter_category == prop->category())
                    properties << prop;
            }
        }
    }
    return properties;
}

GenericProperty *GenericPropertyManager::addProperty(const QString &property_name, QVariant value, bool refresh_browser) {
    GenericProperty* prop = containsProperty(property_name);
    if (prop)
        return prop;
    else {
        prop = new GenericProperty;
        prop->setPropertyName(property_name);
        QString error_msg;
        if (d->properties_observer.attachSubject(prop,Observer::SpecificObserverOwnership,&error_msg)) {
            connectToProperty(prop);

            if (value.isValid()) {
                if (value.type() == QVariant::Bool)
                    prop->setType(GenericProperty::TypeBool);
                else if (value.type() == QVariant::Int)
                    prop->setType(GenericProperty::TypeInteger);
                else if (value.type() == QVariant::Double)
                    prop->setType(GenericProperty::TypeDouble);
                else if (value.type() == QVariant::String)
                    prop->setType(GenericProperty::TypeString);
                prop->setDefaultValueString(value.toString());
                prop->setValue(value);
            }

            if (refresh_browser)
                emit refresh();
            return prop;
        } else {
            LOG_ERROR(error_msg);
            delete prop;
        }
    }

    return 0;
}

bool GenericPropertyManager::addProperty(GenericProperty *property, bool refresh_browser) {
    if (!property)
        return false;
    GenericProperty* existing_prop = containsProperty(property->propertyName());
    if (existing_prop)
        return false;
    else {
        QString error_msg;
        if (d->properties_observer.attachSubject(property,Observer::SpecificObserverOwnership,&error_msg)) {
            connectToProperty(property);
            if (refresh_browser)
                emit refresh();
            return true;
        } else {
            LOG_ERROR(error_msg);
            return false;
        }
    }
}

void GenericPropertyManager::addProperties(QList<GenericProperty *> properties) {
    bool do_refresh = false;
    for (int i = 0; i < properties.count(); i++) {
        if (addProperty(properties.at(i),false))
            do_refresh = true;
    }
    if (do_refresh)
        refresh();
}

void GenericPropertyManager::addProperties(QList<QPointer<GenericProperty> > properties) {
    bool do_refresh = false;
    for (int i = 0; i < properties.count(); i++) {
        if (addProperty(properties.at(i),false))
            do_refresh = true;
    }
    if (do_refresh)
        refresh();
}

bool GenericPropertyManager::removeProperty(const QString &property_name, bool refresh_browser) {
    GenericProperty* prop = containsProperty(property_name);
    if (prop) {
        // SpecificObserverOwnership, thus will be removed from observer.
        delete prop;
        if (refresh_browser)
            refresh();
        return true;
    } else {
        return false;
    }
}

bool GenericPropertyManager::setPropertyValue(const QString &property_name, QVariant value) {
    GenericProperty* prop = containsProperty(property_name);
    if (prop)  {
        prop->setValue(value);
        return true;
    }
    return false;
}

bool GenericPropertyManager::setPropertyValueString(const QString &property_name, const QString &value) {
    GenericProperty* prop = containsProperty(property_name);
    if (prop) {
        prop->setValueString(value);
        return true;
    }
    return false;
}

QString GenericPropertyManager::getPropertyValueString(const QString &property_name) const {
    GenericProperty* prop = containsProperty(property_name);
    if (prop)
        return prop->valueString();
    return "";
}

QVariant GenericPropertyManager::getPropertyValue(const QString &property_name) const {
    GenericProperty* prop = containsProperty(property_name);
    if (prop)
        return prop->value();
    return QVariant();
}

// --------------------------------
// Saving and Loading
// --------------------------------

Qtilities::Core::Interfaces::IExportable::ExportResultFlags GenericPropertyManager::loadDefaultProperties(const QString &file_name, ITask *task_ref, bool add_property_file_property) {
    QFile file(file_name);

    if (!file.exists()) {
        LOG_TASK_ERROR("Failed to find property file for step: \"" + objectName() + "\". Build property file expected at: " + file_name,task_ref);
        return IExportable::Failed;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return IExportable::Failed;

    d->properties_observer.startProcessingCycle();
    clear();

    Qtilities::Core::Interfaces::IExportable::ExportResultFlags result = IExportable::Complete;

    // We ignore the first line always since it contains the headers.
    int count = 0;
    while (!file.atEnd()) {
        QString line = file.readLine();
        ++count;
        if (count == 1)
            continue;

        // Construct a new property:
        GenericProperty* prop = new GenericProperty;
        if (!prop->fromCsvString(line)) {
            result = IExportable::Incomplete;
            delete prop;
        } else {
            QString error_msg;
            if (!d->properties_observer.attachSubject(prop,Observer::SpecificObserverOwnership,&error_msg)) {
                LOG_TASK_ERROR(error_msg,task_ref);
                delete prop;
            } else {
                connect(prop,SIGNAL(valueChanged(GenericProperty*)),SIGNAL(propertyValueChanged(GenericProperty*)));
                connect(prop,SIGNAL(editableChanged(GenericProperty*)),SIGNAL(propertyEditableChanged(GenericProperty*)));
                connect(prop,SIGNAL(contextDependentChanged(GenericProperty*)),SIGNAL(propertyContextDependentChanged(GenericProperty*)));
                connect(prop,SIGNAL(possibleValuesDisplayedChanged(GenericProperty*)),SIGNAL(propertyPossibleValuesChanged(GenericProperty*)));
                connect(prop,SIGNAL(defaultValueChanged(GenericProperty*)),SIGNAL(propertyDefaultValueChanged(GenericProperty*)));
                connect(prop,SIGNAL(noteChanged(GenericProperty*)),SIGNAL(propertyNoteChanged(GenericProperty*)));

                MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
                category_property.setValue(qVariantFromValue(prop->category()),d->properties_observer.observerID());
                ObjectManager::setMultiContextProperty(prop,category_property);
            }
        }
    }

    d->properties_file = file_name;

    // The properties file property:
    if (add_property_file_property) {
        QString file_property_name = tr("Properties File");
        GenericProperty* file_prop = containsProperty(file_property_name);
        if (!file_prop) {
            file_prop = new GenericProperty;
            file_prop->setPropertyName(file_property_name);
            file_prop->setEditable(false);
            file_prop->setCategory(QtilitiesCategory(qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL));
            file_prop->setIsExportable(false);
            file_prop->setType(GenericProperty::TypeString);
            file_prop->setLevel(GenericProperty::LevelAdvanced);
        }
        file_prop->setDefaultValueString(file_name);
        file_prop->setValueString(file_name);
        d->properties_observer << file_prop;
    }

//    MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
//    category_property.setValue(qVariantFromValue(QtilitiesCategory(qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL)),d->properties_observer.observerID());
//    ObjectManager::setMultiContextProperty(file_prop,category_property);

    d->properties_observer.endProcessingCycle();

    emit refresh();
    return result;
}

IExportable::ExportResultFlags GenericPropertyManager::loadNewPropertiesFile(const QString &file_name, ITask *task_ref) {
    QFile file(file_name);

    if (!file.exists()) {
        LOG_TASK_ERROR("Failed to find property file for step: \"" + objectName() + "\". Build property file expected at: " + file_name,task_ref);
        return IExportable::Failed;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return IExportable::Failed;

    // First make a clone of the current set of properties:
    QString errorMsg;
    Observer* properties_clone = d->properties_observer.duplicateInstance<Observer>(&d->properties_observer,&errorMsg);
    if (!properties_clone) {
        LOG_TASK_ERROR(errorMsg,task_ref);
        return IExportable::Failed;
    }

    // Now call loadDefaultProperties() with the file to construct a new clean baseline:
    // clear() happens in here:
    if (loadDefaultProperties(file_name,task_ref) != IExportable::Complete) {
        return IExportable::Failed;
    }

    // Complete assumes all properties were found and matched to their old values:
    Qtilities::Core::Interfaces::IExportable::ExportResultFlags result = IExportable::Complete;

    QList<QObject*> objects = properties_clone->subjectReferences();
    foreach (QObject* obj, objects) {
        GenericProperty* reference_prop = qobject_cast<GenericProperty*> (obj);
        if (reference_prop) {
            if (reference_prop->isInternal())
                continue;

            GenericProperty* new_prop = containsProperty(reference_prop->propertyName(),true);
            if (new_prop) {
                // TODO: What happens here if we can't set the property to that value. For example an enum with a value that does not exist anymore.
                // Another example, what if an integer is outside its bounds.
                if (new_prop->valueString() != reference_prop->valueString()) {
                    QString errorMsg;
                    QString old_value = new_prop->valueString();
                    if (new_prop->setValueString(reference_prop->valueString().split(reference_prop->listSeparatorBackend()).join(new_prop->listSeparatorBackend()),&errorMsg))
                        LOG_TASK_INFO(QString("Successfully matched and updated property \"%1\" with new value \"%2\" (old value \"%3\").").arg(reference_prop->propertyName()).arg(reference_prop->valueString()).arg(old_value),task_ref);
                    else
                        LOG_TASK_WARNING(QString("Successfully matched property \"%1\" with new value \"%2\", however updating the property value failed with error: %3").arg(reference_prop->propertyName()).arg(reference_prop->valueString()).arg(errorMsg),task_ref);
                } else
                    LOG_TASK_INFO(QString("Property \"%1\" with value \"%2\" matched, but is the same and won't be updated.").arg(reference_prop->propertyName()).arg(reference_prop->valueString()),task_ref);
            } else {
                LOG_TASK_WARNING(QString("Property \"%1\" with value \"%2\" could not be matched during property value matching. This property does not exist in the new set of properties.").arg(reference_prop->propertyName()).arg(reference_prop->valueString()),task_ref);
                result = IExportable::Incomplete;
            }
        }
    }

    emit refresh();
    return result;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags GenericPropertyManager::exportManagerProperties(QDomDocument* doc,
                                                                                                            QDomElement* object_node,
                                                                                                            bool export_non_default,
                                                                                                            bool export_default,
                                                                                                            const QString& export_node_name) const {
    Q_UNUSED(doc)

    // Go through all properties and check which ones do not match their default states:
    QList<GenericProperty*> all_properties = allProperties();
    QList<GenericProperty*> changed_properties;
    foreach (GenericProperty* prop, all_properties) {
        if (prop->isExportable()) {
            if (export_non_default && !prop->matchesDefault()) {
                changed_properties << prop;
                continue;
            }

            if (export_default && prop->matchesDefault())
                changed_properties << prop;
        }
    }

    // Now export selected properties:
    bool all_successful = true;
    foreach (GenericProperty* prop, changed_properties) {
        if (!prop->isMacro()) {
            // Create new node:
            QDomElement prop_element = doc->createElement(export_node_name);
            object_node->appendChild(prop_element);
            if (prop->exportXml(doc,&prop_element) != IExportable::Complete) {
                LOG_TASK_WARNING("Export of build property \"" + prop->propertyName() + "\" was not complete.",exportTask());
                all_successful = false;
            }
        }
    }

    if (all_successful)
        return IExportable::Complete;
    else
        return IExportable::Incomplete;
}

Qtilities::Core::Interfaces::IExportable::ExportResultFlags GenericPropertyManager::importManagerProperties(QDomDocument* doc,
                                                                                                            QDomElement* object_node,
                                                                                                            const QString &export_node_name) {
    Q_UNUSED(doc)
    if (!object_node)
        return IExportable::Failed;

    IExportable::ExportResultFlags result = IExportable::Complete;
    QList<QPointer<QObject> > import_list;
    QDomNodeList itemNodes = object_node->childNodes();
    for(int i = 0; i < itemNodes.count(); ++i) {
        QDomNode itemNode = itemNodes.item(i);
        QDomElement item = itemNode.toElement();

        if (item.isNull())
            continue;

        if (item.tagName() == export_node_name) {
            GenericProperty tmp_prop;
            if (tmp_prop.importXml(doc,&item,import_list) == IExportable::Complete) {
                // Now find the matching property, and set its value and editability:
                GenericProperty* matching_property = containsProperty(tmp_prop.propertyName());
                if (matching_property) {
                    //qDebug() << "Found matching property" << tmp_prop.propertyName();
                    matching_property->setValueFromProperty(&tmp_prop);
                    matching_property->setEditable(tmp_prop.editable());
                } else {
                    //qDebug() << "Failed to find matching property" << tmp_prop.propertyName();

                    // In this case we add the missing property:
                    matching_property = addProperty(tmp_prop.propertyName());
                    matching_property->importXml(doc,&item,import_list);
//                    // Only give this message when it happens on the main property set, not the reference property set:
//                    if (element_name == QLatin1String("Property"))
//                        LOG_TASK_INFO("Found property in build step which does not match any of the default properties with name: " + tmp_prop.propertyName() + ". This property will be added.",exportTask());
                }
            } else
                result = IExportable::Incomplete;
        }
    }

    return result;
}

IExportable::ExportResultFlags GenericPropertyManager::exportMacros(GenericProperty::MacroMode macro_mode, QDomDocument *doc, QDomElement *object_node) {
    Q_UNUSED(doc)
    QList<GenericProperty*> macros = macroProperties(macro_mode);

    // Now export only changed properties:
    bool all_successful = true;
    int count = 0;
    foreach (GenericProperty* prop, macros) {
        // Create new node:
        QDomElement macro_element = doc->createElement("scineric:macro");
        object_node->appendChild(macro_element);
        if (prop->exportXml(doc,&macro_element) != IExportable::Complete) {
            LOG_TASK_WARNING("Export of build macro \"" + prop->propertyName() + "\" was not complete.",exportTask());
            all_successful = false;
        }
        ++count;
    }

    if (all_successful)
        return IExportable::Complete;
    else
        return IExportable::Incomplete;
}

IExportable::ExportResultFlags GenericPropertyManager::importMacros(GenericProperty::MacroMode macro_mode, QDomDocument *doc, QDomElement *object_node) {
    Q_UNUSED(doc)
    if (!object_node)
        return IExportable::Failed;

    IExportable::ExportResultFlags result = IExportable::Complete;
    QList<QPointer<QObject> > import_list;
    QDomNodeList itemNodes = object_node->childNodes();
    for(int i = 0; i < itemNodes.count(); ++i) {
        QDomNode itemNode = itemNodes.item(i);
        QDomElement item = itemNode.toElement();

        if (item.isNull())
            continue;

        if (item.tagName() == QLatin1String("scineric:macro")) {
            GenericProperty tmp_prop;
            if (tmp_prop.importXml(doc,&item,import_list) == IExportable::Complete) {
                // Now find the matching property, and set its value:
                GenericProperty* matching_property = containsProperty(tmp_prop.propertyName());
                if (matching_property) {
                    matching_property->setValueFromProperty(&tmp_prop);
                    matching_property->setMacroMode(macro_mode);
                } else {
                    // In this case we add the missing property:
                    matching_property = addProperty(tmp_prop.propertyName());
                    matching_property->setIsMacro(true);
                    matching_property->importXml(doc,&item,import_list);
                    matching_property->setMacroMode(macro_mode);
                    LOG_TASK_INFO("Found macro in build configuration which does not match any of the default macros with name: " + tmp_prop.propertyName() + ". This macro will be added to the build configuration.",exportTask());
                }
            } else
                result = IExportable::Incomplete;
        }
    }

    return result;
}

QHash<QString, QString> GenericPropertyManager::macroValues(GenericProperty::MacroMode macro_mode) {
    QList<GenericProperty*> macros = macroProperties(macro_mode);
    QHash<QString,QString> values_hash;
    foreach (GenericProperty* prop, macros)
        values_hash[prop->propertyName()] = prop->valueString();
    return values_hash;
}

QList<GenericProperty *> GenericPropertyManager::macroProperties(GenericProperty::MacroMode macro_mode) {
    QList<GenericProperty*> all_properties = allProperties();
    QList<GenericProperty*> macros;
    foreach (GenericProperty* prop, all_properties) {
        if (prop->isMacro() && (prop->macroMode() & macro_mode))
            macros << prop;
    };
    return macros;
}

void GenericPropertyManager::clone(GenericPropertyManager *property_manager, bool only_state_dependent_properties) {
    Q_ASSERT(property_manager);
    if (!property_manager)
        return;

    d->properties_observer.startProcessingCycle();
    // Don't use normal clear() here since it does not remove all properties.
    d->properties_observer.deleteAll();

    for (int i = 0; i < property_manager->propertiesObserver()->subjectCount(); ++i) {
        GenericProperty* prop = qobject_cast<GenericProperty*> (property_manager->propertiesObserver()->subjectAt(i));
        Q_ASSERT(prop);
        if (!prop)
            continue;

        if (only_state_dependent_properties) {
            if (!prop->stateDependent() || (prop->category().toString() == qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL))
                continue;
        }

        GenericProperty* duplicate = new GenericProperty(*prop);
        d->properties_observer.attachSubject(duplicate);
    }
    d->properties_observer.endProcessingCycle();
}

bool GenericPropertyManager::compare(GenericPropertyManager *property_manager,
                                       PropertyDiffInfo *property_diff_info,
                                       bool state_independent_properties,
                                       bool context_dependent_properties) const {
    if (!property_manager)
        return false;

    bool identical = true;

    // Check for removed & changed properties:
    for (int i = 0; i < d->properties_observer.subjectCount(); ++i) {
        if (!property_diff_info && !identical)
            return false;

        GenericProperty* prop = qobject_cast<GenericProperty*> (d->properties_observer.subjectAt(i));
        Q_ASSERT(prop);
        if (!prop)
            continue;

        if (prop->propertyName().isEmpty())
            continue;

        if (prop->category().toString() == qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL)
            continue;

        if (!state_independent_properties && !prop->stateDependent())
            continue;

        if (!context_dependent_properties && prop->contextDependent())
            continue;

        // Find the corresponding property:
        GenericProperty* ref_prop = property_manager->containsProperty(prop->propertyName());
        if (ref_prop) {
            if (!prop->compareValue(ref_prop)) {
                identical = false;
                if (property_diff_info) {
                    property_diff_info->d_changed_properties[prop->propertyName()] = prop->valueString() + "," + ref_prop->valueString();
                }
            }
        } else {
            identical = false;
            if (property_diff_info)
                property_diff_info->d_removed_properties[prop->propertyName()] = prop->valueString();
        }
    }

    // Check for added properties:
    for (int i = 0; i < property_manager->propertiesObserver()->subjectCount(); ++i) {
        if (!property_diff_info && !identical)
            return false;

        GenericProperty* ref_prop = qobject_cast<GenericProperty*> (property_manager->propertiesObserver()->subjectAt(i));
        Q_ASSERT(ref_prop);
        if (!ref_prop)
            continue;

        if (ref_prop->propertyName().isEmpty())
            continue;

        if (ref_prop->category().toString() == qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL)
            continue;

        // Find the corresponding property:
        GenericProperty* prop = containsProperty(ref_prop->propertyName());
        if (!prop) {
            identical = false;
            if (property_diff_info)
                property_diff_info->d_added_properties[ref_prop->propertyName()] = ref_prop->valueString();
        }
    }

    return identical;
}

void GenericPropertyManager::setExportTask(ITask *task) {
    if (!task) {
        d->task_base = 0;
        d->task = 0;
    } else {
        d->task_base = task->objectBase();
        d->task = task;
    }
}

ITask *GenericPropertyManager::exportTask() const {
    if (d->task_base)
        return d->task;
    else
        return 0;
}

void GenericPropertyManager::clearExportTask() {
    d->task = 0;
    d->task_base = 0;
}

bool GenericPropertyManager::isModified() const {
    // Remember, properties' modification state are checked in there. And their state
    // is only changed for value and editability.
    return d->properties_observer.isModified();
}

void GenericPropertyManager::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    d->properties_observer.setModificationState(new_state,IModificationNotifier::NotifySubjects,true);
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

void GenericPropertyManager::connectToProperty(GenericProperty *property) {
    if (!property)
        return;

    connect(property,SIGNAL(valueChanged(GenericProperty*)),SIGNAL(propertyValueChanged(GenericProperty*)));
    connect(property,SIGNAL(editableChanged(GenericProperty*)),SIGNAL(propertyEditableChanged(GenericProperty*)));
    connect(property,SIGNAL(contextDependentChanged(GenericProperty*)),SIGNAL(propertyContextDependentChanged(GenericProperty*)));
    connect(property,SIGNAL(possibleValuesDisplayedChanged(GenericProperty*)),SIGNAL(propertyPossibleValuesChanged(GenericProperty*)));
    connect(property,SIGNAL(defaultValueChanged(GenericProperty*)),SIGNAL(propertyDefaultValueChanged(GenericProperty*)));
    connect(property,SIGNAL(noteChanged(GenericProperty*)),SIGNAL(propertyNoteChanged(GenericProperty*)));
}

}
}

