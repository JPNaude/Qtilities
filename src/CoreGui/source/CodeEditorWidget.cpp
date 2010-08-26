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

#include "CodeEditorWidget.h"
#include "ui_CodeEditorWidget.h"
#include "CodeEditor.h"

#include <QtilitiesCoreGui>

#include <QFileInfo>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QClipboard>
#include <QPlainTextEdit>
#include <QPainter>
#include <QRect>

using namespace QtilitiesCoreGui;

QPointer<Qtilities::CoreGui::CodeEditorWidget> Qtilities::CoreGui::CodeEditorWidget::currentWidget;
QPointer<Qtilities::CoreGui::CodeEditorWidget> Qtilities::CoreGui::CodeEditorWidget::actionContainerWidget;

struct Qtilities::CoreGui::CodeEditorWidgetData {
    CodeEditorWidgetData() : actionNew(0),
    actionOpen(0),
    actionSave(0),
    actionSaveAs(0),
    actionPrint(0),
    actionPrintPreview(0),
    actionPrintPdf(0),
    actionUndo(0),
    actionRedo(0),
    actionCut(0),
    actionCopy(0),
    actionPaste(0),
    actionClear(0),
    actionSelectAll(0),
    actionFind(0),
    codeEditor(0),
    syntax_highlighter(0),
    searchBoxWidget(0),
    cursor_word_highlighter(0),
    cursor_find(0),
    cursor_repleace(0) {}
    
    QAction* actionNew;
    QAction* actionOpen;
    QAction* actionSave;
    QAction* actionSaveAs;
    QAction* actionPrint;
    QAction* actionPrintPreview;
    QAction* actionPrintPdf;
    QAction* actionUndo;
    QAction* actionRedo;
    QAction* actionCut;
    QAction* actionCopy;
    QAction* actionPaste;    
    QAction* actionClear;
    QAction* actionSelectAll;
    QAction* actionFind;

    CodeEditor* codeEditor;
    QPointer<QSyntaxHighlighter> syntax_highlighter;
    SearchBoxWidget* searchBoxWidget;

    //! A QTextCursor which handles highlighting of words in the document.
    QTextCursor* cursor_word_highlighter;
    //! A QTextCursor which handles finding of words.
    QTextCursor* cursor_find;
    //! A QTextCursor which handles replacing of words.
    QTextCursor* cursor_repleace;
};

Qtilities::CoreGui::CodeEditorWidget::CodeEditorWidget(bool show_search_box, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CodeEditorWidget)
{
    ui->setupUi(this);
    ui->widgetSearchBox->setVisible(false);
    d = new CodeEditorWidgetData;

    currentWidget = this;

    // Create the code editor:
    d->codeEditor = new CodeEditor();
    d->codeEditor->installEventFilter(this);
    d->codeEditor->viewport()->installEventFilter(this);

    // Create the text cursors:
    d->cursor_word_highlighter = new QTextCursor(d->codeEditor->document());
    d->cursor_find = new QTextCursor(d->codeEditor->document());
    d->cursor_repleace = new QTextCursor(d->codeEditor->document());

    if (ui->widgetEditorHolder->layout())
        delete ui->widgetEditorHolder->layout();

    // Create new layout & splitter
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,ui->widgetEditorHolder);
    layout->addWidget(d->codeEditor);
    layout->setMargin(0);
    layout->setSpacing(0);

    // Read the settings for this editor
    handleSettingsUpdateRequest(CONTEXT_CODE_EDITOR_WIDGET);
    connect(QtilitiesApplication::instance(),SIGNAL(settingsUpdateRequest(QString)),SLOT(handleSettingsUpdateRequest(QString)));

    if (show_search_box)
        handle_actionFindItem_triggered();
}

Qtilities::CoreGui::CodeEditorWidget::~CodeEditorWidget() {
    maybeSave();
    delete d;
}

bool Qtilities::CoreGui::CodeEditorWidget::eventFilter(QObject *object, QEvent *event) {
    if (object == d->codeEditor && event->type() == QEvent::FocusIn) {
        currentWidget = this;
        refreshActions();
        CONTEXT_MANAGER->setNewContext(contextString(),true);
    } else if (object == d->codeEditor->viewport() && event->type() == QEvent::FocusIn) {
        currentWidget = this;
        refreshActions();
        CONTEXT_MANAGER->setNewContext(contextString(),true);
    }
    return false;
}

