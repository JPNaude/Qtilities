#ifndef ITESTABLE_H
#define ITESTABLE_H

#endif // ITESTABLE_H


/****************************************************************************
**
** Copyright (c) 2009-2011, Jaco Naude
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

#ifndef ITestable_H
#define ITestable_H

#include "UnitTests_global.h"
#include "IObjectBase.h"

#include <QObject>
#include <QString>

namespace Qtilities {
    namespace UnitTests {
        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class ITestable
            \brief Objects implementing this interface indicate that unit tests can be performed on them.

            For an example of how to implement ITestable, see Qtilities::UnitTests::TestVersionNumber.

            <i>This class was added in %Qtilities v1.0.</i>
              */
            class UNIT_TESTS_SHARED_EXPORT ITestable : virtual public IObjectBase {
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

Q_DECLARE_INTERFACE(Qtilities::UnitTests::Interfaces::ITestable,"com.Qtilities.UnitTests.ITestable/1.0");

#endif // ITestable_H
