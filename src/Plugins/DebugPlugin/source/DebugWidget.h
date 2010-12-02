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

#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QMainWindow>
#include <IMode>

namespace Ui {
    class DebugWidget;
}

namespace Qtilities {
    namespace Plugins {
        namespace Debug {
            using namespace Qtilities::CoreGui::Interfaces;
            /*!
              \struct DebugWidgetData
              \brief The DebugWidgetData struct stores private data used by the DebugWidget class.
             */
            struct DebugWidgetData;

            /*!
              \class DebugWidget
              \brief A widget providing debug information about a Qtilities application.
             */
            class DebugWidget : public QMainWindow, public IMode
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::CoreGui::Interfaces::IMode)

            public:
                explicit DebugWidget(QWidget *parent = 0);
                ~DebugWidget();

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // IMode Implementation
                // --------------------------------------------
                QWidget* modeWidget();
                void initializeMode() {}
                QIcon modeIcon() const;
                QString modeName() const;

                //! Finalize the mode, will be called in initializeDependencies().
                void finalizeMode();

            public slots:
                //! Handles selection changes in the factory list widget.
                void handle_factoryListSelectionChanged(const QString& factory_name);
                //! Handles the factory refresh button.
                void handle_factoryListRefresh();
                //! Global object pool double click.
                void handle_objectPoolDoubleClick(QObject *object);
                //! Handles the plugin info refresh button.
                void handle_pluginInfoRefresh();
                //! Handles the mode info refresh button.
                void handle_modeInfoRefresh();

            protected:
                void changeEvent(QEvent *e);

            private:
                Ui::DebugWidget *ui;
                DebugWidgetData* d;
            };

        }
    }
}

#endif // DEBUGWIDGET_H
