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

#ifndef CONFIGURATIONWIDGET_H
#define CONFIGURATIONWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IConfigPage.h"

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
    class ConfigurationWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct ConfigurationWidgetData
          \brief The ConfigurationWidgetData struct stores private data used by the ConfigurationWidget class.
         */
        struct ConfigurationWidgetData;

        /*!
        \class ConfigurationWidget
        \brief Configuration widget which displays config pages from widgets implementing the Qtilities::CoreGui::Interfaces::IConfigPage interface.

        Below is an example of the configuartion widget taken from the Object Management Example in the QtilitiesExamples project:

        \image html project_configuration_widget.jpg "Project Configuration Widget"
        \image latex project_configuration_widget.eps "Project Configuration Widget" width=\textwidth

          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ConfigurationWidget : public QWidget {
            Q_OBJECT

        public:
            ConfigurationWidget(DisplayMode display_mode = TableView, QWidget *parent = 0);
            ~ConfigurationWidget();

            //! Initializes the config widget.
            void initialize(QList<IConfigPage*> config_pages);
            //! Initializes the widget with a list of QObjects. All objects in the list which implements the IConfigPage interface will be added.
            /*!
              \note If the list is empty, the function will search the global object pool and automatically add all found config pages. A debug message with information about the found pages will be created.
              */
            void initialize(QList<QObject*> config_pages = QList<QObject*>());

            //! Sets the way the configuration widget handles the \p Apply button.
            /*!
              \param apply_all_pages When true, the apply button will call apply on all the configuration pages. When false, only the active page will be applied.

              Default is true;
              */
            void setApplyAllPages(bool apply_all_pages);
            //! Gets the way the configuration widget handles the \p Apply button.
            bool applyAllPages() const;

       signals:
            //! Signal emitted whenever a config page is applied.
            void appliedPage(IConfigPage* conig_page);

        public slots:
            //! Handles the close button.
            void on_btnClose_clicked();
            //! Handles the apply button.
            void on_btnApply_clicked();
            //! Handles item changes in the page tree.
            void handleActiveItemChanges(QList<QObject*> active_pages);
            //! Function to set the active page.
            void setActivePage(const QString& active_page_name);
            //! Function to get the active page name and category.
            QString activePageName() const;
            //! Function to get the reference to the IConfig interface of the active page.
            IConfigPage* activePageIFace() const;

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::ConfigurationWidget* ui;
            ConfigurationWidgetData* d;

        signals:
        };
    }
}

#endif // GLOBALPROPERTYEDITOR_H
