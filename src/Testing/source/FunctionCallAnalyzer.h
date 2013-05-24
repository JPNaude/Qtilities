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
