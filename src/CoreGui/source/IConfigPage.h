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

            The IConfigPage interface allows objects to expose configuration (settings) pages to the ConfigurationWidget. For more information see the ConfigurationWidget documentation.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IConfigPage : virtual public IObjectBase
            {

            public:
                IConfigPage() {}
                virtual ~IConfigPage() {}

                //! Gets the icon used for the page in the configuration widget.
                /*!
                  \sa setConfigPageIcon()
                  */
                virtual QIcon configPageIcon() const = 0;
                //! Sets the icon used for the page in the configuration widget.
                /*!
                  \note The default implementation does nothing.

                  \sa configPageIcon()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual void setConfigPageIcon(const QIcon& icon) {
                    Q_UNUSED(icon)
                }
                //! Gets widget used in the configuration page area.
                virtual QWidget* configPageWidget() = 0;
                //! Gets the help ID of the config page.
                /*!
                  \note The default implementation returns an empty, invalid help ID.

                  \sa setConfigPageHelpID()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual QString configPageHelpID() const { return d_help_id; }
                //! Sets the help ID of the config page.
                /*!
                  This allows you to use config pages from other libraries, such as %Qtilities and specify
                  help IDs for them which matches your application's help structure.

                  \sa configPageHelpID()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual void setConfigPageHelpID(const QString& config_help_id) {
                    d_help_id = config_help_id;
                }
                //! Gets the category of the config page.
                /*!
                  If you want other parts of your code to be able to modify the category of your page, implement configPageCategory() like this:

\code
if (IConfigPage::configPageCategory().isEmpty())
    return QtilitiesCategory("Your Category");
else
    return IConfigPage::configPageCategory();
\endcode

                 If not, just return your caregory.

                  \sa setConfigPageCategory()
                  */
                virtual QtilitiesCategory configPageCategory() const {
                    return d_category;
                }
                //! Sets the category of the config page.
                /*!
                  This allows you to use config pages from other libraries, such as %Qtilities and group them under
                  your own custom categories.

                  \sa configPageCategory()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual void setConfigPageCategory(const QtilitiesCategory& category) {
                    d_category = category;
                }
                //! Gets the title of the config page.
                virtual QString configPageTitle() const = 0;
                //! Indicates that the current state of the config page must be saved (applied).
                virtual void configPageApply() = 0;
                //! Indicates if the page supports apply operations. When a page only displays information, ExtensionSystemConfig, we don't want the apply button to be active. This function thus controls the activity of the apply button.
                virtual bool supportsApply() const = 0;
                //! Indicates that the defaults of the page must be restored.
                /*!
                  \note The default implementation does nothing.

                  \sa supportsRestoreDefaults()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual void configPageRestoreDefaults() {}
                //! Indicates if the page supports restoration of default settings.
                /*!
                  \note The default implementation does nothing.

                  \sa configPageRestoreDefaults()

                  <i>This function was added in %Qtilities v1.1.</i>
                  */
                virtual bool supportsRestoreDefaults() const { return false; }
                //! Initialization function where you can do any initialization required by your page.
                /*!
                 * This function will be called when the configuration widget searches the object pool for config pages.
                 * The function will be called on all found pages before adding them to the configuration widget.
                 *
                 * In big applications where many configuration pages exists, it makes sense to make use of this function in order
                 * to speed up the launch time of your application. Without this function, you will typically initialize your config
                 * pages in their constructors. Thus, all the pages will be set up during application launch even if the user
                 * never goes to the configuration page. This function provides a solution to that problem where pages will only be
                 * initialized when the configuration widget is actually used.
                 *
                 *\note The default implementation does nothing.
                 *
                 *<i>This function was added in %Qtilities v1.2.</i>
                 */
                virtual void configPageInitialize() {}

            private:
                QString d_help_id;
                QtilitiesCategory d_category;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IConfigPage,"com.Qtilities.CoreGui.IConfigPage/1.0")

#endif // ICONFIGPAGE_H
