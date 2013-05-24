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

#ifndef QTILITIES_APPLICATION_P_H
#define QTILITIES_APPLICATION_P_H

#include "QtilitiesCoreGui_global.h"
#include "ActionManager.h"
#include "ClipboardManager.h"

#ifndef QTILITIES_NO_HELP
#include "HelpManager.h"
#endif

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

            #ifndef QTILITIES_NO_HELP
            //! Function to access help manager pointer.
            HelpManager *helpManager() const;
            #endif

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
            #ifndef QTILITIES_NO_HELP
            HelpManager*            d_helpManager;
            #endif
        };
    }
}

#endif // QTILITIES_APPLICATION_P_H
