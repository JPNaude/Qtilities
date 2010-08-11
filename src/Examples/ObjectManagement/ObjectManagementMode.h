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
        namespace ObjectManagement {
            // Object Management Mode Parameters
            #define MODE_OBJECT_MANAGEMENT_ID                   999
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

                    // --------------------------------------------
                    // IMode Implementation
                    // --------------------------------------------
                    QWidget* widget();
                    void initialize();
                    QIcon icon() const;
                    QString text() const;
                    QString contextString() const { return CONTEXT_OBJECT_MANAGEMENT_MODE; }
                    QString contextHelpId() const { return QString(); }
                    int modeID() const { return MODE_OBJECT_MANAGEMENT_ID; }

                private:
                    ObjectManagementModeData* d;
            };
        }
    }
}

#endif // OBJECT_MANAGEMENT_MODE_H
