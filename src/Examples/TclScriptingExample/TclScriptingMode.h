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

#ifndef TCL_SCRIPTING_MODE_H
#define TCL_SCRIPTING_MODE_H

#include <IMode.h>

#include <QMainWindow>

namespace Ui {
    class TclScriptingMode;
}

namespace Qtilities {
    //! Namespace containing all the %Qtilities examples.
    namespace Examples {
        //! Namespace containing all the classes which forms part of the Tcl Scripting Example.
        /*!
          */
        namespace TclScripting {
            using namespace Qtilities::CoreGui::Interfaces;

            // Tcl Scripting Mode Parameters
            #define TCL_SCRIPTING_MODE_ID                   998
            const char * const TCL_SCRIPTING_MODE_CONTEXT   = "Context.TclScriptingMode";

            /*!
              \struct TclScriptingModePrivateData
              \brief The TclScriptingModePrivateData class stores private data used by the TclScriptingMode class.
             */
            struct TclScriptingModePrivateData;

            /*!
            \class TclScriptingMode
            \brief An example mode widget which demonstrates the dynamic side widget architecture.
              */
            class TclScriptingMode : public QMainWindow, public IMode {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)
            public:
                TclScriptingMode(QWidget* parent = 0);
                ~TclScriptingMode();

                QWidget* dock();

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // IMode Implementation
                // --------------------------------------------
                QWidget* modeWidget();
                void initializeMode();
                QIcon modeIcon() const;
                QString modeName() const;
                QString contextString() const { return TCL_SCRIPTING_MODE_CONTEXT; }
                QString contextHelpId() const { return QString(); }
                int modeID() const { return TCL_SCRIPTING_MODE_ID; }

            private:
                Ui::TclScriptingMode *ui;
                TclScriptingModePrivateData* d;
            };
        }
    }
}

#endif // TCL_SCRIPTING_MODE_H
