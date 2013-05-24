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

#include "FunctionCallAnalyzer.h"

#include <QtilitiesCoreGui>
using namespace QtilitiesCoreGui;

void Qtilities::Testing::qti_private_FunctionCallAnalyzer::count(const QString& function_name) {
    int current_value = 0;
    if (call_counts.count() > 0) {
        if (call_counts.contains(function_name))
            current_value = call_counts[function_name];
    }

    ++current_value;
    call_counts[function_name] = current_value;
}

void Qtilities::Testing::qti_private_FunctionCallAnalyzer::clear(const QString& function_name) {
    call_counts.remove(function_name);
}

void Qtilities::Testing::qti_private_FunctionCallAnalyzer::clearAll() {
    call_counts.clear();
}

void Qtilities::Testing::qti_private_FunctionCallAnalyzer::logCallCount(const QString& function_name) const {
    if (call_counts.count() > 0) {
        if (call_counts.contains(function_name)) {
            LOG_INFO("Call count on function " + function_name + ": " + QString::number(call_counts[function_name]));
            return;
        }
    }

    for (int i = 0; i < call_counts.count(); ++i)
        LOG_INFO("Call count on function " + call_counts.keys().at(i) + ": " + QString::number(call_counts.values().at(i)));
}
