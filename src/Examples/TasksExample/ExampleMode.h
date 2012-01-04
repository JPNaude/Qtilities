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

#include <IMode>
#include <ITask>

#include <QMainWindow>
#include <QFileInfoList>

namespace Ui {
    class ExampleMode;
}

namespace Qtilities {
    //! Namespace containing all the %Qtilities examples.
    namespace Examples {
        using namespace Qtilities::Core::Interfaces;

        //! Namespace containing all the classes which forms part of the Tasks Example.
        /*!
        An example which demonstrates the use of tasking in the %Qtilities libraries.

        This example uses the following modules and plugins:
        - <a class="el" href="namespace_qtilities_1_1_logging.html">Logging Module</a>
        - <a class="el" href="namespace_qtilities_1_1_core.html">Core Module</a>
        - <a class="el" href="namespace_qtilities_1_1_core_gui.html">CoreGui Module</a>

        Below is a screenshot of this example in action:
        \image html example_tasking.jpg "Tasking Example"
          */
        namespace TasksExample {
            using namespace Qtilities::CoreGui::Interfaces;

            // Object Management Mode Parameters
            #define MODE_EXAMPLE_ID                   998
            const char * const CONTEXT_EXAMPLE_MODE   = "Context.TaskingMode";

            /*!
              \struct ExampleModePrivateData
              \brief The ExampleModePrivateData class stores private data used by the ExampleMode class.
             */
            struct ExampleModePrivateData;

            /*!
            \class ExampleMode
            \brief An example mode which allows the user to create tasks.
              */
            class ExampleMode : public QMainWindow, public IMode {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)
            public:
                ExampleMode(QWidget *parent = 0);
                ~ExampleMode();

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

            protected:
                void changeEvent(QEvent *e);

            private slots:
                void on_btnNewTask_clicked();
                void handleTaskCompleted();
                void on_btnShortTask_clicked();
                void on_btnLongTask_clicked();
                void on_btnDoxygen_clicked();

            private:
                QStringList convertQFileInfoList(QFileInfoList list);

                Ui::ExampleMode *ui;
                ExampleModePrivateData* d;
            };
        }
    }
}

#endif // EXAMPLE_MODE_H
