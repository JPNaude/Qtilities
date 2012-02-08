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

#ifndef OBJECTINFOTREEWIDGET_H
#define OBJECTINFOTREEWIDGET_H

#include "QtilitiesCoreGui_global.h"

#include <ObserverMimeData.h>

#include <QTreeWidget>
#include <QMap>
#include <QPointer>
#include <QMouseEvent>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        /*!
        \struct qti_private_ObjectInfoTreeWidgetPrivateData
        \brief The qti_private_ObjectInfoTreeWidgetPrivateData struct stores private data used by the qti_private_ObjectInfoTreeWidget class.
          */
        struct qti_private_ObjectInfoTreeWidgetPrivateData;

        /*!
        \class qti_private_ObjectInfoTreeWidget
        \brief The qti_private_ObjectInfoTreeWidget class provides a widget which shows meta info about the objects inside the widget (properties, events, signals etc.).

        \todo
        - setHierarchyDepth() does nothing yet.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT qti_private_ObjectInfoTreeWidget : public QTreeWidget {
            Q_OBJECT
            Q_ENUMS(MetaCategory)

        public:
            qti_private_ObjectInfoTreeWidget(QWidget *parent = 0);
            virtual ~qti_private_ObjectInfoTreeWidget();

            enum MetaCategory { Event, Property, Method, Dependancy, Source };

            //! Sets the QMap which contains the object references of objects to be displayed in the tree as keys and the names by which these objects are known in the script backend as values.
            virtual void setObjectMap(QMap<QPointer<QObject>, QString> object_map);
            //! Sets the hierarchy depth to be displayed. When > 0, the tree model will check if any objects are observers and expand their children as well, with the depth of to be expanded being equal to the hierachy depth.
            void setHierarchyDepth(int depth);
            //! Enabled paste opertions on this widget. When true, paste operations will trigger the handlePasteAction() signal when a paste occur.
            void setPasteEnabled(bool enable_paste);

        protected:
            void mousePressEvent(QMouseEvent* event);
            void constructActions();
            void refreshActions();

        public slots:
            //! Handle the paste action.
            void handle_actionPaste_triggered();

        signals:
            //! Emits a signal with an object reference and tree widget item. This signal is emitted when the tree is build. Thus you can add additional categories to a tree widget item based on the object reference.
            void populateTreeItem(QObject* obj, QTreeWidgetItem* item);
            //! Emits a signal which notifies classes which inherit from qti_private_ObjectInfoTreeWidget of data pasted in this widget.
            void pasteActionOccured(const ObserverMimeData* paste_mime_data);

        private:
            void populateItem(QTreeWidgetItem* item, QObject* obj);

            static QPointer<qti_private_ObjectInfoTreeWidget> currentWidget;
            static QPointer<qti_private_ObjectInfoTreeWidget> actionContainerWidget;

            qti_private_ObjectInfoTreeWidgetPrivateData* d;
        };
    }
}

#endif // OBJECTINFOTREEWIDGET_H
