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

#ifndef ICONFIGPAGE_H
#define ICONFIGPAGE_H

#include <IObjectBase>

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCategory.h"

#include <QObject>
#include <QIcon>

using namespace Qtilities::Core;
using namespace Qtilities::Core::Interfaces;

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {
            /*!
            \class IConfigPage
            \brief An interface through which widgets can be added to the ConfigurationWidget class.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IConfigPage : virtual public IObjectBase
            {

            public:
                IConfigPage() {}
                virtual ~IConfigPage() {}

                //! Gets the icon used for the page in the configuration widget.
                virtual QIcon configPageIcon() const = 0;
                //! Gets widget used in the configuration page area.
                virtual QWidget* configPageWidget() = 0;
                //! Gets the category of the config page.
                virtual QtilitiesCategory configPageCategory() const = 0;
                //! Gets the title of the config page.
                virtual QString configPageTitle() const = 0;
                //! Indicates that the current state of the config page must be saved (applied).
                virtual void configPageApply() = 0;
                //! Indicates if the page supports apply operations. When a page only displays information, ExtensionSystemConfig, we don't want the apply button to be active. This function thus controls the activity of the apply button.
                virtual bool supportsApply() const = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IConfigPage,"com.qtilities.CoreGui.IConfigPage/1.0");

#endif // ICONFIGPAGE_H
