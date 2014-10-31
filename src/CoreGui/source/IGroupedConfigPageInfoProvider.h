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

                //! Gets the tab order of pages within this group.
                /*!
                 * \return List containing the configPageTitle()'s in the order which they should
                 * be displayed in this category.
                 */
                virtual QStringList pageOrderForCategory(const QtilitiesCategory& category) const {
                    Q_UNUSED(category)
                    return QStringList();
                }
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IGroupedConfigPageInfoProvider,"com.Qtilities.CoreGui.IGroupedConfigPageInfoProvider/1.0")

#endif // I_GROUPED_CONFIG_PAGE_INFO_PROVIDER_H
