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

#include "LoggerConfigWidget.h"
#include "ui_LoggerConfigWidget.h"
#include "LoggerEnginesTableModel.h"
#include "QtilitiesCoreGuiConstants.h"
#include "WidgetLoggerEngine.h"
#include "CodeEditorWidget.h"

#include <LoggerEngines>
#include <Logger>
#include <LoggingConstants>
#include <AbstractLoggerEngine>
#include <QtilitiesApplication>

#include <QString>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QFileDialog>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Logging;
using namespace Qtilities::Logging::Constants;

struct Qtilities::CoreGui::LoggerConfigWidgetPrivateData {
    LoggerConfigWidgetPrivateData() : active_engine(0) {}

    qti_private_LoggerEnginesTableModel logger_engine_model;
    AbstractLoggerEngine* active_engine;
};

Qtilities::CoreGui::LoggerConfigWidget::LoggerConfigWidget(bool applyButtonVisisble, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerConfigWidget)
{
    ui->setupUi(this);
    ui->btnApply->setVisible(applyButtonVisisble);
    d = new LoggerConfigWidgetPrivateData;
}

Qtilities::CoreGui::LoggerConfigWidget::~LoggerConfigWidget()
{
    delete ui;
    delete d;
}

QIcon Qtilities::CoreGui::LoggerConfigWidget::configPageIcon() const {
    return QIcon(qti_icon_CONFIG_LOGGING_48x48);
}

QWidget* Qtilities::CoreGui::LoggerConfigWidget::configPageWidget() {
    return this;
}

QString Qtilities::CoreGui::LoggerConfigWidget::configPageTitle() const {
    return tr(qti_config_page_LOGGING);
}

QtilitiesCategory Qtilities::CoreGui::LoggerConfigWidget::configPageCategory() const {
    if (IConfigPage::configPageCategory().isEmpty())
        return QtilitiesCategory(tr(qti_config_page_DEFAULT_CAT));
    else
        return IConfigPage::configPageCategory();
}

void Qtilities::CoreGui::LoggerConfigWidget::configPageApply() {
    handle_BtnApplyClicked();
}

void Qtilities::CoreGui::LoggerConfigWidget::configPageInitialize() {
    ui->txtFormattingEnginePreview->setFont(QFont("Courier New"));

    connect(ui->tableViewLoggerEngines,SIGNAL(clicked(QModelIndex)),SLOT(handle_LoggerEngineTableClicked(QModelIndex)));
    connect(ui->btnAddLoggerEngine,SIGNAL(clicked(bool)),SLOT(handle_NewLoggerEngineRequest()));
    connect(ui->btnRemoveLoggerEngine,SIGNAL(clicked(bool)),SLOT(handle_RemoveLoggerEngineRequest()));
    connect(ui->checkBoxToggleAll,SIGNAL(clicked(bool)),SLOT(handle_CheckBoxToggleAllClicked(bool)));
    connect(ui->listWidgetFormattingEngines,SIGNAL(currentRowChanged(int)),SLOT(handle_FormattingEnginesCurrentRowChanged(int)));
    connect(ui->comboBoxLoggerFormattingEngine,SIGNAL(currentIndexChanged(int)),SLOT(handle_ComboBoxLoggerFormattingEngineCurrentIndexChange(int)));
    connect(ui->btnLoadConfig,SIGNAL(clicked()),SLOT(handle_BtnLoadConfigClicked()));
    connect(ui->btnSaveConfig,SIGNAL(clicked()),SLOT(handle_BtnSaveConfigClicked()));
    connect(ui->btnApply,SIGNAL(clicked()),SLOT(handle_BtnApplyClicked()));
    connect(ui->tableViewLoggerEngines->verticalHeader(),SIGNAL(sectionCountChanged(int,int)),SLOT(resizeCommandTableRows()));

    connect(Log,SIGNAL(loggerEngineCountChanged(AbstractLoggerEngine*,Logger::EngineChangeIndication)),SLOT(resizeCommandTableRows()));

    // Add log levels:
    QStringList list = Log->allLogLevelStrings();
    list.pop_back();
    ui->comboGlobalLogLevel->addItems(list);

    // Add logger engines:
    ui->tableViewLoggerEngines->setModel(&d->logger_engine_model);
    ui->tableViewLoggerEngines->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewLoggerEngines->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewLoggerEngines->setSortingEnabled(true);
    ui->tableViewLoggerEngines->verticalHeader()->setVisible(false);
    for (int i = 0; i < Log->attachedLoggerEngineCount(); ++i)
        ui->tableViewLoggerEngines->setRowHeight(i,17);
    ui->tableViewLoggerEngines->horizontalHeader()->setStretchLastSection(true);

    // Add formatting engines:
    ui->listWidgetFormattingEngines->addItems(Log->availableFormattingEnginesInFactory());
    ui->comboBoxLoggerFormattingEngine->addItems(Log->availableFormattingEnginesInFactory());

    // Read the logging settings
    readSettings();

    connect(ui->comboGlobalLogLevel,SIGNAL(currentIndexChanged(QString)),SLOT(handle_ComboBoxGlobalLogLevelCurrentIndexChange(QString)));
    connect(ui->checkBoxRememberSession,SIGNAL(clicked(bool)),SLOT(handle_CheckBoxRememberSessionConfigClicked(bool)));

    updateActiveEngine();
}

