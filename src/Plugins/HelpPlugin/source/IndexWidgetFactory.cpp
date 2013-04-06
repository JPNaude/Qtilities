/****************************************************************************
**
** Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
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

#include "IndexWidgetFactory.h"
#include "HelpMode.h"

#include <QHBoxLayout>
#include <QHelpIndexWidget>

Qtilities::Plugins::Help::IndexWidgetFactory::IndexWidgetFactory(QHelpEngine* help_engine, QObject *parent)
    : QObject(parent), d_help_engine(help_engine) {
}

QWidget* Qtilities::Plugins::Help::IndexWidgetFactory::produceWidget() {
    if (!d_index_widget && d_help_engine) {
        d_index_widget = d_help_engine->indexWidget();
        emit newWidgetCreated(d_index_widget);
    }

    return d_index_widget;
}

QString Qtilities::Plugins::Help::IndexWidgetFactory::widgetLabel() const {
    return tr("Index");
}

QList<int> Qtilities::Plugins::Help::IndexWidgetFactory::startupModes() const {
    QList<int> modes;
    return modes;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::Plugins::Help::IndexWidgetFactory::actionProvider() const {
    return 0;
}

QList<int> Qtilities::Plugins::Help::IndexWidgetFactory::destinationModes() const {
    QList<int> modes;
    modes << MODE_HELP_ID;
    return modes;
}

bool Qtilities::Plugins::Help::IndexWidgetFactory::isExclusive() const {
    return true;
}

