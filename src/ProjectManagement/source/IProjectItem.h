/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#ifndef IPROJECTITEM_H
#define IPROJECTITEM_H

#include "ProjectManagement_global.h"

#include <IModificationNotifier>
#include <IExportable>
#include <ITask>

#include <QObject>
#include <QString>

namespace Qtilities {
    namespace ProjectManagement {
        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;
            /*!
            \class IProjectItem
            \brief Interface through which objects can be exposed as project parts.

            Project loading and saving is done through the implementation of IExportable.
              */
            class PROJECT_MANAGEMENT_SHARED_EXPORT IProjectItem : virtual public IModificationNotifier, virtual public IExportable {
            public:
                IProjectItem() {}
                virtual ~IProjectItem() {}

                //! The name of the project item.
                virtual QString projectItemName() const = 0;
                //! Called by the project manager when a new project needs to be created.
                virtual bool newProjectItem() = 0;
                //! Close the project item.
                virtual bool closeProjectItem(ITask *task = 0) = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::ProjectManagement::Interfaces::IProjectItem,"com.Qtilities.ProjectManagement.IProjectItem/1.0");

#endif // IPROJECTITEM_H
