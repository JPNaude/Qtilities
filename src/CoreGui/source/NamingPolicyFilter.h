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

#ifndef NAMINGPOLICYFILTER_H
#define NAMINGPOLICYFILTER_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"

#include <AbstractSubjectFilter>
#include <IModificationNotifier>
#include <Factory>
#include <VersionInformation>

#include <QItemDelegate>
#include <QValidator>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::CoreGui::Constants;
        class NamingPolicyInputDialog;
        struct NamingPolicyFilterData;

        namespace Interfaces {
            class INamingPolicyDialog;
        }
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \class Qtilities::CoreGui::NamingPolicyFilter
        \brief The NamingPolicyFilter class is an implementation of AbstractSubjectFilter which allows control over naming of objects within the context of an Observer.

        The NamingPolicyFilter class is usefull when you need to control the names of subjects within the context of an Observer. Currently NamingPolicyFilter allows control over the validity and uniqueness of a names within the context of an Observer set using setObserverContext().

        Names are checked against the following criteria:
        - Validity of the name, defined through a QValidator set through setValidator().
        - Uniqueness of names, controlled through setUniquenessPolicy().

        To install a NamingPolicyFilter on an Observer is easy:
\code
Observer* obs = new Observer;
NamingPolicyFilter* naming_filter = new NamingPolicyFilter;
naming_filter->setUniquenessPolicy(NamingPolicyFilter::ProhibitDuplicateNames);
naming_filter->setValidityResolutionPolicy(NamingPolicyFilter::PromptUser);
naming_filter->setUniquenessResolutionPolicy(NamingPolicyFilter::Reject);
obs->installSubjectFilter(naming_filter);
\endcode

        When you use Qtilities::CoreGui::TreeNode instead, its even easier:
\code
TreeNode* tree_node = new TreeNode;
NamingPolicyFilter* naming_filter = tree_node->enableNamingControl(ObserverHints::ReadOnlyNames,NamingPolicyFilter::ProhibitDuplicateNames);
\endcode

        \section naming_policy_filter_validity Validity Of Names

        The validity of a name is determined by a QValdiator which is set using setValidator(). To get the current validator use getValidator(). Valid names are those which return QValidator::Acceptable when validated using the validator. By default the NamingPolicySubject filter validates names using a QRegExpValidator constructed as follows:

