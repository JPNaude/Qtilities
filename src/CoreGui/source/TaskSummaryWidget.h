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

            //! Function which will search for all tasks in the global object pool and show them according to how the widget is set up.
            void findCurrentTasks();
            //! Function which will clear all current tasks shown by the summary widget.
            void clear();

        private slots:
            //! Slot which will check if obj is a task and register it if needed.
            void addTask(QObject* obj);
            //! Responds to state changes on a task.
            void handleTaskStateChanged();
            //! Responds to SingleTaskWidget deletions.
            void handleSingleTaskWidgetDestroyed();
            //! Handles task type changes.
            void handleTaskTypeChanged();

        private:
            //! Hides this widget if needed.
            void hideIfNeeded();
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
