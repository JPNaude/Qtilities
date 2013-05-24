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

#ifndef PROJECTMANAGEMENTCONFIG_H
#define PROJECTMANAGEMENTCONFIG_H

#include "ProjectManagement_global.h"
#include <IConfigPage.h>

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
    class ProjectManagementConfig;
}

namespace Qtilities {
    namespace ProjectManagement {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \class ProjectManagementConfig
          \brief The ProjectManagementConfig class provides a ready to use configuration widget for the project manager.

        The ProjectManagementConfig widget exposes settings of the project manager through a configuration page shown below:

        \image html project_configuration_widget.jpg "Project Configuration Widget"

        To add this page to your application's ConfigurationWidget, do the following:
\code
// The project manager configuration page:
OBJECT_MANAGER->registerObject(PROJECT_MANAGER->configWidget());
\endcode

        \note When using the Qtilities::Plugins::ProjectManagement plugin in your application, this page will automatically appear in your configuration widget.

        For more information see the \ref page_project_management article.
         */
        class PROJECT_MANAGEMENT_SHARED_EXPORT ProjectManagementConfig : public QWidget, public IConfigPage {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

        public:
            ProjectManagementConfig(QWidget *parent = 0);
            ~ProjectManagementConfig();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QtilitiesCategory configPageCategory() const;
            QString configPageTitle() const;
            void configPageApply();
            bool supportsApply() const { return true; }
            void configPageInitialize();

        public slots:
            void handleCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
            void handleActiveCustomProjectPathChanged(QTableWidgetItem *item);
            void handle_chkOpenLastProject(bool toggle);
            void handle_chkCreateNewOnStartup(bool toggle);
            void handle_btnClearRecentProjectList();
            void handle_chkSaveModifiedProjects(bool toggle);
            void handle_radioPromptUserToSave(bool toggle);
            void handle_radioSaveAutomatically(bool toggle);
            void handle_chkUseCustomProjectsPath(bool toggle);

        protected:
            void changeEvent(QEvent *e);

        private slots:
            void on_btnRemove_clicked();
            void on_btnAdd_clicked();
            void refreshCustomProjectPaths();

        private:
            void saveCustomProjectsPaths();

            Ui::ProjectManagementConfig*    ui;
            QMap<QString,QString>           custom_paths;
            QString                         active_category;
            QTableWidgetItem*               active_category_item;
            QTableWidgetItem*               default_category_item;
        };
    }
}


#endif // PROJECTMANAGEMENTCONFIG_H
