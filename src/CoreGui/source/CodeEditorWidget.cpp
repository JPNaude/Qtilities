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

#include "CodeEditorWidget.h"
#include "ui_CodeEditorWidget.h"
#include "CodeEditor.h"

#include "ActionProvider.h"
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ConfigurationWidget.h"

#include <FileUtils>

#include <QFileInfo>
#include <QtGui>

#include <QPrintDialog>
#include <QPrintPreviewDialog>

using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui::Actions;

struct Qtilities::CoreGui::CodeEditorWidgetPrivateData {
    CodeEditorWidgetPrivateData() : actionNew(0),
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
        actionSettings(0),
        codeEditor(0),
        syntax_highlighter(0),
        searchBoxWidget(0),
        action_provider(0),
        removed_outside_policy(CodeEditorWidget::CloseFile){}
    ~CodeEditorWidgetPrivateData() {
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
    QAction* actionSettings;

    //! The file name linked to the contents of the code editor. \sa handleNewSideWidget()
    QString current_file;
    //! The default path used by file dialogs in this editor.
    QString default_path;
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

    //! A file system watcher monitoring the open file.
    QFileSystemWatcher watcher;
    //! Mutex on watcher hanlders.
    QMutex watcher_mutex;
    //! FileRemovedOutsideHandlingPolicy
    CodeEditorWidget::FileRemovedOutsideHandlingPolicy removed_outside_policy;
    //! FileModifiedOutsideHandlingPolicy
    CodeEditorWidget::FileModifiedOutsideHandlingPolicy modified_outside_policy;

};

Qtilities::CoreGui::CodeEditorWidget::CodeEditorWidget(ActionFlags action_flags, DisplayFlags display_flags, QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::CodeEditorWidget)
{
    ui->setupUi(this);
    d = new CodeEditorWidgetPrivateData;
    d->action_flags = action_flags;
    d->display_flags = display_flags;
    d->central_widget = new QWidget();
    setCentralWidget(d->central_widget);
    d->default_path = QtilitiesApplication::applicationSessionPath();

    connect(&d->watcher,SIGNAL(fileChanged(QString)),SLOT(handleFileChangedNotification(QString)));

    // Create the code editor:
    d->codeEditor = new CodeEditor();
    d->codeEditor->installEventFilter(this);
    d->codeEditor->viewport()->installEventFilter(this);
    connect(d->codeEditor,SIGNAL(modificationChanged(bool)),SLOT(setModificationState(bool)));

    // Set the tab width:
    QString tab_width_text = "tabs";
    QFontMetrics fm(d->codeEditor->font());
    d->codeEditor->setTabStopWidth(fm.tightBoundingRect(tab_width_text).width());
    //qDebug() << "fm.tightBoundingRect(tab_width_text).width()" << fm.tightBoundingRect(tab_width_text).width();

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
    setObjectName(context_string);

    initialize();
}

Qtilities::CoreGui::CodeEditorWidget::~CodeEditorWidget() {
    CONTEXT_MANAGER->unregisterContext(d->global_meta_type);
    maybeSave();
    delete d;
    delete ui;
}

bool Qtilities::CoreGui::CodeEditorWidget::eventFilter(QObject *object, QEvent *event) {
    if (object == d->codeEditor && event->type() == QEvent::FocusIn) {
        refreshActions();
        CONTEXT_MANAGER->setNewContext(contextString(),true);
        if (d->action_flags & ActionPaste) {
            // Connect to the paste action
            Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
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
            Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
            if (command) {
                if (command->action())
                    connect(command->action(),SIGNAL(triggered()),d->codeEditor,SLOT(paste()));
            }
        }
    } else if (object == d->codeEditor && event->type() == QEvent::FocusOut) {
        if (d->action_flags & ActionPaste) {
            // Disconnect the paste action from the this widget.
            Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
            if (command) {
                if (command->action())
                    command->action()->disconnect(this);
            }
        }
    } else if (object == d->codeEditor->viewport() && event->type() == QEvent::FocusOut) {
        if (d->action_flags & ActionPaste) {
            // Disconnect the paste action from the this widget.
            Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
            if (command) {
                if (command->action())
                    command->action()->disconnect(this);
            }
        }
    }
    return false;
}

Qtilities::CoreGui::CodeEditorWidget::FileRemovedOutsideHandlingPolicy Qtilities::CoreGui::CodeEditorWidget::fileRemovedOutsideHandlingPolicy() const {
    return d->removed_outside_policy;
}

void Qtilities::CoreGui::CodeEditorWidget::setFileRemovedOutsideHandlingPolicy(Qtilities::CoreGui::CodeEditorWidget::FileRemovedOutsideHandlingPolicy policy) {
    d->removed_outside_policy = policy;
}

Qtilities::CoreGui::CodeEditorWidget::FileModifiedOutsideHandlingPolicy Qtilities::CoreGui::CodeEditorWidget::fileModifiedOutsideHandlingPolicy() const {
    return d->modified_outside_policy;
}

void Qtilities::CoreGui::CodeEditorWidget::setFileModifiedOutsideHandlingPolicy(Qtilities::CoreGui::CodeEditorWidget::FileModifiedOutsideHandlingPolicy policy) {
    d->modified_outside_policy = policy;
}

QString Qtilities::CoreGui::CodeEditorWidget::contextString() const {
    return d->global_meta_type;
}

QString Qtilities::CoreGui::CodeEditorWidget::contextHelpId() const {
    return QString();
}

bool Qtilities::CoreGui::CodeEditorWidget::isModified() const {
    if (d->codeEditor)
        return d->codeEditor->document()->isModified();
    else
        return false;
}

void Qtilities::CoreGui::CodeEditorWidget::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    if (!d->codeEditor)
        return;

