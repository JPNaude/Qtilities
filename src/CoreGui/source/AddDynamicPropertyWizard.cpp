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

#include "AddDynamicPropertyWizard.h"

#include <QtilitiesCoreApplication>

#include "QtilitiesCoreGuiConstants.h"
using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Icons;

#include <QListWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QtAlgorithms>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QFormLayout>

struct Qtilities::CoreGui::AddDynamicPropertyWizardPrivateData {
    AddDynamicPropertyWizardPrivateData() : warn_about_duplicate_warnings(true) {}

    QPointer<QObject> obj;
    QList<IAvailablePropertyProvider*> property_providers;
    QList<PropertySpecification> available_properties;

    ObjectManager::PropertyTypes new_property_type;

    // UI controls for property selection wizard page:
    QPointer<QWizardPage> wizard_page_available_properties;
    QListWidget property_list;
    QTextEdit property_description;

    // UI controls for property details wizard page:
    QPointer<QWizardPage> wizard_page_property_details;
    QLabel property_details_heading_label;

    // Widget containing custom property type selection controls (hidden when not custom property):
    QWidget custom_property_widget;
    QLineEdit property_name_edit;
    QListWidget property_types_list;

    // Widget containing message icon and label:
    QWidget message_widget;
    QLabel message_icon;
    QLabel message_label;

    bool validation_enabled;

    AddDynamicPropertyWizard::PropertyCreationHint property_creation_hint;
    QVariant constructed_property;
    bool warn_about_duplicate_warnings;
};

Qtilities::CoreGui::AddDynamicPropertyWizard::AddDynamicPropertyWizard(PropertyCreationHint property_creation_hint, QWidget *parent) :
    QWizard(parent)
{
    d = new AddDynamicPropertyWizardPrivateData;
    d->validation_enabled = true;
    d->new_property_type = ObjectManager::NonQtilitiesProperties;
    d->property_creation_hint = property_creation_hint;
    d->property_description.setReadOnly(true);

    setWindowTitle(tr("Adding Dynamic Property"));

    // Initialize the providers:
    initializePropertyProviders();

    if (!d->wizard_page_available_properties) {
        // Add the wizard page which allows selection of the available properties (only if there are any):
        d->wizard_page_available_properties = new QWizardPage;
        d->wizard_page_available_properties->setTitle(tr("Available Properties"));

        QVBoxLayout *wizard_page_layout = new QVBoxLayout;
        QLabel *label_heading = new QLabel(tr("Please select the property you want to add from the list of available properties. Existing properties are colored in gray.<br>"));
        label_heading->setWordWrap(true);
        wizard_page_layout->addWidget(label_heading);

        QHBoxLayout *selection_layout = new QHBoxLayout;
        selection_layout->addWidget(&d->property_list);
        selection_layout->addWidget(&d->property_description);
        wizard_page_layout->addLayout(selection_layout);

        d->wizard_page_available_properties->setLayout(wizard_page_layout);
        addPage(d->wizard_page_available_properties);

        connect(&d->property_list,SIGNAL(currentTextChanged(QString)),SLOT(handleSelectedPropertyChanged(QString)));
    }

    if (!d->wizard_page_property_details) {
        d->wizard_page_property_details = new QWizardPage;
        d->wizard_page_property_details->setTitle(tr("Property Details"));

        // The heading:
        QVBoxLayout *wizard_page_layout = new QVBoxLayout;
        d->property_details_heading_label.setText(tr("Heading Not Set!"));
        d->property_details_heading_label.setWordWrap(true);
        wizard_page_layout->addWidget(&d->property_details_heading_label);

        // The custom property controls:
        QFormLayout *wizard_custom_property_layout = new QFormLayout(&d->custom_property_widget);
        wizard_custom_property_layout->setMargin(0);
        wizard_custom_property_layout->addRow(tr("Name:"),&d->property_name_edit);
        connect(&d->property_name_edit,SIGNAL(textChanged(QString)),SLOT(handleCustomPropertyNameChanged(QString)));
        wizard_custom_property_layout->addRow(tr("Type:"),&d->property_types_list);
        QStringList type_names;
        type_names << QVariant::typeToName(QVariant::Bool);
        //type_names << QVariant::typeToName(QVariant::Brush);
        //type_names << QVariant::typeToName(QVariant::Color);
        type_names << QVariant::typeToName(QVariant::Date);
        //type_names << QVariant::typeToName(QVariant::Font);
        //type_names << QVariant::typeToName(QVariant::Size);
        type_names << QVariant::typeToName(QVariant::String);
        type_names << QVariant::typeToName(QVariant::UInt);
        type_names << QVariant::typeToName(QVariant::Int);
        type_names << QVariant::typeToName(QVariant::Double);
        qSort(type_names);
        d->property_types_list.addItems(type_names);
        QList<QListWidgetItem *> string_items = d->property_types_list.findItems(QVariant::typeToName(QVariant::String),Qt::MatchExactly);
        if (string_items.count() > 0) {
            d->property_types_list.setCurrentRow(d->property_types_list.row(string_items.front()));
        } else
            d->property_types_list.setCurrentRow(0);
        wizard_page_layout->addWidget(&d->custom_property_widget);

        wizard_page_layout->addStretch();

        // The message area:
        QHBoxLayout *wizard_message_layout = new QHBoxLayout(&d->message_widget);
        wizard_message_layout->setMargin(0);
        wizard_message_layout->addStretch();
        wizard_message_layout->addWidget(&d->message_icon);
        wizard_message_layout->addWidget(&d->message_label);
        wizard_page_layout->addWidget(&d->message_widget);
        d->message_widget.setVisible(false);

        d->wizard_page_property_details->setLayout(wizard_page_layout);
        addPage(d->wizard_page_property_details);
    }

    connect(this,SIGNAL(currentIdChanged(int)),SLOT(handleCurrentIdChanged()));
}

