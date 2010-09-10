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

#ifndef CODE_EDITOR_WIDGET_H
#define CODE_EDITOR_WIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "CodeEditor.h"

#include <IContext>
#include <SearchBoxWidget>
#include <IActionProvider>

#include <QWidget>
#include <QPointer>
#include <QSyntaxHighlighter>
#include <QtGui>

namespace Ui {
    class CodeEditorWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::CoreGui::Constants;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
        \struct CodeEditorWidgetData
        \brief A structure storing private data in the CodeEditorWidget class.
          */
        struct CodeEditorWidgetData;

        /*!
        \class CodeEditorWidget
        \brief A code editor widget which contains a CodeEditor.

        The CodeEditorWidget class is a ready to use code editor widget. The widget contains a search & replace box
        as well as a range of actions which is registered in the action manager and available through the IActionProvider
        interface.

        The CodeEditorWidget has a file loading interface. Through this interface you can load a file into the
        editor using the loadFile() function. The contents of the editor can be saved to a file using the
        saveFile() function. When the contents in the editor was loaded from a file through the loadFile() method,
        you can call saveFile() without any parameters to save the content to the file from which it was loaded.
        The fileName() function returns the name of this file if it was set.

        \note This class is still under development and is not yet ready for production usage.

        \todo
        - Highlight word very very slow.
        - Better way to handle actions. Make like observer widget.
        - Search & Replace functionality lacking.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CodeEditorWidget : public QMainWindow, public IContext
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)

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
            Q_DECLARE_FLAGS(ActionFlags, ActionFlag);
            Q_FLAGS(ActionFlags);
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
            Q_DECLARE_FLAGS(DisplayFlags, DisplayFlag);
            Q_FLAGS(DisplayFlags);
            //! Function to get current display flags hint.
            DisplayFlags displayFlagsHint() const;

            // --------------------------------
            // Construction / Destruction
            // --------------------------------
            /*!
              \param display_flags The display flags which must be used for this code editor.
              */
            CodeEditorWidget(ActionFlags action_flags = ActionAllHints, DisplayFlags display_flags = AllDisplayFlagHint, QWidget* parent = 0);
            virtual ~CodeEditorWidget();
            bool eventFilter(QObject *object, QEvent *event);

            // --------------------------------
            // IContext Implementation
            // --------------------------------
            QString contextString() const { return globalMetaType(); }
            QString contextHelpId() const { return QString(); }

            // --------------------------------
            // Actions & Related Functionality
            // --------------------------------
        public:
            //! Returns the action handler interface for this code editor widget.
            IActionProvider* actionProvider();
            //! Sets the global meta type used for this widget.
            /*!
              \returns True if the meta_type string was valid. The validity check is done by checking if that a context with the same name does not yet exist in the context manager.

              \sa globalMetaType()
              */
            bool setGlobalMetaType(const QString& meta_type);
            //! Gets the global meta type used for this widget.
            /*!
              The global meta type is a string which defines this widget. The string must be a string which
              can be registered in the context manager. Thus, such a string must not yet exist as a context in the context
              manager.

              The global meta type is used for the following:
              - To register actions in the action manager.

              \returns The meta type used for this widget.

              \sa setGlobalMetaType()
              */
            QString globalMetaType() const;

        public slots:
            void handle_actionNew_triggered();
            void handle_actionOpen_triggered();
            bool handle_actionSave_triggered();
            bool handle_actionSaveAs_triggered();
            void handle_actionPrint_triggered();
            void handle_actionPrintPreview_triggered();
            void handle_actionPrintPdf_triggered();
            void handle_actionFindItem_triggered();
            void printPreview(QPrinter *printer);
            void handleSettingsUpdateRequest(const QString& request_id);

        private slots:
            void updateSaveAction();

        protected:
            void constructActions();
            void refreshActions();

            // --------------------------------
            // Handle Search Functionality
            // --------------------------------
        public slots:
            void handleSearchOptionsChanged();
            void handleSearchStringChanged(const QString& filter_string);
            void handleSearchReset();
            void handleSearchFindNext();
            void handleSearchFindPrevious();
            void handleSearchReplaceNext();
            void handleSearchReplacePrevious();

        public:
            // --------------------------------
            // File Loading Interface
            // --------------------------------
            //! Loads a file into the editor.
            /*!
              \returns True if the file was loaded successfully, false otherwise.
              */
            bool loadFile(const QString& file_name);
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

            // --------------------------------
            // Access To Contained Elements
            // --------------------------------
            //! Returns the code editor contained in this code editor widget.
            CodeEditor* const codeEditor();
            //! Sets the syntax highlighter to be used in this code editor.
            /*!
              \note The code editor widget will manage the lifetime of the syntax highlighter.
              */
            void setSyntaxHighlighter(QSyntaxHighlighter* highlighter);
            //! Gets the syntax highlighter used in this code editor.
            QSyntaxHighlighter* syntaxHighlighter() const;
            //! Returns the search box widget of this code editor if it was constructed.
            SearchBoxWidget* searchBoxWidget() const;

        private:
            bool maybeSave();

            Ui::CodeEditorWidget *ui;
            CodeEditorWidgetData* d;
        };
    }
}

#endif // CODE_EDITOR_WIDGET_H
