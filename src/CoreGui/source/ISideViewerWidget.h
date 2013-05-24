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

#ifndef ISIDEVIEWERWIDGET_H
#define ISIDEVIEWERWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IActionProvider.h"

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
            class QTILITIES_CORE_GUI_SHARED_EXPORT ISideViewerWidget : virtual public IObjectBase
            {
            public:
                ISideViewerWidget() {}
                virtual ~ISideViewerWidget() {}

                //! Function which returns the side viewer widget.
                /*!
                  Widget() must be able to produce a new widget every time it is called.
                  The Qtilities::CoreGui::SideViewerWidgetFactory class was designed to help with this process.

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
                  is set to be exclusive. When the mode(s) specified in destinationModes() is not for an exclusive viewer widget, an error message will be printed.
                  */
                virtual bool isExclusive() const = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::ISideViewerWidget,"com.Qtilities.CoreGui.ISideViewerWidget/1.0");

#endif // ISIDEVIEWERWIDGET_H
