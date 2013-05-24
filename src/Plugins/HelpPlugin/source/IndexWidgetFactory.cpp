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

