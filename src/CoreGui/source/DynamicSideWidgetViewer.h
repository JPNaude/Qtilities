/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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
            void setIFaceMap(QMap<QString, ISideViewerWidget*> text_iface_map, QWidget* static_top_widget = 0);

        public slots:
            //! Handles the deletion of side widgets
            void handleSideWidgetDestroyed(QObject* obj);
            //! Handles requests for new side widgets.
            void handleNewSideWidgetRequest();

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::DynamicSideWidgetViewer *ui;
            DynamicSideWidgetViewerData* d;
        };
    }
}

#endif // DYNAMICWIDGETVIEWER_H
