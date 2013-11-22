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

#include "DynamicSideWidgetWrapper.h"
#include "ui_DynamicSideWidgetWrapper.h"
#include "ISideViewerWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IActionProvider.h"

#include <QComboBox>
#include <QBoxLayout>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Interfaces;
using namespace Qtilities::CoreGui::Icons;

struct Qtilities::CoreGui::DynamicSideWidgetWrapperPrivateData {
    DynamicSideWidgetWrapperPrivateData() : current_widget(0),
        ignore_combo_box_changes(false),
        is_current_widget_managed(false) {}

    QComboBox*                          widgetCombo;
    QPointer<QWidget>                   current_widget;
    QMap<QString, ISideViewerWidget*>   text_iface_map;
    QList<QAction*>                     viewer_actions;
    bool                                ignore_combo_box_changes;
    bool                                is_exclusive;
    bool                                is_current_widget_managed;
};

Qtilities::CoreGui::DynamicSideWidgetWrapper::DynamicSideWidgetWrapper(QMap<QString, ISideViewerWidget*> text_iface_map, const QString& current_text, const bool is_exclusive, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DynamicSideWidgetWrapper)
{
    ui->setupUi(this);
    d = new DynamicSideWidgetWrapperPrivateData;
    d->text_iface_map = text_iface_map;
    d->is_exclusive = is_exclusive;

    // Close side viewer widget action
    ui->btnNew->setToolTip(tr("New"));
    ui->btnNew->setIcon(QIcon(qti_icon_VIEW_NEW_16x16));
    connect(ui->btnNew,SIGNAL(clicked()),SIGNAL(newSideWidgetRequest()));
    ui->btnClose->setIcon(QIcon(qti_icon_VIEW_REMOVE_16x16));
    ui->btnClose->setToolTip(tr("Close"));
    connect(ui->btnClose,SIGNAL(clicked()),SLOT(close()));

    // Create the combo box
    ui->widgetCombo->setEditable(false);
    QStringList items;
    int index = 0;
    QList<QString> keys = d->text_iface_map.keys();
    for (int i = 0; i < d->text_iface_map.count(); ++i) {
        items << keys.at(i);
        if (keys.at(i) == current_text)
            index = i;
    }

    ui->widgetCombo->addItems(items);

    // Set before we connect in case the text changes and the index change handler is called twice:
    ui->widgetCombo->setCurrentIndex(index);
    connect(ui->widgetCombo,SIGNAL(currentIndexChanged(QString)),SLOT(handleCurrentIndexChanged(QString)));
    handleCurrentIndexChanged(current_text);
    setObjectName(current_text);

    //ui->widgetCombo->setStyleSheet(DynamicSideWidgetWrapper::comboBoxStyle());
}

Qtilities::CoreGui::DynamicSideWidgetWrapper::~DynamicSideWidgetWrapper() {
//    if (d->current_widget && !d->is_current_widget_managed) {
//        d->current_widget->hide();
//        d->current_widget->setParent(0);
//    }

    delete ui;
    delete d;
}

QLatin1String Qtilities::CoreGui::DynamicSideWidgetWrapper::comboBoxStyle() {
    QLatin1String style(
        "QComboBox {"
            "border: 0.5px solid gray;"
            "border-radius: 0px;"
            "padding: 1px 0px 1px 5px;"
            "min-width: 6em;"
        "}"
        "QComboBox:editable {"
             "background: white;"
        "}"
        "QComboBox::drop-down {"
            "subcontrol-origin: padding;"
            "subcontrol-position: top right;"
            " width: 15px;"

            "border-left-width: 1px;"
            "border-left-color: darkgray;"
            "border-left-style: solid; "
            "border-top-right-radius: 0px; "
            "border-bottom-right-radius: 0px;"
        "}"
        "QComboBox:!editable, QComboBox::drop-down:editable {"
            "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
            "stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,"
            "stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
        "}"
        /* QComboBox gets the "on" state when the popup is open */
        "QComboBox:!editable:on, QComboBox::drop-down:editable:on {"
            "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
            "stop: 0 #D3D3D3, stop: 0.4 #D8D8D8,"
            "stop: 0.5 #DDDDDD, stop: 1.0 #E1E1E1);"
        "}"
        "QComboBox::down-arrow {"
            "image: url(\":/qtilities/coregui/icons/combobox_down_arrow.png\");"
        "}"
    );
    return style;
}

