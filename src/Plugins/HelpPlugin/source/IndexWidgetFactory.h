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

#ifndef INDEXWIDGETFACTORY_H
#define INDEXWIDGETFACTORY_H

#include <QObject>
#include <ISideViewerWidget>

#include <QHelpEngine>

namespace Ui {
    class IndexWidgetFactory;
}

namespace Qtilities {
    using namespace Qtilities::CoreGui::Interfaces;
    namespace Plugins {
        namespace Help {
            /*!
              \class IndexWidgetFactory
              \brief The IndexWidgetFactory class makes a QHelpIndexWidget widget for the \p HELP_MANAGER available as a side viewer widget.
             */
            class IndexWidgetFactory : public QObject, public ISideViewerWidget
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::ISideViewerWidget)

            public:
                IndexWidgetFactory(QHelpEngine* help_engine, QObject *parent = 0);

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // ISideViewerWidget Implementation
                // --------------------------------------------
                QWidget* produceWidget();
                bool manageWidgets() const { return false; }
                QString widgetLabel() const;
                IActionProvider* actionProvider() const;
                QList<int> destinationModes() const;
                QList<int> startupModes() const;
                bool isExclusive() const;

            signals:
                //! Signal which is emitted when a new widget was manufactured.
                void newWidgetCreated(QWidget* widget);

            private:
                QPointer<QHelpEngine>   d_help_engine;
                QPointer<QWidget>       d_index_widget;
            };
        }
    }
}

#endif // INDEXWIDGETFACTORY_H
