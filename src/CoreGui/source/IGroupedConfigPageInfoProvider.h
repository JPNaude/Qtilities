/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#ifndef I_GROUPED_CONFIG_PAGE_INFO_PROVIDER_H
#define I_GROUPED_CONFIG_PAGE_INFO_PROVIDER_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCategory.h"

#include <QIcon>
#include <QObject>

using namespace Qtilities::Core;
using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {
            /*!
            \class IGroupedConfigPageInfoProvider
            \brief An interface through which information for groped config pages can be provided.

            <i>This class was added in %Qtilities v1.1.</i>.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IGroupedConfigPageInfoProvider
            {
            public:
                IGroupedConfigPageInfoProvider() {}
                virtual ~IGroupedConfigPageInfoProvider() {}

                //! Gets if this provider provides information for a specific category.
                virtual bool isProviderForCategory(const QtilitiesCategory& category) const = 0;

                //! Gets the icon used for the page in the configuration widget.
                virtual QIcon groupedConfigPageIcon(const QtilitiesCategory& category) const = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IGroupedConfigPageInfoProvider,"com.Qtilities.CoreGui.IGroupedConfigPageInfoProvider/1.0");

#endif // I_GROUPED_CONFIG_PAGE_INFO_PROVIDER_H
