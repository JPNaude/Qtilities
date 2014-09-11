/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include <QApplication>
#include <QtGui>

#include <QtilitiesCoreGui>

using namespace QtilitiesCoreGui;

int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setOrganizationDomain("Qtilities");
    QtilitiesApplication::setApplicationName("Properties Example");
    QtilitiesApplication::setApplicationVersion(QtilitiesApplication::qtilitiesVersionString());

#ifdef QTILITIES_PROPERTY_BROWSER
    // Demonstrate property browsers in Qtilities:
    QWidget* widget = new QWidget;
    QtilitiesApplication::setMainWindow(widget);
    QHBoxLayout* layout = new QHBoxLayout(widget);

    // QObject Properties (Q_PROPERTY)
    ObjectPropertyBrowser* browser_qproperty = new ObjectPropertyBrowser;
    browser_qproperty->setObject(widget);
    layout->addWidget(browser_qproperty);

    // Dynamic Properties:
    // -> Shared Properties:
    QObject* obj_dynamic = new QObject;
    ObjectManager::setSharedProperty(obj_dynamic,"Boolean",false);
    ObjectManager::setSharedProperty(obj_dynamic,"String","Hello World");

    // -> Dynamic Properties:
    Observer* observerA = new Observer("My Observer A","Example observer description");
    Observer* observerB = new Observer("My Observer B","Example observer description");
    MultiContextProperty multi_context_property("Multi Context Property");
    multi_context_property.setIsExportable(false);
    observerA->attachSubject(obj_dynamic);
    observerB->attachSubject(obj_dynamic);
    multi_context_property.addContext(QVariant(true),observerA->observerID());
    multi_context_property.addContext(QVariant(false),observerB->observerID());
    ObjectManager::setMultiContextProperty(obj_dynamic,multi_context_property);

    ObjectDynamicPropertyBrowser* browser_dynamic = new ObjectDynamicPropertyBrowser;
    // Shows/hides qtilities properties used by observers (uncomment to see)
    // browser_dynamic->toggleQtilitiesProperties(true);
    browser_dynamic->setObject(obj_dynamic);
    layout->addWidget(browser_dynamic);

    // Generic Properties:
    GenericPropertyManager generic_property_manager;
    GenericPropertyBrowser* browser_generic = new GenericPropertyBrowser(&generic_property_manager);
    layout->addWidget(browser_generic);

    // Generic properties are smart, they have default values, specialized types etc.
    // Next we will create all of the possible types:
    // For simple types, we can add them directly like this:
    generic_property_manager.addProperty("Boolean",false);
    generic_property_manager.addProperty("String","Hello World");
    generic_property_manager.addProperty("Integer",5);
    generic_property_manager.addProperty("Double",1.234);

    // Now, more advanced types:
    GenericProperty* gen_enum_prop = new GenericProperty("Enum");
    gen_enum_prop->setType(GenericProperty::TypeEnum);
    QStringList possible_values;
    possible_values << "Value 1";
    possible_values << "Value 2";
    possible_values << "Value 3";
    gen_enum_prop->setEnumPossibleValuesDisplayed(possible_values);
    gen_enum_prop->setDefaultValueString(possible_values.front());
    gen_enum_prop->setCategory(QtilitiesCategory("Category 1"));
    generic_property_manager.addProperty(gen_enum_prop);

    GenericProperty* gen_prop_file = new GenericProperty("File");
    gen_prop_file->setType(GenericProperty::TypeFile);
    gen_prop_file->setDefaultValueString(QApplication::applicationFilePath());
    gen_prop_file->setCategory(QtilitiesCategory("Files and Paths"));
    generic_property_manager.addProperty(gen_prop_file);

    GenericProperty* gen_prop_file_list = new GenericProperty("File List");
    gen_prop_file_list->setType(GenericProperty::TypeFileList);
    QStringList file_list;
    file_list << QApplication::applicationFilePath();
    file_list << QApplication::applicationFilePath();
    file_list << "Dummy File";
    gen_prop_file_list->setFileList(file_list);
    gen_prop_file_list->setCategory(QtilitiesCategory("Files and Paths"));
    generic_property_manager.addProperty(gen_prop_file_list);

    GenericProperty* gen_prop_path = new GenericProperty("Path");
    gen_prop_path->setType(GenericProperty::TypePath);
    gen_prop_path->setDefaultValueString(QApplication::applicationDirPath());
    gen_prop_path->setCategory(QtilitiesCategory("Files and Paths"));
    generic_property_manager.addProperty(gen_prop_path);

    GenericProperty* gen_prop_path_list = new GenericProperty("Path List");
    gen_prop_path_list->setType(GenericProperty::TypeFileList);
    QStringList path_list;
    path_list << QApplication::applicationDirPath();
    path_list << QApplication::applicationDirPath();
    path_list << "Dummy Path";
    gen_prop_path_list->setFileList(path_list);
    gen_prop_path_list->setCategory(QtilitiesCategory("Files and Paths"));
    generic_property_manager.addProperty(gen_prop_path_list);

    widget->resize(900,500);
    widget->show();
#else
    QLabel* no_properties_label = new QLabel("Property browsers not available, enable QTILITIES_PROPERTY_BROWSER in Dependencies.pri");
    no_properties_label->show();
#endif

    return a.exec();
}
