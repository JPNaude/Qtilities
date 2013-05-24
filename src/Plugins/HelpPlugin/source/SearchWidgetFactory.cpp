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

#include "SearchWidgetFactory.h"
#include "HelpMode.h"

#include <QVBoxLayout>
#include <QtHelp>

Qtilities::Plugins::Help::SearchWidgetFactory::SearchWidgetFactory(QHelpSearchEngine* help_search_engine, QObject *parent)
    : QObject(parent), d_help_search_engine(help_search_engine) {


}

QWidget* Qtilities::Plugins::Help::SearchWidgetFactory::produceWidget() {
    if (!d_combined_widget && d_help_search_engine) {
        d_combined_widget = new QWidget();
        if (d_combined_widget->layout())
            delete d_combined_widget->layout();

        QVBoxLayout* layout = new QVBoxLayout(d_combined_widget);
        QHelpSearchQueryWidget* queryWidget = d_help_search_engine->queryWidget();
        connect(queryWidget,SIGNAL(search()),SLOT(handleSearchSignal()));
        layout->addWidget(queryWidget);
        QHelpSearchResultWidget* result_widget = d_help_search_engine->resultWidget();
        layout->addWidget(result_widget);
        layout->setMargin(0);

        emit newWidgetCreated(result_widget);
    }

    return d_combined_widget;
}

void Qtilities::Plugins::Help::SearchWidgetFactory::handleSearchSignal() {
    QHelpSearchQueryWidget* queryWidget = qobject_cast<QHelpSearchQueryWidget*> (sender());
    if (queryWidget) {
        d_help_search_engine->search(queryWidget->query());
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