    d->codeEditor->document()->setModified(new_state);

    if (notification_targets & IModificationNotifier::NotifyListeners)
        emit modificationStateChanged(new_state);
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

    if (file_name.isEmpty())
        return false;

    // Read everything from the file
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString contents = file.readAll();
    d->codeEditor->document()->setModified(false);
    d->codeEditor->setPlainText(contents);
    setWindowModified(false);

    QString prev_file = d->current_file;
    d->current_file = file_name;
    emit fileNameChanged(d->current_file);

    if (d->watcher.files().contains(prev_file))
        d->watcher.removePath(prev_file);
    d->watcher.addPath(d->current_file);

    return true;
}

void Qtilities::CoreGui::CodeEditorWidget::closeFile() {
    if (d->watcher.files().contains(d->current_file))
        d->watcher.removePath(d->current_file);

    d->current_file.clear();
    emit fileNameChanged(tr("Untitled"));

    d->codeEditor->clear();
    refreshActions();
}

bool Qtilities::CoreGui::CodeEditorWidget::saveFile(QString file_name) {
    if (file_name.isEmpty())
        file_name = d->current_file;

    if (file_name.isEmpty())
        return false;

    if (d->watcher.files().contains(d->current_file))
        d->watcher.removePath(d->current_file);

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly))
        return false;
    file.write(d->codeEditor->toPlainText().toUtf8());
    file.close();

    d->codeEditor->document()->setModified(false);

    d->current_file = file_name;
    emit fileNameChanged(d->current_file);

    d->watcher.addPath(d->current_file);

    updateSaveAction();
    return true;
}

QString Qtilities::CoreGui::CodeEditorWidget::fileName() const {
    return d->current_file;
}

void Qtilities::CoreGui::CodeEditorWidget::setDefaultPath(const QString &file_path) {
    d->default_path = file_path;
}

QString Qtilities::CoreGui::CodeEditorWidget::defaultPath() const {
    return d->default_path;
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

void Qtilities::CoreGui::CodeEditorWidget::actionNew() {
    if (maybeSave()) {
        d->codeEditor->clear();
    }
}

void Qtilities::CoreGui::CodeEditorWidget::actionOpen() {
    QString start_path = d->default_path;

    // If there already a file we open in its path:
    if (!d->current_file.isEmpty()) {
        QFileInfo fi(d->current_file);
        start_path = fi.filePath();
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), start_path, tr("All Files (*)"));
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
    QString start_path = d->default_path;

    // If there already a file we open in its path:
    if (!d->current_file.isEmpty()) {
        QFileInfo fi(d->current_file);
        QDir dir(fi.path());
        if (dir.exists())
            start_path = fi.filePath();
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), start_path, tr("All Files (*)"));
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
    QString start_path = d->default_path;

    // If there already a file we open in its path:
    if (!d->current_file.isEmpty()) {
        QFileInfo fi(d->current_file);
        start_path = fi.filePath();
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export PDF"), start_path, "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).completeSuffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        d->codeEditor->document()->print(&printer);
    }
#endif
}