Qtilities::CoreGui::AddDynamicPropertyWizard::~AddDynamicPropertyWizard() {
    delete d;
}

AddDynamicPropertyWizard::PropertyCreationHint AddDynamicPropertyWizard::propertyCreationHint() const {
    return d->property_creation_hint;
}

QVariant AddDynamicPropertyWizard::constructedProperty() const {
    return d->constructed_property;
}

void AddDynamicPropertyWizard::accept() {
    // Determine the property name to use:
    QString property_name = newPropertyName();
    QString selected_available_property = selectedAvailableProperty();
    QByteArray property_name_ba = property_name.toUtf8();
    const char* char_property_name = property_name_ba.data();

    // Validate the name:
    if (!validateNewPropertyName(newPropertyName()))
        return;

    bool success = false;
    if (d->property_creation_hint == ConstructAndAdd) {
        // Determine the property type to use:
        QVariant::Type selected_type;
        if (isCurrentCustomProperty()) {
            QByteArray property_type_ba = d->property_types_list.currentItem()->text().toUtf8();
            const char* type_name = property_type_ba.data();
            selected_type = QVariant::nameToType(type_name);
        } else
            selected_type = propertyType(selected_available_property);

        d->constructed_property = QtilitiesProperty::constructVariant(selected_type,propertyDefaultValue(selected_available_property).toString());
        if (d->new_property_type == ObjectManager::SharedProperties) {
            SharedProperty new_shared_property(char_property_name,d->constructed_property);
            if (propertyReadOnly(selected_available_property))
                new_shared_property.makeReadOnly();
            if (!propertyRemovable(selected_available_property))
                new_shared_property.makeNotRemovable();
            new_shared_property.setIsExportable(propertyIsExportable(selected_available_property));
            if (ObjectManager::setSharedProperty(d->obj,new_shared_property))
                success = true;
        } else if (d->new_property_type == ObjectManager::NonQtilitiesProperties) {
            if (!d->obj->setProperty(char_property_name,d->constructed_property))
                success = true;
        }
    } else {
        success = true;
    }

    if (!success) {
        d->message_icon.setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(16,16));
        d->message_label.setText(tr("Oops, something went wrong and your property could not be added."));
        d->message_widget.setVisible(true);
        // Don't reject, the user can cancel if he wants to abort.
    } else {
        d->message_widget.setVisible(false);
        d->validation_enabled = false;
        QWizard::accept();
        d->validation_enabled = true;
    }
}

