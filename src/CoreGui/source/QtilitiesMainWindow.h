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

#ifndef QTILITIES_MAIN_WINDOW_H
#define QTILITIES_MAIN_WINDOW_H

#include "QtilitiesCoreGui_global.h"
#include "IMode.h"
#include "ModeManager.h"

#include <QResizeEvent>
#include <QMainWindow>
#include <Logger>

namespace Ui
{
    class QtilitiesMainWindow;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::Logging;

        /*!
        \struct QtilitiesMainWindowData
        \brief A structure storing private data in the QtilitiesMainWindow class.
          */
        struct QtilitiesMainWindowData;

        /*!
        \class QtilitiesMainWindow
        \brief A class which can be used as a frontend of applications using the %Qtilities libraries.

        The QtilitiesMainWindow extends the normal QMainWindow class by adding the following:
        - Automatic state storing/loading
        - Ability to display application modes through objects implementing the Qtilities::CoreGui::Interfaces::IMode interface.
        - Provides ready to use integration with the %Qtilities logger's priority messages.

        %Qtilities provides a main window architecture which allows you to create complex main windows easily. The Qtilities::CoreGui::QtilitiesMainWindow
        class is the main class. It supports modes that can be added to it, where modes are classes implementing the Qtilities::CoreGui::Interfaces::IMode interface.
        Each mode provide a widget and an icon identifying the mode. Modes are listed in somewhere in the main window depending on modeLayout() list view on the left hand
        of the main window and when a mode is clicked, its widget is set as the active widget in the main window.

        The Qtilities::CoreGui::DynamicSideWidgetViewer class is able to display widgets implementing the Qtilities::CoreGui::Interfaces::ISideViewerWidget interface.
        Each side viewer is wrapped using the Qtilities::CoreGui::DynamicSideWidgetWrapper class which provides actions to remove the side viewer or to duplicate the side viewer.
        The side viewer wrapper also allows side viewer widgets to add custom actions to the wrapper widget.

        Below is an example of the main window architecture in action. This screenshot was taken form the MainWindowExample in the QtilitiesExamples project:

        \image html main_window_architecture.jpg "Example Of Main Window Architecture"

        The QtilitiesMainWindow widget supports the %Qtilities Logger's priority messaging functionality and
        displays the priority messages in the status bar of the main window by default. To disable this feature
        the disablePriorityMessages() function can be called.
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
            // Functions related to widget setup
            // ----------------------------------
            //! Saves the state of the main window.
            void writeSettings();
            //! Restores the state of the main window.
            void readSettings();
            //! Function to enabled priority messages in the status bar of the main window.
            void enablePriorityMessages();
            //! Function to disable priority messages in the status bar of the main window.
            void disablePriorityMessages();

        public slots:
            //! The mode widget changes the central widget in the main window through this slot.
            void changeCurrentWidget(QWidget* new_central_widget);
            //! Slot which received incomming priority messages from the &Qtilities logger.
            void processPriorityMessage(Logger::MessageType message_type, const QString& message);

        private:
            Ui::QtilitiesMainWindow *ui;
            QtilitiesMainWindowData* d;
        };
    }
}
#endif // QTILITIES_MAIN_WINDOW_H
