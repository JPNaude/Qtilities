/****************************************************************************
**
** Copyright 2010-2013, CSIR
** Author: JP Naude, jpnaude@csir.co.za
**
****************************************************************************/

#include "GenericPropertyBrowser.h"
#include "GenericPropertyTypeManagers.h"

#ifdef QTILITIES_PROPERTY_BROWSER
#include <QtilitiesCoreGui>

#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QAction>
#include <QToolBar>

#include <qtvariantproperty.h>
#include <qtgroupboxpropertybrowser.h>
#include <qttreepropertybrowser.h>
#include <qtpropertybrowser.h>
#include <qtbuttonpropertybrowser.h>
#include <QtSpinBoxFactory>

using namespace QtilitiesCoreGui;
namespace Qtilities {
namespace CoreGui {


struct GenericPropertyBrowserPrivateData {
    GenericPropertyBrowserPrivateData() : current_edited_property(0),
        read_only(false) {}
    ~GenericPropertyBrowserPrivateData() {
        if (property_browser)
            delete property_browser;
        if (category_property_manager)
            delete category_property_manager;
        if (int_property_manager)
            delete int_property_manager;
        if (double_property_manager)
            delete double_property_manager;
        if (enum_property_manager)
            delete enum_property_manager;
        if (bool_property_manager)
            delete bool_property_manager;
        if (string_property_manager)
            delete string_property_manager;
        if (path_property_manager)
            delete path_property_manager;
    }

    QList<QtProperty*>                      top_level_properties;

    QtAbstractPropertyBrowser*              property_browser;

    // Category property manager:
    QtVariantPropertyManager*               category_property_manager;
    // Int property manager:
    QtIntPropertyManager*                   int_property_manager;
    // Double property manager:
    QtDoublePropertyManager*                double_property_manager;
    // Enum property manager:
    QtEnumPropertyManager*                  enum_property_manager;
    // Bool property manager:
    QtBoolPropertyManager*                  bool_property_manager;
    // String property manager:
    QtStringPropertyManager*                string_property_manager;
    // File property manager:
    PathPropertyManager*                    path_property_manager;

    bool                                    ignore_build_display_side_property_changes;

    QPointer<GenericPropertyManager>        generic_property_manager;

    QHash<GenericProperty*,QtProperty*>     property_link_map;
    QMap<GenericProperty*,QPointer<GenericProperty> >   generic_property_safe_map;
    //! When this property is changed, we should not respond to its changed from the build side.
    QtProperty *                            current_edited_property;

