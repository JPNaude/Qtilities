/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
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

#include <QWidget>
#include <QModelIndex>

namespace Ui {
    class CommandEditor;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \struct CommandEditorData
        \brief A structure storing private data in the CommandEditor class
          */
        struct CommandEditorData;

        /*!
        \class CommandEditor
        \brief A widget which allows the user to edit command shortcuts for actions registered in the ActionManager class.

        \todo
        - Add Qtilities icon to the message boxes displayed by this widget.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CommandEditor : public QWidget, public IConfigPage {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

        public:
            CommandEditor(QWidget *parent = 0);
            ~CommandEditor();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QStringList configPageTitle() const;
            void configPageApply();
            bool supportsApply() const { return true; }

        protected:
            void changeEvent(QEvent *e);

        public slots:
            void handleCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);
            void on_btnDefaults_clicked();
            void on_btnExport_clicked();
            void on_btnImport_clicked();
            void handleSearchStringChanged(const QString& text);
            void resizeCommandTableRows();

        private:
            Ui::CommandEditor *ui;
            CommandEditorData* d;
        };
    }
}

#endif // COMMANDEDITOR_H
