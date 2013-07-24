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

#ifndef TASK_SUMMARY_WIDGET_H
#define TASK_SUMMARY_WIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "SingleTaskWidget.h"

#include <QMainWindow>

namespace Ui
{
    class TaskSummaryWidget;
}
class QStringListModel;

namespace Qtilities {
    namespace CoreGui {
        /*!
        \struct TaskSummaryWidgetPrivateData
        \brief A structure storing private data in the TaskSummaryWidget class.
          */
        struct TaskSummaryWidgetPrivateData;

        /*!
        \class TaskSummaryWidget
        \brief A widget which provides a summary of all registered global tasks.

        Qtilities::CoreGui::TaskSummaryWidget provides the ability to display all tasks in an application and is customizable in order for your needs. It can for example only show active tasks, or all tasks (active and inactive).

        To create a task summary widget is straight forward. For example:

\code
TaskSummaryWidget task_summary_widget;

// Tell the widget that it should not hide itself when now active tasks are visible:
task_summary_widget.setNoActiveTaskHandling(TaskSummaryWidget::ShowSummaryWidget);

// Find all current tasks in the global object pool:
task_summary_widget.findCurrentTasks();

// All tasks registered in the global object pool from here on will automatically be displayed
// in the summary widget.
\endcode

        It is important to note that only tasks registered in the global object pool and set as global tasks (see Qtilities::Core::Interfaces::ITask::TaskGlobal) are shown in
        the task summary widget. The figure below shows a task summary widget which shows three active tasks. Note that the QTimer task can be stopped.

        \image html class_tasksummarywidget_screenshot.jpg "Task Summary Widget"

        When using Qtilities::CoreGui::QtilitiesMainWindow you can set it up to show a task summary information automatically for you. This makes
        it very easy to provide an overview of tasks in your application. The <a class="el" href="namespace_qtilities_1_1_examples_1_1_tasks_example.html">Tasking Example</a>
        demonstrates this.

        See the \ref page_tasking article for more information on tasking.

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TaskSummaryWidget : public QMainWindow
        {
            Q_OBJECT

        public:
            //! The possible ways that SingleTaskWidget widgets can be displayed in the summary widget.
            /*!
              Default is DisplayOnlyBusyTasks.
              */
            enum TaskDisplayOptions {
                DisplayOnlyBusyTasks                                = 0,  /*!< Only busy tasks and tasks which are not busy but can be started (canStart()) are shown will be displayed. */
                DisplayAllTasks                                     = 1   /*!< All tasks registered in the global object pool will be displayed. */
            };
            Q_ENUMS(TaskDisplayOptions)
            //! Indicates how the TaskSummaryWidget should be shown when no SingleTaskWidget items are present.
            /*!
              Default is HideSummaryWidget.
              */
            enum NoActiveTaskHandling {
                HideSummaryWidget                                   = 0,  /*!< Hide the summary widget when no tasks are present. */
                ShowSummaryWidget                                   = 1   /*!< Show the summary widget when no tasks are present. */
            };
            Q_ENUMS(TaskDisplayOptions)
            //! The possible ways that SingleTaskWidget widgets can be removed from the summary widget.
            /*!
              In addition to the available remove options listed, tasks will always be removed when they are deleted.

              \note This setting has priority over Qtilities::Core::ITask::TaskRemoveAction.
              */
            enum TaskRemoveOption {
                RemoveWhenDeleted                                   = 0,  /*!< Remove tasks when they are deleted. This is always true. */
                RemoveWhenCompletedSuccessfully                     = 1,  /*!< Remove tasks when they complete without any warnings. */
                RemoveWhenCompletedSuccessfullyWithWarnings         = 2,  /*!< Remove tasks when they complete with warnings. */
                RemoveWhenFailed                                    = 4,  /*!< Remove tasks when they failed. */
                RemoveWhenStopped                                   = 8,  /*!< Remove tasks when they are stopped. */
                RemoveDefault                                       = RemoveWhenCompletedSuccessfully
            };
            Q_ENUMS(TaskRemoveOption)
            Q_DECLARE_FLAGS(TaskRemoveOptionFlags, TaskRemoveOption)
            Q_FLAGS(TaskRemoveOptionFlags)

