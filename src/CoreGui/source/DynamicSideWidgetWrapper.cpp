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

#include "DynamicSideWidgetWrapper.h"
#include "ui_DynamicSideWidgetWrapper.h"
#include "ISideViewerWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <IActionProvider.h>

#include <QComboBox>
#include <QBoxLayout>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::DynamicSideWidgetWrapperData {
    DynamicSideWidgetWrapperData() : widgetCombo(0),
    close_action(0),
    new_action(0),
    current_widget(0) {}

    QComboBox* widgetCombo;
    QAction* close_action;
    QAction* new_action;
    QWidget* current_widget;
    QMap<QString, ISideViewerWidget*> text_iface_map;
    QList<QAction*> viewer_actions;
};

Qtilities::CoreGui::DynamicSideWidgetWrapper::DynamicSideWidgetWrapper(QMap<QString, ISideViewerWidget*> text_iface_map, QString current_text, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DynamicSideWidgetWrapper)
{
    ui->setupUi(this);
    d = new DynamicSideWidgetWrapperData;
    d->text_iface_map = text_iface_map;

    // Close side viewer widget action
    d->new_action = ui->toolBar->addAction(QIcon(ICON_VIEW_NEW_16x16),tr("New"));
    connect(d->new_action,SIGNAL(triggered()),SIGNAL(newSideWidgetRequest()));
    d->close_action = ui->toolBar->addAction(QIcon(ICON_VIEW_REMOVE_16x16),tr("Close"));
    connect(d->close_action,SIGNAL(triggered()),SLOT(handleActionClose_triggered()));

    // Create the combo box
    d->widgetCombo = new QComboBox();
    ui->toolBar->insertWidget(d->new_action, d->widgetCombo);
    ui->toolBar->setMovable(false);
    d->widgetCombo->setEditable(false);
    QStringList items;
    int index = 0;
    for (int i = 0; i < d->text_iface_map.count(); i++) {
        items << d->text_iface_map.keys().at(i);
        if (d->text_iface_map.keys().at(i) == current_text)
            index = i;
    }
    d->widgetCombo->addItems(items);
    connect(d->widgetCombo,SIGNAL(currentIndexChanged(QString)),SLOT(handleCurrentIndexChanged(QString)));
    d->widgetCombo->setCurrentIndex(index);
    handleCurrentIndexChanged(current_text);
    setObjectName(current_text);
}

Qtilities::CoreGui::DynamicSideWidgetWrapper::~DynamicSideWidgetWrapper()
{
    delete ui;
    delete d;
}

void Qtilities::CoreGui::DynamicSideWidgetWrapper::handleCurrentIndexChanged(const QString& text) {
    if (d->text_iface_map.contains(text)) {
        if (d->current_widget) {
            for (int i = 0; i < d->viewer_actions.count(); i++)
                ui->toolBar->removeAction(d->viewer_actions.at(0));

            d->viewer_actions.clear();
            delete d->current_widget;
        }

        QWidget* widget = d->text_iface_map[text]->widget();
        // Check if the viewer widget needs to add actions to the toolbar
        IActionProvider* action_provider = d->text_iface_map[text]->actionProvider();
        if (action_provider) {
            if (action_provider->actions().count() > 0) {
                d->viewer_actions = action_provider->actions();
                ui->toolBar->addActions(d->viewer_actions);
            }
        }

        if (centralWidget()->layout())
            delete centralWidget()->layout();

        // Create new layout with new widget
        QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,centralWidget());
        layout->addWidget(widget);
        widget->show();
        layout->setMargin(0);
        d->current_widget = widget;
        setObjectName(text);
    }
}

void Qtilities::CoreGui::DynamicSideWidgetWrapper::handleActionClose_triggered() {
    // Delete the current widget
    if (d->current_widget)
        delete d->current_widget;

    // Delete this object
    deleteLater();
}

void Qtilities::CoreGui::DynamicSideWidgetWrapper::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