\code
const QRegExp default_expression("\\.{1,200}",Qt::CaseInsensitive);
QRegExpValidator* default_validator = new QRegExpValidator(default_expression,0);
\endcode

        \section naming_policy_filter_uniqueness Uniqueness Of Names

        The uniqueness of names are determined by comparing the names of subjects using QString's compare (overloaded ==) functionality, thus comparisons are case sensitive. The uniqueness of names are controlled by the UniquenessPolicy of the subject filter. This can be accessed using setUniquenessPolicy() and uniquenessPolicy().

        Once the NamingPolicyFilter is set up the way you need it, it will add a single shared property (see Qtilities::Core::SharedProperty) to subjects attached to it's observer context. The name of this property is defined in code using the Qtilities::Core::Properties::qti_prop_NAME constant.

        This dynamic property will be sync'ed with objectName() at all times. Since there is no way to know when setObjectName() is called on a QObject, a dynamic property needed to be added to manage the subject's name. Whenever you update this shared property it will be evaluated by the subject filter which will make sure the new name follows the validity of the filter's validator as well as the UniquenessPolicy of the filter. When a subject is attached to multiple contexts in which names are managed by NamingPolicyFilter objects, the first context to which it was attached will become its name manager. The name manager context will use the \p qti_prop_NAME property and all contexts (with naming policy filters) to which the subject is attached after being attached to the name manager will use Qtilities::Core::Properties::qti_prop_ALIAS_MAP to store aliases for the object in those contexts. The name of a subject in a context can be found using the Qtilities::Core::Observer::subjectNameInContext() function.

        The diagram below shows a QObject which is observed by a number of observers with different, or no naming policy filters installed.

        \image html naming_policy_filter_overview.jpg "Naming Policy Filter Overview"

        Subject names are evaulated during attachment to the filter's observer context and when changing the name properties discussed above. Evaluation is done using the evaluateName() function which returns the validity of the name using a value defined by ValidityCheckResult. You can control how conflicting names are handled by setting the uniqueness resolution policy and the validity resolution policy of the subject filter. Access functions for these policies are setUniquenessResolutionPolicy(), uniquenessResolutionPolicy() setValidityResolutionPolicy() and validityResolutionPolicy(). Possible values for these policies are defined using ResolutionPolicy.

        When isExportable() is true the parameters of the filter will be exported with its Observer context when the observer is exported. Note that when streaming naming policy filter objects, custom validators will not be streamed at present. Thus the default validator will be used when constructing the naming policy filter again in the future.

        \section naming_policy_filter_displayed_names Displayed Names

        By default Observer models display the names of subjects provided by \p qti_prop_NAME or \p qti_prop_ALIAS_MAP depending on who the name manager is. This name is also used during validation and uniqueness checking. In some cases it is required to display a different name to the user and for those cases the Qtilities::Core::Properties::qti_prop_DISPLAYED_ALIAS_MAP property can be used. An example where this is usefull is Qtilities::CoreGui::TreeFileItem which allows you to show different parts of a file path to the user. However in the background the qti_prop_NAME still reffers to the full path, even though only the name is displayed for example, and the naming policy filters will still use the qti_prop_NAME to check for duplicates etc.

        \section naming_policy_filter_changing_names Changing Subject Names

        It is important to understand how to change the name of an object when using NamingPolicyFilter. An example of how to change the name on an object is shown below:

