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

        /*!
        \struct CodeEditorWidgetData
        \brief A structure storing private data in the CodeEditorWidget class.
          */
        struct CodeEditorWidgetData;

        /*!
        \class CodeEditorWidget
        \brief A code editor widget which contains a CodeEditor.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CodeEditorWidget : public QWidget, public IContext
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)

        public:
            CodeEditorWidget(bool show_search_box = false, QWidget* parent = 0);
            virtual ~CodeEditorWidget();
            bool eventFilter(QObject *object, QEvent *event);

            // --------------------------------
            // IContext Implementation
            // --------------------------------
            QString contextString() const { return QString(CONTEXT_CODE_EDITOR_WIDGET); }
            QString contextHelpId() const { return QString(); }

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

            // Search related slots
            void handleSearchOptionsChanged();
            void handleSearchStringChanged(const QString& filter_string);
            void handleSearchReset();
            void handleSearchFindNext();
            void handleSearchFindPrevious();
            void handleSearchReplaceNext();
            void handleSearchReplacePrevious();

        public:
            //! Function which highlights the specified word in the document using the brush.
            void highlightWord(const QString& word, const QBrush& brush);
            //! Function which removes highlighted words set using the highlightWord() function.
            void removeWordHighlighting();
            //! Loads a file into the editor.
            /*!
              \returns True if the file was loaded succesfully, false otherwise.
              */
            bool loadFile(const QString &file_name);

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

        private slots:
            void updateSaveAction();

        protected:
            void constructActions();
            void refreshActions();

        private:
            bool maybeSave();

            static QPointer<CodeEditorWidget> currentWidget;
            static QPointer<CodeEditorWidget> actionContainerWidget;

            Ui::CodeEditorWidget *ui;
            CodeEditorWidgetData* d;
        };
    }
}

#endif // CODE_EDITOR_WIDGET_H
