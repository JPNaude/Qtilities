/****************************************************************************
**
** Copyright 2009-2013, Jaco Naudé
**
** This library is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License.
**  
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this library.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CODEEDITORWIDGETCONFIGPAGE_H
#define CODEEDITORWIDGETCONFIGPAGE_H

#include "QtilitiesCoreGui_global.h"
#include "IConfigPage.h"

#include <QWidget>

namespace Ui {
    class CodeEditorWidgetConfig;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \class CodeEditorWidgetConfig
        \brief A config page for text editors.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CodeEditorWidgetConfig : public QWidget, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

        public:
            CodeEditorWidgetConfig(QWidget* parent = 0, Qt::WindowFlags f = 0);
            ~CodeEditorWidgetConfig();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QString configPageTitle() const;
            QtilitiesCategory configPageCategory() const;
            /*!
              This function calls QtilitiesApplication::newSettingsUpdateRequest("AllCodeEditors") which will refresh all code editors.

              For more information see \ref configuration_widget_storage_layout.
              */
            void configPageApply();
            bool supportsApply() const { return true; }
            void configPageInitialize();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

        private:
            Ui::CodeEditorWidgetConfig *ui;
        };
    }
}

#endif // CODEEDITORWIDGETCONFIGPAGE_H
