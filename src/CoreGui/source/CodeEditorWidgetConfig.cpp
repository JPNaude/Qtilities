/****************************************************************************
**
** Copyright 2009-2013, Jaco Naudé
**
** This library is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License.
**  
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this library.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "CodeEditorWidgetConfig.h"
#include "ui_CodeEditorWidgetConfig.h"

#include "QtilitiesApplication.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QSettings>
#include <QFontDatabase>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;

Qtilities::CoreGui::CodeEditorWidgetConfig::CodeEditorWidgetConfig(QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent,f),
    ui(new Ui::CodeEditorWidgetConfig)
{
    ui->setupUi(this);
}

Qtilities::CoreGui::CodeEditorWidgetConfig::~CodeEditorWidgetConfig() {
    delete ui;
}

QIcon Qtilities::CoreGui::CodeEditorWidgetConfig::configPageIcon() const {
    return QIcon(qti_icon_CONFIG_CODE_EDITOR_48x48);
}

QWidget* Qtilities::CoreGui::CodeEditorWidgetConfig::configPageWidget() {
    return this;
}

QString Qtilities::CoreGui::CodeEditorWidgetConfig::configPageTitle() const {
    return tr(qti_config_page_CODE_EDITORS);
}

QtilitiesCategory Qtilities::CoreGui::CodeEditorWidgetConfig::configPageCategory() const {
    if (IConfigPage::configPageCategory().isEmpty())
        return QtilitiesCategory(tr(qti_config_page_DEFAULT_CAT));
    else
        return IConfigPage::configPageCategory();
}

void Qtilities::CoreGui::CodeEditorWidgetConfig::configPageApply() {
    if (!QtilitiesCoreApplication::qtilitiesSettingsEnabled())
        return;

    // Save fields back to QSettings
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup("Editors");
    settings.beginGroup("Code Editor Widget");
    settings.setValue("font_type",ui->fontComboBox->currentText());
    settings.setValue("font_size",ui->fontSizeComboBox->currentText().toInt());
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();

    // Emit the settings update request signal
    QtilitiesApplication::newSettingsUpdateRequest("AllCodeEditors");
}

void Qtilities::CoreGui::CodeEditorWidgetConfig::configPageInitialize() {  
    // Populate font sizes combo box
    QFontDatabase db;
    foreach(int size, db.standardSizes())
        ui->fontSizeComboBox->addItem(QString::number(size));

    // Populate fields with values from QSettings
    QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
    settings.beginGroup("Qtilities");
    settings.beginGroup("GUI");
    settings.beginGroup("Editors");
    settings.beginGroup("Code Editor Widget");
    ui->fontComboBox->setEditText(settings.value("font_type","Courier").toString());
    ui->fontSizeComboBox->setEditText(settings.value("font_size",8).toString());
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}
