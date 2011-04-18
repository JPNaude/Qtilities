/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#ifndef QTILITIESCORECONSTANTS_H
#define QTILITIESCORECONSTANTS_H

#include <QtCore>

namespace Qtilities {
    //! Namespace containing all the classes which forms part of the Core Module.
    namespace Core {
        //! Namespace containing constants used inside the Core Module.
        namespace Constants {

            //! The name of the global object pool observer.
            const char * const qti_def_GLOBAL_OBJECT_POOL                   = "qti.def.ObjectPool";
            //! %Factory name of the Qtilities factory.
            const char * const qti_def_FACTORY_QTILITIES                    = "qti.def.QtilitiesFactory";
            //! %Factory tag for activity policy filters.
            const char * const qti_def_FACTORY_TAG_ACTIVITY_FILTER          = "qti.def.FactoryTag.ActivityFilter";
            //! %Factory tag for subject type filters.
            const char * const qti_def_FACTORY_TAG_SUBJECT_TYPE_FILTER      = "qti.def.FactoryTag.SubjectTypeFilter";
            //! %Factory tag for observers.
            const char * const qti_def_FACTORY_TAG_OBSERVER                 = "qti.def.FactoryTag.Observer";
            //! The standard context used in the Qtilities::Core::ContextManager class. This context is always active.
            const char * const qti_def_CONTEXT_STANDARD                     = "qti.def.Context.Standard";
        }

