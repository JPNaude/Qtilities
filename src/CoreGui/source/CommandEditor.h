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

#ifndef COMMANDEDITOR_H
#define COMMANDEDITOR_H

#include "QtilitiesCoreGui_global.h"
#include "IConfigPage.h"
#include "ObserverWidget.h"

#include <QWidget>
#include <QModelIndex>
#include <QLineEdit>

namespace Ui {
    class CommandEditor;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        class Command;

        /*!
        \struct CommandEditorPrivateData
        \brief A structure storing private data in the CommandEditor class
          */
        struct CommandEditorPrivateData;

        /*!
        \class CommandEditor
        \brief A widget which allows the user to edit command shortcuts for actions registered in the action manager class.

        The command editor widget gives a overview of all the commands registered in the application and their shortcuts. It allows shortcuts to be edited and allows you to save, load and restore default configurations.

        \image html command_editor.jpg "Command Editor"

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CommandEditor : public QWidget, public IConfigPage {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

        public:
            CommandEditor(QWidget *parent = 0);
            ~CommandEditor();

            //! Returns the observer widget used to display commands.
            /*!
              Through the observer widget you can get access to the selected objects and be notified when the selection changes.
              */
            ObserverWidget* commandWidget() const;

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

        protected:
            void changeEvent(QEvent *e);

        public slots:
            void restoreDefaults();
            void exportConfiguration();
            void importConfiguration();

        private:
            Ui::CommandEditor *ui;
            CommandEditorPrivateData* d;
        };
    }
}

#endif // COMMANDEDITOR_H
