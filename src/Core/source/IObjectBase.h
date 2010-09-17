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

#ifndef IOBJECTBASE_H
#define IOBJECTBASE_H

#include "QtilitiesCore_global.h"

#include <QObject>

namespace Qtilities {
    namespace Core {
        //! Namespace containing available interfaces which forms part of the Core Module.
        namespace Interfaces {
            /*!
            \class IObjectBase
            \brief Interface through which QObjects implementing interfaces can be accessed.
              */
            class QTILIITES_CORE_SHARED_EXPORT IObjectBase {
            public:
                IObjectBase() {}
                virtual ~IObjectBase() {}

                //! Returns the QObject* base of the interface.
                virtual QObject* objectBase() = 0;
                //! Returns a const QObject* base of the interface.
                virtual const QObject* objectBase() const = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IObjectBase,"com.Qtilities.Core.IObjectBase/1.0")

#endif // IOBJECTBASE_H
