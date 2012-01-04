/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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

#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include "QtilitiesCoreGui_global.h"

#include <QPlainTextEdit>

namespace Ui {
    class TextEditor;
}

namespace Qtilities {
    namespace CoreGui {
        class LineNumberArea;

        /*!
        \struct CodeEditorPrivateData
        \brief A structure storing private data in the CodeEditor class.
          */
        struct CodeEditorPrivateData;

        /*!
        \class CodeEditor
        \brief A simple code editor embedded in the CodeEditorWidget class.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT CodeEditor : public QPlainTextEdit
        {
            Q_OBJECT

        public:
            CodeEditor(QWidget* parent = 0);
            virtual ~CodeEditor();

            void lineNumberAreaPaintEvent(QPaintEvent *event);
            int lineNumberAreaWidth();

        public slots:
            void updateLineNumberAreaWidth(int newBlockCount);
            void updateLineNumberArea(const QRect &rect, int dy);

            //! Slot to highlight the current line.
            void highlightCurrentLine();
            //! Slot to highlight the line specified  by the cursor.
            void highlightLine(QTextCursor cursor);
            //! Function which highlights the specified word in the document using the brush.
            void highlightWords(const QStringList& words, const QBrush& brush);
        public:
            //! Function which removes highlighted words set using highlightWords().
            void removeWordHighlighting();

        protected:
            void resizeEvent(QResizeEvent *e);

        private:
            CodeEditorPrivateData* d;
        };


        /*!
        \class qti_private_LineNumberArea
        \brief A class which draws line numbers on the CodeEditor widget.
          */
        class qti_private_LineNumberArea : public QWidget
        {
        public:
            qti_private_LineNumberArea(CodeEditor *editor) : QWidget(editor) {
                textEditor = editor;
            }
            QSize sizeHint() const {
                return QSize(textEditor->lineNumberAreaWidth(), 0);
            }

        protected:
            void paintEvent(QPaintEvent *event) {
                textEditor->lineNumberAreaPaintEvent(event);
            }

        private:
            CodeEditor *textEditor;
        };
    }
}

#endif // CODE_EDITOR_H
