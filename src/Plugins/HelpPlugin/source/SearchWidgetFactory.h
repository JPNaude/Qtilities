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

#ifndef SEARCHWIDGETFACTORY_H
#define SEARCHWIDGETFACTORY_H

#include <QObject>
#include <ISideViewerWidget>

#include <QHelpEngine>

namespace Ui {
    class SearchWidgetFactory;
}

namespace Qtilities {
    using namespace Qtilities::CoreGui::Interfaces;
    namespace Plugins {
        namespace Help {
            /*!
            \class SearchWidgetFactory
            \brief The SearchWidgetFactory class makes a QHelpSearchQueryWidget and QHelpSearchResultsWidget available for the \p HELP_MANAGER available as a side viewer widget.
            */
            class SearchWidgetFactory : public QObject, public ISideViewerWidget
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::ISideViewerWidget)

            public:
                SearchWidgetFactory(QHelpSearchEngine* help_search_engine, QObject *parent = 0);

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

            public slots:
                void handleSearchSignal();

            private:
                QHelpSearchEngine*  d_help_search_engine;
                QPointer<QWidget>   d_combined_widget;
            };
        }
    }
}

#endif // SEARCHWIDGETFACTORY_H
