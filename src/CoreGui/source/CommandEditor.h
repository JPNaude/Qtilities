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
using namespace Qtilities::CoreGui::Interfaces;

namespace Qtilities {
    namespace CoreGui {
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

        To add this page to your application's ConfigurationWidget, do the following:
\code
// The shortcuts editor for commands in your application:
OBJECT_MANAGER->registerObject(ACTION_MANAGER->commandEditor());
\endcode

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
            /*!
              By default the settings are saved to:
\code
QString shortcut_mapping_file = QString("%1%3%2").arg(QtilitiesApplication::applicationSessionPath()).arg(qti_def_PATH_SHORTCUTS_FILE).arg(QDir::separator());
\endcode
              */
            void configPageApply();
            bool supportsApply() const { return true; }
            void configPageInitialize();

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
