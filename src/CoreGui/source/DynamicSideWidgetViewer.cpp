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
    DynamicSideWidgetViewerData() : splitter(0),
    is_exclusive(false) {}

    QSplitter* splitter;
    QMap<QString, ISideViewerWidget*> text_iface_map;
    QList<DynamicSideWidgetWrapper*> active_wrappers;
    int mode_destination;
    bool is_exclusive;
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

void Qtilities::CoreGui::DynamicSideWidgetViewer::setIFaceMap(QMap<QString, ISideViewerWidget*> text_iface_map, bool is_exclusive) {
    d->is_exclusive = is_exclusive;

    QMap<QString, ISideViewerWidget*> filtered_list;
    // Create a filtered list depending on the mode destination:
    for (int i = 0; i < text_iface_map.count(); i++) {
        if (text_iface_map.values().at(i)->destinationModes().contains(d->mode_destination))
            filtered_list[text_iface_map.keys().at(i)] = text_iface_map.values().at(i);
    }
    d->text_iface_map = filtered_list;

    // Only create wrappers for and show widgets which should be shown on startup:
    for (int i = 0; i < filtered_list.count(); i++) {
        if (filtered_list.values().at(i)->startupModes().contains(d->mode_destination)) {
            DynamicSideWidgetWrapper* wrapper = new DynamicSideWidgetWrapper(filtered_list,filtered_list.keys().at(i),d->is_exclusive);
            connect(wrapper,SIGNAL(aboutToBeDestroyed(QWidget*)),SLOT(handleSideWidgetDestroyed(QWidget*)));
            connect(wrapper,SIGNAL(newSideWidgetRequest()),SLOT(handleNewSideWidgetRequest()));
            connect(wrapper,SIGNAL(currentTextChanged(QString)),SLOT(updateWrapperComboBoxes()));
            d->splitter->addWidget(wrapper);
            d->active_wrappers << wrapper;
            wrapper->show();
        }
    }

    if (d->is_exclusive)
        updateWrapperComboBoxes();
}

bool Qtilities::CoreGui::DynamicSideWidgetViewer::isExclusive() const {
    return d->is_exclusive;
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::handleSideWidgetDestroyed(QWidget* widget) {
    DynamicSideWidgetWrapper* wrapper = qobject_cast<DynamicSideWidgetWrapper*> (widget);
    QString wrapper_test = wrapper->currentText();
    if (wrapper) {
        d->active_wrappers.removeOne(wrapper);
        if (d->active_wrappers.count() == 0) {
            // In this case we create the last widget again.
            DynamicSideWidgetWrapper* wrapper = new DynamicSideWidgetWrapper(d->text_iface_map,wrapper_test,d->is_exclusive);
            connect(wrapper,SIGNAL(aboutToBeDestroyed(QWidget*)),SLOT(handleSideWidgetDestroyed(QWidget*)));
            connect(wrapper,SIGNAL(newSideWidgetRequest()),SLOT(handleNewSideWidgetRequest()));
            connect(wrapper,SIGNAL(currentTextChanged(QString)),SLOT(updateWrapperComboBoxes(QString)));
            d->splitter->addWidget(wrapper);
            d->active_wrappers << wrapper;
            wrapper->show();

            emit toggleVisibility(false);
        } else
            updateWrapperComboBoxes();
    }
}

QStringList Qtilities::CoreGui::DynamicSideWidgetViewer::activeWrapperNames() const {
    QStringList names;
    for (int i = 0; i < d->active_wrappers.count(); i++)
        names << d->active_wrappers.at(i)->currentText();
    return names;
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::handleNewSideWidgetRequest() {
    if (d->text_iface_map.count() == 0)
        return;

    // If exclusive, we must construct any widget which is not yet visible.
    QString new_widget_label;
    if (d->is_exclusive) {
        for (int i = 0; i < d->text_iface_map.count(); i++) {
            if (!activeWrapperNames().contains(d->text_iface_map.keys().at(i))) {
                new_widget_label = d->text_iface_map.keys().at(i);
                break;
            }
        }
    } else {
        new_widget_label = d->text_iface_map.keys().at(0);
    }
    DynamicSideWidgetWrapper* wrapper = new DynamicSideWidgetWrapper(d->text_iface_map,new_widget_label,d->is_exclusive);
    connect(wrapper,SIGNAL(aboutToBeDestroyed(QWidget*)),SLOT(handleSideWidgetDestroyed(QWidget*)));
    connect(wrapper,SIGNAL(newSideWidgetRequest()),SLOT(handleNewSideWidgetRequest()));
    connect(wrapper,SIGNAL(currentTextChanged(QString)),SLOT(updateWrapperComboBoxes(QString)));
    d->splitter->addWidget(wrapper);
    d->active_wrappers << wrapper;
    wrapper->show();
    updateWrapperComboBoxes();
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::updateWrapperComboBoxes(const QString& exclude_text) {
    if (d->is_exclusive) {
        // Now we send all side viewer wrappers a list of widgets which can still be produced:
        // Inspect all current wrappers:
        QStringList wrapper_labels;
        for (int i = 0; i < d->active_wrappers.count(); i++)
            wrapper_labels << d->active_wrappers.at(i)->currentText();

        // Now build up a new map with unconstructed widgets:
        QMap<QString, ISideViewerWidget*> available_widgets;
        for (int i = 0; i < d->text_iface_map.count(); i++) {
            if (!wrapper_labels.contains(d->text_iface_map.keys().at(i)))
                available_widgets[d->text_iface_map.keys().at(i)] = d->text_iface_map.values().at(i);
        }

        // Lastly set the new available list in all active wrappers:
        for (int i = 0; i < d->active_wrappers.count(); i++) {
            if (exclude_text.isEmpty())
                d->active_wrappers.at(i)->updateAvailableWidgets(available_widgets);
            else {
                if (d->active_wrappers.at(i)->currentText() != exclude_text)
                    d->active_wrappers.at(i)->updateAvailableWidgets(available_widgets);
            }
        }
    }
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