Qtilities::CoreGui::CodeEditor* const Qtilities::CoreGui::CodeEditorWidget::codeEditor() {
    return d->codeEditor;
}

void Qtilities::CoreGui::CodeEditorWidget::setSyntaxHighlighter(QSyntaxHighlighter* highlighter) {
    if (!highlighter)
        return;

    if (highlighter == d->syntax_highlighter)
        return;

    d->syntax_highlighter = highlighter;
    d->syntax_highlighter->setDocument(d->codeEditor->document());
}

QSyntaxHighlighter* Qtilities::CoreGui::CodeEditorWidget::syntaxHighlighter() const {
    return d->syntax_highlighter;
}

Qtilities::CoreGui::SearchBoxWidget* Qtilities::CoreGui::CodeEditorWidget::searchBoxWidget() const {
    return d->searchBoxWidget;
}

void Qtilities::CoreGui::CodeEditorWidget::highlightWord(const QString& word, const QBrush& brush) {
    // Iterate over all words in the document and check if they must be highlighted.
    d->cursor_word_highlighter->movePosition(QTextCursor::Start);

    d->cursor_word_highlighter->movePosition(QTextCursor::StartOfWord);
    d->cursor_word_highlighter->movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    while (!d->cursor_word_highlighter->atEnd()) {
        if (d->cursor_word_highlighter->hasSelection()) {
            if (d->cursor_word_highlighter->selectedText() == word) {
                QTextCharFormat format(d->cursor_word_highlighter->charFormat());
                format.setBackground(brush);
                d->cursor_word_highlighter->setCharFormat(format);
            } else {
                QTextCharFormat format(d->cursor_word_highlighter->charFormat());
                QBrush default_brush(Qt::white);
                format.setBackground(default_brush);
                d->cursor_word_highlighter->setCharFormat(format);
            }
        }
        d->cursor_word_highlighter->movePosition(QTextCursor::NextWord);
        d->cursor_word_highlighter->movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    }
}

void Qtilities::CoreGui::CodeEditorWidget::removeWordHighlighting() {

}

bool Qtilities::CoreGui::CodeEditorWidget::loadFile(const QString &file_name) {
    // Read everything from the file
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString contents = file.readAll();
    d->codeEditor->document()->setModified(false);
    d->codeEditor->setPlainText(contents);
    setWindowModified(false);

    return true;
}

bool Qtilities::CoreGui::CodeEditorWidget::maybeSave() {
    if (!d->codeEditor->document()->isModified())
        return true;
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Code Editor"),tr("The document has been modified.\nDo you want to save your changes?"),QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return handle_actionSave_triggered();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void Qtilities::CoreGui::CodeEditorWidget::handle_actionNew_triggered() {
    if (maybeSave()) {
        currentWidget->d->codeEditor->clear();
    }
}

void Qtilities::CoreGui::CodeEditorWidget::handle_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              QString(), tr("All Files (*)"));
    if (!fileName.isEmpty())
        loadFile(fileName);
}

bool Qtilities::CoreGui::CodeEditorWidget::handle_actionSave_triggered() {
    return handle_actionSaveAs_triggered();
}

bool Qtilities::CoreGui::CodeEditorWidget::handle_actionSaveAs_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(), tr("All Files (*)"));
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;

    file.write(d->codeEditor->toPlainText().toLocal8Bit());
    file.close();

    updateSaveAction();
    return true;
}

