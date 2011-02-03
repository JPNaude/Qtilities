/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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
#include <Logger>

namespace Ui {
    class DebugWidget;
}

namespace Qtilities {
    namespace CoreGui {
        class Command;
    }
    namespace Plugins {
        namespace Debug {
            using namespace Qtilities::CoreGui;
            using namespace Qtilities::CoreGui::Interfaces;
            using namespace Qtilities::Logging;
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
                void aboutToBeActivated();

                //! Finalize the mode, will be called in initializeDependencies().
                void finalizeMode();

            public slots:
                //! Handles selection changes in the factory list widget.
                void handle_factoryListSelectionChanged(const QString& factory_name);
                //! Global object pool double click.
                void handle_objectPoolDoubleClick(QObject *object);

            protected:
                void changeEvent(QEvent *e);

            private slots:
                void on_btnRefreshViews_clicked();
                void on_btnExplorePluginConfigSetPath_clicked();
                bool on_btnSavePluginConfigSet_clicked();
                bool on_btnSaveNewPluginConfigSet_clicked();
                void on_btnEditPluginConfigSet_clicked();
                void on_btnRemoveActivePlugin_clicked();
                void on_btnAddActivePlugin_clicked();
                void on_btnRemoveInactivePlugin_clicked();
                void on_btnAddInactivePlugin_clicked();
                void on_btnRemoveFilterExpression_clicked();
                void on_btnAddFilterExpression_clicked();
                void handleListMessage(Logger::MessageType type, const QString& msg);
                void on_btnOpenPluginConfigSet_clicked();
                void on_btnContextSetActive_clicked();
                void on_btnContextsClear_clicked();
                void on_btnContextsBroadcast_clicked();
                //! Refreshes the command info table with information about the given command.
                void refreshCommandInformation(Command* command);
                void handle_objectPoolSelectionChanged(QList<QObject*> objects);

                void on_btnExplorePluginCurrentConfigSetPath_clicked();

                void on_btnEditPluginCurrentConfigSet_clicked();

            private:
                //! Refreshes the mode information.
                void refreshModes();
                //! Refreshes the contexts information.
                void refreshContexts();
                //! Refreshes the current plugin state of the application.
                void refreshCurrentPluginState();
                //! Refreshes the current plugin set of the application.
                void refreshCurrentPluginSet();
                //! Refreshes the edited plugin state of the application.
                void refreshEditedPluginState();
                //! Refreshes the application's factory information.
                void refreshFactories();

                Ui::DebugWidget *ui;
                DebugWidgetData* d;
            };

        }
    }
}

#endif // DEBUGWIDGET_H
