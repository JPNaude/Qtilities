/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#ifndef SINGLE_TASK_WIDGET_H
#define SINGLE_TASK_WIDGET_H

#include "QtilitiesCoreGui_global.h"

#include <ITask>

#include <QWidget>
#include <QProgressBar>

namespace Ui
{
    class SingleTaskWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core::Interfaces;

        /*!
        \struct SingleTaskWidgetPrivateData
        \brief A structure storing private data in the SingleTaskWidget class.
          */
        struct SingleTaskWidgetPrivateData;

        /*!
        \class SingleTaskWidget
        \brief A widget provides an overview of a single task.

        Qtilities::CoreGui::SingleTaskWidget is the most basic tasking widget and it is used to represent a single task and can easily be constructed as follows:

\code
Task* task = new Task("Timer Task");
OBJECT_MANAGER->registerObject(task);

SingleTaskWidget* task_widget = TaskManagerGui::instance()->singleTaskWidget(task->taskID());
\endcode

        The figure below shows a single task widget for an example task. Note that clicking on the green icon on the left of the progress bar allows you to view the log for the
        specific task. Also, the task widget will only show controls for the task according to the way the task was set up. In the QTimer example above we call \p setCanStop(true),
        thus the single task widget will provide the ability to stop the task. If it was set to false, the red stop button would have been disabled.

        \image html class_singletaskwidget_screenshot.jpg "Single Task Widget"

        The single task widget can be added anywhere in your application where you would like to display information about a task.

        See the \ref page_tasking article for more information on tasking.

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT SingleTaskWidget : public QWidget
        {
            Q_OBJECT

        public:
            SingleTaskWidget(int task_id, QWidget * parent = 0);
            ~SingleTaskWidget();

            void resizeEvent(QResizeEvent * event);

            //! Returns the task shown by this widget.
            ITask* task() const;

            //! Sets if the pause task button must be visible.
            /*!
              By default this will depend on the canPause() result on the task being shown.
              */
            void setPauseButtonVisible(bool is_visible);
            //! Gets if the pause task button must be visible.
            bool pauseButtonVisible() const;
            //! Sets if the stop task button must be visible.
            /*!
              By default this will depend on the canStop() result on the task being shown.
              */
            void setStopButtonVisible(bool is_visible);
            //! Gets if the stop task button must be visible.
            bool stopButtonVisible() const;
            //! Sets if the start task button must be visible.
            /*!
              By default this will depend on the canStart() result on the task being shown.
              */
            void setStartButtonVisible(bool is_visible);
            //! Gets if the start task button must be visible.
            bool startButtonVisible() const;

            //! Access to the progress bar shown.
            QProgressBar* progressBar();

        private slots:
            void update();
            void on_btnShowLog_clicked();
            void on_btnPause_clicked();
            void on_btnStop_clicked();
            void on_btnStart_clicked();

            void handleTaskDeleted();
            void updateBusyState(ITask::TaskBusyState busy_state);
            //! Sets the displayed name of the task and elides it if needed.
            void setDisplayedName(const QString& name);

        private:
            Ui::SingleTaskWidget *ui;
            SingleTaskWidgetPrivateData* d;
        };
    }
}
#endif // SINGLE_TASK_WIDGET_H

