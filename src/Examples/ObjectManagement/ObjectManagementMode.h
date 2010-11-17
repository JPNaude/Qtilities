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

#ifndef OBJECT_MANAGEMENT_MODE_H
#define OBJECT_MANAGEMENT_MODE_H

#include <IMode.h>

#include <QObject>

namespace Qtilities {
    namespace Examples {
        //! Namespace containing all the classes which forms part of the Object Management Example.
        /*!
            This example allows the user to create new objects and build up a tree visually using a
            Qtilities::CoreGui::ObserverWidget widget as the frontend to a Qtilities::Core::Observer class.

            The example uses all the %Qtilities libraries and showcases many of the concepts and features found in the different modules, more specifically:
            - Uses the %Qtilities main window architecture and shows how to add custom modes to the main window.
            - Uses different plugins alongside the Extension System module.
            - Demonstrates project management where the project item is the top level observer which the user can build a tree structure under.
            - Demonstrates logging capabilities of the Logging module, including priority logging.
            - Demonstrates action management and the configuration widget.
            - Demonstrates the full use of observer hints and using an observer widget to manage an observer context.
            - Loads the %Qtilities debugging plugin which provides an overview of the internals of the %Qtilities managers during runtime.

            This example uses the following modules and plugins:
            - <a class="el" href="namespace_qtilities_1_1_logging.html">Logging Module</a>
            - <a class="el" href="namespace_qtilities_1_1_core.html">Core Module</a>
            - <a class="el" href="namespace_qtilities_1_1_core_gui.html">CoreGui Module</a>
            - <a class="el" href="namespace_qtilities_1_1_project_management.html">Project Management Module</a>
            - <a class="el" href="namespace_qtilities_1_1_extension_system.html">Extension System Module</a>
            - <a class="el" href="namespace_qtilities_1_1_plugins_1_1_session_log.html">Session Log Plugin</a>
            - <a class="el" href="namespace_qtilities_1_1_plugins_1_1_project_management.html">Project Management Plugin</a>
            - <a class="el" href="namespace_qtilities_1_1_plugins_1_1_debug.html">Debug Plugin</a>

            Below is a screenshot of this example in action:
            \image html example_object_management.jpg "Object Management Example"
          */
        namespace ObjectManagement {
            // Object Management Mode Parameters
            const char * const CONTEXT_OBJECT_MANAGEMENT_MODE   = "Context.ObjectManagementMode";

            using namespace Qtilities::CoreGui::Interfaces;

            /*!
              \struct ObjectManagementModeData
              \brief The ObjectManagementModeData struct stores private data used by the ObjectManagementMode class.
             */
            struct ObjectManagementModeData;

            /*!
            \class ObjectManagementMode
            \brief A mode which allows you to manage the objects in an observer.
              */
            class ObjectManagementMode : public QObject, public IMode
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)

                public:
                    ObjectManagementMode(QObject* parent = 0);
                    ~ObjectManagementMode();

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
                    QString contextString() const { return CONTEXT_OBJECT_MANAGEMENT_MODE; }

                private:
                    ObjectManagementModeData* d;
            };
        }
    }
}

#endif // OBJECT_MANAGEMENT_MODE_H
