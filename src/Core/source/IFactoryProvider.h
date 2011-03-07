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

#ifndef IFACTORY
#define IFACTORY

#include "QtilitiesCore_global.h"
#include "PointerList.h"
#include "InstanceFactoryInfo.h"

#include <Logger>

#include <QObject>
#include <QStringList>
#include <QDataStream>

class QDomDocument;
class QDomElement;

namespace Qtilities {
    namespace Core {
        using namespace Qtilities::Logging;

        namespace Interfaces {
            /*!
            \class IFactoryProvider
            \brief Objects managing instances of factories can implement this interface if they want to expose these factories to the object manager.

            Objects managing instances of factories can implement this interface if they want to expose these factories to the object manager using the
            Qtilities::Core::Interfaces::IObjectManager::registerIFactoryProvider() function. The interface can be used to represent multiple factories,
            each represented by a QString values.  To get a list of all factories provided through the interface, see the providedFactoryTags() function.

            For more information see the \ref page_factories article.
            */
            class QTILIITES_CORE_SHARED_EXPORT IFactoryProvider {
            public:
                IFactoryProvider() {}
                virtual ~IFactoryProvider() {}

                //! Provides the names of all the factories exposed through this interface.
                virtual QStringList providedFactories() const = 0;
                //! Provides the tags in a specific factory.
                virtual QStringList providedFactoryTags(const QString& factory_name) const = 0;
                //! Constructs an instance in a specified factory and return it.
                virtual QObject* createInstance(const InstanceFactoryInfo& ifactory_data) = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::Core::Interfaces::IFactoryProvider,"com.Qtilities.Core.IFactoryProvider/1.0")

#endif // IFACTORY
