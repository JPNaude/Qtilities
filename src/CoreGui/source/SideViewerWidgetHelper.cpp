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

#include "SideViewerWidgetHelper.h"
#include "ISideViewerWidget.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QPointer>

struct Qtilities::CoreGui::SideViewerWidgetHelperData {
    SideViewerWidgetHelperData() {}

    QList<int> modes;
    QList<int> startup_modes;
    QList<QPointer<QWidget> > widgets;
    Factory<QWidget> factory;
    QString widget_id;
    bool is_exclusive;
};

Qtilities::CoreGui::SideViewerWidgetHelper::SideViewerWidgetHelper(FactoryInterface<QWidget>* interface, const QString& widget_id, QList<int> modes, QList<int> startup_modes, bool is_exclusive) : QObject(0) {
    d = new SideViewerWidgetHelperData;

    FactoryInterfaceTag factory_data("Produce Me");
    d->factory.registerFactoryInterface(interface,factory_data);
    d->widget_id = widget_id;
    d->modes = modes;
    d->startup_modes = startup_modes;
    d->is_exclusive = is_exclusive;
}

Qtilities::CoreGui::SideViewerWidgetHelper::~SideViewerWidgetHelper() {
    delete d;
}

QWidget* Qtilities::CoreGui::SideViewerWidgetHelper::produceWidget() {
    QWidget* new_instance = d->factory.createInstance("Produce Me");
    d->widgets << new_instance;
    connect(new_instance,SIGNAL(destroyed(QObject*)),SLOT(handleWidgetDestroyed(QObject*)));
    emit newWidgetCreated(new_instance);
    return new_instance;
}

QString Qtilities::CoreGui::SideViewerWidgetHelper::widgetLabel() const {
    return d->widget_id;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::SideViewerWidgetHelper::actionProvider() const {
    return 0;
}

QList<int> Qtilities::CoreGui::SideViewerWidgetHelper::destinationModes() const {
    return d->modes;
}

QList<int> Qtilities::CoreGui::SideViewerWidgetHelper::startupModes() const {
    return d->startup_modes;
}

bool Qtilities::CoreGui::SideViewerWidgetHelper::isExclusive() const {
    return d->is_exclusive;
}

void Qtilities::CoreGui::SideViewerWidgetHelper::handleWidgetDestroyed(QObject* object) {
    QPointer<QWidget> widget = reinterpret_cast<QWidget*> (object);
    d->widgets.removeOne(widget);
}



