/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#ifndef DYNAMICWIDGETVIEWER_H
#define DYNAMICWIDGETVIEWER_H

#include "ISideViewerWidget.h"
#include "QtilitiesCoreGui_global.h"

#include <QWidget>
#include <QMap>
#include <QString>

namespace Ui {
    class DynamicSideWidgetViewer;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct DynamicSideWidgetViewerData
          \brief The DynamicSideWidgetViewerData class stores private data used by the DynamicSideWidgetViewer class.
         */
        struct DynamicSideWidgetViewerData;

        /*!
        \class DynamicSideWidgetViewer
        \brief The widget which can display dynamic side widgets (widgets implementing the ISideViewerWidget interface).
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT DynamicSideWidgetViewer : public QWidget {
            Q_OBJECT
        public:
            DynamicSideWidgetViewer(int mode_destination, QWidget *parent = 0);
            ~DynamicSideWidgetViewer();

            //! Adds a widget to the bottom of the dynamic viewer.
            /*!
              This functions gets a map with name-interface pairs of all the side viewer widgets found in the global
              object pool. The function will automatically filter the widgets depending on the destination mode assigned
              to this widget in its constructor.

              \param text_iface_map A map with the name-interface pairs of side viewer widgets in the global object pool.
              \param is_exclusive When true, the side viewer widgets are exclusive which means that they can only appear once. If a widget is
              already active, it will not appear in the combo box to be switched to again.
              */
            void setIFaceMap(QMap<QString, ISideViewerWidget*> text_iface_map, bool is_exclusive = false);
            //! Indicates if this widget handles dynamic widgets in an exclusive way.
            /*!
              \sa setIFaceMap();
              */
            bool isExclusive() const;

        public slots:
            //! Handles the deletion of side widgets.
            void handleSideWidgetDestroyed(QWidget* widget);
            //! Handles requests for new side widgets.
            void handleNewSideWidgetRequest();
        private slots:
            //! Function which updates the items in all the combo boxes of active wrappers in this side widget viewer.
            /*!
              \param exclude_text When set, all boxes except the box with the exclude text will be updated.
              */
            void updateWrapperComboBoxes(const QString& exclude_text = QString());

        signals:
            //! Signal indicating that this side widget viewer must be hidden.
            void toggleVisibility(bool toggle);

        protected:
            void changeEvent(QEvent *e);

        private:
            //! Gets a QStringList of all active wrapper names.
            QStringList activeWrapperNames() const;

            Ui::DynamicSideWidgetViewer *ui;
            DynamicSideWidgetViewerData* d;
        };
    }
}

#endif // DYNAMICWIDGETVIEWER_H
