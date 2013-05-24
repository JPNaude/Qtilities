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

#ifndef CODE_EDITOR_WIDGET_H
#define CODE_EDITOR_WIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "CodeEditor.h"
#include "SearchBoxWidget.h"
#include "IActionProvider.h"

#include <IContext>
#include <IModificationNotifier>

#include <QWidget>
#include <QPointer>
#include <QSyntaxHighlighter>
#include <QtGui>
#include <QPrinter>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

namespace Ui {
    class CodeEditorWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::CoreGui::Constants;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \struct CodeEditorWidgetPrivateData
        \brief A structure storing private data in the CodeEditorWidget class.
          */
        struct CodeEditorWidgetPrivateData;

        /*!
        \class CodeEditorWidget
        \brief A code editor widget which contains a CodeEditor.

        The CodeEditorWidget class is a ready to use text editor widget. It is a very convenient widget to use if you just want to
        display simple files to a user. For example:

\code
CodeEditorWidget code_editor(CodeEditorWidget::ActionNoHints,CodeEditorWidget::Editor | CodeEditorWidget::SearchBox);
code_editor.loadFile(QApplication::applicationDirPath() + "/changes.txt");
code_editor.codeEditor()->setReadOnly(true);
\endcode

        The screenshot below shows the CodeEditorWidget in the \p MainWindowExample in the \p QtilitiesExamples project.

        \image html example_main_window.jpg "Example Of Main Window Architecture"

        The CodeEditorWidget contains a search & replace box as well as a range of actions which is registered in the action manager
        and available through the Qtilities::CoreGui::Interfaces::IActionProvider interface. For example we can provide the user with toolbar actions to save a file
        and to load a file like this:

\code
CodeEditorWidget code_editor(CodeEditorWidget::ActionSaveFile | CodeEditorWidget::ActionOpenFile);
code_editor.loadFile(QApplication::applicationDirPath() + "/changes.txt");
code_editor.codeEditor()->setReadOnly(true);
\endcode

        A list of available actions are defined in ActionFlag and the widget can further be customized using DisplayFlag. Since the \p IActionProvider interface is implemented you can easily add new actions to the toolbars. See \ref action_providers for more information.

        The CodeEditorWidget provides a file loading interface. Through this interface you can load a file into the
        editor using the loadFile() function. The contents of the editor can be saved to a file using the
        saveFile() function. When the contents in the editor was loaded from a file through the loadFile() method,
        you can call saveFile() without any parameters to save the content to the file from which it was loaded.
        The fileName() function returns the name of this file if it was set.

        Any CodeEditorWidget can easily be made part of a project using Qtilities::ProjectManagement::CodeEditorProjectItemWrapper.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CodeEditorWidget : public QMainWindow, public IContext, public IModificationNotifier
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
            // --------------------------------
            // Display Flags Functionality
            // --------------------------------
            //! The possible actions that can be constructed in a code editor.
            /*!
              \sa actionFlagsHint()
              */
            enum ActionFlag {
                ActionNoHints       = 1 << 0, /*!< No actions hint. */
                ActionNew           = 1 << 1, /*!< Action to create a new document in the editor. This will clear the current file information. \sa fileName() */
                ActionOpenFile      = 1 << 2, /*!< Action to open a file in the editor. */
                ActionSaveFile      = 1 << 3, /*!< Action to save the editor content to a file. */
                ActionSaveFileAs    = 1 << 4, /*!< Action to prompt for a file name to save the editor content to. */
                ActionPrint         = 1 << 5, /*!< Action to print the editor content. */
                ActionPrintPreview  = 1 << 6, /*!< Action to display a print preview of the editor content. */
                ActionPrintPDF      = 1 << 7, /*!< Action to print the editor content to a PDF document. */
                ActionUndo          = 1 << 8, /*!< Action to perform an undo operation. */
                ActionRedo          = 1 << 9, /*!< Action to perform a redo operation. */
                ActionCut           = 1 << 10, /*!< Action to perform a cut operation. */
                ActionCopy          = 1 << 11, /*!< Action to perform a copy operation. */
                ActionPaste         = 1 << 12, /*!< Action to perform a paste operation. */
                ActionClear         = 1 << 13, /*!< Action to clear the content of the editor. */
                ActionSelectAll     = 1 << 14, /*!< Action to select all content in the editor. */
                ActionFind          = 1 << 15, /*!< Action to bring up the search and replace box. */
                ActionAllHints = ActionNoHints | ActionOpenFile | ActionSaveFile | ActionSaveFileAs | ActionPrint | ActionPrintPreview | ActionPrintPDF | ActionUndo | ActionRedo | ActionCut | ActionCopy | ActionPaste |ActionClear | ActionSelectAll | ActionFind
            };
            Q_DECLARE_FLAGS(ActionFlags, ActionFlag)
            Q_FLAGS(ActionFlags)
            //! The possible display flags of the code editor widget.
            /*!
              \sa displayFlagsHint()
              */
            enum DisplayFlag {
                NoDisplayFlagsHint  = 1 << 1, /*!< No display flags hint. Uses Editor by default. */
                Editor              = 1 << 1, /*!< Display the code editor. */
                SearchBox           = 1 << 2, /*!< Display the search box widget. */
                ActionToolBar       = 1 << 3, /*!< Display an action toolbar called "Editing" in the widget. */
                AllDisplayFlagHint = Editor | SearchBox | ActionToolBar
            };
            Q_DECLARE_FLAGS(DisplayFlags, DisplayFlag)
            Q_FLAGS(DisplayFlags)

