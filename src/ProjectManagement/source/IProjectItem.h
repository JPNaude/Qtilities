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
