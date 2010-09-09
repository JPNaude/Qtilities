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
#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"
#include "ActionProvider.h"
#include "ConfigurationWidget"

#include <Logger.h>

#include <QScrollBar>
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
#include <QToolBar>

using namespace Qtilities::Core;
using namespace Qtilities::CoreGui;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::CoreGui::Actions;
using namespace Qtilities::CoreGui::Constants;

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
    actionSettings(0),
    central_widget(0) {}

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

    //! The IActionProvider interface implementation.
    ActionProvider* action_provider;
    //! The action toolbars list. Contains toolbars created for each category in the action provider.
    QList<QToolBar*> action_toolbars;
    //! The global meta type string used for this widget.
    QString global_meta_type;
    //! This is the widget for the plain text editor and search box widget.
    QWidget* central_widget;
};

Qtilities::CoreGui::WidgetLoggerEngineFrontend::WidgetLoggerEngineFrontend(QWidget *parent) : QMainWindow(parent)
{
    d = new WidgetLoggerEngineFrontendData;
    d->action_provider = new ActionProvider(this);

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
    d->central_widget = new QWidget();
    if (d->central_widget->layout())
        delete d->central_widget->layout();

    QVBoxLayout* layout = new QVBoxLayout(d->central_widget);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(&d->txtLog);
    layout->addWidget(d->searchBoxWidget);
    d->txtLog.show();
    d->searchBoxWidget->show();
    setCentralWidget(d->central_widget);

    connect(d->searchBoxWidget,SIGNAL(searchStringChanged(const QString)),SLOT(handleSearchStringChanged(QString)));
    connect(d->searchBoxWidget,SIGNAL(searchOptionsChanged()),SLOT(handle_FindNext()));
    connect(d->searchBoxWidget,SIGNAL(btnClose_clicked()),d->searchBoxWidget,SLOT(hide()));
    connect(d->searchBoxWidget,SIGNAL(btnFindNext_clicked()),SLOT(handle_FindNext()));
    connect(d->searchBoxWidget,SIGNAL(btnFindPrevious_clicked()),SLOT(handle_FindPrevious()));
    d->searchBoxWidget->setEditorFocus();
    setAttribute(Qt::WA_DeleteOnClose, true);

    // Assign a default meta type for this widget:
    // We construct each action and then register it
    QString context_string = "WidgetLoggerEngineFrontend";
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
    LOG_ERROR(d->global_meta_type);
    setObjectName(context_string);

    // Construct actions only after global meta type was set.
    constructActions();
    QList<QtilitiesCategory> categories = d->action_provider->actionCategories();
    for (int i = 0; i < categories.count(); i++) {
        QList<QAction*> action_list = d->action_provider->actions(IActionProvider::FilterHidden,categories.at(i));
        if (action_list.count() > 0) {
            QToolBar* new_toolbar = addToolBar(categories.at(i).toString());
            d->action_toolbars << new_toolbar;
            new_toolbar->addActions(action_list);
        }
    }

    d->txtLog.installEventFilter(this);
}

Qtilities::CoreGui::WidgetLoggerEngineFrontend::~WidgetLoggerEngineFrontend()
{
    delete d;
}

bool Qtilities::CoreGui::WidgetLoggerEngineFrontend::eventFilter(QObject *object, QEvent *event) {
    if (object == &d->txtLog && event->type() == QEvent::FocusIn) {
        CONTEXT_MANAGER->setNewContext(d->global_meta_type,true);
    }
    return false;
}

bool Qtilities::CoreGui::WidgetLoggerEngineFrontend::setGlobalMetaType(const QString& meta_type) {
    // Check if this global meta type is allowed.
    if (CONTEXT_MANAGER->hasContext(meta_type))
        return false;

    d->global_meta_type = meta_type;
    return true;
}

