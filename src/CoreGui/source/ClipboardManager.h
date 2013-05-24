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

#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include "IClipboard.h"
#include "QtilitiesCoreGui_global.h"

#include <Observer.h>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct ClipboardManagerPrivateData
          \brief The ClipboardManagerPrivateData class stores data used by the ClipboardManager class.
         */
        struct ClipboardManagerPrivateData;

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
            // IClipboard Implementation
            // --------------------------------
            ClipboardOrigin clipboardOrigin();
            void setClipboardOrigin(IClipboard::ClipboardOrigin new_origin);
            void initialize();
            void acceptMimeData();
            void setMimeData(QMimeData* mimeData);
            QMimeData* mimeData() const;
            void clearMimeData();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        public slots:
            void handleClipboardChanged();

        private:
            ClipboardManagerPrivateData* d;
        };
    }
}

#endif // CLIPBOARDMANAGER_H
