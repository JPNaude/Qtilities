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
