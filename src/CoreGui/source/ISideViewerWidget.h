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

#ifndef ISIDEVIEWERWIDGET_H
#define ISIDEVIEWERWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"

#include <IActionProvider.h>

#include <QWidget>
#include <QIcon>

using namespace Qtilities::CoreGui::Interfaces;

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {
            /*!
            \class ISideViewerWidget
            \brief Used by the DynamicSideViewerWidget to communicate with child widgets.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT ISideViewerWidget
            {
            public:
                ISideViewerWidget() {}
                virtual ~ISideViewerWidget() {}

                //! Function which returns the side viewer widget.
                /*!
                  Widget() must be able to produce a new widget every time it is called.
                  The Qtilities::CoreGui::SideViewerWidgetHelper class was designed to help with this process.

                  The dynamic side viewer widget will manage the lifetime of widgets returned.
                    */
                virtual QWidget* produceWidget() = 0;
                //! When true, the dynamic side viewer widget will manage the lifetime of widgets returned through produceWidget().
                virtual bool manageWidgets() const = 0;
                //! The text used to represent the widget.
                virtual QString widgetLabel() const = 0;
                //! Provides an action provider interface to provide actions to be shown in the SideWidgetWrapper class.
                virtual IActionProvider* actionProvider() const = 0;
                //! Indication of the modes (which have dynamic side viewer widgets) in which the side viewer may appear.
                virtual QList<int> destinationModes() const = 0;
                //! Indicates if the widget should be shown on startup.
                virtual QList<int> startupModes() const = 0;
                //! Indicates if the widget is exclusive.
                /*!
                  When true, this widget can only appear once inside a dynamic side widget viewer.

                  \note When true this side viewer widget can only be used with a Qtilities::CoreGui::DynamicSideWidgetViewer which
                  is set to be exclusive. When the mode specified is not for an exclusive viewer widget, an error message will be printed.
                  */
                virtual bool isExclusive() const = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::ISideViewerWidget,"com.qtilities.CoreGui.ISideViewerWidget/1.0");

#endif // ISIDEVIEWERWIDGET_H
