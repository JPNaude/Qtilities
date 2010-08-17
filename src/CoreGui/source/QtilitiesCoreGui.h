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

#ifndef QtilitiesCoreGui_H
#define QtilitiesCoreGui_H

#include "QtilitiesCoreGui_global.h"
#include "IActionManager.h"
#include "IClipboard.h"

#include <QList>
#include <QString>
#include <QMainWindow>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct QtilitiesCoreGuiData
          \brief The QtilitiesCoreGuiData class stores private data used by the QtilitiesCoreGui class.
         */
        struct QtilitiesCoreGuiData;

        /*!
          \class QtilitiesCoreGui
          \brief The QtilitiesCore is a singleton which provides accecss to the action and clipboard managers among other things.
         */

        class QTILITIES_CORE_GUI_SHARED_EXPORT QtilitiesCoreGui : public QObject
        {
            Q_OBJECT

        public:
            static QtilitiesCoreGui* instance();
            ~QtilitiesCoreGui();

            //! Returns a reference to the action manager. This reference will always be valid, thus there is no need to check the validity of the pointer.
            IActionManager* actionManager();
            //! Returns a reference to the clipboard manager. This reference will always be valid, thus there is no need to check the validity of the pointer.
            IClipboard* clipboardManager();

            //! Sets the main window reference for the application.
            void setMainWindow(QMainWindow* mainWindow);
            //! Gets the main window reference for the application.
            QMainWindow* mainWindow();

        public slots:
            //! Displays an Qtilities::CoreGui::AboutWindow with information about the Qtilities libraries.
            void aboutQtilities();

        private:
            QtilitiesCoreGui(QObject* parent = 0);

            static QtilitiesCoreGui* m_Instance;
            QtilitiesCoreGuiData* d;
        };
    }
}

#define ACTION_MANAGER Qtilities::CoreGui::QtilitiesCoreGui::instance()->actionManager()
#define CLIPBOARD_MANAGER Qtilities::CoreGui::QtilitiesCoreGui::instance()->clipboardManager()

#endif // QtilitiesCoreGui_H
