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

#ifndef ACTIVITYPOLICYFILTER_H
#define ACTIVITYPOLICYFILTER_H

#include "AbstractSubjectFilter.h"
#include "IModificationNotifier"
#include "Factory.h"

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;

        /*!
        \struct ActivityPolicyFilterData
        \brief A structure storing private data in the ActivityPolicyFilter class.
          */
        struct ActivityPolicyFilterData;

        /*!
            \class Qtilities::Core::ActivityPolicyFilter
            \brief The ActivityPolicyFilter class is an implementation of AbstractSubjectFilter which allows control over activity of objects within the context of an Observer.

            It is usefull when you need to control the activity of subjects within the context of an observer.
        */
        class QTILIITES_CORE_SHARED_EXPORT ActivityPolicyFilter : public AbstractSubjectFilter, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)
            Q_ENUMS(ActivityPolicy)
            Q_ENUMS(MinimumActivityPolicy)
            Q_ENUMS(NewSubjectActivityPolicy)

        public:
            ActivityPolicyFilter(QObject* parent = 0);
            ~ActivityPolicyFilter() {}
            QString filterName() { return tr("Activity Policy Filter"); }

            // --------------------------------
            // Factory Interface Implemenation
            // --------------------------------
            static FactoryItem<AbstractSubjectFilter, ActivityPolicyFilter> factory;

            //! Policy to control if only one, or multiple subjects can be active at any time.
            /*!
              \sa setActivityPolicy(), activityPolicy()
              */
            enum ActivityPolicy {
                UniqueActivity,     /*!< Only one subject can be active at any time. */
                MultipleActivity    /*!< Multiple subjects can be active at any time. */
            };
            //! Policy to control the minimum number of subjects which can be active at any time.
            /*!
              \sa setMinimumActivityPolicy(), minimumActivityPolicy()
              */
            enum MinimumActivityPolicy {
                AllowNoneActive,    /*!< All subjects can be incative at the same time. */
                ProhibitNoneActive  /*!< There should at least be one active subject at any time, unless no subjects are attached to the observer context in which the filter is installed */
            };
            //! Policy to control the activity of new subjects attached to the observer context in which the filter is installed.
            /*!
              \sa setNewSubjectActivityPolicy(), newSubjectActivityPolicy()
              */
            enum NewSubjectActivityPolicy {
                SetNewActive,       /*!< New subjects are automatically set to be active. */
                SetNewInactive      /*!< New subjects are automatically set to be inactive. */
            };

            //! Sets the activity policy used by this subject filter.
            /*!
             The policy can only be changed if no observer context has been set yet.
             */
            void setActivityPolicy(ActivityPolicyFilter::ActivityPolicy naming_policy);
            //! Gets the activity policy used by this subject filter.
            ActivityPolicyFilter::ActivityPolicy activityPolicy() const;
            //! Sets the minimum activity policy used by this subject filter.
            /*!
             The policy can only be changed if no observer context has been set yet.
             */
            void setMinimumActivityPolicy(ActivityPolicyFilter::MinimumActivityPolicy minimum_naming_policy);
            //! Gets the minumum activity policy used by this subject filter.
            ActivityPolicyFilter::MinimumActivityPolicy minimumActivityPolicy() const;
            //! Sets the new subject activity policy used by this subject filter.
            /*!
             The policy can only be changed if no observer context has been set yet.
             */
            void setNewSubjectActivityPolicy(ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy);
            //! Gets the new subject activity policy used by this subject filter.
            ActivityPolicyFilter::NewSubjectActivityPolicy newSubjectActivityPolicy() const;

            //! Gets the number of active subjects in the current observer context.
            int numActiveSubjects() const;
            //! Returns a list with references to all the active subjects in the current observer context.
            QList<QObject*> activeSubjects() const;
            //! Returns a list with references to all the inactive subjects in the current observer context.
            QList<QObject*> inactiveSubjects() const;

            AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj) const;
            bool initializeAttachment(QObject* obj, bool import_cycle);
            void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle);
            AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj) const;
            bool initializeDetachment(QObject* obj, bool subject_deleted = false);
            void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false);

            QStringList monitoredProperties();
            bool monitoredPropertyChanged(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

            bool exportFilterSpecificBinary(QDataStream& stream) const;
            bool importFilterSpecificBinary(QDataStream& stream);

            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }

            // --------------------------------
            // IModificationNotifier Implemenation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners);
        signals:
            void modificationStateChanged(bool is_modified) const;
            void partialStateChanged(const QString& part_name) const;

        public slots:
            //! Sets the active subjects. This function will check the validity of the objects list against the activity policies in the filter.
            /*!
              Note that this function only works in Table View mode at present. Tree views will support this function in a future version.
              */
            void setActiveSubjects(QList<QObject*> objects);

        signals:
            //! Emitted when the active objects changed. Use this signal when you have access to the observer reference, rather than waiting for a QtilitiesPropertyChangeEvent on each object in the observer context.
            void activeSubjectsChanged(QList<QObject*> active_objects, QList<QObject*> inactive_objects);

        private:
            ActivityPolicyFilterData* d;
        };
    }
}

#endif // ACTIVITYPOLICYFILTER_H