void Qtilities::CoreGui::CodeEditorWidget::handle_actionPrint_triggered() {
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (currentWidget->d->codeEditor->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        currentWidget->d->codeEditor->print(&printer);
    }
    delete dlg;
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::handle_actionPrintPreview_triggered() {
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    currentWidget->d->codeEditor->print(printer);
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::handle_actionPrintPdf_triggered() {
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export PDF"),
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        currentWidget->d->codeEditor->document()->print(&printer);
    }
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::handleSettingsUpdateRequest(const QString& request_id) {
    if (request_id == CONTEXT_CODE_EDITOR_WIDGET) {
        // Read the text editor settings from QSettings
        QSettings settings;
        settings.beginGroup("GUI");
        settings.beginGroup("Editors");
        settings.beginGroup("Code Editor Widget");

        QFont font;
        font.setFamily(settings.value("font_type","Courier").toString());
        font.setFixedPitch(true);
        font.setPointSize(settings.value("font_size",10).toInt());
        d->codeEditor->setFont(font);

        settings.endGroup();
        settings.endGroup();
        settings.endGroup();
    }
}

void Qtilities::CoreGui::CodeEditorWidget::handle_actionFindItem_triggered() {
    if (!currentWidget)
        return;

    if (!currentWidget->d->searchBoxWidget) {
        SearchBoxWidget::SearchOptions search_options = 0;
        search_options |= SearchBoxWidget::CaseSensitive;
        search_options |= SearchBoxWidget::WholeWordsOnly;
        SearchBoxWidget::ButtonFlags button_flags = 0;
        button_flags |= SearchBoxWidget::HideButton;
        button_flags |= SearchBoxWidget::NextButtons;
        button_flags |= SearchBoxWidget::PreviousButtons;
        currentWidget->d->searchBoxWidget = new SearchBoxWidget(search_options,SearchBoxWidget::SearchAndReplace,button_flags);
        currentWidget->d->searchBoxWidget->layout()->setContentsMargins(3,0,0,0);
        currentWidget->d->searchBoxWidget->setWholeWordsOnly(false);
        if (currentWidget->ui->widgetSearchBox->layout())
            delete currentWidget->ui->widgetSearchBox->layout();

        QHBoxLayout* layout = new QHBoxLayout(currentWidget->ui->widgetSearchBox);
        layout->addWidget(currentWidget->d->searchBoxWidget);
        layout->setMargin(0);
        currentWidget->d->searchBoxWidget->show();

        connect(currentWidget->d->searchBoxWidget,SIGNAL(searchOptionsChanged()),currentWidget,SLOT(handleSearchOptionsChanged()));
        connect(currentWidget->d->searchBoxWidget,SIGNAL(searchStringChanged(QString)),currentWidget,SLOT(handleSearchStringChanged(QString)));
        connect(currentWidget->d->searchBoxWidget,SIGNAL(btnClose_clicked()),currentWidget->ui->widgetSearchBox,SLOT(hide()));
        connect(currentWidget->d->searchBoxWidget,SIGNAL(btnClose_clicked()),currentWidget,SLOT(handleSearchReset()));
        connect(currentWidget->d->searchBoxWidget,SIGNAL(btnFindNext_clicked()),currentWidget,SLOT(handleSearchFindNext()));
        connect(currentWidget->d->searchBoxWidget,SIGNAL(btnFindPrevious_clicked()),currentWidget,SLOT(handleSearchFindPrevious()));
        connect(currentWidget->d->searchBoxWidget,SIGNAL(btnReplaceNext_clicked()),currentWidget,SLOT(handleSearchReplaceNext()));
        connect(currentWidget->d->searchBoxWidget,SIGNAL(btnReplacePrevious_clicked()),currentWidget,SLOT(handleSearchReplacePrevious()));
    }

    currentWidget->ui->widgetSearchBox->show();
    currentWidget->d->searchBoxWidget->setEditorFocus();

    // We check if there is a selection in the user visible cursor. If so we set that as the search string.
    QTextCursor visible_cursor = currentWidget->d->codeEditor->textCursor();
    if (visible_cursor.hasSelection()) {
        currentWidget->d->searchBoxWidget->setCurrentSearchString(visible_cursor.selectedText());
    }

    currentWidget->handleSearchStringChanged(currentWidget->d->searchBoxWidget->currentSearchString());
}

void Qtilities::CoreGui::CodeEditorWidget::handleSearchOptionsChanged() {
    handleSearchStringChanged(d->searchBoxWidget->currentSearchString());
}

void Qtilities::CoreGui::CodeEditorWidget::handleSearchStringChanged(const QString& filter_string) {
    if (!currentWidget)
        return;

    QTextDocument::FindFlags find_flags = 0;
    if (d->searchBoxWidget->wholeWordsOnly())
        find_flags |= QTextDocument::FindWholeWords;
    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QTextDocument::FindCaseSensitively;

    d->codeEditor->find(filter_string,find_flags | QTextDocument::FindBackward);
    d->codeEditor->find(filter_string,find_flags);

    //QBrush brush(Qt::green);
    //highlightWord(filter_string,brush);
}

void Qtilities::CoreGui::CodeEditorWidget::handleSearchReset() {
    handleSearchStringChanged("");
}

void Qtilities::CoreGui::CodeEditorWidget::updateSaveAction() {
    if (!actionContainerWidget) {
        d->actionSave->setEnabled(false);
        return;
    }

    actionContainerWidget->d->actionSave->setEnabled(true);
}

void Qtilities::CoreGui::CodeEditorWidget::handleSearchFindNext() {
    QTextDocument::FindFlags find_flags = 0;
    if (d->searchBoxWidget->wholeWordsOnly())
        find_flags |= QTextDocument::FindWholeWords;
    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QTextDocument::FindCaseSensitively;

    if (!d->codeEditor->find(d->searchBoxWidget->currentSearchString(),find_flags)) {
        // Implement ability to go to start of document and search again. This does not work.
        d->codeEditor->textCursor().movePosition(QTextCursor::Start);
    }
}

void Qtilities::CoreGui::CodeEditorWidget::handleSearchFindPrevious() {
    QTextDocument::FindFlags find_flags = 0;
    if (d->searchBoxWidget->wholeWordsOnly())
        find_flags |= QTextDocument::FindWholeWords;
    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QTextDocument::FindCaseSensitively;

    d->codeEditor->find(d->searchBoxWidget->currentSearchString(),find_flags | QTextDocument::FindBackward);
}

void Qtilities::CoreGui::CodeEditorWidget::handleSearchReplaceNext() {

}

void Qtilities::CoreGui::CodeEditorWidget::handleSearchReplacePrevious() {

}

void Qtilities::CoreGui::CodeEditorWidget::constructActions() {
    if (actionContainerWidget)
        return;
    actionContainerWidget = this;

    // Register the context:
    int context_id = CONTEXT_MANAGER->registerContext(contextString());

    QList<int> context;
    context.push_front(context_id);

    // ---------------------------
    // New
    // ---------------------------
    d->actionNew = new QAction(QIcon(),"",this);
    connect(d->actionNew,SIGNAL(triggered()),SLOT(handle_actionNew_triggered()));
    ACTION_MANAGER->registerAction(MENU_FILE_NEW,d->actionNew,context);
    // ---------------------------
    // Open
    // ---------------------------
    d->actionOpen = new QAction(QIcon(),"",this);
    connect(d->actionOpen,SIGNAL(triggered()),SLOT(handle_actionOpen_triggered()));
    ACTION_MANAGER->registerAction(MENU_FILE_OPEN,d->actionOpen,context);
    // ---------------------------
    // Save
    // ---------------------------
    d->actionSave = new QAction(QIcon(),"",this);
    d->actionSave->setEnabled(false);
    connect(d->actionSave,SIGNAL(triggered()),SLOT(handle_actionSave_triggered()));
    ACTION_MANAGER->registerAction(MENU_FILE_SAVE,d->actionSave,context);
    // ---------------------------
    // SaveAs
    // ---------------------------
    d->actionSaveAs = new QAction(QIcon(),"",this);
    connect(d->actionSaveAs,SIGNAL(triggered()),SLOT(handle_actionSaveAs_triggered()));
    ACTION_MANAGER->registerAction(MENU_FILE_SAVE_AS,d->actionSaveAs,context);
    // ---------------------------
    // Print
    // ---------------------------
    d->actionPrint = new QAction(QIcon(),"",this);
    connect(d->actionPrint,SIGNAL(triggered()),SLOT(handle_actionPrint_triggered()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT,d->actionPrint,context);
    // ---------------------------
    // PrintPreview
    // ---------------------------
    d->actionPrintPreview = new QAction(QIcon(),"",this);
    connect(d->actionPrintPreview,SIGNAL(triggered()),SLOT(handle_actionPrintPreview_triggered()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PREVIEW,d->actionPrintPreview,context);
    // ---------------------------
    // PrintPDF
    // ---------------------------
    d->actionPrintPdf = new QAction(QIcon(),"",this);
    connect(d->actionPrintPdf,SIGNAL(triggered()),SLOT(handle_actionPrintPdf_triggered()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PDF,d->actionPrintPdf,context);
    // ---------------------------
    // Undo
    // ---------------------------
    d->actionUndo = new QAction(QIcon(),"",this);
    connect(d->actionUndo,SIGNAL(triggered()),d->codeEditor,SLOT(undo()));
    ACTION_MANAGER->registerAction(MENU_EDIT_UNDO,d->actionUndo,context);
    // ---------------------------
    // Redo
    // ---------------------------
    d->actionRedo = new QAction(QIcon(),"",this);
    connect(d->actionRedo,SIGNAL(triggered()),d->codeEditor,SLOT(redo()));
    ACTION_MANAGER->registerAction(MENU_EDIT_REDO,d->actionRedo,context);
    // ---------------------------
    // Cut
    // ---------------------------
    d->actionCut = new QAction(QIcon(),"",this);
    connect(d->actionCut,SIGNAL(triggered()),d->codeEditor,SLOT(cut()));
    ACTION_MANAGER->registerAction(MENU_EDIT_CUT,d->actionCut,context);
    // ---------------------------
    // Copy
    // ---------------------------
    d->actionCopy = new QAction(QIcon(),"",this);
    connect(d->actionCopy,SIGNAL(triggered()),d->codeEditor,SLOT(copy()));
    ACTION_MANAGER->registerAction(MENU_EDIT_COPY,d->actionCopy,context);
    // ---------------------------
    // Paste
    // ---------------------------
    d->actionPaste = new QAction(QIcon(),"",this);
    connect(d->actionPaste,SIGNAL(triggered()),d->codeEditor,SLOT(paste()));
    ACTION_MANAGER->registerAction(MENU_EDIT_PASTE,d->actionPaste,context);
    // ---------------------------
    // Select All
    // ---------------------------
    d->actionSelectAll = new QAction(QIcon(),"",this);
    connect(d->actionSelectAll,SIGNAL(triggered()),d->codeEditor,SLOT(selectAll()));
    ACTION_MANAGER->registerAction(MENU_EDIT_SELECT_ALL,d->actionSelectAll,context);
    // ---------------------------
    // Clear
    // ---------------------------
    d->actionClear = new QAction(QIcon(),"",this);
    connect(d->actionClear,SIGNAL(triggered()),d->codeEditor,SLOT(clear()));
    ACTION_MANAGER->registerAction(MENU_EDIT_CLEAR,d->actionClear,context);
    // ---------------------------
    // Find
    // ---------------------------
    d->actionFind = new QAction(QIcon(),"",this);
    connect(d->actionFind,SIGNAL(triggered()),SLOT(handle_actionFindItem_triggered()));
    ACTION_MANAGER->registerAction(MENU_EDIT_FIND,d->actionFind,context);
}


void Qtilities::CoreGui::CodeEditorWidget::refreshActions() {
    if (!actionContainerWidget)
        constructActions();

    if (!currentWidget)
        return;

    // Connect actions to correct editor
    connect(currentWidget->d->codeEditor, SIGNAL(copyAvailable(bool)), actionContainerWidget->d->actionCut, SLOT(setEnabled(bool)));
    connect(currentWidget->d->codeEditor, SIGNAL(copyAvailable(bool)), actionContainerWidget->d->actionCopy, SLOT(setEnabled(bool)));
    connect(currentWidget->d->codeEditor, SIGNAL(undoAvailable(bool)), actionContainerWidget->d->actionUndo, SLOT(setEnabled(bool)));
    connect(currentWidget->d->codeEditor, SIGNAL(redoAvailable(bool)), actionContainerWidget->d->actionRedo, SLOT(setEnabled(bool)));

    // Update actions
    actionContainerWidget->d->actionUndo->setEnabled(currentWidget->d->codeEditor->document()->isUndoAvailable());
    actionContainerWidget->d->actionRedo->setEnabled(currentWidget->d->codeEditor->document()->isRedoAvailable());
    #ifndef QT_NO_CLIPBOARD
        actionContainerWidget->d->actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
    #endif
}

