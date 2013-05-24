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
#include "CodeEditor.h"

#include <QSyntaxHighlighter>
#include <QPainter>
#include <QtDebug>

struct Qtilities::CoreGui::CodeEditorPrivateData {
    CodeEditorPrivateData() : lineNumberArea(0),
    syntaxHighlighter(0) {}

    QWidget *lineNumberArea;
    QSyntaxHighlighter* syntaxHighlighter;
};

Qtilities::CoreGui::CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent) {

    d = new CodeEditorPrivateData;

    // Setup the line number drawing
    d->lineNumberArea = new qti_private_LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(const QRect &, int)), this, SLOT(updateLineNumberArea(const QRect &, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

Qtilities::CoreGui::CodeEditor::~CodeEditor() {
    delete d;
}

void Qtilities::CoreGui::CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(d->lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, d->lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int Qtilities::CoreGui::CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void Qtilities::CoreGui::CodeEditor::updateLineNumberAreaWidth(int newBlockCount) {
    Q_UNUSED(newBlockCount)

    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void Qtilities::CoreGui::CodeEditor::highlightCurrentLine() {
    highlightLine(textCursor());
}

void Qtilities::CoreGui::CodeEditor::highlightLine(QTextCursor cursor) {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = cursor;
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void Qtilities::CoreGui::CodeEditor::updateLineNumberArea(const QRect& rect, int dy) {
    if (dy)
        d->lineNumberArea->scroll(0, dy);
    else
        d->lineNumberArea->update(0, rect.y(), d->lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void Qtilities::CoreGui::CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    d->lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void Qtilities::CoreGui::CodeEditor::highlightWords(const QStringList& words, const QBrush& brush) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);

    foreach (const QString& current_string, words) {
        while (find(current_string)) {
            QTextCursor find_cursor = document()->find(current_string,cursor);
            QTextCharFormat format(find_cursor.charFormat());
            format.setBackground(brush);
            find_cursor.mergeCharFormat(format);
        }
        cursor.movePosition(QTextCursor::Start);
        setTextCursor(cursor);
    }

    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);
}

void Qtilities::CoreGui::CodeEditor::removeWordHighlighting() {
    QString tmp = document()->toPlainText();
    document()->clear();
    document()->setPlainText(tmp);
}

