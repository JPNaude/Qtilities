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
    QList<QPointer<QWidget> > widgets;
    Factory<QWidget> factory;
    QString widget_id;
    bool show_on_startup;
};

Qtilities::CoreGui::SideViewerWidgetHelper::SideViewerWidgetHelper(FactoryInterface<QWidget>* interface, const QString& widget_id, QList<int> modes, bool show_on_startup) : QObject(0) {
    d = new SideViewerWidgetHelperData;

    FactoryInterfaceData factory_data("Produce Me");
    d->factory.registerFactoryInterface(interface,factory_data);
    d->widget_id = widget_id;
    d->show_on_startup = show_on_startup;
    d->modes = modes;
}

Qtilities::CoreGui::SideViewerWidgetHelper::~SideViewerWidgetHelper() {
    delete d;
}

QWidget* Qtilities::CoreGui::SideViewerWidgetHelper::widget() {
    QWidget* new_instance = d->factory.createInstance("Produce Me");
    d->widgets << new_instance;
    emit newWidgetCreated(new_instance);
    return new_instance;
}

QString Qtilities::CoreGui::SideViewerWidgetHelper::text() const {
    return d->widget_id;
}

bool Qtilities::CoreGui::SideViewerWidgetHelper::showOnStartup() const {
    return true;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::CoreGui::SideViewerWidgetHelper::actionProvider() const {
    return 0;
}

QList<int> Qtilities::CoreGui::SideViewerWidgetHelper::destinationModes() const {
    return d->modes;
}


