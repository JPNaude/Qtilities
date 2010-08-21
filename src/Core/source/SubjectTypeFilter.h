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

#ifndef SUBJECT_TYPE_FILTER_H
#define SUBJECT_TYPE_FILTER_H

#include "AbstractSubjectFilter.h"
#include "ObjectManager.h"
#include "Factory.h"
#include "QtilitiesCoreConstants"

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Constants;

        /*!
          \struct SubjectTypeFilterData
          \brief The SubjectTypeFilterData class stores private data used by the SubjectTypeFilter class.
         */
        struct SubjectTypeFilterData;
	
        /*!
        \class SubjectTypeFilter
        \brief A subject filter which only allows attachement of specific object types.

        The SubjectTypeFilter allows control over the types of objects which can be attached to an observer context. The filter
        only allows known subject types to be attached. A subject type is defined by the SubjectTypeInfo struct and you
        A filter only knows about subject types added to it using the addSubjectType() function. You can inverse the known
        filter types by calling the enableInverseFiltering() function. In this scenario the filter will only allow attachment
        of unknown filter type and it will filter all known types.

        Note: You need to set up your filter before attaching it to an observer.

        \sa Observer, AbstractSubjectFilter, ActivityPolicyFilter, NamingPolicyFilter
          */	
        class QTILIITES_CORE_SHARED_EXPORT SubjectTypeFilter : public AbstractSubjectFilter
        {
            Q_OBJECT

        public:
            SubjectTypeFilter(const QString& known_objects_group_name = QString(), QObject* parent = 0);
            ~SubjectTypeFilter() {}
            QString filterName() { return FACTORY_TAG_SUBJECT_TYPE_FILTER; }

            // --------------------------------
            // Factory Interface Implemenation
            // --------------------------------
            static FactoryItem<AbstractSubjectFilter, SubjectTypeFilter> factory;

            // --------------------------------
            // AbstractSubjectFilter Implemenation
            // --------------------------------
            AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj) const;
            bool initializeAttachment(QObject* obj, bool import_cycle);
            void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle);
            AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj) const;
            bool initializeDetachment(QObject* obj, bool subject_deleted = false);
            void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false);

            QStringList monitoredProperties() const;
            QStringList reservedProperties() const;
        protected:
            bool handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            IFactoryData factoryData() const;
            IExportable::Result exportBinary(QDataStream& stream, QList<QVariant> params = QList<QVariant>()) const;
            IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list, QList<QVariant> params = QList<QVariant>());
            bool exportXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>()) const;
            bool importXML(QDomDocument* doc, QDomElement* object_node, QList<QVariant> params = QList<QVariant>());

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
            SubjectTypeFilterData* d;
        };
    }
}


#endif // SUBJECT_TYPE_FILTER_H
