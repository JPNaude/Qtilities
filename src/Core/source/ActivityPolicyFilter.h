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

#ifndef ACTIVITYPOLICYFILTER_H
#define ACTIVITYPOLICYFILTER_H

#include "AbstractSubjectFilter.h"
#include "IModificationNotifier.h"
#include "Factory.h"
#include "QtilitiesCoreConstants.h"

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::Core::Constants;

        /*!
        \struct ActivityPolicyFilterPrivateData
        \brief A structure storing private data in the ActivityPolicyFilter class.
          */
        struct ActivityPolicyFilterPrivateData;

        /*!
            \class Qtilities::Core::ActivityPolicyFilter
            \brief The ActivityPolicyFilter class is an implementation of AbstractSubjectFilter which allows control over activity of objects within the context of an Observer.

            It is usefull when you need to control the activity of subjects within the context of an observer.

            To install a ActivityPolicyFilter on an Observer is easy:
\code
Observer* obs = new Observer;
ActivityPolicyFilter* activity_filter = new ActivityPolicyFilter;
obs->installSubjectFilter(activity_filter);
\endcode

        When you use Qtilities::CoreGui::TreeNode instead, its even easier:
\code
TreeNode* tree_node = new TreeNode;
ActivityPolicyFilter* activity_filter = tree_node->enableActivityControl(ObserverHints::CheckboxActivityDisplay);
\endcode
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
            virtual ~ActivityPolicyFilter();
            bool eventFilter(QObject *object, QEvent *event);

            ActivityPolicyFilter& operator=(const ActivityPolicyFilter& ref);
            bool operator==(const ActivityPolicyFilter& ref) const;
            bool operator!=(const ActivityPolicyFilter& ref) const;

            //! Reimplemented from AbstractSubjectFilter in order to install event filter on \p observer_context.
            bool setObserverContext(Observer* observer_context);

            //! Disables enforcement of ActivityPolicy.
            /*!
              This can be usefull when the activity of a context is known to be correct and you need to speed up attachment of a large number of items.

              \note When using this, make sure you know that the activity will be valid. Also, make sure to enable activity policy enforcement again using enablePolicyEnforcement().

              \sa enableActivityPolicyEnforcement();
              */
            void disableActivityPolicyEnforcement();
            //! Enables enforcement of ActivityPolicy.
            /*!
               \sa disableActivityPolicyEnforcement();
              */
            void enableActivityPolicyEnforcement();

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, ActivityPolicyFilter> factory;

            // --------------------------------
            // ActivityPolicyFilter Implementation
            // --------------------------------
            //! Policy to control if only one, or multiple subjects can be active at any time.
            /*!
              \sa setActivityPolicy(), activityPolicy()
              */
            enum ActivityPolicy {
                UniqueActivity,     /*!< Only one subject can be active at any time. */
                MultipleActivity    /*!< Multiple subjects can be active at any time. */
            };
            //! Function which returns a string associated with a specific ActivityPolicy.
            static QString activityPolicyToString(ActivityPolicy activity_policy);
            //! Function which returns the ActivityPolicy associated with a string.
            static ActivityPolicy stringToActivityPolicy(const QString& activity_policy_string);
            //! Policy to control if this activity filter follows the activity of it's parent observer.
            /*!
              This policy allows developers to implement the functionality where clicking on a node in a tree will
              enable/disabled (check/uncheck) all items underneath the node.

              For an example of this behaviour see the <b>Parent Tracking Activity</b> tab in the \p BuildingTreesExample.

              \note To track the activity of the observer parent, the observer parent must have only one parent itself (othwerise
              it won't be possible to know in which context the activity must be tracked) and the parent must have an activity
              policy filter installed.

              \note In order for the activity property changes to be delivered to this filter, the delivery of QtilitiesPropertyChangeEvents
              must be enabled on the parent observer. See Qtilities::Core::Observer::toggleQtilitiesPropertyChangeEvents().

              \note When minimumActivityPolicy() is set to ProhibitNoneActive, setting the activity to false on the
              observer parent will attempt to disable all objects within this context. However this will be prohibited and
              nothing will change. The opposite, that is setting the activity to true on the observer parent will still work as expected.

              \note When activityPolicy() is set to UniqueActivity, this policy does nothing when set to ParentFollowActivity since
              only one subject can be active at any time.

              \sa setParentTrackingPolicy(), parentTrackingPolicy()
              */
            enum ParentTrackingPolicy {
                ParentIgnoreActivity,     /*!< This filter does not track or care about the activity of its observer parent. */
                ParentFollowActivity      /*!< This filter tracks the activity of its observer parent. */
            };
            //! Function which returns a string associated with a specific ParentTrackingPolicy.
            static QString parentTrackingPolicyToString(ParentTrackingPolicy parent_tracking_policy);
            //! Function which returns the ParentTrackingPolicy associated with a string.
            static ParentTrackingPolicy stringToParentTrackingPolicy(const QString& parent_tracking_policy_string);
            //! Policy to control the minimum number of subjects which can be active at any time.
            /*!
              \sa setMinimumActivityPolicy(), minimumActivityPolicy()
              */
            enum MinimumActivityPolicy {
                AllowNoneActive,    /*!< All subjects can be incative at the same time. */
                ProhibitNoneActive  /*!< There should at least be one active subject at any time, unless no subjects are attached to the observer context in which the filter is installed */
            };
            //! Function which returns a string associated with a specific MinimumActivityPolicy.
            static QString minimumActivityPolicyToString(MinimumActivityPolicy minimum_activity_policy);
            //! Function which returns the MinimumActivityPolicy associated with a string.
            static MinimumActivityPolicy stringToMinimumActivityPolicy(const QString& minimum_activity_policy_string);
            //! Policy to control the activity of new subjects attached to the observer context in which the filter is installed.
            /*!
              \sa setNewSubjectActivityPolicy(), newSubjectActivityPolicy()
              */
            enum NewSubjectActivityPolicy {
                SetNewActive,       /*!< New subjects are automatically set to be active. */
                SetNewInactive      /*!< New subjects are automatically set to be inactive. */
            };
            //! Function which returns a string associated with a specific NewSubjectActivityPolicy.
            static QString newSubjectActivityPolicyToString(NewSubjectActivityPolicy new_subject_activity_policy);
            //! Function which returns the NewSubjectActivityPolicy associated with a string.
            static NewSubjectActivityPolicy stringToNewSubjectActivityPolicy(const QString& new_subject_activity_policy_string);

            //! Sets the activity policy used by this subject filter.
            /*!
            The policy can only be changed if no observer context has been set yet or if an observer context with no subjects have been set.

             \sa activityPolicy()
             */
            void setActivityPolicy(ActivityPolicyFilter::ActivityPolicy activity_policy);
            //! Gets the activity policy used by this subject filter.
            /*!
              Default is MultipleActivity.

              \sa setActivityPolicy()
              */
            ActivityPolicyFilter::ActivityPolicy activityPolicy() const;
            //! Sets the parent tracking policy used by this subject filter.
            /*!
             The policy can only be changed if no observer context has been set yet or if an observer context with no subjects have been set.

             \sa parentTrackingPolicy()
             */
            void setParentTrackingPolicy(ActivityPolicyFilter::ParentTrackingPolicy parent_tracking_policy);
            //! Gets the parent tracking policy used by this subject filter.
            /*!
              Default is ParentIgnoreActivity.

              \sa setParentTrackingPolicy()
              */
            ActivityPolicyFilter::ParentTrackingPolicy parentTrackingPolicy() const;
            //! Sets the minimum activity policy used by this subject filter.
            /*!
             The policy can only be changed if no observer context has been set yet or if an observer context with no subjects have been set.

             \sa minimumActivityPolicy()
             */
            void setMinimumActivityPolicy(ActivityPolicyFilter::MinimumActivityPolicy minimum_naming_policy);
            //! Gets the minumum activity policy used by this subject filter.
            /*!
              Default is AllowNoneActive.

              \sa setMinimumActivityPolicy()
              */
            ActivityPolicyFilter::MinimumActivityPolicy minimumActivityPolicy() const;
            //! Sets the new subject activity policy used by this subject filter.
            /*!
             The policy can only be changed at any time.

             \sa newSubjectActivityPolicy()
             */
            void setNewSubjectActivityPolicy(ActivityPolicyFilter::NewSubjectActivityPolicy new_subject_activity_policy);
            //! Gets the new subject activity policy used by this subject filter.
            /*!
              Default is SetNewInactive.

              \sa setNewSubjectActivityPolicy()
              */
            ActivityPolicyFilter::NewSubjectActivityPolicy newSubjectActivityPolicy() const;

            //! Gets the number of active subjects in the current observer context.
            int numActiveSubjects() const;
            //! Returns a list with references to all the active subjects in the current observer context.
            QList<QObject*> activeSubjects() const;
            //! Returns a list with references to all the inactive subjects in the current observer context.
            QList<QObject*> inactiveSubjects() const;
            //! Returns a list with the names of all the active subjects in the current observer context.
            QStringList activeSubjectNames() const;
            //! Returns a list with the names of all the inactive subjects in the current observer context.
            QStringList inactiveSubjectNames() const;

            // --------------------------------
            // AbstractSubjectFilter Implementation
            // --------------------------------
            bool initializeAttachment(QObject* obj, QString* rejectMsg = 0, bool import_cycle = false);
            void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle = false);
            void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false);
            QString filterName() const { return qti_def_FACTORY_TAG_ACTIVITY_FILTER; }
            QStringList monitoredProperties() const;
        protected:
            bool handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

        public:
            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            IExportable::ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        public slots:
            //! Sets the active subjects. This function will check the validity of the objects list against the activity policies in the filter.
            bool setActiveSubjects(QList<QObject*> objects, bool broadcast = true);
            //! Sets the active subjects.
            bool setActiveSubjects(QList<QPointer<QObject> > objects, bool broadcast = true);
            //! Sets a single active subject.
            bool setActiveSubject(QObject* obj, bool broadcast = true);
            //! Toggles the activity of a specific subject.
            /*!
              \param obj The subject to toggle the activity for.
              \returns True if successfull, false otherwise.
              */
            bool toggleSubjectActivity(QObject* obj);
            //! Set the activity of a specific subject.
            /*!
              \param obj The subject to toggle the activity for.
              \param is_active The new activity to be set on obj.
              \returns True if successfull, false otherwise.
              */
            bool setSubjectActivity(QObject* obj, bool is_active);
            //! Get the activity of a specific subject.
            /*!
              \param obj The subject to toggle the activity for.
              \param ok True if the return value can be trusted, that is the subject is observed in this context etc. False otherwise.
              \returns The activity of the subject.
              */
            bool getSubjectActivity(const QObject* obj, bool* ok = 0) const;
            //! Invert activity of subjects in the observer context in which this filter is installed.
            /*!
              \returns If the inversion could be done, false otherwise.

              \sa canInvertActivity();
              */
            bool invertActivity();
            //! Set all subjects to be active in the observer context in which this filter is installed.
            /*!
              \returns True if all subjects could be set active, false otherwise.

              \sa canSetAllActive();
              */
            bool setAllActive();
            //! Set all subjects to be inactive in the observer context in which this filter is installed.
            /*!
              \returns True if all subjects could be set inactive, false otherwise.

              \sa canSetNoneActive();
              */
            bool setNoneActive();

        public:
            //! Checks if invertActivity() can change the activity of subjects in this filter.
            /*!
              Inversion depends on the activity policies and has the following requirements:
              - minimumActivityPolicy() must be AllowNoneActive
              - activityPolicy() == MultipleActivity
              - parentTrackingPolicy() == ParentIgnoreActivity

              \returns If the above criteria is not met, false is returned. True otherwise.

              \sa invertActivity();
              */
            bool canInvertActivity() const;
            //! Checks if setAllActive() can change the activity of subjects in this filter.
            /*!
              Being able to set all subjects active depends on the activity policies and has the following requirements:
              - activityPolicy() == MultipleActivity

              \returns If the above criteria is not met, false is returned. True otherwise.

              \sa setAllActive();
              */
            bool canSetAllActive() const;
            //! Checks if setNoneActive() can change the activity of subjects in this filter.
            /*!
              Being able to set all subjects inactive depends on the activity policies and has the following requirements:
              - minimumActivityPolicy() must be AllowNoneActive

              \returns If the above criteria is not met, false is returned. True otherwise.

              \sa setNoneActive();
              */
            bool canSetNoneActive() const;

        private slots:
            //! Handles a processing cycle start on the observer context.
            /*!
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            void handleProcessingCycleStarted();
            //! Handles a processing cycle end on the observer context.
            /*!
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            void handleProcessingCycleEnded();
        signals:
            //! Emitted when the active objects changed.
            /*!
             *Use this signal when you have access to the observer reference, rather than waiting for a
             *QtilitiesPropertyChangeEvent on each object in the observer context.
             *
             *\note This signal is not emmited while a processing cycle is active on this filter's observer
             *context. In that scenario, the signal will be emitted when the processing cycle on the observer
             *context ends only if the active subjects changed during the processing cycle.
             */
            void activeSubjectsChanged(QList<QObject*> active_objects, QList<QObject*> inactive_objects);

        private:
            ActivityPolicyFilterPrivateData* d;
        };
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::ActivityPolicyFilter& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::Core::ActivityPolicyFilter& stream_obj);

#endif // ACTIVITYPOLICYFILTER_H
