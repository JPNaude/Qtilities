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

#ifndef IPROJECT_H
#define IPROJECT_H

#include "ProjectManagement_global.h"
#include "IProjectItem.h"

#include <IModificationNotifier>
#include <ITask>

#include <QStringList>

namespace Qtilities {
    namespace ProjectManagement {
        //! Namespace containing available interfaces which forms part of the ProjectManagement Module.
        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class IProject
            \brief Interface through which the project manager communicates with projects.
              */
            class PROJECT_MANAGEMENT_SHARED_EXPORT IProject : virtual public IModificationNotifier
            {

            public:
                IProject() {}
                virtual ~IProject() {}

                //! Called by the project manager when a new project needs to be created.
                virtual bool newProject() = 0;
                //! Load the project from the specified file.
                /*!
                  \param close_current_first In some cases it is not needed to close the current project first. An example of this is when you just created a new project and now load an existing project file into the project.
                  */
                virtual bool loadProject(const QString& file_name, bool close_current_first = true, ITask* task = 0) = 0;
                //! Save the project to a specified file.
                virtual bool saveProject(const QString& file_name, ITask* task = 0) = 0;
                //! Close the project.
                virtual bool closeProject(ITask* task = 0) = 0;
                //! Project file name. Must return an empty string when no project file is associated with the project at present.
                virtual QString projectFile() const = 0;
                //! Project name.
                virtual QString projectName() const = 0;           

                //! Sets the list of project items for this project.
                /*!
                  \param inherit_modification_state When true, the project will inherit the modification state of the new parts. Else the items will inherit the state of the project.
                  */
                virtual void setProjectItems(QList<IProjectItem*> project_items, bool inherit_modification_state = false) = 0;
                //! Adds a project item to this project.
                virtual void addProjectItem(IProjectItem* project_item, bool inherit_modification_state = false) = 0;
                //! Removes a project item to this project.
                virtual void removeProjectItem(IProjectItem* project_item) = 0;
                //! Gives a list of all the project items in the project.
                virtual QStringList projectItemNames() const = 0;
                //! Gives a the number of project items contained in this project.
                virtual int projectItemCount() const = 0;
                //! Gives a reference to the project item at the given index.
                virtual IProjectItem* projectItem(int index) = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::ProjectManagement::Interfaces::IProject,"com.ProjectManagement.IProject/1.0");

#endif // IPROJECT_H
