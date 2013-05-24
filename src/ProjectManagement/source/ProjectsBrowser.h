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
#ifndef PROJECTS_BROWSER_H
#define PROJECTS_BROWSER_H

#include "ProjectManagement_global.h"

#include <SideWidgetFileSystem>

#include <QWidget>

namespace Ui {
    class ProjectsBrowser;
}

namespace Qtilities {
    namespace ProjectManagement {
        using namespace Qtilities::CoreGui;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \class ProjectsBrowser
          \brief The ProjectsBrowser class provides a ready to use widget which allows browsing of projects.

          <i>This class was added in %Qtilities v1.1.</i>
         */
        class PROJECT_MANAGEMENT_SHARED_EXPORT ProjectsBrowser : public QWidget {
            Q_OBJECT

        public:
            ProjectsBrowser(QWidget *parent = 0);
            ~ProjectsBrowser();

        private slots:
            void refreshContents();
            void on_btnClearRecent_clicked();
            void on_buttonBox_accepted();
            void on_listCustomCategories_itemClicked(QListWidgetItem *item);
            void on_listWidgetRecent_itemDoubleClicked(QListWidgetItem *item);
            void on_buttonBox_rejected();
            void on_btnRemoveNonExisting_clicked();
            void openProjectAtPath(const QString& project_path);

        private:
            Ui::ProjectsBrowser*    ui;
            SideWidgetFileSystem*   file_system_browser;
        };
    }
}


#endif // PROJECTS_BROWSER_H
