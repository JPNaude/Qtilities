/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of the Qtilities Debug Plugin.
** See Dependencies.pri for the licensing requirements of the Debug plugin.
** Alternatively, see http://www.qtilities.org/licensing.html.
**
****************************************************************************/

#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QMainWindow>

#include <IMode>
#include <Logger>

#include "Testing_global.h"

namespace Ui {
    class DebugWidget;
}

namespace Qtilities {
    namespace CoreGui {
        class Command;
    }

    namespace Testing {
        using namespace Qtilities::CoreGui;
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::Logging;
        /*!
          \struct DebugWidgetPrivateData
          \brief The DebugWidgetPrivateData struct stores private data used by the DebugWidget class.
         */
        struct DebugWidgetPrivateData;

        /*!
          \class DebugWidget
          \brief A widget providing debug information about a Qtilities application.

          For more information see the \ref page_debugging article.
         */
        class TESTING_SHARED_EXPORT DebugWidget : public QMainWindow, public IMode
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
            void on_btnProjectsCurrentOpenFile_clicked();
            void on_btnProjectsCurrentOpenPath_clicked();
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
            void refreshCommandInformation();
            void handle_objectPoolSelectionChanged(QList<QObject*> objects);
            void on_btnExplorePluginCurrentConfigSetPath_clicked();
            void on_btnEditPluginCurrentConfigSet_clicked();
            void on_btnAnalyzeCurrentObject_clicked();
            void on_btnAnalyzeAction_clicked();
            void on_chkRefreshProperties_toggled(bool checked);

            void on_btnContextUnregisterSelected_clicked();

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
            //! Refreshes the projects state of the application.
            void refreshProjectsState();
            //! Gets an object's address.
            QString objectAddress(QObject* obj) const;

            Ui::DebugWidget *ui;
            DebugWidgetPrivateData* d;
        };
    }
}

#endif // DEBUGWIDGET_H
