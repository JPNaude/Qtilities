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

#ifndef OBSERVER_PROJECT_ITEM_WRAPPER_H
#define OBSERVER_PROJECT_ITEM_WRAPPER_H

#include "IProjectItem.h"
#include "ProjectManagement_global.h"

#include <QObject>

#include <Observer>

namespace Qtilities {
    namespace ProjectManagement {
        using namespace Qtilities::ProjectManagement::Interfaces;
        using namespace Qtilities::Core;

        /*!
          \struct ObserverProjectItemWrapperPrivateData
          \brief The ObserverProjectItemWrapperPrivateData struct stores private data used by the ObserverProjectItemWrapper class.
         */
        struct ObserverProjectItemWrapperPrivateData;

        /*!
        \class ObserverProjectItemWrapper
        \brief This class wraps Qtilities::Core::Observer as a project item.

        Using ObserverProjectItemWrapper you can easily make any Qtilities::Core::Observer part of a project. For example:

\code
Observer* obs = new Observer;
ObserverProjectItemWrapper* project_item = new ObserverProjectItemWrapper(obs);
OBJECT_MANAGER->registerObject(project_item,QtilitiesCategory("Core::Project Items (IProjectItem)","::"));
\endcode

        Make sure you register the project item in the global object pool before initializing the project manager.
          */
        class PROJECT_MANAGEMENT_SHARED_EXPORT ObserverProjectItemWrapper : public QObject, public IProjectItem
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::ProjectManagement::Interfaces::IProjectItem)

        public:
            explicit ObserverProjectItemWrapper(Observer* observer = 0, QObject *parent = 0);

            //! Sets the observer context for this project item.
            void setObserverContext(Observer* observer);

            // --------------------------------------------
            // IProjectItem Implementation
            // --------------------------------------------
            QString projectItemName() const;
            bool newProjectItem();
            bool closeProjectItem(ITask* task = 0);

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            virtual void setExportVersion(Qtilities::ExportVersion version);
            virtual void setExportTask(ITask* task);
            virtual void clearExportTask();
            virtual IExportable::ExportResultFlags exportBinary(QDataStream& stream ) const;
            virtual IExportable::ExportResultFlags importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            virtual IExportable::ExportResultFlags exportXml(QDomDocument* doc, QDomElement* object_node) const;
            virtual IExportable::ExportResultFlags importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

            //! Sets the export item flags to be used for this project item.
            /*!
              \sa exportItemFlags()
              */
            void setExportItemFlags(ObserverData::ExportItemFlags flags);
            //! Get the export item flags to be used for this project item.
            /*!
              The default ObserverData::ExportData.

              \sa setExportItemFlags()
              */
            ObserverData::ExportItemFlags exportItemFlags() const;

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
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            ObserverProjectItemWrapperPrivateData* d;
        };
    }
}

#endif // OBSERVER_PROJECT_ITEM_WRAPPER_H
