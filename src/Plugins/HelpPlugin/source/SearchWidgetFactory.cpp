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

#include "SearchWidgetFactory.h"
#include "HelpMode.h"

#include <QVBoxLayout>
#include <QtHelp>

Qtilities::Plugins::Help::SearchWidgetFactory::SearchWidgetFactory(QHelpSearchEngine* help_search_engine, QObject *parent)
    : QObject(parent), help_search_engine(help_search_engine) {

}

QWidget* Qtilities::Plugins::Help::SearchWidgetFactory::produceWidget() {
    QWidget* combined_widget = new QWidget();
    if (combined_widget->layout())
        delete combined_widget->layout();

    QVBoxLayout* layout = new QVBoxLayout(combined_widget);
    QHelpSearchQueryWidget* queryWidget = help_search_engine->queryWidget();
    connect(queryWidget,SIGNAL(search()),SLOT(handleSearchSignal()));
    layout->addWidget(queryWidget);
    QWidget* result_widget = help_search_engine->resultWidget();
    layout->addWidget(result_widget);
    layout->setMargin(0);

    emit newWidgetCreated(result_widget);

    return combined_widget;
}

void Qtilities::Plugins::Help::SearchWidgetFactory::handleSearchSignal() {
    QHelpSearchQueryWidget* queryWidget = qobject_cast<QHelpSearchQueryWidget*> (sender());
    if (queryWidget) {
        help_search_engine->search(queryWidget->query());
    }
}

QString Qtilities::Plugins::Help::SearchWidgetFactory::widgetLabel() const {
    return tr("Search");
}

QList<int> Qtilities::Plugins::Help::SearchWidgetFactory::startupModes() const {
    QList<int> modes;
    modes << MODE_HELP_ID;
    return modes;
}

Qtilities::CoreGui::Interfaces::IActionProvider* Qtilities::Plugins::Help::SearchWidgetFactory::actionProvider() const {
    return 0;
}

QList<int> Qtilities::Plugins::Help::SearchWidgetFactory::destinationModes() const {
    QList<int> modes;
    modes << MODE_HELP_ID;
    return modes;
}

bool Qtilities::Plugins::Help::SearchWidgetFactory::isExclusive() const {
    return true;
}