bool AddDynamicPropertyWizard::validateCurrentPage() {
    if (!d->validation_enabled)
        return true;

    if (currentId() > 0) {
        return validateNewPropertyName(newPropertyName());
    } else
        return true;
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::setNewPropertyType(ObjectManager::PropertyTypes new_property_type) {
    if (new_property_type == ObjectManager::NonQtilitiesProperties || new_property_type == ObjectManager::SharedProperties)
        d->new_property_type = new_property_type;
}

ObjectManager::PropertyTypes Qtilities::CoreGui::AddDynamicPropertyWizard::newPropertyType() const {
    return d->new_property_type;
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::setObject(QObject* obj) {
    if (!obj) {
        handleObjectDestroyed();
        return;
    }

    if (d->obj == obj) {
        // Get available properties again in order to refresh display for existing properties:
        getAvailableProperties();
        return;
    }

    if (d->obj)
        d->obj->disconnect(this);

    d->obj = obj;
    connect(d->obj,SIGNAL(destroyed(QObject*)),SLOT(handleObjectDestroyed()));

    // Get available properties for the new object:
    d->validation_enabled = true;
    getAvailableProperties();
//    if (!obj->objectName().isEmpty())
//        setWindowTitle(tr("Adding Dynamic Property To \"") + obj->objectName() + "\"");
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::setObject(QPointer<QObject> obj) {
    QObject* object = obj;
    setObject(object);
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::setObject(QList<QObject*> objects) {
    if (objects.count() == 1)
        setObject(objects.front());
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::setObject(QList<QPointer<QObject> > objects) {
    if (objects.count() == 1)
        setObject(objects.front());
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::handleObjectDestroyed() {
    setEnabled(false);
    if (d->obj)  {
        d->obj->disconnect(this);
        d->obj->removeEventFilter(this);
    }

    d->obj = 0;
    return;
}

void AddDynamicPropertyWizard::handleSelectedPropertyChanged(const QString &property_displayed_name) {
    if (!d->obj) {
        d->property_description.setText(tr("Object not specified"));
        return;
    }

    // Load the correct description:
    QString new_text;
    new_text.append("<b>" + property_displayed_name + "</b>");
    QString description = propertyDescription(property_displayed_name);
    if (!description.isEmpty()) {
        new_text.append("<br><br>");
        new_text.append(description);
    }
    new_text.append("<br><br>");
    QString default_value = propertyDefaultValue(property_displayed_name).toString();
    if (default_value.isEmpty())
        new_text.append(QString(tr("<b>Default value:</b>  Not specified<br>")));
    else
        new_text.append(QString(tr("<b>Default value:</b>  %1<br>")).arg(default_value));

    #ifndef QT_NO_DEBUG
    new_text.append(QString(tr("<b>Type:</b>  %1<br>")).arg(QVariant::typeToName(propertyType(property_displayed_name))));
    new_text.append(QString(tr("<b>Actual name:</b>  %1<br>")).arg(propertyName(property_displayed_name)));

    if (d->new_property_type == ObjectManager::SharedProperties) {
        new_text.append(QString(tr("<b>Removable:</b>  %1<br>")).arg(QVariant(propertyRemovable(property_displayed_name)).toString()));
        new_text.append(QString(tr("<b>Read only:</b>  %1<br>")).arg(QVariant(propertyReadOnly(property_displayed_name)).toString()));
    }
    #endif

    d->property_description.setText(new_text);

    if (isCurrentCustomProperty()) {
        d->property_details_heading_label.setText(tr("Please specify the details of your custom property:"));
        d->custom_property_widget.setVisible(true);
    } else {
        if (d->obj->objectName().isEmpty())
            d->property_details_heading_label.setText(QString(tr("You are ready to add new property \"%1\" to your object")).arg(property_displayed_name));
        else
            d->property_details_heading_label.setText(QString(tr("You are ready to add new property \"%1\" to \"%2\"")).arg(property_displayed_name).arg(d->obj->objectName()));
        d->custom_property_widget.setVisible(false);
    }
}

void AddDynamicPropertyWizard::handleCustomPropertyNameChanged(const QString &name) {
    validateNewPropertyName(name);
}

void AddDynamicPropertyWizard::handleCurrentIdChanged() {
    validateNewPropertyName(newPropertyName());
}

bool AddDynamicPropertyWizard::validateNewPropertyName(const QString &property_name) {
    if (property_name.isEmpty()) {
        d->message_icon.setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(16,16));
        d->message_label.setText(tr("Please specify a name for your property."));
        d->message_widget.setVisible(true);
        return false;
    }

    if (!d->obj) {
        d->message_icon.setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(16,16));
        d->message_label.setText(tr("Object not specified on which property must be set."));
        d->message_widget.setVisible(true);
        return false;
    }

    if (d->warn_about_duplicate_warnings) {
        // Check if a property with the same name already exists:
        bool exists = false;
        QList<QByteArray> existing_properties = d->obj->dynamicPropertyNames();
        foreach (const QByteArray& property, existing_properties) {
            QString name(property.data());
            if (name == property_name) {
                exists = true;
                break;
            }
        }

        if (exists) {
            // Check if its a SharedProperty, and if so we check if its read only:
            SharedProperty shared_property = ObjectManager::getSharedProperty(d->obj,property_name.toUtf8().data());
            if (shared_property.isValid()) {
                // Check if its read only:
                if (shared_property.isReadOnly() || !shared_property.isRemovable()) {
                    // Its not a shared property, thus we can just prompt that it will be overwritten.
                    d->message_icon.setPixmap(QIcon(qti_icon_ERROR_16x16).pixmap(16,16));
                    d->message_label.setText(tr("A <b>read-only/non-removable</b> property with the same name already exists."));
                    d->message_widget.setVisible(true);
                    return false;
                } else {
                    // Its not a shared property, thus we can just prompt that it will be overwritten.
                    d->message_icon.setPixmap(QIcon(qti_icon_WARNING_16x16).pixmap(16,16));
                    d->message_label.setText(tr("A property with the same name already exists, it will be overwritten."));
                    d->message_widget.setVisible(true);
                }
            } else {
                // Its not a shared property, thus we can just prompt that it will be overwritten.
                d->message_icon.setPixmap(QIcon(qti_icon_WARNING_16x16).pixmap(16,16));
                d->message_label.setText(tr("A property with the same name already exists, it will be overwritten."));
                d->message_widget.setVisible(true);
            }
        } else
            d->message_widget.setVisible(false);
    } else
        d->message_widget.setVisible(false);

    return true;
}

QString AddDynamicPropertyWizard::newPropertyName() const {
    if (isCurrentCustomProperty())
        return d->property_name_edit.text();
    else
        return propertyName(selectedAvailableProperty());
}

QString AddDynamicPropertyWizard::selectedAvailableProperty() const {
    QListWidgetItem* item = d->property_list.currentItem();
    if (!item)
        return "";
    else
        return item->text();
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::initializePropertyProviders() {
    d->property_providers.clear();

    // Get a list of all the property providers in the system:
    QList<QObject*> propertyProviderObjects = OBJECT_MANAGER->registeredInterfaces("com.Qtilities.Core.IAvailablePropertyProvider/1.0");
    // Check all items
    for (int i = 0; i < propertyProviderObjects.count(); ++i) {
        IAvailablePropertyProvider* provider = qobject_cast<IAvailablePropertyProvider*> (propertyProviderObjects.at(i));
        if (provider)
            d->property_providers << provider;
    }
}

void Qtilities::CoreGui::AddDynamicPropertyWizard::getAvailableProperties() const {
    d->available_properties.clear();

    QList<PropertySpecification> all_properties;
    foreach (IAvailablePropertyProvider* provider, d->property_providers)
        all_properties << provider->availableProperties();

    foreach (PropertySpecification property, all_properties) {
        if (property.isValid()) {
            if (property.d_internal)
                continue;

            if (property.d_class_name.isEmpty()) {
                d->available_properties << property;
                continue;
            } else {
                if (d->obj->inherits(property.d_class_name.toUtf8().data())) {
                    d->available_properties << property;
                    continue;
                }
            }
        }
    }

    disconnect(&d->property_list,SIGNAL(currentTextChanged(QString)),this,SLOT(handleSelectedPropertyChanged(QString)));
    d->property_list.clear();
    connect(&d->property_list,SIGNAL(currentTextChanged(QString)),SLOT(handleSelectedPropertyChanged(QString)));

    QStringList available_property_name = availablePropertyDisplayedNames();
    foreach (const QString& name, available_property_name) {
        QListWidgetItem* item = new QListWidgetItem(name);

        QString actual_property_name = propertyName(name);
        if (ObjectManager::propertyExists(d->obj,actual_property_name.toUtf8().data()))
            item->setForeground(QBrush(Qt::gray));
        d->property_list.addItem(item);
    }

    d->property_list.setCurrentRow(0);
}

bool AddDynamicPropertyWizard::prohibitCustomProperties() const {
    QStringList base_classes;
    foreach (IAvailablePropertyProvider* provider, d->property_providers)
        base_classes << provider->prohibitedCustomPropertiesClasses();

    foreach (const QString& base_class, base_classes) {
        if (d->obj->inherits(base_class.toUtf8().data()))
            return true;
    }

    return false;
}

QStringList AddDynamicPropertyWizard::availablePropertyDisplayedNames() const {
    QStringList property_names;
    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.isValid())
            property_names << prop.d_displayed_name;
    }

    // Check if any IAvailablePropertyProvider indicates that custom properties are NOT allowed for this object type:
    if (!prohibitCustomProperties())
        property_names.append(tr("Custom Property"));

    qSort(property_names);
    return property_names;
}

QString AddDynamicPropertyWizard::propertyDescription(const QString &displayed_name) const {
    if (isCurrentCustomProperty())
        return QString(tr("A custom property which you define."));

    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.d_displayed_name == displayed_name) {
            if (prop.isValid())
                return prop.d_description;
        }
    }

    return QString();
}

QString AddDynamicPropertyWizard::propertyName(const QString &displayed_name) const {
    if (isCurrentCustomProperty())
        return QString(tr("Not defined yet"));

    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.d_displayed_name == displayed_name) {
            if (prop.isValid()) {
                if (prop.d_property_name.isEmpty())
                    return displayed_name;
                else
                    return prop.d_property_name;
            }
        }
    }

    return QString();
}

