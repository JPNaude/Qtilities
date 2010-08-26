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

#include "WidgetLoggerEngineFrontend.h"
#include "LoggingConstants.h"

#include <QtilitiesApplication.h>
#include <QtilitiesCoreGuiConstants.h>

#include <QScrollBar>
#include <Logger.h>
#include <QMetaObject>
#include <QPrinter>
#include <QPrintDialog>
#include <QAbstractPrintDialog>
#include <QFileDialog>
#include <QAction>
#include <QShortcut>
#include <QKeySequence>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QPrintPreviewDialog>
#include <QApplication>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Constants;

QPointer<Qtilities::CoreGui::WidgetLoggerEngineFrontend> Qtilities::CoreGui::WidgetLoggerEngineFrontend::currentWidget;
QPointer<Qtilities::CoreGui::WidgetLoggerEngineFrontend> Qtilities::CoreGui::WidgetLoggerEngineFrontend::actionContainerWidget;

struct Qtilities::CoreGui::WidgetLoggerEngineFrontendData {
    WidgetLoggerEngineFrontendData() : searchBoxWidget(0),
    txtLog(0),
    actionCopy(0),
    actionSelectAll(0),
    actionClear(0),
    actionPrint(0),
    actionPrintPDF(0),
    actionPrintPreview(0),
    actionSave(0),
    actionFind(0),
    actionSettings(0) {}

    SearchBoxWidget* searchBoxWidget;
    QPlainTextEdit txtLog;
    QAction* actionCopy;
    QAction* actionSelectAll;
    QAction* actionClear;
    QAction* actionPrint;
    QAction* actionPrintPDF;
    QAction* actionPrintPreview;
    QAction* actionSave;
    QAction* actionFind;
    QAction* actionSettings;
};

Qtilities::CoreGui::WidgetLoggerEngineFrontend::WidgetLoggerEngineFrontend(QWidget *parent) : QWidget(parent)
{
    d = new WidgetLoggerEngineFrontendData;

    // Setup search box widget:
    SearchBoxWidget::SearchOptions search_options = 0;
    search_options |= SearchBoxWidget::CaseSensitive;
    search_options |= SearchBoxWidget::WholeWordsOnly;
    search_options |= SearchBoxWidget::RegEx;
    d->searchBoxWidget = new SearchBoxWidget(search_options);
    d->searchBoxWidget->setWholeWordsOnly(false);

    // Setup the log widget:
    d->txtLog.setReadOnly(true);
    d->txtLog.setFont(QFont("Courier New"));
    d->txtLog.setMaximumBlockCount(1000);

    // Setup the widget layout:
    if (layout())
        delete layout();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(&d->txtLog);
    layout->addWidget(d->searchBoxWidget);
    d->txtLog.show();
    d->searchBoxWidget->show();

    connect(d->searchBoxWidget,SIGNAL(searchStringChanged(const QString)),SLOT(handleSearchStringChanged(QString)));
    connect(d->searchBoxWidget,SIGNAL(searchOptionsChanged()),SLOT(handle_FindNext()));
    connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),d->searchBoxWidget,SLOT(hide()));
    connect(d->searchBoxWidget,SIGNAL(btnFindNext_clicked()),SLOT(handle_FindNext()));
    connect(d->searchBoxWidget,SIGNAL(btnFindPrevious_clicked()),SLOT(handle_FindPrevious()));
    d->searchBoxWidget->setEditorFocus();
    setAttribute(Qt::WA_DeleteOnClose, true);

    constructActions();

    d->txtLog.installEventFilter(this);
    currentWidget = this;
}

Qtilities::CoreGui::WidgetLoggerEngineFrontend::~WidgetLoggerEngineFrontend()
{
    delete d;
}