void Qtilities::CoreGui::CodeEditorWidget::handleSettingsUpdateRequest(const QString& request_id) {
    if (request_id == d->global_meta_type || request_id == QLatin1String("AllCodeEditors")) {
        if (!QtilitiesCoreApplication::qtilitiesSettingsEnabled())
            return;

        // Read the text editor settings from QSettings
        QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
        settings.beginGroup("Qtilities");
        settings.beginGroup("GUI");
        settings.beginGroup("Editors");
        settings.beginGroup("Code Editor Widget");

        QFont font;
        font.setFamily(settings.value("font_type","Courier").toString());
        font.setFixedPitch(true);
        #ifdef Q_OS_WIN
        font.setPointSize(settings.value("font_size",8).toInt());
        #else
        font.setPointSize(settings.value("font_size",10).toInt());
        #endif
        d->codeEditor->setFont(font);

        settings.endGroup();
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
    if (!d->actionSave)
        return;

    if (d->codeEditor->document()->isModified())
        d->actionSave->setEnabled(true);
    else
        d->actionSave->setEnabled(false);
}

void Qtilities::CoreGui::CodeEditorWidget::handleFileChangedNotification(const QString &path) {
    if (!d->watcher_mutex.tryLock())
        return;

    if (FileUtils::comparePaths(d->current_file,path)) {
        // Detect the change that happened:
        // Check if it still exists:
        QFile file(path);
        if (file.exists()) {
            // The contents was modified:
            if (d->modified_outside_policy == PromptForUpdate) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText(tr("Your file has changed outside of the editor:<br><br>") + d->current_file);
                msgBox.setInformativeText(tr("Do you want to reload the file?"));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);
                int ret = msgBox.exec();
                switch (ret) {
                    case QMessageBox::Yes:
                        loadFile(d->current_file);
                        break;
                    case QMessageBox::No:
                        // Since the file is not the same as the saved file anymore, we set it as modified.
                        setModificationState(true);
                        break;
                    default:
                        // should never be reached
                        break;
                }
            } else if (d->modified_outside_policy == AutoUpdate) {
                loadFile(d->current_file);
            } else if (d->modified_outside_policy == AutoIgnore) {
                // Do nothing.
            } else if (d->modified_outside_policy == AutoIgnoreSetModified) {
                setModificationState(true);
            }
        } else {
            // The file was removed:
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setText(tr("Your file has been removed outside of the editor:<br><br>") + d->current_file);
            msgBox.setInformativeText(tr("Do you want to keep this file open in the editor?"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            switch (ret) {
                case QMessageBox::Yes:
                    // Since the file does not exist anymore, we set it as modified.
                    setModificationState(true);
                    break;
                case QMessageBox::No:
                    // We need to either delete the editor or just close the file.
                    if (d->removed_outside_policy == CloseFile) {
                        closeFile();
                    } else if (d->removed_outside_policy == DeleteEditor) {
                        deleteLater();
                    }
                    break;
                default:
                    // should never be reached
                    break;
            }
        }

    } else {
        qDebug() << Q_FUNC_INFO << "Found notification on file not used in editor. Current:" << d->current_file << ", Notification for file" << path;
    }

    d->watcher_mutex.unlock();
}

void Qtilities::CoreGui::CodeEditorWidget::constructActions() {
    if (d->action_provider)
        return;

    d->action_provider = new ActionProvider(this);

    // Get the context ID for this widget:
    int context_id = CONTEXT_MANAGER->registerContext(d->global_meta_type);
    QList<int> context;
    context.push_front(context_id);

    ACTION_MANAGER->commandObserver()->startProcessingCycle();

    // ---------------------------
    // New
    // ---------------------------
    Command* command;
    if (d->action_flags & ActionNew) {
        d->actionNew = new QAction(QIcon(qti_icon_EDIT_CLEAR_16x16),tr("New"),this);
        d->action_provider->addAction(d->actionNew,QtilitiesCategory(tr("File")));
        connect(d->actionNew,SIGNAL(triggered()),SLOT(actionNew()));
        command = ACTION_MANAGER->registerAction(qti_action_FILE_NEW,d->actionNew,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Open
    // ---------------------------
    if (d->action_flags & ActionOpenFile) {
        d->actionOpen = new QAction(QIcon(qti_icon_FILE_OPEN_16x16),tr("Open"),this);
        d->action_provider->addAction(d->actionOpen,QtilitiesCategory(tr("File")));
        connect(d->actionOpen,SIGNAL(triggered()),SLOT(actionOpen()));
        command = ACTION_MANAGER->registerAction(qti_action_FILE_OPEN,d->actionOpen,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Save
    // ---------------------------
    if (d->action_flags & ActionSaveFile) {
        d->actionSave = new QAction(QIcon(qti_icon_FILE_SAVE_16x16),tr("Save"),this);
        d->actionSave->setEnabled(false);
        d->actionSave->setShortcut(QKeySequence(QKeySequence::Save));
        d->action_provider->addAction(d->actionSave,QtilitiesCategory(tr("File")));
        connect(d->actionSave,SIGNAL(triggered()),SLOT(actionSave()));
        command = ACTION_MANAGER->registerAction(qti_action_FILE_SAVE,d->actionSave,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // SaveAs
    // ---------------------------
    if (d->action_flags & ActionSaveFileAs) {
        d->actionSaveAs = new QAction(QIcon(qti_icon_FILE_SAVEAS_16x16),tr("Save As"),this);
        d->actionSaveAs->setShortcut(QKeySequence(QKeySequence::SaveAs));
        d->action_provider->addAction(d->actionSaveAs,QtilitiesCategory(tr("File")));
        connect(d->actionSaveAs,SIGNAL(triggered()),SLOT(actionSaveAs()));
        command = ACTION_MANAGER->registerAction(qti_action_FILE_SAVE_AS,d->actionSaveAs,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Print
    // ---------------------------
    if (d->action_flags & ActionPrint) {
        d->actionPrint = new QAction(QIcon(qti_icon_PRINT_16x16),tr("Print"),this);
        d->action_provider->addAction(d->actionPrint,QtilitiesCategory(tr("Print")));
        connect(d->actionPrint,SIGNAL(triggered()),SLOT(actionPrint()));
        command = ACTION_MANAGER->registerAction(qti_action_FILE_PRINT,d->actionPrint,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // PrintPreview
    // ---------------------------
    if (d->action_flags & ActionPrintPreview) {
        d->actionPrintPreview = new QAction(QIcon(qti_icon_PRINT_PREVIEW_16x16),tr("Print Preview"),this);
        d->action_provider->addAction(d->actionPrintPreview,QtilitiesCategory(tr("Print")));
        connect(d->actionPrintPreview,SIGNAL(triggered()),SLOT(actionPrintPreview()));
        command = ACTION_MANAGER->registerAction(qti_action_FILE_PRINT_PREVIEW,d->actionPrintPreview,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // PrintPDF
    // ---------------------------
    if (d->action_flags & ActionPrintPDF) {
        d->actionPrintPdf = new QAction(QIcon(qti_icon_PRINT_PDF_16x16),tr("Print PDF"),this);
        d->action_provider->addAction(d->actionPrintPdf,QtilitiesCategory(tr("Print")));
        connect(d->actionPrintPdf,SIGNAL(triggered()),SLOT(actionPrintPdf()));
        command = ACTION_MANAGER->registerAction(qti_action_FILE_PRINT_PDF,d->actionPrintPdf,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Undo
    // ---------------------------
    if (d->action_flags & ActionUndo) {
        d->actionUndo = new QAction(QIcon(qti_icon_EDIT_UNDO_16x16),tr("Undo"),this);
        d->action_provider->addAction(d->actionUndo,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionUndo,SIGNAL(triggered()),d->codeEditor,SLOT(undo()));
        command = ACTION_MANAGER->registerAction(qti_action_EDIT_UNDO,d->actionUndo,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Redo
    // ---------------------------
    if (d->action_flags & ActionRedo) {
        d->actionRedo = new QAction(QIcon(qti_icon_EDIT_REDO_16x16),tr("Redo"),this);
        d->action_provider->addAction(d->actionRedo,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionRedo,SIGNAL(triggered()),d->codeEditor,SLOT(redo()));
        command = ACTION_MANAGER->registerAction(qti_action_EDIT_REDO,d->actionRedo,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Cut
    // ---------------------------
    if (d->action_flags & ActionCut) {
        d->actionCut = new QAction(QIcon(qti_icon_EDIT_CUT_16x16),tr("Cut"),this);
        d->action_provider->addAction(d->actionCut,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionCut,SIGNAL(triggered()),d->codeEditor,SLOT(cut()));
        command = ACTION_MANAGER->registerAction(qti_action_EDIT_CUT,d->actionCut,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Copy
    // ---------------------------
    if (d->action_flags & ActionCopy) {
        d->actionCopy = new QAction(QIcon(qti_icon_EDIT_COPY_16x16),tr("Copy"),this);
        d->action_provider->addAction(d->actionCopy,QtilitiesCategory(tr("Clipboard")));
        connect(d->actionCopy,SIGNAL(triggered()),d->codeEditor,SLOT(copy()));
        command = ACTION_MANAGER->registerAction(qti_action_EDIT_COPY,d->actionCopy,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Paste
    // ---------------------------
    if (d->action_flags & ActionPaste) {
        Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
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
        d->actionSelectAll = new QAction(QIcon(qti_icon_EDIT_SELECT_ALL_16x16),tr("Select All"),this);
        d->action_provider->addAction(d->actionSelectAll,QtilitiesCategory(tr("Selection")));
        connect(d->actionSelectAll,SIGNAL(triggered()),d->codeEditor,SLOT(selectAll()));
        command = ACTION_MANAGER->registerAction(qti_action_EDIT_SELECT_ALL,d->actionSelectAll,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Clear
    // ---------------------------
    if (d->action_flags & ActionClear) {
        d->actionClear = new QAction(QIcon(qti_icon_EDIT_CLEAR_16x16),tr("Clear"),this);
        d->action_provider->addAction(d->actionClear,QtilitiesCategory(tr("Selection")));
        connect(d->actionClear,SIGNAL(triggered()),d->codeEditor,SLOT(clear()));
        command = ACTION_MANAGER->registerAction(qti_action_EDIT_CLEAR,d->actionClear,context);
        command->setCategory(QtilitiesCategory("Editing"));
    }
    // ---------------------------
    // Find
    // ---------------------------
    if (d->action_flags & ActionFind) {
        d->actionFind = new QAction(QIcon(qti_icon_FIND_16x16),tr("Find"),this);
        d->action_provider->addAction(d->actionFind,QtilitiesCategory(tr("Selection")));
        connect(d->actionFind,SIGNAL(triggered()),SLOT(showSearchBox()));
        command = ACTION_MANAGER->registerAction(qti_action_EDIT_FIND,d->actionFind,context);
    }
    // ---------------------------
    // Code Editor Settings
    // ---------------------------
    // We create the settings action only if there is a config page registered in QtilitiesApplication and
    // it has the Code Editor page as well.
    if (QtilitiesApplication::configWidget()) {
        ConfigurationWidget* config_widget = qobject_cast<ConfigurationWidget*> (QtilitiesApplication::configWidget());
        if (config_widget) {
            d->actionSettings = new QAction(QIcon(qti_icon_PROPERTY_16x16),tr("Editor Settings"),this);
            d->action_provider->addAction(d->actionSettings,QtilitiesCategory(tr("Editor Settings")));
            ACTION_MANAGER->registerAction(qti_action_FILE_SETTINGS,d->actionSettings,context);
            connect(d->actionSettings,SIGNAL(triggered()),SLOT(showEditorSettings()));
        }
    }

    if (d->actionCut)
        connect(d->codeEditor, SIGNAL(copyAvailable(bool)), d->actionCut, SLOT(setEnabled(bool)));
    if (d->actionCopy)
        connect(d->codeEditor, SIGNAL(copyAvailable(bool)), d->actionCopy, SLOT(setEnabled(bool)));
    if (d->actionUndo)
        connect(d->codeEditor, SIGNAL(undoAvailable(bool)), d->actionUndo, SLOT(setEnabled(bool)));
    if (d->actionRedo)
        connect(d->codeEditor, SIGNAL(redoAvailable(bool)), d->actionRedo, SLOT(setEnabled(bool)));
    if (d->actionSave)
        connect(d->codeEditor,SIGNAL(modificationChanged(bool)),d->actionSave,SLOT(setEnabled(bool)));

    ACTION_MANAGER->commandObserver()->endProcessingCycle(false);
}

void Qtilities::CoreGui::CodeEditorWidget::refreshActions() {
    // Update actions
    if (d->actionUndo)
        d->actionUndo->setEnabled(d->codeEditor->document()->isUndoAvailable());
    if (d->actionRedo)
        d->actionRedo->setEnabled(d->codeEditor->document()->isRedoAvailable());
}

void Qtilities::CoreGui::CodeEditorWidget::showEditorSettings() {
    ConfigurationWidget* config_widget = qobject_cast<ConfigurationWidget*> (QtilitiesApplication::configWidget());
    if (config_widget) {
        if (config_widget->hasPage(tr(qti_config_page_CODE_EDITORS))) {
            config_widget->setActivePage(tr(qti_config_page_CODE_EDITORS));
            config_widget->show();
        } else
            LOG_ERROR_P("Cannot open editor settings, the editor settings page is not available in your application.");
    }
}

void Qtilities::CoreGui::CodeEditorWidget::setDisplayFlagsHint(DisplayFlags display_flags) {
    d->display_flags = display_flags;
    initialize();
}

Qtilities::CoreGui::CodeEditorWidget::ActionFlags Qtilities::CoreGui::CodeEditorWidget::actionFlagsHint() const {
    return d->action_flags;
}

void Qtilities::CoreGui::CodeEditorWidget::setActionFlagsHint(ActionFlags action_flags) {
    d->action_flags = action_flags;
    initialize();
}

void Qtilities::CoreGui::CodeEditorWidget::initialize() {
    // Create actions only after global meta type was set.
    constructActions();
    refreshActionToolBar(true);

    // Check if we must connect to the paste action for the new hints:
    Command* command = ACTION_MANAGER->command(qti_action_EDIT_PASTE);
    if (command) {
        if (command->action()) {
            if (d->action_flags & ActionPaste)
                connect(command->action(),SIGNAL(triggered()),d->codeEditor,SLOT(paste()));
            else
                command->action()->disconnect();
        }
    }
}

void Qtilities::CoreGui::CodeEditorWidget::refreshActionToolBar(bool force_full_refresh) {
    // Check if an action toolbar should be created:
    if (d->display_flags & ActionToolBar && d->action_provider) {
        if (!force_full_refresh) {
            // Here we need to hide all toolbars that does not contain any actions:
            // Now create all toolbars:
            for (int i = 0; i < d->action_toolbars.count(); ++i) {
                QToolBar* toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(i));
                bool has_visible_action = false;
                foreach (QAction* action, toolbar->actions()) {
                    if (action->isVisible()) {
                        has_visible_action = true;
                        break;
                    }
                }
                if (!has_visible_action)
                    toolbar->hide();
                else
                    toolbar->show();
            }
            return;
        }

        // First delete all toolbars:
        int toolbar_count = d->action_toolbars.count();
        if (toolbar_count > 0) {
            for (int i = 0; i < toolbar_count; ++i) {
                QToolBar* toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(0));
                removeToolBar(toolbar);
                if (toolbar) {
                    d->action_toolbars.removeOne(toolbar);
                    delete toolbar;
                }
            }
        }

        // Now create all toolbars:
        QList<QtilitiesCategory> categories = d->action_provider->actionCategories();
        for (int i = 0; i < categories.count(); ++i) {
            QList<QAction*> action_list = d->action_provider->actions(IActionProvider::NoFilter,categories.at(i));
            if (action_list.count() > 0) {
                QToolBar* new_toolbar = 0;

                QList<QToolBar *> toolbars = findChildren<QToolBar *>();
                foreach (QToolBar* toolbar, toolbars) {
                    if (toolbar->objectName() == categories.at(i).toString()) {
                        new_toolbar = toolbar;
                        break;
                    }
                }

                if (!new_toolbar) {
                    new_toolbar = addToolBar(categories.at(i).toString());
                    d->action_toolbars << new_toolbar;
                }
                new_toolbar->setObjectName(categories.at(i).toString());
                new_toolbar->addActions(action_list);
            }
        }

        // Here we need to hide all toolbars that does not contain any actions:
        // This implementation will only hide empty toolbars, they will still be there when right clicking on the toolbar.
        // However, this is the best solution I believe since the user can still see which toolbars are available, and
        // then realize that no actions are available for the current selection in hidden toolbars.
        for (int i = 0; i < d->action_toolbars.count(); ++i) {
            QToolBar* toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(i));
            bool has_visible_action = false;
            foreach (QAction* action, toolbar->actions()) {
                if (action->isVisible()) {
                    has_visible_action = true;
                    break;
                }
            }
            if (!has_visible_action)
                toolbar->hide();
            else
                toolbar->show();
        }
    } else {
        d->display_flags = CodeEditorWidget::NoDisplayFlagsHint;
        deleteActionToolBars();
    }
}

void Qtilities::CoreGui::CodeEditorWidget::deleteActionToolBars() {
    int toolbar_count = d->action_toolbars.count();
    if (toolbar_count > 0) {
        for (int i = 0; i < toolbar_count; ++i) {
            QPointer<QToolBar> toolbar = qobject_cast<QToolBar*> (d->action_toolbars.at(0));
            removeToolBar(toolbar);
            if (toolbar) {
                d->action_toolbars.removeOne(toolbar);
                delete toolbar;
            }
        }
    }
}
