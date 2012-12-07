/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
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
** You should have received a copy of the GNU General Public Licese
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
