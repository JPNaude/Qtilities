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

#ifndef CONTENTWIDGETFACTORY_H
#define CONTENTWIDGETFACTORY_H

#include <QObject>
#include <ISideViewerWidget>

#include <QHelpEngine>

namespace Ui {
    class ContentWidgetFactory;
}

using namespace Qtilities::CoreGui::Interfaces;

namespace Qtilities {
    namespace Plugins {
        namespace Help {
            /*!
              \class ContentWidgetFactory
              \brief The ContentWidgetFactory class makes a QHelpContentWidget widget for the \p HELP_MANAGER available as a side viewer widget.
             */
            class ContentWidgetFactory : public QObject, public ISideViewerWidget
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::ISideViewerWidget)

            public:
                ContentWidgetFactory(QHelpEngine* help_engine, QObject *parent = 0);
                ~ContentWidgetFactory();

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
                QPointer<QHelpEngine> d_help_engine;
                QPointer<QWidget> d_content_widget;
            };
        }
    }
}

#endif // CONTENTWIDGETFACTORY_H
