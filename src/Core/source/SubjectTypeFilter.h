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

#ifndef SUBJECT_TYPE_FILTER_H
#define SUBJECT_TYPE_FILTER_H

#include "ObjectManager.h"
#include "Factory.h"
#include "QtilitiesCoreConstants.h"
#include "AbstractSubjectFilter.h"
#include "VersionInformation.h"
#include "IModificationNotifier.h"

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Constants;
        using namespace Qtilities::Core::Interfaces;

        /*!
          \struct SubjectTypeFilterPrivateData
          \brief The SubjectTypeFilterPrivateData class stores private data used by the SubjectTypeFilter class.
         */
        struct SubjectTypeFilterPrivateData;
	
        /*!
        \class SubjectTypeFilter
        \brief A subject filter which only allows attachement of specific object types.

        The SubjectTypeFilter allows control over the types of objects which can be attached to an observer context. The filter only allows known subject types to be attached. A subject type is defined by the SubjectTypeInfo struct and each filter only knows about subject types added to it using the addSubjectType() function. You can inverse the known filter types by calling the enableInverseFiltering() function. In this scenario the filter will only allow attachment of unknown filter type and it will filter all known types.

        Checking if an object to be attached matches any of the known types is done through the QObject::inherits() function. Thus if you add QObject as a known type, any object inheriting QObject will be allowed while non QObject based objects will be filtered.

        Another usefull feature of the SubjectTypeFilter class is that it provides a name for the group of subjects that is known to it which is accessable through groupName(). The group name is set in the constructor of the filter.

        To install a SubjectTypeFilter on an Observer is easy:
\code
Observer* obs = new Observer;
SubjectTypeFilter* subject_type_filter = new SubjectTypeFilter;
subject_type_filter->addSubjectType(SubjectTypeInfo("TestMetaType1","TestTypeName1"));
subject_type_filter->setGroupName("TestGroupName");
subject_type_filter->enableInverseFiltering(true);
obs->installSubjectFilter(subject_type_filter);
\endcode

        When you use Qtilities::CoreGui::TreeNode instead, its even easier:
\code
TreeNode* tree_node = new TreeNode;
SubjectTypeFilter* subject_type_filter = tree_node->setChildType("TestGroupName");
subject_type_filter->addSubjectType(SubjectTypeInfo("TestMetaType1","TestTypeName1"));
subject_type_filter->enableInverseFiltering(true);
\endcode

        \note You need to set up your filter before attaching any subjects to its observer context.

        \sa Observer, AbstractSubjectFilter, ActivityPolicyFilter, NamingPolicyFilter
          */	
        class QTILIITES_CORE_SHARED_EXPORT SubjectTypeFilter : public AbstractSubjectFilter, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
            SubjectTypeFilter(const QString& known_objects_group_name = QString(), QObject* parent = 0);
            virtual ~SubjectTypeFilter();

            SubjectTypeFilter& operator=(const SubjectTypeFilter& ref);
            bool operator==(const SubjectTypeFilter& ref) const;
            bool operator!=(const SubjectTypeFilter& ref) const;

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, SubjectTypeFilter> factory;

            // --------------------------------
            // AbstractSubjectFilter Implementation
            // --------------------------------
            AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj, QString* rejectMsg = 0, bool silent = false) const;
            QString filterName() const { return qti_def_FACTORY_TAG_SUBJECT_TYPE_FILTER; }
        protected:
            bool handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

        public:
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
            IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        public:
            // --------------------------------
            // SubjectTypeFilter Implementation
            // --------------------------------
            //! Gets the name describing the known subject grouping.
            /*!
              \sa setGroupName()
              */
            QString groupName() const;
            //! Sets the name describing the known subject grouping.
            /*!
              \sa groupName()
              */
            void setGroupName(const QString& group_name);
            //! Adds a known subject type .
            /*!
              \note Known subject types can only be added when no subjects are attached to the filter's observer context.

              \sa isKnownType(), knownSubjectTypes()
              */
            void addSubjectType(SubjectTypeInfo subject_type_info);
            //! Returns true if the specified type if a known type in this filter.
            /*!
              This call takes the inverse filtering setting into account. Note that the name of the SubjectTypeInfo struct is not checked but rather the d_meta_type field.

              \sa knownSubjectTypes(), addSubjectType()
              */
            bool isKnownType(const QString& meta_type) const;
            //! Provides a list of all the subject types known to this subject type filter.
            /*!
              \sa isKnownType(), addSubjectType()
              */
            QList<SubjectTypeInfo> knownSubjectTypes() const;
            //! Inverse the list of known subject types. Thus all unknown types are allowed and known types are filtered, rather than the defualt filtering of all unkown types.
            /*!
              \note Inversed filtering can only changed when no subjects are attached to the filter's observer context.
              \note When constructing a SubjectTypeFilter using the Qtilities::CoreGui::TreeNode class's setChildType() function, inversed filtering is enabled by default.

              \sa inverseFilteringEnabled()
              */
            void enableInverseFiltering(bool enabled);
            //! Returns true if inverse filtering is enabled. The default is false, thus the filter will filter all object types unless you add known types.
            /*!
              \sa enableInverseFiltering()
              */
            bool inverseFilteringEnabled() const;

        private:
            SubjectTypeFilterPrivateData* d;
        };
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::SubjectTypeFilter& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::SubjectTypeFilter& stream_obj);

#endif // SUBJECT_TYPE_FILTER_H
