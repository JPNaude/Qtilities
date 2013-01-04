/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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
