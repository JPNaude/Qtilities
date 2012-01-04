/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
            bool closeProjectItem();

            // --------------------------------
            // IExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            InstanceFactoryInfo instanceFactoryInfo() const;
            virtual void setExportVersion(Qtilities::ExportVersion version);
            virtual IExportable::Result exportBinary(QDataStream& stream ) const;
            virtual IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);
            virtual Result exportXml(QDomDocument* doc, QDomElement* object_node) const;
            virtual Result importXml(QDomDocument* doc, QDomElement* object_node, QList<QPointer<QObject> >& import_list);

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
            // IModificationNotifier Implemenation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        public:
            // --------------------------------
            // IObjectBase Implemenation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            ObserverProjectItemWrapperPrivateData* d;
        };
    }
}

#endif // OBSERVER_PROJECT_ITEM_WRAPPER_H
