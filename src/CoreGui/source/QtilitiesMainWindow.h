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

#ifndef QTILITIES_MAIN_WINDOW_H
#define QTILITIES_MAIN_WINDOW_H

#include "QtilitiesCoreGui_global.h"
#include "IMode.h"
#include "ModeManager.h"

#include <QResizeEvent>
#include <QMainWindow>
#include <QLabel>

#include <Logger>

namespace Ui
{
    class QtilitiesMainWindow;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::Logging;
        class TaskSummaryWidget;

        /*!
        \struct QtilitiesMainWindowPrivateData
        \brief A structure storing private data in the QtilitiesMainWindow class.
          */
        struct QtilitiesMainWindowPrivateData;

        /*!
        \class QtilitiesMainWindow
        \brief A class which can be used as a frontend of applications using the %Qtilities libraries.

        The QtilitiesMainWindow extends the normal QMainWindow class by adding the following:
        - Automatic state storing/loading
        - Ability to display application modes through objects implementing the Qtilities::CoreGui::Interfaces::IMode interface.
        - Provides ready to use integration with the %Qtilities logger's priority messages.

        %Qtilities provides a main window architecture which allows you to create complex main windows easily.
        Once you created your widget, it is recommended to set it as the main window on QtilitiesApplication as shown below:

\code
int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setApplicationName("Main Window Example");

    // Create a QtilitiesMainWindow to show our different modes:
    QtilitiesMainWindow* example_main_window = new QtilitiesMainWindow(QtilitiesMainWindow::ModesLeft);
    QtilitiesApplication::setMainWindow(&exampleMainWindow);

    // Lots of application code...
}
\endcode

        Application modes are supported, where modes are classes implementing the Qtilities::CoreGui::Interfaces::IMode interface.
        Each mode provide a widget and an icon identifying the mode. Modes are listed in the main window depending on modeLayout() setting.
        When a mode's icon is clicked, its widget is set as the active widget in the main window.
        It is also possible to the use the main window without modes by using the ModesNone modeLayout().

        Below is an example of the main window in action:

        \image html class_qtilitiesmainwindow_modes_left.jpg "Main Window With Modes On The Left"

        Alternatively, we can put the modes on either the top, right or bottom. For example:

        To create your widget, add something like the following to your \p main.cpp code:

\code
int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);
    QtilitiesApplication::setOrganizationName("Jaco Naude");
    QtilitiesApplication::setApplicationName("Main Window Example");

    // Create a QtilitiesMainWindow with modes at the top:
    QtilitiesMainWindow* example_main_window = new QtilitiesMainWindow(QtilitiesMainWindow::ModesTop);
    QtilitiesApplication::setMainWindow(example_main_window);

    // Lots of application code...
\endcode

        The result is:

        \image html class_qtilitiesmainwindow_modes_top.jpg "Main Window With Modes At The Top"

        The QtilitiesMainWindow widget supports the %Qtilities Logger's priority messaging functionality and
        displays the priority messages in the status bar of the main window by default. To disable this feature
        the disablePriorityMessages() function can be called.

        Further, the Qtilities::CoreGui::DynamicSideWidgetViewer class is able to display widgets implementing the Qtilities::CoreGui::Interfaces::ISideViewerWidget interface.
        Each side viewer is wrapped using the Qtilities::CoreGui::DynamicSideWidgetWrapper class which provides actions to remove the side viewer or to duplicate the side viewer.
        For more information on this see the relevant class documentation.

        It is also possible to show a Qtilities::CoreGui::TaskSummaryWidget automatically using QtilitiesMainWindow through showTaskSummaryWidget(). You can
        access the task summary widget shown through taskSummaryWidget().
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT QtilitiesMainWindow : public QMainWindow
        {
            Q_OBJECT
            Q_ENUMS(ModeLayout)

        public:
            //! The possible places where modes can be displayed.
            /*!
              The default is ModesNone.
              */
            enum ModeLayout {
                ModesNone = 0,         /*!< No mode display. */
                ModesTop = 1,          /*!< Display modes as a horizontal list in the top of the widget. */
                ModesRight = 2,        /*!< Display modes as a vertical list in the right of the widget. */
                ModesBottom = 3,       /*!< Display modes as a horizontal list in the bottom of the widget. */
                ModesLeft = 4          /*!< Display modes as a vertical list in the left of the widget. */
            };

