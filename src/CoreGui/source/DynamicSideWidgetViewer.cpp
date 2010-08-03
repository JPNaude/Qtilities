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

#include "DynamicSideWidgetViewer.h"
#include "ui_DynamicSideWidgetViewer.h"
#include "DynamicSideWidgetWrapper.h"
#include "ISideViewerWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QBoxLayout>
#include <QSplitter>
#include <QLabel>

struct Qtilities::CoreGui::DynamicSideWidgetViewerData {
    DynamicSideWidgetViewerData() : splitter(0) {}

    QSplitter* splitter;
    QMap<QString, ISideViewerWidget*> text_iface_map;
    int mode_destination;
};

Qtilities::CoreGui::DynamicSideWidgetViewer::DynamicSideWidgetViewer(int mode_destination, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DynamicSideWidgetViewer)
{
    ui->setupUi(this);
    d = new DynamicSideWidgetViewerData;
    setObjectName("Dynamic Side Viewer Widget");

    if (layout())
        delete layout();

    // Create new layout & splitter
    d->splitter = new QSplitter(Qt::Vertical);
    d->splitter->setHandleWidth(0);
    d->splitter->setMinimumWidth(220);
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    layout->addWidget(d->splitter);
    layout->setMargin(0);
    layout->setSpacing(0);

    d->mode_destination = mode_destination;
}

Qtilities::CoreGui::DynamicSideWidgetViewer::~DynamicSideWidgetViewer()
{
    delete ui;
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::setIFaceMap(QMap<QString, ISideViewerWidget*> text_iface_map, QWidget* static_top_widget) {
    // Show the static top widget by default.
    if (static_top_widget) {
        static_top_widget->resize(200,200);
        d->splitter->addWidget(static_top_widget);
        static_top_widget->show();
    }

    QMap<QString, ISideViewerWidget*> filtered_list;
    // Create a filtered list depending on the mode destination
    for (int i = 0; i < text_iface_map.count(); i++) {
        if (text_iface_map.values().at(i)->destinationModes().contains(d->mode_destination))
            filtered_list[text_iface_map.keys().at(i)] = text_iface_map.values().at(i);
    }
    d->text_iface_map = filtered_list;

    // Shows the ones which indicate that they need to be shown.
    for (int i = 0; i < filtered_list.count(); i++) {
        if (filtered_list.values().at(i)->showOnStartup()) {
            DynamicSideWidgetWrapper* wrapper = new DynamicSideWidgetWrapper(filtered_list, filtered_list.keys().at(i));
            connect(wrapper,SIGNAL(destroyed(QObject*)),SLOT(handleSideWidgetDestroyed(QObject*)));
            connect(wrapper,SIGNAL(newSideWidgetRequest()),SLOT(handleNewSideWidgetRequest()));
            d->splitter->addWidget(wrapper);
            wrapper->show();
        }
    }
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::handleSideWidgetDestroyed(QObject* obj) {
    ISideViewerWidget* iface = qobject_cast<ISideViewerWidget*> (obj);
    if (d->text_iface_map.values().contains(iface)) {
        for (int i = 0; i < d->text_iface_map.count(); i++) {
            if (d->text_iface_map.values().at(i) == iface)
                d->text_iface_map.remove(d->text_iface_map.keys().at(i));
        }
    }
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::handleNewSideWidgetRequest() {
    if (d->text_iface_map.count() == 0)
        return;

    DynamicSideWidgetWrapper* wrapper = new DynamicSideWidgetWrapper(d->text_iface_map, d->text_iface_map.keys().at(0));
    connect(wrapper,SIGNAL(destroyed(QObject*)),SLOT(handleSideWidgetDestroyed(QObject*)));
    connect(wrapper,SIGNAL(newSideWidgetRequest()),SLOT(handleNewSideWidgetRequest()));
    d->splitter->addWidget(wrapper);
    wrapper->show();
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::changeEvent(QEvent *e)
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
