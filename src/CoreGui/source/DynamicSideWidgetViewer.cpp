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

#include "DynamicSideWidgetViewer.h"
#include "ui_DynamicSideWidgetViewer.h"
#include "DynamicSideWidgetWrapper.h"
#include "ISideViewerWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QBoxLayout>
#include <QSplitter>
#include <QLabel>

struct Qtilities::CoreGui::DynamicSideWidgetViewerPrivateData {
    DynamicSideWidgetViewerPrivateData() : splitter(0),
    is_exclusive(false) {}

    QSplitter* splitter;
    QMap<QString, ISideViewerWidget*> text_iface_map;
    QList<DynamicSideWidgetWrapper*> active_wrappers;
    int mode_destination;
    bool is_exclusive;
    QStringList hidden_widgets;
};

Qtilities::CoreGui::DynamicSideWidgetViewer::DynamicSideWidgetViewer(int mode_destination, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DynamicSideWidgetViewer)
{
    ui->setupUi(this);
    d = new DynamicSideWidgetViewerPrivateData;
    setObjectName("Dynamic Side Viewer Widget");

    if (layout())
        delete layout();

    // Create new layout & splitter
    d->splitter = new QSplitter(Qt::Vertical);
    d->splitter->setHandleWidth(1);
    d->splitter->setMinimumWidth(220);
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight,this);
    layout->addWidget(d->splitter);
    layout->setMargin(0);
    layout->setSpacing(0);

    d->mode_destination = mode_destination;
}

Qtilities::CoreGui::DynamicSideWidgetViewer::~DynamicSideWidgetViewer() {
    delete ui;
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::setIFaceMap(QMap<QString, ISideViewerWidget*> text_iface_map, bool is_exclusive, const QStringList& widget_order) {
    d->is_exclusive = is_exclusive;

    // Clear previous widgets:
    d->active_wrappers.clear();;
    d->text_iface_map.clear();

    // Create a filtered list depending on the mode destination:
    QMap<QString, ISideViewerWidget*> filtered_list;
    QMapIterator<QString, ISideViewerWidget*> itr_unfiltered(text_iface_map);
    while (itr_unfiltered.hasNext()) {
        itr_unfiltered.next();
        if (itr_unfiltered.value()->destinationModes().contains(d->mode_destination))
            filtered_list[itr_unfiltered.key()] = itr_unfiltered.value();
    }
    d->text_iface_map = filtered_list;

    // First handle the order using widget_order:
    for (int i = 0; i < widget_order.count(); ++i) {
        QString current_item = widget_order.at(i);
        if (filtered_list.contains(current_item)) {
            DynamicSideWidgetWrapper* wrapper = new DynamicSideWidgetWrapper(filtered_list,current_item,d->is_exclusive);
            connect(wrapper,SIGNAL(aboutToBeDestroyed(QWidget*)),SLOT(handleSideWidgetDestroyed(QWidget*)));
            connect(wrapper,SIGNAL(newSideWidgetRequest()),SLOT(handleNewSideWidgetRequest()));
            connect(wrapper,SIGNAL(currentTextChanged(QString)),SLOT(updateWrapperComboBoxes()));
            d->splitter->addWidget(wrapper);
            d->active_wrappers << wrapper;
            wrapper->show();
            filtered_list.remove(current_item);
        }
    }

    // Now handle widgets which were not present in widget_order:
    QMapIterator<QString, ISideViewerWidget*> itr_filtered(filtered_list);
    while (itr_filtered.hasNext()) {
        itr_filtered.next();
        if (itr_filtered.value()->startupModes().contains(d->mode_destination)) {
            DynamicSideWidgetWrapper* wrapper = new DynamicSideWidgetWrapper(filtered_list,itr_filtered.key(),d->is_exclusive);
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

QStringList Qtilities::CoreGui::DynamicSideWidgetViewer::widgetNames() const {
    return d->text_iface_map.keys();
}

QList<ISideViewerWidget *> Qtilities::CoreGui::DynamicSideWidgetViewer::sideViewerWidgetInterfaces() const {
    return d->text_iface_map.values();
}

QList<QWidget *> Qtilities::CoreGui::DynamicSideWidgetViewer::sideViewerWidgets() const {
    QList<QWidget*> widgets;
    for (int i = 0; i < d->active_wrappers.count(); ++i)
        widgets << d->active_wrappers.at(i)->currentWidget();
    return widgets;
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::setHiddenSideWidgets(const QStringList &widget_names) {
    if (d->hidden_widgets == widget_names)
        return;

    d->hidden_widgets = widget_names;

    // Hide needed wrappers:
    for (int i = 0; i < d->active_wrappers.count(); ++i) {
        DynamicSideWidgetWrapper* wrapper = d->active_wrappers.at(i);
        if (d->hidden_widgets.contains(wrapper->currentText()))
            wrapper->setVisible(false);
        else
            wrapper->setVisible(true);
    }

    updateWrapperComboBoxes();
}

QStringList Qtilities::CoreGui::DynamicSideWidgetViewer::hiddenSideWidgets() const {
    return d->hidden_widgets;
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
    for (int i = 0; i < d->active_wrappers.count(); ++i)
        names << d->active_wrappers.at(i)->currentText();
    return names;
}

void Qtilities::CoreGui::DynamicSideWidgetViewer::handleNewSideWidgetRequest() {
    if (d->text_iface_map.count() == 0)
        return;

    // If exclusive, we must construct any widget which is not yet visible.
    QString new_widget_label;
    if (d->is_exclusive) {
        for (int i = 0; i < d->text_iface_map.count(); ++i) {
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
        for (int i = 0; i < d->active_wrappers.count(); ++i)
            wrapper_labels << d->active_wrappers.at(i)->currentText();

        // Now build up a new map with unconstructed widgets:
        QMap<QString, ISideViewerWidget*> available_widgets;
        QStringList available_widgets_keys = d->text_iface_map.keys();
        QList<ISideViewerWidget*> available_widgets_values = d->text_iface_map.values();
        for (int i = 0; i < available_widgets_keys.count(); ++i) {
            QString label = available_widgets_keys.at(i);
            if (d->hidden_widgets.contains(label))
                continue;

            if (!wrapper_labels.contains(label)) {
                available_widgets[label] = available_widgets_values.at(i);
            }
        }

        // Lastly set the new available list in all active wrappers:
        for (int i = 0; i < d->active_wrappers.count(); ++i) {
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
