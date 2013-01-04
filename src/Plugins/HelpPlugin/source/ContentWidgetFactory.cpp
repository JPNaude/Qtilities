/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#include "ContentWidgetFactory.h"
#include "HelpMode.h"

#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QDebug>

Qtilities::Plugins::Help::ContentWidgetFactory::ContentWidgetFactory(QHelpEngine* help_engine, QObject *parent)
    : QObject(parent), d_help_engine(help_engine) {

}

Qtilities::Plugins::Help::ContentWidgetFactory::~ContentWidgetFactory() {

}

QWidget* Qtilities::Plugins::Help::ContentWidgetFactory::produceWidget() {
    if (!d_content_widget && d_help_engine) {
        QWidget* widget = d_help_engine->contentWidget();
        d_content_widget = widget;
        emit newWidgetCreated(widget);
    }

    return d_content_widget;
}

QString Qtilities::Plugins::Help::ContentWidgetFactory::widgetLabel() const {
    return tr("Contents");
}

QList<int> Qtilities::Plugins::Help::ContentWidgetFactory::startupModes() const {
    QList<int> modes;
    modes << MODE_HELP_ID;
    return modes;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::Plugins::Help::ContentWidgetFactory::actionProvider() const {
    return 0;
}

QList<int> Qtilities::Plugins::Help::ContentWidgetFactory::destinationModes() const {
    QList<int> modes;
    modes << MODE_HELP_ID;
    return modes;
}

bool Qtilities::Plugins::Help::ContentWidgetFactory::isExclusive() const {
    return true;
}