\code
// We need to check if qti_prop_NAME exists first:
if (ObjectManager::propertyExists(obj,qti_prop_NAME)) {
    SharedProperty new_subject_name_property(qti_prop_NAME,QVariant("New Name"));
    ObjectManager::setSharedProperty(obj,new_subject_name_property);
} else {
    // Handle cases where there is no naming policy filter:
    setObjectName("New Name");

\endcode

        In most cases its not neccessarry to know if the name was actually set or not (thus not rejected by the NamingPolicyFilter). In other cases you might want to know if the property was set successfully and this can be done by connecting a slot to the subjectNameChanged() signal, or through an event filter on the object for which the name must be changed. If the property change in rejected by the NamingPolicyFilter the QDynamicPropertyChangeEvent event will be filtered by the observer context. Thus you can see if the event was filtered or not.

        Another way is to listen for Qtilities::Core::QtilitiesPropertyChangeEvent events on the object (this is not enabled by default on observers). Whenever a property was successfully changed the observer context will post a QtilitiesPropertyChangeEvent event to the object. Thus if you get the event the name was changed successfully. For more information on how this see \ref monitoring_property_changes.

        \section naming_policy_filter_subclassing Subclassing NamingPolicyFilter

        It is possible to customize naming policy filters by subclassing NamingPolicyFilter. The needed functions are virtual and allows you to customize the behaviour. Thus you can for example provide a custom naming policy filter dialog which is presented to the user if you decide to use that resolution policy.

        The following virtual functions can be subclassed to get the described behaviour:
        - evaluateName() : Evaluates a name in the context. By default evaluation is done as described in \ref naming_policy_filter_validity and \ref naming_policy_filter_uniqueness. Reimplement this function in order to customize the way the evaluation is done.
        - getEvaluationName() : Returns the name to be evaluated for a given object.
        - generateValidName() : Generates valid names when AutoRename is used as the ResolutionPolicy and when the standard NamingPolicyInputDialog is used the auto-rename button is clicked. Reimplement this function in order to customize the generation of valid names.
        - validateNamePropertyChange() : Defines the behaviour when name properties change on objects.
        - constructUserDialog() : Constructs the dialog presented ot the user when using PromptUser resolution policies. To present your own widget, overload this function and construct your own widget.

        \sa NamingPolicyInputDialog
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT NamingPolicyFilter : public AbstractSubjectFilter, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)
            Q_ENUMS(UniquenessPolicy)
            Q_ENUMS(ResolutionPolicy)
            Q_ENUMS(ValidityCheckResult)

            friend class NamingPolicyInputDialog;

        public:
            NamingPolicyFilter(QObject* parent = 0);
            virtual ~NamingPolicyFilter();

            NamingPolicyFilter& operator=(const NamingPolicyFilter& ref);
            bool operator==(const NamingPolicyFilter& ref) const;
            bool operator!=(const NamingPolicyFilter& ref) const;

            //! Provides debug information about this filter.
            QString debugInfo() const;

            // --------------------------------
            // Enumerations
            // --------------------------------
            //! Policy to control uniqueness of subject names in the observer context in which this filter is installed.
            /*!
              \sa setUniquenessPolicy(), uniquenessNamingPolicy()
              */
            enum UniquenessPolicy {
                AllowDuplicateNames,                    /*!< Allow duplicate names. */
                ProhibitDuplicateNames,                 /*!< Prohibit duplicate names (checking for duplicate names done in a case in-sensitive way). */
                ProhibitDuplicateNamesCaseSensitive     /*!< Prohibit duplicate names (checking for duplicate names done in a case sensitive way). */
            };
            //! Function which returns a string associated with a specific UniquenessPolicy.
            static QString uniquenessPolicyToString(UniquenessPolicy uniqueness_policy);
            //! Function which returns the UniquenessPolicy associated with a string.
            static UniquenessPolicy stringToUniquenessPolicy(const QString& uniqueness_policy_string);
            //! Policy to control how invalid or duplicate names should be handled.
            /*!
              setUniquenessPolicy(), uniquenessNamingPolicy(), setUniquenessResolutionPolicy(), uniquenessResolutionPolicy(), setValidityResolutionPolicy(), validityResolutionPolicy()
              */
            enum ResolutionPolicy {
                AutoRename = 0,             /*!< Automatically rename new names. \sa generateValidName() */
                PromptUser = 1,             /*!< Bring up a Qtilities::CoreGui::NamingPolicyInputDialog widget from which the user can decide what to do. \note <b>Important:</b> When this NamingPolicyFilter is installed on an Observer which does not live in the main GUI thread, this resolution policy should not be used. If it is used it will attempt to construct a widget in a non-GUI thread and it will crash.*/
                Replace = 2,                /*!< Replace the conflicting object with the current object. This option will only work when the conflicting object is only observed in the context to which the naming policy filter is attached. If this is the case, the replacement operation will delete the conflicting object and attach the new object to the observer. \note The Replace policy is only usable when duplicate names are encountered, not invalid names. For invalid names Reject will be used. */
                Reject = 3                  /*!< Reject unacceptable names.*/
            };
            //! Function which returns a string associated with a specific ResolutionPolicy.
            static QString resolutionPolicyToString(ResolutionPolicy resolution_policy);
            //! Function which returns the ResolutionPolicy associated with a string.
            static ResolutionPolicy stringToResolutionPolicy(const QString& resolution_policy_string);
            //! Policy to control which checks must be performed by the filter.
            /*!
              setProcessingCycleValidationChecks(), processingCycleValidationChecks(), setValidationChecks(), validationChecks()
              */
            enum ValidationCheck {
                NoChecks = 0,             /*!< Automatically rename new names. \sa generateValidName() */
                Validity = 1,             /*!< Checks the validity of subject names. \sa uniquenessNamingPolicy() */
                Uniqueness = 2,           /*!< Checks the uniqueness of subject names. \sa validityResolutionPolicy() */
                AllChecks = Validity | Uniqueness /*!< All checks are performed. */
            };
            Q_DECLARE_FLAGS(ValidationCheckFlags, ValidationCheck)
            Q_FLAGS(ValidationCheckFlags)
            //! Function which returns a string associated with a specific ValidationCheckFlags.
            static QString validationCheckFlagsToString(ValidationCheckFlags validation_checks);
            //! Function which returns the ValidationCheckFlags associated with a string.
            static ValidationCheckFlags stringToValidationCheckFlags(const QString& validation_checks);
            //! Enumeration defining the possible outcames of name evaluation operations.
            /*!
              \sa evaluateName()
              */
            enum ValidityCheckResult {
                Acceptable = 0,         /*!< The name is acceptable. */
                Duplicate = 1,          /*!< A duplicate name exists. */
                Invalid = 2             /*!< The name is invalid in this context. \sa setValidator(), getValidator(). */
            };
            Q_DECLARE_FLAGS(NameValidity, ValidityCheckResult)
            Q_FLAGS(NameValidity)

            // --------------------------------
            // Factory Interface Implementation
            // --------------------------------
            static FactoryItem<QObject, NamingPolicyFilter> factory;

            // --------------------------------
            // AbstractSubjectFilter Implementation
            // --------------------------------
            AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj, QString* rejectMsg = 0, bool silent = false) const;
            bool initializeAttachment(QObject* obj, QString* rejectMsg = 0, bool import_cycle = false);
            void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle = false);
            void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false);
            QString filterName() const { return QString(qti_def_FACTORY_TAG_NAMING_FILTER); }
            QStringList monitoredProperties() const;
            QStringList reservedProperties() const;
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

            //! Signal which is emitted when the name of an object who's name is managed by this policy filter is changed.
            /*!
             * \param obj The object who's name changed.
             * \param old_name The previous name, before it was changed.
             * \param new_name The new name that was changed.
             *
             *<i>This function was added in %Qtilities v1.2.</i>
             */
            void subjectNameChanged(QObject* obj, const QString& old_name, const QString& new_name);

        public:
            // --------------------------------
            // NamingPolicyFilter Implementation
            // --------------------------------
            //! Sets the naming checks that must be done when this filter's observer context is not busy with a processing cycle.
            void setValidationChecks(NamingPolicyFilter::ValidationCheckFlags validation_checks);
            //! Gets the naming checks that must be done when this filter's observer context is not busy with a processing cycle.
            /*!
              The default is AllChecks.
              */
            NamingPolicyFilter::ValidationCheckFlags validationChecks() const;
            //! Sets the naming checks that must be done when this filter's observer context is busy with a processing cycle.
            void setProcessingCycleValidationChecks(NamingPolicyFilter::ValidationCheckFlags validation_checks);
            //! Gets the naming checks that must be done when this filter's observer context is busy with a processing cycle.
            /*!
              The default is AllChecks.
              */
            NamingPolicyFilter::ValidationCheckFlags processingCycleValidationChecks() const;

            //! Sets the naming uniqueness policy of this subject filter.
            /*!
              \note The uniqueness policy can only be changed when no subjects are attached to the filter's observer context.
              */
            void setUniquenessPolicy(NamingPolicyFilter::UniquenessPolicy naming_uniqueness_policy);
            //! Gets the naming uniqueness policy used by this subject filter.
            /*!
              The default is ProhibitDuplicateNames.
              */
            NamingPolicyFilter::UniquenessPolicy uniquenessNamingPolicy() const;

            //! Sets the naming uniqueness conflict policy used by this subject filter.
            void setUniquenessResolutionPolicy(NamingPolicyFilter::ResolutionPolicy naming_uniqueness_resolution_policy);
            //! Gets the naming uniqueness conflict policy used by this subject filter.
            /*!
              The default is PromptUser.
              */
            NamingPolicyFilter::ResolutionPolicy uniquenessResolutionPolicy() const;

            //! Sets the naming validity conflict policy used by this subject filter.
            void setValidityResolutionPolicy(NamingPolicyFilter::ResolutionPolicy naming_validity_resolution_policy);
            //! Gets the naming validity conflict policy used by this subject filter.
            /*!
              The default is PromptUser.
              */
            NamingPolicyFilter::ResolutionPolicy validityResolutionPolicy() const;

            //! Evaluates a name in the observer context in which this subject filter is installed.
            /*!
              \param name The name to validate in observerContext().
              \param object The current object. When null (by default) the context is checked for \p name and if it exists, Duplicate is returned. If object is not null, the context is checked for instances of the name except for the current object.
              */
            virtual NamingPolicyFilter::NameValidity evaluateName(const QString& name, QObject* object = 0) const;
            //! Gets the name to be used during evaluateName() for a specific object.
            /*!
              By default an empty QString is returned and NamingPolicyFilter gets the correct name depending where this function is called.

              \param object The object for which the evaluation name must be obtained.
              */
            virtual QString getEvaluationName(QObject* object) const {
                Q_UNUSED(object)
                return QString();
            }
            //! Gets the object which conflicts with the specified name. If no object conflicts, returns 0.
            QObject* getConflictingObject(const QString& name) const;
            //! Function to set the validator used to validate names.
            /*!
              \note The validator can only be set when the context to which this filter is attached to has no objects attached to it.
              \note NamingPolicyFilter takes ownership of the new validator after this call.
              */
            void setValidator(QValidator* valid_naming_validator);
            //! Gets the validator used to validate names.
            QValidator* getValidator();

            //! Function which makes this naming policy filter the object name manager of the given object.
            void makeNameManager(QObject* obj);

            //! Constructs the input dialog presented to the user when using PromptUser policies.
            virtual INamingPolicyDialog* constructUserDialog() const;

            //! Function which starts a new naming validation cycle.
            /*!
              A naming validation cycle reffers to a bunch of validations that must happen. For example if you
              are parsing an input file which results in the attachment of many subjects to the observer context
              which this filter is validating, you create a validation cycle for this process.

              Validation cycles allow the use of the "All in this cycle" option in the NamingPolicyInputDialog.
              When checked, you may choose to use one option for all subjects in this cycle and the user does not
              need to select an option for each file.

              An cycle is ended by calling endValidationCycle(). If you call startValidationCycle() while
              a cycle is already active the function has no effect. Thus you don't have to match the number of
              startValidationCycle() calls with the equal number of endValidationCycle() calls.
              */
            void startValidationCycle();
            //! Function which ends a naming validation cycle.
            /*!
              Ends a validation cycle started with startValidationCycle(). If no cycle was started this function does
              nothing.
              */
            void endValidationCycle();
            //! Returns true if a validation cycle is active at present.
            bool isValidationCycleActive() const;
            //! Checks if this subject filter is the name manager of the specified object.
            bool isObjectNameManager(QObject* obj) const;
            //! Sets the conflicting object. Only used from NamingPolicyInputDialog.
            void setConflictingObject(QObject* obj);
            //! Attempt to generate a valid name in the context from the given input_name.
            /*!
              The valid name generation attempts the following in the order shown.

              When invalid:
              - Attempt to fix the string using the QRegExpValidator::fixup() function
              - Go on to the steps shown below.

              When invalid and unique: (fixup worked)
              - Append the value of a counter to the name

              When valid and duplicate: (fixup failed)
              - Attempt to remove whitespaces
              - Try a simple string: new_object
              - Try a simple string without an underscore: NewObject

              This function takes the UniquenessPolicy of the subject filter into account when generating a valid name.

              \return A valid QString value. If QString is returned empty the function could not succeed in generating a valid name.
              */
            virtual QString generateValidName(QString input_name = QString(), bool force_change = false);
            //! Function which sets the name of the object.
            /*!
              This function checks if the subject filter is the name manager of the object, in that case
              it sets qti_prop_NAME. If not, it sets qti_prop_ALIAS_MAP with the subject filter's observer context ID.
              */
            void setName(QObject* object, const QString& new_name);
            //! Function which gets the name of the object.
            /*!
              This function checks if the subject filter is the name manager of the object, in that case
              it uses qti_prop_NAME. If not, it uses qti_prop_ALIAS_MAP with the subject filter's observer context ID.
              */
            QString getName(QObject* object);

        protected:
            //! Attempt to assign a new name manager to the object, other than this filter.
            void assignNewNameManager(QObject* obj);
            //! Check if the property actually changed during monitoredPropertyChanged() function call, thus check objectName() against the qti_prop_NAME property.
            bool isObjectNameDirty(QObject* obj) const;

            //! Validates if \p property_name is a valid name for \p obj in this context.
            /*!
              \note When you are loading a project and there is a chance that duplicate names might occur and the user changes it, you need to subclass NamingPolicyFilter and
              call PROJECT_MANAGER->markProjectAsChangedDuringLoad() in order for the project not to set your observer context as not modified.

              \returns True if the changed name is allowed, false otherwise.
              */
            virtual bool validateNamePropertyChange(QObject* obj, const char* property_name);

            NamingPolicyFilterData* d;
        };

        /*!
        \struct NamingPolicyFilterData
        \brief A structure storing protected data in the NamingPolicyFilter class.
          */
        struct NamingPolicyFilterData {
            NamingPolicyFilterData() : is_modified(false),
                conflicting_object(0) { }

            bool is_modified;
            QValidator* validator;
            INamingPolicyDialog* name_dialog;

            QString rollback_name;
            QPointer<QObject> conflicting_object;
            bool validation_cycle_active;
            NamingPolicyFilter::UniquenessPolicy uniqueness_policy;
            NamingPolicyFilter::ResolutionPolicy uniqueness_resolution_policy;
            NamingPolicyFilter::ResolutionPolicy validity_resolution_policy;
            //! Validation checks done while the observer context is busy with a processing cycle.
            NamingPolicyFilter::ValidationCheckFlags processing_cycle_validation_check_flags;
            //! Validation checks done while the observer context is NOT busy with a processing cycle.
            NamingPolicyFilter::ValidationCheckFlags validation_check_flags;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(NamingPolicyFilter::NameValidity)
									
        /*!
        \class Qtilities::CoreGui::NamingPolicyDelegateData
        \brief The NamingPolicyDelegateData class provides stores data needed by the NamingPolicyDelegate class.
          */
        struct NamingPolicyDelegateData;

        /*!
        \class Qtilities::CoreGui::NamingPolicyDelegate
        \brief The NamingPolicyDelegate class provides a ready-to-use delegate to edit the names of subjects attached to an observer.

        The naming policy delegate is used to allow editing of subject names in a text editor which is sensitive to a context's naming policy filter. Below is an example of the delegate in action in an observer widget.

        \image html observer_widget_naming_delegate.jpg "Observer Widget Handling Context Naming Policies"
        */
         class QTILITIES_CORE_GUI_SHARED_EXPORT NamingPolicyDelegate : public QItemDelegate
         {
             Q_OBJECT

         public:
             NamingPolicyDelegate(QObject *parent = 0);
             ~NamingPolicyDelegate();

            //! Sets the observer context for this delegate.
            void setObserverContext(Observer* observer);
            //! Gets a pointer to the observer context used by this delegate.
            Observer* observerContext() const;

            QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
            void setEditorData(QWidget *editor, const QModelIndex &index) const;
            void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
            void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

         public slots:
            void on_LineEdit_TextChanged(const QString & text);
            void handleCurrentObjectChanged(QList<QObject*> obj);

         private:
            NamingPolicyDelegateData* d;
         };									
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::CoreGui::NamingPolicyFilter& stream_obj);
QDataStream & operator>> (QDataStream& stream, Qtilities::CoreGui::NamingPolicyFilter& stream_obj);

#endif // NAMINGPOLICYFILTER_H
