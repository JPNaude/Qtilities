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

        public slots:
            //! Starts the build.
            void startBuild();
            //! Sets if observer hints should be used during tree building.
            void setUseObserverHints(bool use_observer_hints);
            //! Sets the active hints which should be used during tree building.
            void setActiveHints(ObserverHints* active_hints);

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
