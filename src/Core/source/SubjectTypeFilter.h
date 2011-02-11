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

#ifndef SUBJECT_TYPE_FILTER_H
#define SUBJECT_TYPE_FILTER_H

#include "ObjectManager.h"
#include "Factory.h"
#include "QtilitiesCoreConstants.h"
#include "AbstractSubjectFilter.h"

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Constants;

        /*!
          \struct SubjectTypeFilterPrivateData
          \brief The SubjectTypeFilterPrivateData class stores private data used by the SubjectTypeFilter class.
         */
        struct SubjectTypeFilterPrivateData;
	
        /*!
        \class SubjectTypeFilter
        \brief A subject filter which only allows attachement of specific object types.

        The SubjectTypeFilter allows control over the types of objects which can be attached to an observer context. The filter
        only allows known subject types to be attached. A subject type is defined by the SubjectTypeInfo struct and each
        filter only knows about subject types added to it using the addSubjectType() function. You can inverse the known
        filter types by calling the enableInverseFiltering() function. In this scenario the filter will only allow attachment
        of unknown filter type and it will filter all known types.

        Another usefull feature of the SubjectTypeFilter class is that it provides a name for the group of subjects that is known to
        it which is accessable through groupName(). The group name is set in the constructor of the filter.

        \note You need to set up your filter before attaching any subjects to its observer context.

        \sa Observer, AbstractSubjectFilter, ActivityPolicyFilter, NamingPolicyFilter
          */	
        class QTILIITES_CORE_SHARED_EXPORT SubjectTypeFilter : public AbstractSubjectFilter
        {
            Q_OBJECT

        public:
            SubjectTypeFilter(const QString& known_objects_group_name = QString(), QObject* parent = 0);
            ~SubjectTypeFilter() {}

            // --------------------------------
            // Factory Interface Implemenation
            // --------------------------------
            static FactoryItem<QObject, SubjectTypeFilter> factory;

            // --------------------------------
            // AbstractSubjectFilter Implemenation
            // --------------------------------
            bool initializeAttachment(QObject* obj, QString* rejectMsg = 0, bool import_cycle = false);
            QString filterName() const { return qti_def_FACTORY_TAG_SUBJECT_TYPE_FILTER; }
        protected:
            bool handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            IExportable::Result exportBinary(QDataStream& stream, QList<QVariant> params = QList<QVariant>()) const;
            IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());
            IExportable::Result exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            IExportable::Result importXML(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());

        public:
            // --------------------------------
            // SubjectTypeFilter Implemenation
            // --------------------------------
            //! Gets the name describing the known subject grouping.
            QString groupName() const;
            //! Sets the filter list.
            void addSubjectType(SubjectTypeInfo subject_type_info);
            //! Returns true if the specified type if a known type in this filter. This call takes the inverse filtering setting into account. Note that the name of the SubjectTypeInfo struct is not checked but rather the d_meta_type field.
            bool isKnownType(const QString& meta_type) const;
            //! Provides a list of all the subject types known to this subject type filter.
            QList<SubjectTypeInfo> knownSubjectTypes() const;
            //! Inverse the list of known subject types. Thus all unknown types are allowed and known types are filtered, rather than the defualt filtering of all unkown types.
            void enableInverseFiltering(bool enabled);
            //! Returns true if inverse filtering is enabled. The default is false, thus the filter will filter all object types unless you add known types.
            bool inverseFilteringEnabled() const;

        private:
            SubjectTypeFilterPrivateData* d;
        };
    }
}


#endif // SUBJECT_TYPE_FILTER_H