            QtilitiesMainWindow(ModeLayout modeLayout = ModesNone, QWidget * parent = 0, Qt::WindowFlags flags = 0);
            ~QtilitiesMainWindow();

            bool eventFilter(QObject *object, QEvent *event);

            // ----------------------------------
            // Functions related to modes
            // ----------------------------------
            //! Returns a reference to the mode widget which allows management of modes in the main window.
            /*!
              \note <b>Important:</b> If modeLayout() is ModesNone, this function returns null. To use modes make sure you pass your mode layout in the constructor.
              */
            ModeManager* modeManager();
            //! Returns the current mode layout of the widget.
            ModeLayout modeLayout() const;

            // ----------------------------------
            // Functions related to tasks
            // ----------------------------------
            //! Shows a task summary widget next to the mode list widget.
            /*!
            When using Qtilities::CoreGui::QtilitiesMainWindow you can set it up to show a task summary information automatically for you. This makes
            it very easy to provide an overview of tasks in your application. The <a class="el" href="namespace_qtilities_1_1_examples_1_1_tasks_example.html">Tasking Example</a>
            demonstrates this:

            \image html example_tasking.jpg "Tasking Example"

            \note The task summary widget is only available when the ModeLayout() is set up to actually show modes.
            \note At present the task summary is only supported with ModesTop and ModesBottom.

            \sa hideTaskSummaryWidget(), taskSummaryWidgetVisible(), taskSummaryWidget()
            */
            void showTaskSummaryWidget();
            //! Hides a task summary widget next to the mode list widget.
            /*!
              \note The task summary widget is only available when the ModeLayout() is set up to actually show modes.
              \note At present the task summary is only supported with ModesTop and ModesBottom.

              \sa showTaskSummaryWidget(), taskSummaryWidgetVisible(), taskSummaryWidget()
              */
            void hideTaskSummaryWidget();
            //! Hides a task summary widget next to the mode list widget.
            /*!
              False by default.

              \note The task summary widget is only available when the ModeLayout() is set up to actually show modes.
              \note At present the task summary is only supported with ModesTop and ModesBottom.

              \sa setShowTaskSummaryWidget(), taskSummaryWidget()
              */
            bool taskSummaryWidgetVisible() const;
            //! Function which provides access to the label used to display priority messages.
            /*!
              Access to the lable allows you to modify it according to your needs. For example, we can
              enable/disable word wrapping etc.

              \sa showTaskSummaryWidget(), hideTaskSummaryWidget(), taskSummaryWidgetVisible()

              \note At present the task summary is only supported with ModesTop and ModesBottom.

              This function was added in Qtilities v1.1.
              */
            TaskSummaryWidget* taskSummaryWidget() const;

            // ----------------------------------
            // Functions related to widget setup
            // ----------------------------------
            //! Saves the state of the main window.
            void writeSettings();
            //! Restores the state of the main window.
            void readSettings();
            //! Function to enabled priority messages in the status bar of the main window.
            /*!
              Enabled by default.

              \sa disablePriorityMessages(), priorityMessageLabel()
              */
            void enablePriorityMessages();
            //! Function to disable priority messages in the status bar of the main window.
            /*!
              Enabled by default.

              \sa enablePriorityMessages(), priorityMessageLabel()
              */
            void disablePriorityMessages();
            //! Function which provides access to the label used to display priority messages.
            /*!
              Access to the lable allows you to modify it according to your needs. For example, we can
              enable/disable word wrapping etc.

              \sa enablePriorityMessages(), disablePriorityMessages()

              This function was added in Qtilities v1.1.
              */
            QLabel* priorityMessageLabel() const;

        public slots:
            //! Slot which received incomming priority messages from the %Qtilities logger.
            void processPriorityMessage(Logger::MessageType message_type, const QString& message);

        private slots:
            //! The mode widget changes the central widget in the main window through this slot.
            void changeCurrentWidget(QWidget* new_central_widget);
            //! Updates sizes of itesm in the main windows when modes are added to the application:
            void updateItemSizes();

        private:
            //! Do the layout required in the main widget.
            void doLayout();

            Ui::QtilitiesMainWindow *ui;
            QtilitiesMainWindowPrivateData* d;
        };
    }
}
#endif // QTILITIES_MAIN_WINDOW_H
