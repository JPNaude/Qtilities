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

#include "CodeEditorWidget.h"
#include "ui_CodeEditorWidget.h"
#include "CodeEditor.h"

#include "ActionProvider.h"
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QFileInfo>
#include <QtGui>

using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui::Actions;

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
    actionClear(0),
    actionSelectAll(0),
    actionFind(0),
    codeEditor(0),
    syntax_highlighter(0),
    searchBoxWidget(0),
    action_provider(0) {}
    ~CodeEditorWidgetData() {
        if (syntax_highlighter)
            delete syntax_highlighter;
    }
    
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
    QAction* actionClear;
    QAction* actionSelectAll;
    QAction* actionFind;

    //! The file name linked to the contents of the code editor. \sa loadFile()
    QString current_file;
    //! The global meta type string used for this editor.
    QString global_meta_type;

    //! The central widget of the main window.
    QWidget* central_widget;
    //! The contained code editor.
    CodeEditor* codeEditor;
    //! The syntax highlighter used.
    QPointer<QSyntaxHighlighter> syntax_highlighter;
    //! The contained search box widget.
    SearchBoxWidget* searchBoxWidget;

    //! The IActionProvider interface implementation.
    ActionProvider* action_provider;
    //! Stores the display flags for this code editor.
    CodeEditorWidget::DisplayFlags display_flags;
    //! Stores the action flags for this code editor.
    CodeEditorWidget::ActionFlags action_flags;
    //! The action toolbars list. Contains toolbars created for each category in the action provider.
    QList<QToolBar*> action_toolbars;

    //! The central widget layout.
    QBoxLayout* central_widget_layout;
};

Qtilities::CoreGui::CodeEditorWidget::CodeEditorWidget(ActionFlags action_flags, DisplayFlags display_flags, QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::CodeEditorWidget)
{
    ui->setupUi(this);
    d = new CodeEditorWidgetData;
    d->action_flags = action_flags;
    d->display_flags = display_flags;
    d->action_provider = new ActionProvider(this);
    d->central_widget = new QWidget();
    setCentralWidget(d->central_widget);

    // Create the code editor:
    d->codeEditor = new CodeEditor();
    d->codeEditor->installEventFilter(this);
    d->codeEditor->viewport()->installEventFilter(this);

    // Read the settings for this editor:
    handleSettingsUpdateRequest(d->global_meta_type);
    if (QtilitiesApplication::instance(true))
        connect(QtilitiesApplication::instance(),SIGNAL(settingsUpdateRequest(QString)),SLOT(handleSettingsUpdateRequest(QString)));

    // Create new layout:
    if (d->central_widget->layout())
        delete d->central_widget->layout();

    d->central_widget_layout = new QBoxLayout(QBoxLayout::TopToBottom,d->central_widget);
    d->central_widget_layout->addWidget(d->codeEditor);
    d->central_widget_layout->setMargin(0);
    d->central_widget_layout->setSpacing(0);

    // Assign a default meta type for this widget:
    // We construct each action and then register it
    QString context_string = "CodeEditor";
    int count = 0;
    context_string.append(QString("%1").arg(count));
    while (CONTEXT_MANAGER->hasContext(context_string)) {
        QString count_string = QString("%1").arg(count);
        context_string.chop(count_string.length());
        ++count;
        context_string.append(QString("%1").arg(count));
    }
    CONTEXT_MANAGER->registerContext(context_string);
    d->global_meta_type = context_string;

    // Create actions only after global meta type was set.
    constructActions();

    // Action toolbars construction:
    if (d->display_flags & ActionToolBar) {
        QList<QtilitiesCategory> categories = d->action_provider->actionCategories();
        for (int i = 0; i < categories.count(); i++) {
            QList<QAction*> action_list = d->action_provider->actions(IActionProvider::FilterHidden,categories.at(i));
            if (action_list.count() > 0) {
                QToolBar* new_toolbar = addToolBar(categories.at(i).toString());
                d->action_toolbars << new_toolbar;
                new_toolbar->addActions(action_list);
            }
        }
    }

    // Check if we must connect to the paste action for the new hints:
    Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
    if (command) {
        if (command->action()) {
            if (d->action_flags & ActionPaste)
                connect(command->action(),SIGNAL(triggered()),d->codeEditor,SLOT(paste()));
            else
                command->action()->disconnect();
        }
    }

}