        //! Namespace containing reserved observer properties used inside the Core Module.
        /*!
          Observer classes use the dynamic property features of QObject classes extensively to manage objects in different contexts.
          This namespace contains a set of properties which are defined and used internally in %Qtilities.

          For an overview of the different kind of properties, see the \ref observer_properties section of the \ref page_observers article, or Qtilities::Core::QtilitiesProperty. Note that all properties used by %Qtilities starts with "qti_prop".


          For an overview of property changes and changing properties see the \ref monitoring_property_changes section of the
          \ref page_observers article.
          */
        namespace Properties {

//! %Observer Subject IDs Property
/*!
This property is added to any object observed by one or more observers and
is used to keep track in which contexts the object is visible.

<b>Permisson:</b> Reserved<br>
<b>Data Type:</b> QMap<int,int> where the keys are observer IDs of the contexts in which the object is visible and values are the unqiue IDs of the object in the different observers<br>
<b>Property Type:</b> Qtilities::Core::MultiContextProperty<br>
<b>Is Exportable:</b> No<br>
<b>Change Notifications:</b> No

This property should never be used directly. To get a list of all contexts in which
an object is observed you should use the function shown in the example below:
\code
QObject* obj = new Object();
Observer* obs = new Observer("My Observer","");
obs->attachSubject(obj);

// Get the references of any objects' parents
QList<Observer*> parents = Observer::parentReferences(obj);

// Get the subject ID of an object within an observer context.
int subject_id = obs->getQtilitiesPropertyValue(obj,qti_prop_OBSERVER_MAP).toInt();
\endcode
*/
const char * const qti_prop_OBSERVER_MAP  = "qti.core.ObserverMap";

//! Object Ownership Property
/*!
When an object is attached to an observer, the observer adds this property
to the object for each context in which it is attached storing the ownership
of the object in that context.

<b>Permisson:</b> Reserved<br>
<b>Data Type:</b> int, cast to Observer::ObjectOwnership to get the ownership<br>
<b>Property Type:</b> Qtilities::Core::MultiContextProperty<br>
<b>Is Exportable:</b> No<br>
<b>Change Notifications:</b> No

Below is an example to show how you can get the ownership of an object for a
specific context.
\code
Observer* obs = new Observer("My Observer","");
QObject* obj = new QObject();
obs->attachSubject(obj);

QVariant current_ownership = obs->getQtilitiesPropertyValue(obj,qti_prop_OWNERSHIP);
Observer::ObjectOwnership ownership = (Observer::ObjectOwnership) current_ownership.toInt();
\endcode
*/
const char * const qti_prop_OWNERSHIP              = "qti.core.Ownership";

//! %Observer Parent Property
/*!
When an object is attached to an observer using specific ownership this
property will contain the observer ID of the parent observer.

<b>Permisson:</b> Reserved<br>
<b>Data Type:</b> int, this int is the ObserverID of the observer parent.<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> No<br>
<b>Change Notifications:</b> No
*/
const char * const qti_prop_PARENT_ID        = "qti.core.ParentID";

//! %Observer Limit Property
/*!
Using the observer limit property it is possible to limit the number of observers
which can observe any object at one time. The limit must be set before attaching
the object to any observers and cannot be changed afterwards.

<b>Permisson:</b> Read/Write, Reserved (after attachment)<br>
<b>Data Type:</b> int<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Yes by default, depends on usage<br>
<b>Change Notifications:</b> No<br>

Below is an example of how to add the qti_prop_OBSERVER_LIMIT property to an object.
\code
QObject* obj = new QObject();
SharedProperty observer_limit_property(qti_prop_OBSERVER_LIMIT,QVariant(1));
Observer::setSharedProperty(obj, observer_limit_property);
\endcode
*/
const char * const qti_prop_OBSERVER_LIMIT         = "qti.core.ObserverLimit";

//! %Observer Visitor ID Property
/*!
This is an internal property which is used during construction of an
Qtilities::Core::ObserverRelationalTable.

<b>Permisson:</b> Reserved<br>
<b>Data Type:</b> int<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Yes<br>
<b>Change Notifications:</b> No<br>
*/
const char * const qti_prop_VISITOR_ID    = "qti.core.VisitorID";

//! Object Limited Exports Property
/*!
This is an internal property which is used during observer exporting in order
to make sure an object found multiple times in a tree is only exported once.

<b>Permisson:</b> Reserved<br>
<b>Data Type:</b> int<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> No<br>
<b>Change Notifications:</b> No<br>
*/
const char * const qti_prop_LIMITED_EXPORTS   = "qti.core.ExcludeObj";

//! Object Access Mode Property
/*!
%Observer table and tree models uses this property when adding icons to the object access
column for non-observer subjects. For observers the Qtilities::Core::Observer::accessMode() function
is used to determine the access column icon as well as how the hierahcy below the observer is
displayed.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> int, cast to Observer::AccessMode to get the access mode<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Yes by default, depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add the qti_prop_ACCESS_MODE property to an object.
\code
QObject* obj = new QObject();
SharedProperty access_property(qti_prop_ACCESS_MODE,QVariant((int) Observer::ReadOnlyAccess));
Observer::setSharedProperty(obj, access_property);
\endcode
*/
const char * const qti_prop_ACCESS_MODE     = "qti.core.AccessMode";

//! Object Category Property
/*!
Use the object category property to specify the category to use for an object
when attaching it to an observer which has its Qtilities::Core::Observer::HierarchicalDisplay
hint set to categorized hierarchy.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QtilitiesCategory<br>
<b>Property Type:</b> Qtilities::Core::MultiContextProperty<br>
<b>Is Exportable:</b> Yes<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add the qti_prop_CATEGORY_MAP property to an object.
\code
Observer* obs = new Observer("My Observer","Observer showing categories");
obs->setHierarchicalDisplayHint(Observer::CategorizedHierarchy);

QObject* obj = new QObject();
MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
category_property.setValue(qVariantFromValue(QtilitiesCategory("Category 1")),obs->observerID());
Observer::setMultiContextProperty(obj,category_property);

obs->attachSubject(obj);
\endcode
*/
const char * const qti_prop_CATEGORY_MAP        = "qti.core.CategoryMap";

//! Object Name Property
/*!
This property holds the name of the object which is sync'ed with objectName() by the name manager observer.
For more information on this property and name managers see the Qtilities::CoreGui::NamingPolicyFilter class documentation.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QString<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> No<br>
<b>Change Notifications:</b> Yes<br>

When you want to change the name of an object, set this property to make sure that the naming policy
filter(s) managing names in context(s) where this object is attached can handle and validate the name change.
Thus do not use the setObjectName() function on QObject.

Below is an example of how you can create a new name for an object. An alternative approach
is to get the property, change it and add it again to the object. Both approaches yields the
same result.
\code
SharedProperty new_subject_name_property(qti_prop_NAME,QVariant(new_name));
new_subject_name_property.setIsExportable(false);
Observer::setSharedProperty(obj,new_subject_name_property);
\endcode
*/
const char * const qti_prop_NAME            = "qti.filters.Name";

//! Object Name Manager ID Property
/*!
This property holds the observer ID of the observer which has the subject filter installed which
manages the object name (sync'ing it with objectName()). For more information on name managers
see the Qtilities::CoreGui::NamingPolicyFilter class documentation.

<b>Permisson:</b> Reserved<br>
<b>Data Type:</b> int<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> No<br>
<b>Change Notifications:</b> No<br>
*/
const char * const qti_prop_NAME_MANAGER_ID = "qti.filters.NameManagerID";

//! Instance Names Property
/*!
Different instance names for the object in different unique contexts. For more information on instance
names see the Qtilities::CoreGui::NamingPolicyFilter class documentation.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QMap<int,QString>, where the keys are observer IDs and the values are the corresponding instance names<br>
<b>Property Type:</b> Qtilities::Core::MultiContextProperty<br>
<b>Is Exportable:</b> No<br>
<b>Change Notifications:</b> Yes<br>

\note This property is only created for naming policy filters that use \p ProhibitDuplicateNames uniqueness policy.
*/
const char * const qti_prop_ALIAS_MAP         = "qti.filters.AliasMap";

//! Object Activity Property
/*!
When the object is attached to one or more observers which have Qtilities::Core::ActivityPolicyFilter
filters installed, this property holds the activity for each of those contexts

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QMap<int,bool>, where the keys are observer IDs and the values are the corresponding activities<br>
<b>Property Type:</b> Qtilities::Core::MultiContextProperty<br>
<b>Is Exportable:</b> Yes<br>
<b>Change Notifications:</b> Yes<br>
*/
const char * const qti_prop_ACTIVITY_MAP        = "qti.filters.ActivityMap";

// -------------------------------------------------------
// Object Role Properties:
// -------------------------------------------------------

//! Object Tooltip Role Property
/*!
%Observer table and tree models uses this string property for tooltip roles if present.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QString<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> No by default, depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add the OBJECT_TOOLTIP property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_TOOLTIP,"Tooltip Text");
Observer::setSharedProperty(obj,property);
\endcode
*/
const char * const qti_prop_TOOLTIP            = "qti.role.Tooltip";

//! Object Decoration Role Property
/*!
%Observer table and tree models uses this icon property for decoration if present.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QColor, QIcon or QPixmap<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_DECORATION,QVariant(QIcon(QString(":/icon_name.png"))));
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_DECORATION            = "qti.role.Decoration";

//! Object Whats This Role Property
/*!
If an object has this property, the property's value will be used by observer item models when data is requested
for the Qt::WhatsThisRole.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QString<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_WHATS_THIS,"Whats This Text");
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_WHATS_THIS            = "qti.role.WhatsThis";

//! Object Status Tip Role Property
/*!
If an object has this property, the property's value will be used by observer item models when data is requested
for the Qt::StatusTipRole.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QString<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_STATUSTIP,"Whats This Text");
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_STATUSTIP            = "qti.role.StatusTip";

//! Object Size Hint Role Property
/*!
If an object has this property, the property's value will be used by observer item models when data is requested
for the Qt::SizeHintRole.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QSize<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_SIZE_HINT,QSize(10,10));
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_SIZE_HINT            = "qti.role.SizeHint";

//! Object Font Role Property
/*!
If an object has this property, the property's value will be used by observer item models when data is requested
for the Qt::FontRole.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QFont<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_FONT,QFont("Arial"));
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_FONT                 = "qti.role.Font";

//! Object Text Alignment Property
/*!
If an object has this property, the property's value will be used by observer item models when data is requested
for the Qt::TextAlignmentRole.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> int, cast to Qt::AlignmentFlag<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_TEXT_ALIGNMENT,(int) Qt::AlignLeft);
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_TEXT_ALIGNMENT       = "qti.role.TextAlignment";

//! Object Background Role Property
/*!
If an object has this property, the property's value will be used by observer item models when data is requested
for the Qt::BackgroundRole.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QBrush<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_BACKGROUND,QBrush(Qt::NoBrush));
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_BACKGROUND       = "qti.role.Background";

//! Object Foreground Role Property
/*!
If an object has this property, the property's value will be used by observer item models when data is requested
for the Qt::ForegroundRole.

<b>Permisson:</b> Read/Write<br>
<b>Data Type:</b> QBrush<br>
<b>Property Type:</b> Qtilities::Core::SharedProperty<br>
<b>Is Exportable:</b> Depends on usage<br>
<b>Change Notifications:</b> Yes<br>

Below is an example of how to add this property to an object.
\code
QObject* obj = new QObject();
SharedProperty property(qti_prop_FOREGROUNDQBrush(Qt::NoBrush));
Observer::setSharedProperty(obj, property);
\endcode
*/
const char * const qti_prop_FOREGROUND       = "qti.role.Foreground";

        }
    }
}

#endif // QTILITIESCORECONSTANTS_H
