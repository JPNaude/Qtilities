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

#ifndef ITESTABLE_H
#define ITESTABLE_H

#include "Testing_global.h"
#include "IObjectBase.h"

#include <QObject>
#include <QString>

namespace Qtilities {
    namespace Testing {
        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class ITestable
            \brief Objects implementing this interface indicate that unit tests can be performed on them.

            For an example of how to implement ITestable, see Qtilities::Testing::TestVersionNumber.

            <i>This class was added in %Qtilities v1.0.</i>
              */
            class TESTING_SHARED_EXPORT ITestable : virtual public IObjectBase {
            public:
                ITestable() {}
                virtual ~ITestable() {}

                //! Function which will be called when the test must be executed on the testable object.
                virtual int execTest(int argc = 0, char ** argv = 0) = 0;
                //! Provide a name for your test.
                virtual QString testName() const = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Testing::Interfaces::ITestable,"com.Qtilities.Testing.ITestable/1.0");

#endif // ITESTABLE_H