Qtilities::CoreGui::CodeEditorWidget::~CodeEditorWidget() {
    maybeSave();
    delete d;
}

bool Qtilities::CoreGui::CodeEditorWidget::eventFilter(QObject *object, QEvent *event) {
    if (object == d->codeEditor && event->type() == QEvent::FocusIn) {
        refreshActions();
        CONTEXT_MANAGER->setNewContext(contextString(),true);

        if (d->action_flags & ActionPaste) {
            // Connect to the paste action
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    connect(command->action(),SIGNAL(triggered()),d->codeEditor,SLOT(paste()));
            }
        }
    } else if (object == d->codeEditor->viewport() && event->type() == QEvent::FocusIn) {
        refreshActions();
        CONTEXT_MANAGER->setNewContext(contextString(),true);

        if (d->action_flags & ActionPaste) {
            // Connect to the paste action
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    connect(command->action(),SIGNAL(triggered()),d->codeEditor,SLOT(paste()));
            }
        }
    } else if (object == d->codeEditor && event->type() == QEvent::FocusOut) {
        if (d->action_flags & ActionPaste) {
            // Disconnect the paste action from the this widget.
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    command->action()->disconnect(this);
            }
        }
    } else if (object == d->codeEditor->viewport() && event->type() == QEvent::FocusOut) {
        if (d->action_flags & ActionPaste) {
            // Disconnect the paste action from the this widget.
            Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
            if (command) {
                if (command->action())
                    command->action()->disconnect(this);
            }
        }
    }
    return false;
}

Qtilities::CoreGui::CodeEditor* Qtilities::CoreGui::CodeEditorWidget::codeEditor() {
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

bool Qtilities::CoreGui::CodeEditorWidget::loadFile(const QString &file_name) {
    maybeSave();

    // Read everything from the file
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString contents = file.readAll();
    d->codeEditor->document()->setModified(false);
    d->codeEditor->setPlainText(contents);
    setWindowModified(false);
    d->current_file = file_name;

    return true;
}

bool Qtilities::CoreGui::CodeEditorWidget::saveFile(QString file_name) {
    if (file_name.isEmpty())
        file_name = d->current_file;

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly))
        return false;

    file.write(d->codeEditor->toPlainText().toLocal8Bit());
    file.close();

    d->codeEditor->document()->setModified(false);

    updateSaveAction();
    return true;
}

QString Qtilities::CoreGui::CodeEditorWidget::fileName() const {
    return d->current_file;
}

bool Qtilities::CoreGui::CodeEditorWidget::maybeSave() {
    if (!d->codeEditor->document()->isModified())
        return true;
    QMessageBox::StandardButton ret;
    if (d->current_file.isEmpty())
        ret = QMessageBox::warning(this, tr("Code Editor"),tr("The document has been modified.\nDo you want to save your changes?"),QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    else
        ret = QMessageBox::warning(this, tr("File Changed"),QString(tr("The modified document is linked to the following file:\n\n%1\n\nDo you want to save your changes?")).arg(d->current_file),QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return actionSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}


Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::CodeEditorWidget::actionProvider() {
    return d->action_provider;
}

bool Qtilities::CoreGui::CodeEditorWidget::setGlobalMetaType(const QString& meta_type) {
    // Check if this global meta type is allowed.
    if (CONTEXT_MANAGER->hasContext(meta_type))
        return false;

    d->global_meta_type = meta_type;
    return true;
}

QString Qtilities::CoreGui::CodeEditorWidget::globalMetaType() const {
    return d->global_meta_type;
}

void Qtilities::CoreGui::CodeEditorWidget::actionNew() {
    if (maybeSave()) {
        d->codeEditor->clear();
    }
}

void Qtilities::CoreGui::CodeEditorWidget::actionOpen() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              QString(), tr("All Files (*)"));
    if (!fileName.isEmpty())
        loadFile(fileName);
}

