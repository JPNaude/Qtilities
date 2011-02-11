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

#ifndef EXAMPLE_MODE_H
#define EXAMPLE_MODE_H

#include <IMode.h>

#include <QMainWindow>

namespace Ui {
    class ExampleMode;
}

namespace Qtilities {
    //! Namespace containing all the %Qtilities examples.
    namespace Examples {
        //! Namespace containing all the classes which forms part of the Main Window Example.
        /*!
        An example which allows the user to browse the file system and open files in the file system.
        The main goal of this example is to demonstrate the dynamic side widget concept of the %Qtilities main
        window architecture. The example is almost the same as the Object Management Example in terms of
        action management, plugin management etc. The main difference is that a mode was added to the main
        window which demonstrates the use of dynamic side widgets. One thing that this plugin does as well
        is to show a splash window at startup providing progress messages from the extension system.

        This example uses the following modules and plugins:
        - <a class="el" href="namespace_qtilities_1_1_logging.html">Logging Module</a>
        - <a class="el" href="namespace_qtilities_1_1_core.html">Core Module</a>
        - <a class="el" href="namespace_qtilities_1_1_core_gui.html">CoreGui Module</a>
        - <a class="el" href="namespace_qtilities_1_1_extension_system.html">Extension System Module</a>
        - <a class="el" href="namespace_qtilities_1_1_plugins_1_1_session_log.html">Session Log Plugin</a>
        - <a class="el" href="namespace_qtilities_1_1_plugins_1_1_debug.html">Debug Plugin</a>

        Below is a screenshot of this example in action:
        \image html example_main_window.jpg "Main Window Example"
          */
        namespace MainWindow {
            using namespace Qtilities::CoreGui::Interfaces;

            // Object Management Mode Parameters
            #define MODE_EXAMPLE_ID                   998
            const char * const CONTEXT_EXAMPLE_MODE   = "Context.ObjectManagementMode";

            /*!
              \struct ExampleModePrivateData
              \brief The ExampleModePrivateData class stores private data used by the ExampleMode class.
             */
            struct ExampleModePrivateData;

            /*!
            \class ExampleMode
            \brief An example mode widget which demonstrates the dynamic side widget architecture..
              */
            class ExampleMode : public QMainWindow, public IMode {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)
            public:
                ExampleMode(QWidget *parent = 0);
                ~ExampleMode();
                bool eventFilter(QObject *object, QEvent *event);

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
                QString contextString() const { return CONTEXT_EXAMPLE_MODE; }
                QString contextHelpId() const { return QString(); }
                int modeID() const { return MODE_EXAMPLE_ID; }

            public slots:
                void toggleDock(bool toggle);
                //! Loads file into text editor.
                void loadFile(const QString& file_name);
                //! Slot which handles new side widget file system instances being produced.
                void handleNewFileSystemWidget(QWidget* widget);

            protected:
                void changeEvent(QEvent *e);

            private:
                Ui::ExampleMode *ui;
                ExampleModePrivateData* d;
            };
        }
    }
}

#endif // EXAMPLE_MODE_H