            //! Defines how the editor should handle a situation where the open file is removed outside of the editor.
            enum FileRemovedOutsideHandlingPolicy {
                CloseFile      = 0,      /*!< Just close the file in the editor but keep the editor open. */
                DeleteEditor   = 1       /*!< Call deleteLater() on the editor. */
            };
            //! Defines how the editor should handle a situation where the open file is modified outside of the editor.
            enum FileModifiedOutsideHandlingPolicy {
                PromptForUpdate = 0,      /*!< Prompt the user for the action to take. */
                AutoUpdate      = 1,      /*!< Automatically update the editor. */
                AutoIgnore      = 2,      /*!< Ignore the external modifications. */
                AutoIgnoreSetModified = 3 /*!< Ignore the external modifications, but set the editor as modified since the contents is different from that on disk. */
            };

            // --------------------------------
            // Construction / Destruction
            // --------------------------------
            /*!
              \param display_flags The display flags which must be used for this code editor.
              */
            CodeEditorWidget(ActionFlags action_flags = ActionAllHints,
                             DisplayFlags display_flags = AllDisplayFlagHint,
                             QWidget* parent = 0);
            virtual ~CodeEditorWidget();
            bool eventFilter(QObject *object, QEvent *event);

            // --------------------------------
            // Init, Hints etc.
            // --------------------------------
            //! Gets the FileRemovedOutsideHandlingPolicy of the editor widget.
            /*!
              Default is CloseFile.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            FileRemovedOutsideHandlingPolicy fileRemovedOutsideHandlingPolicy() const;
            //! Sets the FileRemovedOutsideHandlingPolicy of the editor widget.
            /*!
              Default is CloseFile.

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setFileRemovedOutsideHandlingPolicy(FileRemovedOutsideHandlingPolicy policy);
            //! Gets the FileModifiedOutsideHandlingPolicy of the editor widget.
            /*!
              Default is PromptForUpdate.

              <i>This function was added in %Qtilities v1.2.</i>
              */
            FileModifiedOutsideHandlingPolicy fileModifiedOutsideHandlingPolicy() const;
            //! Sets the FileModifiedOutsideHandlingPolicy of the editor widget.
            /*!
              Default is PromptForUpdate.

              <i>This function was added in %Qtilities v1.2.</i>
              */
            void setFileModifiedOutsideHandlingPolicy(FileModifiedOutsideHandlingPolicy policy);