QString Qtilities::CoreGui::WidgetLoggerEngineFrontend::globalMetaType() const {
    return d->global_meta_type;
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
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Log"),QApplication::applicationDirPath(),tr("Log File (*.log)"));

    if (file_name.isEmpty())
        return;

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << d->txtLog.toPlainText() << "\n";
    file.close();
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Settings() {
    ConfigurationWidget* config_widget = qobject_cast<ConfigurationWidget*> (QtilitiesApplication::configWidget());
    if (config_widget) {
        config_widget->setActivePage("Logging");
        config_widget->show();
    }
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Print() {
#ifndef QT_NO_PRINTER
     QPrinter printer;

     QPrintDialog *dialog = new QPrintDialog(&printer, this);
     dialog->setWindowTitle(tr("Print Current Log"));
     if (d->txtLog.textCursor().hasSelection())
         dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
     if (dialog->exec() != QDialog::Accepted)
         return;

     delete dialog;
     d->txtLog.print(&printer);
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_PrintPDF() {
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export PDF"), QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        d->txtLog.document()->print(&printer);
    }
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_PrintPreview() {
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, &d->txtLog);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::printPreview(QPrinter *printer) {
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    d->txtLog.print(printer);
#endif
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Clear() {
    d->txtLog.clear();
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_Copy() {
    d->txtLog.copy();
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_SearchShortcut() {
    if (d->searchBoxWidget) {
        if (!d->searchBoxWidget->isVisible()) {
            d->searchBoxWidget->show();
            d->searchBoxWidget->setEditorFocus();
        }
    }
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::handle_SelectAll() {
    d->txtLog.selectAll();
}

void Qtilities::CoreGui::WidgetLoggerEngineFrontend::constructActions() {
    int context_id = CONTEXT_MANAGER->registerContext(d->global_meta_type);
    QList<int> context;
    context.push_front(context_id);

    // ---------------------------
    // Save
    // ---------------------------
    d->actionSave = new QAction(QIcon(ICON_FILE_SAVE_16x16),tr("Save"),this);
    d->action_provider->addAction(d->actionSave,QtilitiesCategory(tr("Log")));
    connect(d->actionSave,SIGNAL(triggered()),SLOT(handle_Save()));
    ACTION_MANAGER->registerAction(MENU_FILE_SAVE,d->actionSave,context);
    // ---------------------------
    // Copy
    // ---------------------------
    d->actionCopy = new QAction(QIcon(ICON_EDIT_COPY_16x16),tr("Copy"),this);
    d->action_provider->addAction(d->actionCopy,QtilitiesCategory(tr("Log")));
    connect(d->actionCopy,SIGNAL(triggered()),SLOT(handle_Copy()));
    ACTION_MANAGER->registerAction(MENU_EDIT_COPY,d->actionCopy,context);
    // ---------------------------
    // Select All
    // ---------------------------
    d->actionSelectAll = new QAction(QIcon(ICON_EDIT_SELECT_ALL_16x16),tr("Select All"),this);
    d->actionSelectAll->setEnabled(true);
    d->action_provider->addAction(d->actionSelectAll,QtilitiesCategory(tr("Log")));
    connect(d->actionSelectAll,SIGNAL(triggered()),SLOT(handle_SelectAll()));
    ACTION_MANAGER->registerAction(MENU_EDIT_SELECT_ALL,d->actionSelectAll,context);
    // ---------------------------
    // Clear
    // ---------------------------
    d->actionClear = new QAction(QIcon(ICON_EDIT_CLEAR_16x16),tr("Clear"),this);
    d->action_provider->addAction(d->actionClear,QtilitiesCategory(tr("Log")));
    connect(d->actionClear,SIGNAL(triggered()),SLOT(handle_Clear()));
    ACTION_MANAGER->registerAction(MENU_EDIT_CLEAR,d->actionClear,context);
    // ---------------------------
    // Find
    // ---------------------------
    d->actionFind = new QAction(QIcon(ICON_FIND_16x16),tr("Find"),this);
    //d->actionFind->setShortcut(QKeySequence(QKeySequence::Find));
    d->action_provider->addAction(d->actionFind,QtilitiesCategory(tr("Log")));
    connect(d->actionFind,SIGNAL(triggered()),SLOT(handle_SearchShortcut()));
    ACTION_MANAGER->registerAction(MENU_EDIT_FIND,d->actionFind,context);
    // ---------------------------
    // Print
    // ---------------------------
    d->actionPrint = new QAction(QIcon(ICON_PRINT_16x16),tr("Print"),this);
    d->action_provider->addAction(d->actionPrint,QtilitiesCategory(tr("Print")));
    connect(d->actionPrint,SIGNAL(triggered()),SLOT(handle_Print()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT,d->actionPrint,context);
    // ---------------------------
    // Print PDF
    // ---------------------------
    d->actionPrintPDF = new QAction(QIcon(ICON_PRINT_PDF_16x16),tr("Print PDF"),this);
    d->action_provider->addAction(d->actionPrintPDF,QtilitiesCategory(tr("Print")));
    connect(d->actionPrintPDF,SIGNAL(triggered()),SLOT(handle_PrintPDF()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PDF,d->actionPrintPDF,context);
    // ---------------------------
    // Print Preview
    // ---------------------------
    d->actionPrintPreview = new QAction(QIcon(ICON_PRINT_PREVIEW_16x16),tr("Print Preview"),this);
    d->action_provider->addAction(d->actionPrintPreview,QtilitiesCategory(tr("Print")));
    connect(d->actionPrintPreview,SIGNAL(triggered()),SLOT(handle_PrintPreview()));
    ACTION_MANAGER->registerAction(MENU_FILE_PRINT_PREVIEW,d->actionPrintPreview,context);
    // ---------------------------
    // Logger Settings
    // ---------------------------
    // We create the settings action only if there is a config page registered in QtilitiesApplication.
    if (QtilitiesApplication::configWidget()) {
        d->actionSettings = new QAction(QIcon(ICON_PROPERTY_16x16),tr("Logging Settings"),this);
        d->action_provider->addAction(d->actionSettings,QtilitiesCategory(tr("Log")));
        ACTION_MANAGER->registerAction(MENU_FILE_SETTINGS,d->actionSettings,context);
        connect(d->actionSettings,SIGNAL(triggered()),SLOT(handle_Settings()));
    }

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

