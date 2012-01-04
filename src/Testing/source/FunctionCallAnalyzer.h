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

#ifndef qti_private_FunctionCallAnalyzer_H
#define qti_private_FunctionCallAnalyzer_H

#include "Testing_global.h"

#include <QString>
#include <QMap>

namespace Qtilities {
    namespace Testing {
        /*!
        \class qti_private_FunctionCallAnalyzer
        \brief Tests can inherit from this base class in order to get access to function call analysis functions.

        <i>This class was added in %Qtilities v1.0.</i>
          */
        class TESTING_SHARED_EXPORT qti_private_FunctionCallAnalyzer
        {
        public:
            qti_private_FunctionCallAnalyzer() {}
            ~qti_private_FunctionCallAnalyzer() {}
            //! Increment the call count for the given function name.
            void count(const QString& function_name);
            //! Clears the function call count for the specific function if it exists.
            void clear(const QString& function_name);
            //! Clears all counts.
            void clearAll();
            //! Prints the function call information to the logger.
            /*!
              \param function_name When empty all function counts are printed. When not empty the function call count for the specified function is printed, if it exists off course.
              */
            void logCallCount(const QString& function_name = QString()) const;

        private:
            QMap<QString,int> call_counts;
        };
    }
}

#endif // qti_private_FunctionCallAnalyzer_H
