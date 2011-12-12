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

#ifndef LOGGERCONFIGWIDGET_H
#define LOGGERCONFIGWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "IConfigPage.h"

#include <Observer.h>

#include <QWidget>
#include <QModelIndex>

class QListWidgetItem;

namespace Ui {
    class LoggerConfigWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \struct LoggerConfigWidgetPrivateData
          \brief The LoggerConfigWidgetPrivateData struct stores private data in LoggerConfigWidget.
          */
        struct LoggerConfigWidgetPrivateData;

        /*!
          \class LoggerConfigWidget
          \brief The LoggerConfigWidget class provides a widget allowing the user to control the logger through a visual interface.

          The LoggerConfigWidget widget exposes settings of the logger through a configuration page shown below:

        \image html logging_configuration_widget.jpg "Logging Configuration Widget"

        To add this page to your application's ConfigurationWidget, do the following:
\code
// Logging configuration page:
OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget());
\endcode

        \note When using the Qtilities::Plugins::SessionLog plugin in your application, this page will automatically appear in your configuration widget.

        For more information see the \ref page_logging article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT LoggerConfigWidget : public QWidget, public IConfigPage {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

        public:
            LoggerConfigWidget(bool applyButtonVisisble = true, QWidget *parent = 0);
            ~LoggerConfigWidget();

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

            //! Function to hide/show the "Apply" button.
            /*!
              When embedding this config widget in an application configuration dialog which already has an apply button, it might be required to hide the apply button. The button is visible by default.
              */
            void setApplyButtonVisible(bool visible);

        protected:
            void changeEvent(QEvent *e);

        private slots:
            void handle_NewLoggerEngineRequest();
            void handle_RemoveLoggerEngineRequest();
            void handle_LoggerEngineTableClicked(const QModelIndex& index);
            void handle_FormattingEnginesCurrentRowChanged(int currentRow);
            void handle_ComboBoxLoggerFormattingEngineCurrentIndexChange(int index);
            void handle_CheckBoxToggleAllClicked(bool checked);
            void handle_CheckBoxRememberSessionConfigClicked(bool checked);
            void handle_ComboBoxGlobalLogLevelCurrentIndexChange(const QString& text);
            void handle_BtnSaveConfigClicked();
            void handle_BtnLoadConfigClicked();
            void handle_BtnApplyClicked();
            void resizeCommandTableRows();
            void on_btnViewLog_clicked();

        private:
            void refreshLoggerEngineInformation();

            void writeSettings();
            void readSettings();

            Ui::LoggerConfigWidget *ui;
            LoggerConfigWidgetPrivateData* d;

            static LoggerConfigWidget* config_widget;
        };
    }
}

#endif // LOGGERCONFIGWIDGET_H