            //! Gets the  current display flags hint.
            /*!
              \sa setDisplayFlagsHint()
              */
            DisplayFlags displayFlagsHint() const;
            //! Sets the current display flags hints.
            /*!
              \note This function will call initialize for you to update your display.

              \sa displayFlagsHint()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setDisplayFlagsHint(DisplayFlags display_flags);
            //! Gets the action display flags hint.
            /*!
              \sa setActionFlagsHint()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            ActionFlags actionFlagsHint() const;
            //! Sets the current action flags hints.
            /*!
              \note This function will call initialize for you to update your display.

              \sa actionFlagsHint()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setActionFlagsHint(ActionFlags action_flags);
            //! Initializes the window according to the current flags.
            /*!
              \sa displayFlagsHint(), actionFlagsHint()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void initialize();

            // --------------------------------
            // IContext Implementation
            // --------------------------------
            QString contextString() const;
            QString contextHelpId() const;

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            bool isModified() const;
        public slots:
            void setModificationState(bool new_state, IModificationNotifier::NotificationTargets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        public:
            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // Actions & Related Functionality
            // --------------------------------
            //! Returns the action handler interface for this code editor widget.
            IActionProvider* actionProvider();

        public slots:
            void actionNew();
            void actionOpen();
            bool actionSave();
            bool actionSaveAs();
            void actionPrint();
            void actionPrintPreview();
            void actionPrintPdf();
            void showEditorSettings();
            void showSearchBox();
            void printPreview(QPrinter *printer);
            void handleSettingsUpdateRequest(const QString& request_id);

        private slots:
            void updateSaveAction();
            void handleFileChangedNotification(const QString& path);

        protected:
            void constructActions();
            void refreshActions();

        signals:
            //! Signal which is emitted as soon as the file shown in the editor changed.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>
              */
            void fileNameChanged(const QString& new_file_name);

        public:
            // --------------------------------
            // File Loading Interface
            // --------------------------------
            //! Loads a file into the editor.
            /*!
              \returns True if the file was loaded successfully, false otherwise.
              */
            bool loadFile(const QString& file_name);
            //! Closes the current file being edited, if any.
            /*!
               <i>This function was added in %Qtilities v1.1.</i>
              */
            void closeFile();
            //! Saves the content of the editor to a file.
            /*!
              \param file_name The file name to which the contents must be saved. If loadFile() was already called, the file name does not need to be specified.
              */
            bool saveFile(QString file_name = QString());
            //! The file name linked to the contents of the code editor.
            /*!
              This name will only be valid if loadFile() has been called.
              */
            QString fileName() const;
            //! Sets the default path used by the code editor widget when using file dialogs.
            /*!
              By default QtilitiesApplication::applicationSessionPath().

              \sa defaultPath()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            void setDefaultPath(const QString& file_path);
            //! Gets the default path used by the code editor widget when using file dialogs.
            /*!
              By default QtilitiesApplication::applicationSessionPath().

              \sa setDefaultPath()

              <i>This function was added in %Qtilities v1.1.</i>
              */
            QString defaultPath() const;

            // --------------------------------
            // Access To Contained Elements
            // --------------------------------
            //! Returns the code editor contained in this code editor widget.
            CodeEditor* codeEditor();
            //! Sets the syntax highlighter to be used in this code editor.
            /*!
              \note The code editor widget will manage the lifetime of the syntax highlighter.
              */
            void setSyntaxHighlighter(QSyntaxHighlighter* highlighter);
            //! Gets the syntax highlighter used in this code editor.
            QSyntaxHighlighter* syntaxHighlighter() const;
            //! Returns the search box widget of this code editor if it was constructed.
            SearchBoxWidget* searchBoxWidget() const;

            void deleteActionToolBars();
            void refreshActionToolBar(bool force_full_refresh);
            bool maybeSave();
        private:
            Ui::CodeEditorWidget *ui;
            CodeEditorWidgetPrivateData* d;
        };

        Q_DECLARE_OPERATORS_FOR_FLAGS(CodeEditorWidget::ActionFlags)
        Q_DECLARE_OPERATORS_FOR_FLAGS(CodeEditorWidget::DisplayFlags)
    }
}

#endif // CODE_EDITOR_WIDGET_H
