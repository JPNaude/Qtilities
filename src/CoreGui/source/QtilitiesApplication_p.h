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

#ifndef QTILITIES_APPLICATION_P_H
#define QTILITIES_APPLICATION_P_H

#include "QtilitiesCoreGui_global.h"
#include "ActionManager.h"
#include "ClipboardManager.h"

#include <QMainWindow>
#include <QPointer>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \class QtilitiesApplicationPrivate
          \brief The QtilitiesApplicationPrivate class stores private data used by the QtilitiesApplication class.
         */
        class QTILITIES_CORE_GUI_SHARED_EXPORT QtilitiesApplicationPrivate {
        public:
            static QtilitiesApplicationPrivate* instance();
            ~QtilitiesApplicationPrivate();

            //! Function to access action manager pointer.
            Qtilities::CoreGui::Interfaces::IActionManager* actionManager() const;
            //! Function to access clipboard manager pointer.
            Qtilities::CoreGui::Interfaces::IClipboard* clipboardManager() const;
            //! Sets the main window reference for the application.
            void setMainWindow(QWidget* mainWindow);
            //! Gets the main window reference for the application.
            QWidget* mainWindow() const;
            //! Sets the configuration widget reference for the application.
            void setConfigWidget(QWidget* configWidget);
            //! Gets the configuration widget reference for the application.
            QWidget* configWidget();

        private:
            QtilitiesApplicationPrivate();
            static QtilitiesApplicationPrivate* m_Instance;

            QPointer<QWidget>       d_mainWindow;
            QPointer<QWidget>       d_configWindow;
            ActionManager*          d_actionManager;
            IActionManager*         d_actionManagerIFace;
            ClipboardManager*       d_clipboardManager;
            IClipboard*             d_clipboardManagerIFace;
        };
    }
}

#endif // QTILITIES_APPLICATION_P_H
