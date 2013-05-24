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