bool Qtilities::CoreGui::WidgetLoggerEngineFrontend::eventFilter(QObject *object, QEvent *event) {
    if (object == &d->txtLog && event->type() == QEvent::FocusIn) {
        currentWidget = this;
        CONTEXT_MANAGER->setNewContext(CONTEXT_LOGGER_WIDGET,true);
    }
    return false;
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::makeCurrentWidget() {
    currentWidget = this;
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::appendMessage(const QString& message) {
    d->txtLog.appendPlainText(message);
    d->txtLog.verticalScrollBar()->setValue(d->txtLog.verticalScrollBar()->maximum()) ;
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_FindPrevious() {
    QTextDocument::FindFlags find_flags = 0;
    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QTextDocument::FindCaseSensitively;
    if (d->searchBoxWidget->wholeWordsOnly())
        find_flags |= QTextDocument::FindWholeWords;
    find_flags |= QTextDocument::FindBackward;
    d->txtLog.find(d->searchBoxWidget->currentSearchString(),find_flags);
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handleSearchStringChanged(const QString& filter_string) {
    QTextDocument::FindFlags find_flags = 0;
    if (d->searchBoxWidget->wholeWordsOnly())
        find_flags |= QTextDocument::FindWholeWords;
    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QTextDocument::FindCaseSensitively;

    d->txtLog.find(filter_string,find_flags | QTextDocument::FindBackward);
    d->txtLog.find(filter_string,find_flags);
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_FindNext() {
    QTextDocument::FindFlags find_flags = 0;
    if (d->searchBoxWidget->caseSensitive())
        find_flags |= QTextDocument::FindCaseSensitively;
    if (d->searchBoxWidget->wholeWordsOnly())
        find_flags |= QTextDocument::FindWholeWords;
    d->txtLog.find(d->searchBoxWidget->currentSearchString(),find_flags);
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Save() {
    if (!currentWidget)
        return;

    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Log"),QApplication::applicationDirPath(),tr("Log File (*.log)"));

    if (file_name.isEmpty())
        return;

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << currentWidget->d->txtLog.toPlainText() << "\n";
    file.close();
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Settings() {
    if (!currentWidget)
        return;

    /*TRACK QWidget* widget = Log->configWidget();
    if (widget)
        widget->show();*/
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Print() {
    if (!currentWidget)
        return;

#ifndef QT_NO_PRINTER
     QPrinter printer;

     QPrintDialog *dialog = new QPrintDialog(&printer, this);
     dialog->setWindowTitle(tr("Print Current Log"));
     if (currentWidget->d->txtLog.textCursor().hasSelection())
         dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
     if (dialog->exec() != QDialog::Accepted)
         return;

     delete dialog;
     currentWidget->d->txtLog.print(&printer);
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_PrintPDF() {
    if (!currentWidget)
        return;

#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export PDF"),
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        currentWidget->d->txtLog.document()->print(&printer);
    }
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_PrintPreview() {
    if (!currentWidget)
        return;

#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, &currentWidget->d->txtLog);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    currentWidget->d->txtLog.print(printer);
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Clear() {
    if (!currentWidget)
        return;

    currentWidget->d->txtLog.clear();
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Copy() {
    if (!currentWidget)
        return;

    currentWidget->d->txtLog.copy();
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_SearchShortcut() {
    if (!currentWidget)
        return;

    if (currentWidget->d->searchBoxWidget) {
        if (!currentWidget->d->searchBoxWidget->isVisible()) {
            currentWidget->d->searchBoxWidget->show();
            currentWidget->d->searchBoxWidget->setEditorFocus();
        }
    }
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_SelectAll() {
    if (!currentWidget)
        return;

    currentWidget->d->txtLog.selectAll();
}


void Qtilities::CoreGui::WidgetLoggerEngineFrontend::constructActions() {
    if (actionContainerWidget)
        return;
    actionContainerWidget = this;

    CONTEXT_MANAGER->registerContext(CONTEXT_LOGGER_WIDGET);
    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(CONTEXT_LOGGER_WIDGET));

    // ---------------------------
    // Save
    // ---------------------------
    d->actionSave = new QAction(QIcon(ICON_SAVE_24x24),tr("Save"),this);
    connect(d->actionSave,SIGNAL(triggered()),SLOT(handle_Save()));
    ACTION_MANAGER->registerAction(MENU_FILE_SAVE,d->actionSave,context);
    // ---------------------------
    // Print
    // ---------------------------
    d->actionPrint = new QAction(QIcon(),tr("Print"),this);
    connect(d->actionPrint,SIGNAL(triggered()),SLOT(handle_Print()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT,d->actionPrint,context);
    // ---------------------------
    // Copy
    // ---------------------------
    d->actionCopy = new QAction(QIcon(),tr("Copy"),this);
    connect(d->actionCopy,SIGNAL(triggered()),SLOT(handle_Copy()));
    ACTION_MANAGER->registerAction(MENU_EDIT_COPY,d->actionCopy,context);
    // ---------------------------
    // Select All
    // ---------------------------
    d->actionSelectAll = new QAction(QIcon(),tr("Select All"),this);
    d->actionSelectAll->setEnabled(true);
    connect(d->actionSelectAll,SIGNAL(triggered()),SLOT(handle_SelectAll()));
    ACTION_MANAGER->registerAction(MENU_EDIT_SELECT_ALL,d->actionSelectAll,context);
    // ---------------------------
    // Clear
    // ---------------------------
    d->actionClear = new QAction(QIcon(ICON_CLEAR_24x24),tr("Clear"),this);
    connect(d->actionClear,SIGNAL(triggered()),SLOT(handle_Clear()));
    ACTION_MANAGER->registerAction(MENU_EDIT_CLEAR,d->actionClear,context);
    // ---------------------------
    // Find
    // ---------------------------
    d->actionFind = new QAction(QIcon(),tr("Find"),this);
    connect(d->actionFind,SIGNAL(triggered()),SLOT(handle_SearchShortcut()));
    ACTION_MANAGER->registerAction(MENU_EDIT_FIND,d->actionFind,context);
    // ---------------------------
    // Print PDF
    // ---------------------------
    d->actionPrintPDF = new QAction(QIcon(),tr("Print PDF"),this);
    connect(d->actionPrintPDF,SIGNAL(triggered()),SLOT(handle_PrintPDF()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PDF,d->actionPrintPDF,context);
    // ---------------------------
    // Print Preview
    // ---------------------------
    d->actionPrintPreview = new QAction(QIcon(),tr("Print Preview"),this);
    connect(d->actionPrintPreview,SIGNAL(triggered()),SLOT(handle_PrintPreview()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PREVIEW,d->actionPrintPreview,context);
    // ---------------------------
    // Logger Settings
    // ---------------------------
    d->actionSettings = new QAction(QIcon(ICON_PROPERTY),tr("Settings"),this);
    connect(d->actionSettings,SIGNAL(triggered()),SLOT(handle_Settings()));

    // Add actions to text edit.
    d->txtLog.addAction(d->actionClear);
    d->txtLog.addAction(d->actionSave);
    QAction* sep1 = new QAction("",0);
    sep1->setSeparator(true);
    d->txtLog.addAction(sep1);
    d->txtLog.addAction(d->actionFind);
    QAction* sep2 = new QAction("",0);
    sep2->setSeparator(true);
    d->txtLog.addAction(sep2);
    //d->txtLog.addAction(d->actionSettings);
    d->txtLog.setContextMenuPolicy(Qt::ActionsContextMenu);
}

