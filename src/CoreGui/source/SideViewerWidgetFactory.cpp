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

#include "SideViewerWidgetFactory.h"
#include "ISideViewerWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QPointer>

struct Qtilities::CoreGui::SideViewerWidgetFactoryPrivateData {
    SideViewerWidgetFactoryPrivateData() {}

    QList<int> modes;
    QList<int> startup_modes;
    QList<QPointer<QWidget> > widgets;
    Factory<QWidget> factory;
    QString widget_id;
    bool is_exclusive;
};

Qtilities::CoreGui::SideViewerWidgetFactory::SideViewerWidgetFactory(FactoryInterface<QWidget>* factory_interface, const QString& widget_id, QList<int> modes, QList<int> startup_modes, bool is_exclusive) : QObject(0) {
    d = new SideViewerWidgetFactoryPrivateData;

    FactoryItemID factory_data("Produce Me");
    d->factory.registerFactoryInterface(factory_interface,factory_data);
    d->widget_id = widget_id;
    d->modes = modes;
    d->startup_modes = startup_modes;
    d->is_exclusive = is_exclusive;

    setObjectName(widget_id);
}

Qtilities::CoreGui::SideViewerWidgetFactory::~SideViewerWidgetFactory() {
    delete d;
}

QWidget* Qtilities::CoreGui::SideViewerWidgetFactory::produceWidget() {
    QWidget* new_instance = d->factory.createInstance("Produce Me");
    d->widgets << new_instance;
    connect(new_instance,SIGNAL(destroyed(QObject*)),SLOT(handleWidgetDestroyed(QObject*)));
    emit newWidgetCreated(new_instance,d->widget_id);
    return new_instance;
}

QString Qtilities::CoreGui::SideViewerWidgetFactory::widgetLabel() const {
    return d->widget_id;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::SideViewerWidgetFactory::actionProvider() const {
    return 0;
}

QList<int> Qtilities::CoreGui::SideViewerWidgetFactory::destinationModes() const {
    return d->modes;
}

QList<int> Qtilities::CoreGui::SideViewerWidgetFactory::startupModes() const {
    return d->startup_modes;
}

bool Qtilities::CoreGui::SideViewerWidgetFactory::isExclusive() const {
    return d->is_exclusive;
}

void Qtilities::CoreGui::SideViewerWidgetFactory::handleWidgetDestroyed(QObject* object) {
    QPointer<QWidget> widget = reinterpret_cast<QWidget*> (object);
    d->widgets.removeOne(widget);
}



