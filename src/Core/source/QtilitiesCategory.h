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

#ifndef QTILITIES_CATEGORY_H
#define QTILITIES_CATEGORY_H

#include <QString>
#include <QVariant>
#include <QMetaType>

#include "QtilitiesCore_global.h"

namespace Qtilities {
    namespace Core {

        /*!
          \struct CategoryLevel
          \brief The CategoryLevel struct contains information for a single category level.

          This struct was added in %Qtilities v0.2.
         */
        struct QTILIITES_CORE_SHARED_EXPORT CategoryLevel {
        public:
            CategoryLevel(QDataStream& stream) {
                importBinary(stream);
            }
            CategoryLevel(const QString& name, int access_mode = 0) {
                d_name = name;
            }
            CategoryLevel(const CategoryLevel& ref) {
                d_name = ref.d_name;
            }
            void operator=(const CategoryLevel& ref) {
                d_name = ref.d_name;
            }
            virtual bool exportBinary(QDataStream& stream) const {
                stream << d_name;
                return true;
            }
            virtual bool importBinary(QDataStream& stream) {
                stream >> d_name;
                return true;
            }

            //! The name of the category level.
            QString                 d_name;
        };

/*!
\class Qtilities::Core::QtilitiesCategory
\brief An QtilitiesCategory object represents a category in %Qtilities.

The QtilitiesCategory class can be used to define a category for an object. The category can be any number of
levels deep.

We can construct a categroy object as follows:
\code
// If we want to use a category with multiple levels:
QtilitiesCategory category;
category << "Top Level Category" << "Middle Level Category" << "Lowest Level Category".

// A category with a single level:
QtiltiesCategory single_category("My Category");
\endcode

The Observer class supports the grouping of objects into categories by inspecting objects attached to it
for the OBJECT_CATEGORY property. We can add a category for an object like this:
\code
ObserverProperty category_property3(OBJECT_CATEGORY);
category_property3.setValue(qVariantFromValue(QtilitiesCategory("Category 1")),observerC->observerID());
Observer::setObserverProperty(object3,category_property3);
\endcode

In %Qtilities categories is used in a few places and using the QtilitiesCategory class everywhere we make
sure that categories are handled the same way everywhere. Example usages in %Qtilities:
- The Qtilities::CoreGui::Interfaces::IActionProvider interface allows grouping of actions into categories.
- Observer classes allows grouping of subjects into categories.
- Factory classes allows grouping of registered interfaces into categories.
- Qtilities::CoreGui::Interfaces::IConfigPage implementations allows grouping of the pages into categories in the Qtilities::CoreGui::ConfigurationWidget.

This class was added in %Qtilities v0.2.
*/
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesCategory
        {
        public:
            //! Constructs a QtilitiesCategory object.
            /*!
              \param category_level_name When this parameter contains a valid string, it will be used as the top level category level.
              \param access_mode The access mode of this category. By default we use Observer::InvalidAccess since the mode is not always going to be used.
              */
            QtilitiesCategory(const QString& category_level_name = QString(), int access_mode = 3);
            //! Constructs a QtilitiesCategory object.
            /*!
              \param category_levels A QString containing a list of categories seperated by the \p seperator parameter.
              \param seperator The seperator string used to split the \p category_levels parameter.
              */
            QtilitiesCategory(const QString& category_levels, const QString& seperator);
            //! Creates a QtilitiesCategory object from a QStringList.
            QtilitiesCategory(const QStringList& category_name_list);
            QtilitiesCategory(QDataStream &ds) {
                importBinary(ds);
            }
            QtilitiesCategory(const QtilitiesCategory& category) {
                d_category_levels = category.d_category_levels;
                d_access_mode = category.d_access_mode;
            }
            virtual ~QtilitiesCategory() {}

            //! Indicates if this category is valid.
            /*!
              The validation check checks if the category has any levels, if not it is invalid.
              */
            inline bool isValid() { return d_category_levels.count() > 0; }

            //! Operator overload to compare two QtilitiesCategory objects with each other.
            /*!
              \note Only the category names are used in this comparison.
              */
            bool operator==(const QtilitiesCategory& ref) const {
                if (ref.d_category_levels.count() != d_category_levels.count())
                    return false;

                for (int i = 0; i < d_category_levels.count(); i++) {
                    if (ref.d_category_levels.at(i).d_name != d_category_levels.at(i).d_name)
                        return false;
                }

                return true;
            }

            //! Operator overload to compare two QtilitiesCategory objects are not equal.
            /*!
              \note Only the category names are used in this comparison.
              */
            bool operator!=(const QtilitiesCategory& ref) const {
                if (ref.d_category_levels.count() != d_category_levels.count())
                    return true;

                for (int i = 0; i < d_category_levels.count(); i++) {
                    if (ref.d_category_levels.at(i).d_name != d_category_levels.at(i).d_name)
                        return true;
                }

                return false;
            }

            //! Operator overload to assign one QtilitiesCategory to another QtilitiesCategory.
            void operator=(const QtilitiesCategory& ref) {
                d_category_levels = ref.d_category_levels;
                d_access_mode = ref.d_access_mode;
            }

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

            //! Returns the depth of this category.
            inline int categoryDepth() const { return d_category_levels.count(); }

            //! Returns the category as a QString.
            /*!
              \param join_string The string that must be used to join the category level names.

              \returns The joined category string. For example when join_string is :: we get TopLevelCategory::MiddleLevelCategory::LowerLevelCategory
              */
            QString toString(const QString& join_string = QString("::")) const;
            //! Returns the category as a QStringList.
            /*!
              \param level The depth of the requested string list. If the category has for example 3 levels, you can only get the first 2 levels by passing 2 as the \p level parameter. If \p level is bigger than the depth (number of levels) of the category, the depth of the category will be used.
              */
            QStringList toStringList(int level = -1) const;

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

            //! Gets the access mode of this category.
            inline int accessMode() const { return d_access_mode; }
            //! Sets the access mode of this category.
            inline void setAccessMode(int access_mode) { d_access_mode = access_mode; }

            //! Exports the category properties to a QDataStream.
            bool exportBinary(QDataStream& stream) const;
            //! Imports the category properties from a QDataStream.
            bool importBinary(QDataStream& stream);

            //! Adds a level to the category by providing the information needed to construct a CategoryLevel.
            void addLevel(const QString& name);
            //! Adds a level to the category by providing a ready to use CategoryLevel structure.
            void addLevel(CategoryLevel category_level);

            //! Returns the category levels where the first item in the list is always the top level category and the last item is the bottom level.
            inline QList<CategoryLevel> categoryLevels() const { return d_category_levels; }

            //! Indicates if this category is empty. Thus no levels have been added to it.
            inline bool isEmpty() const { return (d_category_levels.count() == 0); }

        protected:
            QList<CategoryLevel>    d_category_levels;
            int                     d_access_mode;
        };
    }
}

Q_DECLARE_METATYPE(Qtilities::Core::QtilitiesCategory);

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::QtilitiesCategory& category);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::QtilitiesCategory& category);

#endif // QTILITIES_CATEGORY_H