QString Qtilities::CoreGui::DynamicSideWidgetWrapper::currentText() const {
    if (ui->widgetCombo)
        return ui->widgetCombo->currentText();
    else
        return QString();
}

QWidget *Qtilities::CoreGui::DynamicSideWidgetWrapper::currentWidget() const {
    return d->current_widget;
}

void Qtilities::CoreGui::DynamicSideWidgetWrapper::handleCurrentIndexChanged(const QString& text) {
    if (d->ignore_combo_box_changes)
        return;

    if (d->text_iface_map.contains(text)) {
        if (d->current_widget) {
            // TODO
//            for (int i = 0; i < d->viewer_actions.count(); ++i)
//                ui->toolBar->removeAction(d->viewer_actions.at(i));

            d->viewer_actions.clear();
            d->current_widget->setParent(0);

            // Check if we must delete the current widget:
            if (d->text_iface_map[text]->manageWidgets()) {
                delete d->current_widget;
            }
        }

        ISideViewerWidget* iface = d->text_iface_map[text];
        QWidget* widget = iface->produceWidget();
        if (widget) {
            // Check if the viewer widget needs to add actions to the toolbar:
            // TODO
//            IActionProvider* action_provider = d->text_iface_map[text]->actionProvider();
//            if (action_provider) {
//                if (action_provider->actions().count() > 0) {
//                    d->viewer_actions = action_provider->actions();
//                    ui->toolBar->addActions(d->viewer_actions);
//                }
//            }

            if (ui->centralwidget->layout())
                delete ui->centralwidget->layout();

            QHBoxLayout* layout = new QHBoxLayout(ui->centralwidget);
            layout->setMargin(0);
            layout->addWidget(widget);
            widget->show();
            widget->setEnabled(true);
            d->current_widget = widget;
            d->is_current_widget_managed = iface->manageWidgets();
            setObjectName(text);
            emit currentTextChanged(text);
        }
    }
}

void Qtilities::CoreGui::DynamicSideWidgetWrapper::updateAvailableWidgets(QMap<QString, ISideViewerWidget*> text_iface_map) {
    d->ignore_combo_box_changes = true;
    QString current_text = ui->widgetCombo->currentText();
    ISideViewerWidget* current_iface = d->text_iface_map[current_text];
    d->text_iface_map.clear();
    d->text_iface_map.unite(text_iface_map);
    d->text_iface_map.insert(current_text,current_iface);

    ui->widgetCombo->clear();
    QStringList items;
    for (int i = 0; i < d->text_iface_map.count(); ++i)
        items << d->text_iface_map.keys().at(i);
    ui->widgetCombo->addItems(items);
    ui->widgetCombo->setCurrentIndex(ui->widgetCombo->findText(current_text));
    refreshNewWidgetAction();
    d->ignore_combo_box_changes = false;
}

void Qtilities::CoreGui::DynamicSideWidgetWrapper::close() {
    ui->btnClose->setEnabled(false);
    ui->btnNew->setEnabled(false);
    d->current_widget->setEnabled(false);

    emit aboutToBeDestroyed(this);

    // Delete the current widget if we need to manage it:
    if (d->current_widget) {
        QString current_text = ui->widgetCombo->currentText();
        ISideViewerWidget* current_iface = d->text_iface_map[current_text];
        if (current_iface->manageWidgets())
            delete d->current_widget;
    }

    // Delete this object.
    deleteLater();
}

void Qtilities::CoreGui::DynamicSideWidgetWrapper::refreshNewWidgetAction() {
    if (d->is_exclusive && d->text_iface_map.count() == 1)
        ui->btnNew->setEnabled(false);
    else
        ui->btnNew->setEnabled(true);
}
