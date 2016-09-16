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

#ifndef QTILITIES_CATEGORY_H
#define QTILITIES_CATEGORY_H

#include <QString>
#include <QVariant>
#include <QMetaType>

#include "QtilitiesCore_global.h"
#include "IExportable.h"

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;
        /*!
          \class CategoryLevel
          \brief The CategoryLevel class contains information for a single category level.

          \sa QtilitiesCategory

          <i>This class was added in %Qtilities v0.2.</i>
         */
        class QTILIITES_CORE_SHARED_EXPORT CategoryLevel : public IExportable {
        public:
            CategoryLevel(QDataStream& stream, Qtilities::ExportVersion version) : IObjectBase(), IExportable() {
                QList<QPointer<QObject> > import_list;
                setExportVersion(version);
                importBinary(stream,import_list);
            }
            CategoryLevel(const QString& name = QString()) : IObjectBase(), IExportable() {
                d_name = name;
            }
            CategoryLevel(const CategoryLevel& ref) : IObjectBase(), IExportable() {
                d_name = ref.d_name;
            }
            CategoryLevel& operator=(const CategoryLevel& ref) {
                if (this==&ref) return *this;
                d_name = ref.d_name;
                return *this;
            }
            bool operator==(const CategoryLevel& ref) const {
                return d_name == ref.d_name;
            }
            bool operator!=(const CategoryLevel& ref) const {
                return !(*this==ref);
            }

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            /*!
              \note CategoryLevel is now a QObject, thus it returns 0.
              */
            QObject* objectBase() { return 0; }
            /*!
              \note CategoryLevel is now a QObject, thus it returns 0.
              */
            const QObject* objectBase() const { return 0; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            /*!
              This function adds a category level node under \p object_node with all the information about this category level.
              */
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            //! The name of the category level.
            QString                 d_name;
        };

/*!
\class Qtilities::Core::QtilitiesCategory
\brief A QtilitiesCategory object represents a category in %Qtilities.

The QtilitiesCategory class can be used to define a category for an object. The category can be any number of
levels deep where each level is a CategoryLevel instance.

We can construct a category object as follows:
\code
// A category with a single level:
QtilitiesCategory single_category("My Category");

// If we want to use a category with multiple levels:
// One way to do it:
QtilitiesCategory category;
category << "Top Level Category" << "Middle Level Category" << "Lowest Level Category";

// Another way to do it:
QtilitiesCategory category("Top Level Category::Middle Level Category::Lowest Level Category","::");

// Yet another way to do it:
QStringList category_list;
category_list << "Top Level Category" << "Middle Level Category" << "Lowest Level Category";
QtilitiesCategory category(category_list);
\endcode

We can check if a category is valid using isValid() where a valid category is a category with at least one level.
The category depth is available through categoryDepth() and the top and bottom level category names are
available through categoryTop() and categoryBottom() respectively. The category can be converted to a string using
toString() or to a string list using toStringList().

It is possible to set the access mode of a category using setAccessMode() and the access mode can be accessed
using accessMode(). A QtilitiesCategory object supports streaming to a QDataStream or saving its data
to a XML QDomElement node. This is made possible by the implementation of Qtilities::Core::Interfaces::IExportable.

\section qtilities_category_usage_scenario Usage Scenario

Lets look at one example usage of QtilitiesCategory: The Observer class supports the grouping of objects into categories
by inspecting objects attached to it for the qti_prop_CATEGORY_MAP property. We can add an object in a specific category to
an Observer context in a couple of ways:
\code
// Using Observer directly:
Observer* observer = new Observer("My Node");
observer->useDisplayHints();
observer->displayHints()->setHierarchicalDisplayHint(ObserverHints::CategorizedDisplay);
QObject* object = new QObject();
object->setObjectName("Categorized Item");
MultiContextProperty category_property(qti_prop_CATEGORY_MAP);
category_property.setValue(qVariantFromValue(QtilitiesCategory("Item Category")),observer->observerID());
ObjectManager::setMultiContextProperty(object,category_property);
observer->attachSubject(object);

// Using TreeNode (which inherits from Observer):
TreeNode* node = new TreeNode("My Node");
node->enableCategorizedDisplay();
node->addItem("Categorized Item",QtilitiesCategory("Item Category"));
\endcode

\section qtilities_category_usages_in_qtilities Usage in Qtilities
In %Qtilities categories are used in a few places and by using the QtilitiesCategory class everywhere we make
sure that categories are handled the same way everywhere. Some usages in %Qtilities:
- The Qtilities::CoreGui::Interfaces::IActionProvider interface allows grouping of actions into categories.
- Observer classes allows grouping of subjects into categories.
- Factory classes allows grouping of registered interfaces into categories.
- Qtilities::CoreGui::Interfaces::IConfigPage implementations allows grouping of the pages into categories in the Qtilities::CoreGui::ConfigurationWidget.

<i>This class was added in %Qtilities v0.2.</i>
*/
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesCategory : public IExportable
        {
        public:
            //! Constructs a QtilitiesCategory object.
            /*!
              \param category_level_name When this parameter contains a valid string, it will be used as the top level category level.
              \param access_mode The access mode of this category. By default we use Observer::InvalidAccess since the mode is not always going to be used.
              */
            QtilitiesCategory(const QString& category_level_name = QString());
            //! Constructs a QtilitiesCategory object.
            /*!
              \param category_levels A QString containing a list of categories seperated by the \p separator parameter.
              \param separator The separator string used to split the \p category_levels parameter.
              */
            QtilitiesCategory(const QString& category_levels, const QString& separator);
            //! Creates a QtilitiesCategory object from a QStringList.
            QtilitiesCategory(const QStringList& category_name_list);
            QtilitiesCategory(QDataStream &ds, Qtilities::ExportVersion version) : IObjectBase(), IExportable() {
                QList<QPointer<QObject> > import_list;
                setExportVersion(version);
                importBinary(ds,import_list);
            }
            QtilitiesCategory(const QtilitiesCategory& category) : IObjectBase(), IExportable() {
                d_category_levels = category.d_category_levels;
                d_access_mode = category.d_access_mode;
            }
            virtual ~QtilitiesCategory() {}
            QtilitiesCategory& operator=(const QtilitiesCategory& other);
            //! Operator overload to compare two QtilitiesCategory objects with each other.
            /*!
              \note Only the category names are used in this comparison.
              */
            bool operator==(const QtilitiesCategory& ref) const;
            //! Operator overload to compare two QtilitiesCategory objects are not equal.
            /*!
              \note Only the category names are used in this comparison.
              */
            bool operator!=(const QtilitiesCategory& ref) const;
            //! Overload << operator so that we can build up a category structure in the same way as QStringList.
            inline QtilitiesCategory& operator<<(const QString &category_level_name)
            {
               addLevel(category_level_name);
               return *this;
            }
            //! Overload < operator so that we can use QtilitiesCategory in a QMap.
            inline bool operator<(const QtilitiesCategory &e1) const
            {
                return toString() < e1.toString();
            }
            //! Overload > operator.
            inline bool operator>(const QtilitiesCategory &e1) const
            {
                return toString() > e1.toString();
            }
            //! Overload <= operator.
            inline bool operator<=(const QtilitiesCategory &e1) const
            {
                return toString() <= e1.toString();
            }
            //! Overload >= operator.
            inline bool operator>=(const QtilitiesCategory &e1) const
            {
                return toString() >= e1.toString();
            }

            //! Indicates if this category is valid.
            /*!
              The validation check checks if the category has any levels, if not it is invalid.
              */
            inline bool isValid() const { return d_category_levels.count() > 0; }
            //! Indicates if this category is empty. Thus no levels have been added to it.
            inline bool isEmpty() const { return (d_category_levels.count() == 0); }
            //! Clears the category.
            inline void clear() { d_category_levels.clear(); }

            //! Returns the category as a QString.
            /*!
              \param join_string The string that must be used to join the category level names.

              \returns The joined category string. For example when join_string is :: we get TopLevelCategory::MiddleLevelCategory::LowerLevelCategory
              */
            QString toString(const QString& join_string = "::") const;
            //! Returns the category as a QStringList.
            /*!
              \param level The depth of the requested string list. If the category has for example 3 levels, you can only get the first 2 levels by passing 2 as the \p level parameter. If \p level is bigger than the depth (number of levels) of the category, the depth of the category will be used.
              */
            QStringList toStringList(int level = -1) const;

            //! Gets the access mode of this category.
            inline int accessMode() const { return d_access_mode; }
            //! Sets the access mode of this category.
            inline void setAccessMode(int access_mode) { d_access_mode = access_mode; }

            //! Returns the depth of this category.
            inline int categoryDepth() const { return d_category_levels.count(); }
            //! Adds a level to the category by providing the information needed to construct a CategoryLevel.
            void addLevel(const QString& name);
            //! Adds a level to the category by providing a ready to use CategoryLevel structure.
            void addLevel(CategoryLevel category_level);
            //! Returns the category levels where the first item in the list is always the top level category and the last item is the bottom level.
            inline QList<CategoryLevel> categoryLevels() const { return d_category_levels; }
            //! Returns the top level category in this category object.
            /*!
              This function is a convenience function. It returns the same as categoryLevels().front().d_name;
              */
            inline QString categoryTop() const {
                if (d_category_levels.count() > 0)
                    return d_category_levels.front().d_name;
                else
                    return QString();
            }
            //! Returns the bottom level category in this category object.
            /*!
              This function is a convenience function. It returns the same as categoryLevels().back().d_name;
              */
            inline QString categoryBottom() const {
                if (d_category_levels.count() > 0)
                    return d_category_levels.back().d_name;
                else
                    return QString();
            }

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            /*!
              \note QtilitiesCategory is now a QObject, thus it returns 0.
              */
            QObject* objectBase() { return 0; }
            /*!
              \note QtilitiesCategory is now a QObject, thus it returns 0.
              */
            const QObject* objectBase() const { return 0; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            void setExportVersion(Qtilities::ExportVersion version);
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            /*!
              This function adds a category node under \p object_node with all the information about this category.

              \note The category icon is not exported along with your category.
              */
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

        protected:
            QList<CategoryLevel>    d_category_levels;
            int                     d_access_mode;
            //QIcon*                  d_category_icon;
        };
    }
}

Q_DECLARE_METATYPE(Qtilities::Core::QtilitiesCategory);

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::CategoryLevel& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::CategoryLevel& stream_obj);
QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::QtilitiesCategory& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::QtilitiesCategory& stream_obj);

#endif // QTILITIES_CATEGORY_H
