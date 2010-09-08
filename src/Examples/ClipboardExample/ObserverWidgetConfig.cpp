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

#include "ObserverWidgetConfig.h"
#include "ui_ObserverWidgetConfig.h"

Qtilities::Examples::Clipboard::ObserverWidgetConfig::ObserverWidgetConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObserverWidgetConfig)
{
    ui->setupUi(this);

    // Connections:
    connect(ui->comboWidgetMode1,SIGNAL(currentIndexChanged(QString)),SLOT(handle_comboWidgetMode1(QString)));
    connect(ui->comboWidgetMode2,SIGNAL(currentIndexChanged(QString)),SLOT(handle_comboWidgetMode2(QString)));

    // We read the settings of each widget:
    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup("ObserverWidget0");
    int mode = settings.value("display_mode", Qtilities::TableView).toInt();
    if ((Qtilities::DisplayMode) mode == Qtilities::TableView)
        ui->comboWidgetMode1->setCurrentIndex(0);
    else
        ui->comboWidgetMode1->setCurrentIndex(1);
    ui->spinBoxRowHeight1->setValue(settings.value("default_row_heigth", 17).toInt());
    ui->checkShowGrid1->setChecked(settings.value("table_view_show_grid", false).toBool());
    settings.endGroup();
    settings.beginGroup("ObserverWidget1");
    mode = settings.value("display_mode", Qtilities::TableView).toInt();
    if ((Qtilities::DisplayMode) mode == Qtilities::TableView)
        ui->comboWidgetMode2->setCurrentIndex(0);
    else
        ui->comboWidgetMode2->setCurrentIndex(1);
    ui->spinBoxRowHeight2->setValue(settings.value("default_row_heigth", 17).toInt());
    ui->checkShowGrid2->setChecked(settings.value("table_view_show_grid", false).toBool());
    settings.endGroup();
    settings.endGroup();


    // Update widget according to settings which was read:
    if (ui->comboWidgetMode1->currentText() == "Table View") {
        ui->spinBoxRowHeight1->setEnabled(true);
    } else {
        ui->spinBoxRowHeight1->setEnabled(false);
    }
    if (ui->comboWidgetMode2->currentText() == "Table View") {
        ui->spinBoxRowHeight2->setEnabled(true);
    } else {
        ui->spinBoxRowHeight2->setEnabled(false);
    }
}

Qtilities::Examples::Clipboard::ObserverWidgetConfig::~ObserverWidgetConfig()
{
    delete ui;
}

QIcon Qtilities::Examples::Clipboard::ObserverWidgetConfig::configPageIcon() const {
    return QIcon();
}

QWidget* Qtilities::Examples::Clipboard::ObserverWidgetConfig::configPageWidget() {
    return this;
}

QString Qtilities::Examples::Clipboard::ObserverWidgetConfig::configPageTitle() const {
    return tr("Observer Widgets");
}

Qtilities::Core::QtilitiesCategory Qtilities::Examples::Clipboard::ObserverWidgetConfig::configPageCategory() const {
    return QtilitiesCategory("Example");
}

void Qtilities::Examples::Clipboard::ObserverWidgetConfig::configPageApply() {
    // Write the settings:
    QSettings settings;
    settings.beginGroup("GUI");
    settings.beginGroup("ObserverWidget0");
    if (ui->comboWidgetMode1->currentText() == "Table View") {
        settings.setValue("display_mode", (int) Qtilities::TableView);
    } else {
        settings.setValue("display_mode", (int) Qtilities::TreeView);
    }
    settings.setValue("default_row_heigth", ui->spinBoxRowHeight1->value());
    settings.setValue("table_view_show_grid", ui->checkShowGrid1->isChecked());
    settings.endGroup();
    settings.beginGroup("ObserverWidget1");
    if (ui->comboWidgetMode2->currentText() == "Table View") {
        settings.setValue("display_mode", (int) Qtilities::TableView);
    } else {
        settings.setValue("display_mode", (int) Qtilities::TreeView);
    }
    settings.setValue("default_row_heigth", ui->spinBoxRowHeight2->value());
    settings.setValue("table_view_show_grid", ui->checkShowGrid2->isChecked());
    settings.endGroup();
    settings.endGroup();

    // Send the update requests:
    QtilitiesApplication::newSettingsUpdateRequest("ObserverWidget0");
    QtilitiesApplication::newSettingsUpdateRequest("ObserverWidget1");
}

void Qtilities::Examples::Clipboard::ObserverWidgetConfig::handle_comboWidgetMode1(const QString& new_value) {
    if (new_value == "Table View") {
        ui->spinBoxRowHeight1->setEnabled(true);
    } else {
        ui->spinBoxRowHeight1->setEnabled(false);
    }
}

void Qtilities::Examples::Clipboard::ObserverWidgetConfig::handle_comboWidgetMode2(const QString& new_value) {
    if (new_value == "Table View") {
        ui->spinBoxRowHeight2->setEnabled(true);
    } else {
        ui->spinBoxRowHeight2->setEnabled(false);
    }
}

void Qtilities::Examples::Clipboard::ObserverWidgetConfig::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