void Qtilities::CoreGui::LoggerConfigWidget::setApplyButtonVisible(bool visible) {
    ui->btnApply->setVisible(visible);
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_NewLoggerEngineRequest() {
    bool ok;
    QString new_item_selection = QInputDialog::getItem(this, tr("What type of logger engine would you like to add?"),tr("Available Logger Engines:"), Log->availableLoggerEnginesInFactory(), 0, false, &ok);
    if (!ok)
        return;
    QString engine_name = QInputDialog::getText(this, tr("Name of new engine:"),tr("Engine Name:"), QLineEdit::Normal, "new_logger_engine", &ok);

    if (ok && !new_item_selection.isEmpty() && !engine_name.isEmpty()) {
        // Handle new widget
        if (new_item_selection == QString(qti_def_FACTORY_TAG_FILE_LOGGER_ENGINE)) {
            // Prompt the correct file extensions and select the formatting engine according to the user's selection.
            QString file_ext = "";
            for (int i = 0; i < Log->availableFormattingEnginesInFactory().count(); ++i) {
                AbstractFormattingEngine* engine = Log->formattingEngineReference(Log->availableFormattingEnginesInFactory().at(i));
                if (engine) {
                    if ((!engine->fileExtension().isEmpty()) && (!engine->name().isEmpty()))
                        file_ext.append(QString("%1 (*.%2);;").arg(engine->name()).arg(engine->fileExtension()));
                }
            }

            QString fileName = QFileDialog::getSaveFileName(this,tr("Select Output File"),QtilitiesApplication::applicationSessionPath(),file_ext);
            if (!fileName.isEmpty()) {
                Log->newFileEngine(engine_name,fileName,QString());
            }
        }
    }
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_RemoveLoggerEngineRequest() {
    if (Log->detachLoggerEngine(d->active_engine))
        updateActiveEngine();
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_LoggerEngineTableClicked(const QModelIndex& index) {
    Q_UNUSED(index)
    updateActiveEngine();
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_FormattingEnginesCurrentRowChanged(int currentRow) {
    AbstractFormattingEngine* engine = Log->formattingEngineReferenceAt(currentRow);
    if (engine) {
        // We prepare a formatted message to show how the message will look:
        QString preview_string;
        preview_string.append(engine->initializeString());
        if (!engine->initializeString().isEmpty())
            preview_string.append(engine->endOfLineChar());
        QList<QVariant> message;
        message.push_front(QVariant(tr("Information Message Example")));
        preview_string.append(engine->formatMessage(Logger::Info,message));
        preview_string.append(engine->endOfLineChar());
        message.clear();
        message.push_front(QVariant(tr("Warning Message Example")));
        preview_string.append(engine->formatMessage(Logger::Warning,message));
        preview_string.append(engine->endOfLineChar());
        message.clear();
        message.push_front(QVariant(tr("Error Message Example")));
        preview_string.append(engine->formatMessage(Logger::Error,message));
        preview_string.append(engine->endOfLineChar());
        message.clear();
        message.push_front(QVariant(tr("Fatal Message Example")));
        preview_string.append(engine->formatMessage(Logger::Fatal,message));
        preview_string.append(engine->endOfLineChar());
        message.clear();
        message.push_front(QVariant(tr("Debug Message Example")));
        preview_string.append(engine->formatMessage(Logger::Debug,message));
        preview_string.append(engine->endOfLineChar());
        message.clear();
        message.push_front(QVariant(tr("Trace Message Example")));
        preview_string.append(engine->formatMessage(Logger::Trace,message));
        preview_string.append(engine->endOfLineChar());
        if (!engine->finalizeString().isEmpty())
            preview_string.append(engine->endOfLineChar());
        preview_string.append(engine->finalizeString());
        ui->txtFormattingEnginePreview->setText(preview_string);
    } else {
        ui->txtFormattingEnginePreview->clear();
    }
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_ComboBoxLoggerFormattingEngineCurrentIndexChange(int index) {
    // We need to get the selected formatting engine and change the active logger engine.
    AbstractFormattingEngine* new_formatting_engine = Log->formattingEngineReferenceAt(index);
    if (new_formatting_engine && d->active_engine) {
        d->active_engine->installFormattingEngine(new_formatting_engine);
    }
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_CheckBoxToggleAllClicked(bool checked) {
    if (checked) {
        Log->enableAllLoggerEngines();
    } else {
        Log->disableAllLoggerEngines();
    }
    d->logger_engine_model.requestRefresh();
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_CheckBoxRememberSessionConfigClicked(bool checked) {
    Log->setRememberSessionConfig(checked);
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_ComboBoxGlobalLogLevelCurrentIndexChange(const QString& text) {
    Log->setGlobalLogLevel(Log->stringToLogLevel(text));
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_BtnSaveConfigClicked() {
    QString filter = tr("Log Configurations (*") + qti_def_SUFFIX_LOGGER_CONFIG + ")";
    QString session_log_path = QtilitiesApplication::applicationSessionPath();
    QString output_file = QFileDialog::getSaveFileName(0, tr("Save log configuration to:"), session_log_path, filter);
    if (output_file.isEmpty())
        return;
    else {
        Log->saveSessionConfig(output_file);
    }
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_BtnLoadConfigClicked() {
    QString filter = tr("Log Configurations (*") + qti_def_SUFFIX_LOGGER_CONFIG + ")";
    QString session_log_path = QtilitiesApplication::applicationSessionPath();
    QString input_file = QFileDialog::getOpenFileName(0, tr("Select log configuration to load:"), session_log_path, filter);
    if (input_file.isEmpty())
        return;
    else {
        Log->loadSessionConfig(input_file);
    }
}

void Qtilities::CoreGui::LoggerConfigWidget::handle_BtnApplyClicked() {
    writeSettings();
    Log->saveSessionConfig();
}

void Qtilities::CoreGui::LoggerConfigWidget::resizeCommandTableRows() {
    d->logger_engine_model.requestRefresh();

    for (int i = 0; i < d->logger_engine_model.rowCount(); ++i) {
        ui->tableViewLoggerEngines->setRowHeight(i,17);
    }

    // We also sort in here:
    ui->tableViewLoggerEngines->sortByColumn(qti_private_LoggerEnginesTableModel::NameColumn,Qt::AscendingOrder);
}

void Qtilities::CoreGui::LoggerConfigWidget::writeSettings() {
    if (!QtilitiesCoreApplication::qtilitiesSettingsEnabled())
        return;

    // Store settings using QSettings only if it was initialized
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Logging");
    settings.beginGroup("General");
    settings.setValue("global_log_level", QVariant(Log->globalLogLevel()));
    settings.setValue("remember_session_config", QVariant(Log->rememberSessionConfig()));
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::LoggerConfigWidget::readSettings() {
    if (QCoreApplication::organizationName().isEmpty() || QCoreApplication::organizationDomain().isEmpty() || QCoreApplication::applicationName().isEmpty())
        qDebug() << tr("The logger may not be able to restore paramaters from previous sessions since the correct details in QCoreApplication have not been set.");

    // Load logging paramaters using QSettings()
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("Logging");
    settings.beginGroup("General");
    QVariant log_level =  settings.value("global_log_level", Logger::Fatal);
    Logger::MessageType global_type = (Logger::MessageType) log_level.toInt();
    ui->comboGlobalLogLevel->setCurrentIndex(ui->comboGlobalLogLevel->findText(Log->logLevelToString(global_type)));
    if (settings.value("remember_session_config", true).toBool())
        ui->checkBoxRememberSession->setChecked(true);
    else
        ui->checkBoxRememberSession->setChecked(false);

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void Qtilities::CoreGui::LoggerConfigWidget::refreshLoggerEngineInformation() {
    if (!d->active_engine) {
        ui->txtLoggerEngineStatus->setPlainText(tr("No Engine Selected"));
        ui->txtLoggerEngineDescription->setPlainText(tr("No Engine Selected"));
        ui->comboBoxLoggerFormattingEngine->setEnabled(false);
        ui->txtMessageContexts->setEnabled(false);
        ui->btnRemoveLoggerEngine->setEnabled(false);
        ui->groupBoxEngineDetails->setEnabled(false);
    } else {
        // Status:
        ui->txtLoggerEngineStatus->setPlainText(d->active_engine->status());

        // Description:
        ui->txtLoggerEngineDescription->setPlainText(d->active_engine->description());

        // Formatting Engine:
        ui->comboBoxLoggerFormattingEngine->setCurrentIndex(ui->comboBoxLoggerFormattingEngine->findText(d->active_engine->formattingEngineName()));
        if (d->active_engine->isFormattingEngineConstant())
            ui->comboBoxLoggerFormattingEngine->setEnabled(false);
        else
            ui->comboBoxLoggerFormattingEngine->setEnabled(true);

        // Message Contexts:
        QString contexts_string = Log->messageContextsToString(d->active_engine->messageContexts());
        ui->txtMessageContexts->setText(contexts_string);
        ui->txtMessageContexts->setToolTip(contexts_string);
        ui->txtMessageContexts->setEnabled(true);

        // Remove Button:
        if (d->active_engine->removable())
            ui->btnRemoveLoggerEngine->setEnabled(true);
        else
            ui->btnRemoveLoggerEngine->setEnabled(false);

        // Make all rows the same height:
        for (int i = 0; i < Log->attachedLoggerEngineCount(); ++i)
            ui->tableViewLoggerEngines->setRowHeight(i,17);

        ui->groupBoxEngineDetails->setEnabled(true);
    }
}

void Qtilities::CoreGui::LoggerConfigWidget::updateActiveEngine() {
    d->active_engine = Log->loggerEngineReferenceAt(ui->tableViewLoggerEngines->currentIndex().row());
    refreshLoggerEngineInformation();
}

void Qtilities::CoreGui::LoggerConfigWidget::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Qtilities::CoreGui::LoggerConfigWidget::on_btnViewLog_clicked() {
    if (d->active_engine) {
        // For now we show file and widget logger engines:
        WidgetLoggerEngine* widget_engine = qobject_cast<WidgetLoggerEngine*> (d->active_engine);
        if (widget_engine) {
            QWidget* widget = widget_engine->getWidget();
            if (widget) {
                widget->resize(QSize(1000,600));
                widget->show();
                widget->activateWindow();
            }
            return;
        }

        FileLoggerEngine* file_engine = qobject_cast<FileLoggerEngine*> (d->active_engine);
        if (file_engine) {
            CodeEditorWidget* code_editor = new CodeEditorWidget(CodeEditorWidget::ActionFind);
            code_editor->loadFile(file_engine->getFileName());
            code_editor->showSearchBox();
            code_editor->searchBoxWidget()->setWidgetMode(SearchBoxWidget::SearchOnly);
            code_editor->codeEditor()->setReadOnly(true);
            code_editor->setAttribute(Qt::WA_QuitOnClose,false);
            code_editor->setAttribute(Qt::WA_DeleteOnClose, true);
            code_editor->resize(QSize(1000,600));
            code_editor->show();
            return;
        }
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Cannot View Log");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("The selected logger engine could not be viewed.");
    msgBox.exec();
}


void Qtilities::CoreGui::LoggerConfigWidget::on_tableViewLoggerEngines_doubleClicked(const QModelIndex &index) {
    Q_UNUSED(index)
    on_btnViewLog_clicked();
}
