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

#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include "IClipboard.h"
#include "QtilitiesCoreGui_global.h"

#include <Observer.h>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \class ClipboardManagerData
          \brief The ClipboardManagerData class stores data used by the ClipboardManager class.
         */
        class ClipboardManagerData;

        /*!
          \class ClipboardManager
          \brief The ClipboardManager class provides information to keep track of copy/paste operations in the application's internal clipboard.

          See the Qtilities::CoreGui::Interfaces::IClipboard interface documentation for more information.

         */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ClipboardManager : public IClipboard
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IClipboard)
            friend class Qtilities::Core::Observer;

        public:
            ClipboardManager(QObject* parent = 0);
            ~ClipboardManager();

            // --------------------------------
            // IClipboard Implemenation
            // --------------------------------
            ClipboardOrigin clipboardOrigin();
            void setClipboardOrigin(IClipboard::ClipboardOrigin new_origin);
            void initialize();
            void acceptMimeData();
            void setMimeData(QMimeData* mimeData);
            QMimeData* mimeData() const;
            void clearMimeData();

        public slots:
            void handleClipboardChanged();

        private:
            ClipboardManagerData* d;
        };
    }
}

#endif // CLIPBOARDMANAGER_H
