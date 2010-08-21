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

#include "ConfigurationWidget.h"
#include "ui_ConfigurationWidget.h"
#include "IConfigPage.h"

#include <Logger.h>
#include <QTreeWidgetItem>
#include <QBoxLayout>
#include <QDesktopWidget>

struct Qtilities::CoreGui::ConfigurationWidgetData {
    ConfigurationWidgetData() : active_widget(0) {}

    QWidget* active_widget;
    QMap<QString, IConfigPage*> name_widget_map;
    QString active_page;
    bool initialized;
};

Qtilities::CoreGui::ConfigurationWidget::ConfigurationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigurationWidget)
{
    ui->setupUi(this);
    d = new ConfigurationWidgetData;
    d->initialized = false;

    connect(ui->pageTree,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),SLOT(handleCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    // Put the widget in the center of the screen
    QRect qrect = QApplication::desktop()->availableGeometry(this);
    move(qrect.center() - rect().center());
}

Qtilities::CoreGui::ConfigurationWidget::~ConfigurationWidget()
{
    delete ui;
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<IConfigPage*> config_pages) {
    if (d->initialized)
        return;

    for (int i = 0; i < config_pages.count(); i++) {
        IConfigPage* config_page = config_pages.at(i);
        if (config_page) {
            // First check if it has a valid widget
            if (!config_page->configPageWidget())
                break;

            // Maximum depth is 2 at present
            Q_ASSERT(config_page->configPageTitle().count() <= 2);

            // Build the tree using the QStringList values in the interface's text() function.
            QTreeWidgetItem* parent = 0;
            QTreeWidgetItem* new_parent = 0;
            QStringList name;
            name << config_page->configPageTitle().at(0);
            for (int top_level = 0; top_level < ui->pageTree->topLevelItemCount(); top_level++) {
                if (ui->pageTree->topLevelItem(top_level)->text(0) == name.front()) {
                    parent = ui->pageTree->topLevelItem(top_level);
                    break;
                }
            }

            // If there is not a top level item yet for this category, create it
            if (!parent) {
                parent = new QTreeWidgetItem(ui->pageTree, name);
            }

            for (int depth = 1; depth < config_page->configPageTitle().count(); depth++) {
                name.clear();
                name << config_page->configPageTitle().at(depth);
                for (int r = 0; r < parent->childCount(); r++) {
                    if (parent->child(r)->text(0) == name.front()) {
                        new_parent = parent->child(r);
                        break;
                    }
                }

                // If the current item was not found at this depth, create it.
                if (!new_parent) {
                    new_parent = new QTreeWidgetItem(parent, name);
                    if (depth == config_page->configPageTitle().count()-1)
                        new_parent->setIcon(0,config_page->configPageIcon());
                }
                parent = new_parent;
                new_parent = 0;
            }

            // Add the page to the name_widget_map
            QString text = config_page->configPageTitle().join("::");
            d->name_widget_map[text] = config_page;

            // We set the first valid item to the active item
            if (i == 0) {
                ui->pageTree->setCurrentItem(parent);
            }
        }
    }

    ui->pageTree->expandAll();
    d->initialized = true;
}

void Qtilities::CoreGui::ConfigurationWidget::initialize(QList<QObject*> config_pages) {
    QList<IConfigPage*> config_ifaces;
    for (int i = 0; i < config_pages.count(); i++) {
        IConfigPage* config_iface = qobject_cast<IConfigPage*> (config_pages.at(i));
        if (config_iface)
            config_ifaces.append(config_iface);
    }

    initialize(config_ifaces);
}

void Qtilities::CoreGui::ConfigurationWidget::on_btnClose_clicked() {
    close();
}

void Qtilities::CoreGui::ConfigurationWidget::on_btnApply_clicked() {
    if (d->name_widget_map.keys().contains(d->active_page))
        d->name_widget_map[d->active_page]->configPageApply();
}

void Qtilities::CoreGui::ConfigurationWidget::handleCurrentItemChanged(QTreeWidgetItem* current ,QTreeWidgetItem* previous) {
    Q_UNUSED(previous)

    // Calculate the new widget
    QString current_text = calculateText(current).join("::");
    QWidget* current_widget;

    if (d->name_widget_map.contains(current_text)) {
        // Hide the current widget
        if (d->active_widget)
            d->active_widget->hide();
        current_widget = d->name_widget_map[current_text]->configPageWidget();
        ui->btnApply->setEnabled(d->name_widget_map[current_text]->supportsApply());
    } else
        return;

    if (ui->configWidget->layout())
        delete ui->configWidget->layout();

    // Create new layout with new widget
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,ui->configWidget);
    layout->addWidget(current_widget);
    current_widget->show();
    layout->setMargin(0);
    if (current_widget->layout())
        current_widget->layout()->setMargin(0);
    ui->lblPageHeader->setText(current_text.split("::").last());
    d->active_page = current_text;
    d->active_widget = current_widget;
}

void Qtilities::CoreGui::ConfigurationWidget::changeEvent(QEvent *e)
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

QStringList Qtilities::CoreGui::ConfigurationWidget::calculateText(QTreeWidgetItem* item) {
    // Calculate the QStringList for an item
    QStringList text;
    QTreeWidgetItem* current;

    current = item;
    text.push_front(current->text(0));
    while (!isTopLevelItem(current)) {
        current = item->parent();
        text.push_front(current->text(0));
    }

    return text;
}

bool Qtilities::CoreGui::ConfigurationWidget::isTopLevelItem(QTreeWidgetItem* item) {
    for (int i = 0; i < ui->pageTree->topLevelItemCount(); i++) {
        if (ui->pageTree->topLevelItem(i) == item)
            return true;
    }

    return false;
}
