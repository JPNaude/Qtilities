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

#ifndef OBSERVER_TREE_MODEL_BUILDER
#define OBSERVER_TREE_MODEL_BUILDER

#include "QtilitiesCoreGui_global.h"
#include "ObserverTreeItem.h"

#include <ObserverHints>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct ObserverTreeModelBuilderPrivateData
        \brief Structure used by ObserverTreeModelBuilder to store private data.
          */
        struct ObserverTreeModelBuilderPrivateData;

        /*!
        \class ObserverTreeModelBuilder
        \brief The ObserverTreeModelBuilder builds models for ObserverTreeModel in a different thread.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverTreeModelBuilder : public QObject
        {
            Q_OBJECT

        public:
            ObserverTreeModelBuilder(ObserverTreeItem* item = 0, bool use_observer_hints = false, ObserverHints* observer_hints = 0, QObject *parent = 0);
            virtual ~ObserverTreeModelBuilder();

            //! Sets the root ObserverTreeitem.
            void setRootItem(ObserverTreeItem* item);
            //! Returns the task id of the tree build task.
            int taskID() const;
            //! Sets if threading is enabled in the builder.
            void setThreadingEnabled(bool is_enabled);

        public slots:
            //! Starts the build.
            void startBuild();
            //! Sets if observer hints should be used during tree building.
            void setUseObserverHints(bool use_observer_hints);
            //! Sets the active hints which should be used during tree building.
            void setActiveHints(ObserverHints* active_hints);
            void setOriginThread(QThread* thread);

        signals:
            //! Emitted when build is completed.
            void buildCompleted(ObserverTreeItem* item);

        private:
            //! Function which will rebuild the complete tree structure under the top level observer.
            void buildRecursive(ObserverTreeItem* item, QList<QPointer<QObject> > category_objects = QList<QPointer<QObject> >());
            //! Prints the structure of the tree as trace messages.
            /*!
              \sa LOG_TRACE
              */
            void printStructure(ObserverTreeItem* item = 0, int level = 0);

            ObserverTreeModelBuilderPrivateData* d;
        };
    }
}

#endif // OBSERVER_TREE_MODEL_BUILDER
