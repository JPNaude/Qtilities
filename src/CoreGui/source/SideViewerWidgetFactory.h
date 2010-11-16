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
          \struct SideViewerWidgetFactoryData
          \brief The SideViewerWidgetFactoryData class stores private data used by the SideViewerWidgetFactory class.
         */
        struct SideViewerWidgetFactoryData;

        /*!
        \class SideViewerWidgetFactory
        \brief The SideViewerWidgetFactory class is intended to act as a factory for side viewer widgets.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT SideViewerWidgetFactory : public QObject, public ISideViewerWidget {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::ISideViewerWidget)

        public:
            //! Constructs a side viewer widget helper.
            /*!
              \param interface The factory interface to the widget which must be produced.
              \param widget_id The string used to represent the widget.
              \param modes The modes in which this widget must be present.
              \param startup_modes The modes in which this widget must be visible when the application starts.
              */
            SideViewerWidgetFactory(FactoryInterface<QWidget>* interface, const QString& widget_id, QList<int> modes, QList<int> startup_modes, bool is_exclusive = false);
            ~SideViewerWidgetFactory();

            // --------------------------------------------
            // ISideViewerWidget Implementation
            // --------------------------------------------
            QWidget* produceWidget();
            bool manageWidgets() const { return true; }
            QString widgetLabel() const;
            IActionProvider* actionProvider() const;
            QList<int> destinationModes() const;
            QList<int> startupModes() const;
            bool isExclusive() const;

        private slots:
            //! Slot which detects when a widget was deleted.
            void handleWidgetDestroyed(QObject* object);

        signals:
            //! Signal which is emitted when a new widget was manufactured.
            void newWidgetCreated(QWidget* widget);

        private:
            SideViewerWidgetFactoryData* d;
        };
    }
}

#endif // SIDE_VIEWER_WIDGET_HELPER
