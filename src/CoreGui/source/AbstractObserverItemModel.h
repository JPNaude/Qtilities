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

#ifndef ABSTRACTOBSERVERITEMMODEL_H
#define ABSTRACTOBSERVERITEMMODEL_H

#include "QtilitiesCoreGui_global.h"
#include "NamingPolicyFilter.h"

#include <Observer>
#include <ObserverHints>
#include <ActivityPolicyFilter>

#include <QModelIndex>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct AbstractObserverItemModelData
        \brief Structure used by AbstractObserverItemModel to store private data.
          */
        struct AbstractObserverItemModelData {
            AbstractObserverItemModelData() : respond_to_observer_changes(true),
                child_count_base("QObject"),
                child_count_limit(-1) { }

            //! Used to store default observer hints to be used with this widget.
            QPointer<ObserverHints>         hints_default;
            //! Used to store observer hints for the root observer (top level observer)
            QPointer<ObserverHints>         hints_top_level_observer;
            //! Used to store observer hints for the current selection parent observer.
            QPointer<ObserverHints>         hints_selection_parent;
            //! Indicates if the default observer hints, or the current selection parent observer hints are used.
            bool                            use_observer_hints;

            //! The activity policy filter of the current observer context, if present.
            QPointer<ActivityPolicyFilter>  activity_filter;
            //! The naming policy filter of the current observer context, if present.
            QPointer<NamingPolicyFilter>    naming_filter;

            //! Indicates if this model responds to changes to the observer context.
            bool                            respond_to_observer_changes;
            //! Indicates if this model is read only.
            bool                            read_only;
            //! The base class name used to count items in ColumnChildCount if it is shown.
            QString                         child_count_base;
            //! The limit for the counter counting tree children in the ColumnChildCount if it is shown.
            int                             child_count_limit;
            //! Indicates if lazy initialization is enabled for this mode.
            bool                            lazy_init;
        };

        /*!
          \class AbstractObserverItemModel
          \brief The AbstractObserverItemModel is an abstract base class which is used by all the different observer models in the %Qtilities library.

          This class inhertis ObserverAwareBase which is used to define the context (top level context in tree views) to be used
          in the model. The hints which should be used by the model is provided by the displayHints() function.
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT AbstractObserverItemModel : public ObserverAwareBase
        {

        public:
            AbstractObserverItemModel();
            virtual ~AbstractObserverItemModel();

            //! Enables/disables lazy initialization.
            /*!
              Lazy initialization means that the internal item view model won't initialize itself during the setObserverContext() call. Its usefull when
              using custom implemented proxy models etc. To initialize the model in lazy initialization mode call refresh().

              Lazy initialization is disabled by default.

              \note You must call this function before setObserverContext() for it to have any effect.
              \note From the current models in %Qtilities, only ObserverTreeModel makes use of lazy initialialization. For ObserverTableModel this is not neccesarry.

              \sa lazyInitEnabled()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void toggleLazyInit(bool enabled);
            //! Gets if lazy initialization is enabled.
            /*!
              \sa toggleLazyInit()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool lazyInitEnabled() const;

            //! The possible columns which can be requested by views for an module.
            enum ColumnID {
                ColumnSubjectID,
                ColumnName,
                ColumnChildCount,
                ColumnAccess,
                ColumnTypeInfo,
                ColumnCategory,
                ColumnLast
            };
            //! The model implementation must return the index (starting at 0) for the requested column.
            /*!
              \note If your model does not support a specific column, return -1.
              \note ColumnLast must return the number of columns you use in your model.
              */
            virtual int columnPosition(ColumnID column_id) const = 0;

            //! Abstract function which must be implemented to get the subject ID of the object at the given index.
            virtual int getSubjectID(const QModelIndex &index) const = 0;
            //! Abstract function which must be implemented to get the object at the given index.
            virtual QObject* getObject(const QModelIndex &index) const = 0;

            //! Function to toggle usage of hints from the active parent observer. If not custom hints will be used.
            /*!
              \sa activeHints(), setCustomHints()
              */
            void toggleUseObserverHints(bool toggle);
            //! Function to indicate if observer hints are used in this model. If not, the default hints in the model base class is used.
            bool usesObserverHints() const;
            //! This function allows you to copy the custom hints used by this model from a different ObserverHints instance.
            /*!
              \note These hints are only used when usesObserverHints() is false.
              \note The model will take ownership of the custom hints instance.

              \sa toggleUseObserverHints()
              */
            bool setCustomHints(ObserverHints* custom_hints);
            //! This function will provide the hints which should be used by this model at any time.
            /*!
              \sa toggleUseObserverHints()
              */
            ObserverHints* activeHints() const;

            //! Implement the virtual function to get references to known filters.
            virtual bool setObserverContext(Observer* observer);

            //! Sets if this model responds to changes from the observer context displayed.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setRespondToObserverChanges(bool respond_to_observer_changes);
            //! Gets if this model responds to changes from the observer context displayed.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool respondToObserverChanges() const;

            //! Sets if this model must be read only, thus its actions and property editor will be read only.
            /*!
              \sa readOnly()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            virtual void setReadOnly(bool read_only);
            //! Gets if this model must be read only, thus its actions and property editor will be read only.
            /*!
              \sa setReadOnly()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool readOnly() const;

            //! Sets the base class to use for tree count operations in the ColumnChildCount column if shown.
            /*!
              This base_class_name parameter will be passed on to Qtilities::Core::Observer::treeCount().

              \note This function not refresh the view.

              \sa columnChildCountBaseClass()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setColumnChildCountBaseClass(const QString& base_class_name);
            //! Gets the base class to use for tree count operations in the ColumnChildCount column if shown.
            /*!
              \sa setColumnChildCountBaseClass()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            QString columnChildCountBaseClass() const;
            //! Sets the upper limit tree count operations in the ColumnChildCount column if shown.
            /*!
              This base_class_name parameter will be passed on to Qtilities::Core::Observer::treeCount().

              The default is -1, thus not limit.

              \note This function not refresh the view.

              \sa columnChildCountLimit()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setColumnChildCountLimit(int limit);
            //! Gets the upper limit to use for tree count operations in the ColumnChildCount column if shown.
            /*!
              \sa setColumnChildCountLimit()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            int columnChildCountLimit() const;

            //! Forces this model to refresh its layout.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            virtual void refresh() = 0;

        protected:
            AbstractObserverItemModelData* model;
        };
    }
}

#endif //  ABSTRACTOBSERVERITEMMODEL_H
