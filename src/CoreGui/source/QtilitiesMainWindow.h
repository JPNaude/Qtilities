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

#ifndef QTILITIES_MAIN_WINDOW_H
#define QTILITIES_MAIN_WINDOW_H

#include "QtilitiesCoreGui_global.h"
#include "IMode.h"

#include <QMainWindow>
#include <QList>

namespace Ui
{
    class QtilitiesMainWindow;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \struct QtilitiesMainWindowData
        \brief A structure storing private data in the QtilitiesMainWindow class.
          */
        struct QtilitiesMainWindowData;

        /*!
        \class QtilitiesMainWindow
        \brief A class which can be used as a frontend of applications using the %Qtilities libraries.

        %Qtilities provides a main window architecture which allows you to create complex main windows easily. The Qtilities::CoreGui::QtilitiesMainWindow
        class is the main class. It supports modes that can be added to it, where modes are classes implementing the Qtilities::CoreGui::Interfaces::IMode interface.
        Each mode provide a widget and an icon identifying the mode. Modes are listed in in a top to bottom list view on the left hand of the main window and when a mode is clicked,
        its widget is set as the active widget in the main window.

        The Qtilities::CoreGui::DynamicSideWidgetViewer class is able to display widgets implementing the Qtilities::CoreGui::Interfaces::ISideViewerWidget interface.
        Each side viewer is wrapped using the Qtilities::CoreGui::DynamicSideWidgetWrapper class which provides actions to remove the side viewer or to duplicate the side viewer.
        The side viewer wrapper also allows side viewer widgets to add custom actions to the wrapper widget.

        Below is an example of the main window architecture in action. This screenshot was taken form the MainWindowExample in the QtilitiesExamples project:

        \image html main_window_architecture.jpg "Example Of Main Window Architecture"
        \image latex main_window_architecture.eps "Example Of Main Window Architecture" width=\textwidth
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT QtilitiesMainWindow : public QMainWindow
        {
            Q_OBJECT
        public:
            QtilitiesMainWindow(QWidget * parent = 0, Qt::WindowFlags flags = 0);
            ~QtilitiesMainWindow();

            //! Adds a mode to the main window.
            bool addMode(IMode* mode, bool initialize_mode = true);
            //! Adds a list of modes to the main window.
            void addModes(QList<IMode*> modes, bool initialize_modes = true);
            //! Adds a list of modes to the main window. This call will attempt to cast each object in the list to IMode* and add the successfull interfaces to the main window.
            void addModes(QList<QObject*> modes, bool initialize_modes = true);
            //! A list of the modes in this main window.
            QList<IMode*> modes() const;
            //! Returns the active mode.
            IMode* activeMode() const;

            //! Saves the state of the main window.
            void writeSettings();
            //! Restores the state of the main window.
            void readSettings();           

        public slots:
            //! The mode widget changes the central widget in the main window through this slot.
            void handleChangeCentralWidget(QWidget* new_central_widget);
            //! Slot through which a new mode can be set by specifying the mode ID.
            /*!
              \param mode_id The mode ID of the mode. \sa IMode::modeID().
              */
            void setActiveMode(int mode_id);
            //! Slot through which a new mode can be set by specifying the mode name.
            /*!
              \param mode_name The name of the mode. \sa IMode::text().
              */
            void setActiveMode(const QString& mode_name);
            //! Slot through which a new mode can be set by specifying the mode interface.
            void setActiveMode(IMode* mode_iface);

        private:
            Ui::QtilitiesMainWindow *ui;
            QtilitiesMainWindowData* d;
        };
    }
}
#endif // QTILITIES_MAIN_WINDOW_H
