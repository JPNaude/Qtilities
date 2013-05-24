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

