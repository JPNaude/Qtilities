/****************************************************************************
**
** Copyright 2009, Jaco Naude
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

        \note This class is still under development and is not yet ready for production usage.
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
            QStringList configPageTitle() const;
            void configPageApply();
            bool supportsApply() const { return true; }

        private:
            Ui::CodeEditorWidgetConfig *ui;
        };
    }
}

#endif // CODEEDITORWIDGETCONFIGPAGE_H