            TaskSummaryWidget(TaskRemoveOption remove_options = RemoveDefault, TaskDisplayOptions display_options = DisplayOnlyBusyTasks, QWidget * parent = 0);
            ~TaskSummaryWidget();

            //! Gets the TaskDisplayOptions for this summary widget.
            TaskDisplayOptions taskDisplayOptions() const;
            //! Sets the TaskDisplayOptions for this summary widget.
            void setTaskDisplayOptions(TaskDisplayOptions task_display_options);
            //! Gets the NoActiveTaskHandling for this summary widget.
            NoActiveTaskHandling noActiveTaskHandling() const;
            //! Sets the NoActiveTaskHandling for this summary widget.
            void setNoActiveTaskHandling(NoActiveTaskHandling no_active_task_handling);
            //! Gets the TaskRemoveOptionFlags for this summary widget.
            TaskRemoveOptionFlags taskRemoveOptionFlags() const;
            //! Sets the TaskRemoveOptionFlags for this summary widget.
            void setTaskRemoveOptionFlags(TaskRemoveOptionFlags task_remove_option_flags);

            //! Gets if this task summary widget is enabled.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            bool taskSummaryEnabled() const;
            //! Sets if this task summary widget is enabled.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setTaskSummaryEnabled(bool enable);

            //! Function which will search for all tasks in the global object pool and show them according to how the widget is set up.
            void findCurrentTasks();
            //! Function which will clear all current tasks shown by the summary widget.
            void clear();

            //! Sets a filter in order to only display tasks with IDs that match the filter.
            /*!
             * When empty, all tasks are displayed. This is the default.
             *
             * \sa displayedTasksFilter(), displayedTasksFilterAddId(), displayedTasksFilterRemoveId()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void setDisplayedTasksFilter(QList<int> displayed_task_ids);
            //! Gets the current displayed tasks filter.
            /*!
             * \sa setDisplayedTasksFilter(), displayedTasksFilterAddId(), displayedTasksFilterRemoveId()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            QList<int> displayedTasksFilter() const;
            //! Adds a task ID to the displayed tasks filter.
            /*!
             * \sa displayedTasksFilter(), setDisplayedTasksFilter(), displayedTasksFilterRemoveId()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void displayedTasksFilterAddId(int id);
            //! Adds a task ID to the displayed tasks filter.
            /*!
             * \sa displayedTasksFilter(), setDisplayedTasksFilter(), displayedTasksFilterAddId()
             *
             * <i>This function was added in %Qtilities v1.3.</i>
             */
            void displayedTasksFilterRemoveId(int id);
            //! Function to access the SingleTaskWidget contained in the summary widget for a specific task.
            /*!
             * \returns The SingleTaskWidget if such a widget exists for the specified task at the time when this
             * function is called. Null if no such widget exists for the specified task at the time when this function is
             * called.
             *
             * <i>This function was added in %Qtilities v1.5.</i>
             */
            QPointer<SingleTaskWidget> getSingleTaskWidgetForTask(int task_id);

        private slots:
            //! Slot which will check if obj is a task and register it if needed.
            void addTask(QObject* obj);
            //! Responds to state changes on a task.
            void handleTaskStateChanged();
            //! Responds to SingleTaskWidget deletions.
            void handleSingleTaskWidgetDestroyed();
            //! Handles task type changes.
            void handleTaskTypeChanged();
            //! Hides this widget if needed.
            void hideIfNeeded();

        signals:
            //! Signal which is emitted when the number of displayed tasks changed.
            /*!
             * <i>This signal was added in %Qtilities v1.3.</i>
             */
            void numberOfDisplayedTasksChanged(int number_of_visible_tasks);

        private:
            //! Adds a single task widget to the visible single task widgets.
            void addSingleTaskWidget(SingleTaskWidget* single_task_widget);
            //! Updates the display of a single task.
            void updateTaskWidget(ITask* task);

            Ui::TaskSummaryWidget *ui;
            TaskSummaryWidgetPrivateData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(TaskSummaryWidget::TaskRemoveOptionFlags)
    }
}
#endif // TASK_SUMMARY_WIDGET_H
