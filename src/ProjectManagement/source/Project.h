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

#ifndef PROJECT_H
#define PROJECT_H

#include "ProjectManagement_global.h"
#include "IProject.h"

#include <Logger>

#include <QObject>

namespace Qtilities {
    namespace ProjectManagement {
        using namespace Qtilities::Logging;
        using namespace Qtilities::ProjectManagement::Interfaces;
        /*!
          \struct ProjectPrivateData
          \brief The ProjectPrivateData class stores private data used by the Project class.
         */
        struct ProjectPrivateData;

        /*!
          \class Project
          \brief The Project class provides a ready to use implementation of the IProject plugin.

          When creating a new project using the ProjectManager class it will create an instance of this class
          and set it as the current open project.
         */
        class PROJECT_MANAGEMENT_SHARED_EXPORT Project : public QObject, public IProject, public IExportable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::ProjectManagement::Interfaces::IProject)
            Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)

        public:
            Project(QObject* parent = 0);
            ~Project();

            // --------------------------------------------
            // IProject Implementation
            // --------------------------------------------
            bool newProject();
            bool loadProject(const QString& file_name, bool close_current_first = true, ITask* task = 0);
            bool saveProject(const QString& file_name, ITask* task = 0);
            bool closeProject(ITask* task = 0);
            QString projectFile() const;
            QString projectName() const;

            void setProjectItems(QList<IProjectItem*> project_items, bool inherit_modification_state = false);
            void addProjectItem(IProjectItem* project_item, bool inherit_modification_state = false);
            void removeProjectItem(IProjectItem* project_item);
            QStringList projectItemNames() const;
            int projectItemCount() const;
            IProjectItem* projectItem(int index);

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        public:
            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            IExportable::ExportResultFlags exportBinary(QDataStream& stream) const;
            IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            ProjectPrivateData* d;
        };
    }
}

#endif // PROJECT_H
