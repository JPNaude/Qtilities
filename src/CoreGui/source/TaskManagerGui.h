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

#ifndef TASK_MANAGER_GUI_H
#define TASK_MANAGER_GUI_H

#include "QtilitiesCoreGui_global.h"
#include "SingleTaskWidget.h"
#include "TaskSummaryWidget.h"

#include <QPointer>
#include <QApplication>

namespace Qtilities {
    namespace CoreGui {   
        /*!
        \struct TaskManagerGuiPrivateData
        \brief Structure used by TaskManagerGui to store private data.
          */
        struct TaskManagerGuiPrivateData;

        /*!
        \class TaskManagerGui
        \brief A class providing static member functions to acccess and create GUIs related to task management.

        \sa Qtilities::Core::TaskManager
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TaskManagerGui : public QObject {
            Q_OBJECT

        public:
            static TaskManagerGui* instance();
            virtual ~TaskManagerGui();

        private:
            TaskManagerGui();

        public:
            //! Creates a SingleTaskWidget for the given task.
            /*!
              \param task_id The task ID for which a widget must be produced.
              */
            SingleTaskWidget* singleTaskWidget(int task_id);

        private slots:
            //! Slot which will check if obj is a task and assign a logger engine to it if needed.
            AbstractLoggerEngine* assignLoggerEngineToTask(QObject* obj);

        private:
            static TaskManagerGui* m_Instance;
            TaskManagerGuiPrivateData* d;
        };
    }
}

#endif // TASK_MANAGER_GUI_H