bool Qtilities::CoreGui::CodeEditorWidget::actionSave() {
    if (!d->current_file.isEmpty())
        return saveFile();
    else
        return actionSaveAs();
}

bool Qtilities::CoreGui::CodeEditorWidget::actionSaveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(), tr("All Files (*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void Qtilities::CoreGui::CodeEditorWidget::actionPrint() {
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (d->codeEditor->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        d->codeEditor->print(&printer);
    }
    delete dlg;
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::actionPrintPreview() {
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
    d->codeEditor->print(printer);
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::actionPrintPdf() {
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export PDF"),
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        d->codeEditor->document()->print(&printer);
    }
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::handleSettingsUpdateRequest(const QString& request_id) {
    if (request_id == d->global_meta_type) {
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

void Qtilities::CoreGui::CodeEditorWidget::showSearchBox() {
    if (!d->searchBoxWidget) {
        SearchBoxWidget::SearchOptions search_options = 0;
        search_options |= SearchBoxWidget::CaseSensitive;
        search_options |= SearchBoxWidget::WholeWordsOnly;
        SearchBoxWidget::ButtonFlags button_flags = 0;
        button_flags |= SearchBoxWidget::HideButtonDown;
        button_flags |= SearchBoxWidget::NextButtons;
        button_flags |= SearchBoxWidget::PreviousButtons;
        d->searchBoxWidget = new SearchBoxWidget(search_options,SearchBoxWidget::SearchAndReplace,button_flags);
        d->searchBoxWidget->setObjectName("Search Box: Code Editor (" + objectName() + ")");
        d->searchBoxWidget->setWholeWordsOnly(false);
        d->searchBoxWidget->setPlainTextEditor(d->codeEditor);
        if (d->central_widget_layout)
            d->central_widget_layout->addWidget(d->searchBoxWidget);
    }
    d->searchBoxWidget->setEditorFocus();

    // We check if there is a selection in the user visible cursor. If so we set that as the search string.
    QTextCursor visible_cursor = d->codeEditor->textCursor();
    if (visible_cursor.hasSelection()) {
        d->searchBoxWidget->setCurrentSearchString(visible_cursor.selectedText());
    }

    if (!d->searchBoxWidget->isVisible())
        d->searchBoxWidget->show();
}

void Qtilities::CoreGui::CodeEditorWidget::updateSaveAction() {
    if (d->codeEditor->document()->isModified())
        d->actionSave->setEnabled(true);
    else
        d->actionSave->setEnabled(false);
}

void Qtilities::CoreGui::CodeEditorWidget::constructActions() {
    int context_id = CONTEXT_MANAGER->registerContext(d->global_meta_type);
    QList<int> context;
    context.push_front(context_id);

    // ---------------------------
    // New
    // ---------------------------
    if (d->action_flags & ActionNew) {
        d->actionNew = new QAction(QIcon(ICON_EDIT_CLEAR_16x16),tr("New"),this);
        d->action_provider->addAction(d->actionNew,QtilitiesCategory(tr("File")));
        connect(d->actionNew,SIGNAL(triggered()),SLOT(actionNew()));
        ACTION_MANAGER->registerAction(MENU_FILE_NEW,d->actionNew,context);
    }
    // ---------------------------
    // Open
    // ---------------------------
    if (d->action_flags & ActionOpenFile) {
        d->actionOpen = new QAction(QIcon(ICON_FILE_OPEN_16x16),tr("Open"),this);
        d->action_provider->addAction(d->actionOpen,QtilitiesCategory(tr("File")));
        connect(d->actionOpen,SIGNAL(triggered()),SLOT(actionOpen()));
        ACTION_MANAGER->registerAction(MENU_FILE_OPEN,d->actionOpen,context);
    }
    // ---------------------------
    // Save
    // ---------------------------
    if (d->action_flags & ActionSaveFile) {
        d->actionSave = new QAction(QIcon(ICON_FILE_SAVE_16x16),tr("Save"),this);
        d->actionSave->setEnabled(false);
        d->action_provider->addAction(d->actionSave,QtilitiesCategory(tr("File")));
        connect(d->actionSave,SIGNAL(triggered()),SLOT(actionSave()));
        ACTION_MANAGER->registerAction(MENU_FILE_SAVE,d->actionSave,context);
    }
    // ---------------------------
    // SaveAs
    // ---------------------------
    if (d->action_flags & ActionSaveFileAs) {
        d->actionSaveAs = new QAction(QIcon(ICON_FILE_SAVEAS_16x16),tr("Save As"),this);
        d->action_provider->addAction(d->actionSaveAs,QtilitiesCategory(tr("File")));
        connect(d->actionSaveAs,SIGNAL(triggered()),SLOT(actionSaveAs()));
        ACTION_MANAGER->registerAction(MENU_FILE_SAVE_AS,d->actionSaveAs,context);
    }
    // ---------------------------
    // Print
    // ---------------------------
    if (d->action_flags & ActionPrint) {
        d->actionPrint = new QAction(QIcon(ICON_PRINT_16x16),tr("Print"),this);
        d->action_provider->addAction(d->actionPrint,QtilitiesCategory(tr("Print")));
        connect(d->actionPrint,SIGNAL(triggered()),SLOT(actionPrint()));
        ACTION_MANAGER->registerAction(MENU_FILE_PRINT,d->actionPrint,context);
    }
    // ---------------------------
    // PrintPreview
    // ---------------------------
    if (d->action_flags & ActionPrintPreview) {
        d->actionPrintPreview = new QAction(QIcon(ICON_PRINT_PREVIEW_16x16),tr("Print Preview"),this);
        d->action_provider->addAction(d->actionPrintPreview,QtilitiesCategory(tr("Print")));
        connect(d->actionPrintPreview,SIGNAL(triggered()),SLOT(actionPrintPreview()));
        ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PREVIEW,d->actionPrintPreview,context);
    }
    // ---------------------------
    // PrintPDF
    // ---------------------------
    if (d->action_flags & ActionPrintPDF) {
        d->actionPrintPdf = new QAction(QIcon(ICON_PRINT_PDF_16x16),tr("Print PDF"),this);
        d->action_provider->addAction(d->actionPrintPdf,QtilitiesCategory(tr("Print")));
        connect(d->actionPrintPdf,SIGNAL(triggered()),SLOT(actionPrintPdf()));
        ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PDF,d->actionPrintPdf,context);
    }
    // ---------------------------
    // Undo
    // ---------------------------
    if (d->action_flags & ActionUndo) {
        d->actionUndo = new QAction(QIcon(ICON_EDIT_UNDO_16x16),tr("Undo"),this);
        d->action_provider->addAction(d->actionUndo,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionUndo,SIGNAL(triggered()),d->codeEditor,SLOT(undo()));
        ACTION_MANAGER->registerAction(MENU_EDIT_UNDO,d->actionUndo,context);
    }
    // ---------------------------
    // Redo
    // ---------------------------
    if (d->action_flags & ActionRedo) {
        d->actionRedo = new QAction(QIcon(ICON_EDIT_REDO_16x16),tr("Redo"),this);
        d->action_provider->addAction(d->actionRedo,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionRedo,SIGNAL(triggered()),d->codeEditor,SLOT(redo()));
        ACTION_MANAGER->registerAction(MENU_EDIT_REDO,d->actionRedo,context);
    }
    // ---------------------------
    // Cut
    // ---------------------------
    if (d->action_flags & ActionCut) {
        d->actionCut = new QAction(QIcon(ICON_EDIT_CUT_16x16),tr("Cut"),this);
        d->action_provider->addAction(d->actionCut,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionCut,SIGNAL(triggered()),d->codeEditor,SLOT(cut()));
        ACTION_MANAGER->registerAction(MENU_EDIT_CUT,d->actionCut,context);
    }
    // ---------------------------
    // Copy
    // ---------------------------
    if (d->action_flags & ActionCopy) {
        d->actionCopy = new QAction(QIcon(ICON_EDIT_COPY_16x16),tr("Copy"),this);
        d->action_provider->addAction(d->actionCopy,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionCopy,SIGNAL(triggered()),d->codeEditor,SLOT(copy()));
        ACTION_MANAGER->registerAction(MENU_EDIT_COPY,d->actionCopy,context);
    }
    // ---------------------------
    // Paste
    // ---------------------------
    if (d->action_flags & ActionPaste) {
        Command* command = ACTION_MANAGER->command(MENU_EDIT_PASTE);
        if (command) {
            if (command->action()) {
                d->action_provider->addAction(command->action(),QtilitiesCategory(tr("Clipboard")));
                connect(command->action(),SIGNAL(triggered()),d->codeEditor,SLOT(paste()));
            }
        }
    }
    // ---------------------------
    // Select All
    // ---------------------------
    if (d->action_flags & ActionSelectAll) {
        d->actionSelectAll = new QAction(QIcon(ICON_EDIT_SELECT_ALL_16x16),tr("Select All"),this);
        d->action_provider->addAction(d->actionSelectAll,QtilitiesCategory(tr("Selection")));
        connect(d->actionSelectAll,SIGNAL(triggered()),d->codeEditor,SLOT(selectAll()));
        ACTION_MANAGER->registerAction(MENU_EDIT_SELECT_ALL,d->actionSelectAll,context);
    }
    // ---------------------------
    // Clear
    // ---------------------------
    if (d->action_flags & ActionClear) {
        d->actionClear = new QAction(QIcon(ICON_EDIT_CLEAR_16x16),tr("Clear"),this);
        d->action_provider->addAction(d->actionClear,QtilitiesCategory(tr("Selection")));
        connect(d->actionClear,SIGNAL(triggered()),d->codeEditor,SLOT(clear()));
        ACTION_MANAGER->registerAction(MENU_EDIT_CLEAR,d->actionClear,context);
    }
    // ---------------------------
    // Find
    // ---------------------------
    if (d->action_flags & ActionFind) {
        d->actionFind = new QAction(QIcon(ICON_FIND_16x16),tr("Find"),this);
        d->action_provider->addAction(d->actionFind,QtilitiesCategory(tr("Selection")));
        connect(d->actionFind,SIGNAL(triggered()),SLOT(showSearchBox()));
        ACTION_MANAGER->registerAction(MENU_EDIT_FIND,d->actionFind,context);
    }

    connect(d->codeEditor, SIGNAL(copyAvailable(bool)), d->actionCut, SLOT(setEnabled(bool)));
    connect(d->codeEditor, SIGNAL(copyAvailable(bool)), d->actionCopy, SLOT(setEnabled(bool)));
    connect(d->codeEditor, SIGNAL(undoAvailable(bool)), d->actionUndo, SLOT(setEnabled(bool)));
    connect(d->codeEditor, SIGNAL(redoAvailable(bool)), d->actionRedo, SLOT(setEnabled(bool)));
    connect(d->codeEditor,SIGNAL(modificationChanged(bool)),d->actionSave,SLOT(setEnabled(bool)));
}

void Qtilities::CoreGui::CodeEditorWidget::refreshActions() {
    // Update actions
    d->actionUndo->setEnabled(d->codeEditor->document()->isUndoAvailable());
    d->actionRedo->setEnabled(d->codeEditor->document()->isRedoAvailable());
}

