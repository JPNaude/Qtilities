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
          \struct SideViewerWidgetFactoryPrivateData
          \brief The SideViewerWidgetFactoryPrivateData class stores private data used by the SideViewerWidgetFactory class.
         */
        struct SideViewerWidgetFactoryPrivateData;

        /*!
        \class SideViewerWidgetFactory
        \brief The SideViewerWidgetFactory class is intended to act as a factory for side viewer widgets.

        The SideViewerWidgetFactory class allows you to add any widget which implements Qtilities::Core::Interfaces::IFactoryInterface to a Qtilities::CoreGui::DynamicSideWidgetViewer without needing to implement Qtilities::Core::ISideViewerWidget yourself.

        Every time the dyanmic side widget viewer request a new widget of your type, the factory will produce such a widget and manage its lifetime for you. For example:

\code
DynamicSideWidgetViewer* sideWidget = new DynamicSideWidgetViewer(101);

SideViewerWidgetFactory* fac1 = new SideViewerWidgetFactory(&SideWidgetFileSystem::factory, "File System", QList<int>()<<101, QList<int>()<<101);
SideViewerWidgetFactory* fac2 = new SideViewerWidgetFactory(&ObjectScopeWidget::factory, "Object Scope", QList<int>()<<101, QList<int>()<<101);

QMap<QString, ISideViewerWidget*> map;
map[fac1->widgetLabel()] = fac1;
map[fac2->widgetLabel()] = fac2;

sideWidget->setIFaceMap(map, true);
\endcode

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
            SideViewerWidgetFactory(FactoryInterface<QWidget>* factory_interface, const QString& widget_id, QList<int> modes, QList<int> startup_modes, bool is_exclusive = false);
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

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private slots:
            //! Slot which detects when a widget was deleted.
            void handleWidgetDestroyed(QObject* object);

        signals:
            //! Signal which is emitted when a new widget was manufactured.
            void newWidgetCreated(QWidget* widget, const QString& widget_label);

        private:
            SideViewerWidgetFactoryPrivateData* d;
        };
    }
}

#endif // SIDE_VIEWER_WIDGET_HELPER