bool AddDynamicPropertyWizard::propertyRemovable(const QString &displayed_name) const {
    if (isCurrentCustomProperty())
        return true;

    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.d_displayed_name == displayed_name) {
            if (prop.isValid())
                return prop.d_removable;
        }
    }

    return true;
}

bool AddDynamicPropertyWizard::propertyIsExportable(const QString &displayed_name) const {
    if (isCurrentCustomProperty())
        return true;

    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.d_displayed_name == displayed_name) {
            if (prop.isValid())
                return prop.d_is_exportable;
        }
    }

    return true;
}

bool AddDynamicPropertyWizard::propertyReadOnly(const QString &displayed_name) const {
    if (isCurrentCustomProperty())
        return false;

    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.d_displayed_name == displayed_name) {
            if (prop.isValid())
                return prop.d_read_only;
        }
    }

    return false;
}

QVariant AddDynamicPropertyWizard::propertyDefaultValue(const QString &displayed_name) const {
    if (isCurrentCustomProperty())
        return "";

    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.d_displayed_name == displayed_name) {
            if (prop.isValid()) {
                if (!prop.d_default_value.isNull())
                    return prop.d_default_value;
                else
                    return "";
            }
        }
    }

    return "";
}

QVariant::Type AddDynamicPropertyWizard::propertyType(QString displayed_name) const {
    if (isCurrentCustomProperty()) {
        if (d->property_types_list.currentItem())
            return QVariant::nameToType(d->property_types_list.currentItem()->text().toUtf8().data());
        else
            return QVariant::String;
    }

    if (displayed_name.isEmpty())
        displayed_name = selectedAvailableProperty();

    for (int i = 0; i < d->available_properties.count(); ++i) {
        PropertySpecification prop = d->available_properties.at(i);
        if (prop.d_displayed_name == displayed_name) {
            if (prop.isValid())
                return prop.d_data_type;
        }
    }
    return QVariant::Invalid;
}

void AddDynamicPropertyWizard::setWarnAboutDuplicateWarnings(bool enable) {
    d->warn_about_duplicate_warnings = enable;
}

bool AddDynamicPropertyWizard::warnAboutDuplicateWarnings() const {
    return d->warn_about_duplicate_warnings;
}

bool AddDynamicPropertyWizard::isCurrentCustomProperty() const {
    return (selectedAvailableProperty() == tr("Custom Property"));
}