    bool                                    read_only;
};

GenericPropertyBrowser::GenericPropertyBrowser(GenericPropertyManager* property_manager, BrowserType browser_type, QWidget *parent) : QMainWindow(parent)
{
    d = new GenericPropertyBrowserPrivateData;
    d->generic_property_manager = property_manager;
    d->ignore_build_display_side_property_changes = false;

    if (browser_type == TreeBrowser) {
        QtTreePropertyBrowser* property_browser = new QtTreePropertyBrowser(this);
        property_browser->setRootIsDecorated(false);
        d->property_browser = property_browser;
    } else if (browser_type == GroupBoxBrowser) {
        QtGroupBoxPropertyBrowser* property_browser = new QtGroupBoxPropertyBrowser(this);
        d->property_browser = property_browser;
    } else if (browser_type == ButtonBrowser) {
        QtButtonPropertyBrowser* property_browser = new QtButtonPropertyBrowser(this);
        d->property_browser = property_browser;
    }

    d->property_browser->layout()->setMargin(0);
    setCentralWidget(d->property_browser);

    // Create variant property manager for editable properties:
    d->category_property_manager = new QtVariantPropertyManager(this);

    // Create int property manager for editable properties:
    d->int_property_manager = new QtIntPropertyManager(this);
    QtSpinBoxFactory *int_factory = new QtSpinBoxFactory(this);
    d->property_browser->setFactoryForManager(d->int_property_manager, int_factory);
    connect(d->int_property_manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(handle_intPropertyChanged(QtProperty*,int)));

    // Create int property manager for editable properties:
    d->double_property_manager = new QtDoublePropertyManager(this);
    QtDoubleSpinBoxFactory *double_factory = new QtDoubleSpinBoxFactory(this);
    d->property_browser->setFactoryForManager(d->double_property_manager, double_factory);
    connect(d->double_property_manager, SIGNAL(valueChanged(QtProperty *, double)),
                this, SLOT(handle_doublePropertyChanged(QtProperty*,double)));

    // Create enum property manager for editable properties:
    d->enum_property_manager = new QtEnumPropertyManager(this);
    QtEnumEditorFactory *enum_factory = new QtEnumEditorFactory(this);
    d->property_browser->setFactoryForManager(d->enum_property_manager, enum_factory);
    connect(d->enum_property_manager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(handle_enumPropertyChanged(QtProperty*,int)));

    // Create string property manager for editable properties:
    d->string_property_manager = new QtStringPropertyManager(this);
    QtLineEditFactory *string_factory = new QtLineEditFactory(this);
    d->property_browser->setFactoryForManager(d->string_property_manager, string_factory);
    connect(d->string_property_manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(handle_stringPropertyChanged(QtProperty*,const QString &)));

    // Create bool property manager for editable properties:
    d->bool_property_manager = new QtBoolPropertyManager(this);
    QtCheckBoxFactory *check_box_factory = new QtCheckBoxFactory(this);
    d->property_browser->setFactoryForManager(d->bool_property_manager, check_box_factory);
    connect(d->bool_property_manager, SIGNAL(valueChanged(QtProperty *, bool)),
                this, SLOT(handle_boolPropertyChanged(QtProperty*, bool)));

    // Create path property manager for editable properties:
    d->path_property_manager = new PathPropertyManager(this);
    FileEditorFactory* file_factory = new FileEditorFactory(this);
    d->property_browser->setFactoryForManager(d->path_property_manager, file_factory);
    connect(d->path_property_manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(handle_pathPropertyChanged(QtProperty*,const QString &)));

    d->ignore_build_display_side_property_changes = true;
    inspectPropertyManager();
    d->ignore_build_display_side_property_changes = false;

    connect(d->generic_property_manager,SIGNAL(destroyed()),SLOT(handleObjectDeleted()));
    connect(d->generic_property_manager,SIGNAL(toggleAdvancedSettings(bool)),SLOT(toggleAdvancedSettings(bool)));
    connect(d->generic_property_manager,SIGNAL(toggleSwitchNames(bool)),SLOT(toggleSwitchNames(bool)));
    connect(d->generic_property_manager,SIGNAL(refresh()),SLOT(refresh()));

    connect(d->generic_property_manager,SIGNAL(propertyValueChanged(GenericProperty*)),SLOT(handlePropertyValueChanged(GenericProperty*)));
    connect(d->generic_property_manager,SIGNAL(propertyEditableChanged(GenericProperty*)),SLOT(handlePropertyEditableChanged(GenericProperty*)));
    connect(d->generic_property_manager,SIGNAL(propertyContextDependentChanged(GenericProperty*)),SLOT(handlePropertyContextDependentChanged(GenericProperty*)));
    connect(d->generic_property_manager,SIGNAL(propertyPossibleValuesChanged(GenericProperty*)),SLOT(handlePropertyPossibleValuesChanged(GenericProperty*)));
    connect(d->generic_property_manager,SIGNAL(propertyDefaultValueChanged(GenericProperty*)),SLOT(handlePropertyDefaultValueChanged(GenericProperty*)));
    connect(d->generic_property_manager,SIGNAL(propertyNoteChanged(GenericProperty*)),SLOT(handlePropertyNoteChanged(GenericProperty*)));
}

GenericPropertyBrowser::~GenericPropertyBrowser() {
    delete d;
}

QtAbstractPropertyBrowser *GenericPropertyBrowser::abstractPropertyBrowser() const {
    return d->property_browser;
}

QSize GenericPropertyBrowser::sizeHint() const {
    if (d->property_browser) {
        if (d->property_browser->sizeHint().isValid() && d->property_browser->sizeHint().width() != 0
            && d->property_browser->sizeHint().height() != 0)
            return d->property_browser->sizeHint();
    }

    return this->size();
}

void GenericPropertyBrowser::clear() {
    // File property manager:
    d->path_property_manager->clear();
    d->int_property_manager->clear();
    d->double_property_manager->clear();
    d->enum_property_manager->clear();
    d->bool_property_manager->clear();
    d->string_property_manager->clear();
    d->property_link_map.clear();
    d->generic_property_safe_map.clear();
    QListIterator<QtProperty *> it(d->top_level_properties);
    while (it.hasNext()) {
        d->property_browser->removeProperty(it.next());
    }
    d->top_level_properties.clear();
}

void GenericPropertyBrowser::setReadOnly(bool read_only) {
    if (d->read_only != read_only) {
        d->read_only = read_only;

        if (read_only) {
            // Set all displayed properties to non-editable:
            foreach (QtProperty* prop, d->top_level_properties)
                prop->setEnabled(false);
        } else {
            // Important: Set the category (parent) properties first, then the sub properties, otherwise
            // setting the category to enabled sets all sub properties as enabled.
            // All categories should be editable in this case:
            QSetIterator<QtProperty *> i(d->category_property_manager->properties());
            while (i.hasNext()) {
                QtProperty* prop = i.next();
                prop->setEnabled(true);
                //qDebug() << "Setting enabled" << true << prop->propertyName();
            }

            // Set only displayed properties which has editable backend properties to be editable:
            for (int i = 0; i < d->generic_property_safe_map.count(); ++i) {
                GenericProperty* step_prop = d->generic_property_safe_map.keys().at(i);
                Q_ASSERT(d->generic_property_safe_map.contains(step_prop));
                QPointer<GenericProperty> step_prop_safe = d->generic_property_safe_map[step_prop];

                QtProperty* browser_prop = 0;
                if (d->property_link_map.contains(step_prop))
                    browser_prop = d->property_link_map[step_prop];

                if (!browser_prop) {
                    LOG_ERROR(QString(Q_FUNC_INFO) + ". Failed to get browser property.");
                    continue;
                }

                if (!step_prop_safe) {
                    LOG_ERROR("Failed to get backend property for browser property: " + browser_prop->propertyName() + ", removing it from the property browser for now.");
                    d->property_browser->removeProperty(browser_prop);
                } else {
                    // Path related properties are not set disabled to allow inspecting path lists, and to be able to copy normal paths (not lists):
                    if (step_prop_safe->type() == GenericProperty::TypeFile ||
                            step_prop_safe->type() == GenericProperty::TypeFileList ||
                            step_prop_safe->type() == GenericProperty::TypePath ||
                            step_prop_safe->type() == GenericProperty::TypePathList) {
                        browser_prop->setEnabled(step_prop_safe->editable());
                    } else {
                        browser_prop->setEnabled(step_prop_safe->editable() && !step_prop_safe->contextDependent());
                    }
                }
            }
        }
    }
}

bool GenericPropertyBrowser::readOnly() const {
    return d->read_only;
}

void GenericPropertyBrowser::handle_intPropertyChanged(QtProperty *property, int value) {
    updatePropertyValue(property,value);
}

void GenericPropertyBrowser::handle_doublePropertyChanged(QtProperty *property, double value) {
    updatePropertyValue(property,value);
}

void GenericPropertyBrowser::handle_stringPropertyChanged(QtProperty *property, const QString & value) {
    updatePropertyValue(property,value);
}

void GenericPropertyBrowser::handle_pathPropertyChanged(QtProperty *property, const QString & value) {
    updatePropertyValue(property,value);
}

void GenericPropertyBrowser::handle_boolPropertyChanged(QtProperty *property, bool value) {
    updatePropertyValue(property,value);
}

void GenericPropertyBrowser::handle_enumPropertyChanged(QtProperty *property, int index) {
    // Get the new value using the index:
    QString new_value = d->enum_property_manager->enumNames(property).at(index);
    updatePropertyValue(property,new_value);
}

void GenericPropertyBrowser::updatePropertyValue(QtProperty *property, const QVariant & value) {
    if (d->ignore_build_display_side_property_changes)
        return;

    if (!d->generic_property_manager)
        return;

    QString property_name_string = property->propertyName();
    if (d->generic_property_manager->showSwitchNames()) {
        if (property_name_string.contains(" (")) {
            int pos = property_name_string.indexOf(" (");
            property_name_string.chop(property_name_string.length() - pos);
        }
    }

    QByteArray ba = property_name_string.toUtf8();
    const char* property_name = ba.data();

    // Get the property, and update its value:
    GenericProperty* prop = d->generic_property_manager->containsProperty(property_name);
    if (!prop)
        return;

    d->current_edited_property = property;
    property->setToolTip(getToolTipText(prop));

    prop->setValue(value);

    // When changed from the display side and its a macro, we should
    // set the macro's mode as MacroExpandedCustom:
    if (prop->isMacro())
        prop->setMacroMode(GenericProperty::MacroExpandedCustom);

    d->current_edited_property = 0;
}

void GenericPropertyBrowser::handleObjectDeleted() {
    clear();
}

void GenericPropertyBrowser::toggleAdvancedSettings(bool show) {
    Q_UNUSED(show)
    refresh();
}

void GenericPropertyBrowser::toggleSwitchNames(bool show) {
    Q_UNUSED(show)
    refresh();
}

void GenericPropertyBrowser::refresh() {
    d->ignore_build_display_side_property_changes = true;
    inspectPropertyManager();
    d->ignore_build_display_side_property_changes = false;
}

void GenericPropertyBrowser::handlePropertyValueChanged(GenericProperty *property) {
    if (d->property_link_map.contains(property)) {
        QtProperty* display_property = d->property_link_map[property];
        if (!display_property)
            return;

//        if (display_property == d->current_edited_property)
//            return;

        // Now we must get the correct manager for it:
        d->ignore_build_display_side_property_changes = true;
        if (property->type() == GenericProperty::TypeInteger)
            d->int_property_manager->setValue(display_property,property->intValue());
        else if (property->type() == GenericProperty::TypeDouble)
            d->double_property_manager->setValue(display_property,property->doubleValue());
        else if (property->type() == GenericProperty::TypeString)
            d->string_property_manager->setValue(display_property,property->valueString());
        else if (property->type() == GenericProperty::TypeFile || property->type() == GenericProperty::TypeFileList || property->type() == GenericProperty::TypePath || property->type() == GenericProperty::TypePathList)
            d->path_property_manager->notifyPropertyChanged(display_property);
        else if (property->type() == GenericProperty::TypeEnum)
            d->enum_property_manager->setValue(display_property,property->enumPossibleValuesDisplayed().indexOf(property->valueString()));
        else if (property->type() == GenericProperty::TypeBool)
            d->bool_property_manager->setValue(display_property,property->boolValue());

        display_property->setModified(!property->matchesDefault());
        display_property->setToolTip(getToolTipText(property));
        d->ignore_build_display_side_property_changes = false;
    }
}

void GenericPropertyBrowser::handlePropertyEditableChanged(GenericProperty *property) {
    if (d->property_link_map.contains(property)) {
        QtProperty* display_property = d->property_link_map[property];
        if (!display_property)
            return;

        if (display_property == d->current_edited_property)
            return;

        if (display_property) {
            // Path related properties are not set disabled to allow inspecting path lists, and to be able to copy normal paths (not lists):
            if (property->type() == GenericProperty::TypeFile ||
                    property->type() == GenericProperty::TypeFileList ||
                    property->type() == GenericProperty::TypePath ||
                    property->type() == GenericProperty::TypePathList) {
                display_property->setEnabled(property->editable());
            } else {
                display_property->setEnabled(property->editable() && !property->contextDependent());
            }
        }

        display_property->setModified(!property->matchesDefault());
    }
}

void GenericPropertyBrowser::handlePropertyContextDependentChanged(GenericProperty *property) {
    handlePropertyEditableChanged(property);
}

void GenericPropertyBrowser::handlePropertyPossibleValuesChanged(GenericProperty *property) {
    if (d->property_link_map.contains(property)) {
        QtProperty* display_property = d->property_link_map[property];

        // Now we must get the correct manager for it:
        if (property->type() == GenericProperty::TypeEnum) {
            // setEnumNames() changes the value to the first one in the list! Thus keep the current value and set it again.
            QString current_value = property->valueString();
            d->enum_property_manager->setEnumNames(display_property,property->enumPossibleValuesDisplayed());
            d->enum_property_manager->setValue(display_property,property->enumPossibleValuesDisplayed().indexOf(current_value));
        } else if (property->type() == GenericProperty::TypeInteger) {
            int current_value = property->intValue();
            d->int_property_manager->setMaximum(display_property,property->intMax());
            d->int_property_manager->setMinimum(display_property,property->intMin());
            d->int_property_manager->setSingleStep(display_property,property->intStep());
            d->int_property_manager->setValue(display_property,current_value);
        } else if (property->type() == GenericProperty::TypeDouble) {
            int current_value = property->doubleValue();
            d->double_property_manager->setMaximum(display_property,property->doubleMax());
            d->double_property_manager->setMinimum(display_property,property->doubleMin());
            d->double_property_manager->setSingleStep(display_property,property->doubleStep());
            d->double_property_manager->setValue(display_property,current_value);
        }

        display_property->setModified(!property->matchesDefault());
    }
}

void GenericPropertyBrowser::handlePropertyDefaultValueChanged(GenericProperty *property) {
    if (d->property_link_map.contains(property)) {
        QtProperty* display_property = d->property_link_map[property];
        display_property->setModified(!property->matchesDefault());
        display_property->setToolTip(getToolTipText(property));
    }
}

void GenericPropertyBrowser::handlePropertyNoteChanged(GenericProperty *property) {
    if (d->property_link_map.contains(property)) {
        QtProperty* display_property = d->property_link_map[property];
        display_property->setToolTip(getToolTipText(property));
    }
}

void GenericPropertyBrowser::inspectPropertyManager() {
    clear();

    QMap<QtilitiesCategory,QtProperty*> category_property_map;
    QList<QObject*> objects = d->generic_property_manager->propertiesObserver()->subjectReferences();
    bool use_switch_names = d->generic_property_manager->showSwitchNames();
    for (int s = 0; s < objects.count(); s++) {
        GenericProperty* prop = qobject_cast<GenericProperty*> (objects.at(s));
        if (!prop)
            continue;

        // Check if this property is visible:
        if (!prop->visible())
            continue;
        // Check if this property is advanced, and if so if it must be shown:
        if (prop->level() == GenericProperty::LevelAdvanced && !d->generic_property_manager->showAdvancedSettings())
            continue;

        QtProperty* sub_property = 0;
        QString name = prop->propertyName();
        if (use_switch_names) {
            if (!prop->switchName().isEmpty())
                name.append(" (" + prop->switchName() + ")");
        }
        if (prop->type() == GenericProperty::TypeInteger) {
            sub_property = d->int_property_manager->addProperty(name);
            if (sub_property) {
                d->int_property_manager->setValue(sub_property,prop->intValue());
                d->int_property_manager->setMaximum(sub_property,prop->intMax());
                d->int_property_manager->setMinimum(sub_property,prop->intMin());
                d->int_property_manager->setSingleStep(sub_property,prop->intStep());
            }
        } else if (prop->type() == GenericProperty::TypeDouble) {
            sub_property = d->double_property_manager->addProperty(name);
            if (sub_property) {
                d->double_property_manager->setValue(sub_property,prop->doubleValue());
                d->double_property_manager->setMaximum(sub_property,prop->doubleMax());
                d->double_property_manager->setMinimum(sub_property,prop->doubleMin());
                d->double_property_manager->setSingleStep(sub_property,prop->doubleStep());
            }
        } else if (prop->type() == GenericProperty::TypeString) {
            sub_property = d->string_property_manager->addProperty(name);
            if (sub_property) {
                d->string_property_manager->setValue(sub_property,prop->valueString());
                if (!prop->stringRegExp().isEmpty() && prop->stringRegExp().isValid())
                    d->string_property_manager->setRegExp(sub_property,prop->stringRegExp());
            }
        } else if (prop->type() == GenericProperty::TypeEnum) {
            sub_property = d->enum_property_manager->addProperty(name);
            if (sub_property) {
                d->enum_property_manager->setEnumNames(sub_property,prop->enumPossibleValuesDisplayed());
                d->enum_property_manager->setValue(sub_property,prop->enumPossibleValuesDisplayed().indexOf(prop->valueString()));
            }
        } else if (prop->type() == GenericProperty::TypeBool) {
            sub_property = d->bool_property_manager->addProperty(name);
            if (sub_property) {
                d->bool_property_manager->setValue(sub_property,prop->boolValue());
            }
        } else if (prop->type() == GenericProperty::TypeVariant) {
            // Note that TypeVariant properties do not respond to changes from the property's side, or from the editor's side.
            sub_property = d->category_property_manager->addProperty(QtVariantPropertyManager::groupTypeId(),name);
            if (sub_property)
                d->category_property_manager->setValue(sub_property,prop->value());
        } else if (prop->type() == GenericProperty::TypeFile || prop->type() == GenericProperty::TypeFileList || prop->type() == GenericProperty::TypePath || prop->type() == GenericProperty::TypePathList) {
            sub_property = d->path_property_manager->addProperty(name);
            if (sub_property)
                d->path_property_manager->addPropertyData(sub_property,prop);
        }

        if (sub_property) {
            sub_property->setModified(!prop->matchesDefault());
            sub_property->setToolTip(getToolTipText(prop));

            d->property_link_map[prop] = sub_property;
            QPointer<GenericProperty> safe_prop = prop;
            d->generic_property_safe_map[prop] = safe_prop;

            // Get the category:
            if (prop->category().isEmpty()) {
                d->property_browser->addProperty(sub_property);
                d->top_level_properties << sub_property;
            } else {
                #ifndef QT_DEBUG
                if (prop->category() == QtilitiesCategory(qti_def_GENERIC_PROPERTY_CATEGORY_INTERNAL)) {
                    // We hide it in release mode:
                    d->property_link_map.remove(prop);
                    d->generic_property_safe_map.remove(prop);
                    delete sub_property;
                    continue;
                }
                #endif

                if (category_property_map.contains(prop->category())) {
                    category_property_map[prop->category()]->addSubProperty(sub_property);
                } else {
                    // Make the category property:
                    QtProperty* category_property = d->category_property_manager->addProperty(QtVariantPropertyManager::groupTypeId(),prop->category().toString());
                    d->category_property_manager->setValue(category_property,prop->category().toString());
                    category_property->setEnabled(!d->read_only);
                    category_property->addSubProperty(sub_property);
                    category_property_map[prop->category()] = category_property;

                    d->property_browser->addProperty(category_property);
                    d->top_level_properties << category_property;
                }
            }

            // Path related properties are not set disabled to allow inspecting path lists, and to be able to copy normal paths (not lists):
            if (prop->type() == GenericProperty::TypeFile ||
                    prop->type() == GenericProperty::TypeFileList ||
                    prop->type() == GenericProperty::TypePath ||
                    prop->type() == GenericProperty::TypePathList) {
                sub_property->setEnabled(prop->editable());
            } else {
                sub_property->setEnabled(prop->editable() && !prop->contextDependent());
            }
        }
    }
}

QString GenericPropertyBrowser::getToolTipText(GenericProperty *property) {
    QStringList tooltip_text;
    if (!property->description().isEmpty())
        tooltip_text.append(property->description() + "<br>");

    if (!property->valueString().isEmpty())
        tooltip_text.append(QString("<span style=\"color: gray;\">%1: %2</span>").arg(tr("Current Value")).arg(property->valueString()));

    if (!property->defaultValueString().isEmpty() && !property->switchName().isEmpty())
        tooltip_text.append(QString("<span style=\"color: gray;\">%1: %2<br>%3: %4</span>").arg(tr("Default")).arg(property->defaultValueString()).arg(tr("Switch Name")).arg(property->switchName()));
    else if (!property->defaultValueString().isEmpty())
        tooltip_text.append(QString("<span style=\"color: gray;\">%1: %2</span>").arg(tr("Default")).arg(property->defaultValueString()));
    else if (!property->switchName().isEmpty())
        tooltip_text.append(QString("<span style=\"color: gray;\">%1: %2</span>").arg(tr("Switch Name")).arg(property->switchName()));

//    if (property->contextDependent())
//        tooltip_text.append("<span style=\"color: gray;\">Context Dependent: Yes</span>");
//    else
//        tooltip_text.append("<span style=\"color: gray;\">Context Dependent: No</span>");
//    if (property->stateDependent())
//        tooltip_text.append("<span style=\"color: gray;\">State Dependent: Yes</span>");
//    else
//        tooltip_text.append("<span style=\"color: gray;\">State Dependent: No</span>");
//    if (property->isInternal())
//        tooltip_text.append("<span style=\"color: gray;\">Internal: Yes</span>");
//    else
//        tooltip_text.append("<span style=\"color: gray;\">Internal: No</span>");
//    if (property->isExportable())
//        tooltip_text.append("<span style=\"color: gray;\">Exportable: Yes</span>");
//    else
//        tooltip_text.append("<span style=\"color: gray;\">Exportable: No</span>");

    if (!property->note().isEmpty())
        tooltip_text.append("<span style=\"color: gray;\"><br>Note: " + property->note() + "</span>");

    return tooltip_text.join("<br>");
}


}
}

#endif
