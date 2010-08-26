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

#include "ModeWidget.h"
#include "ui_ModeWidget.h"
#include "IMode.h"
#include "TopToBottomList.h"

#include <QtilitiesCore>

#include <QListWidgetItem>
using namespace QtilitiesCore;

struct Qtilities::CoreGui::ModeWidgetData {
    ModeWidgetData() : active_mode(0) {}

    QMap<QString, IMode*> name_widget_map;
    TopToBottomList* modeList;
    IMode* active_mode;
};

Qtilities::CoreGui::ModeWidget::ModeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModeWidget)
{
    ui->setupUi(this);
    d = new ModeWidgetData;

    if (ui->modeListHolder->layout())
        delete ui->modeListHolder->layout();

    d->modeList = new TopToBottomList();
    d->modeList->setViewMode(QListView::IconMode);

    QHBoxLayout* holder_layout = new QHBoxLayout(ui->modeListHolder);
    holder_layout->addWidget(d->modeList);
    holder_layout->setMargin(0);
}

Qtilities::CoreGui::ModeWidget::~ModeWidget()
{
    delete ui;
}


bool Qtilities::CoreGui::ModeWidget::addMode(IMode* mode, bool initialize_mode) {
    if (mode) {   
        // First check if it has a valid widget, otherwise we don't add the mode.
        if (!mode->widget()) {
            return false;
        }

        if (initialize_mode)
            mode->initialize();

        // Add modes to the modeList
        QIcon icon = mode->icon();
        //if (icon.isNull())
        //    icon =
        QListWidgetItem* new_item = new QListWidgetItem(icon,mode->text(),d->modeList);
        d->modeList->addItem(new_item);
        d->name_widget_map[mode->text()] = mode;
        mode->initialize();

        // We set the first mode we find to be active
        if (d->name_widget_map.count() == 1) {
            connect(d->modeList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(handleModeListCurrentItemChanged(QListWidgetItem*)));
            d->modeList->setCurrentItem(new_item);
        }
    }

    d->modeList->setMinimumWidth(d->modeList->sizeHint().width()+10);
    //d->modeList->setMaximumWidth(d->modeList->sizeHint().width()+25);
    d->modeList->setGridSize(d->modeList->itemSizeHint());

    d->modeList->itemSizeHint();
    for (int i = 0; i < d->modeList->count(); i++ ) {
        d->modeList->item(i)->setSizeHint(d->modeList->itemSizeHint());
    }

    return true;
}

void Qtilities::CoreGui::ModeWidget::addModes(QList<IMode*> modes, bool initialize_modes) {
    for (int i = 0; i < modes.count(); i++) {
        addMode(modes.at(i), initialize_modes);
    }
}

void Qtilities::CoreGui::ModeWidget::addModes(QList<QObject*> modes, bool initialize_modes) {
    for (int i = 0; i < modes.count(); i++) {
        IMode* mode = qobject_cast<IMode*> (modes.at(i));
        if (mode)
            addMode(mode, initialize_modes);
    }
}

QList<Qtilities::CoreGui::Interfaces::IMode*> Qtilities::CoreGui::ModeWidget::modes() const {
    return d->name_widget_map.values();
}

Qtilities::CoreGui::Interfaces::IMode* Qtilities::CoreGui::ModeWidget::activeMode() const {
    return d->active_mode;
}

void Qtilities::CoreGui::ModeWidget::handleModeListCurrentItemChanged(QListWidgetItem * item) {
    if (d->name_widget_map.keys().contains(item->text())) {
        emit changeCentralWidget(d->name_widget_map[item->text()]->widget());
        CONTEXT_MANAGER->setNewContext(d->name_widget_map[item->text()]->contextString());
    }

    d->active_mode = d->name_widget_map[item->text()];
}

void Qtilities::CoreGui::ModeWidget::setActiveMode(int mode_id) {
    // Go through all the registered mode and try to match each mode with new_mode.
    for (int i = 0; i < d->name_widget_map.count(); i++) {
        if (d->name_widget_map.values().at(i)->modeID() == mode_id) {
            d->modeList->setCurrentRow(i);
            break;
        }
    }
}

void Qtilities::CoreGui::ModeWidget::setActiveMode(const QString& mode_name) {
    // Go through all the registered mode and try to match each mode with new_mode.
    for (int i = 0; i < d->name_widget_map.count(); i++) {
        if (d->name_widget_map.values().at(i)->text() == mode_name) {
            d->modeList->setCurrentRow(i);
            break;
        }
    }
}

void Qtilities::CoreGui::ModeWidget::setActiveMode(IMode* mode_iface) {
    // Go through all the registered mode and try to match each mode with new_mode.
    for (int i = 0; i < d->name_widget_map.count(); i++) {
        if (d->name_widget_map.values().at(i) == mode_iface) {
            d->modeList->setCurrentRow(i);
            break;
        }
    }
}

void Qtilities::CoreGui::ModeWidget::changeEvent(QEvent *e)
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
