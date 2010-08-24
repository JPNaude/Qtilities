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

#ifndef SIDE_VIEWER_WIDGET_HELPER
#define SIDE_VIEWER_WIDGET_HELPER

#include "ISideViewerWidget.h"
#include "QtilitiesCoreGui_global.h"

#include <Factory>

#include <QWidget>
#include <QString>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct SideViewerWidgetHelperData
          \brief The SideViewerWidgetHelperData class stores private data used by the SideViewerWidgetHelper class.
         */
        struct SideViewerWidgetHelperData;

        /*!
        \class SideViewerWidgetHelper
        \brief The SideViewerWidgetHelper class is intended to act as a factory for side viewer widgets.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT SideViewerWidgetHelper : public QObject, public ISideViewerWidget {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::ISideViewerWidget)

        public:
            SideViewerWidgetHelper(FactoryInterface<QWidget>* interface, const QString& widget_id, QList<int> modes, bool show_on_startup = true);
            ~SideViewerWidgetHelper();

            // --------------------------------------------
            // ISideViewerWidget Implementation
            // --------------------------------------------
            QWidget* widget();
            QString text() const;
            bool showOnStartup() const;
            IActionProvider* actionProvider() const;
            QList<int> destinationModes() const;

        signals:
            //! Signal which is emitted when a new widget is manufactured.
            void newWidgetCreated(QWidget* widget);

        private:
            SideViewerWidgetHelperData* d;
        };
    }
}

#endif // SIDE_VIEWER_WIDGET_HELPER
